import os, time, shutil, re
from datetime import datetime

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

	def error(self, msg):
		try:
			fp = open(self.path, "ab")
			prettytime = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
			fp.write("[-] " + str(prettytime) + "  " + str(msg) + "\r\n")
			fp.close()
		except Exception, e:
			print e
			print "Logging failed."	



class BackerUpper():

	def __init__(self, target_root, backup_root):

		self.target_root = target_root
		self.backup_root = backup_root

		if not os.path.exists(self.backup_root):
			os.makedirs(self.backup_root)

		self.logger = Logger(r"C:\logs")

	def build_backup_path(self, path):

		if path[:2] == r"\\":
			path = os.path.join(self.backup_root, path[2:])
		else:
			path = os.path.join(self.backup_root, path[3:])
		return path

	def compare_directory(self, dpath):
		if not os.path.exists(dpath):
			self.logger.error(dpath+" Does not exist.")
			return

		bpath = self.build_backup_path(dpath)

		if not os.path.exists(bpath):
			os.makedirs(bpath)

		### get folders and files in target directory
		items = os.listdir(dpath)

		for item in items:
			if os.path.isfile(os.path.join(dpath, item)):
				### if item doesn't exist at backup path create it
				backup_item = os.path.join(bpath, item)
				original_item = os.path.join(dpath, item)
				#print original_item
				if not os.path.exists( backup_item ):
					self.copy_file(original_item, backup_item)
				### if item does exist at backup, compare mod dates
				elif os.path.getmtime(original_item) > os.path.getmtime(backup_item):
					self.copy_file(original_item, backup_item)
			else:
				self.compare_directory(os.path.join(dpath, item))


		return


	def copy_file(self, original_item, backup_item):
		try:
			shutil.copy2(src=original_item, dst=backup_item)
		except IOError, e:
			self.logger.error("Error copying " + original_item)
			self.logger.error(e)

	def run(self):
		try:
			starttime = time.time()
			self.compare_directory(self.target_root)
			endtime = time.time()
			runtime = endtime - starttime
			human_runtime = "Run() took %s seconds to run on %s." % (str(int(runtime)), self.target_root)
			self.logger.note(human_runtime) 
		except Exception, e:
			self.logger.error("Major error. Recursion finished or broke.")
			self.logger.error(e)

if __name__ == "__main__":

	bk = BackerUpper(target_root=r'\\scsfilesrv\Personal\csteinke', backup_root=r'C:\testbackup')
	bk.run()


