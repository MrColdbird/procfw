#include <pspkernel.h>
#include <pspinit.h>
#include <pspiofilemgr.h>
#include <string.h>
#include "main.h"
#include "utils.h"
#include "printk.h"
#include "strsafe.h"

#define PLUGIN_PATH "ms0:/seplugins/"

int load_start_module(char *path)
{
	int ret;
	SceUID modid;
	int status;

	modid = sceKernelLoadModule(path, 0, NULL);

	if(modid < 0 && psp_model == PSP_GO) {
		strncpy(path, "ef0", 3);
		modid = sceKernelLoadModule(path, 0, NULL);
	}

	status = 0;
	ret = sceKernelStartModule(modid, strlen(path) + 1, path, &status, NULL);
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
	char linebuf[256], *p;
	int fd;

	if (path == NULL)
		return;
	
	fd = sceIoOpen(path, PSP_O_RDONLY, 0777);

	if (fd < 0) {
		// retry on ef0
		strncpy(path, "ef0", 3);
		fd = sceIoOpen(path, PSP_O_RDONLY, 0777);

		if(fd < 0) {
			printk("%s: open %s failed 0x%08X\n", __func__, path, fd);

			return;
		}
	}

	do {
		p = get_line(fd, linebuf, sizeof(linebuf));
		
		if (p != NULL) {
			int len;

			printk("%s: %s\n", __func__, p);
			len = strlen(p);

			if (len >= 1 && p[len-1] == '1') {
				char *q;

				q=strrchr(p, ' ');

				if (q != NULL) {
					char mod_path[256];

					memset(mod_path, 0, sizeof(mod_path));
					strncpy_s(mod_path, sizeof(mod_path), p, q-p);
					printk("%s module path: %s\n", __func__, mod_path);
					load_start_module(mod_path);
				}
			}
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
