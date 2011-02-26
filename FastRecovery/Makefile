TARGET = 635kernel
OBJS = main.o imports.o syscall.o ../Common/utils.o
LIBS =
CFLAGS = -Os -G0 -Wall -I ../Common -I ../include
CXXFLAGS = $(CFLAGS)
ASFLAGS = $(CFLAGS)
LDFLAGS =
ENCRYPT=1

ifeq ($(DEBUG), 1)
CFLAGS += -DDEBUG=1
OBJS += ../Common/printk.o
else
endif

BUILD_PRX = 1

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = 635PRO-B Fast Recovery
PSP_EBOOT_ICON = ICON0.PNG
PSP_FW_VERSION = 635

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
