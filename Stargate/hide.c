#include <pspsdk.h>
#include <psploadcore.h>
#include <pspiofilemgr.h>
#include <stdio.h>
#include "strsafe.h"
#include "printk.h"
#include "utils.h"
#include "libs.h"
#include "strsafe.h"

static char *g_blacklist[] = {
	"iso",
	"seplugins",
	"isocache.bin",
};

static inline int is_in_blacklist(const char *dname)
{
	int i;

	for(i=0; i<NELEMS(g_blacklist); ++i) {
		if(0 == strcasecmp(dname, g_blacklist[i])) {
			return 1;
		}
	}

	return 0;
}

int hideIoDread(SceUID fd, SceIoDirent * dir)
{
	int result = sceIoDread(fd, dir);

	if(result > 0 && is_in_blacklist(dir->d_name)) {
		result = sceIoDread(fd, dir);
	}

	return result;
}

// hide cfw folders, this avoids crashing the weird dj max portable 3 savegame algorithm
void hide_cfw_folder(SceModule * mod)
{
	hook_import_bynid(mod, "IoFileMgrForUser", 0xE3EB004C, &hideIoDread, 1);
}
