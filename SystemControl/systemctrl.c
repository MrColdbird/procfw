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
#include <pspsysmem_kernel.h>
#include <pspcrypt.h>
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "utils.h"
#include "systemctrl.h"
#include "systemctrl_se.h"
#include "printk.h"
#include "nid_resolver.h"
#include "strsafe.h"
#include "systemctrl_patch_offset.h"
#include "rebootex_conf.h"

extern int *InitForKernel_040C934B(void);
extern int sceKernelExitVSH(struct SceKernelLoadExecVSHParam *param);
extern int sceKernelLoadExecVSHEf2(const char *file, struct SceKernelLoadExecVSHParam *param);
extern int sceKernelLoadExecVSHMs3(const char *file, struct SceKernelLoadExecVSHParam *param);
extern int sceKernelGetSystemStatus(void);
extern u32 sceKernelQuerySystemCall(void *func);
extern int sceKernelCheckExecFile(unsigned char * buffer, int * check);
extern SceUID _sceKernelLoadModuleWithApitype2(int apitype, const char *path, int flags, SceKernelLMOption *option);
extern int sceKernelBootFromGo_635(void);
void* sceKernelGetBlockHeadAddr(SceUID blockid);
SceUID sceKernelAllocPartitionMemory(SceUID partitionid, const char * name, int type, SceSize size, void * addr);

extern int (*g_on_module_start)(SceModule2*);

// for sctrlHENSetMemory
u32 g_p2_size = 24;
u32 g_p9_size = 24;

static char g_initfilename[80];
static char g_iso_filename[128];

int sctrlKernelSetUserLevel(int level)
{
	u32 k1;
	int ret;
	SceModule2 *mod;
	u32 text_addr;

	k1 = pspSdkSetK1(0);
	ret = sceKernelGetUserLevel();
	mod = (SceModule2*) sctrlKernelFindModuleByName("sceThreadManager");
	text_addr = mod->text_addr;
	_sw((level^8)<<28, *(u32*)(text_addr+g_offs->threadmgr_patch.sctrlKernelSetUserLevel)+0x14); // 0x00019E80 and 0x14 in 6.20, 6.31 remains the same

	pspSdkSetK1(k1);

	return ret;
}

int sctrlKernelSetDevkitVersion(int version)
{
	u32 k1;
	int ret;

	k1 = pspSdkSetK1(0);
	ret = sceKernelDevkitVersion();

	_sh((version>>16), SYSMEM_TEXT_ADDR + g_offs->sysmem_patch.sctrlKernelSetDevkitVersion); // 0x88011AAC in 6.20
	_sh((version&0xFFFF), SYSMEM_TEXT_ADDR + g_offs->sysmem_patch.sctrlKernelSetDevkitVersion + 8); // 0x88011AB4 in 6.20

	sync_cache();
	pspSdkSetK1(k1);

	return ret;
}

int	sctrlHENIsSE()
{
	return 1;
}

int	sctrlHENIsDevhook()
{
	return 0;
}

int sctrlHENGetVersion()
{
	return 0x1003; // 0x1000 in TN's code :)
}

int sctrlHENGetMinorVersion()
{
	return 0;
}

PspIoDrv *sctrlHENFindDriver(char *drvname)
{
	u32 k1;
	int *p;
	SceModule2 *mod;
	int* (*find_driver)(char *drvname);

	k1 = pspSdkSetK1(0);
	mod = (SceModule2*) sctrlKernelFindModuleByName("sceIOFileManager");
	find_driver = (void*)(mod->text_addr + g_offs->iofilemgr_patch.sctrlHENFindDriver); // 0x00002A38 in 6.20/6.31
	p = find_driver(drvname);

	if (p != NULL) {
		p = (int*)(p[1]);
	}

	pspSdkSetK1(k1);

	if(psp_model == PSP_GO && p == NULL) {
		if(0 == stricmp(drvname, "msstor")) {
			return sctrlHENFindDriver("eflash0a0f");
		}

		if(0 == stricmp(drvname, "msstor0p")) {
			return sctrlHENFindDriver("eflash0a0f1p");
		}
	}

	return (PspIoDrv*) p;
}

u32 sctrlHENFindFunction(char* szMod, char* szLib, u32 nid)
{
	struct SceLibraryEntryTable *entry;
	SceModule *pMod;
	void *entTab;
	int entLen;
	resolver_config *resolver;

	resolver = get_nid_resolver(szLib);

	if(resolver != NULL) {
		nid = resolve_nid(resolver, nid);
	}

	pMod = sctrlKernelFindModuleByName(szMod);

	if(!pMod) {
		pMod = sctrlKernelFindModuleByAddress((u32)szMod);

		if (!pMod) {
			printk("%s: Cannot find %s_%08X\n", __func__, szLib == NULL ? "syslib" : szLib, (uint)nid);

			return 0;
		}
	}

	int i = 0;

	entTab = pMod->ent_top;
	entLen = pMod->ent_size;

	while(i < entLen) {
		int count;
		int total;
		unsigned int *vars;

		entry = (struct SceLibraryEntryTable *) (entTab + i);

		if(entry->libname == szLib || (entry->libname && szLib && 0 == strcmp(entry->libname, szLib))) {
			total = entry->stubcount + entry->vstubcount;
			vars = entry->entrytable;

			if(total > 0) {
				for(count=0; count<total; count++) {
					if (vars[count] == nid)
						return vars[count+total];
				}
			}
		}

		i += (entry->len * 4);
	}

	printk("%s: Cannot find %s_%08X\n", __func__, szLib == NULL ? "syslib" : szLib, (uint)nid);
	
	return 0;
}

u32 FindProc(char* szMod, char* szLib, u32 nid) __attribute__((alias("sctrlHENFindFunction")));

STMOD_HANDLER sctrlHENSetStartModuleHandler(STMOD_HANDLER new_handler)
{
	STMOD_HANDLER on_module_start;

	on_module_start = g_on_module_start;
	g_on_module_start = (void*)(((u32)new_handler) | 0x80000000);

	return on_module_start;
}

void sctrlHENLoadModuleOnReboot(char *module_before, void *buf, int size, int flags)
{
	rebootex_conf.insert_module_before = module_before;
	rebootex_conf.insert_module_binary = buf;
	rebootex_conf.insert_module_size = size;
	rebootex_conf.insert_module_flags = flags;
}

// SystemCtrlForKernel_826668E9 in Tn's code
// Look out syscall struct changed in 6XX kernel!
void sctrlHENPatchSyscall(void *addr, void *newaddr)
{
	void *ptr;
	u32 *syscalls;
	int i;
	u32 _addr = (u32)addr;

	// get syscall struct from cop0
	asm("cfc0 %0, $12\n" : "=r"(ptr));

	if (ptr == NULL) {
		return;
	}

	syscalls = (u32*)(ptr+0x10);

	for(i=0; i<0xFF4; ++i) {
		if ((syscalls[i] & 0x0FFFFFFF) == (_addr & 0x0FFFFFFF)) {
			syscalls[i] = (u32)newaddr;
		}
	}
}

int sctrlHENSetMemory(u32 p2, u32 p9)
{
	if(p2 != 0 && (p2 + p9) <= MAX_HIGH_MEMSIZE) {
		g_p2_size = p2;
		g_p9_size = p9;
	}

	return 0;
}

int sctrlKernelSetInitApitype(int apitype)
{
	int prev_apitype;
	int *p;
	
	p = InitForKernel_040C934B();
	prev_apitype = *p;
	*p = apitype;

	return prev_apitype;
}

int sctrlKernelSetUMDEmuFile(const char *iso)
{
	SceModule2 *modmgr = (SceModule2*)sctrlKernelFindModuleByName("sceModuleManager");

	if (modmgr == NULL) {
		return -1;
	}

	STRCPY_S(g_iso_filename, iso);
	*(const char**)(modmgr->text_addr+g_offs->modulemgr_patch.sctrlKernelSetUMDEmuFile) = g_iso_filename;

	return 0;
}

int sctrlKernelSetInitFileName(char *filename)
{
	SceModule2 *modmgr;

	modmgr = (SceModule2*)sctrlKernelFindModuleByName("sceModuleManager");

	if(modmgr == NULL) {
		return -1;
	}

	STRCPY_S(g_initfilename, filename);
	*(const char**)(modmgr->text_addr+g_offs->modulemgr_patch.sctrlKernelSetInitFileName) = g_initfilename;

	return 0;
}

int sctrlPatchModule(char *modname, u32 inst, u32 offset)
{
	u32 k1;
	SceModule2 *mod;
	int ret;

	k1 = pspSdkSetK1(0);
	mod = (SceModule2*) sctrlKernelFindModuleByName(modname);

	if(mod != NULL) {
		_sw(inst, mod->text_addr + offset);
		sync_cache();
		ret = 0;
	} else {
		ret = -1;
	}

	pspSdkSetK1(k1);
	
	return ret;
}

u32 sctrlModuleTextAddr(char *modname)
{
	u32 k1, text_addr;
	SceModule2 *mod;

	k1 = pspSdkSetK1(0);
	mod = (SceModule2*) sctrlKernelFindModuleByName(modname);
	text_addr = 0;

	if(mod != NULL) {
		text_addr = mod->text_addr;
	}

	pspSdkSetK1(k1);
	
	return text_addr;
}

void sctrlSESetDiscType(int type)
{
	rebootex_conf.iso_disc_type = type;
}

int sctrlSEGetDiscType(void)
{
	return rebootex_conf.iso_disc_type;
}

u32 sctrlKernelRand(void)
{
	u32 k1, result;
	u8 *alloc, *ptr;

	enum {
		KIRK_PRNG_CMD=0xE,
	};

	k1 = pspSdkSetK1(0);

	alloc = oe_malloc(20 + 4);

	if(alloc == NULL) {
		asm("break");
	}

	/* output ptr has to be 4 bytes aligned */
	ptr = (void*)(((u32)alloc & (~(4-1))) + 4);
	sceUtilsBufferCopyWithRange(ptr, 20, NULL, 0, KIRK_PRNG_CMD);
	result = *(u32*)ptr;
	oe_free(alloc);
	pspSdkSetK1(k1);

	return result;
}

int sctrlKernelLoadExecVSHWithApitype(int apitype, const char *file, struct SceKernelLoadExecVSHParam *param)
{
	u32 k1;
	int ret;
	SceModule2 *mod;
	u32 text_addr;
	int (*_sctrlKernelLoadExecVSHWithApitype)(int apitype, const char *file, struct SceKernelLoadExecVSHParam *param, u32 unk);

	k1 = pspSdkSetK1(0);
	mod = (SceModule2*) sctrlKernelFindModuleByName("sceLoadExec");
	text_addr = mod->text_addr;

	if (psp_model == PSP_GO) {
		_sctrlKernelLoadExecVSHWithApitype = (void*)(text_addr + g_offs->loadexec_patch_05g.sctrlKernelLoadExecVSHWithApitype);
	} else {
		_sctrlKernelLoadExecVSHWithApitype = (void*)(text_addr + g_offs->loadexec_patch_other.sctrlKernelLoadExecVSHWithApitype);
	}

	ret = _sctrlKernelLoadExecVSHWithApitype(apitype, file, param, 0x10000);

	pspSdkSetK1(k1);

	return ret;
}
