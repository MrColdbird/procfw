PSPSDK=$(shell psp-config --pspsdk-path)
INCLUDES=-I $(PSPSDK)/include -I ../Common

all: a.bin

a.bin:
	psp-gcc $(INCLUDES) -W -Wall -Os -G0 -fno-pic -fno-toplevel-reorder -mno-abicalls -w -S main.c -o main.s
	psp-as main.s -o main.o
	psp-ld -T linkfile.l main.o -o main.elf
	psp-strip -s main.elf
	psp-objcopy -O binary main.elf rebootex.bin
	bin2c rebootex.bin rebootex.h rebootex

clean:
	rm -rf *~ *.s *.o *.elf rebootex.bin rebootex.h
