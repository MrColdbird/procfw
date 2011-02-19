all:
	../contrib/pspgz.py ../dist/635PROUPDATE/popcorn.prx ../contrib/SystemControl.hdr popcorn.prx M33PopcornManager 0x1007

TARGET = popcorn

ifeq ($(OBSERVER), 1)
OBJS = main_observer.o 
else
ifeq ($(PSNSHARE), 1)
OBJS = main_psnshare.o icon.o
else
OBJS = main.o icon.o
endif
endif

OBJS += ../Common/utils.o ../Common/strsafe.o

CFLAGS = -Os -G0 -Wall -I. -I../Common -I../include
PSP_FW_VERSION=635

ifeq ($(DEBUG), 1)
CFLAGS += -DDEBUG=1
OBJS += ../Common/printk.o
else
endif

LDFLAGS = -mno-crt0 -nostartfiles -L. -L../libs
LIBS += -lpspmodulemgr_kernel -lpspsystemctrl_kernel -lpspkubridge
LIBS += -lpspsemaphore

BUILD_PRX=1
USE_KERNEL_LIBS=1
USE_KERNEL_LIBC=1

PSPSDK = $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
