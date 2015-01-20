all:
	../../contrib/pspgz.py ../RebootexPXE/systemctrl.prx ../../contrib/SystemControl.hdr systemctrl.prx

TARGET = systemctrl
OBJS = main.o syspatch.o systemctrl.o kubridge.o modhandler.o elf.o systemctrl_pxe_patch_offset.o ../../SystemControl/systemctrl_patch_offset.o ../../SystemControl/memlmd_patch.o ../../Common/libs.o ../../Common/utils.o

INCDIR = ../../Common/ ../../include/ ../../SystemControl/
CFLAGS = -Os -G0 -Wall

ifeq ($(DEBUG), 1)
CFLAGS += -DDEBUG=1
OBJS += ../../Common/printk.o
endif

CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

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

BUILD_PRX = 1
PRX_EXPORTS = exports.exp

USE_KERNEL_LIBC=1
USE_KERNEL_LIBS=1

LIBDIR = ./lib
LDFLAGS = -mno-crt0 -nostartfiles
LIBS = ../../CrossFW/cross_fw.a

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
