TARGET = systemctrl
OBJS = main.o \
	   syspatch.o \
	   loadexec_patch.o \
	   loadercore_patch.o \
	   memlmd_patch.o \
	   interruptman_patch.o \
	   nid_resolver.o \
	   nid_data.o \
	   elf.o \
	   high_memory.o \
	   systemctrl.o \
	   kubridge.o \
	   module_handler.o \
	   oe_malloc.o \
	   isoload.o \
	   $(PRO_HOME)/Common/libs.o \
	   $(PRO_HOME)/Common/utils.o \
	   $(PRO_HOME)/Common/strsafe.o

IMPORTS = $(PRO_HOME)/Imports/SysMemForKernel.o \
		  $(PRO_HOME)/Imports/LoadCoreForKernel.o \
		  $(PRO_HOME)/Imports/LoadExecForKernel.o \
		  $(PRO_HOME)/Imports/SysMemForKernel.o \
		  $(PRO_HOME)/Imports/ModuleMgrForKernel.o \
		  $(PRO_HOME)/Imports/InterruptManagerForKernel.o

OBJS += $(IMPORTS)

INCDIR = $(PRO_HOME)/Common/ $(PRO_HOME)/include/
CFLAGS = -Os -G0 -Wall -fno-pic

#DEBUG = 1

ifeq ($(DEBUG), 1)
CFLAGS += -DDEBUG=1
OBJS += validate_stub.o $(PRO_HOME)/Common/printk.o $(PRO_HOME)/Imports/IoFileMgrForKernel.o
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
