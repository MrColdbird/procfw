#ifndef MAIN_H
#define MAIN_H

enum {
	TYPE_NORMAL = 0,
	TYPE_SUBMENU = 1,
};

struct MenuEntry {
	char *info;
	int type;
	int color;
	int (*display_callback)(char *, int);
	int (*change_value_callback)(struct MenuEntry *, int);
	int (*enter_callback)(struct MenuEntry *);
	void *arg;
};

struct ValueOption {
	int *value;
	int limit;
};

struct Menu {
	char *banner;
	struct MenuEntry *submenu;
	int submenu_size;
	int cur_sel;
	int banner_color;
};

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

extern int g_ctrl_OK;
extern int g_ctrl_CANCEL;
extern int g_display_flip;
extern SEConfig g_config;

u32 ctrl_read(void);
void *get_drawing_buffer(void);
void *get_display_buffer(void);

int limit_int(int value, int direct, int limit);
void set_bottom_info(const char *str, int color);
void frame_start(void);
void frame_end(void);
void menu_loop(struct Menu *menu);

void main_menu(void);

const char *get_bool_name(int boolean);
const char* get_fake_region_name(int fakeregion);

#endif
