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

char *GetUmdFile(void)
{
	return g_iso_filename;
}

char *sctrlSEGetUmdFile(void) __attribute__((alias("GetUmdFile")));

void SetUmdFile(char *file)
{
	STRCPY_S(g_iso_filename, file);
}

void sctrlSESetUmdFile(char *file) __attribute__((alias("SetUmdFile")));

void sctrlSESetBootConfFileIndex(int index)
{
	rebootex_conf.iso_mode = index;
}
