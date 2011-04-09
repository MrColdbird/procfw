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
#include "systemctrl_patch_offset.h"
#include "rebootex_conf.h"

PSP_MODULE_INFO("SystemControl", 0x3007, 1, 0);

u32 psp_model = 0;
u32 psp_fw_version = 0;

int module_start(SceSize args, void* argp)
{
#ifdef DEBUG
	fill_vram(0x000000ff);
#endif
	psp_fw_version = sctrlKernelDevkitVersion();
	setup_patch_offset_table(psp_fw_version);
	psp_model = sctrlKernelGetModel();
	load_rebootex_config();
	sctrlHENSetMemory(rebootex_conf.p2_size, rebootex_conf.p9_size);
	printk("High memory: p2/p9 %d/%d\n", g_p2_size, g_p9_size);
	
	printk_init(NULL);
	printk("SystemControl: model 0%dg FW 0x%08X\n", psp_model+1, psp_fw_version);
	printk("Compiled in %s %s\n", __DATE__, __TIME__);

	oe_mallocinit();
	syspatch_init();

	return 0;
}
