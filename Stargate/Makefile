all:
	../contrib/pspgz.py stargate.prx ../contrib/SystemControl.hdr stargate.prx stargate 0x1007

TARGET = stargate
OBJS = main.o key_decrypt.o pauth.o nodrm.o hide.o utility_patch.o loadmodule_patch.o analog_patch.o stargate_patch_offset.o fix_old_fw.o chn_iso.o blacklist.o pspcipher.o ../Common/utils.o ../Common/libs.o ../Common/strsafe.o

INCDIR = ../include ../Common
CFLAGS = -Os -G0 -Wall -fno-pic

ifeq ($(PSID_CHECK), 1)
CFLAGS += -DPSID_CHECK=1
OBJS += psid_check.o
endif

ifeq ($(DEBUG), 1)
CFLAGS += -DDEBUG=1
OBJS += ../Common/printk.o
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

CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

BUILD_PRX = 1
PRX_EXPORTS = exports.exp

USE_KERNEL_LIBC=1
USE_KERNEL_LIBS=1

LIBDIR = ../libs
LDFLAGS = -mno-crt0 -nostartfiles
LIBS = -lpspsemaphore -lpspsystemctrl_kernel -lpspkubridge -lpspmodulemgr_kernel

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
