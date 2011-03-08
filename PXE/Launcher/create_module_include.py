#!/usr/bin/python

class FakeTime:
    def time(self):
        return 1225856967.109

import os, gzip, StringIO

gzip.time = FakeTime()

def create_gzip(input, output):
	f_in=open(input, 'rb')
	temp=StringIO.StringIO()
	f=gzip.GzipFile(fileobj=temp, mode='wb')
	f.writelines(f_in)
	f.close()
	f_in.close()
	fout=open(output, 'wb')
	temp.seek(0)
	fout.writelines(temp)
	fout.close()
	temp.close()

def cleanup():
	del_list = [
		"installer.prx.gz",
		"Rebootex.prx.gz",
	]

	for file in del_list:
		try:
			os.remove(file)
		except OSError:
			pass

def main():
	create_gzip("../../Installer/installer.prx", "installer.prx.gz")
	create_gzip("../../Rebootex/Rebootex.prx", "Rebootex.prx.gz")
	os.system("bin2c installer.prx.gz installer.h installer")
	os.system("bin2c Rebootex.prx.gz Rebootex_prx.h Rebootex_prx")
	cleanup()

if __name__ == "__main__":
	main()
