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
	u32 png_signature = 0x474E5089, pos;

	if(fd == 0x10000) {
		size = 152;
		printk("%s: fake rif content %d\n", __func__, size);
		memset(buf, 0, size);
		strcpy((char*)(buf+0x10), FAKE_RIF);

		return size;
	}
	
	pos = sceIoLseek32(fd, 0, SEEK_CUR);
	ret = sceIoRead(fd, buf, size);
	printk("%s: fd=0x%08X pos=0x%08X size=%d -> 0x%08X\n", __func__, fd, pos, size, ret);

	if (ret != size) {
		return ret;
	}

	if (g_missing_PNG_icon && ret == sizeof(g_icon_png)) {
		if (!memcmp(buf, &png_signature, 4)) {
			printk("%s: returns a PNG for mission icon\n", __func__);
			memcpy(buf, g_icon_png, sizeof(g_icon_png));

			return sizeof(g_icon_png);
		}
	}

	// loc_60
	if (ret == 4) {
		u32 magic;

		magic = 0x464C457F; // ~ELF

		if(0 == memcmp(buf, &magic, sizeof(magic))) {
			magic = 0x5053507E; // ~PSP
			memcpy(buf, &magic, sizeof(magic));
		}

		printk("%s: patch ~ELF -> ~PSP\n", __func__);

		return 4;
	}

	// loc_6c
	if (buf[0x41B] == 0x27 &&
			buf[0x41C] == 0x19 &&
			buf[0x41D] == 0x22 &&
			buf[0x41E] == 0x41 &&
			buf[0x41A] == buf[0x41F]) {
		buf[0x41B] = 0x55;
		printk("%s: unknown patch loc_6c\n", __func__);
	}

	return ret;
}

int myIoClose(SceUID fd)
{
	int ret;

	if (fd == 0x10000) {
		printk("%s: [FAKE]\n", __func__);
		ret = 0;
	} else {
		ret = sceIoClose(fd);
	}

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

		_sw(0, mod->text_addr + 0x1E80);

		//kill first 0x80010016 error
		_sw(0x24020000, mod->text_addr + 0x220);
#if 0
		_sw(0x5000FFE3, text_addr+0xE9AC);
		_sw(_lw(text_addr+0x16A08), text_addr+0xE90C); // jal scePopsManExitVSHKernel
		_sw(0, text_addr+0x190A8);

		if (g_missing_PNG_icon) {
			_sw(0x24053730, text_addr+0x2734C);
		}
#endif

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

int myIoOpen(const char *file, int flag, int mode)
{
	int ret;

	if(strstr(file, FAKE_RIF)) {
		printk("%s: [FAKE]\n", __func__);
		ret = 0x10000;
	} else {
		u32 real_flag;

		real_flag = flag & (~0x40000000);
		ret = sceIoOpen(file, real_flag, mode);
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

	if (cmd == 0x04100001) {
		ret = 0;
		printk("%s: [FAKE] 0x%08X 0x%08X -> 0x%08X\n", __func__, fd, cmd, ret);
		goto exit;
	}

	if (cmd == 0x04100002) {
		ret = 0;
		printk("%s: [FAKE] 0x%08X 0x%08X -> 0x%08X\n", __func__, fd, cmd, ret);
		sceIoLseek32(fd, *(u32*)indata, PSP_SEEK_SET);
		goto exit;
	}

	ret = sceIoIoctl(fd, cmd, indata, inlen, outdata, outlen);

exit:
#ifdef DEBUG
	printk("%s: 0x%08X -> 0x%08X\n", __func__, fd, ret);
#endif

	return ret;
}

int myIoGetstat(const char *path, SceIoStat *stat)
{
	int ret;

	if(strstr(path, FAKE_RIF)) {
		ret = 0;
		printk("%s: [FAKE]\n", __func__);
	} else {
		ret = sceIoGetstat(path, stat);
	}

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

static int (*_get_rif_path)(const char *name, char *path) = NULL;

static int get_rif_path(const char *name, char *path)
{
	int ret;

	strcpy(name, FAKE_RIF);
	ret = (*_get_rif_path)(name, path);
	printk("%s: %s %s-> 0x%08X\n", __func__, name, path, ret);

	return ret;
}

static void patch_scePops_Manager(void)
{
	SceModule2 *mod;
	u32 text_addr;

	mod = (SceModule2*) sceKernelFindModuleByName("scePops_Manager");
	text_addr = mod->text_addr;

	// cannot find 635 one
	// _sw(0, text_addr+0x158);

	_sw(0, text_addr+0x1E80); // 158C

	_sw(MAKE_JUMP(&decompress_data), text_addr+0x00001F58); // 162C
	_sw(0, text_addr+0x00001F5C); // 1630

	// it no longer works because 635 sceIoIoctl becames inline now
#if 0
	_sw(MAKE_JUMP(&setup_PGD_fd), text_addr+0x6A8); // for sceIoIoctl
	_sw(0, text_addr+0x6AC);

	_sw(MAKE_JUMP(&setup_PGD_fd), text_addr+0xCA0);
	_sw(0, text_addr+0xCA4);
#endif

#if 0
	_sw(0, text_addr+0x378);  // 0x564 (6.31)
	_sw(0, text_addr+0xA80);  // 0x126C (6.31)
	_sw(0, text_addr+0xE2C);  // 0x14B0 (6.31)
	_sw(0, text_addr+0xECC);  // 0x15B8 (6.31)
	_sw(0, text_addr+0x1100); // 0x1780 (6.31)
	_sw(0, text_addr+0x1354); // 0x1CCC (6.31)
	_sw(0, text_addr+0x278C); // 0x3C8C (6.31)
#endif

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

	_get_rif_path = (void*)(text_addr+0x00000190);
	_sw(MAKE_CALL(&get_rif_path), text_addr+0x00002798);
	_sw(MAKE_CALL(&get_rif_path), text_addr+0x00002C58);

	//removing break instruction
	_sw(0, text_addr + 0x1E80);

	//kill first 0x80010016 error
	_sw(0x24020000, text_addr + 0x220);

	//kill second 0x80010016 error
	_sw(0, text_addr + 0x284C);

	//fake success sceNpDrmGetVersionKey
	_sw(0x24020000, text_addr + 0x29CC);
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
	u8 header[40];
	u32 icon0_offset = 0, psar_offset = 0;

	printk_init("ms0:/popcorn.txt");
	printk("Popcorn: init_file = %s\n", sceKernelInitFileName());
	fd = sceIoOpen(sceKernelInitFileName(), PSP_O_RDONLY, 0);

	if (fd < 0) {
		printk("sceIoOpen returns 0x%08X\n", fd);

		return 1;
	}

	sceIoRead(fd, header, sizeof(header));
	icon0_offset = *(u32*)(header+0x0c);
	psar_offset = *(u32*)(header+0x24);
	sceIoLseek(fd, psar_offset, PSP_SEEK_SET);
	sceIoRead(fd, header, sizeof(header));

	if (0 == memcmp(header, "PSTITLE", 7)) {
		printk("header match PSTITILE\n");
		sceIoLseek(fd, psar_offset + 0x200, PSP_SEEK_SET);
	} else {
		printk("header doesn't match PSTITILE\n");
		sceIoLseek(fd, psar_offset + 0x400, PSP_SEEK_SET);
		g_not_found_pstitle = 1;
	}

	sceIoRead(fd, header, sizeof(header));

	if (*(u32*)header == 0x44475000) { //PDG
		printk("PDG flag found, genius PSX game\n");
		sceIoClose(fd);

		// module exit
		return 1;
	}

	sceIoLseek(fd, icon0_offset, PSP_SEEK_SET);
	sceIoRead(fd, header, sizeof(header));

	if (*(u32*)header == 0x474E5089 && // PNG
			*(u32*)(header+4) == 0xA1A0A0D && // PNG
			*(u32*)(header+0xc) == 0x52444849 && // IHDR
			*(u32*)(header+0x10) == 0x50000000 && // 
			*(u32*)(header+0x14) == *(u32*)(header+0x10)
	   ) {
		printk("PNG file found\n");
		g_missing_PNG_icon = 0;
	} else {
		printk("PNG file is missing\n");
	}
	
	sceIoClose(fd);
	g_previous = sctrlHENSetStartModuleHandler(&popcorn_patch_chain);
	setup_psx_fw_version(0x03090010);
	patch_scePops_Manager();
	sync_cache();

	return 0;
}

int module_stop(SceSize args, void *argp)
{
	return 0;
}
