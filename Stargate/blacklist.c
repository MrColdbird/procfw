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
#include <pspkernel.h>
#include <pspsysmem_kernel.h>
#include <pspthreadman_kernel.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "systemctrl.h"
#include "utils.h"
#include "printk.h"

static char *modname_blacklist[] = {
	"KHBBS_patch",
};

void module_blacklist(SceModule* mod_)
{
	SceModule2 *mod = (SceModule2*) mod_;
	size_t i;

	for(i=0; i<NELEMS(modname_blacklist); ++i) {
		if(0 == strcmp(modname_blacklist[i], mod->modname)) {
			printk("%s: found blacklisted module\n", __func__);
			MAKE_DUMMY_FUNCTION_RETURN_1(mod->entry_addr);
			sync_cache();
			break;
		}
	}
}
