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

static struct Plugin *g_vsh_list;
static struct Plugin *g_game_list;
static struct Plugin *g_pops_list;

static int g_vsh_cnt, g_game_cnt, g_pops_cnt;

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
	save_plugin("ms0:/seplugins/vsh.txt", g_vsh_list, g_vsh_cnt);
	save_plugin("ms0:/seplugins/game.txt", g_game_list, g_game_cnt);
	save_plugin("ms0:/seplugins/pops.txt", g_pops_list, g_pops_cnt);

	return 0;
}

static int enter_callback(struct MenuEntry *entry)
{
	char buf[256];
	struct Plugin *plugin = (struct Plugin*)entry->arg;

	plugin->enabled = limit_int(plugin->enabled, 1, 2);

	(*entry->display_callback)(entry, buf, 256);
	set_bottom_info(buf, 0);
	frame_end();
	save_plugins();
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

	sprintf(buf, "%s [%s] (%s)", name, get_plugin_name(plugin->type), get_bool_name(plugin->enabled));

	return 0;
}

static void create_submenu(struct MenuEntry *entry, struct Plugin *plugins, int plugins_cnt)
{
	int i;

	for(i=0; i<plugins_cnt; ++i) {
		entry->display_callback = &display_callback;
		entry->enter_callback = &enter_callback;
		entry->arg = &plugins[i];
		entry++;
	}
}

static void del_submenu(void)
{
	int i;
	struct MenuEntry *entry;

	if(g_plugins_menu.submenu == NULL) {
		return;
	}

	for(i=0; i<g_plugins_menu.submenu_size; ++i) {
		if(g_plugins_menu.submenu->info != NULL) {
			vpl_free(g_plugins_menu.submenu->info);
		}

		entry++;
	}

	g_plugins_menu.submenu = NULL;
	g_plugins_menu.submenu_size = 0;
}

static void create_menu(void)
{
	struct MenuEntry *entry;
	int total;

	total = g_vsh_cnt + g_game_cnt + g_pops_cnt;
	entry = vpl_alloc(total * sizeof(entry[0]));

	if(entry == NULL) {
		return;
	}

	g_plugins_menu.submenu = entry;
	g_plugins_menu.submenu_size = total;

	create_submenu(entry, g_vsh_list, g_vsh_cnt);
	create_submenu(entry+g_vsh_cnt, g_game_list, g_game_cnt);
	create_submenu(entry+g_vsh_cnt+g_game_cnt, g_pops_list, g_pops_cnt);
}

int plugins_menu(struct MenuEntry *entry)
{
	struct Menu *menu = &g_plugins_menu;
	int psp_model;

	g_vsh_list = vpl_alloc(sizeof(g_vsh_list[0]) * MAX_PLUGINS_SIZE);
	g_game_list = vpl_alloc(sizeof(g_game_list[0]) * MAX_PLUGINS_SIZE);
	g_pops_list = vpl_alloc(sizeof(g_pops_list[0]) * MAX_PLUGINS_SIZE);

	if(g_vsh_list == NULL || g_game_list == NULL || g_pops_list == NULL) {
		goto exit;
	}

	memset(g_vsh_list, 0, sizeof(g_vsh_list[0]) * MAX_PLUGINS_SIZE);
	memset(g_game_list, 0, sizeof(g_game_list[0]) * MAX_PLUGINS_SIZE);
	memset(g_pops_list, 0, sizeof(g_pops_list[0]) * MAX_PLUGINS_SIZE);

	psp_model = kuKernelGetModel();

	if(psp_model == PSP_GO) {
		load_plugins("ef0:/seplugins/vsh.txt", g_vsh_list, &g_vsh_cnt, TYPE_VSH);
		load_plugins("ef0:/seplugins/game.txt", g_game_list, &g_game_cnt, TYPE_GAME);
		load_plugins("ef0:/seplugins/pops.txt", g_pops_list, &g_pops_cnt, TYPE_POPS);
		create_menu();
	} else {
		load_plugins("ms0:/seplugins/vsh.txt", g_vsh_list, &g_vsh_cnt, TYPE_VSH);
		load_plugins("ms0:/seplugins/game.txt", g_game_list, &g_game_cnt, TYPE_GAME);
		load_plugins("ms0:/seplugins/pops.txt", g_pops_list, &g_pops_cnt, TYPE_POPS);
		create_menu();
	}

	menu->cur_sel = 0;
	menu_loop(menu);
	del_submenu();

exit:
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

	return 0;
}
