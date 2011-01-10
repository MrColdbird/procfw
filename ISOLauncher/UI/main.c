#include <pspkernel.h>
#include <pspusb.h>
#include <pspgu.h>
#include <pspsdk.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <pspdebug.h>
#include <pspiofilemgr.h>
#include <psppower.h>
#include <psputility.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "printk.h"
#include "strsafe.h"
#include "loader_conf.h"
#include "kubridge.h"
#include "conf.h"

PSP_MODULE_INFO("ProISOLoader", 0, 0, 0);
PSP_HEAP_SIZE_KB(0);

#define VERSION_STR "0.0"

#define printf pspDebugScreenPrintf
#define NELEMS(a) (sizeof(a) / sizeof(a[0]))

#define CTRL_REPEAT_TIME 0x40000

#define BUF_WIDTH (512)
#define SCR_WIDTH (480)
#define SCR_HEIGHT (272)
#define PIXEL_SIZE (4) /* change this if you change to another screenmode */
#define FRAME_SIZE (BUF_WIDTH * SCR_HEIGHT * PIXEL_SIZE)
#define ZBUF_SIZE (BUF_WIDTH SCR_HEIGHT * 2) /* zbuffer seems to be 16-bit? */
#define CHAR_ROWS 66
#define CHAR_COLS 33

int psplinkPresent(void);

static u32 g_last_btn = 0;
static u32 g_last_tick = 0;

char g_iso_fns[128][128];
u32 g_iso_fns_cnt = 0;

static unsigned int __attribute__((aligned(16))) g_disp_list[1024];

static void *buffer = 0;
static int g_cur_sel = 0;
static int g_x = 0, g_y = 0;
static int g_iso_dir_mode = 0;

struct PLoaderConf g_conf;

enum {
	INPUT_NONE,
	INPUT_EXIT,
	INPUT_UP,
	INPUT_DOWN,
	INPUT_SWITCH_ISO_MODE,
	INPUT_LAUNCH,
	INPUT_SWITCH_DIR,
	INPUT_REFRESH,
};

int g_ctrl_OK;
int g_ctrl_CANCEL;

int is_iso(char *path)
{
	char *p;
	
	p = strrchr(path, '.');

	if (!p)
		return 0;

	if (!stricmp(p+1, "iso") || !stricmp(p+1, "cso"))
		return 1;

	return 0;
}

const char *get_iso_dir(void)
{
	if (g_iso_dir_mode) {
		return "host0:/ISO";
	}

	return "ms0:/ISO";
}

int get_iso(void)
{
	SceIoDirent dirent;
	int fd;

	g_iso_fns_cnt = 0;
	g_cur_sel = 0;
	memset(&dirent, 0, sizeof(dirent));

	fd = sceIoDopen(get_iso_dir());

	if (fd < 0) {
		printk("sceIoDopen returns 0x%08X\r\n", fd);

		return fd;
	}

	while(sceIoDread(fd, &dirent) > 0) {
		if (is_iso(dirent.d_name)) {
			if (g_iso_fns_cnt < NELEMS(g_iso_fns)) {
				STRCPY_S(g_iso_fns[g_iso_fns_cnt++], dirent.d_name);
			} else {
				break;
			}
		}
	}

	sceIoDclose(fd);

	return 0;
}

int draw_iso_menu(void)
{
	int i;

	for(i=0; i<g_iso_fns_cnt; ++i) {
		if (g_cur_sel == i) {
			pspDebugScreenSetTextColor(0xff0000ff);
		}

		pspDebugScreenSetXY(g_x, ++g_y);
		printf("%d: %s", i+1, g_iso_fns[i]);

		if (g_cur_sel == i) {
			pspDebugScreenSetTextColor(0xffffffff);
		}
	}

	return 0;
}

void draw_main_menu(void)
{
	g_x = g_y = 0;
	pspDebugScreenSetXY(g_x, g_y);
	printf("PRO ISO Loader %s by Coldbird&VF", VERSION_STR);
	g_y+=2;
	pspDebugScreenSetXY(g_x, g_y);
	printf("Press Select to change ISO mode.");
	pspDebugScreenSetXY(g_x, ++g_y);
	printf("Press Start to change ISO directory.");
	pspDebugScreenSetXY(g_x, ++g_y);
	printf("Press Square to refresh.");
	pspDebugScreenSetXY(g_x, ++g_y);

	if (g_ctrl_OK == PSP_CTRL_CIRCLE) {
		printf("Press O to launch ISO. Press X to exit");
	} else {
		printf("Press X to launch ISO. Press O to exit");
	}

	g_x = CHAR_ROWS / 2 - ((sizeof("Select Your ISO:") - 1) / 2);
	g_y = CHAR_COLS / 2 - 1 - (g_iso_fns_cnt / 2 + 2);
	pspDebugScreenSetXY(g_x, g_y);
	printf("Select Your ISO:");
	g_y++;
	pspDebugScreenSetXY(g_x, ++g_y);
	draw_iso_menu();
	g_y+=2;

	g_x = CHAR_ROWS - (sizeof("Current ISO MODE: M33 Driver") - 1);
	g_y = CHAR_COLS;
	pspDebugScreenSetXY(g_x, g_y);
	printf("Current ISO MODE: %s", g_conf.iso_mode == USE_NP9660_MODE ? "NP9660" : "M33 Driver");
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
		return INPUT_LAUNCH;
	}

	if (key == PSP_CTRL_UP) {
		return INPUT_UP;
	}

	if (key == PSP_CTRL_DOWN) {
		return INPUT_DOWN;
	}

	if (key == PSP_CTRL_SELECT) {
		return INPUT_SWITCH_ISO_MODE;
	}

	if (key == PSP_CTRL_START) {
		return INPUT_SWITCH_DIR;
	}

	if (key == PSP_CTRL_SQUARE) {
		return INPUT_REFRESH;
	}

	return INPUT_NONE;
}

static int load_start_launcher(struct PLoaderConf *loader)
{
	int result, ret;
	char path[128];

	STRCPY_S(path, loader->prx_path);
	STRCAT_S(path, "launcher.prx");

	ret = sceKernelLoadModule(path, 0, NULL);

	if (ret < 0) {
		printk("sceKernelLoadModule %s returns 0x%08x\r\n", path, ret);

		return ret;
	}

	ret = sceKernelStartModule(ret, sizeof(*loader), loader, &result, NULL);

	if (ret < 0) {
		printk("sceKernelStartModule %s returns 0x%08x\r\n", path, ret);
	}

	return ret;
}

int main(int argc, char* argv[])
{
	int status;

	memset(&g_conf, 0, sizeof(g_conf));
	g_conf.fw_version = sceKernelDevkitVersion();
	g_conf.psp_model = kuKernelGetModel();

	printk_init();
	printk("PLoad GUI started, fw: 0x%08X model: 0x%08X\r\n", g_conf.fw_version, g_conf.psp_model);

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
	
	getcwd(g_conf.prx_path, sizeof(g_conf.prx_path));
	STRCAT_S(g_conf.prx_path, "/");
	printk("g_conf.prx_path %s\r\n", g_conf.prx_path);

	get_iso();
	load_conf();
	
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
		} else if (status == INPUT_LAUNCH && g_iso_fns_cnt != 0) {
			g_x = g_y = 0;
			pspDebugScreenSetXY(g_x, g_y);
			printf("Launching ISO... %s/%s", get_iso_dir(), g_iso_fns[g_cur_sel]);

			break;
		} else if (status == INPUT_UP) {
			if (g_cur_sel > 0)
				g_cur_sel--;
			else if (g_iso_fns_cnt == 0)
				g_cur_sel = 0;
			else
				g_cur_sel = g_iso_fns_cnt - 1;
		} else if (status == INPUT_DOWN) {
			if (g_iso_fns_cnt == 0)
				g_cur_sel = 0;
			else if (g_cur_sel < g_iso_fns_cnt - 1)
				g_cur_sel++;
			else
				g_cur_sel = 0;
		} else if (status == INPUT_SWITCH_ISO_MODE) {
			if (g_conf.iso_mode == 2)
				g_conf.iso_mode = 1;
			else
				g_conf.iso_mode = 2;
		} else if (status == INPUT_REFRESH) {
			get_iso();
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

	if (status == INPUT_LAUNCH) {
		int result;

		save_conf();
		STRCPY_S(g_conf.iso_path, get_iso_dir());
		STRCAT_S(g_conf.iso_path, "/");
		STRCAT_S(g_conf.iso_path, g_iso_fns[g_cur_sel]);

		// launch ISO
		load_start_launcher(&g_conf);

		sceDisplaySetMode(0, 480, 272);
		sceDisplaySetFrameBuf((void*)0x44000000, 512, PSP_DISPLAY_PIXEL_FORMAT_8888, 1);
		
		sceGuDisplay(GU_FALSE);
		sceGuTerm();
		memset((void*)0x44000000, 0, 0x200000);

		sceKernelStopUnloadSelfModule(0, NULL, &result, NULL);
	}

	save_conf();
	sceKernelDelayThread(500000);
	sceKernelExitGame();

	return 0;
}
