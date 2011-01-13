#include <pspkernel.h>
#include <pspiofilemgr.h>
#include <pspctrl.h>
#include <pspsdk.h>
#include <psppower.h>
#include <stdio.h>
#include <string.h>
#include "kubridge.h"
#include "utils.h"

// VSH module can write F0/F1
PSP_MODULE_INFO("635PROUpdater", 0x0800, 1, 0);

/* Define the main thread's attribute value (optional) */
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

#define VERSION_STR "635PRO"
#define printf pspDebugScreenPrintf

#define PSP_GO   4
#define PSP_4000 3
#define PSP_3000 2
#define PSP_2000 1
#define PSP_1000 0

int psp_model = 0;

int copy_file(const char *src, const char *dst)
{
	SceUID fd = -1, fdw = -1;
	int ret;
	u8 buf[64*1024] __attribute__((aligned(64)));

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

int smart_copy_file(const char *src, const char *dst)
{
	int ret;
	SceIoStat srcstat, dststat;

	ret = sceIoGetstat(dst, &dststat);

	if (ret == 0) {
		ret = sceIoGetstat(src, &srcstat);

		if (ret == 0) {
			if (dststat.st_size == srcstat.st_size) {
				if (0 == memcmp(&dststat.st_ctime, &srcstat.st_ctime, sizeof(dststat.st_ctime)) &&
						0 == memcmp(&dststat.st_mtime, &srcstat.st_mtime, sizeof(dststat.st_mtime))) {
					return 0;
				}
			}
		}
	}

	printf("Writing %s...", dst);
	ret = copy_file(src, dst);

	if (ret == 0) {
		printf("OK\n");
	}

	return ret;
}

int write_file(const char *path, unsigned char *buf, int size)
{
	SceUID fd;
	int ret;

	fd = sceIoOpen(path, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);

	if (fd < 0) {
		goto error;
	}

	ret = sceIoWrite(fd, buf, size);

	if (ret < 0) {
		goto error;
	}

	sceIoClose(fd);

	return 0;
error:
	if (fd >= 0)
		sceIoClose(fd);

	return -1;
}

void init_flash()
{
	sceIoUnassign("flash0:");
	sceIoAssign("flash0:", "lflash0:0,0", "flashfat0:", 0, NULL, 0);
	sceIoUnassign("flash1:");
	sceIoAssign("flash1:", "lflash0:0,1", "flashfat1:", 0, NULL, 0);
}

void usage(void)
{
	printf(VERSION_STR " by Coldbird&VF\n");
}

struct CopyList {
	char *src;
	char *dst;
};

struct CopyList g_file_lists[] = {
	{ "systemctrl.prx", "flash0:/kd/systemctrl.prx", },
	{ "vshctrl.prx", "flash0:/kd/vshctrl.prx", },
	{ "galaxy.prx", "flash0:/kd/galaxy.prx", },
	{ "stargate.prx", "flash0:/kd/stargate.prx", },
	{ "march33.prx", "flash0:/kd/march33.prx", },
};

int install_cfw(void)
{
	int ret;

	int i; for(i=0; i<NELEMS(g_file_lists); ++i) {
		ret = smart_copy_file(g_file_lists[i].src, g_file_lists[i].dst);

		if (ret != 0)
			goto exit;
	}

	// per model install goes here:
	switch(psp_model) {
		case PSP_GO:
			break;
		case PSP_4000:
			break;
		case PSP_3000:
			break;
		case PSP_2000:
			break;
		case PSP_1000:
			break;
	}

	return 0;

exit:
	printf("\nCopy file error (0x%08x)! Install aborted.\n", ret);

	return -1;
}

/**
 * mode: 2 for psp3000, 1 for psp2000v3
 */
void start_reboot(int mode)
{
	int modid, ret;
	int delay = 0;
	char modpath[80];

	sprintf(modpath, "rebootex.prx");
	modid = kuKernelLoadModule(modpath, 0, 0);

	if (modid >= 0) {
		ret = sceKernelStartModule(modid, 0, NULL, 0, NULL);

		if (ret < 0) {
			printf("start module error 0x%08x\n", ret);
			delay = 50000000;
		}
	} else {
		printf("load module error 0x%08x\n", modid);
		delay = 50000000;
	}

	sceKernelDelayThread(delay);
	sceKernelExitGame();
}

int main(int argc, char *argv[])
{
	int ret = 0, fw_version;
	struct SceIoStat stat;
	SceCtrlData ctl;
	u32 key;

	memset(&stat, 0, sizeof(stat));
	pspDebugScreenInit();
	fw_version = sceKernelDevkitVersion();
	
	if (fw_version != 0x06030510) {
		printf("This program requires 6.35 Sorry.\n");
		goto exit;
	}

	psp_model = kuKernelGetModel();
	scePowerSetClockFrequency(333, 333, 166);
	init_flash();
	usage();

	printf("Press X to install CFW.\n");
	printf("Press R to exit.\n");

	sceCtrlReadBufferPositive(&ctl, 1);
	key = ctl.Buttons;

	while (key != PSP_CTRL_CROSS && key != PSP_CTRL_RTRIGGER) {
		sceKernelDelayThread(50000);
		sceCtrlReadBufferPositive(&ctl, 1);
		key = ctl.Buttons;
	}

	if (key == PSP_CTRL_RTRIGGER) {
		printf("Exiting...\n");
		sceKernelDelayThread(100000);
		sceKernelExitGame();
	}

	switch(psp_model) {
		case PSP_GO:
			printf("PSP BRITE N-1000(psp-go) Detected ....\n");
			break;
		case PSP_4000:
			printf("PSP BRITE 4000 Detected ....\n");
			break;
		case PSP_3000:
			printf("PSP BRITE 3000 Detected ....\n");
			break;
		case PSP_2000:
			printf("PSP SLIM 2000 Detected ....\n");
			break;
		case PSP_1000:
			printf("PSP FAT 1000 Detected ....\n");
			break;
		default:
			printf("Unkown PSP model 0%dg\n", psp_model+1);
			break;
	}

	printf("Install...\n");
	ret = install_cfw();

	if (ret == 0) {
		printf(" Completed.\nPress X to start CFW.\n");

		sceCtrlReadBufferPositive(&ctl, 1);
		key = ctl.Buttons;

		while (key != PSP_CTRL_CROSS) {
			sceKernelDelayThread(50000);
			sceCtrlReadBufferPositive(&ctl, 1);
			key = ctl.Buttons;
		}

		printf("Now reboot to " VERSION_STR " :)\n");
		start_reboot(psp_model);
	} else {
exit:
		printf("Press X to exit.\n");

		sceCtrlReadBufferPositive(&ctl, 1);
		key = ctl.Buttons;

		while (key != PSP_CTRL_CROSS) {
			sceKernelDelayThread(50000);
			sceCtrlReadBufferPositive(&ctl, 1);
			key = ctl.Buttons;
		}

		sceKernelExitGame();
	}

	printf("Now reboot to %s\n", VERSION_STR);
	start_reboot(psp_model);

	return 0;
}

