TARGET = systemctrl
OBJS = main.o syspatch.o systemctrl.o kubridge.o modhandler.o elf.o ../../SystemControl/memlmd_patch.o ../../Common/libs.o ../../Common/utils.o ../../Imports/sceResmgr_driver.o ../../Imports/InterruptManagerForKernel.o ../../Imports/LoadCoreForKernel.o ../../Imports/ModuleMgrForKernel.o ../../Imports/SysMemForKernel.o ../../Imports/LoadExecForKernel.o

INCDIR = ../../Common/ ../../include/
CFLAGS = -Os -G0 -Wall

ifeq ($(DEBUG), 1)
CFLAGS += -DDEBUG=1
OBJS += ../../Common/printk.o
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
