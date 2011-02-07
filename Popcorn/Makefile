TARGET = popcorn
OBJS = main.o icon.o

CFLAGS = -Os -G0 -Wall -I. -I../Common -I../include
PSP_FW_VERSION=635

ifeq ($(DEBUG), 1)
CFLAGS += -DDEBUG=1
OBJS += ../Common/printk.o ../Common/utils.o
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
