
#include <pspkernel.h>
#include <pspctrl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "../cfw_modules.h"

PSP_MODULE_INFO("u235_module", 0x800 , 1, 0);
PSP_MAIN_THREAD_ATTR( PSP_THREAD_ATTR_VSH );

int PlutoniumGetModel();
int PlutoniumColdReset();

void reset()
{
	sceKernelDelayThread(800*1000);
	PlutoniumColdReset();
	sceKernelSleepThread();
	while(1){}
}

int WriteFile(char *filename, char *buffer, u32 size)
{
	SceUID fd = sceIoOpen(filename, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
	if (fd < 0) return -1;

	int written = sceIoWrite(fd, buffer, size);
	if (sceIoClose(fd) < 0) return -1;

    return written;
}

int main()
{
//	int model = PlutoniumGetModel();
	int i;
/*
	Module *modules = NULL;

	if(model == 0 )
		modules = (Module *)modules_01g;
	else
		modules = (Module *)modules_02g;
*/

	for(i = 0; i < COMMON_MODULES_COUNT; i++) {
		WriteFile(common_modules[i].dst, common_modules[i].buffer, *common_modules[i].size);
	}

	reset();
	return 0;
}
