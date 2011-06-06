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

static u32 read_call = 0;
static u32 read_hit = 0;
static u32 read_missed = 0;

static u32 cache_on = 0;

struct ISOCache {
	char *buf;
	int bufsize;
	SceOff pos; /* -1 = invalid */
	int age;
	int hit;
};

static struct ISOCache *g_caches = NULL;
static int g_caches_num = 0;

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

	for(i=0; i<g_caches_num; ++i) {
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

	read_hit += len;

	return cur - pos;
}

static void update_cache_age(struct ISOCache *cache)
{
	size_t i;

	for(i=0; i<g_caches_num; ++i) {
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
	for(i=0; i<g_caches_num; ++i) {
		if(g_caches[i].pos == -1) {
			max = i;
			goto exit;
		}
	}

	for(i=0; i<g_caches_num; ++i) {
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

static int add_cache(struct IoReadArg *arg)
{
	int read_len, len, ret;
	struct IoReadArg cache_arg;
	struct ISOCache *cache;
	SceOff pos, cur;
	char *data;

	len = arg->size;
	pos = arg->offset;
	data = (char*)arg->address;

	for(cur = pos; cur < pos + len;) {
		cache = get_oldest_cache();
		disable_cache(cache);

		cache_arg.offset = cur;
		cache_arg.address = (u8*)cache->buf;
		cache_arg.size = cache->bufsize;
		ret = iso_read(&cache_arg);

		if(ret >= 0) {
			cache->pos = cache_arg.offset;
			cache->age = 0;

			read_len = MIN(len - (cur - pos), ret);
			memcpy(data + cur - pos, cache->buf + cur - cache->pos, read_len);
			cur += read_len;
		} else {
			printk("%s: read -> 0x%08X\n", __func__, ret);
			return ret;
		}
	}

	update_cache_age(NULL);

	return ret;
}

int iso_cache_read(struct IoReadArg *arg)
{
	int ret, len;
	SceOff pos;
	char *data;

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

		ret = add_cache(arg);
		read_missed += len;
	}

	read_call += len;

	return ret;
}

int infernoCacheInit(int cache_size, int cache_num)
{
	SceUID memid;
	SceUInt i;
	struct ISOCache *cache;

	g_caches_num = cache_num;
	memid = sceKernelAllocPartitionMemory(1, "infernoCacheCtl", PSP_SMEM_High, g_caches_num * sizeof(g_caches[0]), NULL);

	if(memid < 0) {
		printk("%s: sctrlKernelAllocPartitionMemory -> 0x%08X\n", __func__, memid); 
		return -1;
	}

	g_caches = sceKernelGetBlockHeadAddr(memid);

	if(g_caches == NULL) {
		return -2;
	}

	for(i=0; i<g_caches_num; ++i) {
		char memname[20];
		
		cache = &g_caches[i];
		sprintf(memname, "inferoCache%03d\n", i+1);
		memid = sceKernelAllocPartitionMemory(9, memname, PSP_SMEM_High, cache_size + 64, NULL);

		if(memid < 0) {
			printk("%s: sctrlKernelAllocPartitionMemory -> 0x%08X\n", __func__, memid);
			return -3;
		}

		cache->buf = sceKernelGetBlockHeadAddr(memid);

		if(cache->buf == NULL) {
			return -4;
		}

		cache->buf = (void*)(((u32)cache->buf & (~(64-1))) + 64);
		cache->bufsize = cache_size;
		memset(cache->buf, 0, cache->bufsize);
		cache->pos = -1;
		cache->age = cache->hit = 0;
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
		if(1) {
			sprintf(buf, "caches stat:\n");
			sceIoWrite(1, buf, strlen(buf));
		}

		for(i=0, used=0; i<g_caches_num; ++i) {
			if(g_caches[i].pos != -1) {
				used++;
			}

			if(1) {
				sprintf(buf, "%d: 0x%08X age %02d hit %d\n", i+1, (uint)g_caches[i].pos, g_caches[i].age, g_caches[i].hit);
				sceIoWrite(1, buf, strlen(buf));
			}
		}

		sprintf(buf, "iso cache stat: %dKB per cache, %d caches\n", g_caches[0].bufsize / 1024, g_caches_num);
		sceIoWrite(1, buf, strlen(buf));
		sprintf(buf, "hit percent: %02d%%/%02d%%, [%d/%d/%d]\n", (int)(100 * read_hit / read_call), (int)(100 * read_missed / read_call), (int)read_hit, (int)read_missed, (int)read_call);
		sceIoWrite(1, buf, strlen(buf));
		sprintf(buf, "%d caches used(%02d%%)\n", used, 100 * used / g_caches_num);
		sceIoWrite(1, buf, strlen(buf));
	} else {
		sprintf(buf, "no msstor cache call yet\n");
		sceIoWrite(1, buf, strlen(buf));
	}

	if(reset) {
		read_call = read_hit = read_missed = 0;

		for(i=0; i<g_caches_num; ++i) {
			g_caches[i].hit = 0;
		}
	}
}
