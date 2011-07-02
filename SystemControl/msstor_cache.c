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
#include <pspsysmem_kernel.h>
#include <pspkernel.h>
#include <psputilsforkernel.h>
#include <pspsysevent.h>
#include <pspiofilemgr.h>
#include <stdio.h>
#include <string.h>
#include "printk.h"
#include "utils.h"
#include "main.h"
#include "systemctrl_patch_offset.h"

#define CACHE_BUFSIZE (16 * 1024)
#define CACHE_BUFSIZE_GO (8 * 1024)

static int (*msstor_read)(PspIoDrvFileArg *arg, char *data, int len) = NULL;
static int (*msstor_write)(PspIoDrvFileArg *arg, const char *data, int len) = NULL;
static SceOff (*msstor_lseek)(PspIoDrvFileArg *arg, SceOff ofs, int whence) = NULL;
static int(*msstor_open)(PspIoDrvFileArg *arg, char *file, int flags, SceMode mode) = NULL;

static u32 read_call = 0;
static u32 read_hit = 0;
static u32 read_missed = 0;
static u32 read_uncacheable = 0;

struct MsCache {
	char *buf;
	int bufsize;
	SceOff pos; /* -1 = invalid */
	int age;
};

static struct MsCache g_cache;
static int g_cache_cap = 0;

static inline int is_within_range(SceOff pos, SceOff start, int len)
{
	if(pos >= start && pos < start + len) {
		return 1;
	}

	return 0;
}

static struct MsCache *get_hit_cache(SceOff pos, int len)
{
	if(g_cache.pos != -1) {
		if(is_within_range(pos, g_cache.pos, g_cache.bufsize) && is_within_range(pos+len-1, g_cache.pos, g_cache.bufsize)) {
			return &g_cache;
		}
	}

	return NULL;
}

static void disable_cache(struct MsCache *cache)
{
	cache->pos = -1;
	cache->bufsize = 0;
}

static void disable_cache_within_range(SceOff pos, int len)
{
	if(g_cache.pos != -1) {
		if(is_within_range(pos, g_cache.pos, g_cache.bufsize)) {
			disable_cache(&g_cache);
			return;
		}

		if(is_within_range(pos+len-1, g_cache.pos, g_cache.bufsize)) {
			disable_cache(&g_cache);
			return;
		}

		if(pos < g_cache.pos && pos+len-1 >= g_cache.pos + g_cache.bufsize) {
			disable_cache(&g_cache);
			return;
		}
	}
}

static int msstor_cache_read(PspIoDrvFileArg *arg, char *data, int len)
{
	int ret, read_len;
	SceOff pos;
	struct MsCache *cache;
	
	pos = (*msstor_lseek)(arg, 0, PSP_SEEK_CUR);
	cache = get_hit_cache(pos, len);
	
	// cache hit?
	if(cache != NULL) {
#if 1
		read_len = MIN(len, cache->pos + cache->bufsize - pos);
		memcpy(data, cache->buf + pos - cache->pos, read_len);
		ret = read_len;
		(*msstor_lseek)(arg, pos + ret, PSP_SEEK_SET);
		read_hit += len;
#else
		// Check validate code
		ret = (*msstor_read)(arg, data, len);

		if(0 != memcmp(data, cache->buf + pos - cache->pos, len)) {
			char buf[256];

			sprintf(buf, "%s: 0x%08X <%d> cache mismatched!!!\n", __func__, (uint)pos, (int)len);
			sceIoWrite(1, buf, strlen(buf));
		}
#endif
	} else {
#if 0
		{
			char buf[256];

			sprintf(buf, "%s: 0x%08X <%d>\n", __func__, (uint)pos, (int)len);
			sceIoWrite(1, buf, strlen(buf));
		}
#endif
		
		cache = &g_cache;

		if(len <= g_cache_cap) {
			disable_cache(cache);
			ret = (*msstor_read)(arg, cache->buf, g_cache_cap);

			if(ret >= 0) {
				read_len = MIN(len, ret);
				memcpy(data, cache->buf, read_len);
				cache->pos = pos;
				cache->bufsize = ret;
				ret = read_len;
				(*msstor_lseek)(arg, pos + ret, PSP_SEEK_SET);
			} else {
				printk("%s: read -> 0x%08X\n", __func__, ret);
			}

			read_missed += len;
		} else {
			ret = (*msstor_read)(arg, data, len);
//			printk("%s: read len %d too large\n", __func__, len);
			read_uncacheable += len;
		}
	}

	read_call += len;

	return ret;
}

static int msstor_cache_write(PspIoDrvFileArg *arg, const char *data, int len)
{
	int ret;
	SceOff pos;

	pos = (*msstor_lseek)(arg, 0, PSP_SEEK_CUR);
	disable_cache_within_range(pos, len);
	ret = (*msstor_write)(arg, data, len);

	return ret;
}

static int msstor_cache_open(PspIoDrvFileArg *arg, char *file, int flags, SceMode mode)
{
	int ret;

#if 0
	{
		char buf[256];

		sprintf(buf, "%s: %s 0x%08X 0x%08X\n", __func__, file, flags, mode);
		sceIoWrite(1, buf, strlen(buf));
	}
#endif

	disable_cache(&g_cache);
	ret = (*msstor_open)(arg, file, flags, mode);

	return ret;
}

int msstor_init(void)
{
	PspIoDrvFuncs *funcs;
	PspIoDrv *pdrv;
	SceUID memid;
	int bufsize;

	if(psp_model == PSP_GO) {
		bufsize = CACHE_BUFSIZE_GO;
	} else {
		bufsize = CACHE_BUFSIZE;
	}

	if((bufsize % 0x200) != 0) {
		printk("%s: alignment error\n", __func__);

		return -1;
	}

	memid = sctrlKernelAllocPartitionMemory(1, "MsStorCache", PSP_SMEM_High, bufsize + 64, NULL);

	if(memid < 0) {
		printk("%s: sctrlKernelAllocPartitionMemory -> 0x%08X\n", __func__, memid);
		return -2;
	}

	g_cache.buf = sctrlKernelGetBlockHeadAddr(memid);

	if(g_cache.buf == NULL) {
		return -3;
	}

	g_cache.buf = (void*)(((u32)g_cache.buf & (~(64-1))) + 64);
	g_cache_cap =  bufsize;
	disable_cache(&g_cache);

	if(psp_model == PSP_GO && sctrlKernelBootFrom() == 0x50) {
		pdrv = sctrlHENFindDriver("eflash0a0f1p");
	} else {
		pdrv = sctrlHENFindDriver("msstor0p");
	}

	if(pdrv == NULL) {
		return -4;
	}

	funcs = pdrv->funcs;
	msstor_read = funcs->IoRead;
	msstor_write = funcs->IoWrite;
	msstor_lseek = funcs->IoLseek;
	msstor_open = funcs->IoOpen;
	funcs->IoRead = msstor_cache_read;
	funcs->IoWrite = msstor_cache_write;
	funcs->IoOpen= msstor_cache_open;

	return 0;
}

// call @SystemControl:SystemCtrlPrivate,0xD3014719@
void msstor_stat(int reset)
{
	char buf[256];

	if(read_call != 0) {
		sprintf(buf, "Mstor cache size: %dKB\n", g_cache_cap / 1024);
		sceIoWrite(1, buf, strlen(buf));
		sprintf(buf, "hit percent: %02d%%/%02d%%/%02d%%, [%d/%d/%d/%d]\n", 
				(int)(100 * read_hit / read_call), 
				(int)(100 * read_missed / read_call), 
				(int)(100 * read_uncacheable / read_call), 
				(int)read_hit, (int)read_missed, (int)read_uncacheable, (int)read_call);
		sceIoWrite(1, buf, strlen(buf));
		sprintf(buf, "caches stat:\n");
		sceIoWrite(1, buf, strlen(buf));
		sprintf(buf, "Cache Pos: 0x%08X Bufsize: %d Buf: 0x%08X\n", (uint)g_cache.pos, g_cache.bufsize, (uint)g_cache.buf);
		sceIoWrite(1, buf, strlen(buf));
	} else {
		sprintf(buf, "no msstor cache call yet\n");
		sceIoWrite(1, buf, strlen(buf));
	}

	if(reset) {
		read_call = read_hit = read_missed = read_uncacheable = 0;
	}
}

// call @SystemControl:SystemCtrlPrivate,0xC0E151D0@
void msstor_disable_cache(void)
{
	disable_cache(&g_cache);
}
