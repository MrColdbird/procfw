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

#define NODRM_MAGIC_FD 0x9000
#define MAX_NODRM_FD 10

typedef struct _NoDrmHookEntry {
	char *libname;
	u32 nid;
	void *hook_addr;
} NoDrmHookEntry;

struct NoDrmFd {
	SceUID fd;
	SceUID real_fd;
	struct NoDrmFd *next;
};

static struct NoDrmFd *g_head = NULL, *g_tail = NULL;

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
	char buf[8] __attribute__((aligned(64)));

	k1 = pspSdkSetK1(0);
	ret = sceIoRead(fd, buf, sizeof(buf));
	pspSdkSetK1(k1);
	sceIoLseek32(fd, 0, PSP_SEEK_SET);

	if (ret != sizeof(buf))
		return 0;

	if (!memcmp(buf, g_drm_magic_1, sizeof(g_drm_magic_1))) {
		return 1;
	}

	if (!memcmp(buf, g_drm_magic_2, sizeof(g_drm_magic_2))) {
		return 1;
	}

	printk("%s: buf:\n", __func__);
	hexdump(buf, sizeof(buf));

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

static inline SceUID nodrm2real(SceUID nodrm)
{
	struct NoDrmFd *fds;

	if (nodrm < 0)
		return -1;

	for(fds = g_head; fds != NULL; fds = fds->next) {
		if(fds->fd == nodrm)
			break;
	}

	if(fds == NULL)
		return -2;

	return fds->real_fd;
}

static SceUID get_free_fd(void)
{
	struct NoDrmFd *fds;
	SceUID free_fd;

	free_fd = NODRM_MAGIC_FD;

	do {
		for(fds = g_head; fds != NULL; fds = fds->next) {
			if(fds->fd == free_fd)
				break;
		}

		if(fds != NULL) {
			free_fd++;
		}
	} while(fds != NULL);

	return free_fd;
}

static int add_nodrm_fd(SceUID real_fd)
{
	struct NoDrmFd *slot;

	if (real_fd < 0)
		return -1;

	lock();
	slot = (struct NoDrmFd*)oe_malloc(sizeof(*slot));

	if(slot == NULL) {
		unlock();

		return -2;
	}

	slot->fd = get_free_fd();
	slot->real_fd = real_fd;
	slot->next = NULL;
	
	if(g_head == NULL) {
		g_head = g_tail = slot;
	} else {
		g_tail->next = slot;
		g_tail = slot;
	}

	unlock();

	return 0;
}

static int remove_nodrm_fd(SceUID real_fd)
{
	int ret;
	struct NoDrmFd *fds, *prev;

	lock();

	for(prev = NULL, fds = g_head; fds != NULL; prev = fds, fds = fds->next) {
		if(real_fd == fds->real_fd) {
			break;
		}
	}

	if(fds != NULL) {
		if(prev == NULL) {
			g_head = fds->next;

			if(g_head == NULL) {
				g_tail = NULL;
			}
		} else {
			prev->next = fds->next;

			if(g_tail == fds) {
				g_tail = prev;
			}
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

	if (is_encrypted_flag(flag)) {
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
					fd = NODRM_MAGIC_FD + ret; 
				}

				// pass the plain fd to user
				goto exit;
			}
		}
	}

	fd = sceIoOpen(file, flag, mode);

exit:
#ifdef DEBUG
	if (is_encrypted_flag(flag)) {
		printk("%s: %s 0x%08X -> 0x%08X\n", __func__, file, flag, fd);
	}
#endif

	return fd;
}

int myIoOpenAsync(const char *file, int flag, int mode)
{
	int fd;
	int is_plain = 0;

	if (is_encrypted_flag(flag)) {
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
				fd = NODRM_MAGIC_FD + ret; 
			}
		}
	} else {
		fd = sceIoOpenAsync(file, flag, mode);
	}

#ifdef DEBUG
	if (is_encrypted_flag(flag)) {
		printk("%s: %s 0x%08X -> 0x%08X\n", __func__, file, flag, fd);
	}
#endif

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
	SceUID real_fd;
	int ret;

	if (sceKernelFindModuleByName("scePspNpDrm_Driver") == NULL) {
		ret = 0x8002013A;
		goto exit;
	}

	real_fd = nodrm2real(fd);

	if (real_fd >= 0) {
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
	u64 end;
	SceUID real_fd;

	if (sceKernelFindModuleByName("scePspNpDrm_Driver") == NULL) {
		end = 0x8002013A;
		goto exit;
	}
	
	real_fd = nodrm2real(fd);

	if (real_fd >= 0) {
		u64 off;
	   
		off = sceIoLseek(real_fd, 0, PSP_SEEK_CUR);
		end = sceIoLseek(real_fd, 0, PSP_SEEK_END);
		sceIoLseek(real_fd, off, PSP_SEEK_SET);
	} else {
		if (_sceNpDrmEdataGetDataSize != NULL) {
			end = (*_sceNpDrmEdataGetDataSize)(fd);
		} else {
			end = 0x8002013A;
		}
	}

exit:
	printk("%s 0x%08X -> 0x%08X\n", __func__, fd, (u32)end);

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
	SceUID real_fd;

	if (cmd == 0x04100001 || cmd == 0x04100002) {
		real_fd = nodrm2real(fd);

		if (real_fd >= 0) {
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
	SceUID real_fd;

	if (cmd == 0x04100001 || cmd == 0x04100002) {
		real_fd = nodrm2real(fd);

		if (real_fd >= 0) {
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
	SceUID real_fd;
	int ret;

	real_fd = nodrm2real(fd);

	if (real_fd >= 0) {
		ret = sceIoClose(real_fd);

		if (ret == 0) {
			ret = remove_nodrm_fd(real_fd);

			if (ret < 0) {
				printk("%s: remove_nodrm_fd -> %d\n", __func__, ret);
			}

			ret = 0;
		} else {
			printk("%s: sceIoClose 0x%08X -> 0x%08X\n", __func__, real_fd, ret);
		}
		
	} else {
		ret = sceIoClose(fd);
	}

	return ret;
}

int myIoCloseAsync(SceUID fd)
{
	SceUID real_fd;
	int ret;

	real_fd = nodrm2real(fd);

	if (real_fd >= 0) {
		ret = sceIoCloseAsync(real_fd);

		if (ret == 0) {
			ret = remove_nodrm_fd(real_fd);

			if (ret < 0) {
				printk("%s: remove_nodrm_fd -> %d\n", __func__, ret);
			}

			ret = 0;
		} else {
			printk("%s: sceIoCloseAsync 0x%08X -> 0x%08X\n", __func__, real_fd, ret);
		}

//		printk("%s: 0x%08X -> 0x%08X\n", __func__, fd, ret);
	} else {
		ret = sceIoCloseAsync(fd);
	}

	return ret;
}

int myIoRead(SceUID fd, void *data, SceSize size)
{
	int ret;

	fd = (nodrm2real(fd) >= 0) ? nodrm2real(fd) : fd;
	ret = sceIoRead(fd, data, size);

	return ret;
}

int myIoReadAsync(SceUID fd, void *data, SceSize size)
{
	int ret;

	fd = (nodrm2real(fd) >= 0) ? nodrm2real(fd) : fd;
	ret = sceIoReadAsync(fd, data, size);

	return ret;
}

int myIoWrite(SceUID fd, void *data, SceSize size)
{
	int ret;

	fd = (nodrm2real(fd) >= 0) ? nodrm2real(fd) : fd;
	ret = sceIoWrite(fd, data, size);

	return ret;
}

int myIoWriteAsync(SceUID fd, void *data, SceSize size)
{
	int ret;

	fd = (nodrm2real(fd) >= 0) ? nodrm2real(fd) : fd;
	ret = sceIoWriteAsync(fd, data, size);

	return ret;
}

SceOff myIoLseek(SceUID fd, SceOff offset, int whence)
{
	SceOff ret;

	fd = (nodrm2real(fd) >= 0) ? nodrm2real(fd) : fd;
	ret = sceIoLseek(fd, offset, whence);

	return ret;
}

SceOff myIoLseekAsync(SceUID fd, SceOff offset, int whence)
{
	SceOff ret;

	fd = (nodrm2real(fd) >= 0) ? nodrm2real(fd) : fd;
	ret = sceIoLseekAsync(fd, offset, whence);

	return ret;
}

int myIoLseek32(SceUID fd, int offset, int whence)
{
	SceOff ret;

	fd = (nodrm2real(fd) >= 0) ? nodrm2real(fd) : fd;
	ret = sceIoLseek32(fd, offset, whence);

	return ret;
}

int myIoLseek32Async(SceUID fd, int offset, int whence)
{
	SceOff ret;

	fd = (nodrm2real(fd) >= 0) ? nodrm2real(fd) : fd;
	ret = sceIoLseek32Async(fd, offset, whence);

	return ret;
}

int myIoWaitAsync(SceUID fd, SceInt64 *res)
{
	int ret;

	fd = (nodrm2real(fd) >= 0) ? nodrm2real(fd) : fd;
	ret = sceIoWaitAsync(fd, res);

	return ret;
}

int myIoWaitAsyncCB(SceUID fd, SceInt64 *res)
{
	int ret;

	fd = (nodrm2real(fd) >= 0) ? nodrm2real(fd) : fd;
	ret = sceIoWaitAsyncCB(fd, res);

	return ret;
}

int myIoPollAsync(SceUID fd, SceInt64 *res)
{
	int ret;

	fd = (nodrm2real(fd) >= 0) ? nodrm2real(fd) : fd;
	ret = sceIoPollAsync(fd, res);

	return ret;
}

int myIoGetAsyncStat(SceUID fd, int poll, SceInt64 *res)
{
	int ret;

	fd = (nodrm2real(fd) >= 0) ? nodrm2real(fd) : fd;
	ret = sceIoGetAsyncStat(fd, poll, res);

	return ret;
}

int myIoCancel(SceUID fd)
{
	int ret;

	fd = (nodrm2real(fd) >= 0) ? nodrm2real(fd) : fd;
	ret = sceIoCancel(fd);

	return ret;
}

int myIoGetDevType(SceUID fd)
{
	int ret;

	fd = (nodrm2real(fd) >= 0) ? nodrm2real(fd) : fd;
	ret = sceIoGetDevType(fd);

	return ret;
}

int myIoChangeAsyncPriority(SceUID fd, int pri)
{
	int ret;

	fd = (nodrm2real(fd) >= 0) ? nodrm2real(fd) : fd;
	ret = sceIoChangeAsyncPriority(fd, pri);

	return ret;
}

int myIoSetAsyncCallback(SceUID fd, SceUID cb, void *argp)
{
	int ret;

	fd = (nodrm2real(fd) >= 0) ? nodrm2real(fd) : fd;
	ret = sceIoSetAsyncCallback(fd, cb, argp);

	return ret;
}

static NoDrmHookEntry g_nodrm_hook_map[] = {
	{ "IoFileMgrForUser", 0x109F50BC, &myIoOpen },
	{ "IoFileMgrForUser", 0x89AA9906, &myIoOpenAsync },
	{ "IoFileMgrForUser", 0x810C4BC3, &myIoClose },
	{ "IoFileMgrForUser", 0xFF5940B6, &myIoCloseAsync },
	{ "IoFileMgrForUser", 0x6A638D83, &myIoRead },
	{ "IoFileMgrForUser", 0xA0B5A7C2, &myIoReadAsync },
	{ "IoFileMgrForUser", 0x42EC03AC, &myIoWrite },
	{ "IoFileMgrForUser", 0x0FACAB19, &myIoWriteAsync },
	{ "IoFileMgrForUser", 0x27EB27B8, &myIoLseek },
	{ "IoFileMgrForUser", 0x71B19E77, &myIoLseekAsync },
	{ "IoFileMgrForUser", 0x68963324, &myIoLseek32 },
	{ "IoFileMgrForUser", 0x1B385D8F, &myIoLseek32Async },
	{ "IoFileMgrForUser", 0x63632449, &myIoIoctl },
	{ "IoFileMgrForUser", 0xE95A012B, &myIoIoctlAsync },
	{ "IoFileMgrForUser", 0xE23EEC33, &myIoWaitAsync },
	{ "IoFileMgrForUser", 0x35DBD746, &myIoWaitAsyncCB },
	{ "IoFileMgrForUser", 0x3251EA56, &myIoPollAsync },
	{ "IoFileMgrForUser", 0xCB05F8D6, &myIoGetAsyncStat },
	{ "IoFileMgrForUser", 0xE8BC6571, &myIoCancel },
	{ "IoFileMgrForUser", 0x08BD7374, &myIoGetDevType },
	{ "IoFileMgrForUser", 0xB293727F, &myIoChangeAsyncPriority },
	{ "IoFileMgrForUser", 0xA12A0514, &myIoSetAsyncCallback },
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

	return 0;
}

void patch_drm_imports(SceModule *mod)
{
	u32 i;

	for(i=0; i<NELEMS(g_nodrm_hook_map); ++i) {
		hook_import_bynid(mod, g_nodrm_hook_map[i].libname, g_nodrm_hook_map[i].nid, g_nodrm_hook_map[i].hook_addr, 1);
	}
}
