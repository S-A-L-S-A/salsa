#!/usr/bin/env python3
# -*- coding: utf-8 -*-

""" Executes a test on a plugin

This needs python3.
"""

import argparse
import sys
import shutil
import os
import fnmatch
import subprocess
import io
import filecmp


class TestError(Exception):
	""" The exception raised if an error occurs

	This class is used throughout the code to signal test errors. If this
	exception is raised the test has failed
	"""

	def __init__(self, description):
		""" Constructor

		description is the description of the error
		"""
		self.description = description

	def __str__(self):
		""" A string representation of the error
		"""

		return repr(self.description)


def getFilesMatchingPatterns(directory, patterns):
	""" Returns the list of files inside directory matching one of the
	    patterns

	patterns must be a list of lists of patterns (wildcard patterns). This
	function returns a list of lists of files matching the patterns. For
	example if directory contains the following files:
		a.txt b.py c.jpg d.ogg e.ogg
	and pattern is like this:
		[["*.txt", "b.*"] ["*.py", "*.ogg"]]
	the function output is the following:
		[["a.txt", "b.py"], ["d.ogg", "e.ogg"]]
	A file can only be in one list (the patterns coming first in the list
	take precedence). Note that only files are taken into account, not
	directories
	"""

	# Getting the list of files in the directory
	filesInDir = os.listdir(directory)

	# Adding files to lists
	files = []
	for pGroup in patterns:
		filesForCurPatterns = []
		for f in filesInDir:
			if os.path.isfile(os.path.join(directory, f)):
				for p in pGroup:
					if fnmatch.fnmatch(f, p):
						filesForCurPatterns.append(f)
						break

		files.append(filesForCurPatterns)

	return files


def prepareTestDirectory(pluginTestDir, testDir, excludedFiles):
	""" Creates/Cleares the destination directory and copies files

	pluginTestDir is the directory with files needed for the test
	testDir is the directory where the test is executed (files in
	        pluginTestDir are copied here)
	excludedFiles is a list of files not to copy

	This function returns the name of all
	"""

	# Removing the test directory if it exists
	shutil.rmtree(testDir, ignore_errors = True)

	# Creating it again
	os.mkdir(testDir)

	# Getting the list of files from the plugin test directory
	filesInPluginDir = os.listdir(pluginTestDir)

	# Removing from the list of files to copy directories and files matching patterns in excludedFiles
	filesToCopy = []
	for f in filesInPluginDir:
		filename = os.path.join(pluginTestDir, f)
		# This is to be sure no directory is taken
		if os.path.isfile(filename):
			# Checking that the file doesn't match any in excludedFiles
			excludeFile = False

			for e in excludedFiles:
				if e == f:
					excludeFile = True
					break

			if not excludeFile:
				filesToCopy.append(filename)

	# Copying files to the destination directory
	for f in filesToCopy:
		shutil.copy(f, testDir)


def runTest(total99Exe, testDir, pluginBuildDir, configurationFile, action):
	""" Actually runs the test

	total99Exe is the path of the total99 executable
	testDir is the directory where the test is executed
	pluginBuildDir is the directory where the plugin has been built
	configurationFile is the name of the configuration file to use
	action is the action to execute
	"""

	configurationFileFullPath = os.path.join(testDir, configurationFile)
	output = ""
	error = ""
	with open(os.devnull, 'r') as devnull:
		process = subprocess.Popen([total99Exe, "--batch", "--file=" + configurationFileFullPath, "--action=" + action, "-PTOTAL99/pluginPath=" + pluginBuildDir], stdin = devnull, stdout = subprocess.PIPE, stderr = subprocess.PIPE, universal_newlines = True, cwd = testDir)
		# Waiting for the process to end
		output, error = process.communicate()

	if process.returncode != 0:
		raise TestError("Total99 execution ended with code " + str(process.returncode) + "\n-= Output stream =-\n" + output + "\n-= Error stream =-\n" + error)


def checkResults(pluginTestDir, testDir, textFilesToMatch, binaryFilesToMatch):
	""" Checks if files resulting from the test match the ones provided with the test

	pluginTestDir is the directory with files needed for the test (where the
	              original files can be found)
	testDir it the directory with the results of the test
	textFilesToMatch is the list of text files to check
	binaryFilesToMatch is the list of binary files to check
	"""

	# First of all checking text files
	for f in textFilesToMatch:
		origFile = os.path.join(pluginTestDir, f)
		newFile = os.path.join(testDir, f)

		# Checking we actually ahve the files
		if not os.path.isfile(origFile):
			raise TestError("The original file (" + origFile + ") is not accessible")
		if not os.path.isfile(newFile):
			raise TestError("The generated file (" + newFile + ") is not accessible")

		# Comparing files line by lines
		equal = True
		with open(origFile, "rt") as origF, open(newFile, "rt") as newF:
			while True:
				origLine = origF.readline()
				newLine = newF.readline()
				if origLine == "" and newLine == "":
					break
				elif origLine != newLine:
					equal = False
					break

		if not equal:
			raise TestError("The generated " + newFile + " is different from the original one")

	# Now checking binary files
	for f in binaryFilesToMatch:
		origFile = os.path.join(pluginTestDir, f)
		newFile = os.path.join(testDir, f)

		# Checking we actually ahve the files
		if not os.path.isfile(origFile):
			raise TestError("The original file (" + origFile + ") is not accessible")
		if not os.path.isfile(newFile):
			raise TestError("The generated file (" + newFile + ") is not accessible")

		# Comparing files
		if not filecmp.cmp(origFile, newFile, shallow = False):
			raise TestError("The generated " + newFile + " is different from the original one")


def createCommandLineParser():
	""" Creates and initialize the command line parser

	This function creates and initializes all options for the command line
	parser. Returns the newly created parser
	"""

	# Creating parser
	description = """Performs a test on a given plugin and optionally checks generated files. This script
	                 copies all files from the directory with files needed to execute the test (with some
	                 exceptions, see below) inside another directory and starts total99 in batch to
	                 execute the given action (the configuration file used when running total99 is
	                 configuration.ini inside the directory with files needed for the test; the name of
	                 the file can be changed with the -c command line option). The basic behaviour is to
	                 return success if the execution of total99 ends without any error. If the -t or -b
	                 command line options are specified, the files matching the given patterns are not
	                 copied and instead are checked against the same files which should be produced by
	                 the execution of the test. An error is returned if the test doesn't produce the files
	                 as well as if the files are not exactly the same. The difference between the -t and
	                 -b options is that the first compares text files (i.e. endline encoding differences
	                 are ignored), while the second as binary files (the must be exactly the same). If a
	                 file matches both a pattern in -t and -b, the text takes precedence. NOTE:
	                 subdirectories of the directory with files needed to execute the test are not copied
	                 nor checked"""
	parser = argparse.ArgumentParser(description = description)

	# Adding version information
	parser.add_argument("--version", action="version", version='%(prog)s 1.0')

	# Adding positional arguments
	parser.add_argument("total99Exe", action="store", type=str, help="The full path to the total99 executable")
	parser.add_argument("pluginTestDir", action="store", type=str, help="The directory with files needed to execute the test")
	parser.add_argument("pluginBuildDir", action="store", type=str, help="The directory where the plugins to load have been build")
	parser.add_argument("testDir", action="store", type=str, help="The directory in which the test is executed. The directory is created is it doesn't exists and all files and directories are removed from inside before copying the test files")
	parser.add_argument("action", action="store", type=str, help="The action to execute (value of the --action option of total99)")

	# Adding options
	parser.add_argument("-t", "--matchTextFiles", action="append", default=[], type=str, help="The pattern for files that should be checked as text files at the end of the test. The accepts simple wildcard expressions", metavar="PATTERN", dest="matchTextFiles")
	parser.add_argument("-b", "--matchBinaryFiles", action="append", default=[], type=str, help="The pattern for files that should be checked as text files at the end of the test. The accepts simple wildcard expressions", metavar="PATTERN", dest="matchBinaryFiles")
	parser.add_argument("-c", "--configurationFile", action="store", default="configuration.ini", type=str, help="The name of the configuration file to use when running total99. The default is %(default)s", metavar="PATTERN", dest="configurationFile")
	parser.add_argument("-w", "--convertToWindowsPath", action="store_true", help="Converts all paths to the Windows format (i.e. converts / to \\)", dest="convertToWindowsPath")

	return parser


def main():
	""" The main function of the program
	"""

	# Parsing commandline
	parser = createCommandLineParser()
	args = parser.parse_args()

	# Checking if we have to convert path to windows format
	if args.convertToWindowsPath:
		args.total99Exe = args.total99Exe.replace("/", "\\")
		args.pluginTestDir = args.pluginTestDir.replace("/", "\\")
		args.pluginBuildDir = args.pluginBuildDir.replace("/", "\\")
		args.testDir = args.testDir.replace("/", "\\")
		args.matchTextFiles = [s.replace("/", "\\") for s in args.matchTextFiles]
		args.matchBinaryFiles = [s.replace("/", "\\") for s in args.matchBinaryFiles]
		args.configurationFile = args.configurationFile.replace("/", "\\")

	# Generating the list of files that will be checked at the end of the test
	filesToMatch = getFilesMatchingPatterns(args.pluginTestDir, [args.matchTextFiles, args.matchBinaryFiles])

	# First of all creating/clearing the destination directory and copying files
	prepareTestDirectory(args.pluginTestDir, args.testDir, filesToMatch[0] + filesToMatch[1])

	# Executing the test
	runTest(args.total99Exe, args.testDir, args.pluginBuildDir, args.configurationFile, args.action)

	# Checking resulting files
	checkResults(args.pluginTestDir, args.testDir, filesToMatch[0], filesToMatch[1])


# Checking if this is run as a script
if __name__ == "__main__":
	try:
		main()
	except TestError as err:
		print("Execution error, reason:", err.description)
		sys.exit(1)
	except OSError as err:
		print("Operating system error, reason:", str(err))
		sys.exit(1)
	except SystemExit:
		raise
	except:
		print("Execution error:", sys.exc_info()[0])
		sys.exit(1)

	# Exiting with success
	sys.exit(0)
