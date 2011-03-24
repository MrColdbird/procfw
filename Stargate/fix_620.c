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

#ifdef CONFIG_620
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

void patch_for_620(SceModule *mod)
{
	hook_import_bynid(mod, "SysMemUserForUser", 0x1B4217BC, &_SysMemUserForUser_1B4217BC, 1);
	hook_import_bynid(mod, "scePower", 0x469989AD, &myPower_469989AD, 1);
	hook_import_bynid(mod, "scePower", 0xA85880D0, &myPower_A85880D0, 1);
}

void get_620_function(void)
{
	_scePowerSetClockFrequency = (void*)sctrlHENFindFunction("scePower_Service", "scePower", 0x737486F2);
	_KDebugForKernel_93F5D2A6 = (void*)sctrlHENFindFunction("sceSystemMemoryManager", "KDebugForKernel", 0x93F5D2A6);
}
#else
int _SysMemUserForUser_1B4217BC(u32 fw_version)
{
	return 0;
}

int myPower_469989AD(int pllfreq, int cpufreq, int busfreq)
{
	return 0;
}

int myPower_A85880D0(void)
{
	return 0;
}
#endif
