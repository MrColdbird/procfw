#pragma once 

#include "printk.h"

// for 6.20 functions
extern u32 sceKernelDevkitVersion630(void);

SceUID sceKernelCreateHeap630(SceUID partitionid, SceSize size, int unk, const char *name);
void *sceKernelAllocHeapMemory630(SceUID heapid, SceSize size);
int sceKernelFreeHeapMemory630(SceUID heapid, void *block);
int sceKernelDeleteHeap630(SceUID heapid);

SceModule* sceKernelFindModuleByName630(const char * modname);
SceModule* sceKernelFindModuleByUID630(SceUID modid);
SceModule* sceKernelFindModuleByUID630(SceUID modid);
SceUID sceKernelLoadModuleDisc630(const char *path, int flags, SceKernelLMOption *option);

SceUID sceKernelLoadModule630(const char *path, int flags, SceKernelLMOption *option);
int sceKernelStartModule630(SceUID modid, SceSize argsize, void *argp, int *status, SceKernelSMOption *option);
int sceKernelStopModule630(SceUID modid, SceSize argsize, void *argp, int *status, SceKernelSMOption *option);
int sceKernelUnloadModule630(SceUID modid) ;

// sceKernelAllocPartitionMemory630 for kernel
SceUID SysMemForKernel_4621A9CC (SceUID partitionid, const char *name, int type, SceSize size, void *addr);

// sceKernelFreePartitionMemory630 for kernel
void* SysMemForKernel_52B54B93(SceUID blockid);

extern u32 g_fw_version;

#define FW_IS_631 (g_fw_version == 0x06030010)
#define CHECK_FUNCTION(func) check_function(func, #func)

static inline void check_function(void *func_addr, char *func_name)
{
	if (*(u32*)func_addr == 0x0000054C)
		printk("No %s!\r\n", func_name);
	else
		printk("Have %s.\r\n", func_name);
}

