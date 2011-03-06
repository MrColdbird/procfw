#include <pspkernel.h>
#include <pspiofilemgr.h>
#include <pspctrl.h>
#include <pspsdk.h>
#include <psppower.h>
#include <stdio.h>
#include <string.h>
#include "kubridge.h"
#include "utils.h"

#include "galaxy.h"
#include "march33.h"
#include "popcorn.h"
#include "satelite.h"
#include "category.h"
#include "stargate.h"
#include "systemctrl.h"
#include "usbdevice.h"
#include "vshctrl.h"
#include "recovery.h"

// VSH module can write F0/F1
PSP_MODULE_INFO("635PROUpdater", 0x0800, 1, 0);

/* Define the main thread's attribute value (optional) */
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

#define VERSION_STR "635PRO-B"
#define printf pspDebugScreenPrintf

int psp_model = 0;
int disable_smart = 0;
static u8 g_buf[64*1024] __attribute__((aligned(64)));

void cleanup_exit(void)
{
	sceIoRemove("installer.prx");
	sceIoRemove("Rebootex.prx");
}

int compare_file_buffer(const char *path, void *file_buf, int size) 
{
	SceUID fd = -1;
	int ret;
	SceIoStat srcstat;

	ret = sceIoGetstat(path, &srcstat);
	
	if (ret != 0) {
		goto not_equal;
	}

	if (srcstat.st_size != size) {
		goto not_equal;
	}

	ret = sceIoOpen(path, PSP_O_RDONLY, 0777);

	if (ret < 0) {
		goto not_equal;
	}

	fd = ret;

	ret = sizeof(g_buf);
	ret = sceIoRead(fd, g_buf, ret);

	while (ret > 0) {
		if (memcmp(g_buf, file_buf, ret)) {
			goto not_equal;
		}

		file_buf += ret;
		ret = sceIoRead(fd, g_buf, ret);
	}

	if (ret < 0) {
		goto not_equal;
	}

	sceIoClose(fd);

	return 0;

not_equal:
	if (fd >= 0)
		sceIoClose(fd);

	return 1;
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

int smart_write_file(const char *path, unsigned char *buf, int size)
{
	int ret;

	if (!disable_smart) {
		ret = compare_file_buffer(path, buf, size);

		if (ret == 0) {
			return 0;
		}
	}

	printf("Writing %s...", path);
	ret = write_file(path, buf, size);

	if (ret == 0) {
		printf("OK\n");
	}

	return ret;
}

void init_flash()
{
	int ret;
   
	ret = sceIoUnassign("flash0:");

	while(ret < 0) {
		ret = sceIoUnassign("flash0:");
		sceKernelDelayThread(500000);
	}

	sceIoAssign("flash0:", "lflash0:0,0", "flashfat0:", 0, NULL, 0);
	sceIoUnassign("flash1:");
	sceIoAssign("flash1:", "lflash0:0,1", "flashfat1:", 0, NULL, 0);
}

void usage(void)
{
	printf(VERSION_STR " by Coldbird&VF\n");
}

struct InstallList {
	u8 *buf;
	u32 *size;
	char *dst;
};

struct InstallList g_file_lists[] = {
	{ systemctrl, &size_systemctrl, "flash0:/kd/_systemctrl.prx", },
	{ vshctrl, &size_vshctrl, "flash0:/kd/_vshctrl.prx", },
	{ galaxy, &size_galaxy, "flash0:/kd/_galaxy.prx", },
	{ stargate, &size_stargate, "flash0:/kd/_stargate.prx", },
	{ march33, &size_march33, "flash0:/kd/_march33.prx", },
	{ usbdevice, &size_usbdevice, "flash0:/kd/_usbdevice.prx", },
	{ popcorn, &size_popcorn, "flash0:/kd/_popcorn.prx", },
	{ satelite, &size_satelite, "flash0:/vsh/module/_satelite.prx", },
	{ category, &size_category, "flash0:/vsh/module/_category.prx", },
	{ recovery, &size_recovery, "flash0:/vsh/module/_recovery.prx", },
};

static const char *g_old_cfw_files[] = {
	"flash0:/kd/systemctrl.prx",
	"flash0:/kd/vshctrl.prx",
	"flash0:/kd/galaxy.prx",
	"flash0:/kd/stargate.prx",
	"flash0:/kd/march33.prx",
	"flash0:/kd/usbdevice.prx",
	"flash0:/vsh/module/satelite.prx",
};

int install_cfw(void)
{
	int ret, i;

//	sceIoRemove("flash1:/config.se");

	for(i=0; i<NELEMS(g_old_cfw_files); ++i) {
		sceIoRemove(g_old_cfw_files[i]);
	}

	for(i=0; i<NELEMS(g_file_lists); ++i) {
		ret = smart_write_file(g_file_lists[i].dst, g_file_lists[i].buf, *g_file_lists[i].size);

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
	printf("\nWrite file error (0x%08x)! Install aborted.\n", ret);

	return -1;
}

int uninstall_cfw(void)
{
	int ret;

	sceIoRemove("flash1:/config.se");

	int i; for(i=0; i<NELEMS(g_file_lists); ++i) {
		printf("Removing %s...", g_file_lists[i].dst);
		ret = sceIoRemove(g_file_lists[i].dst);

		if(ret == 0 || ret == 0x80010002) {
			printf("OK\n");
		} else {
			printf("failed(0x%08X)\n", ret);
		}
	}

	// per model uninstall goes here:
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
}

/**
 * mode: 0 - OFW mode, 1 - CFW mode
 */
void start_reboot(int mode)
{
	int modid, ret;
	int delay = 0;
	char modpath[80];

	sprintf(modpath, "rebootex.prx");
	modid = kuKernelLoadModule(modpath, 0, 0);

	if (modid >= 0) {
		ret = sceKernelStartModule(modid, sizeof(mode), &mode, 0, NULL);

		if (ret < 0) {
			printf("start module error 0x%08x\n", ret);
			delay = 5000000;
		}
	} else {
		printf("load module error 0x%08x\n", modid);
		delay = 5000000;
	}

	cleanup_exit();
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
		printf("Sorry. This program requires 6.35.\n");
		goto exit;
	}

	psp_model = kuKernelGetModel();
	scePowerSetClockFrequency(333, 333, 166);
	init_flash();
	usage();

	printf("Press X to launch CFW.\n");
	printf("Press Triangle to uninstall CFW.\n");
	printf("Hold L to reinstall CFW.\n");
	printf("Press R to exit.\n");

	sceCtrlReadBufferPositive(&ctl, 1);
	key = ctl.Buttons;

	while (0 == (key & (PSP_CTRL_CROSS | PSP_CTRL_RTRIGGER | PSP_CTRL_TRIANGLE))) {
		sceKernelDelayThread(50000);
		sceCtrlReadBufferPositive(&ctl, 1);
		key = ctl.Buttons;
	}

	if (key & PSP_CTRL_RTRIGGER) {
		printf("Exiting...\n");
		cleanup_exit();
		sceKernelDelayThread(100000);
		sceKernelExitGame();
	}

	switch(psp_model) {
		case PSP_GO:
			printf("PSP BRITE N-1000(psp-go) Detected ....\n");
			break;
		case PSP_9000:
			printf("PSP BRITE 9000 Detected ....\n");
			break;
		case PSP_7000:
			printf("PSP BRITE 7000 Detected ....\n");
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

	sceCtrlReadBufferPositive(&ctl, 1);

	if (ctl.Buttons & PSP_CTRL_LTRIGGER) {
		disable_smart = 1;
	}

	if (key & PSP_CTRL_CROSS) {
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
			start_reboot(1);
		}
	} else if (key & PSP_CTRL_TRIANGLE) {
		ret = uninstall_cfw();
		printf("Now reboot to OFW :)\n");
		sceKernelDelayThread(1000000);
		start_reboot(0);
	}

exit:
	printf("Press X to exit.\n");

	sceCtrlReadBufferPositive(&ctl, 1);
	key = ctl.Buttons;

	while (key != PSP_CTRL_CROSS) {
		sceKernelDelayThread(50000);
		sceCtrlReadBufferPositive(&ctl, 1);
		key = ctl.Buttons;
	}

	cleanup_exit();
	sceKernelExitGame();

	return 0;
}
