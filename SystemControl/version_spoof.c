#include <pspsdk.h>
#include <pspsysmem_kernel.h>
#include <pspkernel.h>
#include <psputilsforkernel.h>
#include <pspsysevent.h>
#include <pspiofilemgr.h>
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "printk.h"
#include "libs.h"
#include "utils.h"
#include "rebootex_conf.h"

int sceResmgr_driver_9DC14891(u8 *buf, u32 size, u32* newsize);

static const char *g_ver_checklist[] = {
	"release:",
	"build:",
	"system:",
	"vsh:",
	"target:",
};

static int load_version_txt(void *buf, int size)
{
	SceUID fd;

   	fd = sceIoOpen("ms0:/seplugins/version.txt", PSP_O_RDONLY, 0777);

	if(fd < 0) {
		fd = sceIoOpen("ef0:/seplugins/version.txt", PSP_O_RDONLY, 0777);
	}

	if(fd < 0) {
		return fd;
	}

	size = sceIoRead(fd, buf, size);
	sceIoClose(fd);

	return size;
}

static int check_valid_version_txt(const void *buf, int size)
{
	const char *p;
	int i;

	p = buf;

	if(size < 159) {
		return -1;
	}

	for(i=0; i<NELEMS(g_ver_checklist); ++i) {
		if(p - (const char*)buf >= size) {
			return -2;
		}

		if(0 != strncmp(p, g_ver_checklist[i], strlen(g_ver_checklist[i]))) {
			return -3;
		}

		while(*p != '\n' && p - (const char*)buf < size) {
			p++;
		}

		if(p - (const char*)buf >= size) {
			return -4;
		}

		p++;
	}

	return 0;
}

int versionspoofer(u8 *buf, u32 size, u32* newsize)
{
	int result = sceResmgr_driver_9DC14891(buf, size, newsize);

	if(!rebootex_conf.recovery_mode && conf.useversion && strstr((const char*)buf, "release:")) {
		char tmpbuf[164];
		int tmpsize;
		u32 k1;

		k1 = pspSdkSetK1(0);
		tmpsize = load_version_txt(tmpbuf, sizeof(tmpbuf));

		if(tmpsize > 0 && check_valid_version_txt(tmpbuf, tmpsize) == 0) {
			printk("%s: custom version.txt loaded\n", __func__);
			memcpy(buf, tmpbuf, tmpsize);
			*newsize = tmpsize;
			result = 0;
		}

		pspSdkSetK1(k1);
	}

	return result;
}

void patch_module_for_version_spoof(SceModule *mod1)
{
	u32 text_addr;
	SceModule2 *mod = (SceModule2*)mod1;
	
	if (mod == NULL) {
		return;
	}

	text_addr = mod->text_addr;

	if(text_addr & 0x80000000) {
		// kernel module
		hook_import_bynid((SceModule*)mod, "sceResmgr_driver", 0x9DC14891, (void*)versionspoofer, 0);
	} else {
		// user module (hooking requires 0x4001 export in exports.exp)
		hook_import_bynid((SceModule*)mod, "sceResmgr", 0x9DC14891, (void*)versionspoofer, 1);
	}
}
