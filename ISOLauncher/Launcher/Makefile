TARGET = launcher
OBJS = main.o $(PRO_HOME)/Common/utils.o $(PRO_HOME)/Common/strsafe.o

CFLAGS = -Os -G0 -Wall -I. -I.. -I$(PRO_HOME)/Common -I$(PRO_HOME)/include

ifeq ($(DEBUG), 1)
CFLAGS += -DDEBUG=1
OBJS += $(PRO_HOME)/Common/printk.o
else
endif

LDFLAGS = -mno-crt0 -nostartfiles -L. -L$(PRO_HOME)/libs
LIBS = -lpspsystemctrl_kernel -lpspkubridge
PSP_FW_VERSION=635

BUILD_PRX=1
USE_KERNEL_LIBS=1
USE_KERNEL_LIBC=1

PSPSDK = $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak


