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

char installerpath[256];

int installer_thread(SceSize args, void * argp)
{
	//installer load result
	int result = 0;

	//log installer start
	printk("Starting %s\n", installerpath);

	//load installer module
	SceUID mod = sceKernelLoadModule(installerpath, 0, NULL);

	//log installer uid
	printk("Module UID: %08X\n", mod);

	//installer loaded
	if (mod >= 0) {
		//start installer
		int status = 0;
		result = sceKernelStartModule(mod, strlen(installerpath) + 1, installerpath, &status, NULL);

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
	_sw(0x03E00008, text_addr + 0xF570);
	_sw(0x24020001, text_addr + 0xF574);

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
	SceModule2 * loadexec = (SceModule2*)sceKernelFindModuleByName("sceLoadExec");

	//allow all user levels to call sceKernelExitVSHVSH (needed for installer reboot)
	_sw(0x10000008, loadexec->text_addr + 0x168C);
	_sw(0, loadexec->text_addr + 0x16C0);
}

