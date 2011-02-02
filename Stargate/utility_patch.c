/*
 * This file is part of stargate.
 *
 * Copyright (C) 2008 hrimfaxi (outmatch@gmail.com)
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
#include "utils.h"
#include "printk.h"
#include "strsafe.h"
#include "libs.h"
#include "stargate.h"

typedef struct _UtilityHookEntry {
	char *libname;
	u32 nid;
	void *hook_addr;
} UtilityHookEntry;

static int (*_sceUtilityLoadModule)(int id);
static int (*_sceUtilityUnloadModule)(int id);

static int g_umd_mode = -1;

int myUtilityLoadModule(int id)
{
	int ret;

	ret = (*_sceUtilityLoadModule)(id);
	printk("%s: 0x%04X -> 0x%08X\n", __func__, id, ret);

	// fake NPDRM load in np9660
	if (ret == 0x80020139 && id == 0x500) {
		if (g_umd_mode == MODE_NP9660) {
			ret = 0;
			printk("%s: [FAKE] -> %d\n", __func__, ret);
		}
	}
	
	return ret;
}

int myUtilityUnloadModule(int id)
{
	int ret;

	ret = (*_sceUtilityUnloadModule)(id);
	printk("%s: 0x%04X -> 0x%08X\n", __func__, id, ret);

	// fake NPDRM unload in np9660
	if (ret == 0x80111103 && id == 0x500) {
		if (g_umd_mode == MODE_NP9660) {
			ret = 0;
			printk("%s: [FAKE] -> %d\n", __func__, ret);
		}
	}
	
	return ret;
}

static UtilityHookEntry g_utility_hook_map[] = {
	{ "sceUtility", 0x2A2B3DE0, &myUtilityLoadModule },
	{ "sceUtility", 0xE49BFE92, &myUtilityUnloadModule },
};

void patch_utility(SceModule *mod)
{
	int i;
	SEConfig config;

	sctrlSEGetConfig(&config);
	g_umd_mode = config.umdmode;

	_sceUtilityLoadModule = (void*)sctrlHENFindFunction("sceUtility_Driver", "sceUtility", 0x2A2B3DE0);
	_sceUtilityUnloadModule = (void*)sctrlHENFindFunction("sceUtility_Driver", "sceUtility", 0xE49BFE92);

	if(_sceUtilityLoadModule == NULL || _sceUtilityUnloadModule == NULL) {
		printk("%s: utility functions not found\n", __func__);

		return;
	}

	for(i=0; i<NELEMS(g_utility_hook_map); ++i) {
		hook_import_bynid(mod, g_utility_hook_map[i].libname, g_utility_hook_map[i].nid, g_utility_hook_map[i].hook_addr, 1);
	}
}
