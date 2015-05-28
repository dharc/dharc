#!/usr/bin/env python

import subprocess
import sys

code = 0
server = subprocess.Popen("./dharcd");

def expect(cmd, result):
	if result != subprocess.check_output('./dharc-arch --cmd="'+cmd+'"',shell=True):
		code = 1
		print "failed: " + cmd
		return False
	else:
		return True

def test_constant_define():
	expect("100 200 = 300; 100 200;","300\n")
	expect("100 400 = {100 200}; 100 400;","300\n")

test_constant_define()

server.terminate()

sys.exit(code)

