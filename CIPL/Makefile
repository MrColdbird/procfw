
PSP_FW_VERSION = 0

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

all: 
	make -C payloadex PSP_FW_VERSION=$(PSP_FW_VERSION) PSP_MODEL=0
	make -C payloadex PSP_FW_VERSION=$(PSP_FW_VERSION) PSP_MODEL=1
	make -C mainbinex PSP_FW_VERSION=$(PSP_FW_VERSION) PSP_MODEL=0
	make -C mainbinex PSP_FW_VERSION=$(PSP_FW_VERSION) PSP_MODEL=1
	make -C combine

clean:
	make clean -C payloadex
	make clean -C mainbinex
	make clean -C combine
	@rm -f ipl_block_01g.h ipl_block_large.h
