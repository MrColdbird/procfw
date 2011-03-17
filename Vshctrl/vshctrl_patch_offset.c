#include <pspsdk.h>
#include "vshctrl_patch_offset.h"

PatchOffset g_635_offsets = {
	.fw_version = 0x06030510,
	.vshctrl_patch = {
		.sceDisplaySetHoldMode = 0x00005618,
		.sceDisplaySetHoldModeCall = 0x00001A14,
		.HibBlockCheck = 0x000051A8,
		.SystemVersionStr = 0x0002A1FC,
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

PatchOffset *g_offs = NULL;

void setup_patch_offset_table(u32 fw_version)
{
	if(fw_version == g_635_offsets.fw_version) {
		g_offs = &g_635_offsets;
	}
}
