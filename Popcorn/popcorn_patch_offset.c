#include <pspsdk.h>
#include "popcorn_patch_offset.h"

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
			0x00000000, // unused
			0x00037D74, // 07G
			0x00000000, // unused
			0x00037D74, // 09G
		},
		.sceMeAudio_67CD7972_NID = 0x67CD7972,
	},
};

PatchOffset g_620_offsets = {
	.fw_version = FW_620,
};

PatchOffset *g_offs = NULL;

void setup_patch_offset_table(u32 fw_version)
{
	if(fw_version == g_635_offsets.fw_version) {
		g_offs = &g_635_offsets;
	} else if(fw_version == g_620_offsets.fw_version) {
		g_offs = &g_620_offsets;
	}
}
