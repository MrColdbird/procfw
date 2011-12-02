/*
 * This file is part of PRO CFW.

 * PRO CFW is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * PRO CFW is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PRO CFW. If not, see <http://www.gnu.org/licenses/ .
 */

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

#define INTR(intr) \
	_sw((intr), address); address +=4;
#define INTR_HIGH(intr) \
	_sw((intr&0xFFFF0000) + ((((intr) + (data_address >> 16)) & 0xFFFF)), address); address +=4;
#define INTR_LOW(intr) \
	_sw((intr&0xFFFF0000) + (((intr) + (data_address & 0xFFFF)) & 0xFFFF), address); address +=4;

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

extern int sceKernelPowerLock(unsigned int, unsigned int);

void do_exploit_639(void);
void do_exploit_660(void);

int scePowerRegisterCallbackPrivate_635(unsigned int slot, int cbid);
int scePowerUnregisterCallbackPrivate_635(unsigned int slot);
int scePowerRegisterCallbackPrivate_620(unsigned int slot, int cbid);
int scePowerUnregisterCallbackPrivate_620(unsigned int slot);
int SysMemUserForUser_D8DE5C1E(int arg1, int arg2, int (* callback)(void), int arg4, int branchkiller);

//register callback function - slot bugged. :D
int scePowerRegisterCallbackPrivate(unsigned int slot, int cbid)
{
	int ret = -1;

	switch(psp_fw_version) {
#ifdef CONFIG_635
		case FW_635:
			ret = scePowerRegisterCallbackPrivate_635(slot, cbid);
			break;
#endif
#ifdef CONFIG_620
		case FW_620:
			ret = scePowerRegisterCallbackPrivate_620(slot, cbid);
			break;
#endif
	}

	return ret;
}

//unregister callback function - its slotcheck is bugged too. ;)
int scePowerUnregisterCallbackPrivate(unsigned int slot)
{
	int ret = -1;

	switch(psp_fw_version) {
#ifdef CONFIG_635
		case FW_635:
			ret = scePowerUnregisterCallbackPrivate_635(slot);
			break;
#endif
#ifdef CONFIG_620
		case FW_620:
			ret = scePowerUnregisterCallbackPrivate_620(slot);
			break;
#endif
	}

	return ret;
}

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

#ifdef CONFIG_620
void recovery_sysmem_620(void)
{
	u32 address;

	address = 0x88000000 + g_offs->patchRangeStart;

	INTR(0xACC24230); /* sw v0, 0x4230(a2) */
	INTR(0x0A003322); /* j 0x0800CC88 */
	INTR(0x00001021); /* addu $v0, $zr, $zr */
	INTR(0x3C058801); /* lui $a1, 0x8801 */
}
#endif

#ifdef CONFIG_635
void recovery_sysmem_635(void)
{
	u32 address, data_address;

	address = 0x88000000 + g_offs->patchRangeStart;
	data_address = 0x88013B40;

	// SysMemForKernel_E82F9E1B:
	INTR_HIGH(0x3C050000);
	INTR_LOW(0x8CA409F8);
	INTR(0x3C038002);
	INTR(0x34620001);
	INTR(0x03E00008);
	INTR(0x0004100A);

	// SysMemForKernel_61BFD5D4:
	INTR_HIGH(0x3C050000);
	INTR_LOW(0x8CA609F8);
	INTR(0x00C41024);
	INTR(0x03E00008);
	INTR(0xACA209F8);

	// SysMemForKernel_458A70B5
	INTR_HIGH(0x3C040000);
	INTR(0x03E00008);
	INTR_LOW(0x8C8209FC);

	// SysMemForKernel_E0A74F2D
	INTR_HIGH(0x3C030000);
	INTR(0x00001021);
	INTR(0x03E00008);
	INTR_LOW(0xAC6404A4);

	// SysMemForKernel_A51E2C33
	INTR_HIGH(0x3C050000);
	INTR_LOW(0x8CA304A4);
	INTR(0x27BDFFF0);
	INTR(0x3C058002);
	INTR(0xAFBF0000);
	INTR(0x10600003);
	INTR(0x34A20001);
	INTR(0x0060F809);
	INTR(0x00000000);
	INTR(0x8FBF0000);
	INTR(0x03E00008);
	INTR(0x27BD0010);

	// SysMemForKernel_25A760F0:
	INTR_HIGH(0x3C050000);
	INTR_LOW(0x8CA604A8);
	INTR(0x3C028002);
	INTR(0x14C00003);
	INTR(0x34430001);
	INTR_LOW(0xACA404A8);
	INTR(0x00001821);
	INTR(0x03E00008);
	INTR(0x00601021);

	// SysMemForKernel_288B090A:
	INTR(0x27BDFFF0);
	INTR(0xAFB00000);
	INTR_HIGH(0x3C100000);
	INTR(0x3C038002);
	INTR_LOW(0x8E0604A8);
	INTR(0xAFBF0004);
	INTR(0x00064027);
	INTR(0x0008102B);
	INTR(0x0006482B);
	INTR(0x01224024);
	INTR(0x15000006);
	INTR(0x34670001);
	INTR(0x8FBF0004);
	INTR(0x8FB00000);
	INTR(0x00E01021);
	INTR(0x03E00008);
	INTR(0x27BD0010);
}
#endif

#ifdef CONFIG_660
void recovery_sysmem_660()
{
	_sw(0x3C058801, SYSMEM_TEXT_ADDR + g_offs->sysmem_patch.sceKernelPowerLockForUser); // lui $a1, 0x8801
}
#endif

#ifdef CONFIG_639
void recovery_sysmem_639()
{
	_sw(0x3C058801, SYSMEM_TEXT_ADDR + g_offs->sysmem_patch.sceKernelPowerLockForUser); // lui $a1, 0x8801
}
#endif

int kernel_permission_call(void)
{
	struct sceLoadExecPatch *patch;
	
	//cache invalidation functions
	void (* _sceKernelIcacheInvalidateAll)(void) = (void *)(SYSMEM_TEXT_ADDR + g_offs->sysmem_patch.sceKernelIcacheInvalidateAll);
	void (* _sceKernelDcacheWritebackInvalidateAll)(void) = (void *)(SYSMEM_TEXT_ADDR + g_offs->sysmem_patch.sceKernelDcacheWritebackInvalidateAll);

#ifdef CONFIG_660
	if(psp_fw_version == FW_660) {
		recovery_sysmem_660();
	}
#endif

#ifdef CONFIG_639
	if(psp_fw_version == FW_639) {
		recovery_sysmem_639();
	}
#endif

#ifdef CONFIG_635
	if(psp_fw_version == FW_635) {
		recovery_sysmem_635();
	}
#endif

#ifdef CONFIG_620
	if(psp_fw_version == FW_620) {
		recovery_sysmem_620();
	}
#endif

	//sync cache
	_sceKernelIcacheInvalidateAll();
	_sceKernelDcacheWritebackInvalidateAll();

	//LoadCoreForKernel_EF8A0BEA
	SceModule2 * (* _sceKernelFindModuleByName)(const char * libname) = (void *)g_offs->sceKernelFindModuleByName;

	//find LoadExec module
	SceModule2 * loadexec = _sceKernelFindModuleByName("sceLoadExec");

	//SysMemForKernel_458A70B5
	int (* _sceKernelGetModel)(void) = (void *)(SYSMEM_TEXT_ADDR + g_offs->sysmem_patch.sceKernelGetModel);

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

#if defined(CONFIG_620) || defined(CONFIG_635)
//safe syscall wrapper for kernel_permission_call - this lets us return safely!
int kernelSyscall(void);
u32 get_power_address(int cbid);

static inline u32 get_power_slot_by_address(u32 address, u32 power_buf_address)
{
	if (address < power_buf_address)
		return (~(power_buf_address - address) + 1) >> 4;

	return (address - power_buf_address) >> 4;
}

#ifdef CONFIG_635
// psp-go don't need this for it's always zero.
static void patch_power_arg(int cbid, u32 power_buf_address)
{
	int slot;

	while (1) {
		slot = 0x204>>4;
		scePowerUnregisterCallbackPrivate(slot);
		scePowerRegisterCallbackPrivate(slot, cbid);

		*(u32*)0x08900008 = 0xDEADBEEF;
		slot = get_power_slot_by_address(0x08900000, power_buf_address);
		scePowerUnregisterCallbackPrivate(slot);
		scePowerRegisterCallbackPrivate(slot, cbid);

		if (*((u32*)0x08900008) == 0) {
			printk("%s: power_arg noped\r\n", __func__);
			break;
		}

		printk("Retrying...\r\n");
		sceKernelDelayThread(1000000);
	}
}

static u16 g_working_intr_prefix[] = {
	0x04A3, // bgezl $a1
	0x04A1, // bgez $a1
	0x04C0, // bltz $a2
	0x04C2, // bltzl $a2
	0x04D0, // bltzal $a2
	0x04D2, // bltzall $a2
	0x04E0, // bltz $a3
	0x04E2, // bltzl $a3
	0x04F0, // bltzal $a3
	0x04F2, // bltzall $a3
	0x0503, // bgezl $t0
	0x0501, // bgez $t0
};

static int is_intr_OK(u32 intr)
{
	u16 prefix;
	int i;

	prefix = intr >> 16;

	for(i=0; i<NELEMS(g_working_intr_prefix); ++i) {
		if (g_working_intr_prefix[i] == prefix)
			return 1;
	}

	return 0; 
}

static int is_pspgo(void)
{
	// This call will always fail, but with a different error code depending on the model
	SceUID result = sceIoOpen("ef0:/", 1, 0777);

	// Check for "No such device" error
	return ((result == (int)0x80020321) ? 0 : 1);
}
#endif

/* for 6.20/6.35 */
void do_exploit(void)
{
	u32 power_buf_address = 0;
	//create empty callback
	int cbid = -1;
	int result;

#ifdef CONFIG_635
	if(psp_fw_version == FW_635) {
		//create a fitting one
		while(!is_intr_OK((u32)cbid)) {
			//sceKernelDeleteCallback(cbid);
			cbid = sceKernelCreateCallback("", NULL, NULL);
		}
	} 
#endif

#ifdef CONFIG_620
	if (psp_fw_version == FW_620) {
		cbid = sceKernelCreateCallback("", NULL, NULL);
	}
#endif

	printk("Got a CBID: 0x%08X\r\n", cbid);

	//Davee $v1 trick, $v1 would leak the power_buf_address when called on an registered slot 0
	scePowerRegisterCallbackPrivate(0, cbid);
	power_buf_address = get_power_address(cbid);
	scePowerUnregisterCallbackPrivate(0);
	printk("power_buf_address 0x%08X\r\n", (uint)power_buf_address);

#ifdef CONFIG_635
	if(psp_fw_version == FW_635 && !is_pspgo()) {
		patch_power_arg(cbid, power_buf_address);
	}
#endif

	//override sysmem function
	unsigned int smpos = g_offs->patchRangeStart; for(; smpos < g_offs->patchRangeEnd; smpos += 16)
	{
		//calculate slot
		unsigned int slot = get_power_slot_by_address(((u32)0x88000000)+smpos, power_buf_address);

		//wipe memory with -1... else overriding fails.
		scePowerUnregisterCallbackPrivate(slot);

		//register dummy callback (override memory ^^)
		result = scePowerRegisterCallbackPrivate(slot, cbid);

		//patching error
		if(result) break;
	}

	//flush changes to memory
	sync_cache();

	//restoring instructions and patching loadexec
	unsigned int interrupts = pspSdkDisableInterrupts();

#ifdef CONFIG_635
	if(psp_fw_version == FW_635) {
		result = SysMemUserForUser_D8DE5C1E(0xC01DB15D, 0xC00DCAFE, kernelSyscall, 0x12345678, -1);
	}
#endif

#ifdef CONFIG_620
	if (psp_fw_version == FW_620) {
		u32 kernel_entry, entry_addr;

		kernel_entry = (u32) &kernel_permission_call;
		entry_addr = ((u32) &kernel_entry) - 16;
		result = sceKernelPowerLock(0, ((u32) &entry_addr) - 0x4234);
	}
#endif

	pspSdkEnableInterrupts(interrupts);
}
#endif

//entry point
int main(int argc, char * argv[])
{
	psp_fw_version = sceKernelDevkitVersion();
	setup_patch_offset_table(psp_fw_version);

	printk_init("ms0:/fastrecovery.txt");
	printk("Hello exploit\r\n");
	pspDebugScreenInit();

	input_dump_kmem();

#ifdef CONFIG_639
	if(psp_fw_version == FW_639) {
		do_exploit_639();
	}
#endif

#if defined(CONFIG_620) || defined(CONFIG_635)
	if(psp_fw_version == FW_620 || psp_fw_version == FW_635) {
		do_exploit();
	}
#endif

#ifdef CONFIG_660
	{
		do_exploit_660();
	}
#endif

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
