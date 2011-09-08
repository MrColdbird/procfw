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
#include "utils.h"
#include "systemctrl.h"
#include "printk.h"
#include "cross_fw.h"

extern u32 psp_fw_version;
extern u32 psp_model;

extern u32 sceKernelGetModel_620(void);
extern u32 sceKernelDevkitVersion_620(void);
extern SceModule* sceKernelFindModuleByName_620(char *modname);
extern int sceKernelExitVSH_620(struct SceKernelLoadExecVSHParam *param);
extern int sceKernelLoadExecVSHDisc_620(const char *file, struct SceKernelLoadExecVSHParam *param);
extern int sceKernelLoadExecVSHDiscUpdater_620(const char *file, struct SceKernelLoadExecVSHParam *param);
extern int sceKernelLoadExecVSHMs1_620(const char *file, struct SceKernelLoadExecVSHParam *param);
extern int sceKernelLoadExecVSHMs2_620(const char *file, struct SceKernelLoadExecVSHParam *param);
extern int sceKernelLoadExecVSHEf2_620(const char *file, struct SceKernelLoadExecVSHParam *param);
extern int sceKernelLoadExecVSHMs3_620(const char *file, struct SceKernelLoadExecVSHParam *param);
extern int sceKernelLoadExecVSHMs4_620(const char *file, struct SceKernelLoadExecVSHParam *param);
extern int sceKernelSetDdrMemoryProtection_620(void *addr, int size, int prot);
extern SceUID sceKernelCreateHeap_620(SceUID partitionid, SceSize size, int unk, const char *name);
extern int sceKernelDeleteHeap_620(SceUID heapid);
extern int sceKernelFreeHeapMemory_620(SceUID heapid, void *block);
extern void* sceKernelAllocHeapMemory_620(SceUID heapid, SceSize size);
extern int sceKernelGetSystemStatus_620(void);
extern int sceKernelQueryMemoryPartitionInfo_620(int pid, PspSysmemPartitionInfo *info);
extern int sceKernelPartitionMaxFreeMemSize_620(int pid);
extern int sceKernelPartitionTotalFreeMemSize_620(int pid);
extern u32 sceKernelQuerySystemCall_620(void *func);
extern SceModule* sceKernelFindModuleByUID_620(SceUID modid);
extern SceModule* sceKernelFindModuleByAddress_620(u32 address);
extern int sceKernelCheckExecFile_620(unsigned char * buffer, int * check);	
extern int sceKernelLoadModule_620(const char *path, int flags, SceKernelLMOption *option);
extern int sceKernelStartModule_620(SceUID modid, SceSize argsize, void *argp, int *status, SceKernelSMOption *option);
extern int sceKernelUnloadModule_620(SceUID modid);
extern SceUID sceKernelLoadModuleWithApitype2_620(int apitype, const char *path, int flags, SceKernelLMOption *option);
extern int sceKernelBootFromGo_620(void);
void* sceKernelGetBlockHeadAddr_620(SceUID blockid);
SceUID sceKernelAllocPartitionMemory_620(SceUID partitionid, const char * name, int type, SceSize size, void * addr);

extern u32 sceKernelGetModel_660(void);
extern u32 sceKernelDevkitVersion_660(void);
extern SceModule* sceKernelFindModuleByName_660(char *modname);
extern int sceKernelExitVSH_660(struct SceKernelLoadExecVSHParam *param);
extern int sceKernelLoadExecVSHDisc_660(const char *file, struct SceKernelLoadExecVSHParam *param);
extern int sceKernelLoadExecVSHDiscUpdater_660(const char *file, struct SceKernelLoadExecVSHParam *param);
extern int sceKernelLoadExecVSHMs1_660(const char *file, struct SceKernelLoadExecVSHParam *param);
extern int sceKernelLoadExecVSHMs2_660(const char *file, struct SceKernelLoadExecVSHParam *param);
extern int sceKernelLoadExecVSHEf2_660(const char *file, struct SceKernelLoadExecVSHParam *param);
extern int sceKernelLoadExecVSHMs3_660(const char *file, struct SceKernelLoadExecVSHParam *param);
extern int sceKernelLoadExecVSHMs4_660(const char *file, struct SceKernelLoadExecVSHParam *param);
extern int sceKernelSetDdrMemoryProtection_660(void *addr, int size, int prot);
extern SceUID sceKernelCreateHeap_660(SceUID partitionid, SceSize size, int unk, const char *name);
extern int sceKernelDeleteHeap_660(SceUID heapid);
extern int sceKernelFreeHeapMemory_660(SceUID heapid, void *block);
extern void* sceKernelAllocHeapMemory_660(SceUID heapid, SceSize size);
extern int sceKernelGetSystemStatus_660(void);
extern int sceKernelQueryMemoryPartitionInfo_660(int pid, PspSysmemPartitionInfo *info);
extern int sceKernelPartitionMaxFreeMemSize_660(int pid);
extern int sceKernelPartitionTotalFreeMemSize_660(int pid);
extern u32 sceKernelQuerySystemCall_660(void *func);
extern SceModule* sceKernelFindModuleByUID_660(SceUID modid);
extern SceModule* sceKernelFindModuleByAddress_660(u32 address);
extern int sceKernelCheckExecFile_660(unsigned char * buffer, int * check);	
extern int sceKernelLoadModule_660(const char *path, int flags, SceKernelLMOption *option);
extern int sceKernelStartModule_660(SceUID modid, SceSize argsize, void *argp, int *status, SceKernelSMOption *option);
extern int sceKernelUnloadModule_660(SceUID modid);
extern SceUID sceKernelLoadModuleWithApitype2_660(int apitype, const char *path, int flags, SceKernelLMOption *option);
extern int sceKernelBootFromGo_660(void);
void* sceKernelGetBlockHeadAddr_660(SceUID blockid);
SceUID sceKernelAllocPartitionMemory_660(SceUID partitionid, const char * name, int type, SceSize size, void * addr);

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

extern int sceCtrlReadBufferPositive_639(SceCtrlData *pad_data, int count);
extern int sceCtrlReadBufferPositive_620(SceCtrlData *pad_data, int count);
extern int sceCtrlReadBufferPositive_660(SceCtrlData *pad_data, int count);

int sctrlKernelExitVSH(struct SceKernelLoadExecVSHParam *param)
{
	u32 k1;
	int ret = -1;

	k1 = pspSdkSetK1(0);

	switch(psp_fw_version) {
#ifdef CONFIG_660
		case FW_660:
			ret = sceKernelExitVSH_660(param);
			break;
#endif
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

int sctrlKernelLoadExecVSHDisc(const char *file, struct SceKernelLoadExecVSHParam *param)
{
	u32 k1;
	int ret = -1;

	k1 = pspSdkSetK1(0);

	switch(psp_fw_version) {
#ifdef CONFIG_660
		case FW_660:
			ret = sceKernelLoadExecVSHDisc_660(file, param);
			break;
#endif
#ifdef CONFIG_639
		case FW_639:
			ret = sceKernelLoadExecVSHDisc(file, param);
			break;
#endif
#ifdef CONFIG_635
		case FW_635:
			ret = sceKernelLoadExecVSHDisc(file, param);
			break;
#endif
#ifdef CONFIG_620
		case FW_620:
			ret = sceKernelLoadExecVSHDisc_620(file, param);
			break;
#endif
	};

	pspSdkSetK1(k1);

	return ret;
}

int sctrlKernelLoadExecVSHDiscUpdater(const char *file, struct SceKernelLoadExecVSHParam *param)
{
	u32 k1;
	int ret = -1;

	k1 = pspSdkSetK1(0);

	switch(psp_fw_version) {
#ifdef CONFIG_660
		case FW_660:
			ret = sceKernelLoadExecVSHDiscUpdater_660(file, param);
			break;
#endif
#ifdef CONFIG_639
		case FW_639:
			ret = sceKernelLoadExecVSHDiscUpdater(file, param);
			break;
#endif
#ifdef CONFIG_635
		case FW_635:
			ret = sceKernelLoadExecVSHDiscUpdater(file, param);
			break;
#endif
#ifdef CONFIG_620
		case FW_620:
			ret = sceKernelLoadExecVSHDiscUpdater_620(file, param);
			break;
#endif
	};
	
	pspSdkSetK1(k1);

	return ret;
}

int sctrlKernelLoadExecVSHMs1(const char *file, struct SceKernelLoadExecVSHParam *param)
{
	u32 k1;
	int ret = -1;

	k1 = pspSdkSetK1(0);

	switch(psp_fw_version) {
#ifdef CONFIG_660
		case FW_660:
			ret = sceKernelLoadExecVSHMs1_660(file, param);
			break;
#endif
#ifdef CONFIG_639
		case FW_639:
			ret = sceKernelLoadExecVSHMs1(file, param);
			break;
#endif
#ifdef CONFIG_635
		case FW_635:
			ret = sceKernelLoadExecVSHMs1(file, param);
			break;
#endif
#ifdef CONFIG_620
		case FW_620:
			ret = sceKernelLoadExecVSHMs1_620(file, param);
			break;
#endif
	};
	
	pspSdkSetK1(k1);

	return ret;
}

int sctrlKernelLoadExecVSHMs2(const char *file, struct SceKernelLoadExecVSHParam *param)
{
	u32 k1;
	int ret = -1;

	k1 = pspSdkSetK1(0);

	switch(psp_fw_version) {
#ifdef CONFIG_660
		case FW_660:
			ret = sceKernelLoadExecVSHMs2_660(file, param);
			break;
#endif
#ifdef CONFIG_639
		case FW_639:
			ret = sceKernelLoadExecVSHMs2(file, param);
			break;
#endif
#ifdef CONFIG_635
		case FW_635:
			ret = sceKernelLoadExecVSHMs2(file, param);
			break;
#endif
#ifdef CONFIG_620
		case FW_620:
			ret = sceKernelLoadExecVSHMs2_620(file, param);
			break;
#endif
	};
	
	pspSdkSetK1(k1);

	return ret;
}

int sctrlKernelLoadExecVSHEf2(const char *file, struct SceKernelLoadExecVSHParam *param)
{
	u32 k1;
	int ret = -1;

	k1 = pspSdkSetK1(0);

	switch(psp_fw_version) {
#ifdef CONFIG_660
		case FW_660:
			ret = sceKernelLoadExecVSHEf2_660(file, param);
			break;
#endif
#ifdef CONFIG_639
		case FW_639:
			ret = sceKernelLoadExecVSHEf2(file, param);
			break;
#endif
#ifdef CONFIG_635
		case FW_635:
			ret = sceKernelLoadExecVSHEf2(file, param);
			break;
#endif
#ifdef CONFIG_620
		case FW_620:
			ret = sceKernelLoadExecVSHEf2_620(file, param);
			break;
#endif
	};
	
	pspSdkSetK1(k1);

	return ret;
}

int sctrlKernelLoadExecVSHMs3(const char *file, struct SceKernelLoadExecVSHParam *param)
{
	u32 k1;
	int ret = -1;

	k1 = pspSdkSetK1(0);

	switch(psp_fw_version) {
#ifdef CONFIG_660
		case FW_660:
			ret = sceKernelLoadExecVSHMs3_660(file, param);
			break;
#endif
#ifdef CONFIG_639
		case FW_639:
			ret = sceKernelLoadExecVSHMs3(file, param);
			break;
#endif
#ifdef CONFIG_635
		case FW_635:
			ret = sceKernelLoadExecVSHMs3(file, param);
			break;
#endif
#ifdef CONFIG_620
		case FW_620:
			ret = sceKernelLoadExecVSHMs3_620(file, param);
			break;
#endif
	};
	
	pspSdkSetK1(k1);

	return ret;
}

int sctrlKernelLoadExecVSHMs4(const char *file, struct SceKernelLoadExecVSHParam *param)
{
	u32 k1;
	int ret = -1;

	k1 = pspSdkSetK1(0);

	switch(psp_fw_version) {
#ifdef CONFIG_660
		case FW_660:
			ret = sceKernelLoadExecVSHMs4_660(file, param);
			break;
#endif
#ifdef CONFIG_639
		case FW_639:
			ret = sceKernelLoadExecVSHMs4(file, param);
			break;
#endif
#ifdef CONFIG_635
		case FW_635:
			ret = sceKernelLoadExecVSHMs4(file, param);
			break;
#endif
#ifdef CONFIG_620
		case FW_620:
			ret = sceKernelLoadExecVSHMs4_620(file, param);
			break;
#endif
	};
	
	pspSdkSetK1(k1);

	return ret;
}

SceUID sctrlKernelAllocPartitionMemory(SceUID partitionid, const char * name, int type, SceSize size, void * addr)
{
	u32 k1;
	int ret = -1;

	k1 = pspSdkSetK1(0);

	switch(psp_fw_version) {
#ifdef CONFIG_660
		case FW_660:
			ret = sceKernelAllocPartitionMemory_660(partitionid, name, type, size, addr);
			break;
#endif
#ifdef CONFIG_639
		case FW_639:
			ret = sceKernelAllocPartitionMemory(partitionid, name, type, size, addr);
			break;
#endif
#ifdef CONFIG_635
		case FW_635:
			ret = sceKernelAllocPartitionMemory(partitionid, name, type, size, addr);
			break;
#endif
#ifdef CONFIG_620
		case FW_620:
			ret = sceKernelAllocPartitionMemory_620(partitionid, name, type, size, addr);
			break;
#endif
	};
	
	pspSdkSetK1(k1);

	return ret;
}

void* sctrlKernelGetBlockHeadAddr(SceUID blockid)
{
	u32 k1;
	void* ret = NULL;

	k1 = pspSdkSetK1(0);

	switch(psp_fw_version) {
#ifdef CONFIG_660
		case FW_660:
			ret = sceKernelGetBlockHeadAddr_660(blockid);
			break;
#endif
#ifdef CONFIG_639
		case FW_639:
			ret = sceKernelGetBlockHeadAddr(blockid);
			break;
#endif
#ifdef CONFIG_635
		case FW_635:
			ret = sceKernelGetBlockHeadAddr(blockid);
			break;
#endif
#ifdef CONFIG_620
		case FW_620:
			ret = sceKernelGetBlockHeadAddr_620(blockid);
			break;
#endif
	};
	
	pspSdkSetK1(k1);

	return ret;
}

u32 sctrlKernelGetModel(void)
{
	u32 model = -1;

	switch(psp_fw_version) {
#ifdef CONFIG_660
		case FW_660:
			model = sceKernelGetModel_660();
			break;
#endif
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

	fw_version = sceKernelDevkitVersion_660();

	if(fw_version == 0x8002013A) {
		fw_version = sceKernelDevkitVersion_620();

		if(fw_version == 0x8002013A) {
			fw_version = sceKernelDevkitVersion();
		}
	}

	return fw_version;
}

SceModule* sctrlKernelFindModuleByName(char *modname)
{
	SceModule *mod = NULL;

	switch(psp_fw_version) {
#ifdef CONFIG_660
		case FW_660:
			mod = sceKernelFindModuleByName_660(modname);
			break;
#endif
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

int sctrlKernelSetDdrMemoryProtection(void *addr, int size, int prot)
{
	int ret = -1;

	switch(psp_fw_version) {
#ifdef CONFIG_660
		case FW_660:
			ret = sceKernelSetDdrMemoryProtection_660(addr, size, prot);
			break;
#endif
#ifdef CONFIG_639
		case FW_639:
			ret = sceKernelSetDdrMemoryProtection(addr, size, prot);
			break;
#endif
#ifdef CONFIG_635
		case FW_635:
			ret = sceKernelSetDdrMemoryProtection(addr, size, prot);
			break;
#endif
#ifdef CONFIG_620
		case FW_620:
			ret = sceKernelSetDdrMemoryProtection_620(addr, size, prot);
			break;
#endif
	};
	
	return ret;
}

SceUID sctrlKernelCreateHeap(SceUID partitionid, SceSize size, int unk, const char *name)
{
	SceUID ret = -1;

	switch(psp_fw_version) {
#ifdef CONFIG_660
		case FW_660:
			ret = sceKernelCreateHeap_660(partitionid, size, unk, name);
			break;
#endif
#ifdef CONFIG_639
		case FW_639:
			ret = sceKernelCreateHeap(partitionid, size, unk, name);
			break;
#endif
#ifdef CONFIG_635
		case FW_635:
			ret = sceKernelCreateHeap(partitionid, size, unk, name);
			break;
#endif
#ifdef CONFIG_620
		case FW_620:
			ret = sceKernelCreateHeap_620(partitionid, size, unk, name);
			break;
#endif
	};
	
	return ret;
}

int sctrlKernelDeleteHeap(SceUID heapid)
{
	int ret = -1;

	switch(psp_fw_version) {
#ifdef CONFIG_660
		case FW_660:
			ret = sceKernelDeleteHeap_660(heapid);
			break;
#endif
#ifdef CONFIG_639
		case FW_639:
			ret = sceKernelDeleteHeap(heapid);
			break;
#endif
#ifdef CONFIG_635
		case FW_635:
			ret = sceKernelDeleteHeap(heapid);
			break;
#endif
#ifdef CONFIG_620
		case FW_620:
			ret = sceKernelDeleteHeap_620(heapid);
			break;
#endif
	};
	
	return ret;
}

int sctrlKernelFreeHeapMemory(SceUID heapid, void *block)
{
	int ret = -1;

	switch(psp_fw_version) {
#ifdef CONFIG_660
		case FW_660:
			ret = sceKernelFreeHeapMemory_660(heapid, block);
			break;
#endif
#ifdef CONFIG_639
		case FW_639:
			ret = sceKernelFreeHeapMemory(heapid, block);
			break;
#endif
#ifdef CONFIG_635
		case FW_635:
			ret = sceKernelFreeHeapMemory(heapid, block);
			break;
#endif
#ifdef CONFIG_620
		case FW_620:
			ret = sceKernelFreeHeapMemory_620(heapid, block);
			break;
#endif
	};
	
	return ret;
}

void* sctrlKernelAllocHeapMemory(SceUID heapid, SceSize size)
{
	void *p = NULL;

	switch(psp_fw_version) {
#ifdef CONFIG_660
		case FW_660:
			p = sceKernelAllocHeapMemory_660(heapid, size);
			break;
#endif
#ifdef CONFIG_639
		case FW_639:
			p = sceKernelAllocHeapMemory(heapid, size);
			break;
#endif
#ifdef CONFIG_635
		case FW_635:
			p = sceKernelAllocHeapMemory(heapid, size);
			break;
#endif
#ifdef CONFIG_620
		case FW_620:
			p = sceKernelAllocHeapMemory_620(heapid, size);
			break;
#endif
	};

	return p;
}

int sctrlKernelGetSystemStatus(void)
{
	int ret = -1;

	switch(psp_fw_version) {
#ifdef CONFIG_660
		case FW_660:
			ret = sceKernelGetSystemStatus_660();
			break;
#endif
#ifdef CONFIG_639
		case FW_639:
			ret = sceKernelGetSystemStatus();
			break;
#endif
#ifdef CONFIG_635
		case FW_635:
			ret = sceKernelGetSystemStatus();
			break;
#endif
#ifdef CONFIG_620
		case FW_620:
			ret = sceKernelGetSystemStatus_620();
			break;
#endif
	};
	
	return ret;
}

int sctrlKernelQueryMemoryPartitionInfo(int pid, PspSysmemPartitionInfo *info)
{
	int ret = -1;

	switch(psp_fw_version) {
#ifdef CONFIG_660
		case FW_660:
			ret = sceKernelQueryMemoryPartitionInfo_660(pid, info);
			break;
#endif
#ifdef CONFIG_639
		case FW_639:
			ret = sceKernelQueryMemoryPartitionInfo(pid, info);
			break;
#endif
#ifdef CONFIG_635
		case FW_635:
			ret = sceKernelQueryMemoryPartitionInfo(pid, info);
			break;
#endif
#ifdef CONFIG_620
		case FW_620:
			ret = sceKernelQueryMemoryPartitionInfo_620(pid, info);
			break;
#endif
	};

	return ret;
}

int sctrlKernelPartitionMaxFreeMemSize(int pid)
{
	int ret = -1;

	switch(psp_fw_version) {
#ifdef CONFIG_660
		case FW_660:
			ret = sceKernelPartitionMaxFreeMemSize_660(pid);
			break;
#endif
#ifdef CONFIG_639
		case FW_639:
			ret = sceKernelPartitionMaxFreeMemSize(pid);
			break;
#endif
#ifdef CONFIG_635
		case FW_635:
			ret = sceKernelPartitionMaxFreeMemSize(pid);
			break;
#endif
#ifdef CONFIG_620
		case FW_620:
			ret = sceKernelPartitionMaxFreeMemSize_620(pid);
			break;
#endif
	};
	
	return ret;
}

int sctrlKernelPartitionTotalFreeMemSize(int pid)
{
	int ret = -1;

	switch(psp_fw_version) {
#ifdef CONFIG_660
		case FW_660:
			ret = sceKernelPartitionTotalFreeMemSize_660(pid);
			break;
#endif
#ifdef CONFIG_639
		case FW_639:
			ret = sceKernelPartitionTotalFreeMemSize(pid);
			break;
#endif
#ifdef CONFIG_635
		case FW_635:
			ret = sceKernelPartitionTotalFreeMemSize(pid);
			break;
#endif
#ifdef CONFIG_620
		case FW_620:
			ret = sceKernelPartitionTotalFreeMemSize_620(pid);
			break;
#endif
	};
	
	return ret;
}

int sctrlKernelQuerySystemCall(void *func_addr)
{
	int ret = -1;
	u32 k1;

	k1 = pspSdkSetK1(0);

	switch(psp_fw_version) {
#ifdef CONFIG_660
		case FW_660:
			ret = sceKernelQuerySystemCall_660(func_addr);
			break;
#endif
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

SceModule* sctrlKernelFindModuleByUID(SceUID modid)
{
	SceModule *mod = NULL;

	switch(psp_fw_version) {
#ifdef CONFIG_660
		case FW_660:
			mod = sceKernelFindModuleByUID_660(modid);
			break;
#endif
#ifdef CONFIG_639
		case FW_639:
			mod = sceKernelFindModuleByUID(modid);
			break;
#endif
#ifdef CONFIG_635
		case FW_635:
			mod = sceKernelFindModuleByUID(modid);
			break;
#endif
#ifdef CONFIG_620
		case FW_620:
			mod = sceKernelFindModuleByUID_620(modid);
			break;
#endif
	};

	return mod;
}

SceModule* sctrlKernelFindModuleByAddress(u32 address)
{
	SceModule *mod = NULL;

	switch(psp_fw_version) {
#ifdef CONFIG_660
		case FW_660:
			mod = sceKernelFindModuleByAddress_660(address);
			break;
#endif
#ifdef CONFIG_639
		case FW_639:
			mod = sceKernelFindModuleByAddress(address);
			break;
#endif
#ifdef CONFIG_635
		case FW_635:
			mod = sceKernelFindModuleByAddress(address);
			break;
#endif
#ifdef CONFIG_620
		case FW_620:
			mod = sceKernelFindModuleByAddress_620(address);
			break;
#endif
	};

	return mod;
}

int sctrlKernelCheckExecFile(unsigned char * buffer, int * check)
{
	int ret = -1;

	switch(psp_fw_version) {
#ifdef CONFIG_660
		case FW_660:
			ret = sceKernelCheckExecFile_660(buffer, check);
			break;
#endif
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
#ifdef CONFIG_660
		case FW_660:
			ret = sceKernelLoadModule_660(path, flags, option);
			break;
#endif
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
#ifdef CONFIG_660
		case FW_660:
			ret = sceKernelStartModule_660(modid, argsize, argp, status, option);
			break;
#endif
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

int sctrlKernelUnloadModule(SceUID modid)
{
	int ret = -1;

	switch(psp_fw_version) {
#ifdef CONFIG_660
		case FW_660:
			ret = sceKernelUnloadModule_660(modid);
			break;
#endif
#ifdef CONFIG_639
		case FW_639:
			ret = sceKernelUnloadModule(modid);
			break;
#endif
#ifdef CONFIG_635
		case FW_635:
			ret = sceKernelUnloadModule(modid);
			break;
#endif
#ifdef CONFIG_620
		case FW_620:
			ret = sceKernelUnloadModule_620(modid);
			break;
#endif
	}
	
	return ret;
}

SceUID sctrlKernelLoadModuleWithApitype2(int apitype, const char *path, int flags, SceKernelLMOption *option)
{
	SceUID ret = -1;

	switch(psp_fw_version) {
#ifdef CONFIG_660
		case FW_660:
			ret = sceKernelLoadModuleWithApitype2_660(apitype, path, flags, option);
			break;
#endif
#ifdef CONFIG_639
		case FW_639:
			ret = _sceKernelLoadModuleWithApitype2(apitype, path, flags, option);
			break;
#endif
#ifdef CONFIG_635
		case FW_635:
			ret = _sceKernelLoadModuleWithApitype2(apitype, path, flags, option);
			break;
#endif
#ifdef CONFIG_620
		case FW_620:
			ret = sceKernelLoadModuleWithApitype2_620(apitype, path, flags, option);
			break;
#endif
	};

	return ret;
}

int sctrlKernelBootFrom(void)
{
	if(psp_model == PSP_GO) {
		if(psp_fw_version == FW_660) {
			return sceKernelBootFromGo_660();
		} else if(psp_fw_version == FW_639) {
			return sceKernelBootFromGo_635();
		} else if(psp_fw_version == FW_635) {
			return sceKernelBootFromGo_635();
		} else if(psp_fw_version == FW_620) {
			return sceKernelBootFromGo_620();
		}
	}

	return sceKernelBootFrom();
}

int sctrlReadBufferPositive(SceCtrlData *pad_data, int count)
{
	int ret = -1;

	switch(psp_fw_version) {
#ifdef CONFIG_660
		case FW_660:
			ret = sceCtrlReadBufferPositive_660(pad_data, count);
			break;
#endif
#ifdef CONFIG_639
		case FW_639:
			ret = sceCtrlReadBufferPositive_639(pad_data, count);
			break;
#endif
#ifdef CONFIG_635
		case FW_635:
			ret = sceCtrlReadBufferPositive_639(pad_data, count);
			break;
#endif
#ifdef CONFIG_620
		case FW_620:
			ret = sceCtrlReadBufferPositive_620(pad_data, count);
			break;
#endif
	};

	return ret;
}
