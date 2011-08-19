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
#include "inferno.h"

#ifdef CONFIG_660
#include "march33_660.h"
#endif

#ifdef CONFIG_639
#include "march33.h"
#endif

#ifdef CONFIG_635
#include "march33.h"
#endif

#ifdef CONFIG_620
#include "march33_620.h"
#endif

#include "popcorn.h"
#include "satelite.h"
#include "stargate.h"
#include "systemctrl.h"
#include "usbdevice.h"
#include "vshctrl.h"
#include "recovery.h"
#include "config.h"

#include "../Permanent/ppatch_config.h"

// VSH module can write F0/F1
PSP_MODULE_INFO("PROUpdater", 0x0800, 1, 0);

/* Define the main thread's attribute value (optional) */
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

#define VERSION_STR "PRO-B"
#define printf pspDebugScreenPrintf

int psp_model = 0;
u32 psp_fw_version;
int disable_smart = 0;
static u8 g_buf[64*1024] __attribute__((aligned(64)));
static u8 g_buf2[64*1024] __attribute__((aligned(64)));

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
	printf(VERSION_STR " by Team PRO\n");
}

struct InstallList {
	u8 *buf;
	uint *size;
	char *dst;
};

struct InstallList g_file_lists[] = {
	{ systemctrl, &size_systemctrl, PATH_SYSTEMCTRL, },
	{ vshctrl, &size_vshctrl, PATH_VSHCTRL, },
	{ galaxy, &size_galaxy, PATH_GALAXY, },
	{ stargate, &size_stargate, PATH_STARGATE, },
	{ NULL, NULL, PATH_MARCH33, },
	{ inferno, &size_inferno, PATH_INFERNO, },
	{ usbdevice, &size_usbdevice, PATH_USBDEVICE, },
	{ popcorn, &size_popcorn, PATH_POPCORN, },
	{ satelite, &size_satelite, PATH_SATELITE, },
	{ recovery, &size_recovery, PATH_RECOVERY, },
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

	if(disable_smart) {
		sceIoRemove("flash1:/config.se");
	}

	for(i=0; i<NELEMS(g_old_cfw_files); ++i) {
		sceIoRemove(g_old_cfw_files[i]);
	}

#ifdef CONFIG_660
	if(psp_fw_version == FW_660) {
		g_file_lists[4].buf = march33_660;
		g_file_lists[4].size = &size_march33_660;
	}
#endif

#ifdef CONFIG_639
	if(psp_fw_version == FW_639) {
		g_file_lists[4].buf = march33;
		g_file_lists[4].size = &size_march33;
	}
#endif

#ifdef CONFIG_635
	if(psp_fw_version == FW_635) {
		g_file_lists[4].buf = march33;
		g_file_lists[4].size = &size_march33;
	}
#endif

#ifdef CONFIG_620
	if (psp_fw_version == FW_620) {
		g_file_lists[4].buf = march33_620;
		g_file_lists[4].size = &size_march33_620;
	}
#endif

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

int compare_file(const char *src, const char *dst)
{
	SceUID fd = -1, fdd = -1;
	int ret, ret2;
	SceIoStat srcstat, dststat;

	ret = sceIoGetstat(src, &srcstat);

	if (ret != 0) {
		goto not_equal;
	}

	ret = sceIoGetstat(dst, &dststat);

	if (ret != 0) {
		goto not_equal;
	}

	if (dststat.st_size != srcstat.st_size) {
		goto not_equal;
	}

	ret = sceIoOpen(src, PSP_O_RDONLY, 0777);

	if (ret < 0) {
		goto not_equal;
	}

	fd = ret;

	ret = sceIoOpen(dst, PSP_O_RDONLY, 0777);

	if (ret < 0) {
		goto not_equal;
	}

	fdd = ret;
	ret = sizeof(g_buf);
	ret = sceIoRead(fd, g_buf, ret);

	while (ret > 0) {
		ret2 = sceIoRead(fdd, g_buf2, ret);

		if (ret2 != ret) {
			goto not_equal;
		}

		if (memcmp(g_buf, g_buf2, ret)) {
			goto not_equal;
		}

		ret = sceIoRead(fd, g_buf, ret);
	}

	if (ret < 0) {
		goto not_equal;
	}

	sceIoClose(fd);
	sceIoClose(fdd);

	return 0;

not_equal:
	if (fd >= 0)
		sceIoClose(fd);

	if (fdd >= 0)
		sceIoClose(fdd);

	return 1;
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
	ret = sizeof(g_buf);
	ret = sceIoRead(fd, g_buf, ret);

	while (ret > 0) {
		ret = sceIoWrite(fdw, g_buf, ret);

		if (ret < 0) {
			goto error;
		}

		ret = sceIoRead(fd, g_buf, ret);
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

int is_file_exist(const char *path)
{
	SceIoStat stat;
	int ret;

	ret = sceIoGetstat(path, &stat);

	return ret == 0 ? 1 : 0;
}

int is_permanent_patch_installed(void)
{
	if(psp_fw_version != FW_620) {
		return 0;
	}

	if(!is_file_exist(VSHORIG)) {
		return 0;
	}

	if(0 == compare_file(VSHMAIN, VSHORIG)) {
		return 0;
	}

	return 1;
}

void uninstall_permanent_patch(void)
{
	int ret;

	do {
		ret = sceIoRemove(VSHMAIN);

		if(ret != 0) {
			printf("Delete %s failed\n", VSHMAIN);
			sceKernelDelayThread(1000000L);
		}
	} while(ret != 0);

	do {
		ret = copy_file(VSHORIG, VSHMAIN);

		if(ret != 0) {
			printf("Copy %s to %s failed 0x%08X\n", VSHORIG, VSHMAIN, ret);
			sceKernelDelayThread(1000000L);
		}
	} while(ret != 0);

	do {
		ret = sceIoRemove(VSHORIG);

		if(ret != 0) {
			printf("Delete %s failed\n", VSHTEMP);
			sceKernelDelayThread(1000000L);
		}
	} while(ret != 0);

	sceIoRemove(VSHTEMP);
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

	if(is_permanent_patch_installed()) {
		printf("Uninstalling permanent patch...");
		uninstall_permanent_patch();
		printf("OK\n");
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
	int ret = 0;
	struct SceIoStat stat;
	SceCtrlData ctl;
	u32 key;

	memset(&stat, 0, sizeof(stat));
	pspDebugScreenInit();
	psp_fw_version = sceKernelDevkitVersion();

#ifdef CONFIG_620
	if(psp_fw_version == FW_620) {
		goto version_OK;
	}
#endif

#ifdef CONFIG_635
	if(psp_fw_version == FW_635) {
		goto version_OK;
	}
#endif

#ifdef CONFIG_639
	if(psp_fw_version == FW_639) {
		goto version_OK;
	}
#endif
	
#ifdef CONFIG_660
	if(psp_fw_version == FW_660) {
		goto version_OK;
	}
#endif
	
	printf("Sorry. This program doesn't support your FW(0x%08X).\n", (uint)psp_fw_version);
	goto exit;
	
version_OK:
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
			printf("PSP GO BRITE Detected ....\n");
			break;
		case PSP_9000:
			printf("PSP BRITE 3000(09g) Detected ....\n");
			break;
		case PSP_7000:
			printf("PSP BRITE 3000(07g) Detected ....\n");
			break;
		case PSP_4000:
			printf("PSP BRITE 3000(04g) Detected ....\n");
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

			while (0 == (key & PSP_CTRL_CROSS)) {
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

	while (0 == (key & PSP_CTRL_CROSS)) {
		sceKernelDelayThread(50000);
		sceCtrlReadBufferPositive(&ctl, 1);
		key = ctl.Buttons;
	}

	cleanup_exit();
	sceKernelExitGame();

	return 0;
}
