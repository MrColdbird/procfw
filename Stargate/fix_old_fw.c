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
#include <pspkernel.h>
#include <pspsysmem_kernel.h>
#include <pspthreadman_kernel.h>
#include <pspdebug.h>
#include <pspinit.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "systemctrl.h"
#include "systemctrl_se.h"
#include "libs.h"
#include "printk.h"
#include "stargate.h"
#include "stargate_patch_offset.h"

static int (*_scePowerSetClockFrequency)(int pllfreq, int cpufreq, int busfreq);
static int (*_KDebugForKernel_93F5D2A6)(int unk);

int _SysMemUserForUser_1B4217BC(u32 fw_version)
{
	int (*sceKernelSetCompiledSdkVersion)(u32 fw_version);

	sceKernelSetCompiledSdkVersion = (void*)sctrlHENFindFunction("sceSystemMemoryManager", "SysMemUserForUser", 0x7591C7DB);

	return (*sceKernelSetCompiledSdkVersion)(fw_version);
}

int myPower_469989AD(int pllfreq, int cpufreq, int busfreq)
{
	int ret;
	
	ret = (*_scePowerSetClockFrequency)(pllfreq, cpufreq, busfreq);
	printk("%s(%d/%d/%d): -> %d\n", __func__, pllfreq, cpufreq, busfreq, ret);

	return ret;
}

int myPower_A85880D0(void)
{
	int ret;

	ret = sceKernelGetModel(); // SysMemForKernel_458A70B5 in 6.36

	if (ret == 0) { // PSP-1000?
		ret = (*_KDebugForKernel_93F5D2A6)(11); // KDebugForKernel_C05553F6 in 6.36

		if (ret == 1) {
			ret = 1;
		} else {
			ret = 0;
		}
	} else {
		ret = 1;
	}

	printk("%s: -> 0x%08X\n", __func__, ret);

	return ret;
}

void patch_for_old_fw(SceModule *mod)
{
	if(psp_fw_version < FW_660) {
		hook_import_bynid(mod, "SysMemUserForUser", 0x358CA1BB, &_SysMemUserForUser_1B4217BC, 1);
	}

	if(psp_fw_version == FW_620) {
		hook_import_bynid(mod, "SysMemUserForUser", 0x1B4217BC, &_SysMemUserForUser_1B4217BC, 1);
		hook_import_bynid(mod, "scePower", 0x469989AD, &myPower_469989AD, 1);
		hook_import_bynid(mod, "scePower", 0xA85880D0, &myPower_A85880D0, 1);
	}
}

void get_functions_for_old_fw(void)
{
	_scePowerSetClockFrequency = (void*)sctrlHENFindFunction("scePower_Service", "scePower", 0x737486F2);
	_KDebugForKernel_93F5D2A6 = (void*)sctrlHENFindFunction("sceSystemMemoryManager", "KDebugForKernel", 0x93F5D2A6);
}
