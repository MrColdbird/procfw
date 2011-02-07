all:
	../../contrib/pspgz.py ../../dist/UPDATE/galaxy.prx ../../contrib/SystemControl.hdr galaxy.prx M33GalaxyController

TARGET = galaxy
OBJS = main.o
OBJS += ../../Imports/SysMemForKernel.o \
		SystemCtrlForKernel.o

CFLAGS = -Os -G0 -Wall -I. -I../../include -I../../Common

LDFLAGS = -mno-crt0 -nostartfiles -L. -L../../libs

ifeq ($(DEBUG), 1)
CFLAGS += -DDEBUG=1
OBJS += ../../Common/printk.o \
		../../Common/utils.o
endif

BUILD_PRX=1
USE_KERNEL_LIBS=1
USE_KERNEL_LIBC=1
PSP_FW_VERSION=635

PRX_EXPORTS = export.exp

PSPSDK = $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
