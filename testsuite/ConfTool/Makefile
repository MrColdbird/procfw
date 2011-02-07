TARGET = conftool
OBJS = main.o ../../Common/strsafe.o

CFLAGS = -Os -G0 -Wall -I. -I .. -I../../Common -I../../include -fno-toplevel-reorder
PSP_FW_VERSION=635

ifeq ($(DEBUG), 1)
CFLAGS += -DDEBUG=1
OBJS += ../../Common/printk.o ../../Common/utils.o
else
endif

LDFLAGS = -L. -L../../libs
LIBS += -lpspgu -lpspkubridge -lpspsystemctrl_user

BUILD_PRX=1

PRX_EXPORTS = export.exp
EXTRA_TARGETS += EBOOT.PBP
PSP_EBOOT_TITLE = 6.35PRO Configure Tool

PSPSDK = $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
