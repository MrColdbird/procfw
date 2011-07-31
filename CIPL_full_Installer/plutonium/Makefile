release: all
	"../tool/mksfo.py" $(PSP_FW_VERSION) PARAM$(PSP_FW_VERSION).SFO
	pack-pbp EBOOT.PBP PARAM$(PSP_FW_VERSION).SFO ICO0_green.PNG NULL NULL NULL NULL plutonium_updater.prx NULL
	bin2c EBOOT.PBP plutonium_update_bin.c pupd
	sed -i 's/static//g' plutonium_update_bin.c
	rm -f *.elf *.prx *.o


TARGET = plutonium_updater
OBJS = main.o ../Plutonium_Driver/Plutonium.o ../Plutonium_Driver/plutonium_driver_bin.c

INCDIR = ../../include
LIBDIR = ../../libs

CFLAGS = -Os -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

#EXTRA_TARGETS = EBOOT.PBP
PSP_FW_VERSION = 150
BUILD_PRX = 1

LIBS = -lpspvshbridge -lpsppower -lpspkubridge

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
