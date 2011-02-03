#include <pspsdk.h>
#include <pspsysmem_kernel.h>
#include <pspkernel.h>
#include <psputilsforkernel.h>
#include <pspsysevent.h>
#include <pspiofilemgr.h>
#include <pspctrl.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "libs.h"
#include "systemctrl.h"
#include "printk.h"
#include "xmbiso.h"
#include "systemctrl_se.h"
#include "main.h"
#include "virtual_pbp.h"

extern int _sceCtrlReadBufferPositive(SceCtrlData *ctrl, int count);
extern void patch_sceUSB_Driver(void);

typedef struct _HookUserFunctions {
	u32 nid;
	void *func;
} HookUserFunctions;

static STMOD_HANDLER previous;
SEConfig conf;

static void patch_sysconf_plugin_module(u32 text_addr);
static void patch_game_plugin_module(u32 text_addr);
static void patch_vsh_module(SceModule2 * mod);

static void hook_iso_file_io(SceModule2 * mod);
static void hook_iso_directory_io(SceModule2 * mod);
static void patch_sceCtrlReadBufferPositive(SceModule2 *mod); 
static void patch_Gameboot(SceModule2 *mod); 
static void patch_msvideo_main_plugin_module(u32 text_addr);

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

	if(0 == strcmp(mod->modname, "sceVshBridge_Driver")) {
		patch_sceCtrlReadBufferPositive(mod);
		patch_Gameboot(mod);
		sync_cache();
	}

	if(0 == strcmp(mod->modname, "msvideo_main_plugin_module")) {
		patch_msvideo_main_plugin_module(text_addr);
		sync_cache();
	}

	if (previous)
		return (*previous)(mod);

	return 0;
}

// sceDisplay_driver_73CA5F45
static int (*GameBoot) (void) = NULL;

static int Gameboot_Patched(void)
{
	if (conf.skipgameboot == 0) {
		return GameBoot();
	}

	return 0;
}

static void patch_sceCtrlReadBufferPositive(SceModule2 *mod)
{
	hook_import_bynid((SceModule*)mod, "sceCtrl_driver", 0x9F3038AC, _sceCtrlReadBufferPositive, 0);
}

static void patch_Gameboot(SceModule2 *mod)
{
	_sw(MAKE_CALL(Gameboot_Patched), mod->text_addr + 0x1A14);
	GameBoot = (void*)(mod->text_addr+0x5618);
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

	if (conf.hidepic) {
		_sw(0x00601021, text_addr + 0x0001D5DC);
		_sw(0x00601021, text_addr + 0x0001D5E8);
	}
	
	if (conf.skipgameboot) {
		_sw(MAKE_CALL(text_addr+0x00019294), text_addr + 0x00018F14);
		_sw(0x24040002, text_addr + 0x00018F18);
	}
}

static void patch_msvideo_main_plugin_module(u32 text_addr)
{
	_sh(0xFE00, text_addr + 0x0003AED4);
	_sh(0xFE00, text_addr + 0x0003AF5C);
	_sh(0xFE00, text_addr + 0x0003D79C);
	_sh(0xFE00, text_addr + 0x0003D9F8);

	_sh(0xFE00, text_addr + 0x00044150);
	_sh(0xFE00, text_addr + 0x00074550);
	_sh(0xFE00, text_addr + 0x00088BA0);

	_sh(0x4003, text_addr + 0x0003D714);
	_sh(0x4003, text_addr + 0x0003D75C);
	_sh(0x4003, text_addr + 0x000431F8);
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
	HookUserFunctions hook_list[] = {
		{ 0x109F50BC, gameopen,    },
		{ 0x6A638D83, gameread,    },
		{ 0x810C4BC3, gameclose,   },
		{ 0x27EB27B8, gamelseek,   },
		{ 0xACE946E8, gamegetstat, },
		{ 0xF27A9C51, gameremove,  },
		{ 0x1117C65F, gamermdir,   },
		{ 0x779103A0, gamerename,  },
		{ 0xB8A740F4, gamechstat,  },
	};

	int i; for(i=0; i<NELEMS(hook_list); ++i) {
		hook_import_bynid((SceModule *)mod, "IoFileMgrForUser", hook_list[i].nid, hook_list[i].func, 1);
	}
}

static void hook_iso_directory_io(SceModule2 * mod)
{
	HookUserFunctions hook_list[] = {
		{ 0xB29DDF9C, gamedopen  }, 
		{ 0xE3EB004C, gamedread  }, 
		{ 0xEB092469, gamedclose },
	};

	int i; for(i=0; i<NELEMS(hook_list); ++i) {
		hook_import_bynid((SceModule *)mod, "IoFileMgrForUser", hook_list[i].nid, hook_list[i].func, 1);
	}
}

int vshpatch_init(void)
{
	sctrlSEGetConfig(&conf);
	previous = sctrlHENSetStartModuleHandler(&vshpatch_module_chain);
	patch_sceUSB_Driver();
	vpbp_init();

	return 0;
}
