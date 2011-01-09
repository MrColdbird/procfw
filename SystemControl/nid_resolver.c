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

int LoadCoreForKernel_07738DC3(void *unk0, SceLibraryStubTable *stub, u32 stub_size);
int LoadCoreForKernel_6565B132(SceLibraryStubTable *stub, u32 stub_size);

void nid_resolver(SceLibraryStubTable *stub)
{
	int i, j, k;

	if (stub == NULL)
		return;

	for(i=0; i<nid_fix_size; ++i) {
		if (!strcmp(stub->libname, nid_fix[i].name)) {
			int cnt = stub->vstubcount+stub->stubcount;

			for(j=0; j<cnt; ++j) {
				for(k=0; k<nid_fix[i].nidcount; ++k) {
					if(stub->nidtable[j] == nid_fix[i].nidtable[k].old) {
						stub->nidtable[j] = nid_fix[i].nidtable[k].new;
						printk("%s: %s_%08X->%s_%08X\n", __func__, stub->libname, nid_fix[i].nidtable[k].old, stub->libname, nid_fix[i].nidtable[k].new);

						break;
					}
				}
			}
		}
	}
}

static int (*aLinkLibEntries)(u32 unk0, SceLibraryStubTable* stub, u32 is_user_mode) = NULL;

static int _aLinkLibEntries(u32 unk0, SceLibraryStubTable* stub, u32 is_user_mode)
{
	int ret;
	int cnt;
   
	ret = (*aLinkLibEntries)(unk0, stub, is_user_mode);
	cnt = stub->vstubcount+stub->stubcount;

	if (stub->nidtable != NULL) {
		nid_resolver(stub);
	}

	return ret;
}

void setup_nid_resolver(u32 text_addr)
{
	aLinkLibEntries = (void*)(text_addr+0x3BCC);
	_sw(MAKE_CALL(_aLinkLibEntries), text_addr+0x3468);
}
