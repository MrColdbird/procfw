#include <pspsdk.h>
#include <pspkernel.h>
#include <pspthreadman_kernel.h>
#include <psputilsforkernel.h>
#include <pspsysevent.h>
#include <pspiofilemgr.h>
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "utils.h"
#include "systemctrl.h"
#include "printk.h"
#include "nid_resolver.h"

extern int LoadExecForKernel_5AA1A6D2(struct SceKernelLoadExecVSHParam *param);
extern int LoadExecForKernel_45C6125B(const char *file, struct SceKernelLoadExecVSHParam *param);
extern int LoadExecForKernel_179D905A(const char *file, struct SceKernelLoadExecVSHParam *param);
extern int LoadExecForKernel_7286CF0B(const char *file, struct SceKernelLoadExecVSHParam *param);
extern int LoadExecForKernel_3D805DE6(const char *file, struct SceKernelLoadExecVSHParam *param);
extern int LoadExecForKernel_BAEB4B89(const char *file, struct SceKernelLoadExecVSHParam *param);
extern int LoadExecForKernel_8EF38192(const char *file, struct SceKernelLoadExecVSHParam *param);
extern int LoadExecForKernel_D35D6403(const char *file, struct SceKernelLoadExecVSHParam *param);

extern int (*g_on_module_start)(SceModule2*);

// for sctrlHENLoadModuleOnReboot
char *g_insert_module_before;
void *g_insert_module_binary;
int g_insert_module_size;
int g_insert_module_flags;

// for sctrlHENSetMemory
u32 p2_size = 24;
u32 p8_size = 24;

int sctrlKernelExitVSH(struct SceKernelLoadExecVSHParam *param)
{
	u32 k1;
	int ret;

	k1 = pspSdkSetK1(0);
	ret = LoadExecForKernel_5AA1A6D2(param);
	pspSdkSetK1(k1);

	return ret;
}

int sctrlKernelLoadExecVSHDisc(const char *file, struct SceKernelLoadExecVSHParam *param)
{
	u32 k1;
	int ret;

	k1 = pspSdkSetK1(0);
	ret = LoadExecForKernel_45C6125B(file, param);
	pspSdkSetK1(k1);

	return ret;
}

int sctrlKernelLoadExecVSHDiscUpdater(const char *file, struct SceKernelLoadExecVSHParam *param)
{
	u32 k1;
	int ret;

	k1 = pspSdkSetK1(0);
	ret = LoadExecForKernel_179D905A(file, param);
	pspSdkSetK1(k1);

	return ret;
}

int sctrlKernelLoadExecVSHMs1(const char *file, struct SceKernelLoadExecVSHParam *param)
{
	u32 k1;
	int ret;

	k1 = pspSdkSetK1(0);
	ret = LoadExecForKernel_7286CF0B(file, param);
	pspSdkSetK1(k1);

	return ret;
}

int sctrlKernelLoadExecVSHMs2(const char *file, struct SceKernelLoadExecVSHParam *param)
{
	u32 k1;
	int ret;

	k1 = pspSdkSetK1(0);
	ret = LoadExecForKernel_3D805DE6(file, param);
	pspSdkSetK1(k1);

	return ret;
}

int sctrlKernelLoadExecVSHEf2(const char *file, struct SceKernelLoadExecVSHParam *param)
{
	u32 k1;
	int ret;

	k1 = pspSdkSetK1(0);
	ret = LoadExecForKernel_D35D6403(file, param);
	pspSdkSetK1(k1);

	return ret;
}

int sctrlKernelLoadExecVSHMs3(const char *file, struct SceKernelLoadExecVSHParam *param)
{
	u32 k1;
	int ret;

	k1 = pspSdkSetK1(0);
	ret = LoadExecForKernel_BAEB4B89(file, param);
	pspSdkSetK1(k1);

	return ret;
}

int sctrlKernelLoadExecVSHMs4(const char *file, struct SceKernelLoadExecVSHParam *param)
{
	u32 k1;
	int ret;

	k1 = pspSdkSetK1(0);
	ret = LoadExecForKernel_8EF38192(file, param);
	pspSdkSetK1(k1);

	return ret;
}

int sctrlKernelLoadExecVSHWithApitype(int apitype, const char *file, struct SceKernelLoadExecVSHParam *param)
{
	u32 k1;
	int ret;
	SceModule2 *mod;
	u32 text_addr;
	int (*_sctrlKernelLoadExecVSHWithApitype)(int apitype, const char *file, struct SceKernelLoadExecVSHParam *param, u32 unk);

	k1 = pspSdkSetK1(0);
	mod = (SceModule2*) sceKernelFindModuleByName("sceLoadExec");
	text_addr = mod->text_addr;

	if (psp_model == 4) {
		_sctrlKernelLoadExecVSHWithApitype = (void*)(text_addr + 0x25C0); // 0x2558 in 6.20
	} else {
		_sctrlKernelLoadExecVSHWithApitype = (void*)(text_addr + 0x236C); // 0x2304 in 6.20
	}

	ret = _sctrlKernelLoadExecVSHWithApitype(apitype, file, param, 0x10000);

	pspSdkSetK1(k1);

	return ret;
}

int sctrlKernelSetUserLevel(int level)
{
	u32 k1;
	int ret;
	SceModule2 *mod;
	u32 text_addr;

	k1 = pspSdkSetK1(0);
	ret = sceKernelGetUserLevel();
	mod = (SceModule2*) sceKernelFindModuleByName("sceThreadManager");
	text_addr = mod->text_addr;
	_sw((level^8)<<28, *(u32*)(text_addr+0x19E80)+0x14); // 0x19E80 and 0x14 in 6.20, 6.31 remains the same

	pspSdkSetK1(k1);

	return ret;
}

int sctrlKernelSetDevkitVersion(int version)
{
	u32 k1;
	int ret;

	k1 = pspSdkSetK1(0);
	ret = sceKernelDevkitVersion();

	_sh((version>>16), 0x88011AB8); // 0x88011AAC in 6.20
	_sh((version&0xFFFF), 0x88011AC0); // 0x88011AB4 in 6.20

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
	return 0x1001; // 0x1000 in TN's code :)
}

PspIoDrv *sctrlHENFindDriver(char *drvname)
{
	u32 k1;
	int *p;
	SceModule2 *mod;
	u32 text_addr;
	int* (*find_driver)(char *drvname);

	k1 = pspSdkSetK1(0);
	mod = (SceModule2*) sceKernelFindModuleByName("sceIOFileManager");
	text_addr = mod->text_addr;
	find_driver = (void*)(text_addr + 0x2A38); // 0x2A38 in 6.20, 6.31 remains the same
	p = find_driver(drvname);

	if (p != NULL) {
		p = (int*)(p[1]);
	}

	pspSdkSetK1(k1);

	return (PspIoDrv*) p;
}

u32 sctrlHENFindFunction(char* szMod, char* szLib, u32 nid)
{
	struct SceLibraryEntryTable *entry;
	SceModule *pMod;
	void *entTab;
	int entLen;

	//nid resolve arguments
	nid = resolve_nid(szMod, nid);

	pMod = sceKernelFindModuleByName(szMod);

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

void sctrlHENLoadModuleOnReboot(char *module_before, void *buf, int size, int flags)
{
	g_insert_module_before = module_before;
	g_insert_module_binary = buf;
	g_insert_module_size = size;
	g_insert_module_flags = flags;
}

// SystemCtrlForKernel_826668E9 in Tn's code
// Look out syscall struct changed in 6XX kernel!
void sctrlHENPatchSyscall(u32 addr, void *newaddr)
{
	void *ptr;
	u32 *syscalls;
	int i;

	// get syscall struct from cop0
	asm("cfc0 %0, $12\n" : "=r"(ptr));

	if (ptr == NULL) {
		return;
	}

	syscalls = (u32*)(ptr+0x10);

	for(i=0; i<0xFF4; ++i) {
		if (syscalls[i] == addr) {
			syscalls[i] = (u32)newaddr;
		}
	}
}

int sctrlHENSetMemory(u32 p2, u32 p8)
{
	if(p2 > 24 && (p2 + p8) == 48) {
		p2_size = p2;
		p8_size = p8;
	}

	return 0;
}
