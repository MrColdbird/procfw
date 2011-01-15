#include <pspkernel.h>
#include <pspinit.h>
#include <pspiofilemgr.h>
#include <string.h>
#include "main.h"
#include "utils.h"
#include "printk.h"

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

void load_plugins(char * path)
{
	if(psp_model == PSP_GO && (sceKernelInitKeyConfig() == PSP_INIT_KEYCONFIG_VSH || sceKernelInitKeyConfig() == PSP_INIT_KEYCONFIG_POPS)) {
		//override device name
		strncpy(path, "ef0", 3);
	}

	//open config file
	SceUID fd = sceIoOpen(path, PSP_O_RDONLY, 0777);

	//opened config
	if (fd < 0)
		return;

	//file buffer
	char buffer[0x1800];

	//initialize buffer
	memset(buffer, 0, sizeof(buffer));

	//read file
	int read = sceIoRead(fd, buffer, sizeof(buffer));

	//close config file
	sceIoClose(fd);

	//read content
	if(read <= 0)
		return;

	//erase carriage returns
	char * terminator = NULL;
	while((terminator = strchr(buffer, '\r'))) {
		strcpy(terminator, terminator + 1);
	}

	//load plugins
	while((terminator = strchr(buffer, '\n'))) {
		//terminate line
		terminator[0] = 0;

		//skip null lines
		if(strlen(buffer) > 0 && buffer[strlen(buffer) - 1] == '1') {
			//drop load flag
			char * dot = strchr(buffer, '.');

			//valid prx filename
			if(dot) {
				//terminate string
				dot[4] = 0;

				//load module
				load_start_module(buffer);

				//restore string
				dot[4] = ' ';
			}
		}

		//move buffer
		strcpy(buffer, terminator + 1);
	}

	//skip null line
	if(strlen(buffer) > 0 && buffer[strlen(buffer) - 1] == '1') {
		//drop load flag
		char * dot = strchr(buffer, '.');

		//valid prx filename
		if(dot) {
			//terminate string
			dot[4] = 0;

			//load module
			load_start_module(buffer);

			//restore string
			dot[4] = ' ';
		}
	}
}

int plugin_thread(SceSize args, void * argp)
{
	//get mode key
	unsigned int key = sceKernelInitKeyConfig();

	//global config
	char * bootconf = NULL;

	//visual shell
	if(key == PSP_INIT_KEYCONFIG_VSH) {
		bootconf = "ms0:/plugins/vsh.txt";
	} //game mode
	else if(key == PSP_INIT_KEYCONFIG_GAME) {
		bootconf = "ms0:/plugins/game.txt";
	} //ps1 mode
	else if(key == PSP_INIT_KEYCONFIG_POPS) {
		bootconf = "ms0:/plugins/pops.txt";
	}

	//load global plugins
	load_plugins("ms0:/plugins/global.txt");

	//load mode specific plugins
	load_plugins(bootconf);

	//kill loader thread
	sceKernelExitDeleteThread(0);

	//return success
	return 0;
}
