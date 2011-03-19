#include <pspkernel.h>
#include <pspinit.h>
#include <pspiofilemgr.h>
#include <string.h>
#include "main.h"
#include "utils.h"
#include "printk.h"
#include "strsafe.h"

#define PLUGIN_PATH "ms0:/seplugins/"

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

	modid = sctrlKernelLoadModule(path, 0, NULL);

	if(modid < 0) {
		if(0 == strnicmp(path, "ef", 2)) {
			strncpy(path, "ms", 2);
		} else { 
			strncpy(path, "ef", 2);
		}

		modid = sctrlKernelLoadModule(path, 0, NULL);
	}

	if(conf.oldplugin && modid >= 0 && psp_model == PSP_GO && 0 == strnicmp(path, "ef", 2)) {
		patch_devicename(modid);
	}

	status = 0;
	ret = sctrlKernelStartModule(modid, strlen(path) + 1, path, &status, NULL);
	printk("%s: %s, UID: %08X, Status: 0x%08X\n", __func__, path, modid, status);

	return ret;
}

static char *get_line(int fd, char *linebuf, int bufsiz)
{
	int i, ret;

	if (linebuf == NULL || bufsiz < 2)
		return NULL;

	i = 0;
	memset(linebuf, 0, bufsiz);

	while (i < bufsiz - 1) {
		char c;

		ret = sceIoRead(fd, &c, 1);

		if (ret < 0 || (ret == 0 && i == 0))
			return NULL;

		if (ret == 0 || c == '\n' || c == '\r') {
			linebuf[i] = '\0';
			break;
		}

		linebuf[i++] = c;
	}

	linebuf[bufsiz-1] = '\0';

	return linebuf;
}

static void load_plugin(char * path)
{
	char linebuf[256], *p, *q;
	int fd, len;

	if (path == NULL)
		return;
	
	fd = sceIoOpen(path, PSP_O_RDONLY, 0777);

	if (fd < 0) {
		// retry on ef0
		strncpy(path, "ef", 2);
		fd = sceIoOpen(path, PSP_O_RDONLY, 0777);

		if(fd < 0) {
			printk("%s: open %s failed 0x%08X\n", __func__, path, fd);

			return;
		}
	}

	do {
		p = get_line(fd, linebuf, sizeof(linebuf));

		if(p == NULL)
			break;

		len = strlen(p);

		if(len == 0) {
			continue;
		}

		for(q=p; *q != ' ' && *q != '\t' && *q != '\0'; ++q) {
		}

		if (*q == '\0') {
			continue;
		}

		while(len >= 1) {
			if(p[len-1] == ' ' || p[len-1] == '\t')
				len--;
			else
				break;
		}

		if(p[len-1] == '1') {
			*q = '\0';
			printk("%s module path: %s\n", __func__, p);
			load_start_module(p);
		}
	} while (p != NULL);

	sceIoClose(fd);
}

int load_plugins(void)
{
	unsigned int key = sceKernelInitKeyConfig();

	char * bootconf = NULL;

	//visual shell
	if(conf.plugvsh && key == PSP_INIT_KEYCONFIG_VSH) {
		bootconf = PLUGIN_PATH "vsh.txt";
	} //game mode
	else if(conf.pluggame && key == PSP_INIT_KEYCONFIG_GAME) {
		bootconf = PLUGIN_PATH "game.txt";
	} //ps1 mode
	else if(conf.plugpop && key == PSP_INIT_KEYCONFIG_POPS) {
		bootconf = PLUGIN_PATH "pops.txt";
	}

	//load mode specific plugins
	load_plugin(bootconf);

	//return success
	return 0;
}
