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

PSP_MODULE_INFO("Recovery", 0, 1, 2);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);
PSP_HEAP_SIZE_KB(0);

#define CTRL_REPEAT_TIME 0x40000
#define CUR_SEL_COLOR 0xFF
#define MAX_SCREEN_X 68
#define MAX_SCREEN_Y 33
#define BUF_WIDTH (512)
#define SCR_WIDTH (480)
#define SCR_HEIGHT (272)
#define PIXEL_SIZE (4)
#define FRAME_SIZE (BUF_WIDTH * SCR_HEIGHT * PIXEL_SIZE)

#define EXIT_DELAY   500000
#define CHANGE_DELAY 500000
#define DRAW_BUF (void*)(0x44000000)
#define DISPLAY_BUF (void*)(0x44000000 + FRAME_SIZE)

#define printf pspDebugScreenPrintf

static int g_ctrl_OK;
static int g_ctrl_CANCEL;
static u32 g_last_btn = 0;
static u32 g_last_tick = 0;

static char g_bottom_info[MAX_SCREEN_X+1];
static int g_bottom_info_color;

enum {
	TYPE_NORMAL = 0,
	TYPE_SUBMENU = 1,
};

struct MenuEntry {
	char *info;
	int type;
	int color;
	int (*display_callback)(char *, int);
	int (*change_value_callback)(void*, int);
	int (*enter_callback)(void*);
	void *arg;
};

struct Menu {
	char *banner;
	struct MenuEntry *submenu;
	int submenu_size;
	int cur_sel;
	int color;
};

static int sub_menu(void *);
static int sub_menu2(void *);
static int sub_menu3(void *);

void set_bottom_info(const char *str, int color);
void frame_end(void);

int menu_change_bool(void *arg)
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

struct ValueOption {
	int value;
	int limit;
};

static struct ValueOption g_test_option1 = {
	0,
	2,
};

static struct ValueOption g_test_option2 = { 
	0,
	4,
};

int display_test_option1(char *buf, int size)
{
	return sprintf(buf, "3. bool option = %d", g_test_option1.value);
}

int display_test_option2(char *buf, int size)
{
	return sprintf(buf, "4. value option = %d", g_test_option2.value);
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

int menu_change_int(void *arg)
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

int change_value_option(void *arg, int direct)
{
	struct ValueOption *c = arg;

	c->value = limit_int(c->value, direct, c->limit);

	return 0;
}

struct MenuEntry g_top_menu_entries[] = {
	{ "1. Empty option", 0, },
	{ "2. Submenu option", 1, 0xFF00, NULL, NULL, &sub_menu, NULL},
	{ NULL, 0, 0, &display_test_option1, &change_value_option, &menu_change_bool, &g_test_option1 },
	{ NULL, 0, 0, &display_test_option2, &change_value_option, &menu_change_int, &g_test_option2 },
};

struct Menu g_top_menu = {
	"PRO Recovery Menu",
	g_top_menu_entries,
	NELEMS(g_top_menu_entries),
	0,
	0xFF,
};

struct MenuEntry g_sub_menu_entries[] = {
	{ "1. Normal option", 0, },
	{ "2. Green  option", 0, 0xFF00, },
	{ "3. Submenu option", 1, 0xFF00, NULL, NULL, &sub_menu2, NULL},
	{ "3. Dynamic Submenu option", 1, 0xFF0000, NULL, NULL, &sub_menu3, NULL},
};

struct Menu g_sub_menu = {
	"PRO Sub Menu",
	g_sub_menu_entries,
	NELEMS(g_sub_menu_entries),
	0,
	0xFF,
};

struct MenuEntry g_sub_menu_2_entries[] = {
	{ "1. Normal option", 0, },
};

struct Menu g_sub_menu_2 = {
	"PRO Sub Menu 2",
	g_sub_menu_2_entries,
	NELEMS(g_sub_menu_2_entries),
	0,
	0xFF00,
};

struct Menu g_sub_menu_3 = {
	"PRO Sub Menu Dynamic",
	NULL,
	0,
	0,
	0xFF00,
};

void set_screen_xy(int x, int y)
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

void write_info_bottom(void)
{
	set_screen_xy(2, MAX_SCREEN_Y-3);
	write_string_with_color(g_bottom_info, g_bottom_info_color);
}

void set_bottom_info(const char *str, int color)
{
	strcpy(g_bottom_info, str);
	g_bottom_info_color = color;
	write_info_bottom();
}

static void draw_button(void)
{
	int i;

	set_screen_xy(0, MAX_SCREEN_Y-5);

	for(i=0; i<MAX_SCREEN_X; ++i) {
		write_string_with_color("*", 0xFF);
	}
}

void draw_menu(struct Menu *menu)
{
	int x, y, i;

	x = 1, y = 1;
	set_screen_xy(x, y);
	write_string_with_color(menu->banner, menu->color);

	x = 3, y = 5;
	set_screen_xy(x, y);

	for(i=0; i<menu->submenu_size; ++i) {
		char buf[256];
		int color;

		if(menu->submenu[i].info != NULL) {
			sprintf(buf, "%s", menu->submenu[i].info);
		} else {
			int (*display_callback)(char *, int);
			
			display_callback = (*menu->submenu[i].display_callback);
			if (display_callback != NULL) {
				(*display_callback)(buf, sizeof(buf));
			} else {
				strcpy(buf, "FIXME");
			}
		}

		if(menu->submenu[i].type == TYPE_SUBMENU) {
			strcat(buf, " ->");
		}

		if(menu->cur_sel == i) {
			color = CUR_SEL_COLOR;
		}
		else {
			color = menu->submenu[i].color;
		}

		write_string_with_color(buf, color);
		set_screen_xy(x, ++y);
	}

	write_info_bottom();
	draw_button();
}

u32 ctrl_read(void)
{
	SceCtrlData ctl;

	sceCtrlReadBufferPositive(&ctl, 1);

	if (ctl.Buttons == g_last_btn) {
		if (ctl.TimeStamp - g_last_tick < CTRL_REPEAT_TIME)
			return 0;

		return g_last_btn;
	}

	g_last_btn = ctl.Buttons;
	g_last_tick = ctl.TimeStamp;

	return g_last_btn;
}

void get_confirm_button(void)
{
	int result = 0;

	sceUtilityGetSystemParamInt(9, &result);

	if (result == 0) { // Circle?
		g_ctrl_OK = PSP_CTRL_CIRCLE;
		g_ctrl_CANCEL = PSP_CTRL_CROSS;
	} else {
		g_ctrl_OK = PSP_CTRL_CROSS;
		g_ctrl_CANCEL = PSP_CTRL_CIRCLE;
	}
}

static void get_sel_index(struct Menu *menu, int direct)
{
	menu->cur_sel = limit_int(menu->cur_sel, direct, menu->submenu_size);
}

static int g_display_flip = 0;

void *get_drawing_buffer(void)
{
	void *buffer;

	if(g_display_flip) {
		buffer = DRAW_BUF;
	} else {
		buffer = DISPLAY_BUF;
	}

	return buffer;
}

void *get_display_buffer(void)
{
	void *buffer;

	if(g_display_flip) {
		buffer = DISPLAY_BUF;
	} else {
		buffer = DRAW_BUF;
	}

	return buffer;
}

void frame_start(void)
{
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

void menu_change_value(struct Menu *menu, int direct)
{
	struct MenuEntry *entry = &menu->submenu[menu->cur_sel];

	if(entry->change_value_callback == NULL)
		return;

	(*entry->change_value_callback)(entry->arg, direct);
}

void menu_loop(struct Menu *menu)
{
	u32 key;

	while (1) {
		frame_start();
		draw_menu(menu);
		key = ctrl_read();

		if(key & PSP_CTRL_UP) {
			get_sel_index(menu, -1);
		}

		if(key & PSP_CTRL_DOWN) {
			get_sel_index(menu, +1);
		}

		if(key & PSP_CTRL_RIGHT) {
			menu_change_value(menu, 1);
		}

		if(key & PSP_CTRL_LEFT) {
			menu_change_value(menu, -1);
		}

		if(key & g_ctrl_OK) {
			if(menu->submenu[menu->cur_sel].enter_callback != NULL) {
				int ret;
				void *arg;

				arg = menu->submenu[menu->cur_sel].arg;
				ret = (*menu->submenu[menu->cur_sel].enter_callback)(arg);
			}
		}

		if(key & g_ctrl_CANCEL) {
			set_bottom_info("Exiting...", 0xFF);
			frame_end();
			frame_start();
			sceKernelDelayThread(EXIT_DELAY);
			frame_end();
			set_bottom_info("", 0);
			break;
		}

		frame_end();
	}
}

void main_menu(void)
{
	struct Menu *menu = &g_top_menu;
	
	menu->cur_sel = 0;
	menu_loop(menu);
}

int sub_menu(void * arg)
{
	struct Menu *menu = &g_sub_menu;

	menu->cur_sel = 0;
	menu_loop(menu);

	return 0;
}

int sub_menu2(void * arg)
{
	struct Menu *menu = &g_sub_menu_2;

	menu->cur_sel = 0;
	menu_loop(menu);

	return 0;
}

static SceUID g_vpl_uid = -1;

void vpl_init(void)
{
	g_vpl_uid = sceKernelCreateVpl("OurVPL", 2, 0, 512*1024, NULL);
}

void vpl_finish(void)
{
	sceKernelDeleteVpl(g_vpl_uid);
}

void *vpl_alloc(int size)
{
	void *p;
	int ret;

	ret = sceKernelAllocateVpl(g_vpl_uid, size, &p, NULL);

	if(ret == 0)
		return p;

	return NULL;
}

void *vpl_strdup(const char *str)
{
	int len;
	void *p;

	len = strlen(str) + 1;
	p = vpl_alloc(len);

	if(p == NULL)
		return p;

	strcpy(p, str);

	return p;
}

void vpl_free(void *p)
{
	sceKernelFreeVpl(g_vpl_uid, p);
}

int sub_menu3(void * arg)
{
	struct Menu *menu = &g_sub_menu_3;

	srand(time(0));
	g_sub_menu_3.submenu_size = rand() % 20 + 1;
	g_sub_menu_3.submenu = vpl_alloc(g_sub_menu_3.submenu_size * sizeof(*g_sub_menu_3.submenu));
	memset(g_sub_menu_3.submenu, 0, g_sub_menu_3.submenu_size * sizeof(*g_sub_menu_3.submenu));

	int i; for(i=0; i<g_sub_menu_3.submenu_size; ++i) {
		char buf[20];

		sprintf(buf, "%d. entry", i);
		g_sub_menu_3.submenu[i].info = vpl_strdup(buf);
	}

	menu->cur_sel = 0;
	menu_loop(menu);

	for(i=0; i<g_sub_menu_3.submenu_size; ++i) {
		vpl_free(g_sub_menu_3.submenu[i].info);
	}

	return 0;
}

SceUID get_thread_id(const char *name)
{
	int ret, count, i;
	SceUID ids[128];

	ret = sceKernelGetThreadmanIdList(SCE_KERNEL_TMID_Thread, ids, sizeof(ids), &count);

	if(ret < 0) {
		return -1;
	}

	for(i=0; i<count; ++i) {
		SceKernelThreadInfo info;

		info.size = sizeof(info);
		ret = sceKernelReferThreadStatus(ids[i], &info);

		if(ret < 0) {
			continue;
		}

		if(0 == strcmp(info.name, name)) {
			return ids[i];
		}
	}

	return -2;
}

void suspend_thread(const char *thread_name)
{
	int ret;

	ret = get_thread_id(thread_name);

	if(ret < 0)
		return;

	sceKernelSuspendThread(ret);
}

void resume_thread(const char *thread_name)
{
	int ret;

	ret = get_thread_id(thread_name);

	if(ret < 0)
		return;

	sceKernelResumeThread(ret);
}

void suspend_vsh_thread(void)
{
	suspend_thread("SCE_VSH_GRAPHICS");
}

void resume_vsh_thread(void)
{
	resume_thread("SCE_VSH_GRAPHICS");
}

int main_thread(SceSize size, void *argp)
{
	vpl_init();
	suspend_vsh_thread();
	pspDebugScreenInit();
	sceDisplaySetFrameBuf(get_display_buffer(), 512, PSP_DISPLAY_PIXEL_FORMAT_8888, PSP_DISPLAY_SETBUF_NEXTFRAME);

	get_confirm_button();
	main_menu();

	sceDisplaySetFrameBuf((void*)0x44000000, 512, PSP_DISPLAY_PIXEL_FORMAT_8888, PSP_DISPLAY_SETBUF_NEXTFRAME);
	resume_vsh_thread();
	vpl_finish();

	sceKernelStopUnloadSelfModule(0, NULL, NULL, NULL);

	return 0;
}

static SceUID thread_id = -1;

int module_start(int argc, char *argv[])
{
	int	thid;

	thid = sceKernelCreateThread("recovery_thread", main_thread, 16, 0x8000 ,0 ,0);

	thread_id=thid;

	if (thid>=0) {
		sceKernelStartThread(thid, 0, 0);
	}
	
	return 0;
}

int module_stop(int argc, char *argv[])
{
	return 0;
}
