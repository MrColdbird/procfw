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
#include "kubridge.h"
#include "utils.h"
#include "printk.h"
#include "strsafe.h"
#include "libs.h"
#include "stargate.h"

PSP_MODULE_INFO("stargate", 0x1007, 1, 0);
PSP_MAIN_THREAD_ATTR(0);

extern int nodrm_init(void);
extern void patch_drm_imports(SceModule *mod);

static STMOD_HANDLER previous;

static int stargate_module_chain(SceModule2 *mod)
{
	// for MHP3rd: a 6.36 game
	hook_import_bynid((SceModule*)mod, "scePauth", 0x98B83B5D, myPauth_98B83B5D, 1);
	patch_drm_imports((SceModule*)mod);
	
	if (previous)
		return (*previous)(mod);

	return 0;
}

int module_start(SceSize args, void *argp)
{
	printk_init("ms0:/log_stargate.txt");
	printk("stargate started\n");
	patch_sceMesgLed();
	myPauth_init();
	nodrm_init();
	previous = sctrlHENSetStartModuleHandler(&stargate_module_chain);

	return 0;
}
