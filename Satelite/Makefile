TARGET = satelite
OBJS = main.o blit.o ui.o menu.o clock.o stubkk.o

BUILD_PRX=1
PSP_FW_VERSION=635

INCDIR = ../include ../Common
CFLAGS = -Os -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

PRX_EXPORTS = exports.exp

LIBS = -lpspsystemctrl_user
LIBDIR = . ../libs
LDFLAGS =

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build_prx.mak
