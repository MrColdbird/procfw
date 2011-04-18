#include <pspsdk.h>
#include <pspkernel.h>
#include <psputilsforkernel.h>
#include <pspthreadman_kernel.h>
#include <pspsysevent.h>
#include <pspiofilemgr.h>
#include <pspsysmem_kernel.h>
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "utils.h"
#include "systemctrl.h"
#include "printk.h"

SceUID kuKernelLoadModule(const char *path, int flags, SceKernelLMOption *option)
{
	u32 k1;
	int ret;

	k1 = pspSdkSetK1(0);
	ret = sctrlKernelLoadModule(path, flags, option);
	pspSdkSetK1(k1);

	return ret;
}

SceUID kuKernelLoadModuleWithApitype2(int apitype, const char *path, int flags, SceKernelLMOption *option)
{
	u32 k1;
	int ret;

	k1 = pspSdkSetK1(0);
	ret = sctrlKernelLoadModuleWithApitype2(apitype, path, flags, option);
	pspSdkSetK1(k1);

	return ret;
}

int kuKernelInitApitype(void)
{
	return sceKernelInitApitype();
}

int kuKernelBootFrom()
{
	return sctrlKernelBootFrom();
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
	return sceKernelApplicationType();
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
	ret = sctrlKernelSetDdrMemoryProtection(addr, size, prot);
	pspSdkSetK1(k1);

	return ret;
}

int kuKernelGetModel(void)
{
	return sctrlKernelGetModel();
}

void kuKernelIcacheInvalidateAll(void)
{
	u32 k1;
	
	k1 = pspSdkSetK1(0);
	sync_cache();
	pspSdkSetK1(k1);
}
