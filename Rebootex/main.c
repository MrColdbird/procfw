#include <pspkernel.h>
#include <psputilsforkernel.h>
#include <pspsysevent.h>
#include <pspiofilemgr.h>
#include <stdio.h>
#include <string.h>
#include "systemctrl.h"
#include "utils.h"
#include "../Rebootex_bin/rebootex.h"
#include "rebootex_patch_offset.h"

u32 psp_model;

PSP_MODULE_INFO("fast_recovery", 0x1000, 1, 0);

static int (*LoadReboot)(void * arg1, unsigned int arg2, void * arg3, unsigned int arg4) = NULL;

//load reboot wrapper
int load_reboot(void * arg1, unsigned int arg2, void * arg3, unsigned int arg4)
{
	//copy reboot extender
	memcpy((void*)0x88FC0000, rebootex, size_rebootex);

	//reset reboot flags
	memset((void*)0x88FB0000, 0, 0x100);
	memset((void*)0x88FB0100, 0, 256);

	//store psp model
	_sw(psp_model, 0x88FB0000);

	//forward
	return (*LoadReboot)(arg1, arg2, arg3, arg4);
}

// mode: 0 - OFW 1 - CFW
void patch_sceLoadExec(int mode)
{
	SceModule2 * loadexec = (SceModule2*)sceKernelFindModuleByName("sceLoadExec");
	unsigned int offsets[6];
	u32 text_addr;
	struct LoadExecPatch *patch;

	if (loadexec == NULL) {
		return;
	}

	text_addr = loadexec->text_addr;

	if(psp_model == PSP_GO) { // PSP-N1000
		patch = &g_offs->loadexec_patch_05g;
	} else {
		patch = &g_offs->loadexec_patch_other;
	}
	
	//save LoadReboot function
	LoadReboot = (void*)loadexec->text_addr;

	if(mode == 0) {
		//restore LoadReboot
		_sw(MAKE_CALL(LoadReboot), loadexec->text_addr + patch->LoadRebootCall);

		//restore jmp to 0x88600000
		_sw(0x3C018860, loadexec->text_addr + patch->RebootJump);
	} else if(mode == 1) {
		//replace LoadReboot function
		_sw(MAKE_CALL(load_reboot), loadexec->text_addr + patch->LoadRebootCall);

		//patch Rebootex position to 0x88FC0000
		_sw(0x3C0188FC, loadexec->text_addr + patch->RebootJump); // lui $at, 0x88FC
	}

	sync_cache();
}

int main_thread(SceSize args, void *argp)
{
	int mode = 1;

	if(args == 4 && 0 == *(u32*)argp) {
		mode = 0;
	}

	patch_sceLoadExec(mode);
	sctrlKernelExitVSH(NULL);

	return 0;
}

int module_start(SceSize args, void* argp)
{
	int thid;
	//SysMemForKernel_458A70B5
	int (* _sceKernelGetModel)(void);

	setup_patch_offset_table(0x06030510);
	//save psp model
	_sceKernelGetModel = (void *)(g_offs->sceKernelGetModel);
	psp_model = _sceKernelGetModel();

	thid = sceKernelCreateThread("fastRecovery", main_thread, 0x1A, 0x1000, 0, NULL);

	if(thid>=0) {
		sceKernelStartThread(thid, args, argp);
	}

	return 0;
}

int module_stop(SceSize args, void *argp)
{
	return 0;
}
