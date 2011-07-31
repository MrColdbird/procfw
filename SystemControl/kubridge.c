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
#include "kubridge.h"
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

u32 kuKernelPeekw(void *addr)
{
	return _lw((u32)addr);
}

void kuKernelPokew(void *addr, u32 value)
{
	_sw(value, (u32)addr);
}

void *kuKernelMemcpy(void *dest, const void *src, size_t num)
{
	void *addr;
	u32 k1;

	k1 = pspSdkSetK1(0);
	addr = memcpy(dest, src, num);
	pspSdkSetK1(k1);

	return addr;
}

int kuKernelFindModuleByName(char *modname, SceModule *mod)
{
	SceModule2 *pmod;

	if(modname == NULL || mod == NULL) {
		return -1;
	}

	pmod = (SceModule2*) sctrlKernelFindModuleByName(modname);

	if(pmod == NULL) {
		return -2;
	}

	memcpy(mod, pmod, sizeof(*pmod));
	
	return 0;
}

int kuKernelCall(void *func_addr, struct KernelCallArg *args)
{
	u32 k1, level;
	u64 ret;
	u64 (*func)(u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32);

	if(func_addr == NULL || args == NULL) {
		return -1;
	}

	k1 = pspSdkSetK1(0);
	level = sctrlKernelSetUserLevel(8);
	func = func_addr;
	ret = (*func)(args->arg1, args->arg2, args->arg3, args->arg4, args->arg5, args->arg6, args->arg7, args->arg8, args->arg9, args->arg10, args->arg11, args->arg12);
	args->ret1 = (u32)(ret);
	args->ret2 = (u32)(ret >> 32);
	sctrlKernelSetUserLevel(level);
	pspSdkSetK1(k1);

	return 0;
}

struct KernelCallArgExtendStack {
	struct KernelCallArg args;
	void *func_addr;
};

static int kernel_call_stack(struct KernelCallArgExtendStack *args_stack)
{
	u64 ret;
	struct KernelCallArg *args;
	int (*func)(u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32);

	args = &args_stack->args;
	func = args_stack->func_addr;
	ret = (*func)(args->arg1, args->arg2, args->arg3, args->arg4, args->arg5, args->arg6, args->arg7, args->arg8, args->arg9, args->arg10, args->arg11, args->arg12);
	args->ret1 = (u32)(ret);
	args->ret2 = (u32)(ret >> 32);

	return 0;
}

int kuKernelCallExtendStack(void *func_addr, struct KernelCallArg *args, int stack_size)
{
	u32 k1, level;
	int ret;
	struct KernelCallArgExtendStack args_stack;

	if(func_addr == NULL || args == NULL) {
		return -1;
	}

	k1 = pspSdkSetK1(0);
	level = sctrlKernelSetUserLevel(8);
	memcpy(&args_stack.args, args, sizeof(*args));
	args_stack.func_addr = func_addr;
	ret = sceKernelExtendKernelStack(stack_size, (void*)&kernel_call_stack, &args_stack);
	sctrlKernelSetUserLevel(level);
	pspSdkSetK1(k1);

	return ret;
}

void kuKernelGetUmdFile(char *umdfile, int size)
{
	strncpy(umdfile, GetUmdFile(), size);
}
