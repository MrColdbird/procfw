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
#include <pspinit.h>
#include <pspsysmem_kernel.h>
#include <malloc.h>
#include "utils.h"
#include "printk.h"
#include "main.h"

static SceUID heapid = -1;

int oe_mallocinit()
{
	int size;
	int key_config;
   
	key_config = sceKernelApplicationType();

	if (key_config == PSP_INIT_KEYCONFIG_POPS) {
		return 0;
	} else if (key_config == PSP_INIT_KEYCONFIG_VSH) {
		size = 14*1024;
	} else {
		size = 45*1024;
	}

	if (heapid >= 0) {
		return 0;
	}
	
	heapid = sctrlKernelCreateHeap(PSP_MEMORY_PARTITION_KERNEL, size, 1, "SystemCtrlHeap");

	printk("%s: 0x%08X heap size %d\n", __func__, heapid, size);

	return (heapid < 0) ? heapid : 0;
}

void *oe_malloc(size_t size)
{
	void *p;

	p = sctrlKernelAllocHeapMemory(heapid, size);
//	printk("%s: %d@0x%08X\n", __func__, size, (u32)p);

	return p;
}

void oe_free(void *p)
{
	sctrlKernelFreeHeapMemory(heapid, p);
}

int oe_mallocterminate()
{
	return sctrlKernelDeleteHeap(heapid);
}

