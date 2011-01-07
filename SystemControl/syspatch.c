#include <pspsdk.h>
#include <pspsysmem_kernel.h>
#include <pspkernel.h>
#include <psputilsforkernel.h>
#include <pspsysevent.h>
#include <pspiofilemgr.h>
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "loadexec_patch.h"
#include "utils.h"
#include "systemctrl.h"
#include "printk.h"

static STMOD_HANDLER previous;

static int syspatch_module_chain(SceModule2 *mod)
{
#ifdef DEBUG
	if (mod != NULL) {
		printk("%s\n", mod->modname);
	}
#endif

	if(0 == strcmp(mod->modname, "sceMediaSync")) {
		patch_sceLoadExec();
	}
	
	if (previous)
		return (*previous)(mod);

	return 0;
}

static void patch_sceLoaderCore(void)
{
	//find module
	SceModule2 * loadcore = (SceModule2 *)sceKernelFindModuleByName("sceLoaderCore");

	//undo rebootex patches
	void * memlmd_3F2AC9C6 = (void*)sctrlHENFindFunction("sceMemlmd", "memlmd", 0x3F2AC9C6);
	_sw(MAKE_CALL(memlmd_3F2AC9C6), loadcore->text_addr + 0x5CC8);
	_sw(MAKE_CALL(memlmd_3F2AC9C6), loadcore->text_addr + 0x5CF8);
	_sw(MAKE_CALL(memlmd_3F2AC9C6), loadcore->text_addr + 0x5D90);
	void * memlmd_E42AFE2E = (void*)sctrlHENFindFunction("sceMemlmd", "memlmd", 0xE42AFE2E);
	_sw(MAKE_CALL(memlmd_E42AFE2E), loadcore->text_addr + 0x41A4);
	_sw(MAKE_CALL(memlmd_E42AFE2E), loadcore->text_addr + 0x5CA4);
}

void syspatch_init()
{
	setup_module_handler();
	previous = sctrlHENSetStartModuleHandler(&syspatch_module_chain);
	patch_sceLoaderCore();

	sync_cache();
}
