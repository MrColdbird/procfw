release: all
	"../tool/mksfo.py" $(PSP_FW_VERSION) PARAM$(PSP_FW_VERSION).SFO
	pack-pbp EBOOT.PBP PARAM$(PSP_FW_VERSION).SFO ICON0.PNG NULL NULL NULL NULL plutonium_updater.prx NULL
#	psp-packer EBOOT.PBP
	rm -f *.elf *.prx *.o


TARGET = plutonium_updater
OBJS = main.o ../plutonium/plutonium_update_bin.c ../u235/u235_bin.c pspIplUpdate.o ../Downloader/downloader_bin.o


CFLAGS = -Os -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

INCDIR = ../../include ../../Common
#EXTRA_TARGETS = EBOOT.PBP
PSP_FW_VERSION = 150
BUILD_PRX = 1

LIBDIR = ../../libs
LIBS = -lpspvshbridge -lpsppower -lpspsystemctrl_user -lpspkubridge

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
