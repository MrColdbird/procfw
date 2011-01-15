#include <pspkernel.h>
#include <psputilsforkernel.h>
#include <pspsysevent.h>
#include <pspiofilemgr.h>
#include <stdio.h>
#include <string.h>
#include "systemctrl.h"
#include "utils.h"
#include "printk.h"

void sync_cache(void)
{
	sceKernelIcacheInvalidateAll();
	sceKernelDcacheWritebackInvalidateAll();
}

#ifdef DEBUG
void hexdump(void *addr, int size)
{
	int i;
	u8 *p = (u8*)addr;

	if (addr == NULL) {
		printk("hexdump: <NULL>\n");

		return;
	}

	for(i=0; i<size; ++i) {
		printk("%02X", p[i]);
	}

	printk("\n");
}
#endif

int is_cpu_intr_enable(void)
{
	int ret;

	asm volatile ("mfic	%0, $0\n"
			"nop\n"
			"nop\n"
			"nop\n"
			"nop\n"
			"nop\n"
			"nop\n"
			"nop\n"
			"nop\n"
			"nop\n"
			"nop\n"
			"nop\n"
			"nop\n"
			"nop\n"
			"nop\n"
			"nop\n"
			"nop\n"
			"nop\n"
			"nop\n"
			"nop\n"
			"nop\n"
			"nop\n"
			: "=r"(ret)
			);

	return ret;
}

void fill_vram(u32 color)
{
	u32 *p = (u32*)0x44000000;

	while (p < (u32*)0x44200000) 
		*p++ = color;
}
