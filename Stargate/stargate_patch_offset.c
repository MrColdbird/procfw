#include <pspsdk.h>
#include "stargate_patch_offset.h"

PatchOffset g_635_offsets = {
	.fw_version = 0x06030510,
	.sceMesgLedDecryptGame1 = {
		0x00002B28, // 01g
		0x00002F08, // 02g
		0x000032A8, // 03g
		0x000032A8, // 04g
		0x00003614, // 05g
		0xDEADBEEF, // 06g
		0x000032A8, // 07g
		0xDEADBEEF, // 08g
		0x000032A8, // 09g
	},
	.sceMesgLedDecryptGame2 = {
		0x00002DC0, // 01g
		0x000031A0, // 02g
		0x00003540, // 03g
		0x00003540, // 04g
		0x000038AC, // 05g
		0xDEADBEEF, // 06g
		0x00003540, // 07g
		0xDEADBEEF, // 08g
		0x00003540, // 09g
	},
};

PatchOffset *g_offs = NULL;

void setup_patch_offset_table(u32 fw_version)
{
	if(fw_version == g_635_offsets.fw_version) {
		g_offs = &g_635_offsets;
	}
}
