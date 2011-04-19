#include <pspsdk.h>
#include <psploadcore.h>
#include <pspiofilemgr.h>
#include <stdio.h>
#include "strsafe.h"
#include "printk.h"
#include "utils.h"
#include "libs.h"

int hideIoDread(SceUID fd, SceIoDirent * dir)
{
	int result = sceIoDread(fd, dir);

	if(strcasecmp(dir->d_name, "iso") == 0 || strcasecmp(dir->d_name, "seplugins") == 0 || strcasecmp(dir->d_name, "isocache.bin") == 0) {
		result = sceIoDread(fd, dir);
	}

	return result;
}

// hide cfw folders, this avoids crashing the weird dj max portable 3 savegame algorithm
void hide_cfw_folder(SceModule * mod)
{
	hook_import_bynid(mod, "IoFileMgrForUser", 0xE3EB004C, &hideIoDread, 1);
}
