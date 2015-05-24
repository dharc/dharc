#!/usr/bin/env python

import subprocess
import os
import fnmatch
import sys

print "==== F-DSB Unit Tests =================="

code = 0

for file in os.listdir('.'):
	if fnmatch.fnmatch(file, '*-unit'):
		print "Running %s" % file
		code = subprocess.call('./'+file, shell=False)

sys.exit(code)

