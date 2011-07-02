release: all
	bin2c $(TARGET).prx $(TARGET)_bin.c $(TARGET)
	sed -i 's/static//g' $(TARGET)_bin.c
	"../tool/md5.py" $(TARGET).prx $(TARGET)_md5.h hash235
	rm -f *.elf *.o


TARGET = u235
OBJS = main.o ../Plutonium_Driver/Plutonium.o

PSP_FW_VERSION = 150
BUILD_PRX = 1

INCDIR = ../../Common

CFLAGS = -Os -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR = 
LDFLAGS =
LIBS = 


#EXTRA_TARGETS = EBOOT.PBP
#PSP_EBOOT_TITLE = recovery

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
