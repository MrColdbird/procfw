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
#include "inferno.h"

#define CACHE_NR 320
#define CACHE_BUFSIZE (64 * 1024 * CACHE_NR) // 20M

static u32 read_call = 0;
static u32 read_hit = 0;
static u32 read_missed = 0;
static u32 read_uncacheable = 0;

static u32 cache_on = 0;

struct ISOCache {
	char *buf;
	int bufsize;
	SceOff pos; /* -1 = invalid */
	int age;
	int hit;
};

static struct ISOCache g_caches[CACHE_NR];

static void update_cache_age(struct ISOCache *cache);

static inline int is_within_range(SceOff pos, SceOff start, int len)
{
	if(start != -1 && pos >= start && pos < start + len) {
		return 1;
	}

	return 0;
}

static struct ISOCache *get_matched_buffer(SceOff pos)
{
	size_t i;

	for(i=0; i<NELEMS(g_caches); ++i) {
		if(is_within_range(pos, g_caches[i].pos, g_caches[i].bufsize)) {
			return &g_caches[i];
		}
	}

	return NULL;
}

static int get_hit_caches(SceOff pos, int len, char *data)
{
	SceOff cur;
	struct ISOCache *cache;
	int read_len;

	for(cur = pos; cur < pos + len;) {
		cache = get_matched_buffer(cur);

		if(cache == NULL) {
			break;
		}

		read_len = MIN(len - (cur - pos), cache->pos + cache->bufsize - cur);
		memcpy(data + cur - pos, cache->buf + cur - cache->pos, read_len);
		cur += read_len;

		cache->age = 0;
		cache->hit++;
	}

	update_cache_age(NULL);

	if(cache == NULL)
		return -1;

	read_hit++;

	return cur - pos;
}

static void update_cache_age(struct ISOCache *cache)
{
	size_t i;

	for(i=0; i<NELEMS(g_caches); ++i) {
		if(&g_caches[i] == cache) {
			g_caches[i].age = 0;
		} else if (g_caches[i].pos != -1) { // Only valid grows old
			g_caches[i].age++;
		}
	}
}

static struct ISOCache *get_oldest_cache(void)
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

static void disable_cache(struct ISOCache *cache)
{
	cache->pos = -1;
	cache->age = 0;
}

int iso_cache_read(struct IoReadArg *arg)
{
	int ret, read_len, len;
	SceOff pos;
	char *data;
	struct ISOCache *cache;

	if(!cache_on) {
		return iso_read(arg);
	}

	data = (char*)arg->address;
	pos = arg->offset;
	len = arg->size;
	ret = get_hit_caches(pos, len, data);
	
	if(ret < 0) {
		if( 1 ) {
			char buf[256];

			sprintf(buf, "%s: 0x%08X <%d>\n", __func__, (uint)arg->offset, (int)arg->size);
			sceIoWrite(1, buf, strlen(buf));
		}

		cache = get_oldest_cache();

		if(len <= cache->bufsize) {
			struct IoReadArg cache_arg;

			disable_cache(cache);
			cache_arg.offset = pos;
			cache_arg.address = (u8*)cache->buf;
			cache_arg.size = cache->bufsize;
			ret = iso_read(&cache_arg);

			if(ret >= 0) {
				read_len = MIN(len, ret);
				memcpy(data, cache->buf, read_len);
				ret = read_len;
				cache->pos = pos;
				update_cache_age(cache);
			} else {
				printk("%s: read -> 0x%08X\n", __func__, ret);
				update_cache_age(NULL);
			}

			read_missed++;
		} else {
			ret = iso_read(arg);
//			printk("%s: read len %d too large\n", __func__, len);
			update_cache_age(NULL);
			read_uncacheable++;
		}
	}

	read_call++;

	return ret;
}

int infernoCacheInit(void)
{
	SceUID memid;
	SceUInt size, i;
	int bufsize;

	bufsize = CACHE_BUFSIZE;

	if((bufsize / NELEMS(g_caches)) % 0x200 != 0) {
		printk("%s: alignment error\n", __func__);

		return -1;
	}

	for(i=0; i<NELEMS(g_caches); ++i) {
		char memname[20];

		sprintf(memname, "ISOCache%02d\n", i+1);
		size = bufsize / NELEMS(g_caches);
		memid = sceKernelAllocPartitionMemory(9, memname, PSP_SMEM_High, size + 64, NULL);

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

	cache_on = 1;

	return 0;
}

// call @PRO_Inferno_Driver:CacheCtrl,0x5CC24481@
void isocache_stat(int reset)
{
	char buf[256];
	size_t i, used;

	if(read_call != 0) {
		if ( 1 ) {
			sprintf(buf, "caches stat:\n");
			sceIoWrite(1, buf, strlen(buf));
		}

		for(i=0, used=0; i<NELEMS(g_caches); ++i) {
			if(g_caches[i].pos != -1) {
				used++;
			}

			if ( 1 ) {
				sprintf(buf, "%d: 0x%08X age %02d hit %d\n", i+1, (uint)g_caches[i].pos, g_caches[i].age, g_caches[i].hit);
				sceIoWrite(1, buf, strlen(buf));
			}
		}

		sprintf(buf, "iso cache stat: %dKB per cache, %d caches\n", g_caches[0].bufsize / 1024, NELEMS(g_caches));
		sceIoWrite(1, buf, strlen(buf));
		sprintf(buf, "hit percent: %02d%%/%02d%%/%02d%%, [%d/%d/%d/%d]\n", (int)(100 * read_hit / read_call), (int)(100 * read_missed / read_call), (int)(100 * read_uncacheable / read_call), (int)read_hit, (int)read_missed, (int)read_uncacheable, (int)read_call);
		sceIoWrite(1, buf, strlen(buf));
		sprintf(buf, "%d caches used(%02d%%)\n", used, 100 * used / NELEMS(g_caches));
		sceIoWrite(1, buf, strlen(buf));
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
