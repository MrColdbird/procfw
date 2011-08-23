TARGET = pxe_launcher
OBJS = main.o imports.o launcher_patch_offset.o syscall.o ../../Common/utils.o
EXTRA_CLEAN = Rebootex_prx.h Installer.h

all: $(OBJS)

$(OBJS): pre-build

pre-build:
	./create_module_include.py
	touch main.c

LIBS = -lpspsystemctrl_user -lz
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

ifeq ($(CONFIG_620), 1)
CFLAGS += -DCONFIG_620=1
PSP_FW_VERSION = 620
PSP_EBOOT_TITLE = 620
endif

ifeq ($(CONFIG_635), 1)
CFLAGS += -DCONFIG_635=1
PSP_FW_VERSION = 635
PSP_EBOOT_TITLE = 635
endif

ifeq ($(CONFIG_639), 1)
OBJS += kxp_639.o
CFLAGS += -DCONFIG_639=1
PSP_FW_VERSION = 639
PSP_EBOOT_TITLE = 639
endif

ifeq ($(CONFIG_660), 1)
OBJS += kxp_660.o
CFLAGS += -DCONFIG_660=1
PSP_FW_VERSION = 660
PSP_EBOOT_TITLE = 660
endif

BUILD_PRX = 1
PSP_EBOOT_TITLE += PRO-B Updater

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_ICON = ICON0.PNG
PSP_EBOOT_PIC1 = PIC1.PNG

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
