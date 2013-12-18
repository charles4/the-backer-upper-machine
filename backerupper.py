import os, time, shutil, pickle
from datetime import datetime


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
		self.path = os.path.join(path, "backerupper.log")

	def note(self, msg):
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

		start_time = time.time()

		self.logger.note("Compare called.")

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
					### document does not exist in self.documents
					if full_name not in self.documents:
						### add to self.documents
						try:
							self.documents[full_name] = Document(root, documentname, os.path.getmtime(full_name))
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
					elif os.path.getmtime(full_name) == self.documents[full_name].modified:
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
							shutil.copy(src=full_name, dst=os.path.join(backup_path, documentname))
							### udpate modified time
							self.documents[full_name].modified = os.path.getmtime(full_name)
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

	def run(self):
		for target in self.targets:
			self.compare(target)


if __name__ == "__main__":

	bu = BackerUpper([r'\\scsfilesrv\Personal\csteinke', r'C:\Users'], r"C:\backup")
	bu.run()