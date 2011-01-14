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
#include "strsafe.h"
#include "rebootex_conf.h"

static void load_default_rebootex_config()
{
	memset(&rebootex_conf, 0, sizeof(rebootex_conf));
	rebootex_conf.magic = REBOOTEX_CONFIG_MAGIC;
	rebootex_conf.iso_mode = 0;
	rebootex_conf.bootdevice = 0x50;
	SetUmdFile("");
}

void load_rebootex_config(void)
{
	rebootex_config *conf = (rebootex_config*)(0x88FB0020);

	load_default_rebootex_config();

	if (conf->magic == REBOOTEX_CONFIG_MAGIC) {
		rebootex_conf.iso_mode = conf->iso_mode;

		if (conf->bootdevice == 0x40 || conf->bootdevice == 0x50) {
			rebootex_conf.bootdevice = conf->bootdevice;
		}

		SetUmdFile((char*)0x88FB0100);
		printk("%s: iso_mode %d fn: %s\n", __func__, rebootex_conf.iso_mode, GetUmdFile());
	}
}
