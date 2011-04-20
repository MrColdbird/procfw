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
#include "nid_resolver.h"
#include "systemctrl_patch_offset.h"

static STMOD_HANDLER previous;

static void patch_sceWlan_Driver(u32 text_addr);
static void patch_scePower_Service(u32 text_addr);
static void patch_sceUmdMan_driver(SceModule* mod);

static inline void set_clock(void)
{
	int key_config, ret;

	ret = sctrlKernelGetSystemStatus();
   
	// status becomes 0x00020000 after init_file loads
	if (ret != 0x00020000) {
		return;
	}
	
	key_config = sceKernelApplicationType();

	if (key_config == PSP_INIT_KEYCONFIG_GAME ||
			key_config == PSP_INIT_KEYCONFIG_POPS
	   ) {
		SetSpeed(conf.umdisocpuspeed, conf.umdisobusspeed);
	} else if (conf.vshcpuspeed != 0) {
		SetSpeed(conf.vshcpuspeed, conf.vshbusspeed);
	}

	sync_cache();
}

static int syspatch_module_chain(SceModule2 *mod)
{
	int apitype;

	apitype = sceKernelInitApitype();
	
#ifdef DEBUG
	printk("Starting %s Apitype: 0x%X\n", mod->modname, apitype);
#endif

	if(0 == strcmp(mod->modname, "sceLoadExec")) {
		u32 key_config;

		key_config = sceKernelApplicationType();
		
		if (key_config == PSP_INIT_KEYCONFIG_GAME) {
			if(PSP_1000 != psp_model) {
				prepatch_partitions();
				sync_cache();
			}
		}
	}

	if(0 == strcmp(mod->modname, "sceSYSCON_Driver")) {
		resolve_syscon_driver((SceModule*)mod);
	}

	// load after lflash
	if(0 == strcmp(mod->modname, "sceDisplay_Service")) {
		load_config();
		patch_sceLoadExec();
		sync_cache();
	}

	if(0 == strcmp(mod->modname, "sceMediaSync")) {
		SceModule * impose;

		patch_sceMediaSync(mod->text_addr);
		impose = sctrlKernelFindModuleByName("sceImpose_Driver");

		if(impose) {
			disable_PauseGame(impose->text_addr);
		}

		sync_cache();
	}

	if(0 == strcmp(mod->modname, "sceUmdMan_driver")) {
		patch_sceUmdMan_driver(mod);
		sync_cache();
	}

	if(0 == strcmp(mod->modname, "sceUmdCache_driver")) {
		patch_umdcache(mod->text_addr);
		sync_cache();
	}

	if(0 == strcmp(mod->modname, "sceWlan_Driver")) {
		patch_sceWlan_Driver(mod->text_addr);
		sync_cache();
	}

	if(0 == strcmp(mod->modname, "scePower_Service")) {
		patch_scePower_Service(mod->text_addr);
		sync_cache();
	}

	if(0 == strcmp(mod->modname, "sceMesgLed")) {
		patch_mesgled((SceModule*)mod);
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

	if (0 == strcmp(mod->modname, "sceImpose_Driver")) {
		patch_sceChkreg();
		usb_charge();
		sync_cache();
	} 

	if(psp_model == PSP_GO && 0 == strcmp(mod->modname, "pspMarch33_Driver")) {
		patch_pspMarch33_Driver(mod->text_addr);
		sync_cache();
	}

#ifdef DEBUG
	if(0 == strcmp(mod->modname, "sceKernelLibrary")) {
		printk_sync();
		printk("printk synchronized\n");
	}

	hook_import_bynid((SceModule*)mod, "KDebugForKernel", 0x84F370BC, printk, 0);
#endif

	patch_module_for_version_spoof((SceModule*)mod);
	set_clock();

	if (previous)
		return (*previous)(mod);

	return 0;
}

static void patch_sceWlan_Driver(u32 text_addr)
{
	_sw(NOP, text_addr + g_offs->wlan_driver_patch.FreqCheck);
}

static void patch_scePower_Service(u32 text_addr)
{
	// scePowerGetBacklightMaximum always returns 4
	_sw(NOP, text_addr + g_offs->power_service_patch.scePowerGetBacklightMaximumCheck);
}

static int _sceKernelBootFromForUmdMan(void)
{
	return 0x20;
}

static void patch_sceUmdMan_driver(SceModule* mod)
{
	if(is_homebrews_runlevel()) {
		hook_import_bynid(mod, "InitForKernel", 0x27932388, _sceKernelBootFromForUmdMan, 0);
	}
}

void syspatch_init()
{
	setup_module_handler();
	previous = sctrlHENSetStartModuleHandler(&syspatch_module_chain);
	patch_sceLoaderCore();
	patch_sceMemlmd();
	patch_sceInterruptManager();
	patch_sceSystemMemoryManager();

	sync_cache();
}
