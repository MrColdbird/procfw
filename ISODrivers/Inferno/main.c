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
#include "inferno.h"

PSP_MODULE_INFO("PRO_Inferno_Driver", 0x1000, 1, 1);

u32 psp_model;
u32 psp_fw_version;

extern int sceKernelSetQTGP3(void *unk0);
extern char *GetUmdFile();

// 00002790
const char *g_iso_fn = NULL;

// 0x00002248
u8 g_umddata[16] = {
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};

// 00000090
int setup_umd_device(void)
{
	int ret;

	g_iso_fn = GetUmdFile();
	ret = sceIoAddDrv(&g_iodrv);

	if(ret < 0) {
		return ret;
	}

	sceKernelSetQTGP3(g_umddata);
	ret = 0;

	return ret;
}

// 00001514
int init_march33(void)
{
	g_drive_status = 50;
	g_umd_cbid = -1;
	g_umd_error_status = 0;
	g_drive_status_evf = sceKernelCreateEventFlag("SceMediaManUser", 0x201, 0, NULL);

	return MIN(g_drive_status_evf, 0);
}

// 0x00000000
int module_start(SceSize args, void* argp)
{
	int ret;

	psp_model = sceKernelGetModel();
	psp_fw_version = sceKernelDevkitVersion();
	setup_patch_offset_table(psp_fw_version);
	printk_init("ms0:/inferno.txt");
	printk("March33 reversed started FW=0x%08X %02dg\n", psp_fw_version, psp_model+1);

	ret = setup_umd_device();

	if(ret < 0) {
		return ret;
	}

	ret = init_march33();

	return MIN(ret, 0);
}

// 0x0000006C
int module_stop(SceSize args, void *argp)
{
	sceIoDelDrv("umd");
	sceKernelDeleteEventFlag(g_drive_status_evf);

	return 0;
}
