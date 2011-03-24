#include <pspsdk.h>
#include <pspkernel.h>
#include <psputilsforkernel.h>
#include <pspsysmem_kernel.h>
#include <pspsysevent.h>
#include <pspiofilemgr.h>
#include <pspinit.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "systemctrl.h"
#include "modhandler.h"
#include "printk.h"
#include "syspatch.h"
#include "libs.h"
#include "systemctrl_patch_offset.h"
#include "systemctrl_pxe_patch_offset.h"

char installerpath[256];

int installer_thread(SceSize args, void * argp)
{
	//installer load result
	int result = 0;

	//log installer start
	printk("Starting %s\n", installerpath);

	//load installer module
	SceUID mod = sctrlKernelLoadModule(installerpath, 0, NULL);

	//log installer uid
	printk("Module UID: %08X\n", mod);

	//installer loaded
	if (mod >= 0) {
		//start installer
		int status = 0;
		result = sctrlKernelStartModule(mod, strlen(installerpath) + 1, installerpath, &status, NULL);

		//log start result
		printk("Start Result: %08X - Status %08X\n", result, status);
	}

	//kill loader thread
	sceKernelExitDeleteThread(0);

	//return dummy result
	return 0;
}

void patch_vsh_module(u32 text_addr)
{
	//force module start error + unload
	_sw(0x03E00008, text_addr + g_pxe_offs->vsh_module_patch.module_start);
	_sw(0x24020001, text_addr + g_pxe_offs->vsh_module_patch.module_start + 4);

	//spawn installer thread (to avoid modulemgr is busy error)
	SceUID thid = sceKernelCreateThread("installer_thread", installer_thread, 0x1A, 0x2000, 0, NULL);
	if(thid >= 0) sceKernelStartThread(thid, 0, NULL);
}

void patch_module(SceModule2 *mod)
{
	//patch loadexec for installer reboot stage
	if(0 == strcmp(mod->modname, "sceMediaSync")) {
		patch_sceLoadExec();
	}

	//replace vsh_module with installer
	if(0 == strcmp(mod->modname, "vsh_module")) {
		patch_vsh_module(mod->text_addr);
	}

	//log module start
	printk("Starting %s (%u, %u)!\n", mod->modname, mod->mpid_text, mod->mpid_data);
}

int on_module_start(SceModule2 *mod)
{
	//patch module
	patch_module(mod);

	//apply changes
	sync_cache();
	
	//return dummy result
	return 0;
}

void patch_sceLoadExec(void)
{
	//find loadexec module
	SceModule2 * loadexec = (SceModule2*)sctrlKernelFindModuleByName("sceLoadExec");

	//allow all user levels to call sceKernelExitVSHVSH (needed for installer reboot)
	_sw(0x10000008, loadexec->text_addr + g_offs->loadexec_patch_other.sceKernelExitVSHVSHCheck1);
	_sw(NOP, loadexec->text_addr + g_offs->loadexec_patch_other.sceKernelExitVSHVSHCheck1);
}

