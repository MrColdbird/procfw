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
#include "main.h"
#include "utils.h"
#include "systemctrl.h"
#include "printk.h"
#include "libs.h"
#include "nid_resolver.h"
#include "systemctrl_patch_offset.h"

typedef struct _CustomResolver {
	void *import_addr;
	void *fp;
} CustomResolver;

typedef struct _MissingNIDEntry {
	u32 nid, fp;
} MissingNIDEntry;

typedef struct _MissingNIDResolver {
	const char *libname;
	MissingNIDEntry *entry;
	u32 size;
} MissingNIDResolver;

static int (*sceKernelLinkLibraryEntries)(void *buf, int size) = NULL;
static int (*sceKernelLinkLibraryEntriesForUser)(u32 unk0, void *buf, int size) = NULL;

resolver_config *nid_fix;
u32 nid_fix_size;

resolver_config* get_nid_resolver(const char *libname)
{
	int i;

	for(i=0; i<nid_fix_size; ++i) {
		if (nid_fix[i].enabled && !strcmp(libname, nid_fix[i].name)) {
			return &nid_fix[i];
		}
	}

	return NULL;
}

static int binary_search(const nid_entry *nids, size_t n, u32 old_nid)
{
	int low, high, mid;

	low = 0;
	high = n - 1;

	while (low <= high) {
		mid = (low + high) / 2;

		if(old_nid == nids[mid].old) {
			return mid;
		} else if (old_nid < nids[mid].old) {
			high = mid - 1;
		} else {
			low = mid + 1;
		}
	}

	return -1;
}

u32 resolve_nid(resolver_config *resolver, u32 nid)
{
	int i;
	u32 new;

	i = binary_search(resolver->nidtable, resolver->nidcount, nid);

	if(i >= 0) {
		new = resolver->nidtable[i].new;

		if(new != UNKNOWNNID) {
			printk("%s: %s_%08X->%s_%08X\n", __func__, resolver->name, (uint)nid, resolver->name, (uint)new);

			return new;
		}
	}

	return nid;
}

static int resolve_missing_nid(SceLibraryStubTable *stub, MissingNIDResolver *resolver)
{
	int cnt, i, j;
	const char *libname;

	libname = resolver->libname;

	if (0 != strcmp(stub->libname, libname)) {
		return -1;
	}

	for(i=0, cnt=stub->vstubcount+stub->stubcount; i<cnt; ++i) {
		for(j=0; j<resolver->size; ++j) {
			u32 nid;

			nid = resolver->entry[j].nid;

			if(stub->nidtable[i] == nid) {
				void *stub_addr;
				u32 fp;

				stub_addr = stub->stubtable + (i << 3);
				fp = resolver->entry[j].fp;
				printk("%s: %s_%08X resolved(fp: 0x%08X)\n", __func__, libname, (uint)nid, (uint)fp);
				REDIRECT_FUNCTION(fp, (u32)stub_addr);
				sceKernelDcacheWritebackInvalidateRange(stub_addr, 8);
				sceKernelIcacheInvalidateRange(stub_addr, 8);
			}
		}
	}

	return 0;
}

//missing sysclib function from 3.XX times
static int ownstrcspn(char * str1, char * str2)
{
	//iterate symbols from str1
	u32 i = 0; for (; i < strlen(str1); i++) {
		//iterate symbols from str2
		u32 j = 0; for (; j < strlen(str2); j++) {
			//match found
			if(str1[i] == str2[j]) break;
		}
	}

	//return read symbol count
	return i;
}

//missing sysclib function from 3.XX times
static int ownstrspn(char * str1, char * str2)
{
	//iterate symbols from str1
	u32 i = 0; for (; i < strlen(str1); i++) {
		//iterate symbols from str2
		u32 j = 0; for (; j < strlen(str2); j++) {
			//symbols not identical
			if(str1[i] != str2[j]) break;
		}
	}

	//return read symbol count
	return i;
}

static char * ownstrtok_r(char * s, const char * delim, char ** last)
{
	char * spanp;
	int c, sc;
	char * tok;

	if (s == NULL && (s = *last) == NULL)
		return (NULL);

	cont:
	c = *s++;
	for (spanp = (char *)delim; (sc = *spanp++) != 0;) {
		if (c == sc)
			goto cont;
	}

	if (c == 0) {
		*last = NULL;
		return (NULL);
	}
	tok = s - 1;

	for (;;) {
		c = *s++;
		spanp = (char *)delim;
		do {
			if ((sc = *spanp++) == c) {
				if (c == 0)
					s = NULL;
				else
					s[-1] = 0;
				*last = s;
				return (tok);
			}
		} while (sc != 0);
	}
}

//missing sysclib function from 3.XX times
static char * ownstrtok(char * s, const char * delim)
{
	static char * last;
	return ownstrtok_r(s, delim, &last);
}

#include "nid_data_missing.h"

static int module_can_skip_nid_resolve(void *buf)
{
	int ret;
	u32 *version;

	/* check module_sdk_version */
	version = (u32*)sctrlHENFindFunction(buf, NULL, 0x11B97506);
	ret = 0;

	if(version != NULL) {
		if ((*version >> 16) == psp_fw_version >> 16) {
			ret = 1;
		}
	}

	return ret;
}

int _sceKernelLinkLibraryEntries(void *buf, int size)
{
	int ret, offset, i;
	u32 stubcount;
	struct SceLibraryStubTable *stub;
	resolver_config *resolver;

	if(module_can_skip_nid_resolve(buf)) {
		return (*sceKernelLinkLibraryEntries)(buf, size);
	}

	offset = 0;

	while(offset < size) {
		stub = buf + offset;
		stubcount = stub->stubcount;
		resolver = get_nid_resolver(stub->libname);

		if(resolver != NULL) {
			for (i=0; i<stubcount; i++) {
				u32 newnid;

				newnid = resolve_nid(resolver, stub->nidtable[i]);

				if(newnid != stub->nidtable[i]) {
					stub->nidtable[i] = newnid;
				}
			}
		}

		offset += stub->len << 2;
	}

	ret = (*sceKernelLinkLibraryEntries)(buf, size);

	offset = 0;

	while(offset < size) {
		stub = buf + offset;
		stubcount = stub->stubcount;

		for(i=0; i<NELEMS(g_missing_resolver); ++i) {
			resolve_missing_nid(stub, g_missing_resolver[i]);
		}
		
		offset += stub->len << 2;
	}

	return ret;
}

int _sceKernelLinkLibraryEntriesForUser(u32 unk0, void *buf, int size)
{
	int ret, offset, i;
	u32 stubcount;
	struct SceLibraryStubTable *stub;
	resolver_config *resolver;

	if(module_can_skip_nid_resolve(buf)) {
		return (*sceKernelLinkLibraryEntriesForUser)(unk0, buf, size);
	}

	offset = 0;

	while(offset < size) {
		stub = buf + offset;
		stubcount = stub->stubcount;
		resolver = get_nid_resolver(stub->libname);

		if(resolver != NULL) {
			for (i=0; i<stubcount; i++) {
				u32 newnid;

				newnid = resolve_nid(resolver, stub->nidtable[i]);

				if(newnid != stub->nidtable[i]) {
					stub->nidtable[i] = newnid;
				}
			}
		}

		offset += stub->len << 2;
	}

	ret = (*sceKernelLinkLibraryEntriesForUser)(unk0, buf, size);

	offset = 0;

	while(offset < size) {
		stub = buf + offset;
		stubcount = stub->stubcount;

		for(i=0; i<NELEMS(g_missing_resolver_user); ++i) {
			resolve_missing_nid(stub, g_missing_resolver_user[i]);
		}
		
		offset += stub->len << 2;
	}

	return ret;
}

static void insert_sort(nid_entry *base, int n, int (*cmp)(const nid_entry*, const nid_entry*))
{
	int j;
	nid_entry saved;

	for(j=1; j<n; ++j) {
		int i = j-1;
		nid_entry *value = &base[j];

		while(i >= 0 && cmp(&base[i], value) > 0) {
			i--;
		}

		if(++i == j)
			continue;

		memmove(&saved, value, sizeof(saved));
		memmove(&base[i+1], &base[i], sizeof(saved)*(j-i));
		memmove(&base[i], &saved, sizeof(saved));
	}
}

static int cmp_nid(const nid_entry *nid_a, const nid_entry *nid_b)
{
	if(nid_a->old < nid_b->old)
		return 0;

	return 1;
}

static void sort_nid_table(resolver_config *table, u32 size)
{
	u32 i;

	for(i=0; i<size; ++i) {
		insert_sort(table[i].nidtable, table[i].nidcount, &cmp_nid);
	}
}

void setup_nid_resolver(void)
{
	SceModule2 *modmgr, *loadcore;

	modmgr = (SceModule2*)sctrlKernelFindModuleByName("sceModuleManager");
	loadcore = (SceModule2*)sctrlKernelFindModuleByName("sceLoaderCore");

	missing_LoadCoreForKernel_entries[0].fp = (loadcore->text_addr + g_offs->loadercore_patch.sceKernelIcacheClearAll);
	sceKernelLinkLibraryEntries = (void*)(loadcore->text_addr + g_offs->loadercore_patch.sceKernelLinkLibraryEntries);
	sceKernelLinkLibraryEntriesForUser = (void*)(loadcore->text_addr + g_offs->loadercore_patch.sceKernelLinkLibraryEntriesForUser);
	_sw(MAKE_CALL(_sceKernelLinkLibraryEntries), modmgr->text_addr + g_offs->modulemgr_patch.sceKernelLinkLibraryEntriesCall);
	_sw(MAKE_CALL(_sceKernelLinkLibraryEntriesForUser), modmgr->text_addr + g_offs->modulemgr_patch.sceKernelLinkLibraryEntriesForUserCall);

	switch(psp_fw_version) {
#if defined(CONFIG_660) || defined(CONFIG_661)
		case FW_660:
		case FW_661:
			nid_fix = nid_660_fix;
			nid_fix_size = nid_660_fix_size;
			break;
#endif
#ifdef CONFIG_639
		case FW_639:
			nid_fix = nid_639_fix;
			nid_fix_size = nid_639_fix_size;
			break;
#endif
#ifdef CONFIG_635
		case FW_635:
			nid_fix = nid_635_fix;
			nid_fix_size = nid_635_fix_size;
			break;
#endif
#ifdef CONFIG_620
		case FW_620:
			nid_fix = nid_620_fix;
			nid_fix_size = nid_620_fix_size;
			break;
#endif
	}

	sort_nid_table(nid_fix, nid_fix_size);
}

void resolve_syscon_driver(SceModule *mod)
{
	SceModule2 *syscon;

	syscon = (SceModule2*)mod;

	if(syscon == NULL)
		return;

	 missing_sceSyscon_driver_entries[0].fp = (syscon->text_addr + g_offs->syscon_patch.sceSysconPowerStandby);
}

int sctrlKernelSetNidResolver(char *libname, u32 enabled)
{
	u32 i;
	u32 old;

	if(!check_memory(libname, strlen(libname) + 1)) {
		return 0x800200D3;
	}

	for(i=0; i<nid_fix_size; ++i) {
		if (0 == strcmp(libname, nid_fix[i].name)) {
			old = nid_fix[i].enabled;
			nid_fix[i].enabled = enabled;

			return old;
		}
	}

	return -1;
}
