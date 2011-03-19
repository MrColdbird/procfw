#ifndef MAIN_H
#define MAIN_H

#include <pspsysmem_kernel.h>
#include "systemctrl.h"
#include "systemctrl_se.h"
#include "systemctrl_private.h"

extern u32 psp_model;
extern u32 psp_fw_version;

int PatchExec1(unsigned char * buffer, int * check);
int PatchExec2(unsigned char * buffer, int * check);
int PatchExec3(unsigned char * buffer, int * check, int isplain, int checkresult);
int LoadCoreForKernel_01DB1EB3(unsigned char * buffer, int * check);

int _ProbeExec1(unsigned char * buffer, int * check);
int _ProbeExec2(unsigned char * buffer, int * check);
int _ProbeExec3(unsigned char * buffer, int * check); //GO ONLY

extern int (* ProbeExec3)(unsigned char * buffer, int * check); //GO ONLY
int _sceKernelCheckExecFile(unsigned char * buffer, int * check);

extern int (* PartitionCheck)(unsigned int * st0, unsigned int * check);
int _PartitionCheck(unsigned int * st0, unsigned int * check);

void syspatch_init();
void setup_module_handler(void);
void patch_sceLoaderCore(void);
void patch_sceLoadExec(void);
void load_rebootex_config(void);
void patch_sceMemlmd(void);
void patch_sceInterruptManager(void);
void patch_sceSystemMemoryManager(void);
void patch_sceKernelStartModule(u32 loadcore_text_addr);

#ifdef DEBUG
void validate_stub(SceModule *pMod);
void validate_stub_by_uid(int modid);
void setup_validate_stub(SceModule *mod);
#endif

void patch_umdcache(u32 text_addr);
void prepatch_partitions(void);
void patch_partitions(void);
void unlock_high_memory(u32 forced);

void patch_mesgled(SceModule* mod);
void patch_npsignup(u32 text_addr);
void patch_npsignin(u32 text_addr);
void patch_np(u32 text_addr, u8 mayor, u8 minor);

void patch_sceMediaSync(u32 text_addr);
void patch_pspMarch33_Driver(u32 text_addr);

int load_plugins(void);
int load_start_module(char *path);

void usb_charge(void);

#define MAX_HIGH_MEMSIZE 55

extern u32 g_p2_size;
extern u32 g_p9_size;
extern int g_high_memory_enabled;

int GetConfig(SEConfig *config);
int SetConfig(SEConfig *config);
void load_config(void);

void patch_module_for_version_spoof(SceModule *mod);

void patch_sceChkreg(void);

// Have to use these functions to support both 6.2/6.3 kernel
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

extern SEConfig conf;

#endif
