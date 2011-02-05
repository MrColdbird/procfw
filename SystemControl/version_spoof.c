#include <pspsdk.h>
#include <pspsysmem_kernel.h>
#include <pspkernel.h>
#include <psputilsforkernel.h>
#include <pspsysevent.h>
#include <pspiofilemgr.h>
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "printk.h"
#include "libs.h"
#include "utils.h"

int sceResmgr_driver_9DC14891(u8 *buf, u32 size, u32* newsize);

int versionspoofer(u8 *buf, u32 size, u32* newsize)
{
	//grab file contents
	int result = sceResmgr_driver_9DC14891(buf, size, newsize);

	//version.txt spotted!
	if(conf.useversion && strstr((const char*)buf, "release:")) {
		//increase permission level
		u32 k1 = pspSdkSetK1(0);

		//open version.txt
		SceUID fd = sceIoOpen("ms0:/seplugins/version.txt", PSP_O_RDONLY, 0777);
		if(fd < 0) fd = sceIoOpen("ef0:/seplugins/version.txt", PSP_O_RDONLY, 0777);

		if(fd >= 0) {
			//read contents
			*newsize = sceIoRead(fd, buf, size);

			//close file
			sceIoClose(fd);
		}

		//restore permission level
		pspSdkSetK1(k1);
	}

	//return result
	return result;
}

void patch_module_for_version_spoof(SceModule *mod1)
{
	u32 text_addr;
	SceModule2 *mod = (SceModule2*)mod1;
	
	if (mod == NULL) {
		return;
	}

	text_addr = mod->text_addr;

	if(text_addr & 0x80000000) {
		// kernel module
		hook_import_bynid((SceModule*)mod, "sceResmgr_driver", 0x9DC14891, (void*)versionspoofer, 0);
	} else {
		// user module (hooking requires 0x4001 export in exports.exp)
		hook_import_bynid((SceModule*)mod, "sceResmgr", 0x9DC14891, (void*)versionspoofer, 1);
	}
}
