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

PSP_MODULE_INFO("M33PopcornManager", 0x1007, 1, 1);

#define PGD_ID "XX0000-XXXX00000_00-XXXXXXXXXX000XXX"
#define ACT_DAT "flash2:/act.dat"

#define RIF_MAGIC_FD 0x10000
#define ACT_DAT_FD 0x10001

static u32 g_keys_bin_found;
static u32 g_is_custom_ps1;

static int myIoRead(int fd, u8 *buf, int size)
{
	int ret;
	u32 pos;

	if(g_keys_bin_found) {
		if(fd == RIF_MAGIC_FD) {
			size = 152;
			printk("%s: fake rif content %d\n", __func__, size);
			memset(buf, 0, size);
			strcpy((char*)(buf+0x10), PGD_ID);

			return size;
		} else if (fd == ACT_DAT_FD) {
			printk("%s: fake act.dat content %d\n", __func__, size);
			memset(buf, 0, size);

			return size;
		}
	}
	
	pos = sceIoLseek32(fd, 0, SEEK_CUR);
	ret = sceIoRead(fd, buf, size);
	printk("%s: fd=0x%08X pos=0x%08X size=%d -> 0x%08X\n", __func__, fd, pos, size, ret);

	return ret;
}

static int myIoClose(SceUID fd)
{
	int ret;

	if(g_keys_bin_found) {
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

static int myIoOpen(const char *file, int flag, int mode)
{
	int ret;

	if(g_keys_bin_found) {
		if(strstr(file, PGD_ID)) {
			printk("%s: [FAKE]\n", __func__);
			ret = RIF_MAGIC_FD;
		} else if (0 == strcmp(file, ACT_DAT)) {
			printk("%s: [FAKE]\n", __func__);
			ret = ACT_DAT_FD;
		} else {
			ret = sceIoOpen(file, flag, mode);
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

static int myIoGetstat(const char *path, SceIoStat *stat)
{
	int ret;

	if(g_keys_bin_found) {
		if(strstr(path, PGD_ID)) {
			ret = 0;
			printk("%s: [FAKE]\n", __func__);
		} else if (0 == strcmp(path, ACT_DAT)) {
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

	if(g_keys_bin_found) {
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

	get_keypath(keypath, sizeof(keypath));

	if (result == 0) {
		ret = save_key(keypath, key, 16);
		printk("%s: save_key -> %d\n", __func__, ret);
	} else {
		ret = load_key(keypath, key, 16);
		printk("%s: load_key[0x%08X] -> %d\n", __func__, result, ret);

		if(ret == 0) {
			result = 0;
		}
	}
	
	return result;
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

	_sw(MAKE_JUMP(&myIoGetstat), text_addr+0x00003BD0);
	_sw(NOP, text_addr+0x00003BD4);

	_sw(MAKE_JUMP(&myIoClose), text_addr+0x00003BC0);
	_sw(NOP, text_addr+0x00003BC4);

	_get_rif_path = (void*)(text_addr+0x00000190);
	_sw(MAKE_CALL(&get_rif_path), text_addr+0x00002798);
	_sw(MAKE_CALL(&get_rif_path), text_addr+0x00002C58);

	sceNpDrmGetVersionKey = (void*)sctrlHENFindFunction("scePspNpDrm_Driver", "scePspNpDrm_driver", 0x0F9547E6);
	_sw(MAKE_CALL(_sceNpDrmGetVersionKey), text_addr+0x000029C4);
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
		result = 1;
	}

exit:
	if(fd >= 0) {
		sceIoClose(fd);
	}

	return result;
}

int module_start(SceSize args, void* argp)
{
	char keypath[128];
	int ret;
	SceIoStat stat;

	printk_init("ms0:/popcorn.txt");
	printk("Popcorn: init_file = %s\n", sceKernelInitFileName());
	patch_scePops_Manager();
	sync_cache();

	get_keypath(keypath, sizeof(keypath));
	ret = sceIoGetstat(keypath, &stat);

	if(ret == 0) {
		printk("keys.bin found\n");
		g_keys_bin_found = 1;
	} else {
		g_keys_bin_found = 0;
	}

	g_is_custom_ps1 = is_custom_ps1();

	if(g_is_custom_ps1) {
		printk("custom pops found\n");
	}

	return 0;
}

int module_stop(SceSize args, void *argp)
{
	return 0;
}
