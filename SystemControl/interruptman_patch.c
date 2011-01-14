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

	_sw(0, mod->text_addr + 0xD70);
	_sw(0, mod->text_addr + 0xDE8);
	_sw(0, mod->text_addr + 0xDEC);
}
