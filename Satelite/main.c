/*
 * vshMenu by neur0n
 * based booster's vshex
 */
#include <pspkernel.h>
#include <stdio.h>

#include "common.h"
#include "vshctrl.h"

int TSRThread(SceSize args, void *argp);

/* Define the module info section */
PSP_MODULE_INFO("VshCtrlSatelite", 0, 1, 2);

/* Define the main thread's attribute value (optional) */
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

extern int scePowerRequestColdReset(int unk);
extern int scePowerRequestStandby(void);

int menu_mode  = 0;
u32 cur_buttons = 0xFFFFFFFF;
u32 button_on  = 0;
int stop_flag=0;
SceCtrlData ctrl_pad;
int stop_stock=0;
int thread_id=0;

SEConfig cnf;
static SEConfig cnf_old;

int module_start(int argc, char *argv[])
{
	int	thid;

	thid = sceKernelCreateThread("VshMenu_Thread", TSRThread, 16 , 0x1000 ,0 ,0);

	thread_id=thid;

	if (thid>=0) {
		sceKernelStartThread(thid, 0, 0);
	}
	
	return 0;
}

int module_stop(int argc, char *argv[])
{
	SceUInt time = 100*1000;
	int ret;

	stop_flag = 1;
	ret = sceKernelWaitThreadEnd( thread_id , &time );

	if(ret < 0) {
		sceKernelTerminateDeleteThread(thread_id);
	}

	return 0;
}

int EatKey(SceCtrlData *pad_data, int count)
{
	u32 buttons;
	int i;

	// copy true value
	scePaf_6BD7452C_memcpy(&ctrl_pad , pad_data , sizeof(SceCtrlData));

	// buttons check
	buttons     = ctrl_pad.Buttons;
	button_on   = ~cur_buttons & buttons;
	cur_buttons = buttons;

	// mask buttons for LOCK VSH controll
	for(i=0;i < count;i++) {
		pad_data[i].Buttons &= ~(
				PSP_CTRL_SELECT|PSP_CTRL_START|
				PSP_CTRL_UP|PSP_CTRL_RIGHT|PSP_CTRL_DOWN|PSP_CTRL_LEFT|
				PSP_CTRL_LTRIGGER|PSP_CTRL_RTRIGGER|
				PSP_CTRL_TRIANGLE|PSP_CTRL_CIRCLE|PSP_CTRL_CROSS|PSP_CTRL_SQUARE|
				PSP_CTRL_HOME|PSP_CTRL_NOTE);

	}

	return 0;
}

static void button_func(void)
{
	int res;

	// menu controll
	switch(menu_mode) {
		case 0:	
			if( (cur_buttons & ALL_CTRL) == 0) {
				menu_mode = 1;
			}
			break;
		case 1:
			res = menu_ctrl(button_on);

			if(res != 0) {
				stop_stock = res;
				menu_mode = 2;
			}
			break;
		case 2: // exit waiting 
			// exit menu
			if((cur_buttons & ALL_CTRL) == 0) {
				stop_flag = stop_stock;
			}
			break;
	}
}

int TSRThread(SceSize args, void *argp)
{
	sceKernelChangeThreadPriority(0, 8);

	vctrlVSHRegisterVshMenu(EatKey);
	sctrlSEGetConfig(&cnf);

	scePaf_memcpy(&cnf_old , &cnf , sizeof(SEConfig));

	while(stop_flag == 0) {
		if( sceDisplayWaitVblankStart() < 0)
			break; // end of VSH ?

		if(menu_mode > 0) {
			menu_draw();
			menu_setup();
		}

		button_func();
	}

	if(scePaf_memcmp(&cnf_old, &cnf, sizeof(SEConfig)))
		sctrlSESetConfig(&cnf);

	if (stop_flag ==2) {
		scePowerRequestColdReset(0);
	} else if (stop_flag ==3) {
		scePowerRequestStandby();
	}

	vctrlVSHExitVSHMenu(&cnf, NULL, 0);

	return sceKernelExitDeleteThread(0);
}
