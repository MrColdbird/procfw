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

#include "main.h"
#include "utils.h"
#include "systemctrl.h"
#include "printk.h"
#include "systemctrl_patch_offset.h"

void patch_sceInterruptManager(void)
{
	SceModule2 *mod = (SceModule2*) sctrlKernelFindModuleByName("sceInterruptManager");

	// disable writing invalid address to reset vector
	_sw(NOP, mod->text_addr + g_offs->interruptman_patch.InvalidSyscallCheck1);
	_sw(NOP, mod->text_addr + g_offs->interruptman_patch.InvalidSyscallCheck1+4);

	// disable crash code
	_sw(0x408F7000, mod->text_addr + g_offs->interruptman_patch.InvalidSyscallCheck2); // mct0 $t7, $EPC
	_sw(NOP, mod->text_addr + g_offs->interruptman_patch.InvalidSyscallCheck2+4);

	//TODO currently syscall on kernel module patch is not really working
	//the syscall call on kernel module would always do nothing but return 0
}
