TARGET = permanent
OBJS = main.o crypto.o kirk_engine.o imports.o KModule.o

INCDIR = ../../include
CFLAGS = -Os -G0 -Wall -mno-gpopt
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LDFLAGS =
LIBS = -lz -lpsppower -lpsputility
LIBDIR = ../../libs

PSP_FW_VERSION = 620

BUILD_PRX = 1
EXTRA_TARGETS=EBOOT.PBP
PSP_EBOOT_TITLE=620 PRO-B Permanent Patch
PSP_EBOOT_ICON = ICON0.PNG

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
