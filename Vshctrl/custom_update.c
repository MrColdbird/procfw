#include <pspsdk.h>
#include <pspsysmem_kernel.h>
#include <pspkernel.h>
#include <psputilsforkernel.h>
#include <pspsysevent.h>
#include <pspiofilemgr.h>
#include <pspctrl.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "libs.h"
#include "systemctrl.h"
#include "printk.h"
#include "systemctrl_se.h"
#include "main.h"
#include "strsafe.h"

void patch_update_plugin_module(u32 text_addr)
{
	int version;

	// ImageVersion
	// If it's lower than the one in updatelist.txt then the FW will update
	version = (sctrlHENGetVersion() << 16) | sctrlHENGetMinorVersion();

	_sw((version >> 16) | 0x3C050000, text_addr + 0x000081B4);
	_sw((version & 0xFFFF) | 0x34A40000, text_addr + 0x000081BC);
}

void patch_SceUpdateDL_Library(u32 text_addr)
{
	char *p;

	p = (char*)(text_addr+0x000032BC);

	if(psp_model == PSP_GO) {
		strcpy(p, "http://pro.coldbird.co.cc/pspgo-updatelist.txt");
	} else {
		strcpy(p, "http://pro.coldbird.co.cc/psp-updatelist.txt");
	}

	_sw(NOP, text_addr+0x00002044);
	_sw(NOP, text_addr+0x00002054);
	_sw(NOP, text_addr+0x00002080);
	_sw(NOP, text_addr+0x0000209C);
}
