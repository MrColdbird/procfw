#!/usr/bin/python

import os

def main():
	os.system("bin2c ../../Installer/installer.prx installer.h installer")
	os.system("bin2c ../../Rebootex/Rebootex.prx Rebootex_prx.h Rebootex_prx")

if __name__ == "__main__":
	main()
