TARGET = flasher
OBJS = main.o pspIplUpdate.o kpspident.o

LIBS = -lpspkubridge
INCDIR = ../Common/ ../include/
LIBDIR = ../libs

BUILD_PRX = 1
PSP_FW_VERSION = 635

CFLAGS = -O2 -G0 -Wall -fshort-wchar
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = CIPL Flasher For PRO-B
PSP_EBOOT_ICON = ICON0.PNG

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
