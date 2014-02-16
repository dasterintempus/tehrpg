#!/usr/bin/env python

import socket
import readline
import sys
import hashlib

passwd_sha = hashlib.sha512()

passwd_sha.update(sys.argv[1])

hashedpasswd = passwd_sha.hexdigest()

print hashedpasswd