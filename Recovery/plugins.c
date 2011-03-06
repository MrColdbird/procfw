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

struct Plugin {
	char *name;
	int enabled;
	int type;
};

enum {
	ON_MS0 = 0,
	ON_EF0,
};

static struct Plugin *g_vsh_list;
static struct Plugin *g_game_list;
static struct Plugin *g_pops_list;

static int g_vsh_cnt, g_game_cnt, g_pops_cnt;

static int plugins_ef0_menu(struct MenuEntry *entry);
static int plugins_ms0_menu(struct MenuEntry *entry);

static int g_type;

static struct Menu g_plugins_menu = {
	&g_messages[PLUGINS],
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

static struct Plugin* add_plugin(struct Plugin **plugins, int *plugins_cnt)
{
	struct Plugin *plug;

	plug = vpl_realloc(*plugins, ((*plugins_cnt)+1)*sizeof(*plugins[0]));

	if(plug == NULL) {
		return NULL;
	}

	(*plugins_cnt)++;
	*plugins = plug;
	plug = &plug[(*plugins_cnt)-1];

	return plug;
}

static int load_plugins(const char *config_path, struct Plugin **plugins, int *plugins_cnt, int type)
{
	SceUID fd;
	char linebuf[256], *p;
	int len;
	char *q;
	struct Plugin *plug;

	fd = sceIoOpen(config_path, PSP_O_RDONLY, 0777);

	if(fd < 0) {
		return fd;
	}

	*plugins_cnt = 0;

	do {
		p = get_line(fd, linebuf, sizeof(linebuf));

		if(p == NULL)
			break;

		len = strlen(p);

		if(len == 0) {
			continue;
		}

		for(q=p; *q != ' ' && *q != '\t' && *q != '\0'; ++q) {
		}

		if (*q == '\0') {
			plug = add_plugin(plugins, plugins_cnt);

			if(plug == NULL) {
				break;
			}

			plug->name = vpl_strdup(p);
			plug->enabled = 1;
			plug->type = type;
			continue;
		}

		while(len >= 1) {
			if(p[len-1] == ' ' || p[len-1] == '\t')
				len--;
			else
				break;
		}

		plug = add_plugin(plugins, plugins_cnt);

		if(plug == NULL) {
			break;
		}

		*q = '\0';
		plug->name = vpl_strdup(p);
		plug->type = type;

		if (p[len-1] == '0') {
			plug->enabled = 0;
		} else {
			plug->enabled = 1;
		}
	} while (1);

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
	{ &g_messages[SYSTEM_STORAGE], 1, 0, NULL, NULL, &plugins_ef0_menu, NULL},
	{ &g_messages[MEMORY_STICK], 1, 0, NULL, NULL, &plugins_ms0_menu, NULL},
};

int init_plugin_list(void)
{
	g_vsh_list = NULL;
	g_game_list = NULL;
	g_pops_list = NULL;

	g_vsh_cnt = g_game_cnt = g_pops_cnt = 0;

	return 0;
}

static void free_plugin_list(struct Plugin* plugin, int size)
{
	int i;

	if(plugin == NULL) {
		return;
	}

	for(i=0; i<size; ++i) {
		if(plugin[i].name) {
			vpl_free(plugin[i].name);
		}
	}

	vpl_free(plugin);
}

static void free_plugin_lists(void)
{
	free_plugin_list(g_vsh_list, g_vsh_cnt);
	g_vsh_list = NULL;

	free_plugin_list(g_game_list, g_game_cnt);
	g_game_list = NULL;
	
	free_plugin_list(g_pops_list, g_pops_cnt);
	g_pops_list = NULL;
}

static struct Menu g_ef0_plugins_menu = {
	&g_messages[PLUGINS_ON_SYSTEM_STORAGE],
	NULL,
	0,
	0,
	0xFF,
};

static struct Menu g_ms0_plugins_menu = {
	&g_messages[PLUGINS_ON_MEMORY_STICK],
	NULL,
	0,
	0,
	0xFF,
};

static int plugins_menu_on_device(struct MenuEntry *entry, struct Menu *menu, const char *devicename)
{
	char path[256];

	init_plugin_list();
	sprintf(path, "%s/seplugins/vsh.txt", devicename);
	load_plugins(path, &g_vsh_list, &g_vsh_cnt, TYPE_VSH);
	sprintf(path, "%s/seplugins/game.txt", devicename);
	load_plugins(path, &g_game_list, &g_game_cnt, TYPE_GAME);
	sprintf(path, "%s/seplugins/pops.txt", devicename);
	load_plugins(path, &g_pops_list, &g_pops_cnt, TYPE_POPS);
	create_submenus(menu);

	menu->cur_sel = 0;
	menu_loop(menu);

	free_submenu(menu);
	free_plugin_lists();

	return 0;
}

static int plugins_ef0_menu(struct MenuEntry *entry)
{
	g_type = ON_EF0;

	return plugins_menu_on_device(entry, &g_ef0_plugins_menu, "ef0:");
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
