#!/usr/bin/python

"""
pspbtcnf_editor: An script that add modules from pspbtcnf
"""

import sys, os, re
from getopt import *
from struct import *

BTCNF_MAGIC=0x0F803001

verbose = False

def print_usage():
	print ("%s: pspbtcnf.bin [-o output.bin] [-a add_module_name:before_module_name:flag]" %(os.path.split(sys.argv[0]))[-1])

def replace_binary(data, offset, newdata):
	newdata = data[0:offset] + newdata + data[offset+len(newdata):]
	assert(len(data) == len(newdata))
	return newdata

def dump_binary(data, offset, size):
	newdata = data[offset:offset+size]
	assert(len(newdata) == size)

	return newdata

def dump_binary_str(data, offset):
	ch = data[offset]
	tmp = b''

	while ch != 0:
		tmp += pack('b', ch)
		offset += 1
		ch = data[offset]
	
	return tmp.decode()

def add_prx_to_bootconf(srcfn, before_modname, modname, modflag):
	"Return new bootconf data"

	fn=open(srcfn, "rb")
	bootconf = fn.read()
	fn.close()

	if len(bootconf) < 64:
		raise Exception("Bad bootconf") 

	signature, devkit, modestart, nmodes, modulestart, nmodules, modnamestart, modnameend = unpack('LL8xLL8xLL8xLL8x', bootconf[:64])

	if verbose:
		print ("Devkit: 0x%08X"%(devkit))
		print ("modestart: 0x%08X"%(modestart))
		print ("nmodes: %d"%(nmodes))
		print ("modulestart: 0x%08X"%(modulestart))
		print ("nmodules: 0x%08X"%(nmodules))
		print ("modnamestart: 0x%08X"%(modnamestart))
		print ("modnameend: 0x%08X"%(modnameend))

	if signature != BTCNF_MAGIC or nmodules <= 0 or nmodes <= 0:
		raise Exception("Bad bootconf") 

	bootconf = bootconf + modname.encode() + b'\0'
	modnameend += len(modname) + 1

	i=0

	while i < nmodules:
		module_path, module_flags = unpack('L4xL4x16x', bootconf[modulestart+i*32:modulestart+(i+1)*32])
		module_name = dump_binary_str(bootconf, modnamestart+module_path)

		if verbose:
			print ("[%02d]: Module path: %s flag: 0x%08X"%(i, module_name, module_flags))

		if before_modname == module_name:
			break

		i+=1
	
	if i >= nmodules:
		raise Exception("module %s not found"%(before_modname))

	module_path = modnameend - len(modname) - 1 - modnamestart
	module_flag = 0x80010000 | (modflag & 0xFFFF) 
	newmod = dump_binary(bootconf, modulestart+i*32, 32)
	newmod = replace_binary(newmod, 0, pack('L', module_path))
	newmod = replace_binary(newmod, 8, pack('L', module_flag))

	bootconf = bootconf[0:modulestart+i*32] + newmod + bootconf[modulestart+i*32:]

	nmodules+=1
	bootconf = replace_binary(bootconf, 0x24, pack('L', nmodules))
	modnamestart += 32
	bootconf = replace_binary(bootconf, 0x30, pack('L', modnamestart))
	modnameend += 32
	bootconf = replace_binary(bootconf, 0x34, pack('L', modnameend))

	i = 0

	while i < nmodes:
		num = unpack('H', bootconf[modestart+i*32:modestart+i*32+2])[0]
		num += 1
		bootconf = replace_binary(bootconf, modestart + i * 32, pack('H', num))
		i += 1

	return bootconf 

def write_file(output_fn, data):
	fn = open(output_fn, "wb")
	fn.write(data)
	fn.close()

def main():
	global verbose

	try:
		optlist, args = gnu_getopt(sys.argv, "a:o:vh")
	except GetoptError as err:
		print(err)
		print_usage()
		sys.exit(1)

	# default configure
	verbose = False
	dst_filename = "-"
	add_module = ""

	for o, a in optlist:
		if o == "-v":
			verbose = True
		elif o == "-h":
			print_usage()
			sys.exit()
		elif o == "-o":
			dst_filename = a
		elif o == "-a":
			add_module = a
		else:
			assert False, "unhandled option"
	
	if verbose:
		print (optlist, args)

	if len(args) < 2:
		print ("Missing input pspbtcnf.bin")
		sys.exit(1)

	src_filename = args[1]

	if verbose:
		print ("src_filename: " + src_filename)
		print ("dst_filename: " + dst_filename)
	
	# check add_module

	if add_module != "":
		t = (re.split(":", add_module, re.I))

		if len(t) != 3:
			print ("Bad add_module input")
			sys.exit(1)

		add_module, before_module, add_module_flag = (re.split(":", add_module, re.I))

		if verbose:
			print ("add_module: " + add_module)
			print ("before_module: " + before_module)
			print ("add_module_flag: " + add_module_flag)

	if add_module != "":
		result = add_prx_to_bootconf(src_filename, before_module, add_module, int(add_module_flag, 16))

	if dst_filename == "-":
#		print("Bootconf result:")
#		print(result)
		pass
	else:
		write_file(dst_filename, result)

if __name__ == "__main__":
	main()
