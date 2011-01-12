#!/usr/bin/python

import sys, hashlib

def toNID(name):
	hashstr = hashlib.sha1(name.encode()).hexdigest().upper()
	return "0x" + hashstr[6:8] + hashstr[4:6] + hashstr[2:4] + hashstr[0:2]

if __name__ == "__main__":
	assert(toNID("sceKernelCpuSuspendIntr") == "0x092968F4")

	for name in sys.argv[1:]:
		print ("%s: %s"%(name, toNID(name)))
