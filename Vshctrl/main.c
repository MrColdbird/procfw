#include <pspsdk.h>
#include <pspsysmem_kernel.h>
#include <pspkernel.h>
#include <psputilsforkernel.h>
#include <pspsysevent.h>
#include <pspiofilemgr.h>
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "utils.h"
#include "systemctrl.h"
#include "printk.h"

PSP_MODULE_INFO("VshCtrl", 0x1007, 1, 0);

int module_start(SceSize args, void* argp)
{
	printk_init("ms0:/log_vshctrl.txt");
	printk("VshCtrl started\n");

	vshpatch_init();

	// always reset to NORMAL mode in VSH
	// to avoid ISO mode is used in homebrews in next reboot
	sctrlSESetBootConfFileIndex(0);
	sctrlSESetUmdFile("");

	return 0;
}
