TARGET = Rebootex
OBJS = main.o $(PRO_HOME)/Common/utils.o

CFLAGS = -Os -G0 -Wall -I $(PRO_HOME)/Common -I $(PRO_HOME)/include
PSP_FW_VERSION=635

LDFLAGS = -mno-crt0 -nostartfiles -L $(PRO_HOME)/libs
LIBS += -lpspsystemctrl_kernel -lpspkubridge

BUILD_PRX=1
USE_KERNEL_LIBS=1
USE_KERNEL_LIBC=1

PSPSDK = $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
