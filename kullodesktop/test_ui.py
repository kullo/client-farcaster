#!/usr/bin/env python3
#
# This script is used on Linux, OS X and Windows.
# Python 3 required.
# Returns 0 on success and >= 1 else.

import argparse
import os
import subprocess
import sys


parser = argparse.ArgumentParser(description='Test UI.')
parser.add_argument('--debug', action='store_true', help='In debug mode (default: release mode)')
args = parser.parse_args()

scriptdir = os.path.dirname(os.path.realpath(__file__))
env = os.environ.copy()

if sys.platform == "win32":
    if args.debug:
        kullo_bin = scriptdir + "/debug/kullo.exe"
    else:
        kullo_bin = scriptdir + "/release/kullo.exe"
elif sys.platform == "darwin":
    kullo_bin = scriptdir + "/Kullo.app/Contents/MacOS/Kullo"
else:
    env["LD_LIBRARY_PATH"] = scriptdir
    if args.debug:
        kullo_bin = scriptdir + "/kullo-debug"
    else:
        kullo_bin = scriptdir + "/kullo"

print("Kullo binary to run:", kullo_bin)

if sys.platform == "win32":
    cmd = [kullo_bin, '--test']
else:
    cmd = ['make_exec_and_run', kullo_bin, '--test']

# 10s testrun + 30s load time (could machines are slow)
subprocess.check_call(cmd, timeout=40, env=env);
