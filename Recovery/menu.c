#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <pspsdk.h>
#include <pspdebug.h>
#include <pspkernel.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <psputility.h>

#include "systemctrl.h"
#include "systemctrl_se.h"
#include "vshctrl.h"
#include "utils.h"
#include "vpl.h"
#include "main.h"

extern int scePowerRequestColdReset(int unk);
extern int scePowerRequestStandby(void);
extern int scePowerRequestSuspend(void);

static int configuration_menu(struct MenuEntry *entry);

static int test_option1, test_option2;

static struct ValueOption g_test_option1 = {
	&test_option1,
	2,
};

static struct ValueOption g_test_option2 = { 
	&test_option2,
	4,
};

static int display_test_option1(char *buf, int size)
{
	return sprintf(buf, "3. bool option = %d", test_option1);
}

static int display_test_option2(char *buf, int size)
{
	return sprintf(buf, "4. value option = %d", test_option2);
}

static int display_fake_region(char *buf, int size)
{
	sprintf(buf, "Fake region (%s)", get_fake_region_name(g_config.fakeregion));

	return 0;
}

static struct ValueOption g_fake_region_option = {
	&g_config.fakeregion,
	FAKE_REGION_CHINA+1,
};

static int change_option(struct MenuEntry *entry, int direct)
{
	struct ValueOption *c = (struct ValueOption*)entry->arg;

	*c->value = limit_int(*c->value, direct, c->limit);

	return 0;
}

static int change_option_by_enter(struct MenuEntry *entry)
{
	char buf[256], *p;
	
	change_option(entry, 1);
	strcpy(buf, "> ");
	p = buf + strlen(buf);

	if(entry->display_callback != NULL) {
		(entry->display_callback)(p, sizeof(buf) - (p - buf));
	} else {
		strcpy(p, entry->info);
	}

	set_bottom_info(buf, 0);
	frame_end();
	sceKernelDelayThread(CHANGE_DELAY);
	set_bottom_info("", 0);
	
	return 0;
}

static struct ValueOption g_mac_hidden_option = {
	&g_config.machidden,
	2,
};

static struct ValueOption g_usb_charge_option = {
	&g_config.usbcharge,
	2,
};

static int display_usb_charge(char *buf, int size)
{
	sprintf(buf, "Charge battery when USB cable plugged(%s)", get_bool_name(g_config.usbcharge));

	return 0;
}

static int display_hidden_mac(char *buf, int size)
{
	sprintf(buf, "Hide MAC address (%s)", get_bool_name(g_config.machidden));

	return 0;
}

static struct MenuEntry g_configuration_menu_entries[] = {
	{ NULL, 0, 0, &display_fake_region, &change_option, &change_option_by_enter, &g_fake_region_option },
	{ NULL, 0, 0, &display_usb_charge, &change_option, &change_option_by_enter, &g_usb_charge_option},
	{ NULL, 0, 0, &display_hidden_mac, &change_option, &change_option_by_enter, &g_mac_hidden_option},
};

static struct Menu g_configuration_menu = {
	"Configuration",
	g_configuration_menu_entries,
	NELEMS(g_configuration_menu_entries),
	0,
	0xFF,
};

static int shutdown_device(struct MenuEntry *entry)
{
	resume_vsh_thread();
	scePowerRequestStandby();
	sceKernelStopUnloadSelfModule(0, NULL, NULL, NULL);

	return 0;
}

static int suspend_device(struct MenuEntry *entry)
{
	resume_vsh_thread();
	scePowerRequestSuspend();
	sceKernelStopUnloadSelfModule(0, NULL, NULL, NULL);

	return 0;
}

static int reset_device(struct MenuEntry *entry)
{
	resume_vsh_thread();
	scePowerRequestColdReset(0);
	sceKernelStopUnloadSelfModule(0, NULL, NULL, NULL);

	return 0;
}

static int reset_vsh(struct MenuEntry *entry)
{
	resume_vsh_thread();
	sctrlKernelExitVSH(NULL);
	sceKernelStopUnloadSelfModule(0, NULL, NULL, NULL);

	return 0;
}

static struct MenuEntry g_top_menu_entries[] = {
	{ "Configuration", 1, 0, NULL, NULL, &configuration_menu, NULL},
	{ "CPU Speed", 0, 0, NULL, NULL, NULL, NULL },
	{ "Plugins", 0, 0, NULL, NULL, NULL, NULL },
	{ "Registery hacks", 0, 0, NULL, NULL, NULL, NULL },
	{ "Shutdown device", 0, 0, NULL, NULL, &shutdown_device, NULL },
	{ "Suspend device", 0, 0, NULL, NULL, &suspend_device, NULL },
	{ "Reset device", 0, 0, NULL, NULL, &reset_device, NULL },
	{ "Reset VSH", 0, 0, NULL, NULL, &reset_vsh, NULL },
	{ NULL, 0, 0, &display_test_option1, &change_option, &change_option_by_enter, &g_test_option1 },
	{ NULL, 0, 0, &display_test_option2, &change_option, &change_option_by_enter, &g_test_option2 },
};

static struct Menu g_top_menu = {
	"Main Menu",
	g_top_menu_entries,
	NELEMS(g_top_menu_entries),
	0,
	0xFF,
};

static int configuration_menu(struct MenuEntry *entry)
{
	struct Menu *menu = &g_configuration_menu;

	sctrlSEGetConfig(&g_config);
	menu->cur_sel = 0;
	menu_loop(menu);
	sctrlSESetConfig(&g_config);

	return 0;
}

void main_menu(void)
{
	struct Menu *menu = &g_top_menu;
	
	menu->cur_sel = 0;
	menu_loop(menu);
}
