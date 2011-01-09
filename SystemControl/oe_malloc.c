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
	
	heapid = sceKernelCreateHeap(PSP_MEMORY_PARTITION_KERNEL, size, 1, "SystemCtrlHeap");

	return (heapid < 0) ? heapid : 0;
}

void *oe_malloc(size_t size)
{
	return sceKernelAllocHeapMemory(heapid, size);
}

void oe_free(void *p)
{
	sceKernelFreeHeapMemory(heapid, p);
}

int oe_mallocterminate()
{
	return sceKernelDeleteHeap(heapid);
}

