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
#include "libs.h"

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
	int ret;

	ret = rebootex_conf.bootdevice;
	printk("%s returns 0x%08X\n", __func__, ret);

	return ret;
}

void patch_sceFATFS_Driver(u32 scefatfs_text_addr)
{
	_sw(MAKE_JUMP(_InitForKernel_EE67E450), scefatfs_text_addr+0xEF3C);
	_sw(0, scefatfs_text_addr+0xEF40);
}

void patch_sceMediaSync(u32 scemediasync_text_addr)
{
	_sw(MAKE_JUMP(_InitForKernel_EE67E450), scemediasync_text_addr+0x2F64);
	_sw(0, scemediasync_text_addr+0x2F68);
}

static int _sceKernelInitApiType(void)
{
	int ret;

	ret = 0x125;
	printk("%s returns 0x%08X\n", __func__, ret);

	return ret;
}

static int _sceKernelApplicationType(void)
{
	int ret;

	ret = 0x200;
	printk("%s returns 0x%08X\n", __func__, ret);

	return ret;
}

static int _sceKernelBootFrom(void)
{
	int ret;

	ret = rebootex_conf.bootdevice;
	printk("%s returns 0x%08X\n", __func__, ret);

	return ret;
}

static const char *_InitForKernel_2213275D(void)
{
	char *iso;

	iso = "ef0:/PSP/GAME/NPHG00014/EBOOT.PBP";

	printk("%s returns %s\n", __func__, iso);

	return iso;
}

void patch_bootfrom(SceModule *mod)
{
	if (!strcmp(((SceModule2*)mod)->modname, "sceMediaSync")) {
		hook_import_bynid(mod, "InitForKernel", 0x2213275D, _InitForKernel_2213275D, 0);

		u32 text_addr = ((SceModule2*)mod)->text_addr;

		_sw(0x1000FFDB, text_addr+0x10B4);

		sync_cache();
	}
}
