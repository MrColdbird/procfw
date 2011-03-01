TARGET = installer
OBJS = main.o
EXTRA_CLEAN= galaxy.h march33.h popcorn.h satelite.h stargate.h systemctrl.h usbdevice.h vshctrl.h

BUILD_PRX=1

all: $(OBJS)

$(OBJS): pre-build

pre-build:
	./create_module_include.py
	touch main.c

EXTRA_TARGETS=EBOOT.PBP
PSP_EBOOT_TITLE=6.35PRO-B Updater(debug)

INCDIR = ../include ../Common
CFLAGS = -Os -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

PSP_FW_VERSION = 635

LIBDIR = ../libs
LDFLAGS =
LIBS =  -lpspkubridge -lpsppower

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
