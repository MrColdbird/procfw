TARGET = Rebootex
OBJS = main.o ../PXE/Launcher/launcher_patch_offset.o ../Common/utils.o

CFLAGS = -Os -G0 -Wall -I ../Common -I ../include

ifeq ($(CONFIG_620), 1)
CFLAGS += -DCONFIG_620=1
PSP_FW_VERSION = 620
endif

ifeq ($(CONFIG_635), 1)
CFLAGS += -DCONFIG_635=1
PSP_FW_VERSION = 635
endif

ifeq ($(CONFIG_639), 1)
CFLAGS += -DCONFIG_639=1
PSP_FW_VERSION = 639
endif

ifeq ($(CONFIG_660), 1)
CFLAGS += -DCONFIG_660=1
PSP_FW_VERSION = 660
endif

LDFLAGS = -mno-crt0 -nostartfiles -L ../libs
LIBS += -lpspkubridge ../CrossFW/cross_fw.a

BUILD_PRX=1
USE_KERNEL_LIBS=1
USE_KERNEL_LIBC=1

PSPSDK = $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
