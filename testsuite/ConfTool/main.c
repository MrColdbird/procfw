#include <pspkernel.h>
#include <pspgu.h>
#include <pspsdk.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <pspdebug.h>
#include <pspiofilemgr.h>
#include <psputility.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "systemctrl.h"
#include "systemctrl_se.h"
#include "utils.h"
#include "printk.h"
#include "strsafe.h"
#include "kubridge.h"

PSP_MODULE_INFO("PROConfTool", 0, 0, 0);
PSP_HEAP_SIZE_KB(0);

#define printf pspDebugScreenPrintf

#define CTRL_REPEAT_TIME 0x40000

#define BUF_WIDTH (512)
#define SCR_WIDTH (480)
#define SCR_HEIGHT (272)
#define PIXEL_SIZE (4) /* change this if you change to another screenmode */
#define FRAME_SIZE (BUF_WIDTH * SCR_HEIGHT * PIXEL_SIZE)
#define ZBUF_SIZE (BUF_WIDTH SCR_HEIGHT * 2) /* zbuffer seems to be 16-bit? */
#define CHAR_ROWS 66
#define CHAR_COLS 33

static u32 g_last_btn = 0;
static u32 g_last_tick = 0;

static unsigned int __attribute__((aligned(16))) g_disp_list[1024];

static void *buffer = 0;
static int g_cur_sel = 0;
static int g_x = 0, g_y = 0;

enum {
	INPUT_NONE,
	INPUT_EXIT,
	INPUT_UP,
	INPUT_DOWN,
	INPUT_FLIP_VALUE,
};

int g_ctrl_OK;
int g_ctrl_CANCEL;

SEConfig g_conf;

const char *g_menu_strs[] = {
	"ISO Mode",
	"USB Charge",
	"Hide MAC Address",
	"Use Version.txt",
};

const char *get_iso_mode_name(int mode)
{
	if (mode == MODE_NP9660) {
		return "NP9660";
	}

	if (mode == MODE_MARCH33) {
		return "M33";
	}

	return "Unknown";
}

const char *get_boolean_name(int boolean)
{
	if (boolean == 1) {
		return "ON";
	}

	if (boolean == 0) {
		return "OFF";
	}

	return "Unknown";
}

const char *is_selected(int i)
{
	if(i == g_cur_sel)
		return "*";

	return " ";
}

void draw_main_menu(void)
{
	int i;

	i=0;

	g_x = g_y = 0;
	pspDebugScreenSetXY(g_x, g_y); 
	printf("635PRO Configure Tool\n\n");

	printf("%s%-20s : %s\n", is_selected(i), g_menu_strs[i], get_iso_mode_name(g_conf.umdmode));
	i++;
	printf("%s%-20s : %s\n", is_selected(i), g_menu_strs[i], get_boolean_name(g_conf.usbcharge));
	i++;
	printf("%s%-20s : %s\n", is_selected(i), g_menu_strs[i], get_boolean_name(g_conf.machidden));
	i++;
	printf("%s%-20s : %s\n", is_selected(i), g_menu_strs[i], get_boolean_name(g_conf.useversion));
	i++;

	if(i != NELEMS(g_menu_strs)) {
		printf("%s: Are you sure you didn't miss anything?\n", __func__);
	}
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

int read_input(void)
{
	u32 key;

	key = ctrl_read();

	if (key == g_ctrl_CANCEL) {
		return INPUT_EXIT;
	}

	if (key == g_ctrl_OK) {
		return INPUT_FLIP_VALUE;
	}

	if (key == PSP_CTRL_UP) {
		return INPUT_UP;
	}

	if (key == PSP_CTRL_DOWN) {
		return INPUT_DOWN;
	}

	return INPUT_NONE;
}

void get_confirm_button(void)
{
	int result;

	sceUtilityGetSystemParamInt(9, &result);

	if (result == 0) { // Circle?
		g_ctrl_OK = PSP_CTRL_CIRCLE;
		g_ctrl_CANCEL = PSP_CTRL_CROSS;
	} else {
		g_ctrl_OK = PSP_CTRL_CROSS;
		g_ctrl_CANCEL = PSP_CTRL_CIRCLE;
	}
}

int main(int argc, char* argv[])
{
	int status, ret;

	printk_init("ms0:/log_conftool.txt");

	memset(&g_conf, 0, sizeof(g_conf));
	ret = sctrlSEGetConfig(&g_conf);
	
	if (ret < 0) {
		printf("sctrlSEGetConfig -> 0x%08X\n", ret);
	}

	get_confirm_button();
	sceGuInit();
	sceGuStart(GU_DIRECT,g_disp_list);
	sceGuDrawBuffer(GU_PSM_8888,(void*)0,BUF_WIDTH);
	sceGuDispBuffer(SCR_WIDTH,SCR_HEIGHT,(void*)(512*272*4),BUF_WIDTH);
	sceGuScissor(0,0,SCR_WIDTH,SCR_HEIGHT);
	sceGuEnable(GU_SCISSOR_TEST);
	sceGuFinish();
	sceGuSync(0,0);
	sceDisplayWaitVblankStart();
	sceGuDisplay(GU_TRUE);
	
	pspDebugScreenInit();
	pspDebugScreenSetOffset((int)buffer);
	
	while (1) {
		status = read_input();
		sceGuStart(GU_DIRECT,g_disp_list);
		sceGuClearColor(0);
		sceGuClear(GU_COLOR_BUFFER_BIT);
		sceGuFinish();
		sceGuSync(0,0);

		if (status == INPUT_EXIT) {
			g_x = g_y = 0;
			pspDebugScreenSetXY(g_x, g_y);
			printf("Exiting...");

			break;
		} else if (status == INPUT_UP) {
			if (g_cur_sel > 0)
				g_cur_sel--;
			else
				g_cur_sel = NELEMS(g_menu_strs) - 1;
		} else if (status == INPUT_DOWN) {
			if (NELEMS(g_menu_strs) == 0)
				g_cur_sel = 0;
			else if (g_cur_sel < NELEMS(g_menu_strs) - 1)
				g_cur_sel++;
			else
				g_cur_sel = 0;
		} else if (status == INPUT_FLIP_VALUE) {
			switch(g_cur_sel) {
				case 0:
					{
						if(g_conf.umdmode == MODE_NP9660)
							g_conf.umdmode = MODE_MARCH33;
						else
							g_conf.umdmode = MODE_NP9660;

						break;
					}
				case 1:
					g_conf.usbcharge = !g_conf.usbcharge;
					break;
				case 2:
					g_conf.machidden = !g_conf.machidden;
					break;
				case 3:
					g_conf.useversion = !g_conf.useversion;
					break;
				default:
					break;
			}

			ret = sctrlSESetConfig(&g_conf);

			if (ret < 0) {
				printk("sctrlSESetConfig -> 0x%08X\n", ret);
			}
		}

		sceKernelDelayThread(100000);
		draw_main_menu();

		sceDisplayWaitVblank();
		buffer = sceGuSwapBuffers();
		pspDebugScreenSetOffset((int)buffer);
	}

	sceDisplayWaitVblank();
	buffer = sceGuSwapBuffers();
	pspDebugScreenSetOffset((int)buffer);

	ret = sctrlSESetConfig(&g_conf);

	if (ret < 0) {
		printf("sctrlSESetConfig -> 0x%08X\n", ret);
	}
	
	sceKernelDelayThread(500000);
	sceKernelExitGame();

	return 0;
}
