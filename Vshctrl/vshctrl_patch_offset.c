#include <pspsdk.h>
#include "vshctrl_patch_offset.h"

#if !defined(CONFIG_635) && !defined(CONFIG_620) && !defined(CONFIG_639)
#error You have to define CONFIG_620 or CONFIG_635 or CONFIG_639
#endif

#ifdef CONFIG_639
PatchOffset g_639_offsets = {
	.fw_version = FW_639,
	.vshctrl_patch = {
		.sceDisplaySetHoldMode = 0x00005618,
		.sceDisplaySetHoldModeCall = 0x00001A14,
		.HibBlockCheck = 0x000051A8,
		.SystemVersionStr = 0x0002A1FC,
		.SystemVersionMessage = "6.39 PRO-%c",
		.SystemVersion = 0x00018F3C,
		.MacAddressStr = 0x0002E4D8,
		.HomebrewCheck = 0x000202A8,
		.PopsCheck = 0x00020BC8,
		.MultiDiscPopsCheck = 0x00014634,
		.HidePicCheck1 = 0x0001D5DC,
		.HidePicCheck2 = 0x0001D5E8,
		.SkipGameBootSubroute = 0x00019294,
		.SkipGameBoot = 0x00018F14,
		.RifFileCheck = 0x000203AC,
		.RifCompareCheck = 0x000203D4,
		.RifTypeCheck = 0x000203E8,
		.RifNpDRMCheck = 0x0002042C,
		.sceCtrlReadBufferPositiveNID = 0x9F3038AC,
		.htmlviewer_save_location = 0x0001C7FC,
	},
	.msvideo_main_patch = {
		.checks = {
			0x0003AED4,
			0x0003AF5C,		
			0x0003D79C,
			0x0003D9F8,
			0x00044150,
			0x00074550,
			0x00088BA0,
			0x0003D714,
			0x0003D75C,
			0x000431F8,
		},
	},
	.vsh_module_patch = {
		.checks = {
			0x00012230,
			0x00011FD8,
			0x00011FE0,
		},
		.loadexecNID1 = 0x59BBA567,
		.loadexecNID2 = 0xD4BA5699,
	},
	.custom_update_patch = {
		.UpdatePluginImageVersion1 = 0x000081B4,
		.UpdatePluginImageVersion2 = 0x000081BC,
		.SceUpdateDL_UpdateListStr = 0x000032BC,
		.SceUpdateDL_UpdateListCall1 = 0x00002044,
		.SceUpdateDL_UpdateListCall2 = 0x00002054,
		.SceUpdateDL_UpdateListCall3 = 0x00002080,
		.SceUpdateDL_UpdateListCall4 = 0x0000209C,
	},
};
#endif

#ifdef CONFIG_635
PatchOffset g_635_offsets = {
	.fw_version = FW_635,
	.vshctrl_patch = {
		.sceDisplaySetHoldMode = 0x00005618,
		.sceDisplaySetHoldModeCall = 0x00001A14,
		.HibBlockCheck = 0x000051A8,
		.SystemVersionStr = 0x0002A1FC,
		.SystemVersionMessage = "6.35 PRO-%c",
		.SystemVersion = 0x00018F3C,
		.MacAddressStr = 0x0002E4D8,
		.HomebrewCheck = 0x000202A8,
		.PopsCheck = 0x00020BC8,
		.MultiDiscPopsCheck = 0x00014634,
		.HidePicCheck1 = 0x0001D5DC,
		.HidePicCheck2 = 0x0001D5E8,
		.SkipGameBootSubroute = 0x00019294,
		.SkipGameBoot = 0x00018F14,
		.RifFileCheck = 0x000203AC,
		.RifCompareCheck = 0x000203D4,
		.RifTypeCheck = 0x000203E8,
		.RifNpDRMCheck = 0x0002042C,
		.sceCtrlReadBufferPositiveNID = 0x9F3038AC,
		.htmlviewer_save_location = 0x0001C7FC,
	},
	.msvideo_main_patch = {
		.checks = {
			0x0003AED4,
			0x0003AF5C,		
			0x0003D79C,
			0x0003D9F8,
			0x00044150,
			0x00074550,
			0x00088BA0,
			0x0003D714,
			0x0003D75C,
			0x000431F8,
		},
	},
	.vsh_module_patch = {
		.checks = {
			0x00012230,
			0x00011FD8,
			0x00011FE0,
		},
		.loadexecNID1 = 0x59BBA567,
		.loadexecNID2 = 0xD4BA5699,
	},
	.custom_update_patch = {
		.UpdatePluginImageVersion1 = 0x000081B4,
		.UpdatePluginImageVersion2 = 0x000081BC,
		.SceUpdateDL_UpdateListStr = 0x000032BC,
		.SceUpdateDL_UpdateListCall1 = 0x00002044,
		.SceUpdateDL_UpdateListCall2 = 0x00002054,
		.SceUpdateDL_UpdateListCall3 = 0x00002080,
		.SceUpdateDL_UpdateListCall4 = 0x0000209C,
	},
};
#endif

#ifdef CONFIG_620
PatchOffset g_620_offsets = {
	.fw_version = FW_620,
	.vshctrl_patch = {
		.sceDisplaySetHoldMode = 0x00005570,
		.sceDisplaySetHoldModeCall = 0x00001A14,
		.HibBlockCheck = 0x000050F8,
		.SystemVersionStr = 0x000298AC,
		.SystemVersionMessage = "6.20 PRO-%c",
		.SystemVersion = 0x00018920,
		.MacAddressStr = 0x0002DB90,
		.HomebrewCheck = 0x0001EB08,
		.PopsCheck = 0x0001F41C,
		.MultiDiscPopsCheck = 0x00013850,
		.HidePicCheck1 = 0x0001C098,
		.HidePicCheck2 = 0x0001C0A4,
		.SkipGameBootSubroute = 0x000181BC,
		.SkipGameBoot = 0x00017E5C,
		.RifFileCheck = 0x0001EC0C,
		.RifCompareCheck = 0x0001EC34,
		.RifTypeCheck = 0x0001EC48,
		.RifNpDRMCheck = 0x0001EC8C,
		.sceCtrlReadBufferPositiveNID = 0xD073ECA4,
		.htmlviewer_save_location = 0x0001C7C0,
	},
	.msvideo_main_patch = {
		.checks = {
			0x0003AB2C,
			0x0003ABB4,
			0x0003D3AC,
			0x0003D608,
			0x00043B98,
            0x00073A84,
            0x000880A0,
			0x0003D324,
            0x0003D36C,
            0x00042C40,
		},
	},
	.vsh_module_patch = {
		.checks = {
			0x00011D84,
			0x00011A70,
			0x00011A78,
		},
		.loadexecNID1 = 0x4ECCCDBC,
		.loadexecNID2 = 0x2D5C9178,
	},
	.custom_update_patch = {
		.UpdatePluginImageVersion1 = 0x0000819C,
		.UpdatePluginImageVersion2 = 0x000081A4,
		.SceUpdateDL_UpdateListStr = 0x000032BC,
		.SceUpdateDL_UpdateListCall1 = 0x00002044,
		.SceUpdateDL_UpdateListCall2 = 0x00002054,
		.SceUpdateDL_UpdateListCall3 = 0x00002080,
		.SceUpdateDL_UpdateListCall4 = 0x0000209C,
	},
};
#endif

PatchOffset *g_offs = NULL;

void setup_patch_offset_table(u32 fw_version)
{
#ifdef CONFIG_639
	if(fw_version == g_639_offsets.fw_version) {
		g_offs = &g_639_offsets;
	}
#endif

#ifdef CONFIG_635
	if(fw_version == g_635_offsets.fw_version) {
		g_offs = &g_635_offsets;
	}
#endif

#ifdef CONFIG_620
   	if(fw_version == g_620_offsets.fw_version) {
		g_offs = &g_620_offsets;
	}
#endif
}
