/*
 * This file is part of PRO CFW.

 * PRO CFW is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * PRO CFW is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PRO CFW. If not, see <http://www.gnu.org/licenses/ .
 */

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

extern u32 sceKernelGetModel_620(void);
extern u32 sceKernelDevkitVersion_620(void);
extern u32 sceKernelDevkitVersion_620(void);
extern SceModule* sceKernelFindModuleByName_620(char *modname);
extern int sceKernelExitVSH(struct SceKernelLoadExecVSHParam *param);
extern int sceKernelExitVSH_620(struct SceKernelLoadExecVSHParam *param);
extern u32 sceKernelQuerySystemCall(void *func);
extern u32 sceKernelQuerySystemCall_620(void *func);
extern SceModule* sceKernelFindModuleByUID_620(SceUID modid);
extern int sceKernelCheckExecFile(unsigned char * buffer, int * check);
extern int sceKernelCheckExecFile_620(unsigned char * buffer, int * check);	
extern int sceKernelLoadModule_620(const char *path, int flags, SceKernelLMOption *option);
extern int sceKernelStartModule_620(SceUID modid, SceSize argsize, void *argp, int *status, SceKernelSMOption *option);

extern u32 psp_fw_version;
extern int (*g_on_module_start)(SceModule2*);

int sctrlKernelExitVSH(struct SceKernelLoadExecVSHParam *param)
{
	u32 k1;
	int ret = -1;

	k1 = pspSdkSetK1(0);

	switch(psp_fw_version) {
#ifdef CONFIG_639
		case FW_639:
			ret = sceKernelExitVSH(param);
			break;
#endif
#ifdef CONFIG_635
		case FW_635:
			ret = sceKernelExitVSH(param);
			break;
#endif
#ifdef CONFIG_620
		case FW_620:
			ret = sceKernelExitVSH_620(param);
			break;
#endif
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
#ifdef CONFIG_639
		case FW_639:
			model = sceKernelGetModel();
			break;
#endif
#ifdef CONFIG_635
		case FW_635:
			model = sceKernelGetModel();
			break;
#endif
#ifdef CONFIG_620
		case FW_620:
			model = sceKernelGetModel_620();
			break;
#endif
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
#ifdef CONFIG_639
		case FW_639:
			mod = sceKernelFindModuleByName(modname);
			break;
#endif
#ifdef CONFIG_635
		case FW_635:
			mod = sceKernelFindModuleByName(modname);
			break;
#endif
#ifdef CONFIG_620
		case FW_620:
			mod = sceKernelFindModuleByName_620(modname);
			break;
#endif
	};

	return mod;
}

int sctrlKernelQuerySystemCall(void *func_addr)
{
	int ret = -1;
	u32 k1;

	k1 = pspSdkSetK1(0);

	switch(psp_fw_version) {
#ifdef CONFIG_639
		case FW_639:
			ret = sceKernelQuerySystemCall(func_addr);
			break;
#endif
#ifdef CONFIG_635
		case FW_635:
			ret = sceKernelQuerySystemCall(func_addr);
			break;
#endif
#ifdef CONFIG_620
		case FW_620:
			ret = sceKernelQuerySystemCall_620(func_addr);
			break;
#endif
	};

	pspSdkSetK1(k1);

	return ret;
}

int sctrlKernelCheckExecFile(unsigned char * buffer, int * check)
{
	int ret = -1;

	switch(psp_fw_version) {
#ifdef CONFIG_639
		case FW_639:
			ret = sceKernelCheckExecFile(buffer, check);
			break;
#endif
#ifdef CONFIG_635
		case FW_635:
			ret = sceKernelCheckExecFile(buffer, check);
			break;
#endif
#ifdef CONFIG_620
		case FW_620:
			ret = sceKernelCheckExecFile_620(buffer, check);
			break;
#endif
	}

	return ret;
}

int sctrlKernelLoadModule(const char *path, int flags, SceKernelLMOption *option)
{
	int ret = -1;

	switch(psp_fw_version) {
#ifdef CONFIG_639
		case FW_639:
			ret = sceKernelLoadModule(path, flags, option);
			break;
#endif
#ifdef CONFIG_635
		case FW_635:
			ret = sceKernelLoadModule(path, flags, option);
			break;
#endif
#ifdef CONFIG_620
		case FW_620:
			ret = sceKernelLoadModule_620(path, flags, option);
			break;
#endif
	}
	
	return ret;
}

int sctrlKernelStartModule(SceUID modid, SceSize argsize, void *argp, int *status, SceKernelSMOption *option)
{
	int ret = -1;

	switch(psp_fw_version) {
#ifdef CONFIG_639
		case FW_639:
			ret = sceKernelStartModule(modid, argsize, argp, status, option);
			break;
#endif
#ifdef CONFIG_635
		case FW_635:
			ret = sceKernelStartModule(modid, argsize, argp, status, option);
			break;
#endif
#ifdef CONFIG_620
		case FW_620:
			ret = sceKernelStartModule_620(modid, argsize, argp, status, option);
			break;
#endif
	}
	
	return ret;
}
