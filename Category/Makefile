TARGET = category
OBJS = main.o category.o iopatch.o systempatch.o imports.o

BUILD_PRX=1
PSP_FW_VERSION=635

INCDIR = ../include ../Common
CFLAGS = -std=gnu99 -Os -G0 -Wall -fshort-wchar
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

PRX_EXPORTS = exports.exp

LIBS = -lpspsystemctrl_user -lpsprtc -lpspsysmem_user
LIBDIR = . ../libs
LDFLAGS =

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build_prx.mak
