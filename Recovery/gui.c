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
#include "prodebug.h"
#include "main.h"

static char g_bottom_info[MAX_SCREEN_X+1];
static int g_bottom_info_color;
static int g_frame_count = 0;

static void set_screen_xy(int x, int y)
{
	proDebugScreenSetXY(x, y);
}

static void write_string_with_color(const char *str, int color)
{
	if(color != 0) {
		proDebugScreenSetTextColor(color);
	}
	
	printf(str);
	proDebugScreenSetTextColor(0xFFFFFFFF);
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

static void set_line_backcolor(int x, int y, int color)
{
	int i;

	proDebugScreenSetBackColor(color);
	proDebugScreenSetXY(0, y);
	proDebugScreenEnableBackColor(1);

	for(i=0; i<MAX_SCREEN_X; ++i) {
		proDebugScreenPrintf(" ");
	}

	proDebugScreenEnableBackColor(0);
	proDebugScreenSetXY(x, y);
}

static int get_back_color(int frame_count)
{
	int color = 0;
	int speed = ((MENU_MAX_BACK_COLOR - MENU_MIN_BACK_COLOR) / 60 / MENU_BACK_COLOR_HALFTIME);
	int half_time_frame = 60 * MENU_BACK_COLOR_HALFTIME;

	if(frame_count < half_time_frame) {
		color = MENU_MIN_BACK_COLOR + frame_count * speed;
	} else {
		color = MENU_MAX_BACK_COLOR - (frame_count - half_time_frame) * speed;
	}

	color = (color) | (color << 8) | (color << 16);

	return color;
}

static void menu_draw(struct Menu *menu)
{
	int x, y, i, cur_page_start, total_page;

	x = 1, y = 1;
	set_screen_xy(x, y);
	write_string_with_color(g_messages[PRO_RECOVERY_MENU], 0xFF);
	x = 1, y = 2;
	set_screen_xy(x, y);
	write_string_with_color(g_messages[menu->banner_id], menu->banner_color);

	x = (cur_language == PSP_SYSTEMPARAM_LANGUAGE_FRENCH ? 0 : 3), y = 4;
	set_screen_xy(x, y);

	{
		char buf[10];
		int color;

		if(menu->cur_sel == 0) {
			strcpy(buf, "> ");
			color = CUR_SEL_COLOR;
		} else {
			strcpy(buf, "  ");
			color = 0;
		}

		strcat(buf, g_messages[BACK]);
		write_string_with_color(buf, color);
	}

	x = (cur_language == PSP_SYSTEMPARAM_LANGUAGE_FRENCH ? 0 : 3), y = 6;
	set_screen_xy(x, y);

	if(menu->cur_sel == 0) {
		cur_page_start = 0;
	} else {
		cur_page_start = (menu->cur_sel-1) / MAX_MENU_NUMBER_PER_PAGE * MAX_MENU_NUMBER_PER_PAGE;
	}

	for(i=cur_page_start; i<MIN(menu->submenu_size, cur_page_start+MAX_MENU_NUMBER_PER_PAGE); ++i) {
		char buf[256], *p;
		int color;
		struct MenuEntry* entry;

		entry = &menu->submenu[i];

		if(menu->cur_sel == i+1) {
			set_line_backcolor(x, y, get_back_color(g_frame_count % (60 * 2 * MENU_BACK_COLOR_HALFTIME)));
			color = CUR_SEL_COLOR;
			strcpy(buf, "> ");
		} else {
			color = entry->color;
			strcpy(buf, "  ");
		}

		p = buf + strlen(buf);

		if(entry->info_idx >= 0) {
			sprintf(p, "%s", g_messages[entry->info_idx]);
		} else {
			int (*display_callback)(struct MenuEntry* , char *, int);
			
			display_callback = (*entry->display_callback);
			if (display_callback != NULL) {
				(*display_callback)(entry, p, sizeof(buf) - (p - buf));
			} else {
				strcpy(p, "FIXME");
			}
		}

		if(entry->type == TYPE_SUBMENU) {
			strcat(p, " ->");
		}

		write_string_with_color(buf, color);
		set_screen_xy(x, ++y);
	}

	total_page = (menu->submenu_size+MAX_MENU_NUMBER_PER_PAGE-1) / MAX_MENU_NUMBER_PER_PAGE;

	if(total_page > 1) {
		char buf[20];
		x = MAX_SCREEN_X - 15;
		y = 4;

		sprintf(buf, "%s %d/%d", g_messages[PAGE], (cur_page_start/MAX_MENU_NUMBER_PER_PAGE)+1, total_page);
		set_screen_xy(x, y);
		write_string_with_color(buf, 0);
	}

	write_bottom_info();
	draw_bottom_line();
}

static void get_sel_index(struct Menu *menu, int direct)
{
	menu->cur_sel = limit_int(menu->cur_sel, direct, menu->submenu_size+1);
}

static void menu_change_value(struct Menu *menu, int direct)
{
	struct MenuEntry *entry;

	if(menu->cur_sel == 0) {
		return;
	}

	entry = &menu->submenu[menu->cur_sel-1];

	if(entry->change_value_callback == NULL)
		return;

	(*entry->change_value_callback)(entry, direct);
}

// exit menu when returns 1
static int menu_ctrl(struct Menu *menu)
{
	u32 key;
	char buf[80];

	key = ctrl_read();

	if(key & PSP_CTRL_UP) {
		get_sel_index(menu, -1);
	} else if(key & PSP_CTRL_DOWN) {
		get_sel_index(menu, +1);
	} else if(key & PSP_CTRL_RIGHT) {
		menu_change_value(menu, 1);
	} else if(key & PSP_CTRL_LEFT) {
		menu_change_value(menu, -1);
	} else if(key & PSP_CTRL_LTRIGGER) {
		if(menu->cur_sel > MAX_MENU_NUMBER_PER_PAGE) {
			menu->cur_sel -= MAX_MENU_NUMBER_PER_PAGE;
		} else {
			menu->cur_sel = 0;
		}
	} else if(key & PSP_CTRL_RTRIGGER) {
		if(menu->cur_sel + MAX_MENU_NUMBER_PER_PAGE < menu->submenu_size) {
			menu->cur_sel += MAX_MENU_NUMBER_PER_PAGE;
		} else {
			menu->cur_sel = menu->submenu_size;
		}
	} else if(key & g_ctrl_OK) {
		struct MenuEntry *entry;
		int (*enter_callback)(struct MenuEntry *);

		if(menu->cur_sel == 0) {
			goto exit;
		}

		entry = &menu->submenu[menu->cur_sel-1];
		enter_callback = entry->enter_callback;

		if(entry->type == TYPE_SUBMENU) {
			sprintf(buf, "> %s...", g_messages[ENTERING]);
			set_bottom_info(buf, 0xFF);
			frame_end();
			sceKernelDelayThread(ENTER_DELAY);
			set_bottom_info("", 0);
		}
		
		if(enter_callback != NULL) {
			(*enter_callback)(entry);
		}
	} else if(key & g_ctrl_CANCEL) {
exit:
		sprintf(buf, "> %s...", g_messages[EXITING]);
		set_bottom_info(buf, 0xFF);
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
	if(limit == 0 || limit == 1)
		return 0;

	value += direct; //-1

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
	proDebugScreenSetOffset((int)get_drawing_buffer() - 0x44000000);
	memset(get_drawing_buffer(), 0, 512*272*4);
}

void frame_end(void)
{
	g_display_flip = !g_display_flip;
	sceDisplaySetFrameBuf(get_display_buffer(), 512, PSP_DISPLAY_PIXEL_FORMAT_8888, PSP_DISPLAY_SETBUF_NEXTFRAME);
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

		g_frame_count++;
		frame_end();
	}
}
