#include <pspsdk.h>
#include <pspsysmem_kernel.h>
#include <pspkernel.h>
#include <psputilsforkernel.h>
#include <psppower.h>
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "utils.h"
#include "systemctrl.h"
#include "printk.h"
#include "libs.h"
#include "rebootex_conf.h"

static const int g_cpu_list[]={20, 75, 100, 133, 222, 266, 300, 333};
static u32 g_scePowerSetClockFrequency_orig;

static inline u32 find_power_function(u32 nid)
{
	return sctrlHENFindFunction("scePower_Service", "scePower", nid);
}

static inline void nullify_function(u32 addr)
{
	if (addr == 0) {
		printk("Warning: nullify_function got a NULL function\n");
		return;
	}

	_sw(0x03E00008, addr);
	_sw(0x00001021, addr + 4);
}

void SetSpeed(int cpuspd, int busspd)
{
	int (*_scePowerSetClockFrequency)(int, int, int);
	u32 fp;
	int i;

	for(i=0; i<NELEMS(g_cpu_list); ++i) {
		if(cpuspd == g_cpu_list[i])
			break;
	}

	if(i >= NELEMS(g_cpu_list)) {
		return;
	}

	fp = find_power_function(0x737486F2); // scePowerSetClockFrequency
	g_scePowerSetClockFrequency_orig = fp;
	_scePowerSetClockFrequency = (void*)fp;
	_scePowerSetClockFrequency(cpuspd, cpuspd, busspd);

	if (sceKernelInitKeyConfig() == PSP_INIT_KEYCONFIG_VSH)
		return;

	// fp = find_power_function(0x737486F2); // scePowerSetClockFrequency
	nullify_function(fp);
	fp = find_power_function(0x545A7F3C); // scePowerSetClockFrequency
	nullify_function(fp);
	fp = find_power_function(0xB8D7B3FB); // scePowerSetBusClockFrequency
	nullify_function(fp);
	fp = find_power_function(0x843FBF43); // scePowerSetCpuClockFrequency
	nullify_function(fp);
	fp = find_power_function(0xEBD177D6);
	nullify_function(fp);
	fp = find_power_function(0x469989AD); // 6.3x new function
	nullify_function(fp);

	sync_cache();
}

void sctrlHENSetSpeed(int cpuspd, int busspd)
{
	int (*_scePowerSetClockFrequency)(int, int, int);

	g_scePowerSetClockFrequency_orig = find_power_function(0x545A7F3C); /* scePowerSetClockFrequency */
	_scePowerSetClockFrequency = (void *) g_scePowerSetClockFrequency_orig;
	_scePowerSetClockFrequency(cpuspd, cpuspd, busspd);
}
