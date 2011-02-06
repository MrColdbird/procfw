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

#define MAX_CUSTOM_RESOLVER 16

static CustomResolver g_custom[MAX_CUSTOM_RESOLVER];
static int g_custom_cnt = 0;

static int (*sceKernelLinkLibraryEntries)(void *buf, int size) = NULL;

static void *g_buf = NULL;

resolver_config* get_nid_resolver(const char *libname)
{
	int i;

	for(i=0; i<nid_fix_size; ++i) {
		if (!strcmp(libname, nid_fix[i].name)) {
			return &nid_fix[i];
		}
	}

	return NULL;
}

u32 resolve_nid(resolver_config *resolver, u32 nid)
{
	int i;
	u32 new;

	for(i=0; i<resolver->nidcount; ++i) {
		new = resolver->nidtable[i].new;

		if(new != UNKNOWNNID && nid == resolver->nidtable[i].old) {
			printk("%s: %s_%08X->%s_%08X\n", __func__, resolver->name, nid, resolver->name, new);

			return new;
		}
	}

	return nid;
}

static int mark_missing_NID(SceLibraryStubTable *stub, MissingNIDResolver *resolver)
{
	int cnt, i, j;
	const char *libname;

	libname = resolver->libname;

	if (0 != strcmp(stub->libname, libname)) {
		return 0;
	}

	for(i=0, cnt=stub->vstubcount+stub->stubcount; i<cnt; ++i) {
		for(j=0; j<resolver->size; ++j) {
			u32 nid;

			nid = resolver->entry[j].nid;

			if(stub->nidtable[i] == nid) {
				void *stub_addr;

				stub_addr = stub->stubtable + (i << 3);

				if(g_custom_cnt < NELEMS(g_custom)) {
					void *fp;

					fp = (void*)resolver->entry[j].fp;
					printk("%s: %s_%08X resolved(fp: 0x%08X)\n", __func__, libname, nid, (u32)fp);
					g_custom[g_custom_cnt].import_addr = stub_addr;
					g_custom[g_custom_cnt].fp = fp;
					g_custom_cnt++;
				} else {
					printk("%s: custom resolve exceed in %s\n", __func__, libname);
					break;
				}
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

int _sceKernelLinkLibraryEntries(void *buf, int size)
{
	int ret, offset, i;
	u32 stubcount;
	struct SceLibraryEntryTable *entry;
	struct SceLibraryStubTable *stub;
	u32 *pnid;
	resolver_config *resolver;

	offset = 0;

	while(offset < size) {
		entry = buf + offset;
		stubcount = entry->stubcount;
		resolver = get_nid_resolver(entry->libname);

		if(resolver != NULL) {
			for (i=0; i<stubcount; i++) {
				pnid = entry->entrytable + (i << 2);
				*pnid = resolve_nid(resolver, *pnid);
			}
		}

		offset += entry->len << 2;
	}

	memset(g_custom, 0, sizeof(g_custom));
	g_custom_cnt = 0;
	g_buf = buf;
	ret = (*sceKernelLinkLibraryEntries)(buf, size);

	offset = 0;

	while(offset < size) {
		stub = buf + offset;
		stubcount = stub->stubcount;

		for(i=0; i<NELEMS(g_missing_resolver); ++i) {
			mark_missing_NID(stub, g_missing_resolver[i]);
		}
		
		offset += stub->len << 2;
	}

	for(i=0; i<NELEMS(g_custom); ++i) {
		void *import_addr;

		import_addr = g_custom[i].import_addr;

		if(import_addr != NULL) {
			_sw(MAKE_JUMP(g_custom[i].fp), (u32)import_addr);
			_sw(NOP, (u32)(import_addr+4));
			sceKernelDcacheWritebackInvalidateRange(import_addr, 8);
			sceKernelIcacheInvalidateRange(import_addr, 8);
		}
	}

	return ret;
}

void setup_nid_resolver(void)
{
	SceModule2 *modmgr, *loadcore;

	modmgr = (SceModule2*)sceKernelFindModuleByName("sceModuleManager");
	loadcore = (SceModule2*)sceKernelFindModuleByName("sceLoaderCore");

	// Sony removed sceKernelIcacheClearAll's export
	// It's at 0x77CC+@LoadCore@ in 6.35
	missing_LoadCoreForKernel_entries[0].fp = (0x77CC + loadcore->text_addr);

	sceKernelLinkLibraryEntries = (void*)(loadcore->text_addr+0x000011D4);
	_sw(MAKE_CALL(_sceKernelLinkLibraryEntries), modmgr->text_addr+0x0000844C);
	sync_cache();
}

void resolve_syscon_driver(SceModule *mod)
{
	SceModule2 *syscon;

	syscon = (SceModule2*)sceKernelFindModuleByName("sceSYSCON_Driver");

	if(syscon == NULL)
		return;

	 missing_sceSyscon_driver_entries[0].fp = (syscon->text_addr + 0x00002C6C); // _sceSysconPowerStandby
}
