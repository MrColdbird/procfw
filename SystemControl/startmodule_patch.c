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
#include "main.h"
#include "utils.h"
#include "printk.h"
#include "strsafe.h"
#include "libs.h"
#include "systemctrl_patch_offset.h"

static int plugin_loaded = 0;
static int opnssmp_loaded = 0;
static u32 sceInit_text_addr = 0;
static int (*custom_start_module)(int modid, SceSize argsize, void *argp, int *modstatus, SceKernelSMOption *opt);

static int get_game_tag(const char *path, u32 *tag)
{
	SceUID fd;
	char buf[0x150];
	int ret;
	u32 *magic;

	*tag = (u32)-1;

	if(path == NULL) {
		return -1;
	}

	fd = sceIoOpen(path, PSP_O_RDONLY, 0777);

	if(fd < 0) {
		return fd;
	}

	ret = sceIoRead(fd, buf, sizeof(buf));
	
	if(ret != sizeof(buf)) {
		ret = -2;
		goto exit;
	}

	magic = (u32*)buf;

	if(*magic != 0x5053507E) /* ~PSP */ {
		ret = -3;
		goto exit;
	}

	*tag = *(u32*)(buf+0xD0);
	ret = 0;

exit:
	sceIoClose(fd);

	return ret;
}

static int load_opnssmp(const char *path, u32 tag)
{
	SceUID modid;
	int opnssmp_type, ret;
	char opnssmp_path[128], *p;

	opnssmp_type = (tag >> 8) & 0xFF;
	STRCPY_S(opnssmp_path, path);
	p = strrchr(opnssmp_path, '/'); 
	
	if (p != NULL) {
		p[1] = '\0';
	} else {
		opnssmp_path[0] = '\0';
	}

	STRCAT_S(opnssmp_path, "OPNSSMP.BIN");
	modid = sctrlKernelLoadModule(opnssmp_path, 0, NULL);

	if(modid < 0) {
		printk("%s: load %s -> 0x%08X\n", __func__, opnssmp_path, modid);

		return modid;
	}

	ret = sctrlKernelStartModule(modid, 4, &opnssmp_type, 0, NULL);

	if(ret < 0) {
		printk("%s: start module -> 0x%08X\n", __func__, modid);
		sctrlKernelUnloadModule(modid);

		return ret;
	}

	return modid;
}

static int _sceKernelStartModule(int modid, SceSize argsize, void *argp, int *modstatus, SceKernelSMOption *opt)
{
	int ret;
	SceModule2 *mod, *mediasync, *kernellibrary;

	mod = (SceModule2*) sctrlKernelFindModuleByUID(modid);

	if(custom_start_module != NULL) {
		ret = custom_start_module(modid, argsize, argp, modstatus, opt);

		if(ret >= 0) {
			return ret;
		}
	}

	if(!plugin_loaded) {
		mediasync = (SceModule2*)sctrlKernelFindModuleByName("sceMediaSync");

		if(mediasync == NULL) {
			goto out;
		}

		load_plugins();
		plugin_loaded = 1;
	}

	if(conf.skiplogo && mod != NULL && 0 == strcmp(mod->modname, "vsh_module")) {
		u32* vshmain_args = oe_malloc(1024);

		memset(vshmain_args, 0, 1024);

		if(argp != NULL && argsize != 0 ) {
			memcpy( vshmain_args , argp ,  argsize);
		}

		vshmain_args[0] = 1024;
		vshmain_args[1] = 0x20;
		vshmain_args[16] = 1;

		ret = sctrlKernelStartModule(modid, 1024, vshmain_args, modstatus, opt);
		oe_free(vshmain_args);

		return ret;
	}

	if(!opnssmp_loaded) {
		u32 key = sceKernelApplicationType();

		if (key == PSP_INIT_KEYCONFIG_GAME) {
			kernellibrary = (SceModule2*)sctrlKernelFindModuleByName("sceKernelLibrary");

			if(kernellibrary != NULL) {
				const char *path;
				u32 tag;

				path = sceKernelInitFileName();

				if(path == NULL) {
					goto out;
				}

				ret = get_game_tag(path, &tag);

				if (ret == 0 && tag != (u32)-1) {
					printk("%s: tag 0x%08X\n", __func__, (uint)tag);
					load_opnssmp(path, tag);
				} else {
					printk("%s: get_game_tag -> 0x%08X\n", __func__, ret);
				}

				opnssmp_loaded = 1;
			}
		}
	}

out:
	ret = sctrlKernelStartModule(modid, argsize, argp, modstatus, opt);

	return ret;
}

static int patch_sceKernelStartModule_in_bootstart(int (*bootstart)(SceSize, void*), void *argp)
{
	u32 import;

	// patch sceInit with offset between module_bootstart and sceKernelStartModule
	import = ((u32)bootstart) + g_offs->init_patch.sceKernelStartModuleImport - g_offs->init_patch.module_bootstart;
	REDIRECT_FUNCTION(_sceKernelStartModule, import);
	sceInit_text_addr = ((u32)bootstart) - g_offs->init_patch.module_bootstart;
	sync_cache();

	return (*bootstart)(4, argp);
}

void patch_sceKernelStartModule(u32 loadcore_text_addr)
{
	_sw(MAKE_CALL(patch_sceKernelStartModule_in_bootstart), loadcore_text_addr + g_offs->loadercore_patch.sceInitBootStartCall);
	_sw(0x02E02021, loadcore_text_addr + g_offs->loadercore_patch.sceInitBootStartCall + 4); // move $a0, $s7
}

// AKA SystemCtrlForKernel_72F29A6E in 5.00M33
u32 sctrlGetInitTextAddr(void)
{
	return sceInit_text_addr;
}

void sctrlSetCustomStartModule(int (*func)(int modid, SceSize argsize, void *argp, int *modstatus, SceKernelSMOption *opt))
{
	custom_start_module = func;
}
