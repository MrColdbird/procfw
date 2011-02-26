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

static int sub_menu(void *);
static int sub_menu2(void *);
static int sub_menu3(void *);

static struct ValueOption g_test_option1 = {
	0,
	2,
};

static struct ValueOption g_test_option2 = { 
	0,
	4,
};

static int display_test_option1(char *buf, int size)
{
	return sprintf(buf, "3. bool option = %d", g_test_option1.value);
}

static int display_test_option2(char *buf, int size)
{
	return sprintf(buf, "4. value option = %d", g_test_option2.value);
}

static int change_value_option(void *arg, int direct)
{
	struct ValueOption *c = (struct ValueOption*)arg;

	c->value = limit_int(c->value, direct, c->limit);

	return 0;
}

static int menu_change_int(void *arg)
{
	struct ValueOption *c = arg;
	char buf[256];
	int *p = &(c->value);

	*p = limit_int(*p, 1, c->limit);
	sprintf(buf, "%s: %d\n", __func__, *p);
	set_bottom_info(buf, 0);
	frame_end();
	sceKernelDelayThread(1000000);
	set_bottom_info("", 0);
	
	return 0;
}

static int menu_change_bool(void *arg)
{
	u32 *p = (u32*)arg;
	char buf[256];

	*p = !*p;

	sprintf(buf, "%s: %d\n", __func__, *p);
	set_bottom_info(buf, 0);
	frame_end();
	sceKernelDelayThread(CHANGE_DELAY);
	set_bottom_info("", 0);
	
	return 0;
}

static struct MenuEntry g_top_menu_entries[] = {
	{ "1. Empty option", 0, },
	{ "2. Submenu option", 1, 0xFF00, NULL, NULL, &sub_menu, NULL},
	{ NULL, 0, 0, &display_test_option1, &change_value_option, &menu_change_bool, &g_test_option1 },
	{ NULL, 0, 0, &display_test_option2, &change_value_option, &menu_change_int, &g_test_option2 },
};

static struct Menu g_top_menu = {
	"PRO Recovery Menu",
	g_top_menu_entries,
	NELEMS(g_top_menu_entries),
	0,
	0xFF,
};

static struct MenuEntry g_sub_menu_entries[] = {
	{ "1. Normal option", 0, },
	{ "2. Green  option", 0, 0xFF00, },
	{ "3. Submenu option", 1, 0xFF00, NULL, NULL, &sub_menu2, NULL},
	{ "3. Dynamic Submenu option", 1, 0xFF0000, NULL, NULL, &sub_menu3, NULL},
};

static struct Menu g_sub_menu = {
	"PRO Sub Menu",
	g_sub_menu_entries,
	NELEMS(g_sub_menu_entries),
	0,
	0xFF,
};

static struct MenuEntry g_sub_menu_2_entries[] = {
	{ "1. Normal option", 0, },
};

static struct Menu g_sub_menu_2 = {
	"PRO Sub Menu 2",
	g_sub_menu_2_entries,
	NELEMS(g_sub_menu_2_entries),
	0,
	0xFF00,
};

static struct Menu g_sub_menu_3 = {
	"PRO Sub Menu Dynamic",
	NULL,
	0,
	0,
	0xFF00,
};

static int sub_menu(void * arg)
{
	struct Menu *menu = &g_sub_menu;

	menu->cur_sel = 0;
	menu_loop(menu);

	return 0;
}

static int sub_menu2(void * arg)
{
	struct Menu *menu = &g_sub_menu_2;

	menu->cur_sel = 0;
	menu_loop(menu);

	return 0;
}

static int sub_menu3(void * arg)
{
	struct Menu *menu = &g_sub_menu_3;

	srand(time(0));
	menu->submenu_size = rand() % 20 + 1;
	menu->submenu = vpl_alloc(menu->submenu_size * sizeof(*menu->submenu));
	memset(menu->submenu, 0, menu->submenu_size * sizeof(*menu->submenu));

	int i; for(i=0; i<menu->submenu_size; ++i) {
		char buf[20];

		sprintf(buf, "%d. entry", i);
		menu->submenu[i].info = vpl_strdup(buf);
	}

	menu->cur_sel = 0;
	menu_loop(menu);

	for(i=0; i<menu->submenu_size; ++i) {
		vpl_free(menu->submenu[i].info);
	}

	return 0;
}

void main_menu(void)
{
	struct Menu *menu = &g_top_menu;
	
	menu->cur_sel = 0;
	menu_loop(menu);
}
