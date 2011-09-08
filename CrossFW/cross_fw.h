#ifndef CROSS_FW_H
#define CROSS_FW_H

#include <pspsysmem_kernel.h>
#include <pspctrl.h>

u32 sctrlKernelGetModel(void);
u32 sctrlKernelDevkitVersion(void);
SceModule* sctrlKernelFindModuleByName(char *modname);
int sctrlKernelSetDdrMemoryProtection(void *addr, int size, int prot);
SceUID sctrlKernelCreateHeap(SceUID partitionid, SceSize size, int unk, const char *name);
int sctrlKernelDeleteHeap(SceUID heapid);
int sctrlKernelFreeHeapMemory(SceUID heapid, void *block);
void* sctrlKernelAllocHeapMemory(SceUID heapid, SceSize size);
int sctrlKernelGetSystemStatus(void);
int sctrlKernelQueryMemoryPartitionInfo(int pid, PspSysmemPartitionInfo *info);
int sctrlKernelPartitionMaxFreeMemSize(int pid);
int sctrlKernelPartitionTotalFreeMemSize(int pid);
SceModule* sctrlKernelFindModuleByUID(SceUID modid);
SceModule* sctrlKernelFindModuleByAddress(u32 address);
int sctrlKernelCheckExecFile(unsigned char * buffer, int * check);
int sctrlKernelLoadModule(const char *path, int flags, SceKernelLMOption *option);
int sctrlKernelStartModule(SceUID modid, SceSize argsize, void *argp, int *status, SceKernelSMOption *option);
int sctrlKernelUnloadModule(SceUID modid);
SceUID sctrlKernelLoadModuleWithApitype2(int apitype, const char *path, int flags, SceKernelLMOption *option);
int sceKernelApplicationType(void);
SceUID sctrlKernelAllocPartitionMemory(SceUID partitionid, const char * name, int type, SceSize size, void * addr);
void* sctrlKernelGetBlockHeadAddr(SceUID blockid);

int sctrlReadBufferPositive(SceCtrlData *pad_data, int count);

#endif
