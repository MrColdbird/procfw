#!/usr/bin/python
import sys, os, struct, gzip, hashlib

def binary_replace(data, newdata, offset):
	return data[0:offset] + newdata + data[offset+len(newdata):]

def prx_compress(output, hdr, input):
	a=open(hdr, "rb")
	fileheader = a.read();
	a.close()

	a=open(input, "rb")
	elf = a.read(4);
	a.close()

	if (elf != b'\x7fELF'):
		print ("not a ELF/PRX file!")
		return

	uncompsize = os.stat(input).st_size
	f_in=open(input, 'rb')

	try:
		os.remove("tmp.gz")
	except:
		pass

	f=gzip.GzipFile("tmp.gz", 'wb')
	f.writelines(f_in)
	f.close()
	f_in.close()

	a=open('tmp.gz', 'rb')
	prx=a.read()
	a.close()

	digest=hashlib.md5(prx).digest()
	filesize = len(fileheader) + len(prx)

	fileheader = binary_replace(fileheader, struct.pack('L', uncompsize), 0x28)
	fileheader = binary_replace(fileheader, struct.pack('L', filesize), 0x2c)
	fileheader = binary_replace(fileheader, struct.pack('L', len(prx)), 0xb0)
	fileheader = binary_replace(fileheader, digest, 0x140)

	a=open(output, "wb")
	assert(len(fileheader) == 0x150)
	a.write(fileheader)
	a.write(prx)
	a.close()

	try:
		os.remove("tmp.gz")
	except:
		pass

	return 0

def main():
	if len(sys.argv) >= 4:
		prx_compress(sys.argv[1], sys.argv[2], sys.argv[3])

if __name__ == "__main__":
	main()
