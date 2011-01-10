TARGET = stargate
OBJS = main.o key_decrypt.o pspcipher.o pauth.o $(PRO_HOME)/Common/utils.o $(PRO_HOME)/Common/libs.o

INCDIR = $(PRO_HOME)/include $(PRO_HOME)/Common
CFLAGS = -Os -G0 -Wall -fno-pic

ifeq ($(DEBUG), 1)
CFLAGS += -DDEBUG=1
OBJS += $(PRO_HOME)/Common/printk.o
endif

CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

PSP_FW_VERSION = 500

BUILD_PRX = 1
PRX_EXPORTS = exports.exp

USE_KERNEL_LIBC=1
USE_KERNEL_LIBS=1

LIBDIR = $(PRO_HOME)/libs
LDFLAGS = -mno-crt0 -nostartfiles
LIBS = -lpspsemaphore -lpspsystemctrl_kernel -lpspkubridge -lpspmodulemgr_kernel

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
