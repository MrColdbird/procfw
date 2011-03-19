#include <pspsdk.h>
#include <pspkernel.h>
#include <pspthreadman_kernel.h>
#include <psputilsforkernel.h>
#include <pspsysevent.h>
#include <pspiofilemgr.h>
#include <pspsysmem_kernel.h>
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "utils.h"
#include "systemctrl.h"
#include "printk.h"
#include "nid_resolver.h"
#include "strsafe.h"
#include "systemctrl_patch_offset.h"

extern int *InitForKernel_040C934B(void);
extern u32 sceKernelGetModel_620(void);
extern u32 sceKernelDevkitVersion_620(void);
extern SceModule* sceKernelFindModuleByName_620(char *modname);
extern int sceKernelExitVSH(struct SceKernelLoadExecVSHParam *param);
extern int sceKernelExitVSH_620(struct SceKernelLoadExecVSHParam *param);
extern int sceKernelLoadExecVSHDisc_620(const char *file, struct SceKernelLoadExecVSHParam *param);
extern int sceKernelLoadExecVSHDiscUpdater_620(const char *file, struct SceKernelLoadExecVSHParam *param);
extern int sceKernelLoadExecVSHMs1_620(const char *file, struct SceKernelLoadExecVSHParam *param);
extern int sceKernelLoadExecVSHMs2_620(const char *file, struct SceKernelLoadExecVSHParam *param);
extern int sceKernelLoadExecVSHEf2_620(const char *file, struct SceKernelLoadExecVSHParam *param);
extern int sceKernelLoadExecVSHEf2(const char *file, struct SceKernelLoadExecVSHParam *param);
extern int sceKernelLoadExecVSHMs3_620(const char *file, struct SceKernelLoadExecVSHParam *param);
extern int sceKernelLoadExecVSHMs3(const char *file, struct SceKernelLoadExecVSHParam *param);
extern int sceKernelLoadExecVSHMs4_620(const char *file, struct SceKernelLoadExecVSHParam *param);
extern int sceKernelSetDdrMemoryProtection_620(void *addr, int size, int prot);
extern SceUID sceKernelCreateHeap_620(SceUID partitionid, SceSize size, int unk, const char *name);
extern int sceKernelDeleteHeap_620(SceUID heapid);
extern int sceKernelFreeHeapMemory_620(SceUID heapid, void *block);
extern void* sceKernelAllocHeapMemory_620(SceUID heapid, SceSize size);
extern int sceKernelGetSystemStatus(void);
extern int sceKernelGetSystemStatus_620(void);
extern int sceKernelQueryMemoryPartitionInfo_620(int pid, PspSysmemPartitionInfo *info);
extern int sceKernelPartitionMaxFreeMemSize_620(int pid);
extern int sceKernelPartitionTotalFreeMemSize_620(int pid);

extern int (*g_on_module_start)(SceModule2*);

// for sctrlHENLoadModuleOnReboot
char *g_insert_module_before;
void *g_insert_module_binary;
int g_insert_module_size;
int g_insert_module_flags;

// for sctrlHENSetMemory
u32 g_p2_size = 24;
u32 g_p9_size = 24;

static char g_initfilename[80];
static char g_iso_filename[128];

int sctrlKernelExitVSH(struct SceKernelLoadExecVSHParam *param)
{
	u32 k1;
	int ret;

	k1 = pspSdkSetK1(0);
	ret = sceKernelExitVSH_620(param);

	if(ret == 0x8002013A) {
		ret = sceKernelExitVSH(param);
	}

	pspSdkSetK1(k1);

	return ret;
}

int sctrlKernelLoadExecVSHDisc(const char *file, struct SceKernelLoadExecVSHParam *param)
{
	u32 k1;
	int ret;

	k1 = pspSdkSetK1(0);
	ret = sceKernelLoadExecVSHDisc_620(file, param);

	if(ret == 0x8002013A) {
		ret = sceKernelLoadExecVSHDisc(file, param);
	}

	pspSdkSetK1(k1);

	return ret;
}

int sctrlKernelLoadExecVSHDiscUpdater(const char *file, struct SceKernelLoadExecVSHParam *param)
{
	u32 k1;
	int ret;

	k1 = pspSdkSetK1(0);
	ret = sceKernelLoadExecVSHDiscUpdater_620(file, param);

	if(ret == 0x8002013A) {
		ret = sceKernelLoadExecVSHDiscUpdater(file, param);
	}

	pspSdkSetK1(k1);

	return ret;
}

int sctrlKernelLoadExecVSHMs1(const char *file, struct SceKernelLoadExecVSHParam *param)
{
	u32 k1;
	int ret;

	k1 = pspSdkSetK1(0);
	ret = sceKernelLoadExecVSHMs1_620(file, param);

	if(ret == 0x8002013A) {
		ret = sceKernelLoadExecVSHMs1(file, param);
	}

	pspSdkSetK1(k1);

	return ret;
}

int sctrlKernelLoadExecVSHMs2(const char *file, struct SceKernelLoadExecVSHParam *param)
{
	u32 k1;
	int ret;

	k1 = pspSdkSetK1(0);
	ret = sceKernelLoadExecVSHMs2_620(file, param);

	if(ret == 0x8002013A) {
		ret = sceKernelLoadExecVSHMs2(file, param);
	}

	pspSdkSetK1(k1);

	return ret;
}

int sctrlKernelLoadExecVSHEf2(const char *file, struct SceKernelLoadExecVSHParam *param)
{
	u32 k1;
	int ret;

	k1 = pspSdkSetK1(0);

	ret = sceKernelLoadExecVSHEf2_620(file, param);

	if(ret == 0x8002013A) {
		ret = sceKernelLoadExecVSHEf2(file, param);
	}

	pspSdkSetK1(k1);

	return ret;
}

int sctrlKernelLoadExecVSHMs3(const char *file, struct SceKernelLoadExecVSHParam *param)
{
	u32 k1;
	int ret;

	k1 = pspSdkSetK1(0);
	ret = sceKernelLoadExecVSHMs3_620(file, param);

	if(ret == 0x8002013A) {
		ret = sceKernelLoadExecVSHMs3(file, param);
	}

	pspSdkSetK1(k1);

	return ret;
}

int sctrlKernelLoadExecVSHMs4(const char *file, struct SceKernelLoadExecVSHParam *param)
{
	u32 k1;
	int ret;

	k1 = pspSdkSetK1(0);
	ret = sceKernelLoadExecVSHMs4_620(file, param);

	if(ret == 0x8002013A) {
		ret = sceKernelLoadExecVSHMs4(file, param);
	}
	
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
	mod = (SceModule2*) sctrlKernelFindModuleByName("sceLoadExec");
	text_addr = mod->text_addr;

	if (psp_model == PSP_GO) {
		_sctrlKernelLoadExecVSHWithApitype = (void*)(text_addr + g_offs->systemctrl_export_patch.sctrlKernelLoadExecVSHWithApitype_05g); // 0x00002558 in 6.20
	} else {
		_sctrlKernelLoadExecVSHWithApitype = (void*)(text_addr + g_offs->systemctrl_export_patch.sctrlKernelLoadExecVSHWithApitype); // 0x00002304 in 6.20
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
	mod = (SceModule2*) sctrlKernelFindModuleByName("sceThreadManager");
	text_addr = mod->text_addr;
	_sw((level^8)<<28, *(u32*)(text_addr+g_offs->systemctrl_export_patch.sctrlKernelSetUserLevel)+0x14); // 0x00019E80 and 0x14 in 6.20, 6.31 remains the same

	pspSdkSetK1(k1);

	return ret;
}

int sctrlKernelSetDevkitVersion(int version)
{
	u32 k1;
	int ret;

	k1 = pspSdkSetK1(0);
	ret = sceKernelDevkitVersion();

	_sh((version>>16), g_offs->systemctrl_export_patch.sctrlKernelSetDevkitVersion); // 0x88011AAC in 6.20
	_sh((version&0xFFFF), g_offs->systemctrl_export_patch.sctrlKernelSetDevkitVersion+4); // 0x88011AB4 in 6.20

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
	return 0x1002; // 0x1000 in TN's code :)
}

int sctrlHENGetMinorVersion()
{
	return 0x3;
}

PspIoDrv *sctrlHENFindDriver(char *drvname)
{
	u32 k1;
	int *p;
	SceModule2 *mod;
	int* (*find_driver)(char *drvname);

	k1 = pspSdkSetK1(0);
	mod = (SceModule2*) sctrlKernelFindModuleByName("sceIOFileManager");
	find_driver = (void*)(mod->text_addr + g_offs->systemctrl_export_patch.sctrlHENFindDriver); // 0x00002A38 in 6.20/6.31
	p = find_driver(drvname);

	if (p != NULL) {
		p = (int*)(p[1]);
	}

	pspSdkSetK1(k1);

	if(psp_model == PSP_GO && p == NULL && 0 == stricmp(drvname, "msstor")) {
		return sctrlHENFindDriver("eflash0a0f");
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
		pMod = sceKernelFindModuleByAddress((u32)szMod);

		if (!pMod) {
			printk("%s: Cannot find %s_%08X\n", __func__, szLib == NULL ? "syslib" : szLib, nid);

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

	printk("%s: Cannot find %s_%08X\n", __func__, szLib == NULL ? "syslib" : szLib, nid);
	
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
	g_insert_module_before = module_before;
	g_insert_module_binary = buf;
	g_insert_module_size = size;
	g_insert_module_flags = flags;
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
		if (syscalls[i] == _addr) {
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
	*(const char**)(modmgr->text_addr+g_offs->systemctrl_export_patch.sctrlKernelSetUMDEmuFile) = g_iso_filename;

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
	*(const char**)(modmgr->text_addr+g_offs->systemctrl_export_patch.sctrlKernelSetInitFileName) = g_initfilename;

	return 0;
}

u32 sctrlKernelGetModel(void)
{
	u32 model;
   
	model = sceKernelGetModel_620();

	if(model == 0x8002013A) {
		model = sceKernelGetModel();
	}

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
	SceModule *mod;

	mod = sceKernelFindModuleByName_620(modname);

	if((u32)mod == 0x8002013A) {
		mod = sceKernelFindModuleByName(modname);
	}

	return mod;
}

int sctrlKernelSetDdrMemoryProtection(void *addr, int size, int prot)
{
	int ret;

	ret = sceKernelSetDdrMemoryProtection_620(addr, size, prot);

	if(ret == 0x8002013A) {
		ret = sceKernelSetDdrMemoryProtection(addr, size, prot);
	}

	return ret;
}

SceUID sctrlKernelCreateHeap(SceUID partitionid, SceSize size, int unk, const char *name)
{
	SceUID ret;

	ret = sceKernelCreateHeap_620(partitionid, size, unk, name);

	if(ret == 0x8002013A) {
		ret = sceKernelCreateHeap(partitionid, size, unk, name);
	}

	return ret;
}

int sctrlKernelDeleteHeap(SceUID heapid)
{
	int ret;

	ret = sceKernelDeleteHeap_620(heapid);

	if(ret == 0x8002013A) {
		ret = sceKernelDeleteHeap(heapid);
	}

	return ret;
}

int sctrlKernelFreeHeapMemory(SceUID heapid, void *block)
{
	int ret;

	ret = sceKernelFreeHeapMemory_620(heapid, block);

	if(ret == 0x8002013A) {
		ret = sceKernelFreeHeapMemory(heapid, block);
	}

	return ret;
}

void* sctrlKernelAllocHeapMemory(SceUID heapid, SceSize size)
{
	void *p;

	p = sceKernelAllocHeapMemory_620(heapid, size);

	if((u32)p == 0x8002013A) {
		p = sceKernelAllocHeapMemory(heapid, size);
	}

	return p;
}

int sctrlKernelGetSystemStatus(void)
{
	int ret;

	ret = sceKernelGetSystemStatus_620();

	if(ret == 0x8002013A) {
		ret = sceKernelGetSystemStatus();
	}

	return ret;
}

int sctrlKernelQueryMemoryPartitionInfo(int pid, PspSysmemPartitionInfo *info)
{
	int ret;

	ret = sceKernelQueryMemoryPartitionInfo_620(pid, info);

	if(ret == 0x8002013A) {
		ret = sceKernelQueryMemoryPartitionInfo(pid, info);
	}

	return ret;
}

int sctrlKernelPartitionMaxFreeMemSize(int pid)
{
	int ret;

	ret = sceKernelPartitionMaxFreeMemSize_620(pid);

	if(ret == 0x8002013A) {
		ret = sceKernelPartitionMaxFreeMemSize(pid);
	}

	return pid;
}

int sctrlKernelPartitionTotalFreeMemSize(int pid)
{
	int ret;

	ret = sceKernelPartitionTotalFreeMemSize_620(pid);

	if(ret == 0x8002013A) {
		ret = sceKernelPartitionTotalFreeMemSize(pid);
	}

	return pid;
}
