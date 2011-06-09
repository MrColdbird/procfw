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

#define CACHE_NR 2
#define CACHE_BUFSIZE (16 * 1024)
#define CACHE_BUFSIZE_GO (8 * 1024)

static int (*msstor_read)(PspIoDrvFileArg *arg, char *data, int len) = NULL;
static int (*msstor_write)(PspIoDrvFileArg *arg, const char *data, int len) = NULL;
static SceOff (*msstor_lseek)(PspIoDrvFileArg *arg, SceOff ofs, int whence) = NULL;

static u32 read_call = 0;
static u32 read_hit = 0;
static u32 read_missed = 0;
static u32 read_uncacheable = 0;

struct MsCache {
	char *buf;
	int bufsize;
	SceOff pos; /* -1 = invalid */
	int age;
	int hit;
};

static struct MsCache g_caches[CACHE_NR];

static struct MsCache *get_hit_cache(SceOff pos, int len)
{
	size_t i;

	for(i=0; i<NELEMS(g_caches); ++i) {
		if(g_caches[i].pos != -1 && pos >= g_caches[i].pos && pos + len <= g_caches[i].pos + g_caches[i].bufsize) {
			return &g_caches[i];
		}
	}

	return NULL;
}

static void update_cache_info(void)
{
	size_t i;

	for(i=0; i<NELEMS(g_caches); ++i) {
		if(g_caches[i].pos != -1) {
			g_caches[i].age++;
		}
	}
}

static struct MsCache *get_oldest_cache(void)
{
	size_t i, max;

	max = 0;

	// invalid cache first
	for(i=0; i<NELEMS(g_caches); ++i) {
		if(g_caches[i].pos == -1) {
			max = i;
			goto exit;
		}
	}

	for(i=0; i<NELEMS(g_caches); ++i) {
		if(g_caches[i].age > g_caches[max].age) {
			max = i;
		}
	}

exit:
	return &g_caches[max];
}

static void disable_cache(struct MsCache *cache)
{
	cache->pos = -1;
	cache->age = -1;
}

static void disable_caches(SceOff pos, int len)
{
	size_t i;

	for(i=0; i<NELEMS(g_caches); ++i) {
		if(g_caches[i].pos != -1) {
			if(pos >= g_caches[i].pos && pos < g_caches[i].pos + g_caches[i].bufsize) {
				disable_cache(&g_caches[i]);
			}

			if(pos + len >= g_caches[i].pos && pos + len < g_caches[i].pos + g_caches[i].bufsize) {
				disable_cache(&g_caches[i]);
			}
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
		read_len = MIN(len, cache->bufsize - (pos - cache->pos));
		memcpy(data, cache->buf + pos - cache->pos, read_len);
		ret = read_len;
		(*msstor_lseek)(arg, pos + ret, PSP_SEEK_SET);
		cache->age = -1;
		cache->hit++;
		read_hit++;
	} else {
		cache = get_oldest_cache();

		if(len <= cache->bufsize) {
			disable_cache(cache);
			ret = (*msstor_read)(arg, cache->buf, cache->bufsize);

			if(ret >= 0) {
				read_len = MIN(len, ret);
				memcpy(data, cache->buf, read_len);
				ret = read_len;
				cache->pos = pos;
				cache->age = -1;
				(*msstor_lseek)(arg, pos + ret, PSP_SEEK_SET);
			} else {
				printk("%s: read -> 0x%08X\n", __func__, ret);
			}

			read_missed++;
		} else {
			ret = (*msstor_read)(arg, data, len);
//			printk("%s: read len %d too large\n", __func__, len);
			read_uncacheable++;
		}
	}

	read_call++;
	update_cache_info();

	return ret;
}

static int msstor_cache_write(PspIoDrvFileArg *arg, const char *data, int len)
{
	int ret;
	SceOff pos;

	pos = (*msstor_lseek)(arg, 0, PSP_SEEK_CUR);
	disable_caches(pos, len);
	ret = (*msstor_write)(arg, data, len);

	return ret;
}

int msstor_init(void)
{
	PspIoDrvFuncs *funcs;
	PspIoDrv *pdrv;
	SceUID memid;
	SceUInt size, i;
	int bufsize;

	if(psp_model == PSP_GO) {
		bufsize = CACHE_BUFSIZE_GO;
	} else {
		bufsize = CACHE_BUFSIZE;
	}

	if((bufsize / NELEMS(g_caches)) % 0x200 != 0) {
		printk("%s: alignment error\n", __func__);

		return -1;
	}

	for(i=0; i<NELEMS(g_caches); ++i) {
		char memname[20];

		sprintf(memname, "MsStorCache%02d\n", i+1);
		size = bufsize / NELEMS(g_caches);
		memid = sceKernelAllocPartitionMemory(1, memname, PSP_SMEM_High, size + 64, NULL);

		if(memid < 0) {
			printk("%s: sctrlKernelAllocPartitionMemory -> 0x%08X\n", __func__, memid);
			return -2;
		}

		g_caches[i].buf = sceKernelGetBlockHeadAddr(memid);

		if(g_caches[i].buf == NULL) {
			return -3;
		}

		g_caches[i].buf = (void*)(((u32)g_caches[i].buf & (~(64-1))) + 64);
		g_caches[i].bufsize = bufsize / NELEMS(g_caches);
		memset(g_caches[i].buf, 0, g_caches[i].bufsize);
		g_caches[i].pos = -1;
		g_caches[i].age = 0;
		g_caches[i].hit = 0;
	}	

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
	funcs->IoRead = msstor_cache_read;
	funcs->IoWrite = msstor_cache_write;

	return 0;
}

// call @SystemControl:SystemCtrlPrivate,0xD3014719@
void msstor_stat(int reset)
{
	char buf[256];
	size_t i;

	if(read_call != 0) {
		sprintf(buf, "msstorcache: %dKB per cache, %d caches\n", g_caches[0].bufsize / 1024, NELEMS(g_caches));
		sceIoWrite(1, buf, strlen(buf));
		sprintf(buf, "hit percent: %02d%%/%02d%%/%02d%%, [%d/%d/%d/%d]\n", (int)(100 * read_hit / read_call), (int)(100 * read_missed / read_call), (int)(100 * read_uncacheable / read_call), (int)read_hit, (int)read_missed, (int)read_uncacheable, (int)read_call);
		sceIoWrite(1, buf, strlen(buf));
		sprintf(buf, "caches stat:\n");
		sceIoWrite(1, buf, strlen(buf));

		for(i=0; i<NELEMS(g_caches); ++i) {
			sprintf(buf, "%d: 0x%08X age %02d hit %d\n", i+1, (uint)g_caches[i].pos, g_caches[i].age, g_caches[i].hit);
			sceIoWrite(1, buf, strlen(buf));
		}
	} else {
		sprintf(buf, "no msstor cache call yet\n");
		sceIoWrite(1, buf, strlen(buf));
	}

	if(reset) {
		read_call = read_hit = read_missed = read_uncacheable = 0;

		for(i=0; i<NELEMS(g_caches); ++i) {
			g_caches[i].hit = 0;
		}
	}
}
