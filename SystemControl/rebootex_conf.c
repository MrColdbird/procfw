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

void load_rebootex_config(void)
{
	rebootex_config *conf = (rebootex_config*)(REBOOTEX_CONFIG);

	if (conf->magic == REBOOTEX_CONFIG_MAGIC) {
		memcpy(&rebootex_conf, conf, sizeof(*conf));
		SetUmdFile((char*)REBOOTEX_CONFIG_ISO_PATH);
		printk("%s: iso_mode %d fn: %s\n", __func__, rebootex_conf.iso_mode, GetUmdFile());
	} else {
		fill_vram(0xFF00);
		_sw(0, 0);
	}
}
