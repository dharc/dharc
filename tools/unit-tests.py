#!/usr/bin/env python

import subprocess
import os
import fnmatch
import sys

print "==== F-DSB Unit Tests =================="

code = 0

for file in os.listdir('./fabric/tests'):
	if fnmatch.fnmatch(file, '*-unit'):
		print "Running %s" % file
		if subprocess.call('./fabric/tests/'+file, shell=False) != 0:
			code = 1

for file in os.listdir('./common/tests'):
	if fnmatch.fnmatch(file, '*-unit'):
		print "Running %s" % file
		if subprocess.call('./common/tests/'+file, shell=False) != 0:
			code = 1

sys.exit(code)

