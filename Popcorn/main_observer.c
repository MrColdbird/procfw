#include <pspkernel.h>
#include <psputilsforkernel.h>
#include <pspsysevent.h>
#include <pspiofilemgr.h>
#include <stdio.h>
#include <string.h>

#include "pspmodulemgr_kernel.h"
#include "systemctrl.h"
#include "printk.h"
#include "utils.h"

PSP_MODULE_INFO("M33PopcornManager", 0x1007, 1, 1);

#define FAKE_RIF "JP0700-ULJS00237_00-EXTRAMISSION0001"

// 4300
u32 g_missing_PNG_icon = 1;

// 4308
u32 g_not_found_pstitle = 0;

// BCC
extern u8 g_icon_png[0x3730];

// 4304
STMOD_HANDLER g_previous = NULL;

// 0
int myIoRead(int fd, u8 *buf, int size)
{
	int ret;
	u32 pos;

	pos = sceIoLseek32(fd, 0, SEEK_CUR);
	ret = sceIoRead(fd, buf, size);
	printk("%s: fd=0x%08X pos=0x%08X size=%d -> 0x%08X\n", __func__, fd, pos, size, ret);

	return ret;
}

int myIoClose(SceUID fd)
{
	int ret;

	ret = sceIoClose(fd);
	printk("%s: 0x%08X -> 0x%08X\n", __func__, fd, ret);

	return ret;
}

// 750
int popcorn_patch_chain(SceModule2 *mod)
{
	printk("%s: %s\n", __func__, mod->modname);

	if (0 == strcmp(mod->modname, "pops")) {
		u32 text_addr = mod->text_addr;

		printk("%s: patch pops\n", __func__);
		(void)(text_addr);

		sync_cache();
	}

	if(g_previous)
		return g_previous(mod);
	
	return 0;
}

// 2E4
void reboot_vsh_with_error(u32 error)
{
	struct SceKernelLoadExecVSHParam param;	
	u32 vshmain_args[0x20/4];

	memset(&param, 0, sizeof(param));
	memset(vshmain_args, 0, sizeof(vshmain_args));

	vshmain_args[0/4] = 0x0400;
	vshmain_args[4/4] = 0x20;
	vshmain_args[0x14/4] = error;

	param.size = sizeof(param);
	param.args = 0x400;
	param.argp = vshmain_args;
	param.vshmain_args_size = 0x400;
	param.vshmain_args = vshmain_args;
	param.configfile = "/kd/pspbtcnf.txt";

	sctrlKernelExitVSH(&param);
}

// 358
int decompress_data(u32 destSize, const u8 *src, u8 *dest)
{
	u32 k1;
	int ret;

	k1 = pspSdkSetK1(0);

	if (destSize < 0) {
		reboot_vsh_with_error((u32)destSize);
		pspSdkSetK1(k1);

		return 0;
	}

	ret = sceKernelDeflateDecompress(dest, destSize, src, 0);

	if (ret == 0x9300) {
		printk("%s: [FAKE] 0x9300 -> 0x92FF\n", __func__);

		ret = 0x92FF;
	}

	pspSdkSetK1(k1);

	return ret;
}

// 19C
int setup_PGD_fd(char *path, void *keys, int flags, int filemode, int offset)
{
	SceUID fd;
	u32 signature;
	int ret;

	fd = sceIoOpen(path, flags, filemode);

	if (fd < 0) {
		return fd;
	}

	sceIoLseek(fd, offset, PSP_SEEK_SET);
	sceIoRead(fd, &signature, 4);
	
	if (signature != 0x44475000) {
		printk("%s: NO PDG signature found, ignore DRM key setting\n", __func__);
		
		sceIoLseek(fd, offset, PSP_SEEK_SET);

		return fd;
	}

	sceIoClose(fd);
	fd = sceIoOpen(path, flags | 0x40000000, filemode);

	if (fd < 0) {
		return fd;
	}

	// setting PDG offset
	ret = sceIoIoctl(fd, 0x04100002, &offset, 4, NULL, 0);

	if (ret < 0) {
		sceIoClose(fd);

		return ret;
	}

	// setting PDG decrypt key
	ret = sceIoIoctl(fd, 0x04100001, keys, 0x10, NULL, 0);

	if (ret < 0) {
		sceIoClose(fd);

		return ret;
	}

	printk("%s: DRM key setup OK\n", __func__);
	
	return fd;
}

int myIoOpen(const char *file, int flag, int mode)
{
	int ret;

	if(strstr(file, FAKE_RIF)) {
		printk("%s: [FAKE]\n", __func__);
		ret = 0x10000;
	} else {
		ret = sceIoOpen(file, flag, mode);
	}

	printk("%s: %s 0x%08X -> 0x%08X\n", __func__, file, flag, ret);

	return ret;
}

int myIoIoctl(SceUID fd, unsigned int cmd, void * indata, int inlen, void * outdata, int outlen)
{
	int ret;

	if(cmd == 0x04100001 && inlen == 16) {
		printk("%s: setting PGD key\n", __func__);
		hexdump(indata, inlen);
	}

	if(cmd == 0x04100002 && inlen == 4) {
		printk("%s: setting PGD offset: 0x%08X\n", __func__, *(u32*)indata);
	}

	ret = sceIoIoctl(fd, cmd, indata, inlen, outdata, outlen);

#ifdef DEBUG
	printk("%s: 0x%08X -> 0x%08X\n", __func__, fd, ret);
#endif

	return ret;
}

int myIoGetstat(const char *path, SceIoStat *stat)
{
	int ret;

	ret = sceIoGetstat(path, stat);
	printk("%s: %s -> 0x%08X\n", __func__, path, ret);

	return ret;
}

extern int strnlen(const char *str, int size);

static int _strnlen(const char *str, int size)
{
	int ret;

	ret = strnlen(str, size);
	printk("%s: %s %d -> %d\n", __func__, str, size, ret);

	return ret;
}

static int _strncmp(const char *a, const char *b, int size)
{
	int ret;

	ret = strncmp(a, b, size);
	printk("%s: %s %s %d-> %d\n", __func__, a, b, size, ret);

	return ret;
}

static int (*_check_rif_path)(const char *src, char *dst) = NULL;

static int check_rif_path(char *src, char *dst)
{
	int ret;

	ret = (*_check_rif_path)(src, dst);
	printk("%s: %s %s-> 0x%08X\n", __func__, src, dst, ret);

	return ret;
}

static void patch_scePops_Manager(void)
{
	SceModule2 *mod;
	u32 text_addr;

	mod = (SceModule2*) sceKernelFindModuleByName("scePops_Manager");
	text_addr = mod->text_addr;

	_sw(MAKE_JUMP(&myIoOpen), text_addr+0x00003B98);
	_sw(NOP, text_addr+0x00003B9C);

	_sw(MAKE_JUMP(&myIoIoctl), text_addr+0x00003BA8);
	_sw(NOP, text_addr+0x00003BAC);

	_sw(MAKE_JUMP(&myIoRead), text_addr+0x00003BB0);
	_sw(NOP, text_addr+0x00003BB4);

	_sw(MAKE_JUMP(&_strncmp), text_addr+0x00003C38);
	_sw(NOP, text_addr+0x00003C3C);

	_sw(MAKE_JUMP(&_strnlen), text_addr+0x00003C40);
	_sw(NOP, text_addr+0x00003C44);

	_sw(MAKE_JUMP(&myIoGetstat), text_addr+0x00003BD0);
	_sw(NOP, text_addr+0x00003BD4);

	_sw(MAKE_JUMP(&myIoClose), text_addr+0x00003BC0);
	_sw(NOP, text_addr+0x00003BC4);

	_check_rif_path = (void*)(text_addr+0x00000190);
	_sw(MAKE_CALL(&check_rif_path), text_addr+0x00002798);
	_sw(MAKE_CALL(&check_rif_path), text_addr+0x00002C58);
}

static void setup_psx_fw_version(u32 fw_version)
{
	int (*_SysMemUserForUser_315AD3A0)(u32 fw_version);
	
	_SysMemUserForUser_315AD3A0 = (void*)sctrlHENFindFunction("sceSystemMemoryManager", "SysMemUserForUser", 0x315AD3A0);

	if (_SysMemUserForUser_315AD3A0 == NULL) {
		printk("_SysMemUserForUser_315AD3A0 not found\n");
		reboot_vsh_with_error(0x80000001);
	}

	_SysMemUserForUser_315AD3A0(fw_version);
}

int module_start(SceSize args, void* argp)
{
	int fd;

	printk_init("ms0:/popcorn.txt");
	printk("Popcorn: init_file = %s\n", sceKernelInitFileName());
	fd = sceIoOpen(sceKernelInitFileName(), PSP_O_RDONLY, 0);

	if (fd < 0) {
		printk("sceIoOpen returns 0x%08X\n", fd);

		return 1;
	}

	(void)(setup_psx_fw_version);
	patch_scePops_Manager();
	sync_cache();

	return 0;
}

int module_stop(SceSize args, void *argp)
{
	return 0;
}
