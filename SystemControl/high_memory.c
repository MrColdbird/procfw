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

typedef struct _MemPart {
	u32 *meminfo;
	void *addr;
	u32 size;
} MemPart;

int g_high_memory_enabled = 0;

//prevent umd-cache in homebrew, so we can drain the cache partition.
void patch_umdcache(u32 text_addr)
{
	int ret;

	ret = sceKernelInitKeyConfig();

	if (ret != PSP_INIT_KEYCONFIG_GAME)
		return;

	ret = sceKernelBootFrom();

	if (ret != 0x40 && ret != 0x50)
		return;
	
	//kill module start
	_sw(0x03E00008, text_addr + 0x9C8);
	_sw(0x24020001, text_addr + 0x9CC);
}

void unlock_high_memory(u32 forced)
{
	if(!forced && !g_high_memory_enabled) {
		return;
	}
	
	//unlock memory
	unsigned int i = 0; for(; i < 0x40; i += 4) {
		_sw(0xFFFFFFFF, 0xBC000040 + i);
	}
}

static int display_meminfo(void)
{
#ifdef DEBUG
	int i;
	int pid = 1;
	int max = 256;

	printk("Memory Partitions:\n");
	printk("N  |    BASE    |   SIZE   | TOTALFREE |  MAXFREE  | ATTR |\n");
	printk("---|------------|----------|-----------|-----------|------|\n");

	for(i = pid; i <= max; i++) {
		SceSize total;
		SceSize free;
		PspSysmemPartitionInfo info;

		memset(&info, 0, sizeof(info));
		info.size = sizeof(info);

		if(sceKernelQueryMemoryPartitionInfo(i, &info) == 0) {
			free = sceKernelPartitionMaxFreeMemSize(i);
			total = sceKernelPartitionTotalFreeMemSize(i);
			printk("%-2d | 0x%08X | %8d | %9d | %9d | %04X |\n", 
					i, info.startaddr, info.memsize, total, free, info.attr);
		}
	}
#endif

	return 0;
}

void prepatch_partitions(void)
{
	unsigned int * (*GetPartition)(int pid) = (void *)(0x88003E34);
	MemPart p8, p11;

	p8.meminfo = (*GetPartition)(8);
	p8.size = 4;

	p11.meminfo = (*GetPartition)(11);
	p11.size = 0; // nuke p11

	if (p8.meminfo != NULL) {
		// move p8 to 52M
		p8.meminfo[1] = (52 << 20) + 0x88800000;
		p8.meminfo[2] = p8.size << 20;
		((unsigned int *)(p8.meminfo[4]))[5] = (p8.size << 21) | 0xFC;
	} else {
		printk("%s: part8 not found\n", __func__);
	}

	if (p11.meminfo != NULL) {
		// move p11 to the end of p8
		p11.meminfo[1] = ((52 + p8.size) << 20) + 0x88800000;
		p11.meminfo[2] = p11.size << 20;
		((unsigned int *)(p11.meminfo[4]))[5] = (p11.size << 21) | 0xFC;
	} else {
//		printk("%s: part11 not found\n", __func__);
	}
}

void patch_partitions(void) 
{
	MemPart p2, p9;
	//system memory manager
	unsigned int * (*GetPartition)(int pid) = (void *)(0x88003E34);

	// shut up gcc warning
	(void)display_meminfo;

	p2.meminfo = (*GetPartition)(2);
	p9.meminfo = (*GetPartition)(9);

	if(g_p2_size == 24 && g_p9_size == 24) {
		p2.size = MAX_HIGH_MEMSIZE;
		p9.size = 0;
	} else {
		p2.size = g_p2_size;
		p9.size = g_p9_size;
	}

	//reset partition length for next reboot
	sctrlHENSetMemory(24, 24);

	if(p2.meminfo != NULL) {
		//resize partition 2
		p2.meminfo[2] = p2.size << 20;
		((unsigned int *)(p2.meminfo[4]))[5] = (p2.size << 21) | 0xFC;
	} else {
		printk("%s: part2 not found\n", __func__);
	}

	if (p9.meminfo != NULL) {
		// at end of p2
		p9.meminfo[1] = (p2.size << 20) + 0x88800000;
		//resize partition 9
		p9.meminfo[2] = p9.size << 20;
		((unsigned int *)(p9.meminfo[4]))[5] = (p9.size << 21) | 0xFC;
	} else {
		printk("%s: part9 not found\n", __func__);
	}

	g_high_memory_enabled = 1;
	unlock_high_memory(0);
}
