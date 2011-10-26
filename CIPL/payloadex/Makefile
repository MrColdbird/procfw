PSPDEV=$(shell psp-config --pspsdk-path)
INCLUDES=-I $(PSPDEV)/include -I ../../Common

CFLAGS=$(INCLUDES) -W -Wall -G0 -Os -fno-pic -mno-abicalls -w -S

ifeq ($(PSP_MODEL),1)
TARGET = nand_payloadex_02g
else
TARGET = nand_payloadex_01g
endif

all:	a.bin

a.bin: 
	psp-gcc $(CFLAGS) -DPSP_MODEL=$(PSP_MODEL) -D_PSP_FW_VERSION=$(PSP_FW_VERSION) main.c -o main.s
	psp-as main.s -o main.o
	psp-ld -T linkfile.l main.o -o main.elf
	psp-strip -s main.elf
	psp-objcopy -O binary main.elf h.bin
	bin2c h.bin $(TARGET).h nand_payloadex
	@rm -rf *~ *.o *.elf *.bin *.s

clean:
	@rm -f nand_payloadex_01g.h nand_payloadex_02g.h
