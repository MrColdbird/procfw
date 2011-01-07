#include <pspsdk.h>
#include <pspsysmem_kernel.h>
#include <pspkernel.h>
#include <psputilsforkernel.h>
#include <pspsysevent.h>
#include <pspiofilemgr.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "systemctrl.h"
#include "printk.h"

static STMOD_HANDLER previous;

static void patch_sysconf_plugin_module(u32 text_addr);
static void patch_game_plugin_module(u32 text_addr);
static void patch_vsh_module(u32 text_addr);

static int vshpatch_module_chain(SceModule2 *mod)
{
	u32 text_addr;

	text_addr = mod->text_addr;

	if(0 == strcmp(mod->modname, "sysconf_plugin_module")) {
		patch_sysconf_plugin_module(text_addr);
	}

	if(0 == strcmp(mod->modname, "game_plugin_module")) {
		patch_game_plugin_module(text_addr);
	}

	if(0 == strcmp(mod->modname, "vsh_module")) {
		patch_vsh_module(text_addr);
	}

	sync_cache();

	if (previous)
		return (*previous)(mod);

	return 0;
}

// 6.35 PRO
static u8 new_version_str[] = {
	0x36, 0x00, 0x2e, 0x00, 0x33, 0x00, 0x35, 0x00, 0x20, 0x00, 0x50, 0x00, 0x52, 0x00, 0x4F, 0x00, 0x00, 0x00
};

static void patch_sysconf_plugin_module(u32 text_addr)
{
	void *p;

	p = (void*)(text_addr + 0x2A1FC);
	memcpy(p, new_version_str, sizeof(new_version_str));

	_sw(0x3C020000 | ((u32)(p) >> 16), text_addr+0x18F3C); // lui $v0, 
	_sw(0x34420000 | ((u32)(p) & 0xFFFF), text_addr+0x18F40); // or $v0, $v0, 
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

static void patch_vsh_module(u32 text_addr)
{
	_sw(0, text_addr+0x12230);
	_sw(0, text_addr+0x11FD8);
	_sw(0, text_addr+0x11FE0);
}

int vshpatch_init(void)
{
	previous = sctrlHENSetStartModuleHandler(&vshpatch_module_chain);

	sync_cache();

	return 0;
}

