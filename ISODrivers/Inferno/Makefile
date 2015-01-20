all:
	../../contrib/pspgz.py inferno.prx ../../contrib/SystemControl.hdr inferno.prx PRO_Inferno_Driver 0x1000

TARGET = inferno
OBJS = main.o iodrv_funcs.o umd.o isoread.o inferno_patch_offset.o isocache.o imports.o ../../Common/utils.o
OBJS += SystemCtrlForKernel.o

CFLAGS = -Os -G0 -Wall -I. -I.. -I ../../Common -I ../../include

ifeq ($(DEBUG), 1)
CFLAGS += -DDEBUG=1
OBJS += ../../Common/printk.o
else
endif

LDFLAGS = -mno-crt0 -nostartfiles -L. -L ../../libs
LIBS = -lpspsystemctrl_kernel -lpspsemaphore

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

ifeq ($(CONFIG_661), 1)
CFLAGS += -DCONFIG_661=1
PSP_FW_VERSION = 661
endif

PRX_EXPORTS = exports.exp

BUILD_PRX=1
USE_KERNEL_LIBS=1
USE_KERNEL_LIBC=1

PSPSDK = $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
