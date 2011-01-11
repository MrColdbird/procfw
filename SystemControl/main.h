#ifndef MAIN_H
#define MAIN_H

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
void get_iso_status_from_rebootex(void);
void patch_sceMemlmd(void);
void patch_sceInterruptManager(void);
void resolve_sceKernelIcacheClearAll(SceModule *pMod);

#ifdef DEBUG
void validate_stub(SceModule *pMod);
void validate_stub_by_uid(int modid);
void setup_validate_stub(SceModule *mod);
#endif

#endif
