#include <pspsdk.h>
#include <pspkernel.h>
#include <psputilsforkernel.h>
#include <pspthreadman_kernel.h>
#include <pspsysevent.h>
#include <pspiofilemgr.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "systemctrl.h"
#include "modhandler.h"
#include "printk.h"
#include "syspatch.h"

extern int SysMemForKernel_458A70B5(void);

SceUID kuKernelLoadModule(const char *path, int flags, SceKernelLMOption *option)
{
	u32 k1 = pspSdkSetK1(0);
	int result = sctrlKernelLoadModule(path, flags, option);
	pspSdkSetK1(k1);

	return result;
}

int kuKernelGetModel(void)
{
	return sctrlKernelGetModel();
}

