PSPDEV=$(shell psp-config --pspsdk-path)

#MODULE_PATH = ../../installer/modules
MODULE_PATH = ..

all:	a.bin

a.bin: 
	./combine.exe ipl_header_01g.bin ../mainbinex/patch_01g.bin ipl_block_01g.bin
	./combine.exe -l 0x24000 ipl_header_02g.bin ../mainbinex/patch_02g.bin ipl_block_02g.bin
	bin2c ipl_block_01g.bin ipl_block_01g.h ipl_block_01g
	bin2c ipl_block_02g.bin ipl_block_large.h ipl_block_large
	sed s/((aligned(16)))/((aligned(64)))/ ipl_block_large.h > $(MODULE_PATH)/ipl_block_large.h
	sed "/int/d" ipl_block_01g.h > $(MODULE_PATH)/ipl_block_01g.h

clean:
	rm ipl_block_01g.h ipl_block_large.h 
	rm ipl_block_01g.bin ipl_block_02g.bin