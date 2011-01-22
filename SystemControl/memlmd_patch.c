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

		for(i=0; i<0x18; ++i) {
			if (prx[i+0xB8]) {
				result = 1;
				break;
			}
		}

		if (result == 0) {
			return 0;
		}

		result = 0;
	}

	return (*memlmd_unsigner)(prx, size, use_polling);
}

// sub_1B38
static int _memlmd_decrypt(u8 *prx, u32 size, u32 *newsize, u32 use_polling)
{
	int ret;

	if (prx != NULL && newsize != NULL) {
		if (*(u32*)(prx+0x130) == 0xC6BA41D3 || *(u32*)(prx+0x130) == 0x55778D96) { // Gziped PRX signature
			if (prx[0x150] == 0x1F && prx[151] == 0x8B) { // Gzip magic
				u32 compsize = *(u32*)(prx + 0xB0);

				memmove(prx, prx+0x150, compsize);
				*newsize = compsize;

				return 0;
			}
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
	unsigned int patches[7];

	//32mb psp
	if(psp_model == 0)
	{
		patches[0] = 0x0F88;
		patches[1] = 0x11D0;
		patches[2] = 0x1150;
		patches[3] = 0x11A4;
		patches[4] = 0x0E88;
		patches[5] = 0x0EEC;
		patches[6] = 0x0AFC;
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
		patches[6] = 0x0B74;
	}

	//patches
	memlmd_unsigner = (void*)memlmd->text_addr + patches[0]; // inner function which unsigns a PRX module 
	memlmd_8450109F = (void*)memlmd->text_addr + patches[1]; // memlmd_8450109F: the 0xBFC00200 xor key setup function

	_sw(MAKE_CALL(_memlmd_unsigner), memlmd->text_addr + patches[2]); // the offset where memlmd_3F2AC9C6 call memlmd_unsigner
	_sw(MAKE_CALL(_memlmd_unsigner), memlmd->text_addr + patches[3]); // the offset where memlmd_97DA82BC call memlmd_unsigner
	_sw(MAKE_CALL(_memlmd_decrypt), memlmd->text_addr + patches[4]); // the offset where memlmd_E42AFE2E call memlmd_decrypt
	_sw(MAKE_CALL(_memlmd_decrypt), memlmd->text_addr + patches[5]); // the offset where memlmd_D56F8AEC call memlmd_decrypt

	memlmd_decrypt = (void*)memlmd->text_addr + 0x0134; // inner function which decrypt a PRX module
	_sw(0, memlmd->text_addr + patches[6]); // the check prohibits old updater prx
}
