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
	// MsCheckMedia: mediasync used it to check EBOOT.PBP
	// Let it return 1 always
	_sw(0x03E00008, scemediasync_text_addr+0x744);
	_sw(0x24020001, scemediasync_text_addr+0x748);

	// patch DiscCheckMedia
	_sw(0x1000001D, scemediasync_text_addr+0x3C4);
	_sw(0x1000001D, scemediasync_text_addr+0xDC8);
}
