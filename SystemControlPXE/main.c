#include <pspsdk.h>
#include <pspsysmem_kernel.h>
#include <pspkernel.h>
#include <psputilsforkernel.h>
#include <pspsysevent.h>
#include <pspiofilemgr.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "systemctrl.h"
#include "modhandler.h"
#include "printk.h"
#include "syspatch.h"
#include "funcs_630.h"
#include "libs.h"

PSP_MODULE_INFO("SystemControl", 0x3007, 2, 5);

//taken from live systemcontrol codebase
void patch_sceMemlmd(void);

//previous start handler
STMOD_HANDLER previous = NULL;

//psp model number
u32 psp_model = 0;

//installer path buffer
extern char installerpath[256];

//entry point of pxe sysctrl
int module_start(SceSize args, void* argp)
{
	//save psp model for patching
	psp_model = *(u32*)(0x88FB0000);

	//link to log file
	printk_init("ms0:/pxesysctrl.txt");

	//grab installer executable path
	memcpy(installerpath, (char*)0x88FB0100, sizeof(installerpath));

	//disable executable checks
	patch_sceLoadCore();
	patch_sceModuleManager();
	patch_sceMemlmd();

	//apply changes
	sync_cache();

	//register start handler
	previous = sctrlHENSetStartModuleHandler(on_module_start);
	
	return 0;
}

