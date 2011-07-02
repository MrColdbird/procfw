
EXTRA_CLEAN = galaxy.h march33.h march33_620.h popcorn.h satelite.h stargate.h systemctrl.h usbdevice.h vshctrl.h
EXTRA_CLEAN+= inferno.h recovery.h

TARGET_FW = 150

ifeq ($(CONFIG_620), 1)
TARGET_FW = 620
endif

ifeq ($(CONFIG_635), 1)
TARGET_FW = 635
endif

ifeq ($(CONFIG_639), 1)
TARGET_FW = 639
endif

all: 
	"../Installer/create_module_include.py"
	make -C u235			PSP_FW_VERSION=$(TARGET_FW)
	make -C Plutonium_Driver PSP_FW_VERSION=$(TARGET_FW)
	make -C plutonium 		PSP_FW_VERSION=$(TARGET_FW)
	make -C Downloader 		PSP_FW_VERSION=$(TARGET_FW)
	make -C unpacker 		PSP_FW_VERSION=$(TARGET_FW)
	cp unpacker/EBOOT.PBP EBOOT.PBP

clean:
	make clean -C u235
	make clean -C Plutonium_Driver
	make clean -C plutonium
	make clean -C Downloader
	make clean -C unpacker
	@rm -f EBOOT.PBP
	@rm -f $(EXTRA_CLEAN)
