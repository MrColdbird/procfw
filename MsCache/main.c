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
#include "printk.h"
#include "systemctrl.h"

PSP_MODULE_INFO("MsCacheCtrl", 0x1007, 1, 0);

u32 psp_model = 0;
u32 psp_fw_version = 0;

int module_start(SceSize args, void* argp)
{
	psp_fw_version = sceKernelDevkitVersion();
	psp_model = sceKernelGetModel();

	printk_init("ms0:/mscache.txt");
	msstor_init();

	return 0;
}
