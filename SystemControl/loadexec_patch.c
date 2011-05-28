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
#include "systemctrl_se.h"
#include "printk.h"
#include "../Rebootex_bin/rebootex.h"
#include "rebootex_conf.h"
#include "strsafe.h"
#include "systemctrl_patch_offset.h"

static int (*LoadReboot)(void * arg1, unsigned int arg2, void * arg3, unsigned int arg4) = NULL;
rebootex_config rebootex_conf;

//load reboot wrapper
static int load_reboot(void * arg1, unsigned int arg2, void * arg3, unsigned int arg4)
{
	//copy reboot extender
	memcpy((void*)REBOOTEX_START, rebootex, size_rebootex);
	//reset reboot flags
	memset((void*)REBOOTEX_CONFIG, 0, 0x100);

	//store custom partition size
	if(g_p2_size != 24 || g_p9_size != 24) {
		rebootex_conf.p2_size = g_p2_size;
		rebootex_conf.p9_size = g_p9_size;
	}

	memcpy((void*)REBOOTEX_CONFIG, &rebootex_conf, sizeof(rebootex_conf));
	memset((void*)REBOOTEX_CONFIG_ISO_PATH, 0, 256);
	strcpy_s((char*)REBOOTEX_CONFIG_ISO_PATH, 256, sctrlSEGetUmdFile());

	//forward
	return (*LoadReboot)(arg1, arg2, arg3, arg4);
}

void patch_sceLoadExec(void)
{
	SceModule2 * loadexec = (SceModule2*)sctrlKernelFindModuleByName("sceLoadExec");
	u32 text_addr;
	struct sceLoadExecPatch *patch;

	if (loadexec == NULL) {
		return;
	}

	text_addr = loadexec->text_addr;

	if(psp_model == PSP_GO) {
		patch = &g_offs->loadexec_patch_05g;
	} else {
		patch = &g_offs->loadexec_patch_other;
	}

	//replace LoadReboot function
	_sw(MAKE_CALL(load_reboot), loadexec->text_addr + patch->LoadRebootCall);

	//patch Rebootex position to 0x88FC0000
	_sw(0x3C0188FC, loadexec->text_addr + patch->RebootJump); // lui $at, 0x88FC

	//save LoadReboot function
	LoadReboot = (void*)(loadexec->text_addr + patch->LoadReboot);

	//allow user $k1 configs to call sceKernelLoadExecWithApiType
	_sw(0x1000000C, loadexec->text_addr + patch->sceKernelLoadExecWithApiTypeCheck1);
	//allow all user levels to call sceKernelLoadExecWithApiType
	_sw(NOP, loadexec->text_addr + patch->sceKernelLoadExecWithApiTypeCheck2);

	//allow all user levels to call sceKernelExitVSHVSH
	_sw(0x10000008, loadexec->text_addr + patch->sceKernelExitVSHVSHCheck1);
	_sw(NOP, loadexec->text_addr + patch->sceKernelExitVSHVSHCheck2);
}
