

all: 
	make -C payloadex PSP_MODEL=0
	make -C payloadex PSP_MODEL=1
	make -C mainbinex PSP_MODEL=0
	make -C mainbinex PSP_MODEL=1
	make -C combine

clean:
	make clean -C payloadex
	make clean -C mainbinex
	make clean -C combine
	rm ipl_block_01g.h ipl_block_large.h
