TARGET = libpsardumper
OBJS = main.o pspDecrypt.o

INCDIR = ../include
CFLAGS = -O2 -G0 -Wall -fno-pic
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

PSP_FW_VERSION = 271

BUILD_PRX = 1
PRX_EXPORTS = libpsardumper.exp

USE_KERNEL_LIBC=1
USE_KERNEL_LIBS=1

LIBDIR = ../lib
LDFLAGS = -mno-crt0 -nostartfiles
LIBS = -lpspsemaphore -lpspmesgd_driver -lpspnwman_driver


PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
