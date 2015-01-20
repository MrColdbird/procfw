all:
	../contrib/pspgz.py systemctrl.prx ../contrib/SystemControl.hdr systemctrl.prx

TARGET = systemctrl
OBJS = main.o \
	   syspatch.o \
	   loadexec_patch.o \
	   rebootex_conf.o \
	   loadercore_patch.o \
	   startmodule_patch.o \
	   memlmd_patch.o \
	   interruptman_patch.o \
	   mediasync_patch.o \
	   sysmem_patch.o \
	   version_spoof.o \
	   usbversionfake.o \
	   march33_patch.o \
	   nid_resolver.o \
	   elf.o \
	   conf.o \
	   high_memory.o \
	   systemctrl.o \
	   kubridge.o \
	   module_handler.o \
	   usb_charge.o \
	   oe_malloc.o \
	   plugins.o \
	   psnfix.o \
	   isoload.o \
	   cpuclock.o \
	   fake_region.o \
	   msstor_cache.o \
	   libertas.o \
	   systemctrl_patch_offset.o \
	   ../Common/libs.o \
	   ../Common/utils.o \
	   ../Common/strsafe.o \
	   setlongjmp.o \
	   lz4.o

INCDIR = ../Common/ ../include/
CFLAGS = -Os -G0 -Wall -fno-pic

ifeq ($(DEBUG), 1)
CFLAGS += -DDEBUG=1
OBJS += validate_stub.o ../Common/printk.o
endif

ifeq ($(CONFIG_620), 1)
CFLAGS += -DCONFIG_620=1
OBJS += nid_620_data.o
PSP_FW_VERSION = 620
endif

ifeq ($(CONFIG_635), 1)
CFLAGS += -DCONFIG_635=1
OBJS += nid_635_data.o
PSP_FW_VERSION = 635
endif

ifeq ($(CONFIG_639), 1)
CFLAGS += -DCONFIG_639=1
OBJS += nid_639_data.o
PSP_FW_VERSION = 639
endif

ifeq ($(CONFIG_660), 1)
CFLAGS += -DCONFIG_660=1
OBJS += nid_660_data.o
PSP_FW_VERSION = 660
endif

ifeq ($(CONFIG_661), 1)
CFLAGS += -DCONFIG_661=1
OBJS += nid_660_data.o
PSP_FW_VERSION = 661
endif

CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

BUILD_PRX = 1
PRX_EXPORTS = exports.exp

USE_KERNEL_LIBC=1
USE_KERNEL_LIBS=1

LIBDIR = ../libs
LDFLAGS = -mno-crt0 -nostartfiles
LIBS = -lpspsemaphore ../CrossFW/cross_fw.a

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
