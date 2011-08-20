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
#include "systemctrl_private.h"
#include "kubridge.h"
#include "utils.h"
#include "printk.h"
#include "strsafe.h"
#include "libs.h"
#include "stargate.h"
#include "stargate_patch_offset.h"

#ifdef PSID_CHECK
#include "psid_check.h"
#endif

PSP_MODULE_INFO("stargate", 0x1007, 1, 0);
PSP_MAIN_THREAD_ATTR(0);

static STMOD_HANDLER previous;
SEConfig conf;
u32 psp_model;
u32 psp_fw_version;

#ifdef PSID_CHECK
static int g_crash = 0;
#endif

#define MAX_MODULE_NUMBER 256

static void fix_weak_imports(void)
{
	SceUID *modids;
	int ret;
	int i, count;
	int k1;

	k1 = pspSdkGetK1();
	pspSdkSetK1(0);
	modids = oe_malloc(MAX_MODULE_NUMBER * sizeof(SceUID));

	if(modids == NULL) {
		printk("%s: allocating modids failed\n", __func__);
		goto exit;
	}

	memset(modids, 0, MAX_MODULE_NUMBER * sizeof(SceUID));
	ret = sceKernelGetModuleIdList(modids, MAX_MODULE_NUMBER * sizeof(SceUID), &count);
	
	if (ret < 0) {
		printk("%s: sceKernelGetThreadmanIdList -> 0x%08x\n", __func__, ret);
		oe_free(modids);
		goto exit;
	}

	for(i=0; i<count; ++i) {
		SceModule2 *pMod;

		pMod = (SceModule2*)sceKernelFindModuleByUID(modids[i]);

		if (pMod != NULL && (pMod->attribute & 0x1000) == 0) {
			patch_drm_imports((SceModule*)pMod);
		}
	}

	oe_free(modids);
exit:
	pspSdkSetK1(k1);
}

static int stargate_module_chain(SceModule2 *mod)
{
	if (previous)
		(*previous)(mod);

#ifdef CONFIG_620
	if(psp_fw_version == FW_620) {
		// for MHP3rd: a 6.36 game
		hook_import_bynid((SceModule*)mod, "scePauth", 0x98B83B5D, myPauth_98B83B5D, 1);
	}
#endif

#ifdef CONFIG_635
	if(psp_fw_version == FW_635) {
		// for MHP3rd: a 6.36 game
		hook_import_bynid((SceModule*)mod, "scePauth", 0x98B83B5D, myPauth_98B83B5D, 1);
	}
#endif

	patch_utility((SceModule*)mod);
	patch_load_module((SceModule*)mod);
	patch_for_old_fw((SceModule*)mod);

	// m33 mode: until npdrm loaded
	if(conf.usenodrm) {
		if(0 == strcmp(mod->modname, "scePspNpDrm_Driver")) {
			int ret;

			fix_weak_imports();
			ret = nodrm_get_npdrm_functions();

#ifdef DEBUG
			if (ret < 0) {
				printk("%s: nodrm_get_npdrm_functions -> %d\n", __func__, ret);
			}
#endif
		}

		patch_drm_imports((SceModule*)mod);
	}

	if(conf.noanalog) {
		patch_analog_imports((SceModule*)mod);
	}

	if(conf.hide_cfw_dirs) {
		hide_cfw_folder((SceModule*)mod);
	}

	module_blacklist((SceModule*)mod);

#ifdef PSID_CHECK
	if(g_crash) {
		crash_me();
	}
#endif
	
	return 0;
}

static inline int is_homebrews_runlevel(void)
{
	int apitype;

	apitype = sceKernelInitApitype();

	if(apitype == 0x152 || apitype == 0x141) {
		return 1;
	}

	return 0;
}

// This fixes the mysterious idol master SP genuine check. 
static void patch_sceLoadExec(void)
{
	sctrlPatchModule("sceLoadExec", NOP, g_offs->LoadExecForUser_362A956B_fix);
}

static int start_module_handler(int modid, SceSize argsize, void *argp, int *modstatus, SceKernelSMOption *opt)
{
	SceModule *mod;
	PspModuleImport *import;
	int patched = 0;

	mod = sceKernelFindModuleByUID(modid);

	if(mod == NULL) {
		return -1;
	}

	import = find_import_lib(mod, "Kernel_LibrarZ");

	if(import != NULL) {
		strcpy((char*)import->name, "Kernel_Library");
		patched = 1;
	}

	import = find_import_lib(mod, "Kernel_Librar0");

	if(import != NULL) {
		strcpy((char*)import->name, "Kernel_Library");
		patched = 1;
	}

	import = find_import_lib(mod, "sceUtilitO");

	if(import != NULL) {
		strcpy((char*)import->name, "sceUtility");
		patched = 1;
	}

	if(patched) {
		printk("%s: patched imported library name\n", __func__);
	}

	return -1;
}

int module_start(SceSize args, void *argp)
{
	if(is_homebrews_runlevel()) {
		return 1;
	}

	psp_model = sceKernelGetModel();
	psp_fw_version = sceKernelDevkitVersion();
	setup_patch_offset_table(psp_fw_version);
	printk_init("ms0:/log_stargate.txt");
	printk("stargate started\n");
	sctrlSEGetConfig(&conf);
	patch_sceMesgLed();
	patch_sceLoadExec();

	if(conf.chn_iso) {
		patch_IsoDrivers();
	}

#ifdef CONFIG_620
	if(psp_fw_version == FW_620) {
		myPauth_init();
	}
#endif

#ifdef CONFIG_635
	if(psp_fw_version == FW_635) {
		myPauth_init();
	}
#endif

	load_module_get_function();

	if(conf.usenodrm) {
		nodrm_init();
		nodrm_get_npdrm_functions(); // np9660 mode: npdrm already loaded
	}

	get_functions_for_old_fw();
	previous = sctrlHENSetStartModuleHandler(&stargate_module_chain);
	sctrlSetCustomStartModule(&start_module_handler);

#ifdef PSID_CHECK
	g_crash = confirm_usage_right();
#endif

	sync_cache();

	return 0;
}
