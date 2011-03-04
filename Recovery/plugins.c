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

#define MAX_PLUGINS_SIZE 32

struct Plugin {
	char name[256];
	int enabled;
	int type;
};

enum {
	ON_MS0 = 0,
	ON_EF0,
	ON_EH0,
};

static struct Plugin *g_vsh_list;
static struct Plugin *g_game_list;
static struct Plugin *g_pops_list;

static int g_vsh_cnt, g_game_cnt, g_pops_cnt;

static int plugins_ef0_menu(struct MenuEntry *entry);
static int plugins_eh0_menu(struct MenuEntry *entry);
static int plugins_ms0_menu(struct MenuEntry *entry);

static int g_type;

static struct Menu g_plugins_menu = {
	"Plugins",
	NULL,
	0,
	0,
	0xFF,
};

static char *get_line(int fd, char *linebuf, int bufsiz)
{
	int i, ret;

	if (linebuf == NULL || bufsiz < 2)
		return NULL;

	i = 0;
	memset(linebuf, 0, bufsiz);

	while (i < bufsiz - 1) {
		char c;

		ret = sceIoRead(fd, &c, 1);

		if (ret < 0 || (ret == 0 && i == 0))
			return NULL;

		if (ret == 0 || c == '\n' || c == '\r') {
			linebuf[i] = '\0';
			break;
		}

		linebuf[i++] = c;
	}

	linebuf[bufsiz-1] = '\0';

	return linebuf;
}

static int load_plugins(const char *config_path, struct Plugin *plugins, int *plugins_cnt, int type)
{
	SceUID fd;
	char linebuf[256], *p;
	int len;
	char *q;

	fd = sceIoOpen(config_path, PSP_O_RDONLY, 0777);

	if(fd < 0) {
		return fd;
	}

	*plugins_cnt = 0;

	do {
		p = get_line(fd, linebuf, sizeof(linebuf));

		if(p == NULL) {
			break;
		}

		q=strrchr(p, ' ');

		if(q == NULL)
			continue;

		strncpy(plugins->name, p, q-p);
		len = strlen(p);

		if (len >= 1 && p[len-1] == '1') {
			plugins->enabled = 1;
		} else {
			plugins->enabled = 0;
		}

		plugins->type = type;
		plugins++;
		(*plugins_cnt)++;
	} while (*plugins_cnt < MAX_PLUGINS_SIZE);

	sceIoClose(fd);

	return 0;
}

static int save_plugin(const char *path, struct Plugin *plugin, int size)
{
	SceUID fd;
	int i;

	if(plugin == NULL || size == 0) {
		return -1;
	}

	fd = sceIoOpen(path, PSP_O_WRONLY | PSP_O_TRUNC | PSP_O_CREAT, 0777);

	if(fd < 0) {
		return -2;
	}

	for(i=0; i<size; ++i) {
		char linebuf[256];

		sprintf(linebuf, "%s %d\r\n", plugin[i].name, plugin[i].enabled);
		sceIoWrite(fd, linebuf, strlen(linebuf));
	}

	sceIoClose(fd);

	return 0;
}

static int save_plugins(void)
{
	if(g_type == ON_MS0) {
		save_plugin("ms0:/seplugins/vsh.txt", g_vsh_list, g_vsh_cnt);
		save_plugin("ms0:/seplugins/game.txt", g_game_list, g_game_cnt);
		save_plugin("ms0:/seplugins/pops.txt", g_pops_list, g_pops_cnt);
	} else if(g_type == ON_EF0) {
		save_plugin("ef0:/seplugins/vsh.txt", g_vsh_list, g_vsh_cnt);
		save_plugin("ef0:/seplugins/game.txt", g_game_list, g_game_cnt);
		save_plugin("ef0:/seplugins/pops.txt", g_pops_list, g_pops_cnt);
	} else if(g_type == ON_EH0) {
		save_plugin("eh0:/seplugins/vsh.txt", g_vsh_list, g_vsh_cnt);
		save_plugin("eh0:/seplugins/game.txt", g_game_list, g_game_cnt);
		save_plugin("eh0:/seplugins/pops.txt", g_pops_list, g_pops_cnt);
	}

	return 0;
}

static int change_value_callback(struct MenuEntry *entry, int direct)
{
	struct Plugin *plugin = (struct Plugin*)entry->arg;

	plugin->enabled = limit_int(plugin->enabled, direct, 2);
	save_plugins();

	return 0;
}

static int enter_callback(struct MenuEntry *entry)
{
	char buf[256], *p;
	
	change_value_callback(entry, 1);

	strcpy(buf, "> ");
	p = buf + strlen(buf);
	(*entry->display_callback)(entry, p, 256 - (p - buf));
	set_bottom_info(buf, 0);
	frame_end();
	sceKernelDelayThread(CHANGE_DELAY);
	set_bottom_info("", 0);

	return 0;
}

static int display_callback(struct MenuEntry* entry, char *buf, int size)
{
	struct Plugin *plugin = (struct Plugin *)entry->arg;
	const char *name;

	name = strrchr(plugin->name, '/');

	if(name) {
		name++;
	} else {
		name = plugin->name;
	}

	sprintf(buf, "%s [%s]", name, get_plugin_name(plugin->type));
	sprintf(buf, "%-48s %-11s", buf, get_bool_name(plugin->enabled));

	return 0;
}

static void create_submenu(struct MenuEntry *entry, struct Plugin *plugins, int plugins_cnt)
{
	int i;

	for(i=0; i<plugins_cnt; ++i) {
		entry->display_callback = &display_callback;
		entry->change_value_callback = &change_value_callback;
		entry->enter_callback = &enter_callback;
		entry->arg = &plugins[i];
		entry++;
	}
}

static void free_submenu(struct Menu *menu)
{
	int i;

	if(menu->submenu == NULL) {
		return;
	}

	for(i=0; i<menu->submenu_size; ++i) {
		if(menu->submenu->info != NULL) {
			vpl_free(menu->submenu->info);
		}
	}

	menu->submenu = NULL;
	menu->submenu_size = 0;
}

static void create_submenus(struct Menu *menu)
{
	struct MenuEntry *entry;
	int total;

	total = g_vsh_cnt + g_game_cnt + g_pops_cnt;
	entry = vpl_alloc(total * sizeof(entry[0]));

	if(entry == NULL) {
		return;
	}

	memset(entry, 0, total * sizeof(entry[0]));
	create_submenu(entry, g_vsh_list, g_vsh_cnt);
	create_submenu(entry+g_vsh_cnt, g_game_list, g_game_cnt);
	create_submenu(entry+g_vsh_cnt+g_game_cnt, g_pops_list, g_pops_cnt);

	menu->submenu = entry;
	menu->submenu_size = total;
}

static struct MenuEntry g_plugins_pspgo[] = {
	{ "System storage", 1, 0, NULL, NULL, &plugins_ef0_menu, NULL},
	{ "Memory stick", 1, 0, NULL, NULL, &plugins_eh0_menu, NULL},
};

int init_plugin_list(void)
{
	g_vsh_list = vpl_alloc(sizeof(g_vsh_list[0]) * MAX_PLUGINS_SIZE);
	g_game_list = vpl_alloc(sizeof(g_game_list[0]) * MAX_PLUGINS_SIZE);
	g_pops_list = vpl_alloc(sizeof(g_pops_list[0]) * MAX_PLUGINS_SIZE);

	if(g_vsh_list == NULL || g_game_list == NULL || g_pops_list == NULL) {
		return -1;
	}

	memset(g_vsh_list, 0, sizeof(g_vsh_list[0]) * MAX_PLUGINS_SIZE);
	memset(g_game_list, 0, sizeof(g_game_list[0]) * MAX_PLUGINS_SIZE);
	memset(g_pops_list, 0, sizeof(g_pops_list[0]) * MAX_PLUGINS_SIZE);

	g_vsh_cnt = g_game_cnt = g_pops_cnt = 0;

	return 0;
}

void free_plugin_list(void)
{
	if(g_vsh_list != NULL) {
		vpl_free(g_vsh_list);
		g_vsh_list = NULL;
	}

	if(g_game_list != NULL) {
		vpl_free(g_game_list);
		g_game_list = NULL;
	}

	if(g_pops_list != NULL) {
		vpl_free(g_pops_list);
		g_pops_list = NULL;
	}
}

static struct Menu g_ef0_plugins_menu = {
	"Plugins on system storage",
	NULL,
	0,
	0,
	0xFF,
};

static struct Menu g_eh0_plugins_menu = {
	"Plugins on memory stick",
	NULL,
	0,
	0,
	0xFF,
};

static struct Menu g_ms0_plugins_menu = {
	"Plugins",
	NULL,
	0,
	0,
	0xFF,
};

static int plugins_menu_on_device(struct MenuEntry *entry, struct Menu *menu, const char *devicename)
{
	int ret;
	char path[256];

	ret = init_plugin_list();

	if(ret < 0) {
		return ret;
	}

	sprintf(path, "%s/seplugins/vsh.txt", devicename);
	load_plugins(path, g_vsh_list, &g_vsh_cnt, TYPE_VSH);
	sprintf(path, "%s/seplugins/game.txt", devicename);
	load_plugins(path, g_game_list, &g_game_cnt, TYPE_GAME);
	sprintf(path, "%s/seplugins/pops.txt", devicename);
	load_plugins(path, g_pops_list, &g_pops_cnt, TYPE_POPS);
	create_submenus(menu);

	menu->cur_sel = 0;
	menu_loop(menu);

	free_submenu(menu);
	free_plugin_list();

	return 0;
}

static int plugins_ef0_menu(struct MenuEntry *entry)
{
	g_type = ON_EF0;

	return plugins_menu_on_device(entry, &g_ef0_plugins_menu, "ef0:");
}

static int plugins_eh0_menu(struct MenuEntry *entry)
{
	g_type = ON_EH0;

	return plugins_menu_on_device(entry, &g_eh0_plugins_menu, "eh0:");
}

static int plugins_ms0_menu(struct MenuEntry *entry)
{
	g_type = ON_MS0;

	return plugins_menu_on_device(entry, &g_ms0_plugins_menu, "ms0:");
}

int plugins_menu(struct MenuEntry *entry)
{
	int psp_model;

	psp_model = kuKernelGetModel();

	if(psp_model == PSP_GO) {
		struct Menu *menu = &g_plugins_menu;

		menu->submenu = g_plugins_pspgo;
		menu->submenu_size = NELEMS(g_plugins_pspgo);
		menu->cur_sel = 0;

		menu_loop(menu);

		return 0;
	}

	return plugins_ms0_menu(entry);
}
