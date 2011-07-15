#!/usr/bin/python

from hashlib import *
import sys, struct

def sha512(psid):
	if len(psid) != 16:
		return "".encode()

	for i in range(512):
		psid = sha1(psid).digest()

	return psid

def get_psid(str):
	if len(str) != 32:
		return "".encode()

	b = "".encode()
	for i in range(0, len(str), 2):
		b += struct.pack('B', int(str[i] + str[i+1], 16))

	return b

def main():
	if len(sys.argv) < 2:
		print ("Usage: sha512.py psid")
		exit(0)
	
	psid = get_psid(sys.argv[1])
	xhash = sha512(psid)

	if len(xhash) == 0:
		print ("wrong PSID")
		exit(0)

	print ("{\n\t"),

	for i in range(len(xhash)):
		if i != 0 and i % 8 == 0:
			print ("\n\t"),
		print ("0x%02X, "%(struct.unpack('B', xhash[i])[0])),
	print ("\n},")

if __name__ == "__main__":
	main()
