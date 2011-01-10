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

static char g_iso_filename[256];

char *sctrlSEGetUmdFile(void)
{
	return g_iso_filename;
}

void sctrlSESetUmdFile(char *file)
{
	STRCPY_S(g_iso_filename, file);
}

void sctrlSESetBootConfFileIndex(int index)
{
	rebootex_conf.iso_mode = index;
}

void get_iso_status_from_rebootex(void)
{
	rebootex_config *conf = (rebootex_config*)(0x88FB0020);

	if (conf->magic == REBOOTEX_CONFIG_MAGIC) {
		rebootex_conf.iso_mode = conf->iso_mode;
		STRCPY_S(g_iso_filename, (const char*)0x88FB0100);

		printk("%s: iso_mode %d fn: %s\n", __func__, rebootex_conf.iso_mode, g_iso_filename);
	}
}
