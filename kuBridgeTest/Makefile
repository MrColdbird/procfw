TARGET = ku_test
OBJS = main.o

CFLAGS = -Os -G0 -Wall -I. -I ../include
PSP_FW_VERSION=620

LDFLAGS = -mno-crt0 -nostartfiles -L. -L ../libs
LIBS = -lpspumd -lpsppower -lpspkubridge -lpspsystemctrl_user -lpspdebug

BUILD_PRX=1

PSPSDK = $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
