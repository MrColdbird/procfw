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
	int offset;
	int size;
} MemPart;

int g_high_memory_enabled = 0;

//in system memory manager
static unsigned int * (*get_partition)(int pid) = (void *)(0x88003E34);

static u8 g_p8_size = 4;

static inline int is_homebrews_runlevel(void);

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
	if(!is_homebrews_runlevel() && !forced && !g_high_memory_enabled) {
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

static void modify_partition(MemPart *part)
{
	u32 *meminfo;
	u32 offset, size;

	meminfo = part->meminfo;
	offset = part->offset;
	size = part->size;

	if(meminfo == NULL) {
		return;
	}

	if (offset != 0) {
		meminfo[1] = (offset << 20) + 0x88800000;
	}

	meminfo[2] = size << 20;
	((u32*)(meminfo[4]))[5] = (size << 21) | 0xFC;
}

static void delete_part_11(void)
{
	// P11 pointer in sysmem+0x00003EF8
	_sw(0, 0x880145e8);
}

static inline int is_homebrews_runlevel(void)
{
	int apitype;

	apitype = sceKernelInitApitype();
	
	if(apitype == 0x152 || apitype == 0x141) {
		return 1;
	}

	return 0;
}

void prepatch_partitions(void)
{
	MemPart p8, p11;

	(void)(delete_part_11);

	if(!is_homebrews_runlevel()) {
		printk("%s: no need to patch partition, quit\n", __func__);

		return;
	}

#if 0
	// Enable this 4MB but it will crash on standby
	delete_part_11();
#endif

	p8.meminfo = (*get_partition)(8);
	p11.meminfo = (*get_partition)(11);

	g_p8_size = p8.size = 1;
	
	if(p11.meminfo != NULL) {
		p8.offset = 56-4-p8.size;
	} else {
		p8.offset = 56-p8.size;
	}

	modify_partition(&p8);

	p11.size = 4;
	p11.offset = 56-4;
	modify_partition(&p11);
	display_meminfo();
}

void patch_partitions(void) 
{
	MemPart p2, p9;
	int max_user_part_size;

	// shut up gcc warning
	(void)display_meminfo;

	p2.meminfo = (*get_partition)(2);
	p9.meminfo = (*get_partition)(9);

	if(g_p2_size == 24 && g_p9_size == 24) {
		p2.size = MAX_HIGH_MEMSIZE;
		p9.size = 0;
	} else {
		p2.size = g_p2_size;
		p9.size = g_p9_size;
	}

	if((*get_partition)(11) != NULL) {
		max_user_part_size = 56 - 4 - g_p8_size;
	} else {
		max_user_part_size = 56 - g_p8_size;
	}

	if (p2.size + p9.size > max_user_part_size) {
		// reserved 4MB for P11
		int reserved_len;

		reserved_len = p2.size + p9.size - max_user_part_size;

		if(p9.size > reserved_len) {
			p9.size -= reserved_len;
		} else {
			p2.size -= reserved_len - p9.size; 
			p9.size = 0;
		}
	}

	printk("%s: p2/p9 %d/%d\n", __func__, p2.size, p9.size);

	//reset partition length for next reboot
	sctrlHENSetMemory(24, 24);

	p2.offset = 0;
	modify_partition(&p2);

	p9.offset = p2.size;
	modify_partition(&p9);

	g_high_memory_enabled = 1;
	display_meminfo();
	unlock_high_memory(0);
}
