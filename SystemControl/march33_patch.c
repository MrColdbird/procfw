#include <pspsdk.h>
#include <pspsysmem_kernel.h>
#include <pspkernel.h>
#include <psputilsforkernel.h>
#include <pspsysevent.h>
#include <pspiofilemgr.h>
#include <stdio.h>
#include <string.h>
#include "printk.h"

void patch_pspMarch33_Driver(u32 march33_text_addr)
{
	_sw(0, march33_text_addr+0x000009E8);
}
