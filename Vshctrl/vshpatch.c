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
#include "strsafe.h"
#include "vshctrl_patch_offset.h"

extern int _sceCtrlReadBufferPositive(SceCtrlData *ctrl, int count);
extern void patch_sceUSB_Driver(void);

typedef struct _HookUserFunctions {
	u32 nid;
	void *func;
} HookUserFunctions;

static STMOD_HANDLER previous;
SEConfig conf;

static void patch_sysconf_plugin_module(SceModule2 *mod);
static void patch_game_plugin_module(u32 text_addr);
static void patch_vsh_module(SceModule2 * mod);

static void hook_iso_file_io(void);
static void hook_iso_directory_io(void);
static void patch_sceCtrlReadBufferPositive(SceModule2 *mod); 
static void patch_Gameboot(SceModule2 *mod); 
static void patch_hibblock(SceModule2 *mod); 
static void patch_msvideo_main_plugin_module(u32 text_addr);

static int vshpatch_module_chain(SceModule2 *mod)
{
	u32 text_addr;

	text_addr = mod->text_addr;

	if(0 == strcmp(mod->modname, "sysconf_plugin_module")) {
		patch_sysconf_plugin_module(mod);
		sync_cache();
	}

	if(0 == strcmp(mod->modname, "game_plugin_module")) {
		patch_game_plugin_module(text_addr);
		sync_cache();
	}

	if(0 == strcmp(mod->modname, "vsh_module")) {
		patch_vsh_module(mod);
		sync_cache();
	}

	if(0 == strcmp(mod->modname, "sceVshBridge_Driver")) {
		patch_sceCtrlReadBufferPositive(mod);
		patch_Gameboot(mod);

		if(psp_model == PSP_GO && conf.hibblock) {
			patch_hibblock(mod);
		}

		sync_cache();
	}

	if(0 == strcmp(mod->modname, "msvideo_main_plugin_module")) {
		patch_msvideo_main_plugin_module(text_addr);
		sync_cache();
	}

	if(0 == strcmp(mod->modname, "update_plugin_module")) {
		patch_update_plugin_module(text_addr);
		sync_cache();
	}

	if(conf.useownupdate && 0 == strcmp(mod->modname, "SceUpdateDL_Library")) {
		patch_SceUpdateDL_Library(text_addr);
		sync_cache();
	}

	if (previous)
		return (*previous)(mod);

	return 0;
}

// sceDisplay_driver_73CA5F45
static int (*sceDisplaySetHoldMode)(int a0) = NULL;

static int _sceDisplaySetHoldMode(int a0)
{
	if (conf.skipgameboot == 0) {
		return (*sceDisplaySetHoldMode)(a0);
	}

	return 0;
}

static void patch_sceCtrlReadBufferPositive(SceModule2 *mod)
{
	hook_import_bynid((SceModule*)mod, "sceCtrl_driver", 0x9F3038AC, _sceCtrlReadBufferPositive, 0);
}

static void patch_Gameboot(SceModule2 *mod)
{
	_sw(MAKE_CALL(_sceDisplaySetHoldMode), mod->text_addr + g_offs->vshctrl_patch.sceDisplaySetHoldModeCall);
	sceDisplaySetHoldMode = (void*)(mod->text_addr + g_offs->vshctrl_patch.sceDisplaySetHoldMode);
}

static void patch_hibblock(SceModule2 *mod)
{
	_sw(0x03E00008, mod->text_addr + g_offs->vshctrl_patch.HibBlockCheck);
	_sw(0x00001021, mod->text_addr + g_offs->vshctrl_patch.HibBlockCheck + 4);
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

static const char *g_cfw_dirs[] = {
	"/seplugins",
	"/ISO",
};

int myIoMkdir(const char *dir, SceMode mode)
{
	int ret, i;
	u32 k1;

	if(0 == strcmp(dir, "ms0:/PSP/GAME") || 
			0 == strcmp(dir, "ef0:/PSP/GAME")) {
		k1 = pspSdkSetK1(0);

		for(i=0; i<NELEMS(g_cfw_dirs); ++i) {
			char path[40];

			get_device_name(path, sizeof(path), dir);
			STRCAT_S(path, g_cfw_dirs[i]);
			sceIoMkdir(path, mode);
		}

		pspSdkSetK1(k1);
	}

	ret = sceIoMkdir(dir, mode);

	return ret;
}

static void patch_sysconf_plugin_module(SceModule2 *mod)
{
	void *p;
	char str[20];
	u32 text_addr;
	u32 minor_version;

	text_addr = mod->text_addr;

	minor_version = sctrlHENGetMinorVersion();

	sprintf(str, g_offs->vshctrl_patch.SystemVersionMessage, 'A'+(sctrlHENGetVersion()&0xF)-1);

	if(minor_version != 0) {
		sprintf(str+strlen(str), "%d", minor_version);
	}

	p = (void*)(text_addr + g_offs->vshctrl_patch.SystemVersionStr);
	ascii2utf16(p, str);

	_sw(0x3C020000 | ((u32)(p) >> 16), text_addr + g_offs->vshctrl_patch.SystemVersion); // lui $v0, 
	_sw(0x34420000 | ((u32)(p) & 0xFFFF), text_addr + g_offs->vshctrl_patch.SystemVersion + 4); // or $v0, $v0, 

	if (conf.machidden) {
		p = (void*)(text_addr + g_offs->vshctrl_patch.MacAddressStr);
		sprintf(str, "[ Hidden: 0%dg ]", psp_model+1);
		ascii2utf16(p, str);
	}

	hook_import_bynid((SceModule*)mod, "IoFileMgrForUser", 0x06A70004, myIoMkdir, 1);
	
	sync_cache();
}

static void patch_game_plugin_module(u32 text_addr)
{
	//disable executable check for normal homebrew
	_sw(0x03E00008, text_addr + g_offs->vshctrl_patch.HomebrewCheck); // jr $ra
	_sw(0x00001021, text_addr + g_offs->vshctrl_patch.HomebrewCheck + 4); // move $v0, $zr

	//kill ps1 eboot check
	_sw(0x03E00008, text_addr + g_offs->vshctrl_patch.PopsCheck); //jr $ra
	_sw(0x00001021, text_addr + g_offs->vshctrl_patch.PopsCheck + 4); // move $v0, $zr

	//kill multi-disc ps1 check
	_sw(NOP, text_addr + g_offs->vshctrl_patch.MultiDiscPopsCheck);

	if (conf.hidepic) {
		_sw(0x00601021, text_addr + g_offs->vshctrl_patch.HidePicCheck1);
		_sw(0x00601021, text_addr + g_offs->vshctrl_patch.HidePicCheck2);
	}
	
	if (conf.skipgameboot) {
		_sw(MAKE_CALL(text_addr + g_offs->vshctrl_patch.SkipGameBootSubroute), text_addr + g_offs->vshctrl_patch.SkipGameBoot);
		_sw(0x24040002, text_addr + g_offs->vshctrl_patch.SkipGameBoot + 4);
	}

	// disable check for custom psx eboot restore 
	// rif file check
	_sw(0x00001021, text_addr + g_offs->vshctrl_patch.RifFileCheck);
	// rif content memcmp check
	_sw(NOP, text_addr + g_offs->vshctrl_patch.RifCompareCheck);
	// some type check, branch it
	_sw(0x10000010, text_addr + g_offs->vshctrl_patch.RifTypeCheck);
	// fake npdrm call
	_sw(0x00001021, text_addr + g_offs->vshctrl_patch.RifNpDRMCheck);
}

static void patch_msvideo_main_plugin_module(u32 text_addr)
{
	_sh(0xFE00, text_addr + g_offs->msvideo_main_patch.checks[0]);
	_sh(0xFE00, text_addr + g_offs->msvideo_main_patch.checks[1]);
	_sh(0xFE00, text_addr + g_offs->msvideo_main_patch.checks[2]);
	_sh(0xFE00, text_addr + g_offs->msvideo_main_patch.checks[3]);

	_sh(0xFE00, text_addr + g_offs->msvideo_main_patch.checks[4]);
	_sh(0xFE00, text_addr + g_offs->msvideo_main_patch.checks[5]);
	_sh(0xFE00, text_addr + g_offs->msvideo_main_patch.checks[6]);

	_sh(0x4003, text_addr + g_offs->msvideo_main_patch.checks[7]);
	_sh(0x4003, text_addr + g_offs->msvideo_main_patch.checks[8]);
	_sh(0x4003, text_addr + g_offs->msvideo_main_patch.checks[9]);
}

static void patch_vsh_module(SceModule2 * mod)
{
	//enable homebrew boot
	_sw(NOP, mod->text_addr + g_offs->vsh_module_patch.checks[0]);
	_sw(NOP, mod->text_addr + g_offs->vsh_module_patch.checks[1]);
	_sw(NOP, mod->text_addr + g_offs->vsh_module_patch.checks[2]);

	//loadexec calls to vsh_bridge
	u32 nids[2] = { /* ms0 */ 0x59BBA567, /* ef0 */ 0xD4BA5699 };

	//hook imports
	int i = 0; for(; i < NELEMS(nids); i++) hook_import_bynid((SceModule *)mod, "sceVshBridge", nids[i], gameloadexec, 1);
}

static void hook_iso_file_io(void)
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
		void *fp;

		fp = (void*)sctrlHENFindFunction("sceIOFileManager", "IoFileMgrForUser", hook_list[i].nid);

		if(fp != NULL) {
			sctrlHENPatchSyscall(fp, hook_list[i].func);
		}
	}
}

static void hook_iso_directory_io(void)
{
	HookUserFunctions hook_list[] = {
		{ 0xB29DDF9C, gamedopen  }, 
		{ 0xE3EB004C, gamedread  }, 
		{ 0xEB092469, gamedclose },
	};

	int i; for(i=0; i<NELEMS(hook_list); ++i) {
		void *fp;

		fp = (void*)sctrlHENFindFunction("sceIOFileManager", "IoFileMgrForUser", hook_list[i].nid);

		if(fp != NULL) {
			sctrlHENPatchSyscall(fp, hook_list[i].func);
		}
	}
}

int vshpatch_init(void)
{
	sctrlSEGetConfig(&conf);
	previous = sctrlHENSetStartModuleHandler(&vshpatch_module_chain);
	patch_sceUSB_Driver();
	vpbp_init();
	hook_iso_file_io();
	hook_iso_directory_io();

	return 0;
}
