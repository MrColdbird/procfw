#include <pspkernel.h>
#include <pspsysevent.h>
#include <pspiofilemgr.h>
#include <stdio.h>
#include <string.h>

#include "main.h"
#include "systemctrl.h"
#include "printk.h"
#include "utils.h"
#include "kubridge.h"
#include "libs.h"

SceUID g_mod_start_thid = -1;
SceModule2 *g_mod_start = NULL;
int (*g_on_module_start)(SceModule2*) = NULL;

// sceKernelFindModuleByAddress most likely?
extern SceModule2 *LoadCoreForKernel_312CA47E(void *addr);

static int myKernelStartThread(SceUID thid, SceSize arglen, void *argp)
{
	int ret;

	if (g_mod_start_thid != -1 && thid == g_mod_start_thid) {
		if (g_on_module_start != NULL && g_mod_start != NULL) {
			(*g_on_module_start)(g_mod_start);
		}

		g_mod_start_thid = -1;
		g_mod_start = NULL;
	}

	ret = sceKernelStartThread(thid, arglen, argp);

	return ret;
}

static int myKernelCreateThread(const char *name, void *entry, int pri, int stack, u32 attr, void *opt)
{
	int ret;

	ret = sceKernelCreateThread(name, entry, pri, stack, attr, opt);

	if (ret >= 0 && !strcmp(name, "SceModmgrStart")) {
		g_mod_start = LoadCoreForKernel_312CA47E(entry);
		g_mod_start_thid = ret;
	}

	return ret;
}

void setup_module_handler(void)
{
	SceModule2 * mod;
   
	mod = (SceModule2 *)sceKernelFindModuleByName("sceModuleManager");

	if (mod == NULL)
		return;

	hook_import_bynid((SceModule*)mod, "ThreadManForKernel", 0xF475845D, myKernelStartThread, 0);
	hook_import_bynid((SceModule*)mod, "ThreadManForKernel", 0x446D8DE6, myKernelCreateThread, 0);
}	
