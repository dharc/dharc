#!/usr/bin/env python

import subprocess
import os
import fnmatch
import sys

print "==== DHarc Unit Tests =================================================="

code = 0

print "==== Fabric ============================================================"

for file in os.listdir('./fabric/tests'):
	if fnmatch.fnmatch(file, '*-unit'):
		print "Running %s" % file
		if subprocess.call('./fabric/tests/'+file, shell=False) != 0:
			code = 1

print "==== Common ============================================================"

for file in os.listdir('./common/tests'):
	if fnmatch.fnmatch(file, '*-unit'):
		print "Running %s" % file
		if subprocess.call('./common/tests/'+file, shell=False) != 0:
			code = 1

print "==== Monitors =========================================================="

print "==== Senses ============================================================"

print "==== Actors ============================================================"

print "==== Architect ========================================================="

sys.exit(code)

