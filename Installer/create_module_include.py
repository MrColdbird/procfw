#!/usr/bin/python

import os

def main():
	lists = [
			"ISODrivers/Galaxy/galaxy.prx",
			"ISODrivers/March33/march33.prx",
			"ISODrivers/March33/march33_620.prx",
			"ISODrivers/March33/march33_660.prx",
			"ISODrivers/Inferno/inferno.prx",
			"Popcorn/popcorn.prx",
			"Satelite/satelite.prx",
			"Stargate/stargate.prx",
			"SystemControl/systemctrl.prx",
			"usbdevice/usbdevice.prx",
			"Vshctrl/vshctrl.prx",
			"Recovery/recovery.prx",
	]

	for fn in lists:
		path = "../" + fn
		name=os.path.split(fn)[-1]
		name=os.path.splitext(name)[0]
		ret = os.system("bin2c %s %s.h %s"%(path, name, name))
		assert(ret == 0)

if __name__ == "__main__":
	main()
