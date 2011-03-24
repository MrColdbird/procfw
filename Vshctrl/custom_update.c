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
#include "vshctrl_patch_offset.h"

void patch_update_plugin_module(u32 text_addr)
{
	int version;

	// ImageVersion
	// If it's lower than the one in updatelist.txt then the FW will update
	version = (sctrlHENGetVersion() << 16) | sctrlHENGetMinorVersion();

	_sw((version >> 16) | 0x3C050000, text_addr + g_offs->custom_update_patch.UpdatePluginImageVersion1);
	_sw((version & 0xFFFF) | 0x34A40000, text_addr + g_offs->custom_update_patch.UpdatePluginImageVersion2);
}

void patch_SceUpdateDL_Library(u32 text_addr)
{
	char *p;

	if(NULL == sceKernelFindModuleByName("update_plugin_module")) {
		return;
	}
	
	p = (char*)(text_addr + g_offs->custom_update_patch. SceUpdateDL_UpdateListStr);

	if(psp_model == PSP_GO) {
		strcpy(p, "http://pro.coldbird.co.cc/pspgo-updatelist.txt");
	} else {
		strcpy(p, "http://pro.coldbird.co.cc/psp-updatelist.txt");
	}

	_sw(NOP, text_addr + g_offs->custom_update_patch.SceUpdateDL_UpdateListCall1);
	_sw(NOP, text_addr + g_offs->custom_update_patch.SceUpdateDL_UpdateListCall2);
	_sw(NOP, text_addr + g_offs->custom_update_patch.SceUpdateDL_UpdateListCall3);
	_sw(NOP, text_addr + g_offs->custom_update_patch.SceUpdateDL_UpdateListCall4);
}
