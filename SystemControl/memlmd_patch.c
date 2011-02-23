#include <pspkernel.h>
#include <pspsysevent.h>
#include <pspiofilemgr.h>
#include <stdio.h>
#include <string.h>

#include "main.h"
#include "utils.h"
#include "systemctrl.h"
#include "printk.h"

// func_6360
static int (*memlmd_unsigner)(u8 *prx, u32 size, u32 use_polling);

// func_6238
// unk - pass 0
static int (*memlmd_8450109F)(u32 unk, void *hash_addr);

// func_6230
static int (*memlmd_decrypt)(u8 *prx, u32 size, u32 *newsize, u32 use_polling);

// sub_334
int _memlmd_unsigner(u8 *prx, u32 size, u32 use_polling)
{
	if (prx != NULL && *(u32*)prx == 0x5053507E) { // ~PSP
		u32 i;
		int result = 0;

		// 6.3x kernel modules use type 3 PRX... 0xd4~0x10C is zero padded
		for(i=0; i<0x38; ++i) {
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

static inline int is_prx_compressed(u8 *prx, u32 size)
{
	if (size < 0x160)
		return 0;

	if (*(u16*)(prx+0x150) == 0x8B1F) {
		if (*(u16*)(prx+0x1E) == 0x0000 || *(u16*)(prx+0x3E) == 0x0000) {
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
	if ((*memlmd_8450109F)(0, (void*)0xBFC00200) < 0)
		return ret;

	if (_memlmd_unsigner(prx, size, use_polling) < 0) {
		return ret;
	}

	return (*memlmd_decrypt)(prx, size, newsize, use_polling);
}

void patch_sceMemlmd(void)
{
	//find module
	SceModule2 * memlmd = (SceModule2*) sceKernelFindModuleByName("sceMemlmd");

	//patch offsets
	unsigned int patches[6];

	//32mb psp
	if(psp_model == 0)
	{
		patches[0] = 0x0F88;
		patches[1] = 0x11D0;
		patches[2] = 0x1150;
		patches[3] = 0x11A4;
		patches[4] = 0x0E88;
		patches[5] = 0x0EEC;
	}
	//64mb psps
	else
	{
		patches[0] = 0x1078;
		patches[1] = 0x12C0;
		patches[2] = 0x1240;
		patches[3] = 0x1294;
		patches[4] = 0x0F78;
		patches[5] = 0x0FDC;
	}

	//patches
	memlmd_unsigner = (void*)memlmd->text_addr + patches[0]; // inner function which unsigns a PRX module 
	memlmd_8450109F = (void*)memlmd->text_addr + patches[1]; // memlmd_8450109F: the 0xBFC00200 xor key setup function

	_sw(MAKE_CALL(_memlmd_unsigner), memlmd->text_addr + patches[2]); // the offset where memlmd_3F2AC9C6 call memlmd_unsigner
	_sw(MAKE_CALL(_memlmd_unsigner), memlmd->text_addr + patches[3]); // the offset where memlmd_97DA82BC call memlmd_unsigner
	_sw(MAKE_CALL(_memlmd_decrypt), memlmd->text_addr + patches[4]); // the offset where memlmd_E42AFE2E call memlmd_decrypt
	_sw(MAKE_CALL(_memlmd_decrypt), memlmd->text_addr + patches[5]); // the offset where memlmd_D56F8AEC call memlmd_decrypt

	memlmd_decrypt = (void*)memlmd->text_addr + 0x0134; // inner function which decrypt a PRX module
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
	u32 text_addr;
	u32 offsets[] = {
		0x00001D20, // 01g
		0x00001DD0, // 02g
		0x00001E60, // 03g
		0x00001E60, // 04g
		0x00001EF8, // 05g
		0xDEADBEEF, // 06g
		0x00001E60, // 07g
		0xDEADBEEF, // 08g
		0x00001E60, // 09g
	};
   
	text_addr = mod->text_addr;
	mesgled_decrypt = (void*)(text_addr+0xE0);

	if (psp_model < NELEMS(offsets) && offsets[psp_model] != 0xDEADBEEF) {
		_sw(MAKE_CALL(_mesgled_decrypt), text_addr+offsets[psp_model]);
	}
}
