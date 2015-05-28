#!/usr/bin/env python

import subprocess
import sys

code = 0

if subprocess.call('../tools/unit-tests.py', shell=False) != 0:
	code = 1

if subprocess.call('../tools/system-tests.py', shell=False) != 0:
	code = 1

if subprocess.call('cppcheck -I../daemon/includes --error-exitcode=2 ../daemon/src ../common/src ../arch/lib/src', shell=True) != 0:
	code = 1

if subprocess.call('../tools/cpplint.py --filter=-whitespace/braces,-whitespace/tab,-build/c++11,-runtime/int,-runtime/references --extensions=hpp,cpp ../daemon/src/* ../daemon/includes/dharc/* ../common/src/* ../common/includes/dharc/* ../arch/cli/* ../arch/lib/src/* ../arch/lib/includes/dharc/*', shell=True) != 0:
	code = 1

sys.exit(code);

