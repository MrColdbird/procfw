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

#include <pspkernel.h>
#include <pspsysevent.h>
#include <pspiofilemgr.h>
#include <stdio.h>
#include <string.h>
#include <psploadcore.h>

#include "main.h"
#include "systemctrl.h"
#include "printk.h"
#include "utils.h"
#include "kubridge.h"
#include "libs.h"
#include "systemctrl_patch_offset.h"

int (*g_on_module_start)(SceModule2*) = NULL;

static int (*_prologue_module)(void *unk0, SceModule2 *mod) = NULL;

static int prologue_module(void *unk0, SceModule2 *mod)
{
	int ret = (*_prologue_module)(unk0, mod);
	
	if (ret >= 0) {
		unlock_high_memory(0);

		if(g_on_module_start != NULL) {
			(*g_on_module_start)(mod);
		}
	}
	
	return ret;
}

void setup_module_handler(void)
{
	SceModule2 *mod;

	mod = (SceModule2*) sctrlKernelFindModuleByName("sceModuleManager");

	if (mod == NULL)
		return;
	
	ProbeExec3 = (void*)mod->text_addr + g_offs->modulemgr_patch.ProbeExec3;
	_sw(MAKE_CALL(_ProbeExec3), mod->text_addr + g_offs->modulemgr_patch.ProbeExec3Call);
	_sw(MAKE_JUMP(_sceKernelCheckExecFile), mod->text_addr + g_offs->modulemgr_patch.sceKernelCheckExecFileImport);

	PartitionCheck = (void*)mod->text_addr + g_offs->modulemgr_patch.PartitionCheck;
	_sw(MAKE_CALL(_PartitionCheck), mod->text_addr + g_offs->modulemgr_patch.PartitionCheckCall1);
	_sw(MAKE_CALL(_PartitionCheck), mod->text_addr + g_offs->modulemgr_patch.PartitionCheckCall2);

	//no device check patches
	_sw(NOP, mod->text_addr + g_offs->modulemgr_patch.DeviceCheck1);
	_sw(0x24020000, mod->text_addr + g_offs->modulemgr_patch.DeviceCheck2);
	_sw(NOP, mod->text_addr + g_offs->modulemgr_patch.DeviceCheck3);
	_sw(NOP, mod->text_addr + g_offs->modulemgr_patch.DeviceCheck4);
	_sw(0x10000009, mod->text_addr + g_offs->modulemgr_patch.DeviceCheck5);
	_sw(NOP, mod->text_addr + g_offs->modulemgr_patch.DeviceCheck6);
	_sw(NOP, mod->text_addr + g_offs->modulemgr_patch.DeviceCheck7);
	_sw(0x10000010, mod->text_addr + g_offs->modulemgr_patch.DeviceCheck8);

	_prologue_module = (void*)(mod->text_addr + g_offs->modulemgr_patch.PrologueModule);
	_sw(MAKE_CALL(prologue_module), mod->text_addr + g_offs->modulemgr_patch.PrologueModuleCall);

#ifdef DEBUG
	setup_validate_stub((SceModule*)mod);
#endif
}
