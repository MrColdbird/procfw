#include <pspsdk.h>
#include <pspsysmem_kernel.h>
#include <pspkernel.h>
#include <psputilsforkernel.h>
#include <pspsysevent.h>
#include <pspiofilemgr.h>
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "utils.h"
#include "systemctrl.h"
#include "systemctrl_se.h"
#include "printk.h"
#include "../Rebootex_bin/rebootex.h"
#include "rebootex_conf.h"
#include "strsafe.h"
#include "systemctrl_patch_offset.h"

static int (*LoadReboot)(void * arg1, unsigned int arg2, void * arg3, unsigned int arg4) = NULL;
rebootex_config rebootex_conf;

//load reboot wrapper
static int load_reboot(void * arg1, unsigned int arg2, void * arg3, unsigned int arg4)
{
	//copy reboot extender
	memcpy((void*)0x88FC0000, rebootex, size_rebootex);

	//reset reboot flags
	memset((void*)0x88FB0000, 0, 0x100);

	//store psp model
	_sw(psp_model, 0x88FB0000);

	//store custom partition size
	if(g_p2_size != 24 || g_p9_size != 24) {
		//store partition 2 length
		_sw(g_p2_size, 0x88FB0008);

		//store partition 9 length
		_sw(g_p9_size, 0x88FB000C);
	}

	_sw((u32)g_insert_module_before, 0x88FB0010);
	_sw((u32)g_insert_module_binary, 0x88FB0014);
	_sw((u32)g_insert_module_size, 0x88FB0018);
	_sw((u32)g_insert_module_flags, 0x88FB001C);
	
	memcpy((void*)0x88FB0020, &rebootex_conf, sizeof(rebootex_conf));

	memset((void*)0x88FB0100, 0, 256);
	strcpy_s((char*)0x88FB0100, 256, sctrlSEGetUmdFile());

	//forward
	return (*LoadReboot)(arg1, arg2, arg3, arg4);
}

void patch_sceLoadExec(void)
{
	SceModule2 * loadexec = (SceModule2*)sceKernelFindModuleByName("sceLoadExec");
	u32 text_addr;
	struct sceLoadExecPatch *patch;

	if (loadexec == NULL) {
		return;
	}

	text_addr = loadexec->text_addr;

	if(psp_model == PSP_GO) {
		patch = &g_offs->loadexec_patch_05g;
	} else {
		patch = &g_offs->loadexec_patch_other;
	}

	//replace LoadReboot function
	_sw(MAKE_CALL(load_reboot), loadexec->text_addr + patch->LoadRebootCall);

	//patch Rebootex position to 0x88FC0000
	_sw(0x3C0188FC, loadexec->text_addr + patch->RebootAddress); // lui $at, 0x88FC

	//save LoadReboot function
	LoadReboot = (void*)(loadexec->text_addr + patch->LoadReboot);

	//allow user $k1 configs to call sceKernelLoadExecWithApiType
	_sw(0x1000000C, loadexec->text_addr + patch->sceKernelLoadExecWithApiTypeCheck1);
	//allow all user levels to call sceKernelLoadExecWithApiType
	_sw(NOP, loadexec->text_addr + patch->sceKernelLoadExecWithApiTypeCheck2);

	//allow all user levels to call sceKernelExitVSHVSH
	_sw(0x10000008, loadexec->text_addr + patch->sceKernelExitVSHVSHCheck1);
	_sw(NOP, loadexec->text_addr + patch->sceKernelExitVSHVSHCheck2);
}
