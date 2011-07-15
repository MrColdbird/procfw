#include <pspkernel.h>
#include <pspreg.h>
#include <stdio.h>
#include <string.h>
#include <psprtc.h>
#include <psputilsforkernel.h>
#include "systemctrl.h"

PSP_MODULE_INFO("PermanentKmodule", 0x1000, 1, 0);

#define MAKE_CALL(f) (0x0C000000 | (((u32)(f) >> 2) & 0x03ffffff)) 

static u8 vshmain_kirkheader[272];
static int vshmain_kirkheader_copied = 0;
static int (*sceUtilsBufferCopyWithRange)(u32 a0, u32 a1, u32 a2, u32 a3, u32 t0);

static int _sceUtilsBufferCopyWithRange(u32 a0, u32 a1, u32 a2, u32 a3, u32 t0)
{
	int ret;

	if(!vshmain_kirkheader_copied) {
		memcpy(vshmain_kirkheader, (void*)a2, sizeof(vshmain_kirkheader));
		vshmain_kirkheader_copied = 1;
	}

	ret = (*sceUtilsBufferCopyWithRange)(a0, a1, a2, a3, t0);

	return ret;
}

/*
	0x000009F0: 0x2687FFC0 '...&' - addiu      $a3, $s4, -64
	0x000009F4: 0x02802821 '!(..' - move       $a1, $s4
	0x000009F8: 0x02002021 '! ..' - move       $a0, $s0
	0x000009FC: 0x0C0017B6 '....' - jal        sceUtilsBufferCopyWithRange
	0x00000A00: 0x24080001 '...$' - li         $t0, 1
*/
void patch_mesgled(SceModule* mod1)
{
	SceModule2 *mod = (SceModule2*) mod1;
	u32 text_addr;

	text_addr = mod->text_addr;
	sceUtilsBufferCopyWithRange = (void*)(sctrlHENFindFunction("sceMemlmd", "semaphore", 0x4C537C72));
	_sw(MAKE_CALL(_sceUtilsBufferCopyWithRange), text_addr+0x000009FC);
}

void sync_cache(void)
{
	sceKernelIcacheInvalidateAll();
	sceKernelDcacheWritebackInvalidateAll();
}

int get_kirk_header(void *buf)
{
	if(vshmain_kirkheader_copied) {
		memcpy(buf, vshmain_kirkheader, sizeof(vshmain_kirkheader));

		return 0;
	}

	return -1;
}

int module_start(SceSize argc, void *argp)
{
	patch_mesgled(sceKernelFindModuleByName("sceMesgLed"));
	sync_cache();

	return 0;
}
