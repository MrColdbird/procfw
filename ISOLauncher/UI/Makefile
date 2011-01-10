TARGET = ui
OBJS = main.o conf.o $(PRO_HOME)/Common/strsafe.o psplink.o

CFLAGS = -Os -G0 -Wall -I. -I .. -I$(PRO_HOME)/Common -I$(PRO_HOME)/include -fno-toplevel-reorder
PSP_FW_VERSION=635

ifeq ($(DEBUG), 1)
CFLAGS += -DDEBUG=1
OBJS += $(PRO_HOME)/Common/printk.o
else
endif

LDFLAGS = -L. -L$(PRO_HOME)/libs
LIBS += -lpspgu -lpspkubridge

BUILD_PRX=1

PRX_EXPORTS = export.exp
EXTRA_TARGETS += EBOOT.PBP
PSP_EBOOT_TITLE = ISO Loader for 6.35PRO

PSPSDK = $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
