TARGET = vshctrl
OBJS = main.o vshpatch.o xmbiso.o isoreader.o virtual_pbp.o ../Common/utils.o ../Common/libs.o ../Common/strsafe.o
IMPORTS = ../Imports/LoadExecForKernel.o

INCDIR = ../Common/ ../include/
CFLAGS = -Os -G0 -Wall -fno-pic

#DEBUG = 1

ifeq ($(DEBUG), 1)
CFLAGS += -DDEBUG=1
OBJS += ../Common/printk.o
endif

OBJS += $(IMPORTS)

CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

PSP_FW_VERSION = 635

BUILD_PRX = 1
PRX_EXPORTS = exports.exp

USE_KERNEL_LIBC=1
USE_KERNEL_LIBS=1

LIBDIR = ../libs
LDFLAGS = -mno-crt0 -nostartfiles
LIBS = -lpspsystemctrl_kernel

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
