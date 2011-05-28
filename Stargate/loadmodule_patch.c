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
#include <pspiofilemgr_kernel.h>
#include <psploadcore.h>
#include <pspsysmem_kernel.h>
#include <stdio.h>
#include <string.h>
#include "printk.h"
#include "systemctrl.h"
#include "systemctrl_private.h"
#include "strsafe.h"
#include "libs.h"
#include "utils.h"

int (*_sceKernelLoadModule)(char *fname, int flag, void *opt) = NULL;

int load_module_get_function(void)
{
	_sceKernelLoadModule = (void*)sctrlHENFindFunction("sceModuleManager", "ModuleMgrForUser", 0x977DE386);
	
	if (_sceKernelLoadModule == NULL) return -5;

	return 0;
}

int myKernelLoadModule(char *fname, int flag, void *opt)
{
	int ret;

	ret = (*_sceKernelLoadModule)(fname, flag, opt);

	if (ret == 0x80020148 || ret == 0x80020130) {
		if (!strncasecmp(fname, "ms0:", sizeof("ms0:")-1)) {
			ret = 0x80020146;
			printk("%s: [FAKE] -> 0x%08X\n", __func__, ret);
		}
	}
	
	return ret;
}

void patch_load_module(SceModule *mod)
{
	hook_import_bynid(mod, "ModuleMgrForUser", 0x977DE386, &myKernelLoadModule, 1);
}
