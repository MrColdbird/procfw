#include <pspsdk.h>
#include <pspsysmem_kernel.h>
#include <pspkernel.h>
#include <psputilsforkernel.h>
#include <pspsysevent.h>
#include <pspiofilemgr.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "libs.h"
#include "systemctrl.h"
#include "printk.h"
#include "xmbiso.h"
#include "systemctrl_se.h"
#include "main.h"

static STMOD_HANDLER previous;
SEConfig conf;

static void patch_sysconf_plugin_module(u32 text_addr);
static void patch_game_plugin_module(u32 text_addr);
static void patch_vsh_module(SceModule2 * mod);

static void hook_iso_file_io(SceModule2 * mod);
static void hook_iso_directory_io(SceModule2 * mod);

static int vshpatch_module_chain(SceModule2 *mod)
{
	u32 text_addr;

	text_addr = mod->text_addr;

	if(0 == strcmp(mod->modname, "sysconf_plugin_module")) {
		patch_sysconf_plugin_module(text_addr);
		sync_cache();
	}

	if(0 == strcmp(mod->modname, "game_plugin_module")) {
		patch_game_plugin_module(text_addr);
		hook_iso_file_io(mod);
		hook_iso_directory_io(mod);
		sync_cache();
	}

	if(0 == strcmp(mod->modname, "vsh_module")) {
		patch_vsh_module(mod);
		hook_iso_file_io(mod);
		sync_cache();
	}

	if (previous)
		return (*previous)(mod);

	return 0;
}

static inline void ascii2utf16(char *dest, const char *src)
{
	while(*src != '\0') {
		*dest++ = *src;
		*dest++ = '\0';
		src++;
	}

	*dest++ = '\0';
	*dest++ = '\0';
}

static void patch_sysconf_plugin_module(u32 text_addr)
{
	void *p;
	char str[20];

#ifdef NIGHTLY_VERSION
	sprintf(str, "6.35 PRO-r%d", NIGHTLY_VERSION);
#else
	sprintf(str, "6.35 PRO-%c", 'A'+(sctrlHENGetVersion()&0xF)-1);
#endif

	p = (void*)(text_addr + 0x2A1FC);
	ascii2utf16(p, str);

	_sw(0x3C020000 | ((u32)(p) >> 16), text_addr+0x18F3C); // lui $v0, 
	_sw(0x34420000 | ((u32)(p) & 0xFFFF), text_addr+0x18F40); // or $v0, $v0, 

	if (conf.machidden) {
		p = (void*)(text_addr + 0x2E4D8);
		sprintf(str, "[ Hidden: 0%dg ]", psp_model+1);
		ascii2utf16(p, str);
	}

	sync_cache();
}

static void patch_game_plugin_module(u32 text_addr)
{
	//disable executable check for normal homebrew
	_sw(0x03E00008, text_addr+0x202A8); // jr $ra
	_sw(0x00001021, text_addr+0x202AC); // move $v0, $zr

	//kill ps1 eboot check
	_sw(0x03E00008, text_addr + 0x20BC8); //jr $ra
	_sw(0x00001021, text_addr + 0x20BCC); // move $v0, $zr
}

static void patch_vsh_module(SceModule2 * mod)
{
	//enable homebrew boot
	_sw(0, mod->text_addr+0x12230);
	_sw(0, mod->text_addr+0x11FD8);
	_sw(0, mod->text_addr+0x11FE0);

	//loadexec calls to vsh_bridge
	u32 nids[2] = { /* ms0 */ 0x59BBA567, /* ef0 */ 0xD4BA5699 };

	//hook imports
	int i = 0; for(; i < NELEMS(nids); i++) hook_import_bynid((SceModule *)mod, "sceVshBridge", nids[i], gameloadexec, 1);
}

static void hook_iso_file_io(SceModule2 * mod)
{
	//hook file io
	hook_import_bynid((SceModule *)mod, "IoFileMgrForUser", 0x109F50BC, gameopen, 1);
	hook_import_bynid((SceModule *)mod, "IoFileMgrForUser", 0x6A638D83, gameread, 1);
	hook_import_bynid((SceModule *)mod, "IoFileMgrForUser", 0x810C4BC3, gameclose, 1);
	hook_import_bynid((SceModule *)mod, "IoFileMgrForUser", 0x27EB27B8, gamelseek, 1);
	hook_import_bynid((SceModule *)mod, "IoFileMgrForUser", 0xACE946E8, gamegetstat, 1);
	hook_import_bynid((SceModule *)mod, "IoFileMgrForUser", 0xF27A9C51, gameremove, 1);
	hook_import_bynid((SceModule *)mod, "IoFileMgrForUser", 0x1117C65F, gamermdir, 1);
}

static void hook_iso_directory_io(SceModule2 * mod)
{
	//hook directory io
	hook_import_bynid((SceModule *)mod, "IoFileMgrForUser", 0xB29DDF9C, gamedopen, 1);
	hook_import_bynid((SceModule *)mod, "IoFileMgrForUser", 0xE3EB004C, gamedread, 1);
	hook_import_bynid((SceModule *)mod, "IoFileMgrForUser", 0xEB092469, gamedclose, 1);
}

int vshpatch_init(void)
{
	sctrlSEGetConfig(&conf);
	previous = sctrlHENSetStartModuleHandler(&vshpatch_module_chain);

	return 0;
}
