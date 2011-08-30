#include <pspsdk.h>
#include <pspsysmem_kernel.h>
#include <pspkernel.h>
#include <psputilsforkernel.h>
#include <pspsysevent.h>
#include <pspiofilemgr.h>
#include <stdio.h>
#include <string.h>
#include "pspcrypt.h"
#include "printk.h"
#include "utils.h"
#include "inferno.h"
#include "systemctrl.h"
#include "systemctrl_se.h"
#include "systemctrl_private.h"

static u32 read_call = 0;
static u32 read_hit = 0;
static u32 read_missed = 0;

static u32 cache_on = 0;

#define NR_CACHE_REQ 8
#define CACHE_MINIMUM_THRESHOLD (16 * 1024)

static u32 cache_policy = CACHE_POLICY_LRU;

struct ISOCacheRequest {
	int pos;
	int len;
};

static struct ISOCacheRequest g_cache_request[NR_CACHE_REQ];
static int g_cache_request_idx = 0;

struct ISOCache {
	char *buf;
	int bufsize;
	int pos; /* -1 = invalid */
	int age;
};

static struct ISOCache *g_caches = NULL;
static int g_caches_num = 0, g_caches_cap = 0;

static inline int is_within_range(int pos, int start, int len)
{
	if(start != -1 && pos >= start && pos < start + len) {
		return 1;
	}

	return 0;
}

static int binary_search(const struct ISOCache *caches, size_t n, int pos)
{
	int low, high, mid;

	low = 0;
	high = n - 1;

	while (low <= high) {
		mid = (low + high) / 2;

		if(is_within_range(pos, caches[mid].pos, caches[mid].bufsize)) {
			return mid;
		} else if (pos < caches[mid].pos) {
			high = mid - 1;
		} else {
			low = mid + 1;
		}
	}

	return -1;
}

static struct ISOCache *get_matched_buffer(int pos)
{
	int cache_pos;

	cache_pos = binary_search(g_caches, g_caches_num, pos);

	if(cache_pos == -1) {
		return NULL;
	}

	return &g_caches[cache_pos];
}

static int get_hit_caches(int pos, int len, char *data, struct ISOCache **last_cache)
{
	int cur, read_len;
	struct ISOCache *cache = NULL;

	*last_cache = NULL;

	for(cur = pos; cur < pos + len;) {
		*last_cache = cache;
		cache = get_matched_buffer(cur);

		if(cache == NULL) {
			break;
		}

		read_len = MIN(len - (cur - pos), cache->pos + cache->bufsize - cur);

		if(data != NULL) {
			memcpy(data + cur - pos, cache->buf + cur - cache->pos, read_len);
		}

		cur += read_len;
		cache->age = -1;
	}

	if(cache == NULL)
		return -1;

	read_hit += len;

	return cur - pos;
}

static void update_cache_info(void)
{
	size_t i;

	if(cache_policy != CACHE_POLICY_LRU)
		return;

	for(i=0; i<g_caches_num; ++i) {
		if (g_caches[i].pos != -1) {
			g_caches[i].age++;
		}
	}
}

static struct ISOCache *get_retirng_cache(void)
{
	size_t i, retiring;

	retiring = 0;

	// invalid cache first
	for(i=0; i<g_caches_num; ++i) {
		if(g_caches[i].pos == -1) {
			retiring = i;
			goto exit;
		}
	}

	if(cache_policy == CACHE_POLICY_LRU) {
		for(i=0; i<g_caches_num; ++i) {
			if(g_caches[i].age > g_caches[retiring].age) {
				retiring = i;
			}
		}
	} else if(cache_policy == CACHE_POLICY_RR) {
		retiring = sctrlKernelRand() % g_caches_num;
	}

exit:
	return &g_caches[retiring];
}

static void disable_cache(struct ISOCache *cache)
{
	cache->pos = -1;
	cache->age = -1;
	cache->bufsize = 0;
}

static void reorder_iso_cache(int idx)
{
	struct ISOCache tmp;
	int i;

	if(idx < 0 && idx >= g_caches_num) {
		printk("%s: wrong idx\n", __func__);
		return;
	}

	memmove(&tmp, &g_caches[idx], sizeof(g_caches[idx]));
	memmove(&g_caches[idx], &g_caches[idx+1], sizeof(g_caches[idx]) * (g_caches_num - idx - 1));

	for(i=0; i<g_caches_num-1; ++i) {
		if(g_caches[i].pos >= tmp.pos) {
			break;
		}
	}

	memmove(&g_caches[i+1], &g_caches[i], sizeof(g_caches[idx]) * (g_caches_num - i - 1));
	memmove(&g_caches[i], &tmp, sizeof(tmp));
}

static int add_cache(struct IoReadArg *arg)
{
	int read_len, len, ret;
	struct IoReadArg cache_arg;
	struct ISOCache *cache, *last_cache;
	int pos, cur, next;
	char *data;

	len = arg->size;
	pos = arg->offset;
	data = (char*)arg->address;

	for(cur = pos; cur < pos + len;) {
		next = len - (cur - pos);

		if(data == NULL) {
			ret = get_hit_caches(cur, next, NULL, &last_cache);
		} else {
			ret = get_hit_caches(cur, next, data + cur - pos, &last_cache);
		}

		if(ret >= 0) {
			cur += ret;
			continue;
		}

		if(last_cache != NULL) {
			if(pos + len <= last_cache->pos + last_cache->bufsize) {
				printk("%s: error pos\n", __func__);
				asm("break");
			}

			cur = last_cache->pos + last_cache->bufsize;
		}

		cache = get_retirng_cache();
		disable_cache(cache);

		cache_arg.offset = cur & (~(64-1));
		cache_arg.address = (u8*)cache->buf;
		cache_arg.size = MIN(g_total_sectors * ISO_SECTOR_SIZE - cur, g_caches_cap);

		ret = iso_read(&cache_arg);

		if(ret >= 0) {
			cache->pos = cache_arg.offset;
			cache->age = -1;
			cache->bufsize = ret;

			read_len = MIN(len - (cur - pos), cache->pos + cache->bufsize - cur);

			if(data != NULL) {
				memcpy(data + cur - pos, cache->buf + cur - cache->pos, read_len);
			}

			cur += read_len;
			reorder_iso_cache(cache - g_caches);
		} else {
			reorder_iso_cache(cache - g_caches);
			printk("%s: read -> 0x%08X\n", __func__, ret);
			return ret;
		}
	}

	return cur - pos;
}

static void process_request(void)
{
	int pos, len;
	struct IoReadArg cache_arg;

	if(g_cache_request_idx <= 0) {
		return;
	}

	g_cache_request_idx--;
	pos = g_cache_request[g_cache_request_idx].pos;
	len = g_cache_request[g_cache_request_idx].len;

	cache_arg.size = len;
	cache_arg.offset = pos;
	cache_arg.address = NULL;

	add_cache(&cache_arg);
}

int iso_cache_read(struct IoReadArg *arg)
{
	int ret, len;
	int pos;
	char *data;
	struct ISOCache *last_cache;

	if(!cache_on) {
		return iso_read(arg);
	}

	data = (char*)arg->address;
	pos = arg->offset;
	len = arg->size;
	ret = get_hit_caches(pos, len, data, &last_cache);
	
	if(ret < 0) {
#if 0
		{
			char buf[256];

			sprintf(buf, "%s: 0x%08X <%d>\n", __func__, (uint)arg->offset, (int)arg->size);
			sceIoWrite(1, buf, strlen(buf));
		}
#endif

		// abandon the caching, because the bufsize is too small
		// if we cache it then random access performance will be hurt
		if(arg->size < MIN(CACHE_MINIMUM_THRESHOLD, g_caches_cap)) {
			return iso_read(arg);
		}

		ret = add_cache(arg);
		read_missed += len;
	} else {
		// we have to sleep a bit to prevent mystery freeze in KHBBS (from worldmap to location)
		// it's caused by caching too fast 
		// tested NFS Carbon: Own the city, seems delaying 100 wasn't enough...
		sceKernelDelayThread(500);
	}

	read_call += len;
	process_request();
	update_cache_info();

#if 0
	{
		SceUID memid;
		struct IoReadArg testarg;
		int retv;

		memcpy(&testarg, arg, sizeof(testarg));

		// Check validate code
		memid = sceKernelAllocPartitionMemory(9, "infernoCacheTest", PSP_SMEM_High, testarg.size + 64, NULL);

		if(memid >= 0) {
			testarg.address = sceKernelGetBlockHeadAddr(memid);
			testarg.address = (void*)(((u32)testarg.address & (~(64-1))) + 64);
			
			retv = iso_read(&testarg);

			if(0 != memcmp(data, testarg.address, testarg.size)) {
				char buf[256];

				sprintf(buf, "%s: 0x%08X <%d> cache mismatched\n", __func__, (uint)testarg.offset, (int)testarg.size);
				sceIoWrite(1, buf, strlen(buf));

//				memcpy(data, testarg.address, testarg.size);
			}

			if(ret != retv) {
				char buf[256];
				
				sprintf(buf, "%s: 0x%08X <%d> return (%d/%d) mismatched\n", __func__, (uint)testarg.offset, (int)testarg.size, ret, retv);
				sceIoWrite(1, buf, strlen(buf));

//				ret = retv;
			}

			sceKernelFreePartitionMemory(memid);
		} else {
			char buf[256];

			sprintf(buf, "%s: 0x%08X <%d> cache too large to verify\n", __func__, (uint)testarg.offset, (int)testarg.size);
			sceIoWrite(1, buf, strlen(buf));
		}
	}
#endif

	return ret;
}

int infernoCacheInit(int cache_size, int cache_num)
{
	SceUID memid;
	SceUInt i;
	struct ISOCache *cache;
	void *pbuf;

	g_caches_num = cache_num;
	g_caches_cap = cache_size;

	if(g_caches_cap % 0x200 != 0) {
		return -1;
	}
	
	memid = sceKernelAllocPartitionMemory(9, "infernoCacheCtl", PSP_SMEM_High, g_caches_num * sizeof(g_caches[0]), NULL);

	if(memid < 0) {
		printk("%s: sctrlKernelAllocPartitionMemory -> 0x%08X\n", __func__, memid); 
		return -2;
	}

	g_caches = sceKernelGetBlockHeadAddr(memid);

	if(g_caches == NULL) {
		return -3;
	}

	memid = sceKernelAllocPartitionMemory(9, "inferoCache", PSP_SMEM_High, g_caches_cap * g_caches_num + 64, NULL);

	if(memid < 0) {
		printk("%s: sctrlKernelAllocPartitionMemory -> 0x%08X\n", __func__, memid);
		return -4;
	}

	pbuf = sceKernelGetBlockHeadAddr(memid);
	pbuf = (void*)(((u32)pbuf & (~(64-1))) + 64);

	for(i=0; i<g_caches_num; ++i) {
		cache = &g_caches[i];
		cache->buf = pbuf + i * g_caches_cap;
		disable_cache(cache);
	}

	cache_on = 1;

	return 0;
}

int infernoCacheAdd(int pos, int len)
{
	if(!cache_on) {
		return -1;
	}

	if(g_cache_request_idx < NELEMS(g_cache_request)) {
		g_cache_request[g_cache_request_idx].pos = pos;
		g_cache_request[g_cache_request_idx].len = len;
		g_cache_request_idx++;

#if 0
		{
			char buf[256];

			sprintf(buf, "%s: 0x%08X <%d> added\n", __func__, pos, len);
			sceIoWrite(1, buf, strlen(buf));
		}
#endif

		return 0;
	}

	// TOO BUSY
	return -2;
}

// call @PRO_Inferno_Driver:CacheCtrl,0x5CC24481@
void isocache_stat(int reset)
{
	char buf[256];
	size_t i, used;

	if(read_call != 0) {
		sprintf(buf, "caches stat:\n");
		sceIoWrite(1, buf, strlen(buf));

		for(i=0, used=0; i<g_caches_num; ++i) {
			if(g_caches[i].pos != -1) {
				used++;
			}

			sprintf(buf, "%d: 0x%08X size %d age %02d\n", i+1, (uint)g_caches[i].pos, g_caches[i].bufsize, g_caches[i].age);
			sceIoWrite(1, buf, strlen(buf));
		}

		sprintf(buf, "%dKB per cache, %d caches policy %d\n", g_caches_cap / 1024, g_caches_num, (int)cache_policy);
		sceIoWrite(1, buf, strlen(buf));
		sprintf(buf, "hit percent: %02d%%/%02d%%, [%d/%d/%d]\n", (int)(100 * read_hit / read_call), (int)(100 * read_missed / read_call), (int)read_hit, (int)read_missed, (int)read_call);
		sceIoWrite(1, buf, strlen(buf));
		sprintf(buf, "%d caches used(%02d%%)\n", used, 100 * used / g_caches_num);
		sceIoWrite(1, buf, strlen(buf));
	} else {
		sprintf(buf, "no cache call yet\n");
		sceIoWrite(1, buf, strlen(buf));
	}

	if(reset) {
		read_call = read_hit = read_missed = 0;
	}
}

// call @PRO_Inferno_Driver:CacheCtrl,0xC0736FD6@
void infernoCacheSetPolicy(int policy)
{
	cache_policy = policy;
}
