#!/usr/bin/env python

import subprocess
import sys

code = 0

if subprocess.call('../tools/unit-tests.py', shell=False) != 0:
	code = 1

if subprocess.call('../tools/system-tests.py', shell=False) != 0:
	code = 1

if subprocess.call('cppcheck -I../fabric/includes --error-exitcode=2 ../fabric/src ../common/src ../monitor/common/src', shell=True) != 0:
	code = 1

if subprocess.call('../tools/cpplint.py --filter=-runtime/string,-whitespace/braces,-whitespace/tab,-build/c++11,-runtime/int,-runtime/references --extensions=hpp,cpp ../fabric/src/* ../fabric/includes/dharc/* ../common/src/* ../common/includes/dharc/* ../monitor/command-line/* ../monitor/common/src/* ../monitor/common/includes/dharc/*', shell=True) != 0:
	code = 1

sys.exit(code);

