#!/usr/bin/env python

import socket
import sys
import time
import hashlib
import getpass

def answer_challenge(challenge, passwd):
	passwd_sha = hashlib.sha512()
	challenge_sha = hashlib.sha512()

	passwd_sha.update(passwd)

	hashedpasswd = passwd_sha.hexdigest()

	challenge_sha.update(challenge + hashedpasswd)

	return challenge_sha.hexdigest()


if __name__ == "__main__":
	s = socket.create_connection((sys.argv[1], 3137))
	data = s.recv(4096)
	for line in data.split("\n"):
		print line
	s.sendall("/login " + sys.argv[2] + "\r\n")
	time.sleep(1)
	data = s.recv(4096)
	challengenext = False
	for line in data.split("\n"):
		print line
		if challengenext:
			passwd = getpass.getpass()
			s.sendall("/passwd " + answer_challenge(line, passwd) + "\r\n")
			break
		if "challenge" in line.lower():
			challengenext = True
	
	data = s.recv(4096)
	for line in data.split("\n"):
		print line