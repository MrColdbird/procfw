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
#include "strsafe.h"

struct Plugin {
	struct Plugin *next;
	char *name;
	int enabled;
	int type;
};

enum {
	ON_MS0 = 0,
	ON_EF0,
};

static struct Plugin *g_vsh_list_head, *g_vsh_list_tail;
static struct Plugin *g_game_list_head, *g_game_list_tail;
static struct Plugin *g_pops_list_head, *g_pops_list_tail;

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

static char read_buf[80];
static char *read_ptr = NULL;
static int read_cnt = 0;

static int buf_read(SceUID fd, char *p)
{
	if(read_cnt <= 0) {
		read_cnt = sceIoRead(fd, read_buf, sizeof(read_buf));

		if(read_cnt < 0) {
			return read_cnt;
		}

		if(read_cnt == 0) {
			return read_cnt;
		}

		read_ptr = read_buf;
	}

	read_cnt--;
	*p = *read_ptr++;

	return 1;
}

static char *get_line(int fd, char *linebuf, int bufsiz)
{
	int i, ret;

	if (linebuf == NULL || bufsiz < 2)
		return NULL;

	i = 0;
	memset(linebuf, 0, bufsiz);

	while (i < bufsiz - 1) {
		char c;

		ret = buf_read(fd, &c);

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

static struct Plugin* new_plugin(const char *name, int enabled, int type)
{
	struct Plugin *plug;

	plug = vpl_alloc(sizeof(*plug));

	if(plug == NULL) {
		return NULL;
	}

	plug->next = NULL;
	plug->name = vpl_strdup(name);
	plug->enabled = enabled;
	plug->type = type;

	return plug;
}

static int load_plugins(const char *config_path, struct Plugin **head, struct Plugin **tail, int type)
{
	SceUID fd;
	char linebuf[256], *p;
	int len, enabled;
	char *q;
	struct Plugin *plug;

	fd = sceIoOpen(config_path, PSP_O_RDONLY, 0777);

	if(fd < 0) {
		return fd;
	}

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
			plug = new_plugin(p, 0, type);

			if(plug == NULL) {
				break;
			}

			if(*head == NULL) {
				*head = *tail = plug;
			} else {
				(*tail)->next = plug;
				(*tail) = plug;
			}

			continue;
		}

		while(len >= 1) {
			if(p[len-1] == ' ' || p[len-1] == '\t')
				len--;
			else
				break;
		}

		if (p[len-1] == '1') {
			enabled = 1;
		} else {
			enabled = 0;
		}

		*q = '\0';
		plug = new_plugin(p, enabled, type);

		if(plug == NULL) {
			break;
		}

		if(*head == NULL) {
			*head = *tail = plug;
		} else {
			(*tail)->next = plug;
			(*tail) = plug;
		}
	} while (1);

	sceIoClose(fd);

	return 0;
}

static int save_plugin(const char *path, struct Plugin *plugin)
{
	SceUID fd;
	char linebuf[256];

	if(plugin == NULL) {
		return -1;
	}

	fd = sceIoOpen(path, PSP_O_WRONLY | PSP_O_TRUNC | PSP_O_CREAT, 0777);

	if(fd < 0) {
		return -2;
	}

	while(plugin != NULL) {
		sprintf(linebuf, "%s %d\r\n", plugin->name, plugin->enabled);
		sceIoWrite(fd, linebuf, strlen(linebuf));
		plugin = plugin->next;
	}

	sceIoClose(fd);

	return 0;
}

static int save_plugins(void)
{
	if(g_type == ON_MS0) {
		save_plugin("ms0:/seplugins/vsh.txt", g_vsh_list_head);
		save_plugin("ms0:/seplugins/game.txt", g_game_list_head);
		save_plugin("ms0:/seplugins/pops.txt", g_pops_list_head);
	} else if(g_type == ON_EF0) {
		save_plugin("ef0:/seplugins/vsh.txt", g_vsh_list_head);
		save_plugin("ef0:/seplugins/game.txt", g_game_list_head);
		save_plugin("ef0:/seplugins/pops.txt", g_pops_list_head);
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

	sprintf(buf, "%.64s [%s]", name, get_plugin_name(plugin->type));
	sprintf(buf, "%-48s %-11s", buf, get_bool_name(plugin->enabled));

	return 0;
}

static void create_submenu(struct MenuEntry *entry, struct Plugin *plugin)
{
	while(plugin != NULL) {
		entry->display_callback = &display_callback;
		entry->change_value_callback = &change_value_callback;
		entry->enter_callback = &enter_callback;
		entry->arg = plugin;
		entry++;
		plugin = plugin->next;
	}
}

static void free_submenu(struct Menu *menu)
{
	if(menu->submenu == NULL) {
		return;
	}

	vpl_free(menu->submenu);
	menu->submenu = NULL;
	menu->submenu_size = 0;
}

static int get_plugin_list_count(struct Plugin *plugin)
{
	int cnt;

	cnt = 0;

	while(plugin != NULL) {
		cnt++;
		plugin = plugin->next;
	}

	return cnt;
}

static void create_submenus(struct Menu *menu)
{
	struct MenuEntry *entry;
	int total;
	int vsh_cnt, game_cnt, pops_cnt;

	vsh_cnt = get_plugin_list_count(g_vsh_list_head);
	game_cnt = get_plugin_list_count(g_game_list_head);
	pops_cnt = get_plugin_list_count(g_pops_list_head);

	total = vsh_cnt + game_cnt + pops_cnt;
	entry = vpl_alloc(total * sizeof(entry[0]));

	if(entry == NULL) {
		return;
	}

	memset(entry, 0, total * sizeof(entry[0]));
	create_submenu(entry, g_vsh_list_head);
	create_submenu(entry+vsh_cnt, g_game_list_head);
	create_submenu(entry+vsh_cnt+game_cnt, g_pops_list_head);

	menu->submenu = entry;
	menu->submenu_size = total;
}

static struct MenuEntry g_plugins_pspgo[] = {
	{ &g_messages[SYSTEM_STORAGE], 1, 0, NULL, NULL, &plugins_ef0_menu, NULL},
	{ &g_messages[MEMORY_STICK], 1, 0, NULL, NULL, &plugins_ms0_menu, NULL},
};

int init_plugin_list(void)
{
	g_vsh_list_head = g_vsh_list_tail = NULL;
	g_game_list_head = g_game_list_tail = NULL;
	g_pops_list_head = g_pops_list_tail = NULL;

	return 0;
}

static void free_plugin_list(struct Plugin ** head, struct Plugin ** tail)
{
	struct Plugin *p;

	while((*head) != NULL) {
		p = (*head)->next;

		if((*head)->name) {
			vpl_free((*head)->name);
		}

		vpl_free(*head);
		*head = p;
	}

	*tail = *head;
}

static void free_plugin_lists(void)
{
	free_plugin_list(&g_vsh_list_head, &g_vsh_list_tail);
	free_plugin_list(&g_game_list_head, &g_game_list_tail);
	free_plugin_list(&g_pops_list_head, &g_pops_list_tail);
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
	load_plugins(path, &g_vsh_list_head, &g_vsh_list_tail, TYPE_VSH);
	sprintf(path, "%s/seplugins/game.txt", devicename);
	load_plugins(path, &g_game_list_head, &g_game_list_tail, TYPE_GAME);
	sprintf(path, "%s/seplugins/pops.txt", devicename);
	load_plugins(path, &g_pops_list_head, &g_pops_list_tail, TYPE_POPS);

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
