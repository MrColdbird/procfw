TARGET = recovery
OBJS = main.o ../Common/utils.o

BUILD_PRX=1
PSP_FW_VERSION=635

CFLAGS = -Os -G0 -Wall -I ../include -I ../Common
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

PRX_EXPORTS = exports.exp

LIBS = -lpspsystemctrl_user -lpspkubridge
LIBDIR = . ../libs
LDFLAGS =

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build_prx.mak
