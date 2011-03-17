release: all
	psp-fixup-imports -m $(PSP_FW_VERSION)mapfile.txt $(TARGET).prx

TARGET = Rebootex
OBJS = main.o rebootex_patch_offset.o ../Common/utils.o

CFLAGS = -Os -G0 -Wall -I ../Common -I ../include
PSP_FW_VERSION=635

LDFLAGS = -mno-crt0 -nostartfiles -L ../libs
LIBS += -lpspsystemctrl_kernel -lpspkubridge

BUILD_PRX=1
USE_KERNEL_LIBS=1
USE_KERNEL_LIBC=1

PSPSDK = $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
