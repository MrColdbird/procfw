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

#include <pspsdk.h>
#include <pspiofilemgr_kernel.h>
#include <psploadcore.h>
#include <pspsysmem_kernel.h>
#include <stdio.h>
#include <string.h>
#include "printk.h"
#include "systemctrl.h"
#include "systemctrl_private.h"
#include "strsafe.h"
#include "libs.h"
#include "utils.h"

typedef struct _NoDrmHookEntry {
	char *libname;
	u32 nid;
	void *hook_addr;
} NoDrmHookEntry;

struct NoDrmFd {
	SceUID fd;
	struct NoDrmFd *next;
};

static struct NoDrmFd g_head, *g_tail = &g_head;

// found in KHBBS
static u8 g_drm_magic_1[8] = {
	0x00, 0x50, 0x53, 0x50, 0x45, 0x44, 0x41, 0x54 // PSPEDATA
};

// found in Valkyrie 3
static u8 g_drm_magic_2[4] = {
	0x00, 0x50, 0x47, 0x44 // PGD
};

static int (*_sceNpDrmRenameCheck)(char *fn);
static int (*_sceNpDrmEdataSetupKey)(int fd);
static SceOff (*_sceNpDrmEdataGetDataSize)(int fd);
static int (*_sceKernelLoadModuleNpDrm)(char *fn, int flag, void *opt);
extern int (*_sceKernelLoadModule)(char *fname, int flag, void *opt);

static SceUID g_nodrm_sema = -1;

static int check_file_is_encrypted(int fd)
{
	int ret;
	u32 k1;
	char p[8 + 64], *buf;

	k1 = pspSdkSetK1(0);
	buf = (char*)((((u32)p) & ~(64-1)) + 64);
	ret = sceIoRead(fd, buf, 8);
	pspSdkSetK1(k1);
	sceIoLseek32(fd, 0, PSP_SEEK_SET);

	if (ret != 8)
		return 0;

	if (!memcmp(buf, g_drm_magic_1, sizeof(g_drm_magic_1))) {
		return 1;
	}

	if (!memcmp(buf, g_drm_magic_2, sizeof(g_drm_magic_2))) {
		return 1;
	}

#if 0
	printk("%s: buf:\n", __func__);
	hexdump(buf, 8);
#endif

	return 0;
}

static int check_file_is_encrypted_by_path(const char* path)
{
	SceUID fd;
	u32 k1;
	int ret;

	k1 = pspSdkSetK1(0);
	fd = sceIoOpen(path, PSP_O_RDONLY, 0777);

	if (fd >= 0) {
		ret = check_file_is_encrypted(fd);
	} else {
		// assume we got a encrypted one
		ret = 1;
	}

	sceIoClose(fd);
	pspSdkSetK1(k1);

	return ret;
}

static inline void lock(void)
{
	u32 k1;

	k1 = pspSdkSetK1(0);
	sceKernelWaitSema(g_nodrm_sema, 1, 0);
	pspSdkSetK1(k1);
}

static inline void unlock(void)
{
	u32 k1;

	k1 = pspSdkSetK1(0);
	sceKernelSignalSema(g_nodrm_sema, 1);
	pspSdkSetK1(k1);
}

static inline int is_encrypted_flag(int flag)
{
	if (flag == 0x40004001 || flag == 0x40000001)
		return 1;

	return 0;
}

static inline int is_nodrm_fd(SceUID fd)
{
	struct NoDrmFd *fds;

	if (fd < 0)
		return 0;

	for(fds = g_head.next; fds != NULL; fds = fds->next) {
		if(fds->fd == fd)
			break;
	}

	if(fds == NULL)
		return 0;

	return 1;
}

static int add_nodrm_fd(SceUID fd)
{
	struct NoDrmFd *slot;

	if (fd < 0)
		return -1;

	lock();
	slot = (struct NoDrmFd*)oe_malloc(sizeof(*slot));

	if(slot == NULL) {
		unlock();

		return -2;
	}

	slot->fd = fd;
	
	g_tail->next = slot;
	g_tail = slot;
	slot->next = NULL;

	unlock();

	return slot->fd;
}

static int remove_nodrm_fd(SceUID fd)
{
	int ret;
	struct NoDrmFd *fds, *prev;

	lock();

	for(prev = &g_head, fds = g_head.next; fds != NULL; prev = fds, fds = fds->next) {
		if(fd == fds->fd) {
			break;
		}
	}

	if(fds != NULL) {
		prev->next = fds->next;

		if(g_tail == fds) {
			g_tail = prev;
		}

		oe_free(fds);
		ret = 0;
	} else {
		ret = -1;
	}

	unlock();

	return ret;
}

int myIoOpen(const char *file, int flag, int mode)
{
	int fd;
	int encrypted;

	encrypted = is_encrypted_flag(flag);

	if (encrypted) {
		fd = sceIoOpen(file, PSP_O_RDONLY, mode);

		if (fd >= 0) {
			if (check_file_is_encrypted(fd)) {
				sceIoClose(fd);
			} else {
				int ret;

				ret = add_nodrm_fd(fd); 

				if (ret < 0) {
					printk("%s: add_nodrm_fd -> %d\n", __func__, ret);
				} else {
					fd = ret; 
				}

				// pass the plain fd to user
				goto exit;
			}
		}
	}

	fd = sceIoOpen(file, flag, mode);

exit:
#ifdef DEBUG
	if (encrypted) {
		printk("%s: %s 0x%08X -> 0x%08X\n", __func__, file, flag, fd);
	}
#endif

	return fd;
}

int myIoOpenAsync(const char *file, int flag, int mode)
{
	int fd;
	int is_plain = 0, encrypted;

	encrypted = is_encrypted_flag(flag);

	if (encrypted) {
		fd = sceIoOpen(file, PSP_O_RDONLY, mode);

		if (fd >= 0) {
			if (check_file_is_encrypted(fd)) {
				printk("%s: %s is encrypted, nodrm disabled\n", __func__, file);
			} else {
				printk("%s: %s is plain, nodrm enabled\n", __func__, file);
				is_plain = 1;
			}
		}

		sceIoClose(fd);
	}

	if(is_plain) {
		fd = sceIoOpenAsync(file, PSP_O_RDONLY, mode);

		if(fd >= 0) {
			int ret;

			ret = add_nodrm_fd(fd); 

			if (ret < 0) {
				printk("%s: add_nodrm_fd -> %d\n", __func__, ret);
			} else {
				fd = ret; 
			}
		}
	} else {
		fd = sceIoOpenAsync(file, flag, mode);
	}

	// printk after sceIoOpenAsync would freeze the system

	return fd;
}

int myNpDrmRenameCheck(char *fn)
{
	int ret;
	
	// don't worry, it works without setting $k1 to 0
	if (sceKernelFindModuleByName("scePspNpDrm_Driver") == NULL) {
		ret = 0x8002013A;
		goto exit;
	}

	if(!check_memory(fn, strlen(fn) + 1)) {
		ret = 0x80550910;
		goto exit;
	}

	ret = check_file_is_encrypted_by_path(fn);

	if (ret == 0) {
		SceIoStat stat;
		u32 k1;
		
		k1 = pspSdkSetK1(0);
		ret = sceIoGetstat(fn, &stat) == 0 ? 0 : 0x80550901;
		pspSdkSetK1(k1);
	} else {
		if (_sceNpDrmRenameCheck != NULL) {
			ret = (*_sceNpDrmRenameCheck)(fn);
		} else {
			ret = 0x8002013A;
		}
	}

exit:
	printk("%s: %s -> 0x%08X\n", __func__, fn, ret);

	return ret;
}

int myNpDrmEdataSetupKey(SceUID fd)
{
	int ret;

	if (sceKernelFindModuleByName("scePspNpDrm_Driver") == NULL) {
		ret = 0x8002013A;
		goto exit;
	}

	if (is_nodrm_fd(fd)) {
		ret = 0;
	} else {
		if (_sceNpDrmEdataSetupKey != NULL) {
			ret = (*_sceNpDrmEdataSetupKey)(fd);
		} else {
			ret = 0x8002013A;
		}
	}

exit:
	printk("%s: 0x%08X -> 0x%08X\n", __func__, fd, ret);

	return ret;	
}

SceOff myNpDrmEdataGetDataSize(SceUID fd)
{
	SceOff end;

	if (sceKernelFindModuleByName("scePspNpDrm_Driver") == NULL) {
		end = 0x8002013A;
		goto exit;
	}
	
	if (is_nodrm_fd(fd)) {
		SceOff off;
	   
		off = sceIoLseek(fd, 0, PSP_SEEK_CUR);
		end = sceIoLseek(fd, 0, PSP_SEEK_END);
		sceIoLseek(fd, off, PSP_SEEK_SET);
	} else {
		if (_sceNpDrmEdataGetDataSize != NULL) {
			end = (*_sceNpDrmEdataGetDataSize)(fd);
		} else {
			end = 0x8002013A;
		}
	}

exit:
	printk("%s 0x%08X -> 0x%08X\n", __func__, fd, (uint)end);

	return end;
}

int myKernelLoadModuleNpDrm(char *fn, int flag, void *opt)
{
	int ret;

	ret = (*_sceKernelLoadModule)(fn, flag, opt);

	if (ret < 0) {
		printk("%s: load plain module %s -> 0x%08X\n", __func__, fn, ret);
	} else {
		return ret;
	}

	ret = (*_sceKernelLoadModuleNpDrm)(fn, flag, opt);
	printk("%s: load drm module %s -> 0x%08X\n", __func__, fn, ret);

	return ret;
}

int myIoIoctl(SceUID fd, unsigned int cmd, void * indata, int inlen, void * outdata, int outlen)
{
	int ret;

	if (cmd == 0x04100001 || cmd == 0x04100002) {
		if (is_nodrm_fd(fd)) {
			ret = 0;
			goto exit;
		}
	}

	ret = sceIoIoctl(fd, cmd, indata, inlen, outdata, outlen);

exit:
#ifdef DEBUG
	if (cmd == 0x04100001 || cmd == 0x04100002) {
		printk("%s: 0x%08X -> 0x%08X\n", __func__, fd, ret);
	}
#endif

	return ret;
}

int myIoIoctlAsync(SceUID fd, unsigned int cmd, void * indata, int inlen, void * outdata, int outlen)
{
	int ret;

	if (cmd == 0x04100001 || cmd == 0x04100002) {
		if (is_nodrm_fd(fd)) {
			ret = 0;
			goto exit;
		}
	} 

	ret = sceIoIoctlAsync(fd, cmd, indata, inlen, outdata, outlen);

exit:
#ifdef DEBUG
	if (cmd == 0x04100001 || cmd == 0x04100002) {
		printk("%s: 0x%08X -> 0x%08X\n", __func__, fd, ret);
	}
#endif

	return ret;
}

int myIoClose(SceUID fd)
{
	int ret;

	if (is_nodrm_fd(fd)) {
		ret = sceIoClose(fd);

		if (ret == 0) {
			ret = remove_nodrm_fd(fd);

			if (ret < 0) {
				printk("%s: remove_nodrm_fd -> %d\n", __func__, ret);
			}

			ret = 0;
		} else {
			printk("%s: sceIoClose 0x%08X -> 0x%08X\n", __func__, fd, ret);
		}
		
	} else {
		ret = sceIoClose(fd);
	}

	return ret;
}

int myIoCloseAsync(SceUID fd)
{
	int ret;

	if (is_nodrm_fd(fd)) {
		ret = sceIoCloseAsync(fd);

		if (ret == 0) {
			ret = remove_nodrm_fd(fd);

			if (ret < 0) {
				printk("%s: remove_nodrm_fd -> %d\n", __func__, ret);
			}

			ret = 0;
		} else {
			printk("%s: sceIoCloseAsync 0x%08X -> 0x%08X\n", __func__, fd, ret);
		}

//		printk("%s: 0x%08X -> 0x%08X\n", __func__, fd, ret);
	} else {
		ret = sceIoCloseAsync(fd);
	}

	return ret;
}

static NoDrmHookEntry g_nodrm_hook_map[] = {
	{ "IoFileMgrForUser", 0x109F50BC, &myIoOpen },
	{ "IoFileMgrForUser", 0x89AA9906, &myIoOpenAsync },
	{ "IoFileMgrForUser", 0x810C4BC3, &myIoClose },
	{ "IoFileMgrForUser", 0xFF5940B6, &myIoCloseAsync },
	{ "IoFileMgrForUser", 0x63632449, &myIoIoctl },
	{ "IoFileMgrForUser", 0xE95A012B, &myIoIoctlAsync },
	{ "scePspNpDrm_user", 0x275987D1, &myNpDrmRenameCheck },
	{ "scePspNpDrm_user", 0x08D98894, &myNpDrmEdataSetupKey },
	{ "scePspNpDrm_user", 0x219EF5CC, &myNpDrmEdataGetDataSize },
	{ "ModuleMgrForUser", 0xF2D8D1B4, &myKernelLoadModuleNpDrm },
};

int nodrm_get_npdrm_functions(void)
{
	_sceNpDrmRenameCheck = (void*)sctrlHENFindFunction("scePspNpDrm_Driver", "scePspNpDrm_user", 0x275987D1);
	_sceNpDrmEdataSetupKey = (void*)sctrlHENFindFunction("scePspNpDrm_Driver", "scePspNpDrm_user", 0x08D98894);
	_sceNpDrmEdataGetDataSize = (void*)sctrlHENFindFunction("scePspNpDrm_Driver", "scePspNpDrm_user", 0x219EF5CC);
	_sceKernelLoadModuleNpDrm = (void*)sctrlHENFindFunction("sceModuleManager", "ModuleMgrForUser", 0xF2D8D1B4);

	if (_sceNpDrmRenameCheck == NULL) return -1;
	if (_sceNpDrmEdataSetupKey == NULL) return -2;
	if (_sceNpDrmEdataGetDataSize == NULL) return -3;
	if (_sceKernelLoadModuleNpDrm == NULL) return -4;

	return 0;
}

int nodrm_init(void)
{
#ifdef DEBUG
	g_nodrm_sema = sceKernelCreateSema("NoDrmSema", 0, 1, 1, NULL);
#else
	g_nodrm_sema = sceKernelCreateSema("", 0, 1, 1, NULL);
#endif

	g_head.next = NULL;
	g_tail = &g_head;

	return 0;
}

void patch_drm_imports(SceModule *mod)
{
	u32 i;

	for(i=0; i<NELEMS(g_nodrm_hook_map); ++i) {
		hook_import_bynid(mod, g_nodrm_hook_map[i].libname, g_nodrm_hook_map[i].nid, g_nodrm_hook_map[i].hook_addr, 1);
	}
}
