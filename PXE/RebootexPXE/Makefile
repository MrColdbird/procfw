PSPSDK=$(shell psp-config --pspsdk-path)
INCLUDES=-I $(PSPSDK)/include -I ../../Common

all: a.bin

CFLAGS=$(INCLUDES) -W -Wall -Os -G0 -fno-pic -fno-toplevel-reorder -mno-abicalls -w -S

ifeq ($(CONFIG_635), 1)
CFLAGS+=-DCONFIG_635=1
endif

ifeq ($(CONFIG_620), 1)
CFLAGS+=-DCONFIG_620=1
endif

ifeq ($(CONFIG_639), 1)
CFLAGS+=-DCONFIG_639=1
endif

ifeq ($(CONFIG_660), 1)
CFLAGS+=-DCONFIG_660=1
endif

ifeq ($(CONFIG_661), 1)
CFLAGS+=-DCONFIG_661=1
endif

a.bin:
	bin2c systemctrl.prx systemctrl.h systemcontrol
	psp-gcc $(CFLAGS) main.c -o main.s
	psp-as main.s -o main.o
	psp-ld -T linkfile.l main.o -o main.elf
	psp-strip -s main.elf
	psp-objcopy -O binary main.elf rebootex.bin
	bin2c rebootex.bin rebootex.h rebootex

clean:
	rm -rf *~ *.s *.o *.elf *.bin rebootex.h systemctrl.h systemctrl.prx
