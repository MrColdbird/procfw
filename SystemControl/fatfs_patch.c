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
#include "rebootex_conf.h"

void sctrlSESetBootDevice(int ms0)
{
	if (ms0) {
		rebootex_conf.bootdevice = 0x40;
	}

	rebootex_conf.bootdevice = 0x50;
}

// sceKernelBootFrom
// 0x50 = PSP_BOOT_EF0?
static int _InitForKernel_EE67E450(void)
{
	return rebootex_conf.bootdevice;
}

void patch_sceFATFS_Driver(u32 scefatfs_text_addr)
{
	_sw(MAKE_JUMP(_InitForKernel_EE67E450), scefatfs_text_addr+0xEF3C);
	_sw(0, scefatfs_text_addr+0xEF40);
}
