/*
 * This file is part of PRO CFW.

 * PRO CFW is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * PRO CFW is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PRO CFW. If not, see <http://www.gnu.org/licenses/ .
 */

#include <pspsdk.h>
#include "stargate_patch_offset.h"

#if !defined(CONFIG_635) && !defined(CONFIG_620) && !defined(CONFIG_639) && !defined(CONFIG_660) && !defined(CONFIG_661)
#error You have to define CONFIG_620 or CONFIG_635 or CONFIG_639 or CONFIG_660 or CONFIG_661
#endif

#if defined(CONFIG_660) || defined(CONFIG_661)
#ifdef CONFIG_660
PatchOffset g_660_offsets = {
	.fw_version = FW_660,
#endif
#ifdef CONFIG_661
PatchOffset g_661_offsets = {
	.fw_version = FW_661,
#endif
	.sceMesgLedDecryptGame1 = {
		0x00003160, // 01g
		0x000034F8, // 02g
		0x00003804, // 03g
		0x00003804, // 04g
		0x00003B28, // 05g
		0xDEADBEEF, // 06g
		0x00003804, // 07g
		0xDEADBEEF, // 08g
		0x00003804, // 09g
		0xDEADBEEF, // unused
		0x00003804, // 11G
	},
	.sceMesgLedDecryptGame2 = {
		0x000033F8, // 01g
		0x000037D8, // 02g
		0x00003B78, // 03g
		0x00003B78, // 04g
		0x00003EE4, // 05g
		0xDEADBEEF, // 06g
		0x00003B78, // 07g
		0xDEADBEEF, // 08g
		0x00003B78, // 09g
		0xDEADBEEF, // unused
		0x00003B78, // 11G
	},
	.mesgled_decrypt = 0x000000E0,
	.LoadExecForUser_362A956B_fix = 0x000009B4,
};
#endif

#ifdef CONFIG_639
PatchOffset g_639_offsets = {
	.fw_version = FW_639,
	.sceMesgLedDecryptGame1 = {
		0x00002BB8, // 01g
		0x00002F98, // 02g
		0x00003338, // 03g
		0x00003338, // 04g
		0x000036A4, // 05g
		0xDEADBEEF, // 06g
		0x00003338, // 07g
		0xDEADBEEF, // 08g
		0x00003338, // 09g
		0xDEADBEEF, // unused
		0xDEADBEEF, // 11G
	},
	.sceMesgLedDecryptGame2 = {
		0x00002E50, // 01g
		0x00003230, // 02g
		0x000035D0, // 03g
		0x000035D0, // 04g
		0x0000393C, // 05g
		0xDEADBEEF, // 06g
		0x000035D0, // 07g
		0xDEADBEEF, // 08g
		0x000035D0, // 09g
		0xDEADBEEF, // unused
		0xDEADBEEF, // 11G
	},
	.mesgled_decrypt = 0x000000E0,
	.LoadExecForUser_362A956B_fix = 0x000009B4,
};
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
		0xDEADBEEF, // unused
		0xDEADBEEF, // 11G
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
		0xDEADBEEF, // unused
		0xDEADBEEF, // 11G
	},
	.mesgled_decrypt = 0x000000E0,
	.LoadExecForUser_362A956B_fix = 0x000009B4,
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
		0xDEADBEEF, // unused
		0xDEADBEEF, // 11G
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
		0xDEADBEEF, // unused
		0xDEADBEEF, // 11G
	},
	.mesgled_decrypt = 0x000000E0,
	.LoadExecForUser_362A956B_fix = 0x000009B4,
};
#endif

PatchOffset *g_offs = NULL;

void setup_patch_offset_table(u32 fw_version)
{
#ifdef CONFIG_661
	if(fw_version == g_661_offsets.fw_version) {
		g_offs = &g_661_offsets;
	}
#endif

#ifdef CONFIG_660
	if(fw_version == g_660_offsets.fw_version) {
		g_offs = &g_660_offsets;
	}
#endif

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
