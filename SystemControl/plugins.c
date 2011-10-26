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
#include <pspinit.h>
#include <pspiofilemgr.h>
#include <string.h>
#include <pspctrl.h>
#include "main.h"
#include "utils.h"
#include "printk.h"
#include "strsafe.h"
#include "rebootex_conf.h"

#define WAIT_MEMORY_STICK_TIMEOUT ( 2 * 1000000 )

static void wait_memory_stick_ready_timeout(int wait);

static void patch_devicename(SceUID modid)
{
	SceModule2 *mod;
	int i;

	mod = (SceModule2*)sctrlKernelFindModuleByUID(modid);

	if(mod == NULL) {
		return;
	}

	for(i=0; i<mod->nsegment; ++i) {
		u32 addr;
		u32 end;

		end = mod->segmentaddr[i] + mod->segmentsize[i];

		for(addr = mod->segmentaddr[i]; addr < end; addr += 4) {
			char *str = (char*)addr;

			if (0 == strncmp(str, "ms0", 3)) {
				str[0] = 'e';
				str[1] = 'f';
			} else if (0 == strncmp(str, "fatms", 5)) {
				str[3] = 'e';
				str[4] = 'f';
			}
		}
	}

	sync_cache();
}

int load_start_module(char *path)
{
	int ret;
	SceUID modid;
	int status;
	int bootfrom;

	bootfrom = sctrlKernelBootFrom();

	if(psp_model == PSP_GO) {
		if(0 == strncmp(path, "ef", 2) ||
				0 == strncmp(path, "ms", 2)) {
			if(bootfrom == 0x50) {
				strncpy(path, "ef", 2);
			} else if(bootfrom == 0x40) {
				strncpy(path, "ms", 2);
			}
		}
	}

	modid = sctrlKernelLoadModule(path, 0, NULL);

	if(modid < 0) {
		printk("%s: load module %s -> 0x%08X\n", __func__, path, modid);

		return modid;
	}

	if(conf.oldplugin && psp_model == PSP_GO && 0 == strnicmp(path, "ef", 2)) {
		patch_devicename(modid);
	}

	status = 0;
	ret = sctrlKernelStartModule(modid, strlen(path) + 1, path, &status, NULL);
	printk("%s: %s, UID: %08X, Status: 0x%08X\n", __func__, path, modid, status);

	return ret;
}

#define READ_BUF_SIZE 1024

static char *read_buf = NULL;
static char *read_ptr = NULL;
static int read_cnt = 0;

static int buf_read(SceUID fd, char *p)
{
	if(read_cnt <= 0) {
		read_cnt = sceIoRead(fd, read_buf, READ_BUF_SIZE);

		if(read_cnt < 0) {
			return read_cnt;
		}

		if(read_cnt == 0) {
			return read_cnt;
		}

		read_ptr = read_buf;
	}

	read_cnt--;
	*p = *read_ptr++;

	return 1;
}

static int read_lines(SceUID fd, char *lines, size_t linebuf_size)
{
	char *p;
	int ret;
	size_t re;

	if(linebuf_size == 0) {
		return -1;
	}

	p = lines;
	re = linebuf_size;

	while(re -- != 0) {
		ret = buf_read(fd, p);

		if(ret < 0) {
			break;
		}

		if(ret == 0) {
			if(p == lines) {
				ret = -1;
			}

			break;
		}

		if(*p == '\r') {
			continue;
		}

		if(*p == '\n') {
			break;
		}

		p++;
	}

	if(p < lines + linebuf_size) {
		*p = '\0';
	}

	return ret >= 0 ? p - lines : ret;
}

static inline int ourisspace(char ch)
{
	if(ch == '\t' || ch == ' ')
		return 1;

	return 0;
}

static void parse_plugin(char *linebuf)
{
	char *p;
	int enabled = 1;

	for(p=linebuf; *p != '\0'; p++) {
		if(ourisspace(*p)) {
			*p++ = '\0';
			enabled = 0;

			while(*p != '\0' && ourisspace(*p)) {
				p++;
			}

			if(*p == '1') {
				enabled = 1;
			}

			break;
		}
	}

	if(enabled) {
		load_start_module(linebuf);
	}
}

static void load_plugin(char * path, int wait)
{
	char linebuf[256];
	int fd;
	char *read_alloc_buf;

	if (path == NULL)
		return;

	if(wait && 0 == strncmp(path, "ms", 2)) {
		wait_memory_stick_ready_timeout(wait);
	}

	fd = sceIoOpen(path, PSP_O_RDONLY, 0777);

	if(fd < 0) {
		printk("%s: open %s failed 0x%08X\n", __func__, path, fd);

		return;
	}

	read_alloc_buf = oe_malloc(READ_BUF_SIZE + 64);

	if(read_alloc_buf == NULL) {
		sceIoClose(fd);
		return;
	}

	read_buf = (void*)(((u32)read_alloc_buf & (~(64-1))) + 64);
	linebuf[sizeof(linebuf)-1] = '\0';

	while(read_lines(fd, linebuf, sizeof(linebuf)-1) >= 0) {
		parse_plugin(linebuf);
	}

	sceIoClose(fd);
	oe_free(read_alloc_buf);
}

static void wait_memory_stick_ready_timeout(int wait)
{
	int retries = 0;
	SceUID dfd;

	dfd = -1;

	while(retries < wait / 100000) {
		dfd = sceIoDopen("ms0:/");

		if(dfd >= 0) {
			break;
		}

		sceKernelDelayThread(100000);
		retries++;
	}

	if(dfd >= 0) {
		sceIoDclose(dfd);
	}
}

static int is_vsh_plugins_enabled(void)
{
	SceCtrlData pad;
	int ret;

	ret = sctrlReadBufferPositive(&pad, 1);

	if(ret >= 0 && pad.Buttons & PSP_CTRL_RTRIGGER) {
		return 0;
	}

	return 1;
}

int load_plugins(void)
{
	unsigned int key = sceKernelApplicationType();

	if(rebootex_conf.recovery_mode) {
		return 0;
	}

	if(conf.plugvsh && key == PSP_INIT_KEYCONFIG_VSH && is_vsh_plugins_enabled()) {
		if(psp_model == PSP_GO) {
			load_plugin("ef0:/seplugins/vsh.txt", WAIT_MEMORY_STICK_TIMEOUT);
		}

		// pspgo has smaller wait time
		load_plugin("ms0:/seplugins/vsh.txt", psp_model == PSP_GO ? WAIT_MEMORY_STICK_TIMEOUT / 10 : WAIT_MEMORY_STICK_TIMEOUT);
	} //game mode
	else if(conf.pluggame && key == PSP_INIT_KEYCONFIG_GAME) {
		if(psp_model == PSP_GO && sctrlKernelBootFrom() == 0x50) {
			load_plugin("ef0:/seplugins/game.txt", WAIT_MEMORY_STICK_TIMEOUT);
		} else {
			load_plugin("ms0:/seplugins/game.txt", WAIT_MEMORY_STICK_TIMEOUT);
		}
	} //ps1 mode
	else if(conf.plugpop && key == PSP_INIT_KEYCONFIG_POPS) {
		if(psp_model == PSP_GO && sctrlKernelBootFrom() == 0x50) {
			load_plugin("ef0:/seplugins/pops.txt", WAIT_MEMORY_STICK_TIMEOUT);
		} else {
			load_plugin("ms0:/seplugins/pops.txt", WAIT_MEMORY_STICK_TIMEOUT);
		}
	}

	//return success
	return 0;
}
