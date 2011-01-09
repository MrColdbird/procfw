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
	}

	return ret;
}

void setup_nid_resolver(u32 text_addr)
{
	aLinkLibEntries = (void*)(text_addr+0x3BCC);
	_sw(MAKE_CALL(_aLinkLibEntries), text_addr+0x3468);
}

u32 resolve_nid(const char *libname, u32 nid)
{
	int i, j;
	u32 new;

	for(i=0; i<nid_fix_size; ++i) {
		if (!strcmp(libname, nid_fix[i].name)) {
			for(j=0; j<nid_fix[i].nidcount; ++j) {
				new = nid_fix[i].nidtable[j].new;
				
				if(nid == nid_fix[i].nidtable[j].old) {
					printk("%s: %s_%08X->%s_%08X\n", __func__, libname, nid, libname, new);

					return new;
				}
			}
		}
	}

	return nid;
}
