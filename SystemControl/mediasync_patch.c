#include <pspsdk.h>
#include <pspsysmem_kernel.h>
#include <pspkernel.h>
#include <psputilsforkernel.h>
#include <pspsysevent.h>
#include <pspiofilemgr.h>
#include <stdio.h>
#include <string.h>
#include "printk.h"

void patch_sceMediaSync(u32 scemediasync_text_addr)
{
	// patch MsCheckMedia
	_sw(0x03E00008, scemediasync_text_addr+0x744);
	_sw(0x24020001, scemediasync_text_addr+0x748);

	// patch InitForKernel_2213275D check
	_sw(0, scemediasync_text_addr+0x00000D50);
}
