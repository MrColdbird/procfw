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

#include "kubridge.h"
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
static int registery_hack_menu(struct MenuEntry *entry);
static int cpu_speed_menu(struct MenuEntry *entry);
static int advanced_menu(struct MenuEntry *entry);

const char * g_messages[] = {
	"PRO Recovery Menu",
	"Main Menu",
	"Entering",
	"Exiting",
	"Back",
	"Default",
	"Enabled",
	"Disabled",
	"Configuration",
	"Fake Region",
	"ISO Mode",
	"Charge battery when USB cable is plugged in",
	"Hide MAC address",
	"Skip Sony Logo at Startup",
	"Skip Game Boot Screen",
	"Hide PIC0.PNG and PIC1.PNG in game menu",
	"Protect flash in USB device mount",
	"Use version.txt in ms0:/seplugins",
	"Use Custom Update Server",
	"Prevent Hibernation Deletion (PSP-Go only)",
	"Advanced",
	"XMB Plugin",
	"Game Plugin",
	"Pops Plugin",
	"NoDRM Engine",
	"Block Analog Input in Game",
	"Old Plugin Support (PSP-Go only)",
	"Game Category",
	"CPU Speed",
	"XMB CPU/BUS",
	"Game CPU/BUS",
	"Plugins",
	"System storage",
	"Memory stick",
	"Plugins on system storage",
	"Plugins on memory stick",
	"Registry hacks",
	"WMA activated",
	"Flash activated",
	"Buttons swapped",
	"Activate WMA",
	"Activate Flash",
	"Swap O/X buttons (needs Reset VSH to take effect)",
	"Run ms0:/PSP/GAME/RECOVERY/EBOOT.PBP",
	"Shutdown device",
	"Suspend device",
	"Reset device",
	"Reset VSH",
};

static u8 message_test[NELEMS(g_messages) == RESET_VSH + 1 ? 0 : -1];

static int display_fake_region(struct MenuEntry* entry, char *buf, int size)
{
	sprintf(buf, "%-48s %-11s", g_messages[FAKE_REGION], get_fake_region_name(g_config.fakeregion));

	return 0;
}

static struct ValueOption g_iso_mode_option = {
	&g_config.umdmode,
	MODE_NP9660+1,
};

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
		(entry->display_callback)(entry, p, sizeof(buf) - (p - buf));
	} else {
		strcpy(p, *entry->info);
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

static struct ValueOption g_skip_logo_option = {
	&g_config.skiplogo,
	2,
};

static struct ValueOption g_skip_gameboot_option = {
	&g_config.skipgameboot,
	2,
};

static struct ValueOption g_hide_pic_option = {
	&g_config.hidepic,
	2,
};

static struct ValueOption g_use_version_option = {
	&g_config.useversion,
	2,
};

static struct ValueOption g_use_ownupdate_option = {
	&g_config.useownupdate,
	2,
};

static struct ValueOption g_flash_protect_option = {
	&g_config.flashprot,
	2,
};

static struct ValueOption g_hibblock_option = {
	&g_config.hibblock,
	2,
};

static int display_iso_mode(struct MenuEntry* entry, char *buf, int size)
{
	sprintf(buf, "%-48s %-11s", g_messages[ISO_MODE], get_iso_name(g_config.umdmode));

	return 0;
}

static int display_usb_charge(struct MenuEntry* entry, char *buf, int size)
{
	sprintf(buf, "%-48s %-11s", g_messages[USB_CHARGE], get_bool_name(g_config.usbcharge));

	return 0;
}

static int display_hidden_mac(struct MenuEntry* entry, char *buf, int size)
{
	sprintf(buf, "%-48s %-11s", g_messages[HIDE_MAC], get_bool_name(g_config.machidden));

	return 0;
}

static int display_skip_logo(struct MenuEntry* entry, char *buf, int size)
{
	sprintf(buf, "%-48s %-11s", g_messages[SKIP_SONY_LOGO], get_bool_name(g_config.skiplogo));

	return 0;
}

static int display_skip_gameboot(struct MenuEntry* entry, char *buf, int size)
{
	sprintf(buf, "%-48s %-11s", g_messages[SKIP_GAME_BOOT], get_bool_name(g_config.skipgameboot));

	return 0;
}

static int display_hide_pic(struct MenuEntry* entry, char *buf, int size)
{
	sprintf(buf, "%-48s %-11s", g_messages[HIDE_PIC], get_bool_name(g_config.hidepic));

	return 0;
}

static int display_flash_protect(struct MenuEntry* entry, char *buf, int size)
{
	sprintf(buf, "%-48s %-11s", g_messages[FLASH_PROTECT], get_bool_name(g_config.flashprot));

	return 0;
}

static int display_use_version(struct MenuEntry* entry, char *buf, int size)
{
	sprintf(buf, "%-48s %-11s", g_messages[USE_VERSION_TXT], get_bool_name(g_config.useversion));

	return 0;
}

static int display_use_ownupdate(struct MenuEntry* entry, char *buf, int size)
{
	sprintf(buf, "%-48s %-11s", g_messages[USE_CUSTOM_UPDATE_SERVER], get_bool_name(g_config.useownupdate));

	return 0;
}

static int display_hibernation_deletion(struct MenuEntry* entry, char *buf, int size)
{
	sprintf(buf, "%-48s %-11s", g_messages[PREVENT_HIB_DEL], get_bool_name(g_config.hibblock));

	return 0;
}

static struct MenuEntry g_configuration_menu_entries[] = {
	{ NULL, 0, 0, &display_iso_mode, &change_option, &change_option_by_enter, &g_iso_mode_option },
	{ NULL, 0, 0, &display_fake_region, &change_option, &change_option_by_enter, &g_fake_region_option },
	{ NULL, 0, 0, &display_hidden_mac, &change_option, &change_option_by_enter, &g_mac_hidden_option },
	{ NULL, 0, 0, &display_skip_gameboot, &change_option, &change_option_by_enter, &g_skip_gameboot_option },
	{ NULL, 0, 0, &display_skip_logo, &change_option, &change_option_by_enter, &g_skip_logo_option },
	{ NULL, 0, 0, &display_use_ownupdate, &change_option, &change_option_by_enter, &g_use_ownupdate_option},
	{ NULL, 0, 0, &display_flash_protect, &change_option, &change_option_by_enter, &g_flash_protect_option },
	{ NULL, 0, 0, &display_use_version, &change_option, &change_option_by_enter, &g_use_version_option},
	{ NULL, 0, 0, &display_hide_pic, &change_option, &change_option_by_enter, &g_hide_pic_option },
	{ NULL, 0, 0, &display_hibernation_deletion, &change_option, &change_option_by_enter, &g_hibblock_option},
	{ NULL, 0, 0, &display_usb_charge, &change_option, &change_option_by_enter, &g_usb_charge_option },
};

static struct Menu g_configuration_menu = {
	&g_messages[CONFIGURATION],
	g_configuration_menu_entries,
	NELEMS(g_configuration_menu_entries),
	0,
	0xFF,
};

static int display_xmb_plugin(struct MenuEntry* entry, char *buf, int size)
{
	sprintf(buf, "%-48s %-11s", g_messages[XMB_PLUGIN], get_bool_name(g_config.plugvsh));

	return 0;
}

static int display_game_plugin(struct MenuEntry* entry, char *buf, int size)
{
	sprintf(buf, "%-48s %-11s", g_messages[GAME_PLUGIN], get_bool_name(g_config.pluggame));

	return 0;
}

static int display_pops_plugin(struct MenuEntry* entry, char *buf, int size)
{
	sprintf(buf, "%-48s %-11s", g_messages[POPS_PLUGIN], get_bool_name(g_config.plugpop));

	return 0;
}

static int display_use_nodrm(struct MenuEntry* entry, char *buf, int size)
{
	sprintf(buf, "%-48s %-11s", g_messages[USE_NODRM_ENGINE], get_bool_name(g_config.usenodrm));

	return 0;
}

static int display_use_noanalog(struct MenuEntry* entry, char *buf, int size)
{
	sprintf(buf, "%-48s %-11s", g_messages[BLOCK_ANALOG_INPUT], get_bool_name(g_config.noanalog));

	return 0;
}

static int display_use_oldplugin(struct MenuEntry* entry, char *buf, int size)
{
	sprintf(buf, "%-48s %-11s", g_messages[OLD_PLUGINS_SUPPORT], get_bool_name(g_config.oldplugin));

	return 0;
}

static int display_use_category(struct MenuEntry* entry, char *buf, int size)
{
	sprintf(buf, "%-48s %-11s", g_messages[CATEGORY], get_bool_name(g_config.category));

	return 0;
}

static struct ValueOption g_xmb_plugin_option = {
	&g_config.plugvsh,
	2,
};

static struct ValueOption g_game_plugin_option = {
	&g_config.pluggame,
	2,
};

static struct ValueOption g_pops_plugin_option = {
	&g_config.plugpop,
	2,
};

static struct ValueOption g_use_nodrm_option = {
	&g_config.usenodrm,
	2,
};

static struct ValueOption g_use_noanalog_option = {
	&g_config.noanalog,
	2,
};

static struct ValueOption g_use_oldplugin = {
	&g_config.oldplugin,
	2,
};

static struct ValueOption g_use_category = {
	&g_config.category,
	2,
};

static struct MenuEntry g_advanced_menu_entries[] = {
	{ NULL, 0, 0, &display_xmb_plugin, &change_option, &change_option_by_enter, &g_xmb_plugin_option },
	{ NULL, 0, 0, &display_game_plugin, &change_option, &change_option_by_enter, &g_game_plugin_option },
	{ NULL, 0, 0, &display_pops_plugin, &change_option, &change_option_by_enter, &g_pops_plugin_option },
	{ NULL, 0, 0, &display_use_nodrm, &change_option, &change_option_by_enter, &g_use_nodrm_option},
	{ NULL, 0, 0, &display_use_noanalog, &change_option, &change_option_by_enter, &g_use_noanalog_option},
	{ NULL, 0, 0, &display_use_oldplugin, &change_option, &change_option_by_enter, &g_use_oldplugin},
	{ NULL, 0, 0, &display_use_category, &change_option, &change_option_by_enter, &g_use_category},
};

static struct Menu g_advanced_menu = {
	&g_messages[ADVANCED],
	g_advanced_menu_entries,
	NELEMS(g_advanced_menu_entries),
	0,
	0xFF,
};

static int advanced_menu(struct MenuEntry *entry)
{
	struct Menu *menu = &g_advanced_menu;

	menu->cur_sel = 0;
	menu_loop(menu);
	sctrlSESetConfig(&g_config);

	return 0;
}

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

static int run_recovery_eboot(struct MenuEntry *entry)
{
	struct SceKernelLoadExecVSHParam param;
	int ret;
	SceIoStat stat;
	u32 psp_model;

	memset(&param, 0, sizeof(param));
	param.size = sizeof(param);
	param.key = "game";
	psp_model = kuKernelGetModel();

	if(psp_model == PSP_GO && sceIoGetstat(RECOVERY_EBOOT_PATH_EF0, &stat) == 0) {
		param.args = strlen(RECOVERY_EBOOT_PATH_EF0)+1;
		param.argp = RECOVERY_EBOOT_PATH_EF0;
		ret = sctrlKernelLoadExecVSHWithApitype(0x152, RECOVERY_EBOOT_PATH_EF0, &param);
	}

	param.args = strlen(RECOVERY_EBOOT_PATH)+1;
	param.argp = RECOVERY_EBOOT_PATH;
	ret = sctrlKernelLoadExecVSHWithApitype(0x141, RECOVERY_EBOOT_PATH, &param);

	return ret;
}

static struct MenuEntry g_top_menu_entries[] = {
	{ &g_messages[RUN_RECOVERY_EBOOT], 0, 0, NULL, NULL, &run_recovery_eboot, NULL },
	{ &g_messages[CONFIGURATION], 1, 0, NULL, NULL, &configuration_menu, NULL},
	{ &g_messages[ADVANCED], 1, 0, NULL, NULL, &advanced_menu, NULL},
	{ &g_messages[CPU_SPEED], 1, 0, NULL, NULL, &cpu_speed_menu, NULL },
	{ &g_messages[PLUGINS], 1, 0, NULL, NULL, &plugins_menu, NULL },
	{ &g_messages[REGISTERY_HACKS], 1, 0, NULL, NULL, &registery_hack_menu, NULL },
	{ &g_messages[SHUTDOWN_DEVICE], 0, 0, NULL, NULL, &shutdown_device, NULL },
	{ &g_messages[SUSPEND_DEVICE], 0, 0, NULL, NULL, &suspend_device, NULL },
	{ &g_messages[RESET_DEVICE], 0, 0, NULL, NULL, &reset_device, NULL },
	{ &g_messages[RESET_VSH], 0, 0, NULL, NULL, &reset_vsh, NULL },
};

static struct Menu g_top_menu = {
	&g_messages[MAIN_MENU],
	g_top_menu_entries,
	NELEMS(g_top_menu_entries),
	0,
	0xFF,
};

static int configuration_menu(struct MenuEntry *entry)
{
	struct Menu *menu = &g_configuration_menu;

	menu->cur_sel = 0;
	menu_loop(menu);
	sctrlSESetConfig(&g_config);

	return 0;
}

static int g_xmb_clock_number, g_game_clock_number;

static int display_xmb(struct MenuEntry* entry, char *buf, int size)
{
	int cpu, bus;

	cpu = get_cpu_freq(g_xmb_clock_number);
	bus = get_bus_freq(g_xmb_clock_number);

	sprintf(buf, "%s:", g_messages[XMB_CPU_BUS]);

	if(cpu == 0 || bus == 0) {
		sprintf(buf, "%-40s %s/%s", buf, g_messages[DEFAULT], g_messages[DEFAULT]);
	} else {
		sprintf(buf, "%-40s %d/%d", buf, cpu, bus);
	}

	return 0;
}

static int display_game(struct MenuEntry* entry, char *buf, int size)
{
	int cpu, bus;

	cpu = get_cpu_freq(g_game_clock_number);
	bus = get_bus_freq(g_game_clock_number);

	sprintf(buf, "%s:", g_messages[GAME_CPU_BUS]);

	if(cpu == 0 || bus == 0) {
		sprintf(buf, "%-40s %s/%s", buf, g_messages[DEFAULT], g_messages[DEFAULT]);
	} else {
		sprintf(buf, "%-40s %d/%d", buf, cpu, bus);
	}

	return 0;
}

struct ValueOption g_xmb_clock_option = {
	&g_xmb_clock_number,
	9,
};

struct ValueOption g_game_clock_option = {
	&g_game_clock_number,
	9
};

static void update_clock_config(void)
{
	g_config.vshcpuspeed = get_cpu_freq(g_xmb_clock_number);
	g_config.vshbusspeed = get_bus_freq(g_xmb_clock_number);
	g_config.umdisocpuspeed = get_cpu_freq(g_game_clock_number);
	g_config.umdisobusspeed = get_bus_freq(g_game_clock_number);

	if(g_config.vshcpuspeed == 0) {
		sctrlHENSetSpeed(222, 111);
	} else {
		sctrlHENSetSpeed(g_config.vshcpuspeed, g_config.vshbusspeed);
	}
}

static int change_clock_option(struct MenuEntry *entry, int direct)
{
	change_option(entry, direct);
	update_clock_config();

	return 0;
}

static int change_clock_option_by_enter(struct MenuEntry *entry)
{
	change_option_by_enter(entry);
	update_clock_config();

	return 0;
}

static struct MenuEntry g_cpu_speed_menu_entries[] = {
	{ NULL, 0, 0, &display_xmb, &change_clock_option, &change_clock_option_by_enter, &g_xmb_clock_option},
	{ NULL, 0, 0, &display_game, &change_clock_option, &change_clock_option_by_enter, &g_game_clock_option},
};

static struct Menu g_cpu_speed_menu = {
	&g_messages[CPU_SPEED],
	g_cpu_speed_menu_entries,
	NELEMS(g_cpu_speed_menu_entries),
	0,
	0xFF,
};

static int cpu_speed_menu(struct MenuEntry *entry)
{
	struct Menu *menu = &g_cpu_speed_menu;

	g_xmb_clock_number = get_cpu_number(g_config.vshcpuspeed);
	g_game_clock_number = get_cpu_number(g_config.umdisocpuspeed);
	menu->cur_sel = 0;
	menu_loop(menu);

	sctrlSESetConfig(&g_config);

	return 0;
}

static int active_wma(struct MenuEntry *entry)
{
	char buf[80];

	set_registry_value("/CONFIG/MUSIC", "wma_play", 1);
	sprintf(buf, "> %s", g_messages[WMA_ACTIVATED]);
	set_bottom_info(buf, 0);
	frame_end();
	sceKernelDelayThread(CHANGE_DELAY);
	set_bottom_info("", 0);

	return 0;
}

static int active_flash(struct MenuEntry *entry)
{
	char buf[80];

	sprintf(buf, "> %s", g_messages[FLASH_ACTIVATED]);
	set_registry_value("/CONFIG/BROWSER", "flash_activated", 1);
	set_registry_value("/CONFIG/BROWSER", "flash_play", 1);
	set_bottom_info(buf, 0);
	frame_end();
	sceKernelDelayThread(CHANGE_DELAY);
	set_bottom_info("", 0);
	
	return 0;
}

static int swap_buttons(struct MenuEntry *entry)
{
	u32 value;
	char buf[80];

	sprintf(buf, "> %s", g_messages[SWAP_BUTTONS]);
	get_registry_value("/CONFIG/SYSTEM/XMB", "button_assign", &value);
	value = !value;
	set_registry_value("/CONFIG/SYSTEM/XMB", "button_assign", value); 
	set_bottom_info(buf, 0);
	frame_end();
	sceKernelDelayThread(CHANGE_DELAY);
	set_bottom_info("", 0);
	
	return 0;
}

static struct MenuEntry g_registery_menu_entries[] = {
	{ &g_messages[ACTIVATE_WMA], 0, 0, NULL, NULL, &active_wma, NULL },
	{ &g_messages[ACTIVATE_FLASH], 0, 0, NULL, NULL, &active_flash, NULL },
	{ &g_messages[SWAP_BUTTONS], 0, 0, NULL, NULL, &swap_buttons, NULL },
};

static struct Menu g_registery_hack_menu = {
	&g_messages[REGISTERY_HACKS],
	g_registery_menu_entries,
	NELEMS(g_registery_menu_entries),
	0,
	0xFF,
};

static int registery_hack_menu(struct MenuEntry *entry)
{
	struct Menu *menu = &g_registery_hack_menu;

	menu->cur_sel = 0;
	menu_loop(menu);

	return 0;
}

void main_menu(void)
{
	struct Menu *menu = &g_top_menu;
	(void)message_test;
	
	menu->cur_sel = 0;
	menu_loop(menu);
}
