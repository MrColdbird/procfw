#include <pspsdk.h>
#include <pspkernel.h>
#include <pspsysmem_kernel.h>
#include <pspthreadman_kernel.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "systemctrl.h"
#include "utils.h"
#include "printk.h"

static char *modname_blacklist[] = {
	"KHBBS_patch",
};

void module_blacklist(SceModule* mod_)
{
	SceModule2 *mod = (SceModule2*) mod_;
	size_t i;

	for(i=0; i<NELEMS(modname_blacklist); ++i) {
		if(0 == strcmp(modname_blacklist[i], mod->modname)) {
			printk("%s: found blacklisted module\n", __func__);
			MAKE_DUMMY_FUNCTION_RETURN_1(mod->entry_addr);
			sync_cache();
			break;
		}
	}
}
