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
#include "libertas.h"
#include "systemctrl_patch_offset.h"

static STMOD_HANDLER previous;

static void patch_sceWlan_Driver(u32 text_addr);
static void patch_scePower_Service(u32 text_addr);
static void patch_sceUmdMan_driver(SceModule* mod);

static int need_msstor_speed(void)
{
	int key_config;

	if(conf.msspeed == MSSPEED_NONE)
		return 0;

	if(conf.msspeed == MSSPEED_ALWAYS) {
		return 1;
	}

	key_config = sceKernelApplicationType();

	if(key_config == PSP_INIT_KEYCONFIG_GAME) {
		if(conf.msspeed == MSSPEED_GAME || conf.msspeed == MSSPEED_POP_GAME) {
			return 1;
		}
	} else if(key_config == PSP_INIT_KEYCONFIG_POPS) {
		if(conf.msspeed == MSSPEED_POP || conf.msspeed == MSSPEED_POP_GAME) {
			return 1;
		}
	} else if(key_config == PSP_INIT_KEYCONFIG_VSH) {
		if(conf.msspeed == MSSPEED_VSH || conf.msspeed == MSSPEED_GAME_VSH) {
			return 1;
		}
	}

	return 0;
}

static void system_booted_action(void)
{
	int key_config;
	static int booted = 0;

	if (booted) {
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

	if(need_msstor_speed()) {
		msstor_init();
		printk("%s: msstor cache enabled\n", __func__);
	}

	booted = 1;
	sync_cache();
}

static int is_system_booted(void)
{
	int status;

	status = sctrlKernelGetSystemStatus();

	if (status == 0x00020000) {
		return 1;
	}

	return 0;
}

static int syspatch_module_chain(SceModule2 *mod)
{
	int apitype;

	apitype = sceKernelInitApitype();
	
#ifdef DEBUG
	printk("Starting %s Apitype: 0x%X\n", mod->modname, apitype);
	hook_import_bynid((SceModule*)mod, "KDebugForKernel", 0x84F370BC, printk, 0);
#endif

	if (is_system_booted()) {
		if(0 == strcmp(mod->modname, "sceNpSignupPlugin_Module")) {
			patch_npsignup(mod->text_addr);
			sync_cache();
			goto exit;
		}

		if(0 == strcmp(mod->modname, "sceVshNpSignin_Module")) {
			patch_npsignin(mod->text_addr);
			sync_cache();
			goto exit;
		}

		if(0 == strcmp(mod->modname, "sceNp")) {
			patch_np(mod->text_addr, 9, 90);
			sync_cache();
			goto exit;
		}

		if(conf.usbversion && 0 == strcmp(mod->modname, "sceUSB_Stor_Ms_Driver")) {
			patch_sceUSB_Stor_Ms_Driver((SceModule*)mod);
			goto exit;
		}

		system_booted_action();
		patch_module_for_version_spoof((SceModule*)mod);
		goto exit;
	}

	if(0 == strcmp(mod->modname, "sceLoadExec")) {
		u32 key_config;

		key_config = sceKernelApplicationType();
		
		if (key_config == PSP_INIT_KEYCONFIG_GAME) {
			if(PSP_1000 != psp_model) {
				prepatch_partitions();
				sync_cache();
			}
		}

		goto exit;
	}

	if(0 == strcmp(mod->modname, "sceSYSCON_Driver")) {
		resolve_syscon_driver((SceModule*)mod);
		goto exit;
	}

	// load after lflash
	if(0 == strcmp(mod->modname, "sceDisplay_Service")) {
		load_config();
		patch_sceLoadExec();
		sync_cache();
		goto exit;
	}

	if(0 == strcmp(mod->modname, "sceMediaSync")) {
		patch_sceMediaSync(mod->text_addr);
		sync_cache();
		goto exit;
	}

	if(0 == strcmp(mod->modname, "sceUmdMan_driver")) {
		patch_sceUmdMan_driver((SceModule*)mod);
		sync_cache();
		goto exit;
	}

	if(0 == strcmp(mod->modname, "sceUmdCache_driver")) {
		patch_umdcache(mod->text_addr);
		sync_cache();
		goto exit;
	}

	if(0 == strcmp(mod->modname, "sceWlan_Driver")) {
		patch_sceWlan_Driver(mod->text_addr);
		patch_Libertas_MAC(mod);
		sync_cache();
		goto exit;
	}

	if(0 == strcmp(mod->modname, "scePower_Service")) {
		patch_scePower_Service(mod->text_addr);
		sync_cache();
		goto exit;
	}

	if(0 == strcmp(mod->modname, "sceMesgLed")) {
		patch_mesgled((SceModule*)mod);
		sync_cache();
		goto exit;
	}

	if (0 == strcmp(mod->modname, "sceImpose_Driver")) {
		patch_sceChkreg();
		disable_PauseGame(mod->text_addr);
		usb_charge();
		sync_cache();
		goto exit;
	} 

	if(psp_model == PSP_GO && 0 == strcmp(mod->modname, "pspMarch33_Driver")) {
		patch_pspMarch33_Driver(mod->text_addr);
		sync_cache();
		goto exit;
	}

#ifdef DEBUG
	if(0 == strcmp(mod->modname, "sceKernelLibrary")) {
		printk_sync();
		printk("printk synchronized\n");
		goto exit;
	}
#endif

exit:
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
