import os, time, shutil, pickle
from datetime import datetime
import traceback, sys
import hashlib


class Document():

	def __init__(self, path, name, time_modified):
		self.path = path
		self.name = name
		self.modified = time_modified

	def __repr__(self):
		return "<Document: %s>" % os.path.join(self.path, self.name)

class Logger():

	def __init__(self, path):
		if not os.path.exists(path):
			os.makedirs(path)
		self.path = os.path.join(path, "backerupper.1.log")
		self.maxsize = 1000 * 1000 * 25 ### in bytes

		if not os.path.exists(self.path):
			try:
				fp = open(self.path, "wb")
				fp.write("~~~~~~~~~~~~ Backer Upper Log File ~~~~~~~~~~~\r\n")
				fp.close()
			except Exception, e:
				print "Error touching log file."
				print e

	def note(self, msg):
		if os.path.getsize(self.path) < self.maxsize:
			self.write(msg)
		else:
			self.incrementfile()
			self.write(msg)

	def incrementfile(self):
		i = 1
		stop = False
		while(not stop):
			parts = self.path.split(".")
			parts[-2] = str(i)
			newpath = ".".join(parts)
			
			if not os.path.exists(newpath):
				stop = True
				self.path = newpath

			i += 1 


	def write(self, msg):
		try:
			fp = open(self.path, "ab")
			prettytime = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
			fp.write("[+] " + str(prettytime) + "  " + str(msg) + "\r\n")
			fp.close()
		except Exception, e:
			print e
			print "Logging failed."


class BackerUpper():

	def __init__(self, target_dirs, backup_root):

		self.backup_root = backup_root
		self.targets = target_dirs
		self.tmppath = r"C:\backerupper"
		if not os.path.exists(self.tmppath):
			os.makedirs(self.tmppath)

		try:
			fp = open(os.path.join(self.tmppath, "documents.pickle"), "rb")
			self.documents = pickle.load( fp )
			fp.close()
		except Exception, e:
			print "Error opening documents.pickle" , e
			self.documents = {}

		self.ignorefiles = ["ntuser.pol"]



		self.logger = Logger(self.tmppath)

	def compare(self, target_dir):

		try:
			start_time = time.time()

			self.logger.note("Compare called on %s." % target_dir)

			for root, dirs, files in os.walk(target_dir):
				#print root, dirs, files
				#build backup path
				if root[:2] == r"\\":
					backup_path = os.path.join(self.backup_root, root[2:])
				else:
					backup_path = os.path.join(self.backup_root, root[3:])

				### mirror folder and sub folders
				for directory in dirs:
					if not os.path.exists(os.path.join(backup_path, directory)):
						self.logger.note( "Making: " + os.path.join(backup_path, directory) )
						try:
							os.makedirs(os.path.join(backup_path, directory))
						except OSError, e:
							self.logger.note( "Error making " + os.path.join(backup_path, directory) )
							self.logger.note( e )

	 			for documentname in files:
	 				### only proceed if not ignoring that file
	 				if documentname not in self.ignorefiles:
		 				full_name = os.path.join(root, documentname)
		 				### use hash of full_name as dictonary key to avoid characterspace/encoding issues
		 				sha = hashlib.sha256()
		 				sha.update(full_name)
		 				secret = sha.digest()
						### document does not exist in self.documents
						if secret not in self.documents:
							### add to self.documents
							try:
								self.documents[secret] = Document(root, documentname, os.path.getmtime(full_name))
							except WindowsError, e:
								self.logger.note( "Error adding document to queue: ")
								self.logger.note(e)
								self.logger.note("root = " + root)
								self.logger.note("documentname = " + documentname)
							### copy document to backup location
							try:
								self.logger.note("Copying ... " + full_name + " to " + os.path.join(backup_path, documentname))
								shutil.copy2(src=full_name, dst=os.path.join(backup_path, documentname))
							except IOError, e:
								self.logger.note("Error copying document:")
								self.logger.note(e)
								self.logger.note("root = " + root)
								self.logger.note("documentname = " + documentname)


						### document exists and is unmodified
						elif os.path.getmtime(full_name) == self.documents[secret].modified:
							pass

						### document exists and is modified 
						else:
							### copy backup file to alternate name
							try:
								self.logger.note("Versioning document ... " + os.path.join(backup_path, documentname))						
								shutil.copy2(src=full_name, dst=os.path.join(backup_path, documentname + ".old"))
							except IOError, e:
								self.logger.note("Error versioning document: ")
								self.logger.note(e)

							### copy file
							try:
								self.logger.note( "Updating ... " + os.path.join(backup_path, documentname))
								shutil.copy2(src=full_name, dst=os.path.join(backup_path, documentname))
								### udpate modified time
								self.documents[secret].modified = os.path.getmtime(full_name)
							except IOError, e:
								self.logger.note( "Error updating document:")
								self.logger.note(e)				

			try:
				fp = open(os.path.join(self.tmppath, "documents.pickle"), "wb")
				pickle.dump( self.documents, fp )
			except Exception, e:
				self.logger.note("Error pickle dumping: ")
				self.logger.note(e)
			finally:
				fp.close()

			end_time = time.time()

			runtime = end_time - start_time
			human_runtime = "Compare took %s seconds to run on %s." % (str(int(runtime)), target_dir)
			self.logger.note(human_runtime) 

		except Exception, e:
			trace = traceback.format_exc()
			self.logger.note("BIG ERROR")
			self.logger.note(e)
			self.logger.note(trace)

	def run(self):
		for target in self.targets:
			self.compare(target)


if __name__ == "__main__":

	bu = BackerUpper([r'\\scsfilesrv\Personal\csteinke', r'C:\Users'], r"C:\backup")
	bu.run()