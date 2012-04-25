#!/usr/bin/python

import os, sys, getopt

def usage():
	print ("Usage: %s [-l size ] basefile input output" % (sys.argv[0]))

def write_file(fn, buf):
	fp = open(fn, "wb")
	fp.write(buf)
	fp.close()

def main():
	inputsize = 0

	try:
		optlist, args = getopt.getopt(sys.argv[1:], 'l:h')
	except getopt.GetoptError:
		usage()
		sys.exit(2)

	for o, a in optlist:
		if o == "-h":
			usage()
			sys.exit()
		if o == "-l":
			inputsize = int(a, 16)
	
	inputsize = max(inputsize, 0x4000);

	if len(args) < 3:
		usage()
		sys.exit(2)
	
	basefile = args[0]
	inputfile = args[1]
	outputfile = args[2]

	fp = open(basefile, "rb")
	buf = fp.read(0x1000)
	fp.close()

	if len(buf) < inputsize:
		buf += '\0' * (inputsize - len(buf))

	assert(len(buf) == inputsize)

	fp = open(inputfile, "rb")
	ins = fp.read(0x3000)
	fp.close()
	buf = buf[0:0x1000] + ins + buf[0x1000+len(ins):]
	assert(len(buf) == inputsize)
	write_file(outputfile, buf)

if __name__ == "__main__":
	main()
