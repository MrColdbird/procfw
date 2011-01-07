TARGET = systemctrl
OBJS = main.o $(PRO_HOME)/Imports/SysMemForKernel.o

INCDIR = $(PRO_HOME)/Common/ $(PRO_HOME)/include/
CFLAGS = -Os -G0 -Wall -fno-pic

#DEBUG = 1

ifeq ($(DEBUG), 1)
CFLAGS += -DDEBUG=1
OBJS += $(PRO_HOME)/Common/printk.o $(PRO_HOME)/Imports/IoFileMgrForKernel.o
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
