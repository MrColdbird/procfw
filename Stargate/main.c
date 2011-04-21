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

	// for MHP3rd: a 6.36 game
	hook_import_bynid((SceModule*)mod, "scePauth", 0x98B83B5D, myPauth_98B83B5D, 1);
	patch_utility((SceModule*)mod);
	patch_load_module((SceModule*)mod);

#ifdef CONFIG_620
	if(psp_fw_version == FW_620) {
		patch_for_620((SceModule*)mod);
	}
#endif

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
	myPauth_init();
	load_module_get_function();

	if(conf.usenodrm) {
		nodrm_init();
		nodrm_get_npdrm_functions(); // np9660 mode: npdrm already loaded
	}

#ifdef CONFIG_620
	if(psp_fw_version == FW_620) {
		get_620_function();
	}
#endif
	
	previous = sctrlHENSetStartModuleHandler(&stargate_module_chain);

#ifdef PSID_CHECK
	g_crash = confirm_usage_right();
#endif

	sync_cache();

	return 0;
}
