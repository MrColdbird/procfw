#include <stdio.h>
#include <string.h>

#include <pspinit.h>
#include <pspctrl.h>
#include <pspsdk.h>
#include <pspkernel.h>
#include <psputility.h>
#include <psputilsforkernel.h>

#include "utils.h"
#include "../ppatch_config.h"
#include "systemctrl.h"
#include "kubridge.h"
#include "config.h"

PSP_MODULE_INFO("HEN", 0x800, 1, 0);
PSP_HEAP_SIZE_KB(0);

extern int start_exploit(int recovery_mode, int ofw_mode);
extern int scePowerRequestColdReset(int unk);

static u8 buf[1024] __attribute__((aligned(64)));

void init_flash(void)
{
	sceIoUnassign("flash0:");
	sceIoAssign("flash0:", "lflash0:0,0", "flashfat0:", 0, NULL, 0);
	sceIoUnassign("flash1:");
	sceIoAssign("flash1:", "lflash0:0,1", "flashfat1:", 0, NULL, 0);
}

int copy_file(const char *src, const char *dst)
{
	SceUID fd = -1, fdw = -1;
	int ret;

	ret = sceIoOpen(src, PSP_O_RDONLY, 0777);

	if (ret < 0) {
		goto error;
	}

	fd = ret;

	ret = sceIoOpen(dst, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);

	if (ret < 0) {
		goto error;
	}

	fdw = ret;
	ret = sizeof(buf);
	ret = sceIoRead(fd, buf, ret);

	while (ret > 0) {
		ret = sceIoWrite(fdw, buf, ret);

		if (ret < 0) {
			goto error;
		}

		ret = sceIoRead(fd, buf, ret);
	}

	if (ret < 0) {
		goto error;
	}

	sceIoClose(fd);
	sceIoClose(fdw);

	return 0;

error:
	sceIoClose(fd);
	sceIoClose(fdw);

	return ret;
}

static void uninstall_fake_vsh(void)
{
	SceIoStat stat;
	init_flash();

	if(sceIoGetstat(VSHORIG, &stat) == 0) {
		sceIoRemove(VSHMAIN);
		copy_file(VSHORIG, VSHMAIN);
		sceIoRemove(VSHORIG);
		sceIoRemove(VSHTEMP);
	}
}

static SceUID load_start_module(char *path)
{
	SceUID modid;

	pspDebugScreenInit();

	modid = kuKernelLoadModule(path, 0, NULL);
	pspDebugScreenPrintf("load module -> 0x%08X\n", modid);
	modid = sceKernelStartModule(modid, strlen(path) + 1, path, NULL, NULL);
	pspDebugScreenPrintf("start module -> 0x%08X\n", modid);

	return modid;
}

// first exploit and disable vshorig replace,
// and then launch recovery
static int launch_recovery(void)
{
	if(sctrlHENGetVersion() == 0x8002013A) {
		return -1;
	}

	return load_start_module(PATH_RECOVERY);
}

int main(void)
{
	SceCtrlData ctl;
	u32 key;
	int recovery_mode = 0, ofw_mode = 0;
	
	sceCtrlReadBufferPositive(&ctl, 1);
	key = ctl.Buttons;

	if((key & (PSP_CTRL_CIRCLE | PSP_CTRL_CROSS | PSP_CTRL_SELECT | PSP_CTRL_START)) == (PSP_CTRL_CIRCLE | PSP_CTRL_CROSS | PSP_CTRL_SELECT | PSP_CTRL_START)) {
		uninstall_fake_vsh();
		sceKernelDelayThread(100000);
		scePowerRequestColdReset(0);

		return 0;
	} else if(key & PSP_CTRL_RTRIGGER) {
		recovery_mode = 1;

		if (launch_recovery() >= 0) {
			return 0;
		}
	} else if(key & PSP_CTRL_SELECT) {
		ofw_mode = 1;
	}

	start_exploit(recovery_mode, ofw_mode);
	
	return 0;
}
