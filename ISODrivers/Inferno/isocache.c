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
#include "systemctrl_private.h"

static u32 read_call = 0;
static u32 read_hit = 0;
static u32 read_missed = 0;

static u32 cache_on = 0;

struct ISOCache {
	char *buf;
	int bufsize;
	int pos; /* -1 = invalid */
	int age;
};

static struct ISOCache *g_caches = NULL;
static int g_caches_num = 0, g_caches_cap = 0;

static void update_cache_age(struct ISOCache *cache);

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

static void insert_sort(void *base, int n, int s, int (*cmp)(const void *, const void *))
{
	int j;
	void *saved = oe_malloc(s);

	for(j=1; j<n; ++j) {
		int i = j-1;
		void *value = base + j*s;

		while(i >= 0 && cmp(base + i*s, value) > 0) {
			i--;
		}

		if(++i == j)
			continue;

		memmove(saved, value, s);
		memmove(base+(i+1)*s, base+i*s, s*(j-i));
		memmove(base+i*s, saved, s);
	}

	oe_free(saved);
}

static int cmp_cache(const void *a, const void *b)
{
	const struct ISOCache *iso_cache_a, *iso_cache_b;

	iso_cache_a = a, iso_cache_b = b;

	if(iso_cache_a->pos < iso_cache_b->pos)
		return 0;

	return 1;
}

static void sort_iso_cache(void)
{
	insert_sort(g_caches, g_caches_num, sizeof(g_caches[0]), &cmp_cache);
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

static int get_hit_caches(int pos, int len, char *data)
{
	int cur, read_len;
	struct ISOCache *cache = NULL;

	for(cur = pos; cur < pos + len;) {
		cache = get_matched_buffer(cur);

		if(cache == NULL) {
			break;
		}

		read_len = MIN(len - (cur - pos), cache->pos + cache->bufsize - cur);
		memcpy(data + cur - pos, cache->buf + cur - cache->pos, read_len);
		cur += read_len;

		cache->age = 0;
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
	cache->bufsize = 0;
}

static inline int has_cache(int pos, int len)
{
	int cur;
	struct ISOCache *cache = NULL;
	int next_len;

	for(cur = pos; cur < pos + len;) {
		cache = get_matched_buffer(cur);

		if(cache == NULL) {
			break;
		}

		next_len = MIN(len - (cur - pos), cache->pos + cache->bufsize - cur);
		cur += next_len;
	}

	if(cache == NULL)
		return 0;

	return 1;
}

static int add_cache(struct IoReadArg *arg)
{
	int read_len, len, ret;
	struct IoReadArg cache_arg;
	struct ISOCache *cache;
	int pos, cur, next;
	char *data;

	len = arg->size;
	pos = arg->offset;
	data = (char*)arg->address;

	for(cur = pos; cur < pos + len;) {
		next = MIN(len - (cur - pos), g_caches_cap);

		// already in cache, goto next block
		if(has_cache(cur, next)) {
			cur += next;
			continue;
		}

		// replace with oldest cache
		cache = get_oldest_cache();
		disable_cache(cache);

		cache_arg.offset = cur;
		cache_arg.address = (u8*)cache->buf;
		cache_arg.size = g_caches_cap;
		ret = iso_read(&cache_arg);

		if(ret >= 0) {
			cache->pos = cache_arg.offset;
			cache->age = 0;
			cache->bufsize = ret;

			read_len = MIN(len - (cur - pos), ret);
			memcpy(data + cur - pos, cache->buf + cur - cache->pos, read_len);
			cur += read_len;
		} else {
			printk("%s: read -> 0x%08X\n", __func__, ret);
			return ret;
		}
	}

	update_cache_age(NULL);
	sort_iso_cache();

	return ret;
}

int iso_cache_read(struct IoReadArg *arg)
{
	int ret, len;
	int pos;
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
	void *pbuf;

	g_caches_num = cache_num;
	g_caches_cap = 0x4000;

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
		cache->bufsize = 0;
		memset(cache->buf, 0, cache->bufsize);
		cache->pos = -1;
		cache->age = 0;
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
				sprintf(buf, "%d: 0x%08X age %02d\n", i+1, (uint)g_caches[i].pos, g_caches[i].age);
				sceIoWrite(1, buf, strlen(buf));
			}
		}

		sprintf(buf, "iso cache stat: %dKB per cache, %d caches\n", g_caches_cap / 1024, g_caches_num);
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
	}
}
