#include <pspsdk.h>
#include "popcorn_patch_offset.h"

#if !defined(CONFIG_635) && !defined(CONFIG_620) && !defined(CONFIG_639)
#error You have to define CONFIG_620 or CONFIG_635 or CONFIG_639
#endif

#ifdef CONFIG_639
PatchOffset g_639_offsets = {
	.fw_version = FW_639,
	.popsman_patch = {
		.sceIoOpenImport = 0x00003BA8,
		.sceIoLseekImport = 0x00003BB0,
		.sceIoIoctlImport = 0x00003BB8,
		.sceIoReadImport = 0x00003BC0,
		.sceIoReadAsyncImport = 0x00003BD8,
		.sceIoGetstatImport = 0x00003BE0,
		.sceIoCloseImport = 0x00003BD0,
		.get_rif_path = 0x00000190,
		.get_rif_path_call1 = 0x00002798,
		.get_rif_path_call2 = 0x00002C58,
		.sceNpDrmGetVersionKeyCall = 0x000029C4,
		.scePspNpDrm_driver_9A34AC9F_Call = 0x00002DA8,
		.scePopsManLoadModuleCheck = 0x00001E80,
		.sceDrmBBCipherInitImport = 0x00003CC0,
		.sceDrmBBCipherUpdateImport = 0x00003CB8,
		.sceDrmBBCipherFinalImport = 0x00003CD8,
		.sceDrmBBMacInitImport = 0x00003CC8,
		.sceDrmBBMacUpdateImport = 0x00003CD0,
		.sceDrmBBMacFinalImport = 0x00003CE0,
		.sceDrmBBMacFinal2Import = 0x00003CE8,
	},
	.pops_patch = {
		.decomp = {
			{ 0x000D5654, 0x0000DC04 }, // 01G
			{ 0x000D671C, 0x0000DC04 }, // 02G
			{ 0x000D671C, 0x0000DC04 }, // 03G
			{ 0x000D670C, 0x0000DC08 }, // 04G
			{ 0x000D8488, 0x0000E218 }, // 05G TODO
			{ 0x00000000, 0x00000000 }, // unused
			{ 0x000D670C, 0x0000DC08 }, // 07G
			{ 0x00000000, 0x00000000 }, // unused
			{ 0x000D670C, 0x0000DC08 }, // 09G
		},
		.ICON0SizeOffset = {
			0x00036F54, // 01G
			0x00037F90, // 02G
			0x00037F90, // 03G
			0x00037F90, // 04G
			0x00039B5C, // 05G TODO
			0xDEADBEEF, // unused
			0x00037F90, // 07G
			0xDEADBEEF, // unused
			0x00037F90, // 09G
		},
		.manualNameCheck = {
			0x0002541C, // 01G
			0x00025928, // 02G
			0x00025928, // 03G
			0x00025928, // 04G
			0x00025FA0, // 05G TODO
			0xDEADBEEF, // unused
			0x00025928, // 07G
			0xDEADBEEF, // unused
			0x00025928, // 09G
		},
		.sceMeAudio_67CD7972_NID = 0x67CD7972,
	},
};
#endif

#ifdef CONFIG_635
PatchOffset g_635_offsets = {
	.fw_version = FW_635,
	.popsman_patch = {
		.sceIoOpenImport = 0x00003B98,
		.sceIoLseekImport = 0x00003BA0,
		.sceIoIoctlImport = 0x00003BA8,
		.sceIoReadImport = 0x00003BB0,
		.sceIoReadAsyncImport = 0x00003BC8,
		.sceIoGetstatImport = 0x00003BD0,
		.sceIoCloseImport = 0x00003BC0,
		.get_rif_path = 0x00000190,
		.get_rif_path_call1 = 0x00002798,
		.get_rif_path_call2 = 0x00002C58,
		.sceNpDrmGetVersionKeyCall = 0x000029C4,
		.scePspNpDrm_driver_9A34AC9F_Call = 0x00002DA8,
		.scePopsManLoadModuleCheck = 0x00001E80,
		.sceDrmBBCipherInitImport = 0x00003CB0,
		.sceDrmBBCipherUpdateImport = 0x00003CA8,
		.sceDrmBBCipherFinalImport = 0x00003CC8,
		.sceDrmBBMacInitImport = 0x00003CB8,
		.sceDrmBBMacUpdateImport = 0x00003CC0,
		.sceDrmBBMacFinalImport = 0x00003CD0,
		.sceDrmBBMacFinal2Import = 0x00003CD8,
	},
	.pops_patch = {
		.decomp = {
			{ 0x000D5404, 0x0000DAC0 }, // 01G
			{ 0x000D64BC, 0x0000DAC0 }, // 02G
			{ 0x000D64BC, 0x0000DAC0 }, // 03G
			{ 0x000D64FC, 0x0000DB00 }, // 04G
			{ 0x000D8488, 0x0000E218 }, // 05G
			{ 0x00000000, 0x00000000 }, // unused
			{ 0x000D64FC, 0x0000DB00 }, // 07G
			{ 0x00000000, 0x00000000 }, // unused
			{ 0x000D64FC, 0x0000DB00 }, // 09G
		},
		.ICON0SizeOffset = {
			0x00036CF8, // 01G
			0x00037D34, // 02G
			0x00037D34, // 03G
			0x00037D74, // 04G
			0x00039B5C, // 05G
			0xDEADBEEF, // unused
			0x00037D74, // 07G
			0xDEADBEEF, // unused
			0x00037D74, // 09G
		},
		.manualNameCheck = {
			0x000251C0, // 01G
			0x000256CC, // 02G
			0x000256CC, // 03G
			0x0002570C, // 04G
			0x00025FA0, // 05G
			0xDEADBEEF, // unused
			0x0002570C, // 07G
			0xDEADBEEF, // unused
			0x0002570C, // 09G
		},
		.sceMeAudio_67CD7972_NID = 0x67CD7972,
	},
};
#endif

#ifdef CONFIG_620
PatchOffset g_620_offsets = {
	.fw_version = FW_620,
	.popsman_patch = {
		.sceIoOpenImport = 0x00003CD4,
		.sceIoLseekImport = 0x00003CDC,
		.sceIoIoctlImport = 0x00003CE4,
		.sceIoReadImport = 0x00003CEC,
		.sceIoReadAsyncImport = 0x00003D04,
		.sceIoGetstatImport = 0x00003D0C,
		.sceIoCloseImport = 0x00003CFC,
		.get_rif_path = 0x00000190,
		.get_rif_path_call1 = 0x00002824,
		.get_rif_path_call2 = 0x00002CE4,
		.sceNpDrmGetVersionKeyCall = 0x00002A50,
		.scePspNpDrm_driver_9A34AC9F_Call = 0x00002E34,
		.scePopsManLoadModuleCheck = 0x00001EA8,
		.sceDrmBBCipherInitImport = 0x00003DEC,
		.sceDrmBBCipherUpdateImport = 0x00003DE4,
		.sceDrmBBCipherFinalImport = 0x00003E04,
		.sceDrmBBMacInitImport = 0x00003DF4,
		.sceDrmBBMacUpdateImport = 0x00003DFC,
		.sceDrmBBMacFinalImport = 0x00003E0C,
		.sceDrmBBMacFinal2Import = 0x00003E14,
	},
	.pops_patch = {
		.decomp = {
			{ 0x000DA600, 0x0000DE18 }, // 01G
			{ 0x000DA600, 0x0000DE18 }, // 02G
			{ 0x000DA600, 0x0000DE18 }, // 03G
			{ 0x000DA630, 0x0000DE1C }, // 04G
			{ 0x000DC5CC, 0x0000E534 }, // 05G
			{ 0xDEADBEEF, 0xDEADBEEF }, // unused
			{ 0xDEADBEEF, 0xDEADBEEF }, // unused
			{ 0xDEADBEEF, 0xDEADBEEF }, // unused
			{ 0xDEADBEEF, 0xDEADBEEF }, // unused
		},
		.ICON0SizeOffset = {
			0x0003BCD0, // 01G
			0x0003BCD0, // 02G
			0x0003BCD0, // 03G
			0x0003BCFC, // 04G
			0x0003DAE4, // 05G
			0xDEADBEEF, // unused
			0xDEADBEEF, // unused
			0xDEADBEEF, // unused
			0xDEADBEEF, // unused
		},
		.manualNameCheck = {
			0x00029710, // 01G
			0x00029710, // 02G
			0x00029710, // 03G
			0x0002973C, // 04G
			0x00029FD0, // 05G
			0xDEADBEEF, // unused
			0xDEADBEEF, // unused
			0xDEADBEEF, // unused
			0xDEADBEEF, // unused
		},
		.sceMeAudio_67CD7972_NID = 0xF43E573A,
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
