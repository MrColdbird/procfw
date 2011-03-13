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

struct SysmemPatch {
	u16 offset;
	u8 value;
};

static struct SysmemPatch g_sysmem_patch[] = {
	{ 0x9A2C, 0x1F }, // sceKernelSetCompiledSdkVersion
	{ 0x9B4C, 0x12 }, // SysMemUserForUser_342061E5
	{ 0x9BE4, 0x18 }, // SysMemUserForUser_315AD3A0
	{ 0x9C94, 0x1C }, // SysMemUserForUser_EBD5C3E6
	{ 0x9D68, 0x15 }, // SysMemUserForUser_057E7380
	{ 0x9E0C, 0x15 }, // SysMemUserForUser_91DE343C
	{ 0x9EB0, 0x12 }, // SysMemUserForUser_7893F79A
	{ 0x9F48, 0x18 }, // SysMemUserForUser_35669D4C
	{ 0x9FF8, 0x12 }, // SysMemUserForUser_1B4217BC
};

void patch_sceSystemMemoryManager(void)
{
	SceModule2 *mod;
	int i;

	mod = (SceModule2*)sceKernelFindModuleByName("sceSystemMemoryManager");

	if(mod == NULL)
		return;

	// allow invalid complied sdk version
	for(i=0; i<NELEMS(g_sysmem_patch); ++i) {
		_sw(g_sysmem_patch[i].value | 0x10000000, mod->text_addr+g_sysmem_patch[i].offset);
	}
}
