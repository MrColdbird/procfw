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
#include "systemctrl_patch_offset.h"

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
	ProbeExec3 = (void*)mod->text_addr + g_offs->module_handler_patch.ProbeExec3;

	//override function (sub_0045C)
	_sw(MAKE_CALL(_ProbeExec3), mod->text_addr + g_offs->module_handler_patch.ProbeExec3Call);

	_sw(MAKE_JUMP(_sceKernelCheckExecFile), mod->text_addr + g_offs->module_handler_patch.sceKernelCheckExecFileImport);

	PartitionCheck = (void*)mod->text_addr + g_offs->module_handler_patch.PartitionCheck;
	_sw(MAKE_CALL(_PartitionCheck), mod->text_addr + g_offs->module_handler_patch.PartitionCheckCall1);
	_sw(MAKE_CALL(_PartitionCheck), mod->text_addr + g_offs->module_handler_patch.PartitionCheckCall2);

	//no device check patches
	_sw(NOP, mod->text_addr + g_offs->module_handler_patch.DeviceCheck1);
	_sw(0x24020000, mod->text_addr + g_offs->module_handler_patch.DeviceCheck2);
	_sw(NOP, mod->text_addr + g_offs->module_handler_patch.DeviceCheck3);
	_sw(NOP, mod->text_addr + g_offs->module_handler_patch.DeviceCheck4);
	_sw(0x10000009, mod->text_addr + g_offs->module_handler_patch.DeviceCheck5);
	_sw(NOP, mod->text_addr + g_offs->module_handler_patch.DeviceCheck6);
	_sw(NOP, mod->text_addr + g_offs->module_handler_patch.DeviceCheck7);
	_sw(0x10000010, mod->text_addr + g_offs->module_handler_patch.DeviceCheck8);

	_prologue_module = (void*)(mod->text_addr + g_offs->module_handler_patch.PrologueModule);
	_sw(MAKE_CALL(prologue_module), mod->text_addr + g_offs->module_handler_patch.PrologueModuleCall);

#ifdef DEBUG
	setup_validate_stub((SceModule*)mod);
#endif
}
