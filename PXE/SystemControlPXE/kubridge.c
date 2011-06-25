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
#include <psputilsforkernel.h>
#include <pspthreadman_kernel.h>
#include <pspsysevent.h>
#include <pspiofilemgr.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "systemctrl.h"
#include "modhandler.h"
#include "printk.h"
#include "syspatch.h"

extern int SysMemForKernel_458A70B5(void);

SceUID kuKernelLoadModule(const char *path, int flags, SceKernelLMOption *option)
{
	u32 k1 = pspSdkSetK1(0);
	int result = sctrlKernelLoadModule(path, flags, option);
	pspSdkSetK1(k1);

	return result;
}

int kuKernelGetModel(void)
{
	return sctrlKernelGetModel();
}

