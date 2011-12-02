#!/usr/bin/python

""" PRO build script"""

import os, shutil, sys

NIGHTLY=0
VERSION="B10"

PRO_BUILD = [
			{ "fn": "620PRO-%s.rar", "config": "CONFIG_620=1" },
			{ "fn": "635PRO-%s.rar", "config": "CONFIG_635=1" },
			{ "fn": "639PRO-%s.rar", "config": "CONFIG_639=1" },
			{ "fn": "660PRO-%s.rar", "config": "CONFIG_660=1" },

#			{ "fn": "620PRO-%s.zip", "config": "CONFIG_620=1" },
#			{ "fn": "635PRO-%s.zip", "config": "CONFIG_635=1" },
#			{ "fn": "639PRO-%s.zip", "config": "CONFIG_639=1" },
#			{ "fn": "660PRO-%s.zip", "config": "CONFIG_660=1" },

#			{ "fn": "620PRO-%s.tar.gz", "config": "CONFIG_620=1" },
#			{ "fn": "635PRO-%s.tar.gz", "config": "CONFIG_635=1" },
#			{ "fn": "639PRO-%s.tar.gz", "config": "CONFIG_639=1" },
#			{ "fn": "660PRO-%s.tar.gz", "config": "CONFIG_660=1" },

#			{ "fn": "620PRO-%s.tar.bz2", "config": "CONFIG_620=1" },
#			{ "fn": "635PRO-%s.tar.bz2", "config": "CONFIG_635=1" },
#			{ "fn": "639PRO-%s.tar.bz2", "config": "CONFIG_639=1" },
#			{ "fn": "660PRO-%s.tar.bz2", "config": "CONFIG_660=1" },
]

OPT_FLAG = ""

def build_pro(build_conf):
	global OPT_FLAG

	if NIGHTLY:
		build_conf += " " + "NIGHTLY=1"

	build_conf += " " + OPT_FLAG
	os.system("make clean %s" % (build_conf))
	os.system("make deps %s" % (build_conf))
	build_conf = "make " + build_conf
	os.system(build_conf)

def copy_sdk():
	try: 
		os.mkdir("dist/sdk")
		os.mkdir("dist/sdk/lib")
		os.mkdir("dist/sdk/include")
	except OSError:
		pass

	shutil.copytree("kuBridgeTest", "dist/sdk/kuBridgeTest")
	shutil.copy("include/kubridge.h", "dist/sdk/include")
	shutil.copy("include/systemctrl.h", "dist/sdk/include")
	shutil.copy("include/systemctrl_se.h", "dist/sdk/include")
	shutil.copy("include/pspvshbridge.h", "dist/sdk/include")
	shutil.copy("libs/libpspkubridge.a", "dist/sdk/lib")
	shutil.copy("libs/libpspsystemctrl_kernel.a", "dist/sdk/lib")
	shutil.copy("libs/libpspsystemctrl_user.a", "dist/sdk/lib")

def restore_chdir():
	os.chdir(os.path.join(os.path.dirname(sys.argv[0]), ".."))

def make_archive(fn):
	shutil.copy("credit.txt", "dist")
	copy_sdk()
	ext = os.path.splitext(fn)[-1].lower()

	try:
		os.remove(fn)
	except OSError:
		pass

	path = os.getcwd()
	os.chdir("dist");

	if ext == ".rar":
		os.system("rar a -r ../%s ." % (fn))
	elif ext == ".gz":
		os.system("tar -zcvf ../%s ." % (fn))
	elif ext == ".bz2":
		os.system("tar -jcvf ../%s ." % (fn))
	elif ext == ".zip":
		os.system("zip -r ../%s ." % (fn))

	os.chdir(path)

def main():
	global OPT_FLAG

	OPT_FLAG = os.getenv("PRO_OPT_FLAG", "")
	restore_chdir()
	
	for conf in PRO_BUILD:
		build_pro(conf["config"])
		make_archive(conf["fn"] % (VERSION))

if __name__ == "__main__":
	main()
