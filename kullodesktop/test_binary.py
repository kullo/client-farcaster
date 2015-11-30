#!/usr/bin/env python3
#
# This script is used on Linux, OS X and Windows.
# Python 3 required.
# Returns 0 on success and >= 1 else.

import subprocess
import sys
import unittest

from os import path

scriptdir = path.dirname(path.realpath(__file__))

if sys.platform == "win32":
    if path.isfile(scriptdir + "/debug/kullo.exe"):
        kullo_bin = scriptdir + "/debug/kullo.exe"
    else:
        kullo_bin = scriptdir + "/release/kullo.exe"
elif sys.platform == "darwin":
    kullo_bin = scriptdir + "/Kullo.app/Contents/MacOS/Kullo"
else:
    if path.isfile(scriptdir + "/kullo-debug"):
        kullo_bin = scriptdir + "/kullo-debug"
    else:
        kullo_bin = scriptdir + "/kullo"

print("Kullo binary to test:", kullo_bin)

#class KulloBinaryTests(unittest.TestCase):
#    def test_openssl_version(self):
#        # OpenSSL must be 1.0.1x
#        output = subprocess.check_output([kullo_bin, '--openssl-version'], timeout=30);
#        self.assertRegex(output.decode('utf8'), r"^1\.0\.1(\D.*)?$")

if __name__ == '__main__':
    unittest.main()

