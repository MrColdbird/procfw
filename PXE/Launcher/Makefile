TARGET = 635kernel
OBJS = main.o imports.o syscall.o ../../Common/utils.o
LIBS = -lpspsystemctrl_user
LIBDIR = . ../../libs
CFLAGS = -Os -G0 -Wall -I ../../Common
CXXFLAGS = $(CFLAGS)
ASFLAGS = $(CFLAGS)
LDFLAGS =
INCDIR = ../../Common/ ../../include/
ENCRYPT=1

ifeq ($(DEBUG), 1)
CFLAGS += -DDEBUG=1
OBJS += ../../Common/printk.o
else
endif

BUILD_PRX = 1

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = 635 PRO-B Updater
PSP_EBOOT_ICON = ICON0.PNG
PSP_FW_VERSION = 635

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
