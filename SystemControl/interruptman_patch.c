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
	_sw(NOP, mod->text_addr + g_offs->interruptman_patch.InvalidSyscallCheck2);
	_sw(NOP, mod->text_addr + g_offs->interruptman_patch.InvalidSyscallCheck2+4);

	//TODO currently syscall on kernel module patch is not really working
	//the syscall call on kernel module would always do nothing but return 0
}
