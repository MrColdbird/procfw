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
#include <psppower.h>

#include "systemctrl.h"
#include "systemctrl_se.h"
#include "vshctrl.h"
#include "utils.h"
#include "vpl.h"
#include "main.h"

static char g_bottom_info[MAX_SCREEN_X+1];
static int g_bottom_info_color;

static void set_screen_xy(int x, int y)
{
	pspDebugScreenSetXY(x, y);
}

static void write_string_with_color(const char *str, int color)
{
	if(color != 0) {
		pspDebugScreenSetTextColor(color);
	}
	
	printf(str);
	pspDebugScreenSetTextColor(0xFFFFFFFF);
}

static void write_bottom_info(void)
{
	set_screen_xy(2, MAX_SCREEN_Y-3);
	write_string_with_color(g_bottom_info, g_bottom_info_color);
}

static void draw_bottom_line(void)
{
	int i;

	set_screen_xy(0, MAX_SCREEN_Y-5);

	for(i=0; i<MAX_SCREEN_X; ++i) {
		write_string_with_color("*", 0xFF);
	}
}

static void menu_draw(struct Menu *menu)
{
	int x, y, i;

	x = 1, y = 1;
	set_screen_xy(x, y);
	write_string_with_color("PRO Recovery Menu", 0xFF);
	x = 1, y = 2;
	set_screen_xy(x, y);
	write_string_with_color(menu->banner, menu->banner_color);

	x = 3, y = 4;
	set_screen_xy(x, y);

	for(i=0; i<menu->submenu_size; ++i) {
		char buf[256], *p;
		int color;

		if(menu->cur_sel == i) {
			color = CUR_SEL_COLOR;
			strcpy(buf, "* ");
		}
		else {
			color = menu->submenu[i].color;
			strcpy(buf, "  ");
		}

		p = buf + strlen(buf);

		if(menu->submenu[i].info != NULL) {
			sprintf(p, "%s", menu->submenu[i].info);
		} else {
			int (*display_callback)(char *, int);
			
			display_callback = (*menu->submenu[i].display_callback);
			if (display_callback != NULL) {
				(*display_callback)(p, sizeof(buf) - (p - buf));
			} else {
				strcpy(p, "FIXME");
			}
		}

		if(menu->submenu[i].type == TYPE_SUBMENU) {
			strcat(p, " ->");
		}

		write_string_with_color(buf, color);
		set_screen_xy(x, ++y);
	}

	write_bottom_info();
	draw_bottom_line();
}

static void get_sel_index(struct Menu *menu, int direct)
{
	menu->cur_sel = limit_int(menu->cur_sel, direct, menu->submenu_size);
}

static void menu_change_value(struct Menu *menu, int direct)
{
	struct MenuEntry *entry = &menu->submenu[menu->cur_sel];

	if(entry->change_value_callback == NULL)
		return;

	(*entry->change_value_callback)(entry, direct);
}

// exit menu when returns 1
static int menu_ctrl(struct Menu *menu)
{
	u32 key;

	key = ctrl_read();

	if(key & PSP_CTRL_UP) {
		get_sel_index(menu, -1);
	} else if(key & PSP_CTRL_DOWN) {
		get_sel_index(menu, +1);
	} else if(key & PSP_CTRL_RIGHT) {
		menu_change_value(menu, 1);
	} else if(key & PSP_CTRL_LEFT) {
		menu_change_value(menu, -1);
	} else if(key & g_ctrl_OK) {
		struct MenuEntry *entry;
		int (*enter_callback)(struct MenuEntry *);

		entry = &menu->submenu[menu->cur_sel];
		enter_callback = entry->enter_callback;

		if(entry->type == TYPE_SUBMENU) {
			set_bottom_info("> Entering...", 0xFF);
			frame_end();
			sceKernelDelayThread(ENTER_DELAY);
			set_bottom_info("", 0);
		}
		
		if(enter_callback != NULL) {
			(*enter_callback)(entry);
		}
	} else if(key & g_ctrl_CANCEL) {
		set_bottom_info("> Exiting...", 0xFF);
		frame_end();
		sceKernelDelayThread(EXIT_DELAY);
		set_bottom_info("", 0);

		return 1;
	} else if(key & PSP_CTRL_SELECT) {
		recovery_exit();
		
		return 1;
	}

	return 0;
}

int limit_int(int value, int direct, int limit)
{
	if(limit == 0)
		return 0;

	value += direct;

	if(value >= limit) {
		value = value % limit;
	} else if(value < 0) {
		value = limit - ((-value) % limit);
	}

	return value;
}

void set_bottom_info(const char *str, int color)
{
	strcpy(g_bottom_info, str);
	g_bottom_info_color = color;
	write_bottom_info();
}

void frame_start(void)
{
	scePowerTick(0);
	sceDisplayWaitVblank();
	memset(get_drawing_buffer(), 0, 512*272*4);
}

void frame_end(void)
{
	g_display_flip = !g_display_flip;
	sceDisplaySetFrameBuf(get_display_buffer(), 512, PSP_DISPLAY_PIXEL_FORMAT_8888, PSP_DISPLAY_SETBUF_NEXTFRAME);
	pspDebugScreenSetOffset((int)get_drawing_buffer() - 0x44000000);
	sceDisplayWaitVblank();
}

void menu_loop(struct Menu *menu)
{
	int ret;

	while (1) {
		frame_start();
		menu_draw(menu);
		ret = menu_ctrl(menu);

		if(ret != 0)
			break;

		frame_end();
	}
}
