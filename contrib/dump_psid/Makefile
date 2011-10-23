EXTRA_TARGETS=EBOOT.PBP
PSP_EBOOT_TITLE=PSID dumper
TARGET = dump_psid
OBJS = main.o

CFLAGS = -Os -G0 -Wall -I../../Common -I ../../include
PSP_FW_VERSION=620

CFLAGS += -DDEBUG=1
OBJS += ../../Common/printk.o ../../Common/utils.o

LDFLAGS = -mno-crt0 -nostartfiles -L../../libs
LIBS += -lpspsystemctrl_kernel -lpspsemaphore

BUILD_PRX=1
USE_KERNEL_LIBS=1
USE_KERNEL_LIBC=1

PSPSDK = $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
