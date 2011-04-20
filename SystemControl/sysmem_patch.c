#include <pspsdk.h>
#include <pspsysmem_kernel.h>
#include <pspkernel.h>
#include <psputilsforkernel.h>
#include <pspsysevent.h>
#include <pspiofilemgr.h>
#include <stdio.h>
#include <string.h>
#include "printk.h"
#include "utils.h"
#include "main.h"
#include "systemctrl_patch_offset.h"

void patch_sceSystemMemoryManager(void)
{
	SceModule2 *mod;
	int i;

	mod = (SceModule2*)sctrlKernelFindModuleByName("sceSystemMemoryManager");

	if(mod == NULL)
		return;

	// allow invalid complied sdk version
	for(i=0; i<NELEMS(g_offs->sysmemforuser_patch); ++i) {
		if(g_offs->sysmemforuser_patch[i].offset == 0xFFFF)
			continue;

		_sw(g_offs->sysmemforuser_patch[i].value | 0x10000000, mod->text_addr+g_offs->sysmemforuser_patch[i].offset);
	}
}
