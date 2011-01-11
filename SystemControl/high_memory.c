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

static int forced_umdcache_kill = 0;

//prevent umd-cache in homebrew, so we can drain the cache partition.
void patch_umdcache(u32 text_addr)
{
	if(forced_umdcache_kill || (sceKernelInitKeyConfig() == PSP_INIT_KEYCONFIG_GAME && sceKernelBootFrom() == PSP_BOOT_MS)) {
		//kill module start
		_sw(0x03E00008, text_addr + 0x9C8);
		_sw(0x24020001, text_addr + 0x9CC);
	}
}

void unlock_high_memory(void)
{
	//unlock memory
	unsigned int i = 0; for(; i < 0x40; i += 4) {
		_sw(0xFFFFFFFF, 0xBC000040 + i);
	}
}

//it's partition 9 on 6.31... its smaller than 5.X one too... only 24MB instead of 28MB...
void patch_partitions(void) 
{
	int apitype;

	//system memory manager
	unsigned int * (*GetPartition)(int pid) = (void *)(0x88003E34);

	//get partition 2
	unsigned int * p2 = GetPartition(2);

	//get partition 9
	unsigned int * p9 = GetPartition(9);

	//get new length
	u32 p2_len = p2_size;
	u32 p9_len = p8_size;

	//grab executable name
	char * path = sceKernelInitFileName();

	apitype = sceKernelInitApitype();

	//set highmemory for homebrew eboots, it doesn't hurt.
	if ((apitype == PSP_INIT_APITYPE_MS2 || apitype == 0x152) && p2_size == 24
			&& p8_size == 24 && path && strlen(path) >= 25 && strncmp(path + 2,
				"0:/PSP/GAME/", 12) == 0 && strcmp(path + strlen(path) - 3, "PBP") == 0) {
		//override memory settings
		p2_len = 48;
		p9_len = 0;
	}

	//force umdcache kill for disc0 reboots
	if(p2_len != 24) forced_umdcache_kill = 1;

	//reset saved length
	p2_size = 24;
	p8_size = 24;

	//valid partition size
	if(p2_len > 24 && (p2_len + p9_len) == 48 && p9) {
		//resize partition 2
		p2[2] = p2_len << 20;
		((unsigned int *)(p2[4]))[5] = (p2_len << 21) | 0xFC;

		//resize partition 9
		p9[1] = (p2_len << 20) + 0x88800000;
		p9[2] = p9_len << 20;
		((unsigned int *)(p9[4]))[5] = (p9_len << 21) | 0xFC;
	}

	unlock_high_memory();
}
