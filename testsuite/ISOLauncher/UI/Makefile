TARGET = ui
OBJS = main.o conf.o ../../../Common/strsafe.o psplink.o

CFLAGS = -Os -G0 -Wall -I. -I .. -I../../../Common -I../../../include -fno-toplevel-reorder
PSP_FW_VERSION=635

ifeq ($(DEBUG), 1)
CFLAGS += -DDEBUG=1
OBJS += ../../../Common/printk.o
else
endif

LDFLAGS = -L. -L../../../libs
LIBS += -lpspgu -lpspkubridge

BUILD_PRX=1

PRX_EXPORTS = export.exp
EXTRA_TARGETS += EBOOT.PBP
PSP_EBOOT_TITLE = ISO Loader for 6.35PRO

PSPSDK = $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
