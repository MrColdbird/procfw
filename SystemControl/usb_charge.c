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

static inline void *get_usb_driver_function(u32 nid)
{
	return (void*)sctrlHENFindFunction("sceUSB_Driver", "sceUsb_driver", nid);
}

static inline void *get_power_driver_function(u32 nid)
{
	return (void*)sctrlHENFindFunction("scePower_Service", "scePower_driver", nid);
}

static u32 usb_charge_timer_handler(SceUID uid, SceInt64 unk0, SceInt64 unk1, void *common)
{
	int (*_scePowerBatteryEnableUsbCharging)(int) = NULL;
	int (*_scePowerBatteryDisableUsbCharging)(int) = NULL;
	int (*_sceUsbStart)(char *, int, int) = NULL;
	int (*_sceUsbGetState)(void) = NULL;
	int ret;
	static int is_charging = 0;

	_sceUsbStart = get_usb_driver_function(0xAE5DE6AF);
	_sceUsbGetState = get_usb_driver_function(0xC21645A4);
	_scePowerBatteryDisableUsbCharging = get_power_driver_function(0x90285886);
	_scePowerBatteryEnableUsbCharging = get_power_driver_function(0x733F973B);

	if(_sceUsbStart == NULL ||
			_sceUsbGetState == NULL ||
			_scePowerBatteryDisableUsbCharging == NULL ||
			_scePowerBatteryEnableUsbCharging == NULL) {
		return 2000000;
	}
	
	(*_sceUsbStart)("USBBusDriver", 0, 0);
	ret = (*_sceUsbGetState)() & 0x20;

	if (is_charging == 1 && ret == 0) {
		printk("%s: stops charging\n", __func__);
		(*_scePowerBatteryDisableUsbCharging)(0);
		is_charging = 0;
	} else if (is_charging == 0 && ret != 0) {
		printk("%s: starts charging\n", __func__);
		(*_scePowerBatteryEnableUsbCharging)(1);
		is_charging = 1;
	}

	return 5000000;
}

void usb_charge(void)
{
	SceUID vtimer;

	if (!conf.usbcharge || psp_model == PSP_1000 ) {
		return;
	}

	vtimer = sceKernelCreateVTimer("", NULL);

	if(vtimer < 0) {
		printk("%s: sceKernelCreateVTimer -> 0x%08X\n", __func__, vtimer);

		return;
	}

	sceKernelStartVTimer(vtimer);
	sceKernelSetVTimerHandlerWide(vtimer, 5000000, usb_charge_timer_handler, NULL);
}
