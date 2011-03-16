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
				printk("%s: %s_%08X resolved(fp: 0x%08X)\n", __func__, libname, nid, fp);
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

void setup_nid_resolver(void)
{
	SceModule2 *modmgr, *loadcore;

	modmgr = (SceModule2*)sceKernelFindModuleByName("sceModuleManager");
	loadcore = (SceModule2*)sceKernelFindModuleByName("sceLoaderCore");

	missing_LoadCoreForKernel_entries[0].fp = (loadcore->text_addr + g_offs->nid_resolver_patch.sceKernelIcacheClearAll);
	sceKernelLinkLibraryEntries = (void*)(loadcore->text_addr + g_offs->nid_resolver_patch.sceKernelLinkLibraryEntries);
	sceKernelLinkLibraryEntriesForUser = (void*)(loadcore->text_addr + g_offs->nid_resolver_patch.sceKernelLinkLibraryEntriesForUser);
	_sw(MAKE_CALL(_sceKernelLinkLibraryEntries), modmgr->text_addr + g_offs->nid_resolver_patch.sceKernelLinkLibraryEntriesCall);
	_sw(MAKE_CALL(_sceKernelLinkLibraryEntriesForUser), modmgr->text_addr + g_offs->nid_resolver_patch.sceKernelLinkLibraryEntriesForUserCall);
}

void resolve_syscon_driver(SceModule *mod)
{
	SceModule2 *syscon;

	syscon = (SceModule2*)mod;

	if(syscon == NULL)
		return;

	 missing_sceSyscon_driver_entries[0].fp = (syscon->text_addr + g_offs->nid_resolver_patch.sceSysconPowerStandby);
}
