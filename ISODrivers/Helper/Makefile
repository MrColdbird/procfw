TARGET = test
OBJS = main.o

CFLAGS = -Os -G0 -Wall -I. -I.. -I ../../Common -I ../../include

ifeq ($(DEBUG), 1)
CFLAGS += -DDEBUG=1
OBJS += ../../Common/printk.o
else
endif

LDFLAGS = -mno-crt0 -nostartfiles -L. -L ../../libs
LIBS = -lpspsystemctrl_kernel -lpspkubridge -lpspreg_driver -lpsprtc -lpspumd_driver
PSP_FW_VERSION=635
#PRX_EXPORTS = exports.exp

BUILD_PRX=1
USE_KERNEL_LIBS=1
USE_KERNEL_LIBC=1

PSPSDK = $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
