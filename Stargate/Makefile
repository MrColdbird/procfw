all:
	../contrib/pspgz.py ../dist/UPDATE/stargate.prx ../contrib/SystemControl.hdr stargate.prx stargate

TARGET = stargate
OBJS = main.o key_decrypt.o pspcipher.o pauth.o nodrm.o utility_patch.o ../Common/utils.o ../Common/libs.o

INCDIR = ../include ../Common
CFLAGS = -Os -G0 -Wall -fno-pic

ifeq ($(DEBUG), 1)
CFLAGS += -DDEBUG=1
OBJS += ../Common/printk.o
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
LIBS = -lpspsemaphore -lpspsystemctrl_kernel -lpspkubridge -lpspmodulemgr_kernel

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
