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

static int (*aLinkLibEntries)(u32 unk0, SceLibraryStubTable* stub, u32 is_user_mode) = NULL;

static void nid_resolver(SceLibraryStubTable *stub)
{
	int i, cnt;
	u32 new;

	cnt = stub->vstubcount+stub->stubcount;
	
	for(i=0; i<cnt; ++i) {
		new = resolve_nid(stub->libname, stub->nidtable[i]);

		if (stub->nidtable[i] != new) {
			stub->nidtable[i] = new;
		}
	}
}

static int _aLinkLibEntries(u32 unk0, SceLibraryStubTable* stub, u32 is_user_mode)
{
	int ret;
   
	ret = (*aLinkLibEntries)(unk0, stub, is_user_mode);

	if (stub != NULL && stub->nidtable != NULL) {
		nid_resolver(stub);

#if 0
		printk("Name: %s Attr: 0x%08X\n", stub->libname, (stub->attribute << 16) | (*(u16*)stub->version));

		int i, cnt; for(i=0, cnt=stub->vstubcount+stub->stubcount; i<cnt; ++i) {
			printk("%s_%08X\n", stub->libname, stub->nidtable[i]);
		}
#endif
	}

	return ret;
}

void setup_nid_resolver(u32 loadcore)
{
	aLinkLibEntries = (void*)(loadcore+0x3BCC);
	_sw(MAKE_CALL(_aLinkLibEntries), loadcore+0x3468);
}

u32 resolve_nid(const char *libname, u32 nid)
{
	int i, j;
	u32 new;

	for(i=0; i<nid_fix_size; ++i) {
		if (!strcmp(libname, nid_fix[i].name)) {
			for(j=0; j<nid_fix[i].nidcount; ++j) {
				new = nid_fix[i].nidtable[j].new;
				
				if(new != UNKNOWNNID && nid == nid_fix[i].nidtable[j].old) {
					printk("%s: %s_%08X->%s_%08X\n", __func__, libname, nid, libname, new);

					return new;
				}
			}
		}
	}

	return nid;
}

static void resolve_sceKernelIcacheClearAll(SceModule *mod)
{
	void *address;
	SceModule2 *loadcore;

	// Sony removed sceKernelIcacheClearAll's export
	// It's at 0x77CC+@LoadCore@ in 6.35
	loadcore = (SceModule2*) sceKernelFindModuleByName("sceLoaderCore");
	address = (void*)(0x77CC + loadcore->text_addr);
	hook_import_bynid(mod, "LoadCoreForKernel", 0xD8779AC6, address, 0);
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

static SceUID proKernelSearchModuleByName(char * name)
{
	//find module by name
	SceModule2 * mod = (SceModule2 *)sceKernelFindModuleByName(name);

	//return uid
	return mod->modid;
}

static void resolve_removed_clib(SceModule *mod)
{
	hook_import_bynid(mod, "ModuleMgrForKernel", 0x04B7BD22, (void*)proKernelSearchModuleByName, 0);
	hook_import_bynid(mod, "SysclibForKernel", 0x89B79CB1, (void*)ownstrcspn, 0);
	hook_import_bynid(mod, "SysclibForKernel", 0x62AE052F, (void*)ownstrspn, 0);
	hook_import_bynid(mod, "SysclibForKernel", 0x87F8D2DA, (void*)ownstrtok, 0);
	hook_import_bynid(mod, "SysclibForKernel", 0x909C228B, (void*)0x88002E88, 0); // setjmp
	hook_import_bynid(mod, "SysclibForKernel", 0x18FE80DB, (void*)0x88002EC4, 0); // longjmp
}

void resolve_removed_nid(SceModule *mod)
{
	resolve_sceKernelIcacheClearAll(mod);
	resolve_removed_clib(mod);
}
