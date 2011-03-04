#include <pspsdk.h>
#include <pspkernel.h>
#include <pspsysmem_kernel.h>
#include <pspthreadman_kernel.h>
#include <pspdebug.h>
#include <pspinit.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <pspctrl.h>
#include "systemctrl.h"
#include "systemctrl_se.h"
#include "systemctrl_private.h"
#include "kubridge.h"
#include "utils.h"
#include "printk.h"
#include "strsafe.h"
#include "libs.h"
#include "stargate.h"

struct CtrlHookImports {
	u32 nid;
	void *fp;
};

static void remove_analog_input(SceCtrlData *data)
{
	if(data == NULL)
		return;

	data->Lx = 0xFF/2;
	data->Ly = 0xFF/2;
}

int myCtrlPeekBufferPositive(SceCtrlData *pad_data, int count)
{
	int ret;

	ret = sceCtrlPeekBufferPositive(pad_data, count);
	remove_analog_input(pad_data);

	return ret;
}

int myCtrlPeekBufferNegative(SceCtrlData *pad_data, int count)
{
	int ret;

	ret = sceCtrlPeekBufferNegative(pad_data, count);
	remove_analog_input(pad_data);

	return ret;
}

int myCtrlReadBufferPositive(SceCtrlData *pad_data, int count)
{
	int ret;

	ret = sceCtrlReadBufferPositive(pad_data, count);
	remove_analog_input(pad_data);

	return ret;
}

int myCtrlReadBufferNegative(SceCtrlData *pad_data, int count)
{
	int ret;

	ret = sceCtrlReadBufferNegative(pad_data, count);
	remove_analog_input(pad_data);

	return ret;
}

static struct CtrlHookImports g_ctrl_hooks[] = {
	{ 0x3A622550, &myCtrlPeekBufferPositive, },
	{ 0xC152080A, &myCtrlPeekBufferNegative, },
	{ 0x1F803938, &myCtrlReadBufferPositive, },
	{ 0x60B81F86, &myCtrlReadBufferNegative, },
};

void patch_analog_imports(SceModule *mod)
{
	int i;

	for(i=0; i<NELEMS(g_ctrl_hooks); ++i) {
		hook_import_bynid(mod, "sceCtrl", g_ctrl_hooks[i].nid, g_ctrl_hooks[i].fp, 1);
	}
}
