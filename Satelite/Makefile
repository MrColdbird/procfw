TARGET = satelite
OBJS = main.o blit.o ui.o menu.o clock.o stubkk.o exports.o 

BUILD_PRX=1

INCDIR = ../include ../Common
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

PRX_EXPORTS = exports.exp

LIBDIR =
LDFLAGS =

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build_prx.mak
