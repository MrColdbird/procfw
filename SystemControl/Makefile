all:
	../contrib/pspgz.py ../dist/Installer/systemctrl.prx ../contrib/SystemControl.hdr systemctrl.prx

TARGET = systemctrl
OBJS = main.o \
	   syspatch.o \
	   loadexec_patch.o \
	   rebootex_conf.o \
	   loadercore_patch.o \
	   memlmd_patch.o \
	   interruptman_patch.o \
	   mediasync_patch.o \
	   version_spoof.o \
	   march33_patch.o \
	   nid_resolver.o \
	   nid_data.o \
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
	   ../Common/libs.o \
	   ../Common/utils.o \
	   ../Common/strsafe.o

IMPORTS = ../Imports/SysMemForKernel.o \
		  ../Imports/LoadCoreForKernel.o \
		  ../Imports/LoadExecForKernel.o \
		  ../Imports/SysMemForKernel.o \
		  ../Imports/ModuleMgrForKernel.o \
		  ../Imports/InterruptManagerForKernel.o \
		  ../Imports/sceResmgr_driver.o \
		  ../Imports/InitForKernel.o

OBJS += $(IMPORTS)

INCDIR = ../Common/ ../include/
CFLAGS = -Os -G0 -Wall -fno-pic

#DEBUG = 1

ifeq ($(DEBUG), 1)
CFLAGS += -DDEBUG=1
OBJS += validate_stub.o ../Common/printk.o
endif

CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

PSP_FW_VERSION = 635

BUILD_PRX = 1
PRX_EXPORTS = exports.exp

USE_KERNEL_LIBC=1
USE_KERNEL_LIBS=1

LIBDIR = ./lib
LDFLAGS = -mno-crt0 -nostartfiles
LIBS = 

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
