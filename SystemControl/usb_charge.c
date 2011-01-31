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

static int (*sceUsb_driver_AE5DE6AF)(char *, int, int) = NULL;
static int (*sceUsb_driver_C21645A4)(void) = NULL;
static int (*scePower_driver_E11A1999)(int) = NULL;
static int (*scePower_driver_F3A08560)(int) = NULL;

static int usb_charge_thread(SceSize argc, void *argp)
{
	int ret;
	int is_charging = 0;

	printk("Starting %s\n", __func__);

	while (1) {
		sceUsb_driver_AE5DE6AF("USBBusDriver", 0, 0);
		ret = sceUsb_driver_C21645A4() & 0x20;

		if (is_charging == 1) {
			if (ret == 0) {
				printk("%s: stops charging\n", __func__);
				scePower_driver_F3A08560(0);
				is_charging = 0;
			}
		} else if (ret != 0) {
			printk("%s: starts charging\n", __func__);
			scePower_driver_E11A1999(1);
			is_charging = 1;
		}

		sceKernelDelayThread(50000);
	}

	return 0;
}

void usb_charge(void)
{
	int thid;

	if (!conf.usbcharge) {
		return;
	}

	sceUsb_driver_AE5DE6AF = (void*)sctrlHENFindFunction("sceUSB_Driver", "sceUsb_driver", 0xAE5DE6AF);
	sceUsb_driver_C21645A4 = (void*)sctrlHENFindFunction("sceUSB_Driver", "sceUsb_driver", 0xC21645A4);
	scePower_driver_E11A1999 = (void*)sctrlHENFindFunction("scePower_Service", "scePower_driver", 0xE11A1999);
	scePower_driver_F3A08560 = (void*)sctrlHENFindFunction("scePower_Service", "scePower_driver", 0xF3A08560);

	if (!sceUsb_driver_AE5DE6AF || !sceUsb_driver_C21645A4 || !scePower_driver_E11A1999 || !scePower_driver_F3A08560) {
		return;
	}

	thid = sceKernelCreateThread("UsbCharger", &usb_charge_thread, 32, 0x800, 0, 0);

	if (thid >= 0) {
		sceKernelStartThread(thid, 0, NULL);
	}
}
