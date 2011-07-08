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

#ifdef CONFIG_620
static int g_mp3_loaded = 0;
#endif

int myUtilityLoadModule(int id)
{
	int ret;

#ifdef CONFIG_620
	// In FW 6.20, libmp3.prx load/unload code is buggy
	// It cannot load/unload again once got loaded
	// Doing so will result in user memory corrupted (in DBTVS)
	if(psp_fw_version == FW_620 && id == 0x304 && g_mp3_loaded) {
		ret = 0;
		printk("%s: [FAKE] 0x%04X -> %d\n", __func__, id, ret);

		return ret;
	}
#endif
	
	ret = (*_sceUtilityLoadModule)(id);

#ifdef CONFIG_620
	if(psp_fw_version == FW_620 && id == 0x304 && ret == 0) {
		g_mp3_loaded = 1;
	}
#endif
	
	printk("%s: 0x%04X -> 0x%08X\n", __func__, id, ret);

	// fake NPDRM load in np9660
	if (ret == 0x80020139 && id == 0x500) {
		ret = 0;
		printk("%s: [FAKE] -> %d\n", __func__, ret);
	}
	
	return ret;
}

int myUtilityUnloadModule(int id)
{
	int ret;

#ifdef CONFIG_620
	if(psp_fw_version == FW_620 && id == 0x304 && g_mp3_loaded) {
		ret = 0;
		printk("%s: [FAKE] 0x%04X -> %d\n", __func__, id, ret);

		return ret;
	}
#endif

	ret = (*_sceUtilityUnloadModule)(id);
	printk("%s: 0x%04X -> 0x%08X\n", __func__, id, ret);

	// fake NPDRM unload in np9660
	if (ret == 0x80111103 && id == 0x500) {
		ret = 0;
		printk("%s: [FAKE] -> %d\n", __func__, ret);
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
