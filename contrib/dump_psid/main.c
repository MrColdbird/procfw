#include <pspkernel.h>
#include <psputils.h>
#include <psputilsforkernel.h>
#include <psploadexec_kernel.h>
#include <pspsysevent.h>
#include <pspiofilemgr.h>
#include <stdio.h>
#include <string.h>
#include "systemctrl.h"
#include "printk.h"

PSP_MODULE_INFO("dump_psid", 0x1000, 1, 0);

u32 (*_sceOpenPSIDGetOpenPSID)(u8 psid[0x10]);
u8 shabuf[20] __attribute__((aligned(64)));

int sha1(u8 *buf, u32 size)
{
	int ret;

	ret = sceKernelUtilsSha1Digest(buf, size, shabuf);

	return ret;
}

int main_thread(SceSize args, void *argp)
{
	u8 psid[16];

	_sceOpenPSIDGetOpenPSID = (void*)sctrlHENFindFunction("sceOpenPSID_Service", "sceOpenPSID_driver", 0xC69BEBCE);

	if (_sceOpenPSIDGetOpenPSID != NULL) {
		memset(psid, 0, sizeof(psid));
		(*_sceOpenPSIDGetOpenPSID)(psid);
		printk("Dumping PSID:\r\n");
		printk( "%02X%02X%02X%02X"
				"%02X%02X%02X%02X"
				"%02X%02X%02X%02X"
				"%02X%02X%02X%02X\r\n", psid[0], psid[1], psid[2], psid[3], psid[4], psid[5], psid[6], psid[7], psid[8], psid[9], psid[10], psid[11], psid[12], psid[13], psid[14], psid[15]);
	} else {
		printk("_sceOpenPSIDGetOpenPSID failed\r\n");
	}

	sceKernelExitVSHKernel(NULL);
	return 0;
}

int module_start(SceSize args, void* argp)
{
	int thid;

	printk_init("ms0:/PSID.txt");
	thid = sceKernelCreateThread("main_thread", main_thread, 0x1A, 0x8000, 0, NULL);

	if(thid>=0) {
		sceKernelStartThread(thid, args, argp);
	}

	return 0;
}

int module_stop(SceSize args, void *argp)
{
	return 0;
}
