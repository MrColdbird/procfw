#include <pspsdk.h>
#include "stargate_patch_offset.h"

#if !defined(CONFIG_635) && !defined(CONFIG_620)
#error You have to define CONFIG_620 or CONFIG_635
#endif

#ifdef CONFIG_635
PatchOffset g_635_offsets = {
	.fw_version = FW_635,
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
	.mesgled_decrypt = 0x000000E0,
};
#endif

#ifdef CONFIG_620
PatchOffset g_620_offsets = {
	.fw_version = FW_620,
	.sceMesgLedDecryptGame1 = {
		0x000028EC, // 01g
		0x00002BD4, // 02g
		0x00002E9C, // 03g
		0x00002E9C, // 04g
		0x00003128, // 05g
		0xDEADBEEF, // 06g
		0xDEADBEEF, // 07g
		0xDEADBEEF, // 08g
		0xDEADBEEF, // 09g
	},
	.sceMesgLedDecryptGame2 = {
		0x00002CA0, // 01g
		0x00002FD0, // 02g
		0x000032E0, // 03g
		0x000032E0, // 04g
		0x000035B4, // 05g
		0xDEADBEEF, // 06g
		0xDEADBEEF, // 07g
		0xDEADBEEF, // 08g
		0xDEADBEEF, // 09g
	},
	.mesgled_decrypt = 0x000000E0,
};
#endif

PatchOffset *g_offs = NULL;

void setup_patch_offset_table(u32 fw_version)
{
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
