/*
 * This file is part of stargate.
 *
 * Copyright (C) 2008 hrimfaxi (outmatch@gmail.com)
 */

#include <pspsdk.h>
#include <pspkernel.h>
#include <pspsysmem_kernel.h>
#include <pspthreadman_kernel.h>
#include <pspcrypt.h>
#include <pspdebug.h>
#include <pspinit.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "pspmodulemgr_kernel.h"
#include "psputilsforkernel.h"
#include "systemctrl.h"
#include "kubridge.h"
#include "utils.h"
#include "printk.h"

// 6.36 game key
static u8 g_key_d91681f0[16] = {
	0x52, 0xB6, 0x36, 0x6C, 0x8C, 0x46, 0x7F, 0x7A, 
	0xCC, 0x11, 0x62, 0x99, 0xC1, 0x99, 0xBE, 0x98, 
};

typedef struct {
	u32 tag;
	u8 *key;
	u32 code;
	u32 type;
} GameCipher;

static GameCipher g_cipher[] = {
	{ 0xd91681f0, g_key_d91681f0, 0x5d, 6},
};

static GameCipher *get_game_cipher(u32 tag)
{
	int i;

	for(i=0; i<NELEMS(g_cipher); ++i) {
		if (g_cipher[i].tag == tag)
			return &g_cipher[i];
	}

	return NULL;
}

int (*mesgled_decrypt)(u32 *tag, u8 *key, u32 code, u8 *prx, u32 size, u32 *newsize, u32 use_polling, u8 *blacklist, u32 blacklistsize, u32 type, u8 *xor_key1, u8 *xor_key2) = NULL;

static int _mesgled_decrypt(u32 *tag, u8 *key, u32 code, u8 *prx, u32 size, u32 *newsize, u32 use_polling, u8 *blacklist, u32 blacklistsize, u32 type, u8 *xor_key1, u8 *xor_key2)
{
	int ret;
	u32 keytag;
	GameCipher *cipher = NULL;

	// try default decrypt
	ret = (*mesgled_decrypt)(tag, key, code, prx, size, newsize, use_polling, blacklist, blacklistsize, type, xor_key1, xor_key2);

	if (ret == 0) {
		return 0;
	}
	
	// try game key
	keytag = *((u32*)(prx+0xd0));
	cipher = get_game_cipher(keytag);

	if (cipher != NULL) {
		ret = (*mesgled_decrypt)(&cipher->tag, cipher->key, cipher->code, prx, size, newsize, use_polling, blacklist, blacklistsize, cipher->type, NULL, NULL);
		
		if (ret == 0) {
			printk("%s: tag=0x%08X type=%d decrypt OK\n", __func__, cipher->tag, cipher->type);
			fill_vram(0x000000ff);

			return ret;
		}
	}

	return -301;
}

void patch_sceMesgLed()
{
	SceModule2 *mod;
	u32 psp_model;
	u32 intr, text_addr;

	mod = (SceModule2*)sceKernelFindModuleByName("sceMesgLed");

	if (mod == NULL) {
		return;
	}

	psp_model = sceKernelGetModel();
	intr = MAKE_CALL(_mesgled_decrypt);
	text_addr = mod->text_addr;
	mesgled_decrypt = (void*)(text_addr+0xE0);

	if (psp_model == PSP_GO) {
		_sw(intr, text_addr+0x3614);
		_sw(intr, text_addr+0x38AC);
	} else if (psp_model == PSP_3000 || psp_model == PSP_4000 || psp_model == PSP_7000 || psp_model == PSP_9000) {
		_sw(intr, text_addr+0x32A8);
		_sw(intr, text_addr+0x3540);
	} else if (psp_model == PSP_2000) {
		_sw(intr, text_addr+0x2F08);
		_sw(intr, text_addr+0x31A0);
	} else if (psp_model == PSP_1000) {
		_sw(intr, text_addr+0x2B28); // sceMesgLed_driver_CA17E61A
		_sw(intr, text_addr+0x2DC0); // sceMesgLed_driver_E9BF25D2
	}

	sync_cache();
}
