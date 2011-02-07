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

int g_high_memory_enabled = 0;

//prevent umd-cache in homebrew, so we can drain the cache partition.
void patch_umdcache(u32 text_addr)
{
	if(g_high_memory_enabled == 1) {
		//kill module start
		_sw(0x03E00008, text_addr + 0x9C8);
		_sw(0x24020001, text_addr + 0x9CC);
	}
}

void unlock_high_memory(void)
{
	if(!g_high_memory_enabled) {
		return;
	}
	
	//unlock memory
	unsigned int i = 0; for(; i < 0x40; i += 4) {
		_sw(0xFFFFFFFF, 0xBC000040 + i);
	}
}

static inline int can_override_high_memory_setting(void)
{
	int apitype;
	const char *path;

	path = sceKernelInitFileName();
	apitype = sceKernelInitApitype();

	// have init file?
	if(path == NULL)
		return 0;

	// homebrew runlevel?
	if(apitype != PSP_INIT_APITYPE_MS2 && apitype != 0x152)
		return 0;

	// p2 and p9 size untouch?
	if(g_p2_size != 24 || g_p9_size != 24)
		return 0;

	// minimal size as "xx0:/PSP/GAME/x/EBOOT.PBP"
	if(strlen(path) < 25)
		return 0;

	// validate path
	if(0 != strncmp(path + 2, "0:/PSP/GAME/", sizeof("0:/PSP/GAME/")-1))
		return 0;

	// validate ext
	if (0 != strcmp(path + strlen(path) - 3, "PBP")) {
		return 0;
	}	

	return 1;
}

//it's partition 9 on 6.31... its smaller than 5.X one too... only 24MB instead of 28MB...
void patch_partitions(void) 
{
	// real len we are going to use
	u32 p2_len = g_p2_size;
	u32 p9_len = g_p9_size;
	u32 p8_len = 4;

	//memory layout: P2 P9 P8 P11
	//P11 only occurs in the first boot

	//system memory manager
	unsigned int * (*GetPartition)(int pid) = (void *)(0x88003E34);

	//get partition 2
	unsigned int * p2 = GetPartition(2);

	//get partition 8
	unsigned int * p8 = GetPartition(8);

	//get partition 9
	unsigned int * p9 = GetPartition(9);

	//get partition 11, only occurs in first boot
	unsigned int * p11 = GetPartition(11);

	//set highmemory for homebrew eboots, it doesn't hurt.
	if (can_override_high_memory_setting()) {
		p2_len = MAX_HIGH_MEMSIZE;
		p9_len = 0;
	}

	if(p2_len <= 24 || (p2_len + p9_len) > MAX_HIGH_MEMSIZE) {
		return;
	}

	//force umdcache kill for disc0 reboots
	if(p2_len != 24) g_high_memory_enabled = 1;

	//reset partition length for next reboot
	g_p2_size = 24;
	g_p9_size = 24;

	if(p2 != NULL) {
		//resize partition 2
		p2[2] = p2_len << 20;
		((unsigned int *)(p2[4]))[5] = (p2_len << 21) | 0xFC;
	} else {
		printk("%s: part2 not found\n", __func__);
	}

	if (p9 != NULL) {
		//resize partition 9
		p9[1] = (p2_len << 20) + 0x88800000;
		p9[2] = p9_len << 20;
		((unsigned int *)(p9[4]))[5] = (p9_len << 21) | 0xFC;
	} else {
		printk("%s: part9 not found\n", __func__);
	}

	if (p8 != NULL) {
		// move p8 to the end of p9
		p8[1] = ((p2_len + p9_len) << 20) + 0x88800000;
	} else {
		printk("%s: part8 not found\n", __func__);
	}

	if (p11 != NULL) {
		u32 p11_len = 0;

		// move p11 to the end of p8
		p11[1] = ((p2_len + p9_len + p8_len) << 20) + 0x88800000;
		p11[2] = p11_len << 20;
		((unsigned int *)(p11[4]))[5] = (p11_len << 21) | 0xFC;
	} else {
//		printk("%s: part11 not found\n", __func__);
	}

	unlock_high_memory();
}
