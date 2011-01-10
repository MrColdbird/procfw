#include <pspkernel.h>
#include <pspsysevent.h>
#include <pspiofilemgr.h>
#include <stdio.h>
#include <string.h>

#include "main.h"
#include "systemctrl.h"
#include "printk.h"
#include "utils.h"
#include "kubridge.h"
#include "libs.h"

SceUID g_mod_start_thid = -1;
SceModule2 *g_mod_start = NULL;
int (*g_on_module_start)(SceModule2*) = NULL;

// sceKernelFindModuleByAddress most likely?
extern SceModule2 *LoadCoreForKernel_312CA47E(void *addr);

static int myKernelStartThread(SceUID thid, SceSize arglen, void *argp)
{
	int ret;

	if (g_mod_start_thid != -1 && thid == g_mod_start_thid) {
		if (g_on_module_start != NULL && g_mod_start != NULL) {
			(*g_on_module_start)(g_mod_start);
		}

		g_mod_start_thid = -1;
		g_mod_start = NULL;
	}

	ret = sceKernelStartThread(thid, arglen, argp);

	return ret;
}

static int myKernelCreateThread(const char *name, void *entry, int pri, int stack, u32 attr, void *opt)
{
	int ret;

	ret = sceKernelCreateThread(name, entry, pri, stack, attr, opt);

	if (ret >= 0 && !strcmp(name, "SceModmgrStart")) {
		g_mod_start = LoadCoreForKernel_312CA47E(entry);
		g_mod_start_thid = ret;
	}

	return ret;
}

void setup_module_handler(void)
{
	SceModule2 *mod;

	mod = (SceModule2*) sceKernelFindModuleByName("sceModuleManager");

	if (mod == NULL)
		return;
	
	//psp go patches
	if(psp_model == 4 ) {
		//backup function pointer (dword_622C)
		ProbeExec3 = (void*)mod->text_addr + 0x8864;

		//override function (sub_0045C)
		_sw(MAKE_CALL(_ProbeExec3), mod->text_addr + 0x7C6C);
	}

	_sw(MAKE_JUMP(_sceKernelCheckExecFile), mod->text_addr+0x87E4);

	PartitionCheck = (void*)mod->text_addr + 0x7FE0;
	_sw(MAKE_CALL(_PartitionCheck), mod->text_addr + 0x652C);
	_sw(MAKE_CALL(_PartitionCheck), mod->text_addr + 0x68A8);

	//no device check patches
	_sw(0, mod->text_addr + 0x0760);
	_sw(0x24020000, mod->text_addr + 0x07C0);
	_sw(0, mod->text_addr + 0x30B0);
	_sw(0, mod->text_addr + 0x310C);
	_sw(0x10000009, mod->text_addr + 0x3138);
	_sw(0, mod->text_addr + 0x3444);
	_sw(0, mod->text_addr + 0x349C);
	_sw(0x10000010, mod->text_addr + 0x34C8);
	_sw(0, mod->text_addr + 0x4360);
	_sw(0, mod->text_addr + 0x43A8);
	_sw(0, mod->text_addr + 0x43C0);

	hook_import_bynid((SceModule*)mod, "ThreadManForKernel", 0xF475845D, myKernelStartThread, 0);
	hook_import_bynid((SceModule*)mod, "ThreadManForKernel", 0x446D8DE6, myKernelCreateThread, 0);
}
