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

static STMOD_HANDLER previous;

static int syspatch_module_chain(SceModule2 *mod)
{
#if 0
	if (mod != NULL) {
		int apitype;

		apitype = sceKernelInitApitype();
		printk("Starting %s Apitype: 0x%X\n", mod->modname, apitype);
	}
#endif

	if(0 == strcmp(mod->modname, "sceLoadExec")) {
		if(psp_model != PSP_1000) {
			patch_partitions();
		}

		sync_cache();
	}

	if(0 == strcmp(mod->modname, "sceMediaSync")) {
		patch_sceLoadExec();
		sync_cache();
	}

	if(0 == strcmp(mod->modname, "sceUmdCache_driver")) {
		patch_umdcache(mod->text_addr);
		sync_cache();
	}

	resolve_sceKernelIcacheClearAll((SceModule*)mod);

#ifdef DEBUG
	if(0 == strcmp(mod->modname, "sceKernelLibrary")) {
		printk_sync();
		printk("printk synchronized\n");
	}
#endif

#ifdef DEBUG
	setup_validate_stub((SceModule*)mod);
#endif

	if(psp_model != PSP_1000) {
		unlock_high_memory();
	}

	if (previous)
		return (*previous)(mod);

	return 0;
}

int _sceKernelStartModule(SceUID modid, SceSize argsize, void *argp, int *status, SceKernelSMOption *option)
{
#if DEBUG
	int ret;

	ret = sceKernelStartModule(modid, argsize, argp, status, option);

	if (ret >= 0) {
		validate_stub_by_uid(ret);
	}

	return ret;
#else
	return 0;
#endif
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
