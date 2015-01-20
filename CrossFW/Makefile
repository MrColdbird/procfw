TARGET_LIB = cross_fw.a
OBJS = cross_fw.o

IMPORTS = ../Imports/SysMemForKernel.o \
		  ../Imports/LoadCoreForKernel.o \
		  ../Imports/SysMemForKernel.o \
		  ../Imports/ModuleMgrForKernel.o \
		  ../Imports/InterruptManagerForKernel.o \
		  ../Imports/sceResmgr_driver.o \
		  ../Imports/InitForKernel.o \
		  ../Imports/LoadExecForKernel.o \
		  ../Imports/sceCtrl_driver.o

OBJS += $(IMPORTS)

INCDIR = ../Common/ ../include/
CFLAGS = -Os -G0 -Wall -fno-pic

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

ifeq ($(CONFIG_661), 1)
CFLAGS += -DCONFIG_661=1
PSP_FW_VERSION = 661
endif

CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

USE_KERNEL_LIBC=1
USE_KERNEL_LIBS=1

LIBDIR = ../libs
LDFLAGS = -mno-crt0 -nostartfiles
LIBS =

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
