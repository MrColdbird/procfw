TARGET = galaxy
OBJS = main.o
OBJS += $(PRO_HOME)/Imports/SysMemForKernel.o \
		SystemCtrlForKernel.o

CFLAGS = -Os -G0 -Wall -I. -I$(PRO_HOME)/include -I$(PRO_HOME)/Common

LDFLAGS = -mno-crt0 -nostartfiles -L. -L$(PRO_HOME)/libs

ifeq ($(DEBUG), 1)
CFLAGS += -DDEBUG=1
OBJS += $(PRO_HOME)/Common/printk.o \
		$(PRO_HOME)/Common/utils.o
endif

BUILD_PRX=1
USE_KERNEL_LIBS=1
USE_KERNEL_LIBC=1
PSP_FW_VERSION=635

PRX_EXPORTS = export.exp

PSPSDK = $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
