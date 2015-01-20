all:
	../contrib/pspgz.py popcorn.prx ../contrib/SystemControl.hdr popcorn.prx PROPopcornManager 0x1007

TARGET = popcorn

OBJS = main.o icon.o popcorn_patch_offset.o ../Stargate/analog_patch.o
OBJS += ../Common/utils.o ../Common/strsafe.o ../Common/libs.o

CFLAGS = -Os -G0 -Wall -I. -I../Common -I../include

ifeq ($(DEBUG), 1)
CFLAGS += -DDEBUG=1
OBJS += ../Common/printk.o
else
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

ifeq ($(CONFIG_661), 1)
CFLAGS += -DCONFIG_661=1
PSP_FW_VERSION = 661
endif

LDFLAGS = -mno-crt0 -nostartfiles -L. -L../libs
LIBS += -lpspmodulemgr_kernel -lpspsystemctrl_kernel -lpspkubridge
LIBS += -lpspsemaphore

BUILD_PRX=1
PRX_EXPORTS = exports.exp

USE_KERNEL_LIBS=1
USE_KERNEL_LIBC=1

PSPSDK = $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
