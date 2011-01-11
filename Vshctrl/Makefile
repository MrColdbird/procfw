TARGET = vshctrl
OBJS = main.o vshpatch.o ../Common/utils.o

IMPORTS =  ../Imports/SysMemForKernel.o \
		   ../Imports/LoadCoreForKernel.o \
		  ../Imports/LoadExecForKernel.o \
		  ../Imports/SysMemForKernel.o \
		  ../Imports/ModuleMgrForKernel.o \

OBJS += $(IMPORTS)

INCDIR = ../Common/ ../include/
CFLAGS = -Os -G0 -Wall -fno-pic

#DEBUG = 1

ifeq ($(DEBUG), 1)
CFLAGS += -DDEBUG=1
OBJS += ../Common/printk.o ../Imports/IoFileMgrForKernel.o
endif

CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

PSP_FW_VERSION = 635

BUILD_PRX = 1
PRX_EXPORTS = exports.exp

USE_KERNEL_LIBC=1
USE_KERNEL_LIBS=1

LIBDIR = ../libs
LDFLAGS = -mno-crt0 -nostartfiles
LIBS = -lpspkubridge -lpspsystemctrl_kernel

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
