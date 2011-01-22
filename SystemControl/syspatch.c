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
#include "printk.h"
#include "libs.h"
#include "rebootex_conf.h"

static STMOD_HANDLER previous;

static int syspatch_module_chain(SceModule2 *mod)
{
	int apitype;

	apitype = sceKernelInitApitype();
	
#ifdef DEBUG
	if (mod != NULL) {
		printk("Starting %s Apitype: 0x%X\n", mod->modname, apitype);
	}
#endif

	if(0 == strcmp(mod->modname, "sceLoadExec")) {
		if(psp_model != PSP_1000) {
			patch_partitions();
			sync_cache();
		}
	}

	// load after lflash
	if(0 == strcmp(mod->modname, "sceDisplay_Service")) {
		load_config();

		patch_sceLoadExec();
		sync_cache();
	}

	if(0 == strcmp(mod->modname, "sceMediaSync")) {
		if (psp_model == PSP_GO && rebootex_conf.iso_mode != NORMAL_MODE) {
			patch_sceMediaSync(mod->text_addr);
		}

		load_plugin();
		usb_charge();
	}

	if(0 == strcmp(mod->modname, "sceUmdCache_driver")) {
		patch_umdcache(mod->text_addr);
		sync_cache();
	}

	if(0 == strcmp(mod->modname, "sceNpSignupPlugin_Module")) {
		patch_npsignup(mod->text_addr);
		sync_cache();
	}

	if(0 == strcmp(mod->modname, "sceVshNpSignin_Module")) {
		patch_npsignin(mod->text_addr);
		sync_cache();
	}

	if(0 == strcmp(mod->modname, "sceNp")) {
		patch_np(mod->text_addr, 9, 90);
		sync_cache();
	}

	if(psp_model == PSP_GO && 0 == strcmp(mod->modname, "pspMarch33_Driver")) {
		patch_pspMarch33_Driver(mod->text_addr);
		sync_cache();
	}

	resolve_removed_nid((SceModule*)mod);
	
#ifdef DEBUG
	if(0 == strcmp(mod->modname, "sceKernelLibrary")) {
		printk_sync();
		printk("printk synchronized\n");
	}

	hook_import_bynid((SceModule*)mod, "KDebugForKernel", 0x84F370BC, printk, 0);
#endif

	patch_module_for_updater((SceModule*)mod);

	if (previous)
		return (*previous)(mod);

	return 0;
}

void syspatch_init()
{
	setup_module_handler();
	previous = sctrlHENSetStartModuleHandler(&syspatch_module_chain);
	patch_sceLoaderCore();
	patch_sceMemlmd();
	patch_sceInterruptManager();

	sync_cache();
}
