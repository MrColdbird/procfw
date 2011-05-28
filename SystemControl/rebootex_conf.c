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
