#!/usr/bin/python

import sys, os, gzip, StringIO

def dump_binary(fn, data):
	f = open(fn, "wb")
	f.write(data)
	f.close()

def dec_prx(fn):
	f = open(fn, "rb")
	f.seek(0x150)
	dat = f.read()
	f.close()

	temp=StringIO.StringIO(dat)
	f=gzip.GzipFile(fileobj=temp, mode='rb')
	dec = f.read(f)
	f.close()

	fn = "%s.dec.prx" % os.path.splitext(fn)[0]
	print ("Decompressed to %s" %(fn))
	dump_binary(fn, dec)

def main():
	if len(sys.argv) < 2:
		print ("Usage: %s <file>" % (sys.argv[0]))
		sys.exit(-1)

	dec_prx(sys.argv[1])

if __name__ == "__main__":
	main()
