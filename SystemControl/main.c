/*
 * This file is part of PRO CFW.

 * PRO CFW is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * PRO CFW is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PRO CFW. If not, see <http://www.gnu.org/licenses/ .
 */

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

	printk_init(NULL);
	printk("SystemControl: model 0%dg FW 0x%08X\n", (int)psp_model+1, (uint)psp_fw_version);
	printk("Compiled in %s %s\n", __DATE__, __TIME__);

	oe_mallocinit();
	syspatch_init();

	return 0;
}
