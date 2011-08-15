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
#include <pspsysmem_kernel.h>
#include <pspkernel.h>
#include <psputilsforkernel.h>
#include <pspsysevent.h>
#include <pspiofilemgr.h>
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "utils.h"
#include "systemctrl.h"
#include "printk.h"
#include "rebootex_conf.h"
#include "libs.h"
#include "systemctrl_patch_offset.h"

static char g_version_buf[5];
static char g_cur_version_buf[5];

static char *_strncpy(char * dst, const char * src, size_t num)
{
	printk("%s: %s %u\n", __func__, src, num);

	if(0 == strcmp(src, g_cur_version_buf)) {
		printk("%s: patched with %s\n", __func__, g_version_buf);

		return strncpy(dst, g_version_buf, num);
	}

	return strncpy(dst, src, num);
}

static int load_usb_version_text(void)
{
	const char *path;
	SceUID fd;
	
	path = "ms0:/seplugins/usbversion.txt";
	fd = sceIoOpen(path, PSP_O_RDONLY, 0777);

	if (fd < 0) {
		path = "ef0:/seplugins/usbversion.txt";
		fd = sceIoOpen(path, PSP_O_RDONLY, 0777);

		if (fd < 0) {
			return fd;
		}
	}

	if (4 != sceIoRead(fd, g_version_buf, 4)) {
		sceIoClose(fd);
		return -1;
	}

	sceIoClose(fd);
	
	if (!isdigit(g_version_buf[0]) || g_version_buf[1] != '.' || !isdigit(g_version_buf[2]) || !isdigit(g_version_buf[3])) {
		return -2;
	}

	g_version_buf[4] = '\0';

	g_cur_version_buf[0] = (psp_fw_version >> 24) + '0';
	g_cur_version_buf[1] = '.';
	g_cur_version_buf[2] = (psp_fw_version >> 16) + '0';
	g_cur_version_buf[3] = (psp_fw_version >> 8) + '0';
	g_cur_version_buf[4] = '\0';

	return 0;
}

void patch_sceUSB_Stor_Ms_Driver(SceModule* mod)
{
	if (load_usb_version_text() >= 0) {
		hook_import_bynid(mod, "SysclibForKernel", 0xB49A7697, &_strncpy, 0);
	}
}
