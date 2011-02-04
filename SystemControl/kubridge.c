#include <pspsdk.h>
#include <pspkernel.h>
#include <psputilsforkernel.h>
#include <pspthreadman_kernel.h>
#include <pspsysevent.h>
#include <pspiofilemgr.h>
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "utils.h"
#include "systemctrl.h"
#include "printk.h"

int SysMemForKernel_458A70B5(void);
SceUID ModuleMgrForKernel_32292450(int apitype, const char *path, int flags, SceKernelLMOption *option);
int SysMemForKernel_00E9A04A(void *addr, int size, int prot);

SceUID kuKernelLoadModule(const char *path, int flags, SceKernelLMOption *option)
{
	u32 k1;
	int ret;

	k1 = pspSdkSetK1(0);
	ret = sceKernelLoadModule(path, flags, option);
	pspSdkSetK1(k1);

	return ret;
}

SceUID kuKernelLoadModuleWithApitype2(int apitype, const char *path, int flags, SceKernelLMOption *option)
{
	u32 k1;
	int ret;

	k1 = pspSdkSetK1(0);
	ret = ModuleMgrForKernel_32292450(apitype, path, flags, option);
	pspSdkSetK1(k1);

	return ret;
}

int kuKernelInitApitype(void)
{
	return sceKernelInitApitype();
}

int kuKernelBootFrom()
{
	return sceKernelBootFrom();
}

int kuKernelInitFileName(char *initfilename)
{
	u32 k1;
	char* init_fn;

	k1 = pspSdkSetK1(0);
	init_fn = sceKernelInitFileName();
	strcpy(initfilename, init_fn);
	pspSdkSetK1(k1);

	return 0;
}

int kuKernelInitKeyConfig()
{
	// sceKernelApplicationType
	return InitForKernel_7233B5BC();
}

int kuKernelGetUserLevel(void)
{
	u32 k1;
	int ret;

	k1 = pspSdkSetK1(0);
	ret = sceKernelGetUserLevel();
	pspSdkSetK1(k1);

	return ret;
}

int kuKernelSetDdrMemoryProtection(void *addr, int size, int prot)
{
	int ret;
	u32 k1;

	k1 = pspSdkSetK1(0);
	ret = SysMemForKernel_00E9A04A(addr, size, prot);
	pspSdkSetK1(k1);

	return ret;
}

int kuKernelGetModel(void)
{
	return SysMemForKernel_458A70B5();
}

