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

int (*g_on_module_start)(SceModule2*) = NULL;

// sceKernelFindModuleByAddress most likely?
extern SceModule2 *LoadCoreForKernel_312CA47E(void *addr);

static int (*_prologue_module)(void *unk0, SceModule2 *mod) = NULL;

static int prologue_module(void *unk0, SceModule2 *mod)
{
	int ret = (*_prologue_module)(unk0, mod);
	
	if (ret >= 0) {
		unlock_high_memory(0);
		(*g_on_module_start)(mod);
	}
	
	return ret;
}

void setup_module_handler(void)
{
	SceModule2 *mod;

	mod = (SceModule2*) sceKernelFindModuleByName("sceModuleManager");

	if (mod == NULL)
		return;
	
	//backup function pointer (dword_622C)
	ProbeExec3 = (void*)mod->text_addr + 0x8864;

	//override function (sub_0045C)
	_sw(MAKE_CALL(_ProbeExec3), mod->text_addr + 0x7C6C);

	_sw(MAKE_JUMP(_sceKernelCheckExecFile), mod->text_addr+0x87E4);

	PartitionCheck = (void*)mod->text_addr + 0x7FE0;
	_sw(MAKE_CALL(_PartitionCheck), mod->text_addr + 0x652C);
	_sw(MAKE_CALL(_PartitionCheck), mod->text_addr + 0x68A8);

	//no device check patches
	_sw(NOP, mod->text_addr + 0x0760);
	_sw(0x24020000, mod->text_addr + 0x07C0);
	_sw(NOP, mod->text_addr + 0x30B0);
	_sw(NOP, mod->text_addr + 0x310C);
	_sw(0x10000009, mod->text_addr + 0x3138);
	_sw(NOP, mod->text_addr + 0x3444);
	_sw(NOP, mod->text_addr + 0x349C);
	_sw(0x10000010, mod->text_addr + 0x34C8);

	_prologue_module = (void*)(mod->text_addr+0x8134);
	_sw(MAKE_CALL(prologue_module), mod->text_addr+0x7058);

#ifdef DEBUG
	setup_validate_stub((SceModule*)mod);
#endif
}
