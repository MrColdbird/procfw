TARGET = fast_recovery
OBJS = main.o imports.o syscall.o ../PXE/Launcher/launcher_patch_offset.o ../Common/utils.o
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

BUILD_PRX = 1
PSP_EBOOT_TITLE += PRO-B Fast Recovery

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_ICON = ICON0.PNG
PSP_FW_VERSION = 635

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
