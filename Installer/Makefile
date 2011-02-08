TARGET = 635PRO
OBJS = main.o

BUILD_PRX=1
EXTRA_TARGETS=EBOOT.PBP
PSP_LARGE_MEMORY = 0

PSP_EBOOT_TITLE=6.35PRO-A Updater
PSP_EBOOT_ICON=ICON0.PNG

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
