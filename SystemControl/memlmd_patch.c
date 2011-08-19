/*
 * This file is part of PRO CFW.

 * PRO CFW is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * PRO CFW is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PRO CFW. If not, see <http://www.gnu.org/licenses/ .
 */

#include <pspkernel.h>
#include <pspsysevent.h>
#include <pspiofilemgr.h>
#include <stdio.h>
#include <string.h>

#include "main.h"
#include "utils.h"
#include "systemctrl.h"
#include "printk.h"
#include "systemctrl_patch_offset.h"

// func_6360
static int (*memlmd_unsigner)(u8 *prx, u32 size, u32 use_polling);

// func_6238
// unk - pass 0
static int (*sceMemlmdInitializeScrambleKey)(u32 unk, void *hash_addr);

// func_6230
static int (*memlmd_decrypt)(u8 *prx, u32 size, u32 *newsize, u32 use_polling);

// sub_334
int _memlmd_unsigner(u8 *prx, u32 size, u32 use_polling)
{
	if (prx != NULL && *(u32*)prx == 0x5053507E) { // ~PSP
		u32 i;
		int result = 0;

		// 6.60 kernel modules use type 9 PRX... 0xd4~0x104 is zero padded
		for(i=0; i<0x30; ++i) {
			if (prx[i+0xd4]) {
				result = 1;
				break;
			}
		}

		if (result == 0) {
			return 0;
		}

		result = 0;

		// updater prx ... 0xB8~0x18 is zero padded
		for(i=0; i<0x18; ++i) {
			if (prx[i+0xB8]) {
				result = 1;
				break;
			}
		}

		if (result == 0) {
			return 0;
		}
	}

	return (*memlmd_unsigner)(prx, size, use_polling);
}

static u32 g_comp_tag[] = {
	0x28796DAA,
	0x7316308C,
	0x3EAD0AEE,
	0x8555ABF2,
	0xC6BA41D3,
	0x55668D96,
	0xC01DB15D,
};

static inline int is_comp_tag(u32 tag)
{
	int i;

	for(i=0; i<NELEMS(g_comp_tag); ++i) {
		if(g_comp_tag[i] == tag)
			return 1;
	}

	return 0;
}

static inline int is_prx_compressed(u8 *prx, u32 size)
{
	if (size < 0x160)
		return 0;

	if (*(u16*)(prx+0x150) == 0x8B1F) {
		if(is_comp_tag(*(u32*)(prx + 0x130))) {
			return 1;
		}
	}

	return 0;
}

// sub_1B38
static int _memlmd_decrypt(u8 *prx, u32 size, u32 *newsize, u32 use_polling)
{
	int ret;

	if (prx != NULL && newsize != NULL) {
		if (is_prx_compressed(prx, size)) {
			u32 compsize = *(u32*)(prx + 0xB0);

			memmove(prx, prx+0x150, compsize);
			*newsize = compsize;

			return 0;
		}
	}

	ret = (*memlmd_decrypt)(prx, size, newsize, use_polling);

	if (ret >= 0) {
		return ret;
	}

	// re-calculate key with xor seed
	if ((*sceMemlmdInitializeScrambleKey)(0, (void*)0xBFC00200) < 0)
		return ret;

	if (_memlmd_unsigner(prx, size, use_polling) < 0) {
		return ret;
	}

	return (*memlmd_decrypt)(prx, size, newsize, use_polling);
}

void patch_sceMemlmd(void)
{
	//find module
	SceModule2 * memlmd = (SceModule2*) sctrlKernelFindModuleByName("sceMemlmd");
	struct MemlmdPatch *patch;

	//32mb psp
	if(psp_model == 0)
	{
		patch = &g_offs->memlmd_patch_01g;
	}
	//64mb psps
	else
	{
		patch = &g_offs->memlmd_patch_other;
	}

	//This patch allow to load packed usermode module.
	_sh( 0xF005 , memlmd->text_addr + patch->memlmd_TagPatch );

	//patches
	memlmd_unsigner = (void*)memlmd->text_addr + patch->memlmd_unsigner; // inner function which unsigns a PRX module 
	sceMemlmdInitializeScrambleKey = (void*)memlmd->text_addr + patch->sceMemlmdInitializeScrambleKey;

	_sw(MAKE_CALL(_memlmd_unsigner), memlmd->text_addr + patch->memlmd_unsigner_call1); // the offset where memlmd_3F2AC9C6 call memlmd_unsigner
	_sw(MAKE_CALL(_memlmd_unsigner), memlmd->text_addr + patch->memlmd_unsigner_call2); // the offset where memlmd_97DA82BC call memlmd_unsigner
	_sw(MAKE_CALL(_memlmd_decrypt), memlmd->text_addr + patch->memlmd_decrypt_call1); // the offset where memlmd_E42AFE2E call memlmd_decrypt
	_sw(MAKE_CALL(_memlmd_decrypt), memlmd->text_addr + patch->memlmd_decrypt_call2); // the offset where memlmd_D56F8AEC call memlmd_decrypt

	memlmd_decrypt = (void*)memlmd->text_addr + patch->memlmd_decrypt; // inner function which decrypt a PRX module
}

static int (*mesgled_decrypt)(u32 *tag, u8 *key, u32 code, u8 *prx, u32 size, u32 *newsize, u32 use_polling, u8 *blacklist, u32 blacklistsize, u32 type, u8 *xor_key1, u8 *xor_key2);

static int _mesgled_decrypt(u32 *tag, u8 *key, u32 code, u8 *prx, u32 size, u32 *newsize, u32 use_polling, u8 *blacklist, u32 blacklistsize, u32 type, u8 *xor_key1, u8 *xor_key2)
{
	if (prx != NULL && newsize != NULL) {
		if (is_prx_compressed(prx, size)) {
			u32 compsize = *(u32*)(prx + 0xB0);

			memmove(prx, prx+0x150, compsize);
			*newsize = compsize;

			return 0;
		}
	}

	return (*mesgled_decrypt)(tag, key, code, prx, size, newsize, use_polling, blacklist, blacklistsize, type, xor_key1, xor_key2);
}

void patch_mesgled(SceModule* mod1)
{
	SceModule2 *mod = (SceModule2*) mod1;
	u32 text_addr, offset, i;
   
	text_addr = mod->text_addr;
	mesgled_decrypt = (void*)(text_addr + g_offs->mesgled_patch.mesgled_decrypt);

	for(i=0; i<NELEMS(g_offs->mesgled_patch.mesg_decrypt_call[psp_model]); ++i) {
		offset = g_offs->mesgled_patch.mesg_decrypt_call[psp_model][i];

		if(offset != 0xDEADBEEF) {
			_sw(MAKE_CALL(_mesgled_decrypt), text_addr + offset);
		}
	}
}
