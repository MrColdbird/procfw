PSPDEV=$(shell psp-config --pspsdk-path)
INCLUDES=-I $(PSPDEV)/include

ifeq ($(PSP_MODEL),1)
TARGET = patch_02g
else
TARGET = patch_01g
endif

CFLAGS=$(INCLUDES) -W -Wall -G0 -Os -DPSP_MODEL=$(PSP_MODEL) -D_PSP_FW_VERSION=$(PSP_FW_VERSION)

all:	a.bin

a.bin: 
	psp-gcc $(CFLAGS) -fno-pic -mno-abicalls -w -S main.c -o main.s
	psp-as main.s -o main.o
	psp-ld -T linkfile.l main.o -o main.elf
	psp-strip -s main.elf
	psp-objcopy -O binary main.elf $(TARGET).bin
#	bin2c h.bin rebootex.h rebootex
	@rm -rf *~ *.o *.elf *.s

clean:
	@rm -f *.bin
