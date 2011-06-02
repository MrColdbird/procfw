/*
 * This file is part of PRO CFW.

 * PRO CFW is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * PRO CFW is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PRO CFW. If not, see <http://www.gnu.org/licenses/ .
 */

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

u32 sctrlSEGetBootConfFileIndex(void)
{
	return rebootex_conf.iso_mode;
}
