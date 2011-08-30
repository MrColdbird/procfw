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
#include <ctype.h>

#include "kubridge.h"
#include "systemctrl.h"
#include "systemctrl_se.h"
#include "vshctrl.h"
#include "utils.h"
#include "vpl.h"
#include "main.h"
#include "strsafe.h"
#include "vpl.h"

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

static struct Plugin g_vsh_list_head, *g_vsh_list_tail;
static struct Plugin g_game_list_head, *g_game_list_tail;
static struct Plugin g_pops_list_head, *g_pops_list_tail;

static int plugins_ef0_menu(struct MenuEntry *entry);
static int plugins_ms0_menu(struct MenuEntry *entry);

static int g_type;

static struct Menu g_plugins_menu = {
	PLUGINS,
	NULL,
	0,
	0,
	0xFF,
};

#define READ_BUF_SIZE 1024

static char *read_buf = NULL;
static char *read_ptr = NULL;
static int read_cnt = 0;

static int buf_read(SceUID fd, char *p)
{
	if(read_cnt <= 0) {
		read_cnt = sceIoRead(fd, read_buf, READ_BUF_SIZE);

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

static int read_lines(SceUID fd, char *lines, size_t linebuf_size)
{
	char *p;
	int ret;
	size_t re;

	if(linebuf_size == 0) {
		return -1;
	}

	p = lines;
	re = linebuf_size;

	while(re -- != 0) {
		ret = buf_read(fd, p);

		if(ret < 0) {
			break;
		}

		if(ret == 0) {
			if(p == lines) {
				ret = -1;
			}

			break;
		}

		if(*p == '\r') {
			continue;
		}

		if(*p == '\n') {
			break;
		}

		p++;
	}

	if(p < lines + linebuf_size) {
		*p = '\0';
	}

	return ret >= 0 ? p - lines : ret;
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

static void add_plugin(struct Plugin **tail, struct Plugin *plugin)
{
	(*tail)->next = plugin;
	plugin->next = NULL;
	*tail = plugin;
}

void parse_plugin(struct Plugin **tail, char *linebuf, int type)
{
	char *p;
	int enabled = 1;
	struct Plugin *plugin;

	for(p=linebuf; *p != '\0'; p++) {
		if(isspace((int)*p)) {
			*p++ = '\0';
			enabled = 0;

			while(*p != '\0' && isspace((int)*p)) {
				p++;
			}

			if(*p == '1') {
				enabled = 1;
				break;
			}

			break;
		}
	}

	plugin = new_plugin(linebuf, enabled, type);

	if(plugin != NULL) {
		add_plugin(tail, plugin);
	}
}

static int load_plugins(const char *config_path, struct Plugin **tail, int type)
{
	SceUID fd;
	char linebuf[256];
	char *read_alloc_buf;

	fd = sceIoOpen(config_path, PSP_O_RDONLY, 0777);

	if(fd < 0) {
		return fd;
	}

	read_alloc_buf = vpl_alloc(READ_BUF_SIZE + 64);

	if(read_alloc_buf == NULL) {
		sceIoClose(fd);
		return -1;
	}

	read_buf = (void*)(((u32)read_alloc_buf & (~(64-1))) + 64);
	linebuf[sizeof(linebuf)-1] = '\0';

	while(read_lines(fd, linebuf, sizeof(linebuf)-1) >= 0) {
		parse_plugin(tail, linebuf, type);
	}

	sceIoClose(fd);
	vpl_free(read_alloc_buf);

	return 0;
}

static int save_plugin(const char *path, struct Plugin *head)
{
	SceUID fd;
	char linebuf[256];
	struct Plugin *plugin;

	fd = sceIoOpen(path, PSP_O_WRONLY | PSP_O_TRUNC | PSP_O_CREAT, 0777);

	if(fd < 0) {
		return -2;
	}

	plugin = head->next;

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
		save_plugin("ms0:/seplugins/vsh.txt", &g_vsh_list_head);
		save_plugin("ms0:/seplugins/game.txt", &g_game_list_head);
		save_plugin("ms0:/seplugins/pops.txt", &g_pops_list_head);
	} else if(g_type == ON_EF0) {
		save_plugin("ef0:/seplugins/vsh.txt", &g_vsh_list_head);
		save_plugin("ef0:/seplugins/game.txt", &g_game_list_head);
		save_plugin("ef0:/seplugins/pops.txt", &g_pops_list_head);
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
	char fmt[20];

	name = strrchr(plugin->name, '/');

	if(name) {
		name++;
	} else {
		name = plugin->name;
	}

	sprintf(fmt, "%%.%ds [%%s]", 48 - 3 - strlen(get_plugin_name(plugin->type)));
	sprintf(buf, fmt, name, get_plugin_name(plugin->type));
	sprintf(buf, "%-48s %-11s", buf, get_bool_name(plugin->enabled));

	return 0;
}

static void create_submenu(struct MenuEntry *entry, struct Plugin *head)
{
	struct Plugin *plugin;

	plugin = head->next;

	while(plugin != NULL) {
		entry->info_idx = -1;
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

static int get_plugin_list_count(struct Plugin *head)
{
	int cnt;
	struct Plugin *plugin;

	cnt = 0;
	plugin = head->next;

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

	vsh_cnt = get_plugin_list_count(&g_vsh_list_head);
	game_cnt = get_plugin_list_count(&g_game_list_head);
	pops_cnt = get_plugin_list_count(&g_pops_list_head);

	total = vsh_cnt + game_cnt + pops_cnt;
	entry = vpl_alloc(total * sizeof(entry[0]));

	if(entry == NULL) {
		return;
	}

	memset(entry, 0, total * sizeof(entry[0]));
	create_submenu(entry, &g_vsh_list_head);
	create_submenu(entry+vsh_cnt, &g_game_list_head);
	create_submenu(entry+vsh_cnt+game_cnt, &g_pops_list_head);

	menu->submenu = entry;
	menu->submenu_size = total;
}

static struct MenuEntry g_plugins_pspgo[] = {
	{ SYSTEM_STORAGE, 1, 0, NULL, NULL, &plugins_ef0_menu, NULL},
	{ MEMORY_STICK, 1, 0, NULL, NULL, &plugins_ms0_menu, NULL},
};

int init_plugin_list(void)
{
	g_vsh_list_head.next = NULL;
	g_game_list_head.next = NULL;
	g_pops_list_head.next =  NULL;

	g_vsh_list_tail = &g_vsh_list_head;
	g_game_list_tail = &g_game_list_head;
	g_pops_list_tail = &g_pops_list_head;

	return 0;
}

static void free_plugin_list(struct Plugin * head, struct Plugin ** tail)
{
	struct Plugin *p, *next;

	p = head->next;

	while(p != NULL) {
		next = p->next;

		if(p->name) {
			vpl_free(p->name);
		}

		vpl_free(p);
		p = next;
	}

	head->next = NULL;
	*tail = head;
}

static void free_plugin_lists(void)
{
	free_plugin_list(&g_vsh_list_head, &g_vsh_list_tail);
	free_plugin_list(&g_game_list_head, &g_game_list_tail);
	free_plugin_list(&g_pops_list_head, &g_pops_list_tail);
}

static struct Menu g_ef0_plugins_menu = {
	PLUGINS_ON_SYSTEM_STORAGE,
	NULL,
	0,
	0,
	0xFF,
};

static struct Menu g_ms0_plugins_menu = {
	PLUGINS_ON_MEMORY_STICK,
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
	load_plugins(path, &g_vsh_list_tail, TYPE_VSH);
	sprintf(path, "%s/seplugins/game.txt", devicename);
	load_plugins(path, &g_game_list_tail, TYPE_GAME);
	sprintf(path, "%s/seplugins/pops.txt", devicename);
	load_plugins(path, &g_pops_list_tail, TYPE_POPS);

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
