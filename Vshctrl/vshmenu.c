#include <pspsdk.h>
#include <pspsysmem_kernel.h>
#include <pspkernel.h>
#include <psputilsforkernel.h>
#include <pspsysevent.h>
#include <pspiofilemgr.h>
#include <pspctrl.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "libs.h"
#include "systemctrl.h"
#include "printk.h"
#include "systemctrl_se.h"
#include "main.h"

static int (*g_VshMenuCtrl) (SceCtrlData *, int);
static SceUID g_satelite_mod_id = -1;

int vctrlVSHRegisterVshMenu(int (* ctrl)(SceCtrlData *, int))
{
	u32 k1;
   
	k1 = pspSdkSetK1(0);
	g_VshMenuCtrl = (void *) ((u32) ctrl | 0x80000000);
	pspSdkSetK1(k1);

	return 0;
}

int vctrlVSHExitVSHMenu(SEConfig *config, char *videoiso, int disctype)
{
	u32 k1;
	int ret;

   	k1 = pspSdkSetK1(0);
	memcpy(&conf, config, sizeof(conf));
	ret = sctrlSESetConfig(&conf);
	g_VshMenuCtrl = NULL;
	pspSdkSetK1(k1);
	
	return ret;
}

static SceUID load_satelite(void)
{
	SceUID modid;
	const char *mod = "flash0:/vsh/module/satelite.prx";
	SceKernelLMOption opt = {
		.size = 0x14,
		.flags = 0,
		.access = 1,
		.position = 1,
	};

	modid = sceKernelLoadModule(mod, 0, &opt);
	printk("%s: loading %s -> 0x%08X\n", __func__, mod, modid);

	return modid;
}

int _sceCtrlReadBufferPositive(SceCtrlData *ctrl, int count)
{
	int ret;
	u32 k1;
	SceUID modid;

	ret = sceCtrlReadBufferPositive(ctrl, count);
	k1 = pspSdkSetK1(0);

	if (sceKernelFindModuleByName("VshCtrlSatelite")) {
		if (g_VshMenuCtrl) {
			(*g_VshMenuCtrl)(ctrl, count);
		} else {
			if (g_satelite_mod_id >= 0) {
				if (sceKernelStopModule(g_satelite_mod_id, 0, 0, 0, 0) >= 0) {
					sceKernelUnloadModule(g_satelite_mod_id);
				}
			}
		}
	} else {
		if (sceKernelFindModuleByName("htmlviewer_plugin_module"))
			goto exit;

		if (sceKernelFindModuleByName("sceVshOSK_Module"))
			goto exit;

		if (sceKernelFindModuleByName("camera_plugin_module"))
			goto exit;

		if (!(ctrl->Buttons & PSP_CTRL_SELECT))
			goto exit;

		printk("%s: loading satelite\n", __func__);
		modid = load_satelite();

		if (modid >= 0) {
			g_satelite_mod_id = modid;
			modid = sceKernelStartModule(g_satelite_mod_id, 0, 0, 0, 0);

			if (modid < 0) {
				printk("%s: start module -> 0x%08X\n", __func__, modid);
			}

			ctrl->Buttons &= (~PSP_CTRL_SELECT); // Filter SELECT
		}
	}
	
exit:
	pspSdkSetK1(k1);
	
	return ret;
}
