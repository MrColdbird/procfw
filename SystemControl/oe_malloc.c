#include <pspsdk.h>
#include <pspkernel.h>
#include <pspinit.h>
#include <pspsysmem_kernel.h>
#include <malloc.h>
#include "utils.h"
#include "printk.h"

static SceUID heapid = -1;

int oe_mallocinit()
{
	int size;
	int key_config;
   
	key_config = sceKernelInitKeyConfig();

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
	
	heapid = sceKernelCreateHeap(PSP_MEMORY_PARTITION_KERNEL, size, 1, "SystemCtrlHeap");

	printk("%s: 0x%08X heap size %d\n", __func__, heapid, size);

	return (heapid < 0) ? heapid : 0;
}

void *oe_malloc(size_t size)
{
	void *p;

	p = sceKernelAllocHeapMemory(heapid, size);
	printk("%s: %d@0x%08X\n", __func__, size, (u32)p);

	return p;
}

void oe_free(void *p)
{
	sceKernelFreeHeapMemory(heapid, p);
}

int oe_mallocterminate()
{
	return sceKernelDeleteHeap(heapid);
}

