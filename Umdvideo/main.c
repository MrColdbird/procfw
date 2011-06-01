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

#include <pspkernel.h>
#include <pspreg.h>
#include <stdio.h>
#include <string.h>
#include <systemctrl.h>
#include <systemctrl_se.h>
#include <pspsysmem_kernel.h>
#include <psprtc.h>
#include "utils.h"
#include "printk.h"
#include "libs.h"
#include "utils.h"
#include "systemctrl.h"
#include "systemctrl_se.h"
#include "systemctrl_private.h"

PSP_MODULE_INFO("PRO_Umdvideo_driver", 0x1000, 1, 1);

u32 psp_model;
u32 psp_fw_version;

int module_start(SceSize args, void* argp)
{
	psp_model = sceKernelGetModel();
	psp_fw_version = sceKernelDevkitVersion();
//	setup_patch_offset_table(psp_fw_version);
	printk_init("ms0:/umdvideo.txt");
	printk("Umdvideo started FW=0x%08X %02dg\n", psp_fw_version, psp_model+1);

	return 0;
}

int module_stop(SceSize args, void *argp)
{
	return 0;
}
