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
#include "strsafe.h"
#include "libs.h"
#include "popcorn_patch_offset.h"

enum {
	ICON0_OK = 0,
	ICON0_MISSING = 1,
	ICON0_CORRUPTED = 2,
};

PSP_MODULE_INFO("M33PopcornManager", 0x1007, 1, 1);

#define PGD_ID "XX0000-XXXX00000_00-XXXXXXXXXX000XXX"
#define ACT_DAT "flash2:/act.dat"

#define RIF_MAGIC_FD 0x10000
#define ACT_DAT_FD 0x10001

extern u8 g_icon_png[0x3730];

static u32 g_icon0_status;
static u32 g_keys_bin_found;
static u32 g_is_custom_ps1;

static STMOD_HANDLER g_previous = NULL;

static u8 g_keys[16];

u32 psp_fw_version = 0;
u32 psp_model = 0;

static int myIoRead(int fd, u8 *buf, int size)
{
	int ret;
	u32 pos;

	if(fd != RIF_MAGIC_FD && fd != ACT_DAT_FD) {
		pos = sceIoLseek32(fd, 0, SEEK_CUR);
	} else {
		pos = 0;
	}
	
	if(g_keys_bin_found || g_is_custom_ps1) {
		if(fd == RIF_MAGIC_FD) {
			size = 152;
			printk("%s: fake rif content %d\n", __func__, size);
			memset(buf, 0, size);
			strcpy((char*)(buf+0x10), PGD_ID);
			ret = size;
			goto exit;
		} else if (fd == ACT_DAT_FD) {
			printk("%s: fake act.dat content %d\n", __func__, size);
			memset(buf, 0, size);
			ret = size;
			goto exit;
		}
	}
	
	ret = sceIoRead(fd, buf, size);

	if(ret != size) {
		goto exit;
	}

	if (size == 4) {
		u32 magic;

		magic = 0x464C457F; // ~ELF

		if(0 == memcmp(buf, &magic, sizeof(magic))) {
			magic = 0x5053507E; // ~PSP
			memcpy(buf, &magic, sizeof(magic));
			printk("%s: patch ~ELF -> ~PSP\n", __func__);
		}

		ret = size;
		goto exit;
	}
	
	if(size == sizeof(g_icon_png)) {
		u32 png_signature = 0x474E5089;

		if(g_icon0_status == ICON0_MISSING || ((g_icon0_status == ICON0_CORRUPTED) && 0 == memcmp(buf, &png_signature, 4))) {
			printk("%s: fakes a PNG for icon0\n", __func__);
			memcpy(buf, g_icon_png, size);

			ret = size;
			goto exit;
		}
	}

	if (g_is_custom_ps1 && size >= 0x420 && buf[0x41B] == 0x27 &&
			buf[0x41C] == 0x19 &&
			buf[0x41D] == 0x22 &&
			buf[0x41E] == 0x41 &&
			buf[0x41A] == buf[0x41F]) {
		buf[0x41B] = 0x55;
		printk("%s: unknown patch loc_6c\n", __func__);
	}

exit:
	printk("%s: fd=0x%08X pos=0x%08X size=%d -> 0x%08X\n", __func__, fd, pos, size, ret);

	return ret;
}

static int myIoReadAsync(int fd, u8 *buf, int size)
{
	int ret;
	u32 pos;

	pos = sceIoLseek32(fd, 0, SEEK_CUR);
	ret = sceIoReadAsync(fd, buf, size);
	printk("%s: 0x%08X 0x%08X 0x%08X -> 0x%08X\n", __func__, fd, pos, size, ret);

	return ret;
}

static SceOff myIoLseek(SceUID fd, SceOff offset, int whence)
{
	SceOff ret;

	if(g_keys_bin_found || g_is_custom_ps1) {
		if (fd == RIF_MAGIC_FD) {
			printk("%s: [FAKE]\n", __func__);
			ret = 0;
		} else if (fd == ACT_DAT_FD) {
			printk("%s: [FAKE]\n", __func__);
			ret = 0;
		} else {
			ret = sceIoLseek(fd, offset, whence);
		}
	} else {
		ret = sceIoLseek(fd, offset, whence);
	}

	printk("%s: 0x%08X 0x%08X 0x%08X -> 0x%08X\n", __func__, fd, (u32)offset, whence, (u32)ret);

	return ret;
}

static int myIoClose(SceUID fd)
{
	int ret;

	if(g_keys_bin_found || g_is_custom_ps1) {
		if (fd == RIF_MAGIC_FD) {
			printk("%s: [FAKE]\n", __func__);
			ret = 0;
		} else if (fd == ACT_DAT_FD) {
			printk("%s: [FAKE]\n", __func__);
			ret = 0;
		} else {
			ret = sceIoClose(fd);
		}
	} else {
		ret = sceIoClose(fd);
	}

	printk("%s: 0x%08X -> 0x%08X\n", __func__, fd, ret);

	return ret;
}

static int check_file_is_decrypted(const char *filename)
{
	SceUID fd = -1;
	u32 k1;
	int result = 0, ret;
	u8 buf[16] __attribute__((aligned(64)));

	k1 = pspSdkSetK1(0);
	fd = sceIoOpen(filename, PSP_O_RDONLY, 0777);

	if(fd < 0) {
		goto exit;
	}

	ret = sceIoRead(fd, buf, sizeof(buf));

	if(ret != sizeof(buf)) {
		goto exit;
	}

	if (*(u32*)buf == 0x44475000) { // PGD
		goto exit;
	}

	result = 1;

exit:
	if(fd >= 0) {
		sceIoClose(fd);
	}

	pspSdkSetK1(k1);

	return result;
}

static int myIoOpen(const char *file, int flag, int mode)
{
	int ret;

	if(g_keys_bin_found || g_is_custom_ps1) {
		if(strstr(file, PGD_ID)) {
			printk("%s: [FAKE]\n", __func__);
			ret = RIF_MAGIC_FD;
		} else if (0 == strcmp(file, ACT_DAT)) {
			printk("%s: [FAKE]\n", __func__);
			ret = ACT_DAT_FD;
		} else {
			if(g_is_custom_ps1 && check_file_is_decrypted(file)) {
				printk("%s: removed PGD open flag\n", __func__);
				ret = sceIoOpen(file, flag & ~0x40000000, mode);
			} else {
				ret = sceIoOpen(file, flag, mode);
			}
		}		
	} else {
		ret = sceIoOpen(file, flag, mode);
	}

	printk("%s: %s 0x%08X -> 0x%08X\n", __func__, file, flag, ret);

	return ret;
}

static int myIoIoctl(SceUID fd, unsigned int cmd, void * indata, int inlen, void * outdata, int outlen)
{
	int ret;

	if(cmd == 0x04100001) {
		printk("%s: setting PGD key\n", __func__);
		hexdump(indata, inlen);
	}

	if(cmd == 0x04100002) {
		printk("%s: setting PGD offset: 0x%08X\n", __func__, *(u32*)indata);
	}

	if (g_is_custom_ps1) {
		if (cmd == 0x04100001) {
			ret = 0;
			printk("%s: [FAKE] 0x%08X 0x%08X -> 0x%08X\n", __func__, fd, cmd, ret);
			goto exit;
		}

		if (cmd == 0x04100002) {
			ret = sceIoLseek32(fd, *(u32*)indata, PSP_SEEK_SET);

			if(ret < 0) {
				printk("%s: sceIoLseek32 -> 0x%08X\n", __func__, ret);
			}

			ret = 0;
			printk("%s: [FAKE] 0x%08X 0x%08X -> 0x%08X\n", __func__, fd, cmd, ret);
			goto exit;
		}
	}

	ret = sceIoIoctl(fd, cmd, indata, inlen, outdata, outlen);

exit:
	printk("%s: 0x%08X -> 0x%08X\n", __func__, fd, ret);

	return ret;
}

static int myIoGetstat(const char *path, SceIoStat *stat)
{
	int ret;

	if(g_keys_bin_found || g_is_custom_ps1) {
		if(strstr(path, PGD_ID)) {
			stat->st_mode = 0x21FF;
			stat->st_attr = 0x20;
			stat->st_size = 152;
			ret = 0;
			printk("%s: [FAKE]\n", __func__);
		} else if (0 == strcmp(path, ACT_DAT)) {
			stat->st_mode = 0x21FF;
			stat->st_attr = 0x20;
			stat->st_size = 4152;
			ret = 0;
			printk("%s: [FAKE]\n", __func__);
		} else {
			ret = sceIoGetstat(path, stat);
		}
	} else {
		ret = sceIoGetstat(path, stat);
	}

	printk("%s: %s -> 0x%08X\n", __func__, path, ret);

	return ret;
}

static int (*_get_rif_path)(const char *name, char *path) = NULL;

static int get_rif_path(char *name, char *path)
{
	int ret;

	if(g_keys_bin_found || g_is_custom_ps1) {
		strcpy(name, PGD_ID);
	}

	ret = (*_get_rif_path)(name, path);
	printk("%s: %s %s -> 0x%08X\n", __func__, name, path, ret);

	return ret;
}

static int get_keypath(char *keypath, int size)
{
	char *p;

	strcpy_s(keypath, size, sceKernelInitFileName());
	p = strrchr(keypath, '/');

	if(p == NULL) {
		return -1;
	}

	p[1] = '\0';
	strcat_s(keypath, size, "KEYS.BIN");

	return 0;
}

static int save_key(const char *keypath, u8 *key, int size)
{
	SceUID keys;
	int ret;

	keys = sceIoOpen(keypath, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);

	if (keys < 0) {
		return -1;
	}

	ret = sceIoWrite(keys, key, size);

	if(ret == size) {
		ret = 0;
	} else {
		ret = -2;
	}

	sceIoClose(keys);

	return ret;
}

static int load_key(const char *keypath, u8 *key, int size)
{
	SceUID keys; 
	int ret;

	keys = sceIoOpen(keypath, PSP_O_RDONLY, 0777);

	if (keys < 0) {
		printk("%s: sceIoOpen %s -> 0x%08X\n", __func__, keypath, keys);

		return -1;
	}

	ret = sceIoRead(keys, key, size); 

	if (ret == size) {
		ret = 0;
	} else {
		ret = -2;
	}

	sceIoClose(keys);

	return ret;
}

static int (*sceNpDrmGetVersionKey)(u8 * key, u8 * act, u8 * rif, u32 flags);

static int _sceNpDrmGetVersionKey(u8 * key, u8 * act, u8 * rif, u32 flags)
{
	char keypath[128];
	int ret, result;
   
	result = (*sceNpDrmGetVersionKey)(key, act, rif, flags);

	if (g_is_custom_ps1) {
		printk("%s: -> 0x%08X\n", __func__, result);
		result = 0;

		if (g_keys_bin_found) {
			memcpy(key, g_keys, sizeof(g_keys));
		}
		
		printk("%s:[FAKE] -> 0x%08X\n", __func__, result);
	} else {
		get_keypath(keypath, sizeof(keypath));

		if (result == 0) {
			memcpy(g_keys, key, sizeof(g_keys));
			ret = save_key(keypath, g_keys, sizeof(g_keys));
			printk("%s: save_key -> %d\n", __func__, ret);
		} else {
			if (g_keys_bin_found) {
				memcpy(key, g_keys, sizeof(g_keys));
				result = 0;
			}
		}
	}
	
	return result;
}

static int (*scePspNpDrm_driver_9A34AC9F)(u8 *rif);

static int _scePspNpDrm_driver_9A34AC9F(u8 *rif)
{
	int result;

	result = (*scePspNpDrm_driver_9A34AC9F)(rif);
	printk("%s: 0x%08X -> 0x%08X\n", __func__, (u32)rif, result);

	if (result != 0) {
		if (g_keys_bin_found || g_is_custom_ps1) {
			result = 0;
			printk("%s:[FAKE] -> 0x%08X\n", __func__, result);
		}
	}

	return result;
}

static int _sceDrmBBCipherUpdate(void *ckey, u8 *data, int size)
{
	return 0;
}

static int _sceDrmBBCipherInit(void *ckey, int type, int mode, u8 *header_key, u8 *version_key, u32 seed)
{
	return 0;
}

static int _sceDrmBBMacInit(void *mkey, int type)
{
	return 0;
}

static int _sceDrmBBMacUpdate(void *mkey, u8 *buf, int size)
{
	return 0;
}

static int _sceDrmBBCipherFinal(void *ckey)
{
	return 0;
}

static int _sceDrmBBMacFinal(void *mkey, u8 *buf, u8 *vkey)
{
	return 0;
}

static int _sceDrmBBMacFinal2(void *mkey, u8 *out, u8 *vkey)
{
	return 0;
}

static void patch_scePops_Manager(void)
{
	SceModule2 *mod;
	u32 text_addr;

	mod = (SceModule2*) sceKernelFindModuleByName("scePops_Manager");
	text_addr = mod->text_addr;

	REDIRECT_FUNCTION(myIoOpen, text_addr + g_offs->popsman_patch.sceIoOpenImport);
	REDIRECT_FUNCTION(myIoLseek, text_addr + g_offs->popsman_patch.sceIoLseekImport);
	REDIRECT_FUNCTION(myIoIoctl, text_addr + g_offs->popsman_patch.sceIoIoctlImport);
	REDIRECT_FUNCTION(myIoRead, text_addr + g_offs->popsman_patch.sceIoReadImport);
	REDIRECT_FUNCTION(myIoReadAsync, text_addr + g_offs->popsman_patch.sceIoReadAsyncImport);
	REDIRECT_FUNCTION(myIoGetstat, text_addr + g_offs->popsman_patch.sceIoGetstatImport);
	REDIRECT_FUNCTION(myIoClose, text_addr + g_offs->popsman_patch.sceIoCloseImport);

	_get_rif_path = (void*)(text_addr + g_offs->popsman_patch.get_rif_path);
	_sw(MAKE_CALL(&get_rif_path), text_addr + g_offs->popsman_patch.get_rif_path_call1);
	_sw(MAKE_CALL(&get_rif_path), text_addr + g_offs->popsman_patch.get_rif_path_call2);

	sceNpDrmGetVersionKey = (void*)sctrlHENFindFunction("scePspNpDrm_Driver", "scePspNpDrm_driver", 0x0F9547E6);
	scePspNpDrm_driver_9A34AC9F = (void*)sctrlHENFindFunction("scePspNpDrm_Driver", "scePspNpDrm_driver", 0x9A34AC9F);

	_sw(MAKE_CALL(_sceNpDrmGetVersionKey), text_addr + g_offs->popsman_patch.sceNpDrmGetVersionKeyCall);
	_sw(MAKE_CALL(_scePspNpDrm_driver_9A34AC9F), text_addr + g_offs->popsman_patch.scePspNpDrm_driver_9A34AC9F_Call);

	// remove the check in scePopsManLoadModule that only allows loading module below the FW 3.XX
	_sw(NOP, text_addr + g_offs->popsman_patch.scePopsManLoadModuleCheck);

	if (g_is_custom_ps1) {
		REDIRECT_FUNCTION(_sceDrmBBCipherInit, text_addr + g_offs->popsman_patch.sceDrmBBCipherInitImport);
		REDIRECT_FUNCTION(_sceDrmBBCipherUpdate, text_addr + g_offs->popsman_patch.sceDrmBBCipherUpdateImport);
		REDIRECT_FUNCTION(_sceDrmBBCipherFinal, text_addr + g_offs->popsman_patch.sceDrmBBCipherFinalImport);

		REDIRECT_FUNCTION(_sceDrmBBMacInit, text_addr + g_offs->popsman_patch.sceDrmBBMacInitImport);
		REDIRECT_FUNCTION(_sceDrmBBMacUpdate, text_addr + g_offs->popsman_patch.sceDrmBBMacUpdateImport);
		REDIRECT_FUNCTION(_sceDrmBBMacFinal, text_addr + g_offs->popsman_patch.sceDrmBBMacFinalImport);
		REDIRECT_FUNCTION(_sceDrmBBMacFinal2, text_addr + g_offs->popsman_patch.sceDrmBBMacFinal2Import);
	}
}

static u32 is_custom_ps1(void)
{
	SceUID fd = -1;
	const char *filename;
	int result, ret;
	u32 psar_offset, pgd_offset;
	u8 header[40] __attribute__((aligned(64)));

	filename = sceKernelInitFileName();
	result = 0;

	if(filename == NULL) {
		result = 0;
		goto exit;
	}

	fd = sceIoOpen(filename, PSP_O_RDONLY, 0777);

	if(fd < 0) {
		printk("%s: sceIoOpen %s -> 0x%08X\n", __func__, filename, fd);
		result = 0;
		goto exit;
	}

	ret = sceIoRead(fd, header, sizeof(header));

	if(ret != sizeof(header)) {
		printk("%s: sceIoRead -> 0x%08X\n", __func__, ret);
		result = 0;
		goto exit;
	}

	psar_offset = *(u32*)(header+0x24);
	sceIoLseek32(fd, psar_offset, PSP_SEEK_SET);
	ret = sceIoRead(fd, header, sizeof(header));

	if(ret != sizeof(header)) {
		printk("%s: sceIoRead -> 0x%08X\n", __func__, ret);
		result = 0;
		goto exit;
	}

	pgd_offset = psar_offset;

	if(0 == memcmp(header, "PSTITLE", sizeof("PSTITLE")-1)) {
		pgd_offset += 0x200;
	} else {
		pgd_offset += 0x400;
	}

	sceIoLseek32(fd, pgd_offset, PSP_SEEK_SET);
	ret = sceIoRead(fd, header, 4);

	if(ret != 4) {
		printk("%s: sceIoRead -> 0x%08X\n", __func__, ret);
		result = 0;
		goto exit;
	}

	// PGD offset
	if(*(u32*)header != 0x44475000) {
		printk("%s: custom pops found\n", __func__);
		result = 1;
	}

exit:
	if(fd >= 0) {
		sceIoClose(fd);
	}

	return result;
}

static int place_syscall_stub(void* func, void *addr)
{
	u32 syscall_num;
	extern u32 sceKernelQuerySystemCall(void *func);

	syscall_num = sceKernelQuerySystemCall(func);

	if(syscall_num == (u32)-1) {
		return -1;
	}

	_sw(0x03E00008, (u32)addr);
	_sw(((syscall_num<<6)|12), (u32)(addr+4));

	return 0;
}

static void reboot_vsh_with_error(u32 error)
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
	printk("%s: 0x%08X 0x%08X 0x%08X -> 0x%08X\n", __func__, destSize, (u32)src, (u32)dest, ret);

	if (ret == 0x9300) {
		ret = 0x92FF;
		printk("%s: [FAKE] -> 0x%08X\n", __func__, ret);
	}

	pspSdkSetK1(k1);

	return ret;
}

static int patch_decompress_data(u32 text_addr)
{
	int ret;
	void *stub_addr, *patch_addr;

	stub_addr = (void*)(text_addr + g_offs->pops_patch.decomp[psp_model].stub_offset);
	patch_addr = (void*)(text_addr + g_offs->pops_patch.decomp[psp_model].patch_offset);
	ret = place_syscall_stub(decompress_data, stub_addr);

	if (ret != 0) {
		printk("%s: place_syscall_stub -> 0x%08X\n", __func__, ret);

		return -1;
	}

	_sw(MAKE_CALL(stub_addr), (u32)patch_addr);

	return 0;
}

static void patch_icon0_size(u32 text_addr)
{
	u32 patch_addr;
	
	patch_addr = text_addr + g_offs->pops_patch.ICON0SizeOffset[psp_model];
	_sw(0x24050000 | (sizeof(g_icon_png) & 0xFFFF), patch_addr);
}

static int (*sceMeAudio_67CD7972)(void *buf, int size);

int _sceMeAudio_67CD7972(void *buf, int size)
{
	int ret;
	u32 k1;

	k1 = pspSdkSetK1(0);
	ret = (*sceMeAudio_67CD7972)(buf, size);
	pspSdkSetK1(k1);

	printk("%s: 0x%08X -> 0x%08X\n", __func__, size, ret);

	return ret;
}

static int popcorn_patch_chain(SceModule2 *mod)
{
	printk("%s: %s\n", __func__, mod->modname);

	if (0 == strcmp(mod->modname, "pops")) {
		u32 text_addr = mod->text_addr;

		printk("%s: patching pops\n", __func__);

		if(g_is_custom_ps1) {
			patch_decompress_data(text_addr);
		}

		if(g_icon0_status) {
			patch_icon0_size(text_addr);
		}

		sceMeAudio_67CD7972 = (void*)sctrlHENFindFunction("scePops_Manager", "sceMeAudio", g_offs->pops_patch.sceMeAudio_67CD7972_NID);
		hook_import_bynid((SceModule*)mod, "sceMeAudio", g_offs->pops_patch.sceMeAudio_67CD7972_NID, _sceMeAudio_67CD7972, 1);
		_sw(0x24020001, text_addr + g_offs->pops_patch.manualNameCheck[psp_model]);

		sync_cache();
	}

	if(g_previous)
		return g_previous(mod);
	
	return 0;
}

static int get_icon0_status(void)
{
	u32 icon0_offset = 0;
	int result = ICON0_MISSING;
	SceUID fd = -1;;
	const char *filename;
	u8 header[40] __attribute__((aligned(64)));
	
	filename = sceKernelInitFileName();

	if(filename == NULL) {
		goto exit;
	}
	
	fd = sceIoOpen(filename, PSP_O_RDONLY, 0777);

	if(fd < 0) {
		printk("%s: sceIoOpen %s -> 0x%08X\n", __func__, filename, fd);
		goto exit;
	}
	
	sceIoRead(fd, header, sizeof(header));
	icon0_offset = *(u32*)(header+0x0c);
	sceIoLseek32(fd, icon0_offset, PSP_SEEK_SET);
	sceIoRead(fd, header, sizeof(header));

	if(*(u32*)(header+4) == 0xA1A0A0D) {
		if ( *(u32*)(header+0xc) == 0x52444849 && // IHDR
				*(u32*)(header+0x10) == 0x50000000 && // 
				*(u32*)(header+0x14) == *(u32*)(header+0x10)
		   ) {
			result = ICON0_OK;
		} else {
			result = ICON0_CORRUPTED;
		}
	} else {
		result = ICON0_MISSING;
	}

	printk("%s: PNG file status -> %d\n", __func__, result);

exit:
	if(fd >= 0) {
		sceIoClose(fd);
	}

	return result;
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
	char keypath[128];
	int ret;
	SceIoStat stat;

	psp_fw_version = sceKernelDevkitVersion();
	setup_patch_offset_table(psp_fw_version);
	psp_model = sceKernelGetModel();
	printk_init("ms0:/popcorn.txt");
	printk("Popcorn: init_file = %s psp_fw_version = 0x%08X psp_model = %d\n", sceKernelInitFileName(), psp_fw_version, psp_model);

	get_keypath(keypath, sizeof(keypath));
	ret = sceIoGetstat(keypath, &stat);
	g_keys_bin_found = 0;

	if(ret == 0) {
		ret = load_key(keypath, g_keys, sizeof(g_keys));

		if(ret == 0) {
			g_keys_bin_found = 1;
			printk("keys.bin found\n");
		}
	}

	g_is_custom_ps1 = is_custom_ps1();
	g_icon0_status = get_icon0_status();

	if(g_is_custom_ps1) {
		setup_psx_fw_version(psp_fw_version);
	}

	g_previous = sctrlHENSetStartModuleHandler(&popcorn_patch_chain);
	patch_scePops_Manager();
	sync_cache();
	
	return 0;
}

int module_stop(SceSize args, void *argp)
{
	return 0;
}
