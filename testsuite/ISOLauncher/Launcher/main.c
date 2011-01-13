#include <pspkernel.h>
#include <stdio.h>
#include <string.h>
#include <systemctrl.h>
#include <systemctrl_se.h>
#include "utils.h"
#include "printk.h"
#include "libs.h"
#include "utils.h"
#include "loader_conf.h"
#include "strsafe.h"

PSP_MODULE_INFO("PROIsoLoader", 0x1000, 1, 1);

struct PLoaderConf g_conf;

void launch_game(void)
{
	struct SceKernelLoadExecVSHParam param;
	SceSize args; 
	void *argp;
	char *eboot = "disc0:/PSP_GAME/SYSDIR/EBOOT.BIN";
	int ret;

	if (g_conf.psp_model == PSP_GO) {
		strncpy(g_conf.iso_path, "ef0", sizeof("ef0")-1);
	}

	sctrlSESetUmdFile(g_conf.iso_path);
	sctrlSESetBootConfFileIndex(g_conf.iso_mode);

	//full memory doesn't hurt on isos
	sctrlHENSetMemory(48, 0);

	args = sizeof(eboot);
	argp = eboot;
	memset(&param, 0, sizeof(param));
	param.size = sizeof(param);
	param.args = args;
	param.argp = argp;
	param.key = "game";

	ret = sceKernelLoadExecVSHDisc(eboot, &param);
	printk("sceKernelLoadExecVSHDisc returns 0x%08X\n", ret);
}

int main_thread(SceSize args, void *argp)
{
	printk("Launcher started\n");
	launch_game();
	sctrlKernelExitVSH(NULL);

	return 0;
}

int module_start(SceSize args, void* argp)
{
	int thid;

	if (args != sizeof(g_conf)) {
		return -1;
	}

	memcpy(&g_conf, argp, args);
	printk_init("ms0:/log_launcher.txt");
	thid = sceKernelCreateThread("LauncherThread", main_thread, 0x1A, 0x8000, 0, NULL);

	if(thid>=0) {
		sceKernelStartThread(thid, args, argp);
	}

	return 0;
}
