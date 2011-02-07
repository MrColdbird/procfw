#include <pspkernel.h>
#include <psputilsforkernel.h>
#include <pspsysevent.h>
#include <pspiofilemgr.h>
#include <stdio.h>
#include <string.h>

#include "pspmodulemgr_kernel.h"
#include "systemctrl.h"
#include "printk.h"

PSP_MODULE_INFO("M33PopcornManager", 0x1007, 1, 1);

#define MAKE_JUMP(f) (0x08000000 | (((u32)(f) >> 2)  & 0x03ffffff))
#define MAKE_CALL(f) (0x0c000000 | (((u32)(f) >> 2) & 0x03ffffff)) 

// 0x4300
u32 g_missing_PNG_icon = 1;

// 0x00004308
u32 g_data_4308 = 0;

// 0xBCC
extern u8 g_icon_png[0x3730];

// 0x00004304
STMOD_HANDLER g_previous = NULL;

// 0x0
int sceIoReadPatched(int fd, u8 *buf, int size)
{
	int ret;
	u32 png_signature = 0x474E5089;
	
	ret = sceIoRead(fd, buf, size);

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
	if (ret == 4 && !(((u32)buf) & 4)) {
		if (*(u32*)(buf) == 0x464C457F) { // ~ELF
			*(u32*)(buf) = 0x5053507E; // ~PSP
		}

		printk("%s: patch plain ELF header to ~PSP\n", __func__);

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

// sub_41c
void sync_cache()
{
	sceKernelDcacheWritebackAll();
	sceKernelIcacheClearAll();
}

int sub_750(SceModule2 *mod)
{
	if (!strcmp(mod->modname, "pops")) {
		u32 text_addr = mod->text_addr;

		printk("%s: patch pops\n", __func__);

		_sw(0x5000FFE3, text_addr+0xE9AC);
		_sw(_lw(text_addr+0x16A08), text_addr+0xE90C); // jal scePopsManExitVSHKernel
		_sw(0, text_addr+0x190A8);

		if (g_missing_PNG_icon) {
			_sw(0x24053730, text_addr+0x2734C);
		}

		sync_cache();
	}

	if(g_previous)
		return g_previous(mod);
	
	return 0;
}

void sub_2E4(u32 error)
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

int sub_358(u32 destSize, const u8 *src, u8 *dest)
{
	int k1 = pspSdkSetK1(0);
	int ret;

	if (destSize < 0) {
		sub_2E4((u32)destSize);
		pspSdkSetK1(k1);

		return 0;
	}

	ret = sceKernelDeflateDecompress(dest, destSize, src, 0);

	if (ret == 0x9300) {
		printk("%s: return 0x92FF instead of 0x9300\n", __func__);

		ret = 0x92FF;
	}

	pspSdkSetK1(k1);

	return ret;
}

int sub_19C(char *path, void *keys, int flags, int filemode, int offset)
{
	SceUID fd = sceIoOpen(path, flags, filemode);
	u32 signature;
	int ret;

	if (fd < 0)
		return fd;

	sceIoLseek(fd, offset, PSP_SEEK_SET);
	sceIoRead(fd, &signature, 4);
	
	if (signature != 0x44475000) {
		printk("%s: NO PDG signature found, ignore DRM key setting\n", __func__);
		
		sceIoLseek(fd, offset, PSP_SEEK_SET);

		return fd;
	}

	sceIoClose(fd);
	fd = sceIoOpen(path, flags | 0x40000000, filemode);

	if (fd < 0)
		return fd;

	ret = sceIoIoctl(fd, 0x04100002, &offset, 4, NULL, 0);

	if (ret < 0) {
		sceIoClose(fd);

		return ret;
	}

	ret = sceIoIoctl(fd, 0x04100001, keys, 0x10, NULL, 0);

	if (ret < 0) {
		sceIoClose(fd);

		return ret;
	}

	printk("%s: DRM key setup OK\n", __func__);
	
	return fd;
}

int module_start(SceSize args, void* argp)
{
	int fd;
	u8 header[40];
	u32 offset_at_0xc = 0;
	u32 offset_at_0x24 = 0;
	SceModule2 *mod;
	u32 text_addr;
	int (*SysMemUserForUser_315AD3A0)(u32 fw_version);

	printk_init();

	printk("%s: open %s\n", __func__, sceKernelInitFileName());
	fd = sceIoOpen(sceKernelInitFileName(), PSP_O_RDONLY, 0);

	if (fd < 0) {
		printk("%s: sceIoOpen returns 0x%08X\n", __func__, fd);

		return 1;
	}

	sceIoRead(fd, header, sizeof(header));
	offset_at_0xc = *(u32*)(header+0x0c);
	offset_at_0x24 = *(u32*)(header+0x24);
	sceIoLseek(fd, offset_at_0x24, PSP_SEEK_SET);
	sceIoRead(fd, header, sizeof(header));

	if (memcmp(header, "PSTITLE", 7)) {
		// loc_00000688
		printk("%s: header doesn't match PSTITILE\n", __func__);
		sceIoLseek(fd, offset_at_0x24 + 0x400, PSP_SEEK_SET);
		g_data_4308 = 1;
	} else {
		printk("%s: header match PSTITILE\n", __func__);
		sceIoLseek(fd, offset_at_0x24 + 0x200, PSP_SEEK_SET);
	}

	sceIoRead(fd, header, sizeof(header));

	if (*(u32*)header == 0x44475000) { //PDG
		printk("%s: PDG flag found, patching scePops_Manager\n", __func__);
		sceIoClose(fd);
		mod = (SceModule2*) sceKernelFindModuleByName("scePops_Manager");
		text_addr = mod->text_addr;
		_sw(0, text_addr+0x158);
		sync_cache();

		printk("%s: exit 1\n", __func__);

		return 1;
	}

	sceIoLseek(fd, offset_at_0xc, PSP_SEEK_SET);
	sceIoRead(fd, header, sizeof(header));

	if (*(u32*)header == 0x474E5089 && // PNG
			*(u32*)(header+4) == 0xA1A0A0D && // PNG
			*(u32*)(header+0xc) == 0x52444849 && // IHDR
			*(u32*)(header+0x10) == 0x50000000 && // 
			*(u32*)(header+0x14) == *(u32*)(header+0x10)
	   ) {
		printk("%s: PNG file found\n", __func__);
		g_missing_PNG_icon = 0;
	}
	
	// loc_53c
	sceIoClose(fd);
	mod = (SceModule2*) sceKernelFindModuleByName("scePops_Manager");
	text_addr = mod->text_addr;
	_sw(0, text_addr + 0x158);
	g_previous = sctrlHENSetStartModuleHandler(&sub_750);
	SysMemUserForUser_315AD3A0 = (void*)sctrlHENFindFunction("sceSystemMemoryManager", "SysMemUserForUser", 0x315AD3A0);

	if (!SysMemUserForUser_315AD3A0) {
		sub_2E4(0x80000001);
	}

	//loc_59C
	SysMemUserForUser_315AD3A0(0x03090010);

	printk("%s: patching scePops_Manager\n", __func__);

	_sw(0, text_addr+0x158C); // 0x1EA8 (6.31)

	_sw(MAKE_JUMP(&sub_358), text_addr+0x162C); // 0x1F80 (6.31)
	_sw(0, text_addr+0x1630); // 0x1F84 (6.31)

	// In 6.31 popsman.prx has more calls to sceIoIoctl, needs more patch
	// List of using sceIoIoctl DRM functions:
	// sceMeAudio_9EA8B21A sceMeAudio_F8FD8B48 sceIoReadPatched0000D64
	// sceMeAudio_B213763F sceIoReadPatched0001AEC
	// Should I use sceIoIoctl NODRM patch?
	_sw(MAKE_JUMP(&sub_19C), text_addr+0x6A8); // for sceIoIoctl
	_sw(0, text_addr+0x6AC);

	_sw(MAKE_JUMP(&sub_19C), text_addr+0xCA0);
	_sw(0, text_addr+0xCA4);
	
	_sw(0, text_addr+0x378);  // 0x564 (6.31)
	_sw(0, text_addr+0xA80);  // 0x126C (6.31)
	_sw(0, text_addr+0xE2C);  // 0x14B0 (6.31)
	_sw(0, text_addr+0xECC);  // 0x15B8 (6.31)
	_sw(0, text_addr+0x1100); // 0x1780 (6.31)
	_sw(0, text_addr+0x1354); // 0x1CCC (6.31)
	_sw(MAKE_JUMP(&sceIoReadPatched), text_addr+0x2788); // sceIoRead 0x3C88 (6.31)
	_sw(0, text_addr+0x278C); // 0x3C8C (6.31)

	// sceIoReadAsync (6.31)???
	sync_cache();

	return 0;
}

int module_stop(SceSize args, void *argp)
{
	return 0;
}
