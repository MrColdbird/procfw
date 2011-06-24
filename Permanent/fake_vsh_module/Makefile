TARGET = hen
OBJS = main.o exploit.o ../../PXE/Launcher/launcher_patch_offset.o syscall.o imports.o

all:

main.c: rebootex_bin.h

rebootex_bin.h:
	bin2c ../../Rebootex_bin/rebootex.bin $@ rebootex_bin

INCDIR = ../../Common ../../include
CFLAGS = -O2 -G0 -Wall -mno-gpopt -DCONFIG_620=1

CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LDFLAGS =
LIBS = -lpsppower -lpsputility

PSP_FW_VERSION = 620

EXTRA_TARGETS = EBOOT.PBP
EXTRA_CLEAN = rebootex_bin.h
PSP_EBOOT_TITLE = HEN

BUILD_PRX = 1

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
