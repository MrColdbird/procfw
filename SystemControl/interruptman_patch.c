#include <pspkernel.h>
#include <pspsysevent.h>
#include <pspiofilemgr.h>
#include <stdio.h>
#include <string.h>

#include "main.h"
#include "utils.h"
#include "systemctrl.h"
#include "printk.h"

void patch_sceInterruptManager(void)
{
	SceModule2 *mod = (SceModule2*) sceKernelFindModuleByName("sceInterruptManager");

	// disable writing invalid address to reset vector
	_sw(0, mod->text_addr + 0x00000DE8);
	_sw(0, mod->text_addr + 0x00000DEC);

	// disable crash code
	_sw(0, mod->text_addr + 0x00000E94);
	_sw(0, mod->text_addr + 0x00000E98);

	//TODO currently syscall on kernel module patch is not really working
	//the syscall call on kernel module would always do nothing but return 0
}
