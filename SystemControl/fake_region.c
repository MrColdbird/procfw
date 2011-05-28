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
#include "printk.h"
#include "main.h"
#include "utils.h"

static u8 get_pscode_from_region(int region)
{
	u8 code;

	code = region;
	
	if(code < 12) {
		code += 2;
	} else {
		code -= 11;
	}

	if(code == 2) {
		code = 3;
	}

	printk("%s: region %d code %d\n", __func__, region, code);

	return code;
}

static int _sceChkregGetPsCode(u8 *pscode)
{
	pscode[0] = 1;
	pscode[1] = 0;
	pscode[2] = get_pscode_from_region(conf.fakeregion);
	pscode[3] = 0;
	pscode[4] = 1;
	pscode[5] = 0;
	pscode[6] = 1;
	pscode[7] = 0;

	return 0;
}

void patch_sceChkreg(void)
{
	u32 fp;
   
	// sceChkregGetPsCode
	fp = sctrlHENFindFunction("sceChkreg", "sceChkreg_driver", 0x59F8491D); 

	if (fp && conf.fakeregion) {
		REDIRECT_FUNCTION(_sceChkregGetPsCode, fp);
	}
}
