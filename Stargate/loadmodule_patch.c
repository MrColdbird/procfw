#include <pspsdk.h>
#include <pspiofilemgr_kernel.h>
#include <psploadcore.h>
#include <pspsysmem_kernel.h>
#include <stdio.h>
#include <string.h>
#include "printk.h"
#include "systemctrl.h"
#include "systemctrl_private.h"
#include "strsafe.h"
#include "libs.h"
#include "utils.h"

int (*_sceKernelLoadModule)(char *fname, int flag, void *opt) = NULL;

int load_module_get_function(void)
{
	_sceKernelLoadModule = (void*)sctrlHENFindFunction("sceModuleManager", "ModuleMgrForUser", 0x977DE386);
	
	if (_sceKernelLoadModule == NULL) return -5;

	return 0;
}

int myKernelLoadModule(char *fname, int flag, void *opt)
{
	int ret;

	ret = (*_sceKernelLoadModule)(fname, flag, opt);

	if (ret == 0x80020148 || ret == 0x80020130) {
		if (!strncasecmp(fname, "ms0:", sizeof("ms0:")-1)) {
			ret = 0x80020146;
			printk("%s: [FAKE] -> 0x%08X\n", __func__, ret);
		}
	}
	
	return ret;
}

void patch_load_module(SceModule *mod)
{
	hook_import_bynid(mod, "ModuleMgrForUser", 0x977DE386, &myKernelLoadModule, 1);
}
