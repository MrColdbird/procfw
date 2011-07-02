#!/usr/bin/python

import  sys, hashlib

def write_file(fn, buf):
	fp = open(fn, "wb")
	fp.write(buf)
	fp.close()

def usage():
	print ("Usage: " + sys.argv[0] + " infile outfile label")


def main():

	if( len(sys.argv) != 4):
		usage()
		sys.exit()

	fp = open( sys.argv[1] , "rb")
	hash_str =  hashlib.md5( fp.read() ).hexdigest()
	fp.close()

	out_str = "u8 " + sys.argv[3] + "[16] = { "
	i=0

	for str in hash_str:
		if i % 2 == 0:
			out_str += "0x" + str
		else:
			out_str += str
			if i < 31:
				out_str += ", "

		i += 1

	out_str += " };"
	print out_str
	write_file( sys.argv[2] , out_str )

if __name__ == "__main__":
	main()
