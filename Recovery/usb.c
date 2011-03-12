#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <pspsdk.h>
#include <pspdebug.h>
#include <pspkernel.h>
#include <pspusb.h>
#include <pspusbstor.h>

#include "kubridge.h"
#include "systemctrl.h"
#include "systemctrl_se.h"
#include "vshctrl.h"
#include "utils.h"
#include "vpl.h"
#include "main.h"
#include "pspusbdevice.h"

static int g_usb_module_loaded = 0;
static int g_usb_actived = 0;

struct UsbModule {
	char *path;
	int modid;
};

static struct UsbModule g_usb_modules[] = {
	{ PATH_USBDEVICE, -1, },
	{ "flash0:/kd/semawm.prx", -1, },
	{ "flash0:/kd/usbstor.prx", -1, },
	{ "flash0:/kd/usbstormgr.prx", -1, },
	{ "flash0:/kd/usbstorms.prx", -1, },
	{ "flash0:/kd/usbstoreflash.prx", -1, },
	{ "flash0:/kd/usbstorboot.prx", -1, },
};

static int load_start_module(char *path)
{
	int ret;
	SceUID modid;

	modid = sceKernelLoadModule(path, 0, NULL);
	ret = sceKernelStartModule(modid, strlen(path) + 1, path, 0, NULL);

	return ret;
}

static void enable_usb(void)
{
	int i;

	if(!g_usb_module_loaded) {
		for(i=0; i<NELEMS(g_usb_modules); ++i) {
			g_usb_modules[i].modid = load_start_module(g_usb_modules[i].path);
		}

		g_usb_module_loaded = 1;
	}

	if(g_config.usbdevice != 0) {
		pspUsbDeviceSetDevice(g_config.usbdevice-1, g_config.flashprot, 0);
	}

	sceUsbStart(PSP_USBBUS_DRIVERNAME, 0, 0);
	sceUsbStart(PSP_USBSTOR_DRIVERNAME, 0, 0);
	sceUsbstorBootSetCapacity(0x800000);
	sceUsbActivate(0x1c8);
	g_usb_actived = 1;
}

static void disable_usb(void)
{
	if(!g_usb_actived) {
		return;
	}

	sceUsbDeactivate(0x1c8);
	sceUsbStop(PSP_USBSTOR_DRIVERNAME, 0, 0);
	sceUsbStop(PSP_USBBUS_DRIVERNAME, 0, 0);
	pspUsbDeviceFinishDevice();
	g_usb_actived = 0;
}

static void stop_unload_mod(SceUID modid)
{
	sceKernelStopModule(modid, 0, NULL, NULL, NULL);
	sceKernelUnloadModule(modid);
}

void exit_usb(void)
{
	int i;

	disable_usb();

	if(g_usb_module_loaded) {
		for(i=NELEMS(g_usb_modules)-1; i>=0; --i) {
			stop_unload_mod(g_usb_modules[i].modid);
			g_usb_modules[i].modid = -1;
		}

		g_usb_module_loaded = 0;
	}
}

int toggle_usb(struct MenuEntry *entry)
{
	char buf[256];

	if(g_usb_actived) {
		disable_usb();
		sprintf(buf, "> %s %s", g_messages[TOGGLE_USB], g_messages[USB_DISABLED]);
	} else {
		enable_usb();
		sprintf(buf, "> %s %s", g_messages[TOGGLE_USB], g_messages[USB_ENABLED]);
	}

	set_bottom_info(buf, 0);
	frame_end();
	sceKernelDelayThread(CHANGE_DELAY);
	set_bottom_info("", 0);

	return 0;
}
