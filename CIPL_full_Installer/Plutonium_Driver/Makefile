release: all
	psp-build-exports -s exports.exp
	psp-packer plutonium.prx
	bin2c plutonium.prx plutonium_Driver_bin.c plutonium
	sed -i 's/((aligned(16)))/((aligned(64)))/g' plutonium_driver_bin.c
	sed -i 's/static//g' plutonium_driver_bin.c
	rm -f *.elf *.o


TARGET = plutonium
OBJS = main.o

CFLAGS = -Os -G0 -Wall -fno-pic
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS) -c

PSP_FW_VERSION = 150

BUILD_PRX = 1
PRX_EXPORTS = exports.exp

USE_KERNEL_LIBC = 1
USE_KERNEL_LIBS = 1

INCDIR = ../../include

LDFLAGS = -mno-crt0 -nostartfiles
LIBS = -lpsppower_driver -lpspsystemctrl_kernel -lpspmodulemgr_kernel
LIBDIR = ../../libs

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
