#!/usr/bin/python

import  sys, re, os

def write_file(fn, buf):
	fp = open(fn, "wb")
	fp.write(buf)
	fp.close()

def usage():
	print ("Usage: " + os.path.split(__file__)[1] + " version outfile")

def crete_version_str(ver):
	base = list("0.00")
	version = str(ver)
	ret = []
	base[0] = version[0]
	base[2] = version[1]
	base[3] = version[2]
	ret.append("".join(base))

	version = str(int(ver) + 1)
	base[0] = version[0]
	base[2] = version[1]
	base[3] = version[2]
	ret.append("".join(base))

	return ret

def main():

	if( len(sys.argv) != 3):
		usage()
		sys.exit()

	if len( sys.argv[1]) != 3:
		print sys.argv[1] + ":Version error."
		sys.exit()

	fp = open( os.path.dirname(os.path.abspath(__file__)) + "/update_base.sfo" , "rb")
	sfo_buf = fp.read()
	fp.close()

	after_str =  crete_version_str(sys.argv[1])
	print "Target version:" + after_str[0]

	sfo_buf = re.sub("X.YZ", after_str[0], sfo_buf )
	sfo_buf = re.sub("Z.YX", after_str[1], sfo_buf )
	write_file( sys.argv[2] , sfo_buf )

if __name__ == "__main__":
	main()
