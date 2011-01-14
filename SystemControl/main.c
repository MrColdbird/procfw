#include <pspsdk.h>
#include <pspsysmem_kernel.h>
#include <pspkernel.h>
#include <psputilsforkernel.h>
#include <pspsysevent.h>
#include <pspiofilemgr.h>
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "utils.h"
#include "systemctrl.h"
#include "printk.h"

PSP_MODULE_INFO("SystemControl", 0x3007, 1, 0);

u32 psp_model = 0;
u32 psp_fw_version = 0;

int module_start(SceSize args, void* argp)
{
	fill_vram(0x000000ff);
	sctrlHENSetMemory(*(u32*)(0x88FB0008), *(u32*)(0x88FB000C));
	psp_model = sceKernelGetModel();
	psp_fw_version = sceKernelDevkitVersion();
	
	printk_init(NULL);
	printk("SystemControl: model 0%dg FW 0x%08X\n", psp_model+1, psp_fw_version);
	printk("Compiled in %s %s\n", __DATE__, __TIME__);

	load_rebootex_config();
	oe_mallocinit();
	syspatch_init();

	return 0;
}
