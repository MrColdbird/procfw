TARGET = recovery
OBJS = main.o vpl.o menu.o gui.o name.o registery.o plugins.o usb.o font.o scr_printf.o font_list.o trans_en.o ../Common/utils.o imports.o

BUILD_PRX=1
PSP_FW_VERSION=635

CFLAGS = -Os -G0 -Wall -I ../include -I ../Common
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

PRX_EXPORTS = exports.exp

LIBS = -lpspsystemctrl_user -lpspkubridge -lpsppower -lpspreg -lpspusb -lpspusbstor -lpspusbdevice
LIBDIR = . ../libs
LDFLAGS =

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build_prx.mak
