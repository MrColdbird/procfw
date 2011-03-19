#include <pspsdk.h>
#include <pspkernel.h>
#include <pspthreadman_kernel.h>
#include <psputilsforkernel.h>
#include <pspsysevent.h>
#include <pspiofilemgr.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "systemctrl.h"
#include "modhandler.h"
#include "printk.h"
#include "syspatch.h"

#define FW_635 0x06030510
#define FW_620 0x06020010

extern u32 sceKernelGetModel_620(void);
extern u32 sceKernelDevkitVersion_620(void);
extern u32 sceKernelDevkitVersion_620(void);
extern SceModule* sceKernelFindModuleByName_620(char *modname);
extern int sceKernelExitVSH(struct SceKernelLoadExecVSHParam *param);
extern int sceKernelExitVSH_620(struct SceKernelLoadExecVSHParam *param);
extern u32 sceKernelQuerySystemCall(void *func);
extern u32 sceKernelQuerySystemCall_620(void *func);
extern SceModule* sceKernelFindModuleByUID_620(SceUID modid);

extern u32 psp_fw_version;
extern int (*g_on_module_start)(SceModule2*);

int sctrlKernelExitVSH(struct SceKernelLoadExecVSHParam *param)
{
	u32 k1;
	int ret = -1;

	k1 = pspSdkSetK1(0);

	switch(psp_fw_version) {
		case FW_635:
			ret = sceKernelExitVSH(param);
			break;
		case FW_620:
			ret = sceKernelExitVSH_620(param);
			break;
	};
	
	pspSdkSetK1(k1);

	return ret;
}

u32 sctrlHENFindFunction(char* szMod, char* szLib, u32 nid)
{
	struct SceLibraryEntryTable *entry;
	SceModule *pMod;
	void *entTab;
	int entLen;

	pMod = sctrlKernelFindModuleByName(szMod);

	if (!pMod)
	{
		Kprintf("Cannot find module %s\n", szMod);
		return 0;
	}

	int i = 0;

	entTab = pMod->ent_top;
	entLen = pMod->ent_size;

	while(i < entLen)
	{
		int count;
		int total;
		unsigned int *vars;

		entry = (struct SceLibraryEntryTable *) (entTab + i);

		if(entry->libname == szLib || (entry->libname && !strcmp(entry->libname, szLib)))
		{
			total = entry->stubcount + entry->vstubcount;
			vars = entry->entrytable;

			if(entry->stubcount > 0)
			{
				for(count = 0; count < entry->stubcount; count++)
				{
					if (vars[count] == nid)
						return vars[count+total];					
				}
			}
		}

		i += (entry->len * 4);
	}

	return 0;
}

STMOD_HANDLER sctrlHENSetStartModuleHandler(STMOD_HANDLER new_handler)
{
	STMOD_HANDLER on_module_start;

	on_module_start = g_on_module_start;
	g_on_module_start = (void*)(((u32)new_handler) | 0x80000000);

	return on_module_start;
}

u32 sctrlKernelGetModel(void)
{
	u32 model = -1;

	switch(psp_fw_version) {
		case FW_635:
			model = sceKernelGetModel();
			break;
		case FW_620:
			model = sceKernelGetModel_620();
			break;
	};
   
	return model;
}

u32 sctrlKernelDevkitVersion(void)
{
	u32 fw_version;
   
	fw_version = sceKernelDevkitVersion_620();

	if(fw_version == 0x8002013A) {
		fw_version = sceKernelDevkitVersion();
	}

	return fw_version;
}

SceModule* sctrlKernelFindModuleByName(char *modname)
{
	SceModule *mod = NULL;

	switch(psp_fw_version) {
		case FW_635:
			mod = sceKernelFindModuleByName(modname);
			break;
		case FW_620:
			mod = sceKernelFindModuleByName_620(modname);
			break;
	};

	return mod;
}

int sctrlKernelQuerySystemCall(void *func_addr)
{
	int ret = -1;
	u32 k1;

	k1 = pspSdkSetK1(0);

	switch(psp_fw_version) {
		case FW_635:
			ret = sceKernelQuerySystemCall(func_addr);
			break;
		case FW_620:
			ret = sceKernelQuerySystemCall_620(func_addr);
			break;
	};

	pspSdkSetK1(k1);

	return ret;
}

int sctrlKernelCheckExecFile(unsigned char * buffer, int * check)
{
	int ret = -1;

	switch(psp_fw_version) {
		case FW_635:
			ret = sceKernelCheckExecFile(buffer, check);
			break;
		case FW_620:
			ret = sceKernelCheckExecFile_620(buffer, check);
			break;
	}

	return ret;
}
