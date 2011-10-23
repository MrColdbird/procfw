all:
	../../contrib/pspgz.py galaxy.prx ../../contrib/SystemControl.hdr galaxy.prx PROGalaxyController 0x1006

TARGET = galaxy
OBJS = main.o galaxy_patch_offset.o isocache.o imports.o
OBJS += SystemCtrlForKernel.o

CFLAGS = -Os -G0 -Wall -I. -I../../include -I../../Common

LDFLAGS = -mno-crt0 -nostartfiles -L. -L../../libs

ifeq ($(DEBUG), 1)
CFLAGS += -DDEBUG=1
OBJS += ../../Common/printk.o \
		../../Common/utils.o
endif

ifeq ($(CONFIG_620), 1)
CFLAGS += -DCONFIG_620=1
PSP_FW_VERSION = 620
endif

ifeq ($(CONFIG_635), 1)
CFLAGS += -DCONFIG_635=1
PSP_FW_VERSION = 635
endif

ifeq ($(CONFIG_639), 1)
CFLAGS += -DCONFIG_639=1
PSP_FW_VERSION = 639
endif

ifeq ($(CONFIG_660), 1)
CFLAGS += -DCONFIG_660=1
PSP_FW_VERSION = 660
endif

BUILD_PRX=1
USE_KERNEL_LIBS=1
USE_KERNEL_LIBC=1

PRX_EXPORTS = export.exp
LIBS += -lpspsystemctrl_kernel -lpspsemaphore 

PSPSDK = $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
