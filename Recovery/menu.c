/*
 * This file is part of PRO CFW.

 * PRO CFW is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * PRO CFW is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PRO CFW. If not, see <http://www.gnu.org/licenses/ .
 */

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
#include "pspusbdevice.h"
#include "font_list.h"
#include "prodebug.h"

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
	"Toggle USB",
	"USB Enabled",
	"USB Disabled",
	"Configuration",
	"Fake Region",
	"Recovery Font",
	"ISO Mode",
	"XMB USB Device",
	"Flash 0",
	"Flash 1",
	"Flash 2",
	"Flash 3",
	"UMD Disc",
	"Charge battery when USB cable is plugged in",
	"Use Slim Color on PSP-1000",
	"Use htmlviewer custom save location",
	"Hide MAC address",
	"Skip Sony Logo at Startup",
	"Skip Game Boot Screen",
	"Hide PIC0.PNG and PIC1.PNG in game menu",
	"Protect flash in USB device mount",
	"Use version.txt in /seplugins",
	"Use usbversion.txt in /seplugins",
	"Use Custom Update Server",
	"Prevent Hibernation Deletion (PSP-Go only)",
	"Advanced",
	"XMB Plugin",
	"Game Plugin",
	"Pops Plugin",
	"NoDRM Engine",
	"Hide CFW Files from game",
	"Block Analog Input in Game",
	"Old Plugin Support (PSP-Go only)",
	"Inferno & NP9660 Use ISO Cache",
	"Inferno & NP9660 Cache Size(in MB)",
	"Inferno & NP9660 Cache Number",
	"Inferno & NP9660 Cache Policy",
	"Allow Non-latin1 ISO Filename",
	"Memory Stick Speedup",
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
	"Confirm Button: X",
	"Confirm Button: O",
	"Activate WMA",
	"Activate Flash",
	"Swap O/X buttons",
	"Swap O/X buttons (needs Reset VSH to take effect)",
	"Run /PSP/GAME/RECOVERY/EBOOT.PBP",
	"Shutdown device",
	"Suspend device",
	"Reset device",
	"Reset VSH",
	"Japan",
	"America",
	"Europe",
	"Korea",
	"United Kingdom",
	"Mexico",
	"Australia",
	"Hongkong",
	"Taiwan",
	"Russia",
	"China",
	"Debug Type I",
	"Debug Type II",
};

static u8 message_test[NELEMS(g_messages) == DEBUG_TYPE_II + 1 ? 0 : -1];

static int display_fake_region(struct MenuEntry* entry, char *buf, int size)
{
	sprintf(buf, "%-48s %-11s", g_messages[FAKE_REGION], get_fake_region_name(g_config.fakeregion));

	return 0;
}

static struct ValueOption g_iso_mode_option = {
	&g_config.umdmode,
	1, MODE_INFERNO+1,
};

static struct ValueOption g_xmb_usbdevice_option = {
	&g_config.usbdevice,
	0, PSP_USBDEVICE_UMD9660+1+1, /* plus 1 for no usbdevice */
};

static struct ValueOption g_fake_region_option = {
	&g_config.fakeregion,
	0, FAKE_REGION_DEBUG_TYPE_II+1,
};

static int change_option(struct MenuEntry *entry, int direct)
{
	struct ValueOption *c = (struct ValueOption*)entry->arg;

	*c->value -= c->limit_start;
	*c->value = limit_int(*c->value, direct, c->limit_end - c->limit_start);
	*c->value += c->limit_start;

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

static int change_iso_cache_number_option(struct MenuEntry *entry, int direct)
{
	struct ValueOption *c = (struct ValueOption*)entry->arg;
	s16 cache_sels[] = { 64, 128, 160, 196, 256, 320, 384, 512, 640, 768, };
	size_t i;

	for(i=0; i<NELEMS(cache_sels); ++i) {
		if(*c->value <= cache_sels[i]) {
			break;
		}
	}

	i = limit_int(i, direct, NELEMS(cache_sels));
	*c->value = cache_sels[i];

	return 0;
}

static int change_iso_cache_number_option_by_enter(struct MenuEntry *entry)
{
	char buf[256], *p;
	
	change_iso_cache_number_option(entry, 1);
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
	0, 2,
};

static struct ValueOption g_usb_charge_option = {
	&g_config.usbcharge,
	0, 2,
};

static s16 g_font_cur_sel = 0;

static struct ValueOption g_recovery_font_option = {
	&g_font_cur_sel,
	0, 0,
};

static struct ValueOption g_slim_color_option = {
	&g_config.slimcolor,
	0, 2,
};

static struct ValueOption g_htmlviewer_custom_save_location_option = {
	&g_config.htmlviewer_custom_save_location,
	0, 2,
};

static struct ValueOption g_skip_logo_option = {
	&g_config.skiplogo,
	0, 2,
};

static struct ValueOption g_skip_gameboot_option = {
	&g_config.skipgameboot,
	0, 2,
};

static struct ValueOption g_hide_pic_option = {
	&g_config.hidepic,
	0, 2,
};

static struct ValueOption g_use_version_option = {
	&g_config.useversion,
	0, 2,
};

static struct ValueOption g_use_usbversion_option = {
	&g_config.usbversion,
	0, 2,
};

static struct ValueOption g_use_ownupdate_option = {
	&g_config.useownupdate,
	0, 2,
};

static struct ValueOption g_flash_protect_option = {
	&g_config.flashprot,
	0, 2,
};

static struct ValueOption g_hibblock_option = {
	&g_config.hibblock,
	0, 2,
};

static int display_iso_mode(struct MenuEntry* entry, char *buf, int size)
{
	sprintf(buf, "%-48s %-11s", g_messages[ISO_MODE], get_iso_name(g_config.umdmode));

	return 0;
}

static int display_xmb_usbdevice(struct MenuEntry* entry, char *buf, int size)
{
	sprintf(buf, "%-48s %-11s", g_messages[XMB_USBDEVICE], get_usbdevice_name(g_config.usbdevice));

	return 0;
}

static int display_usb_charge(struct MenuEntry* entry, char *buf, int size)
{
	sprintf(buf, "%-48s %-11s", g_messages[USB_CHARGE], get_bool_name(g_config.usbcharge));

	return 0;
}

extern FontList g_font_list;

static const char *get_recovery_fontname(size_t idx)
{
	char *fontname;
	
	if(idx == 0) {
		fontname = "Default";
	} else {
		fontname = fontlist_get(&g_font_list, idx - 1);

		if(fontname == NULL) {
			fontname = "Default";
		}
	}

	return fontname;
}

static int display_recovery_font(struct MenuEntry* entry, char *buf, int size)
{
	const char *fontname;
	const char *p;

	fontname = get_recovery_fontname((size_t)(g_font_cur_sel));
	p = strrchr(fontname, '/');

	if(p != NULL) {
		fontname = p + 1;
	}

	sprintf(buf, "%-48s %-11s", g_messages[RECOVERY_FONT], fontname);

	return 0;
}

static int change_font_select_option(struct MenuEntry *entry, int direct)
{
	struct ValueOption *c = (struct ValueOption*)entry->arg;
	const char *fontname;

	*c->value -= c->limit_start;
	*c->value = limit_int(*c->value, direct, c->limit_end - c->limit_start);
	*c->value += c->limit_start;

	fontname = get_recovery_fontname((size_t)(g_font_cur_sel));

	if(0 == strcmp(fontname, "Default")) {
		fontname = "";
		proDebugScreenReleaseFont();
	} else {
		proDebugScreenSetFontFile(fontname, 1);
	}

	strcpy(g_cur_font_select, fontname);

	return 0;
}

static int change_font_select_option_by_enter(struct MenuEntry *entry)
{
	char buf[256], *p;
	
	change_font_select_option(entry, 1);
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

static int display_slim_color(struct MenuEntry* entry, char *buf, int size)
{
	sprintf(buf, "%-48s %-11s", g_messages[SLIM_COLOR], get_bool_name(g_config.slimcolor));

	return 0;
}

static int display_htmlviewer_custom_save_location(struct MenuEntry* entry, char *buf, int size)
{
	sprintf(buf, "%-48s %-11s", g_messages[HTMLVIEWER_CUSTOM_SAVE_LOCATION], get_bool_name(g_config.htmlviewer_custom_save_location));

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

static int display_use_usbversion(struct MenuEntry* entry, char *buf, int size)
{
	sprintf(buf, "%-48s %-11s", g_messages[USE_USBVERSION_TXT], get_bool_name(g_config.usbversion));

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
	{ NULL, 0, 0, &display_recovery_font, &change_font_select_option, &change_font_select_option_by_enter, &g_recovery_font_option },
	{ NULL, 0, 0, &display_xmb_usbdevice, &change_option, &change_option_by_enter, &g_xmb_usbdevice_option },
	{ NULL, 0, 0, &display_hidden_mac, &change_option, &change_option_by_enter, &g_mac_hidden_option },
	{ NULL, 0, 0, &display_skip_gameboot, &change_option, &change_option_by_enter, &g_skip_gameboot_option },
	{ NULL, 0, 0, &display_skip_logo, &change_option, &change_option_by_enter, &g_skip_logo_option },
	{ NULL, 0, 0, &display_use_ownupdate, &change_option, &change_option_by_enter, &g_use_ownupdate_option},
	{ NULL, 0, 0, &display_flash_protect, &change_option, &change_option_by_enter, &g_flash_protect_option },
	{ NULL, 0, 0, &display_htmlviewer_custom_save_location, &change_option, &change_option_by_enter, &g_htmlviewer_custom_save_location_option },
	{ NULL, 0, 0, &display_slim_color, &change_option, &change_option_by_enter, &g_slim_color_option },
	{ NULL, 0, 0, &display_use_version, &change_option, &change_option_by_enter, &g_use_version_option},
	{ NULL, 0, 0, &display_use_usbversion, &change_option, &change_option_by_enter, &g_use_usbversion_option},
	{ NULL, 0, 0, &display_hide_pic, &change_option, &change_option_by_enter, &g_hide_pic_option },
	{ NULL, 0, 0, &display_hibernation_deletion, &change_option, &change_option_by_enter, &g_hibblock_option },
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

static int display_iso_cache(struct MenuEntry* entry, char *buf, int size)
{
	sprintf(buf, "%-48s %-11s", g_messages[ISO_CACHE], get_bool_name(g_config.iso_cache));

	return 0;
}

static int display_iso_cache_total_size(struct MenuEntry* entry, char *buf, int size)
{
	sprintf(buf, "%-48s %-11d", g_messages[ISO_CACHE_TOTAL_SIZE], g_config.iso_cache_total_size);

	return 0;
}

static int display_iso_cache_number(struct MenuEntry* entry, char *buf, int size)
{
	sprintf(buf, "%-48s %-11d", g_messages[ISO_CACHE_NUMBER], g_config.iso_cache_num);

	return 0;
}

static int display_iso_cache_policy(struct MenuEntry* entry, char *buf, int size)
{
	sprintf(buf, "%-48s %-11s", g_messages[ISO_CACHE_POLICY], get_cache_policy_name(g_config.iso_cache_policy));

	return 0;
}

static int display_chn_iso(struct MenuEntry* entry, char *buf, int size)
{
	sprintf(buf, "%-48s %-11s", g_messages[ALLOW_NON_LATIN1_ISO_FILENAME], get_bool_name(g_config.chn_iso));

	return 0;
}

static int display_msspeed(struct MenuEntry* entry, char *buf, int size)
{
	char speedstr[20];

	if(g_config.msspeed == MSSPEED_NONE) {
		sprintf(speedstr, "None");
	} else if(g_config.msspeed == MSSPEED_POP) {
		sprintf(speedstr, "Pop");
	} else if(g_config.msspeed == MSSPEED_GAME) {
		sprintf(speedstr, "Game");
	} else if(g_config.msspeed == MSSPEED_VSH) {
		sprintf(speedstr, "Vsh");
	} else if(g_config.msspeed == MSSPEED_POP_GAME) {
		sprintf(speedstr, "Pop & Game");
	} else if(g_config.msspeed == MSSPEED_GAME_VSH) {
		sprintf(speedstr, "Game & Vsh");
	} else if(g_config.msspeed == MSSPEED_VSH_POP) {
		sprintf(speedstr, "Vsh & Pop");
	} else if(g_config.msspeed == MSSPEED_ALWAYS) {
		sprintf(speedstr, "Always");
	}

	sprintf(buf, "%-48s %-11s", g_messages[MSSPEED_UP], speedstr);

	return 0;
}

static int display_hide_cfw_dirs(struct MenuEntry* entry, char *buf, int size)
{
	sprintf(buf, "%-48s %-11s", g_messages[HIDE_CFW_DIRS], get_bool_name(g_config.hide_cfw_dirs));

	return 0;
}

static struct ValueOption g_xmb_plugin_option = {
	&g_config.plugvsh,
	0, 2,
};

static struct ValueOption g_game_plugin_option = {
	&g_config.pluggame,
	0, 2,
};

static struct ValueOption g_pops_plugin_option = {
	&g_config.plugpop,
	0, 2,
};

static struct ValueOption g_use_nodrm_option = {
	&g_config.usenodrm,
	0, 2,
};

static struct ValueOption g_use_noanalog_option = {
	&g_config.noanalog,
	0, 2,
};

static struct ValueOption g_use_oldplugin = {
	&g_config.oldplugin,
	0, 2,
};

static struct ValueOption g_iso_cache = {
	&g_config.iso_cache,
	0, 2,
};

static struct ValueOption g_iso_cache_total_size = {
	&g_config.iso_cache_total_size,
	1, 23+1,
};

static struct ValueOption g_iso_cache_number = {
	&g_config.iso_cache_num,
	64, 768+1,
};

static struct ValueOption g_iso_cache_policy = {
	&g_config.iso_cache_policy,
	0, CACHE_POLICY_RR+1,
};

static struct ValueOption g_chn_iso = {
	&g_config.chn_iso,
	0, 2,
};

static struct ValueOption g_msspeed = {
	&g_config.msspeed,
	0, MSSPEED_ALWAYS+1,
};

static struct ValueOption g_hide_cfw_dirs = {
	&g_config.hide_cfw_dirs,
	0, 2,
};

static struct MenuEntry g_advanced_menu_entries[] = {
	{ NULL, 0, 0, &display_xmb_plugin, &change_option, &change_option_by_enter, &g_xmb_plugin_option },
	{ NULL, 0, 0, &display_game_plugin, &change_option, &change_option_by_enter, &g_game_plugin_option },
	{ NULL, 0, 0, &display_pops_plugin, &change_option, &change_option_by_enter, &g_pops_plugin_option },
	{ NULL, 0, 0, &display_use_nodrm, &change_option, &change_option_by_enter, &g_use_nodrm_option},
	{ NULL, 0, 0, &display_msspeed, &change_option, &change_option_by_enter, &g_msspeed},
	{ NULL, 0, 0, &display_hide_cfw_dirs, &change_option, &change_option_by_enter, &g_hide_cfw_dirs},
	{ NULL, 0, 0, &display_use_noanalog, &change_option, &change_option_by_enter, &g_use_noanalog_option},
	{ NULL, 0, 0, &display_chn_iso, &change_option, &change_option_by_enter, &g_chn_iso},
	{ NULL, 0, 0, &display_use_oldplugin, &change_option, &change_option_by_enter, &g_use_oldplugin},
	{ NULL, 0, 0, &display_iso_cache, &change_option, &change_option_by_enter, &g_iso_cache},
	{ NULL, 0, 0, &display_iso_cache_total_size, &change_option, &change_option_by_enter, &g_iso_cache_total_size},
	{ NULL, 0, 0, &display_iso_cache_number, &change_iso_cache_number_option, &change_iso_cache_number_option_by_enter, &g_iso_cache_number},
	{ NULL, 0, 0, &display_iso_cache_policy, &change_option, &change_option_by_enter, &g_iso_cache_policy},
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
	{ &g_messages[TOGGLE_USB], 0, 0, NULL, NULL, &toggle_usb, NULL },
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

extern char g_cur_font_select[256];

static int configuration_menu(struct MenuEntry *entry)
{
	struct Menu *menu = &g_configuration_menu;

	menu->cur_sel = 0;
	menu_loop(menu);

	save_recovery_font_select();
	sctrlSESetConfig(&g_config);

	return 0;
}

static s16 g_xmb_clock_number, g_game_clock_number;

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
	0, 10,
};

struct ValueOption g_game_clock_option = {
	&g_game_clock_number,
	0, 10,
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

	get_registry_value("/CONFIG/SYSTEM/XMB", "button_assign", &value);
	value = !value;
	set_registry_value("/CONFIG/SYSTEM/XMB", "button_assign", value); 

	if(value) {
		sprintf(buf, "> %s %s", g_messages[SWAP_BUTTONS], g_messages[CONFIRM_BUTTON_IS_X]);
	} else {
		sprintf(buf, "> %s %s", g_messages[SWAP_BUTTONS], g_messages[CONFIRM_BUTTON_IS_O]);
	}

	set_bottom_info(buf, 0);
	frame_end();
	sceKernelDelayThread(CHANGE_DELAY);
	set_bottom_info("", 0);
	
	return 0;
}

static struct MenuEntry g_registery_menu_entries[] = {
	{ &g_messages[ACTIVATE_WMA], 0, 0, NULL, NULL, &active_wma, NULL },
	{ &g_messages[ACTIVATE_FLASH], 0, 0, NULL, NULL, &active_flash, NULL },
	{ &g_messages[SWAP_BUTTONS_FULL], 0, 0, NULL, NULL, &swap_buttons, NULL },
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
	int idx;

	(void)message_test;

	// setup font list size and cur select
	g_recovery_font_option.limit_end = (s16)fontlist_count(&g_font_list) + 1;
	idx = fontlist_find(&g_font_list, g_cur_font_select);

	if(idx == -1) {
		g_font_cur_sel = 0;
	} else {
		g_font_cur_sel = idx + 1;
	}
	
	menu->cur_sel = 0;
	menu_loop(menu);
}
