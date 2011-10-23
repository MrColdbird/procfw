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
#include "galaxy_patch_offset.h"

#if !defined(CONFIG_635) && !defined(CONFIG_620) && !defined(CONFIG_639) && !defined(CONFIG_660)
#error You have to define CONFIG_620 or CONFIG_635 or CONFIG_639 or CONFIG_660
#endif

#ifdef CONFIG_660
PatchOffset g_660_offsets = {
	.fw_version = FW_660,
	.StoreFd = 0x00000188 + 0x00008900, /* See 0x00004D98 */
	.Data1 = 0x00005BB4 - 0x00005BA4 + 0x00000188 + 0x00008900,
	.Data2 = 0x00005BBC - 0x00005BA4 + 0x00000188 + 0x00008900,
	.Data3 = 0x00005BD0 - 0x00005BA4 + 0x00000188 + 0x00008900,
	.Data4 = 0x00005BD8 - 0x00005BA4 + 0x00000188 + 0x00008900,
	.Data5 = 0x00000114 + 0x00008900, /* See 0x000033B0 */
	.InitForKernelCall = 0x00003C5C,
	.Func1 = 0x00003C78,
	.Func2 = 0x00004414,
	.Func3 = 0x0000596C,
	.Func4 = 0x000036A8,
	.Func5 = 0x00004FEC,
	.Func6 = 0x0000505C,
	.sceIoClose = 0x00007D68,
	.sceKernelCreateThread = 0x00019264,
	.sceKernelStartThread = 0x00019408,
};
#endif

#ifdef CONFIG_639
PatchOffset g_639_offsets = {
	.fw_version = FW_639,
	.StoreFd = 0x00000188 + 0x00008900, /* See 0x00004D98 */
	.Data1 = 0x00005BB4 - 0x00005BA4 + 0x00000188 + 0x00008900,
	.Data2 = 0x00005BBC - 0x00005BA4 + 0x00000188 + 0x00008900,
	.Data3 = 0x00005BD0 - 0x00005BA4 + 0x00000188 + 0x00008900,
	.Data4 = 0x00005BD8 - 0x00005BA4 + 0x00000188 + 0x00008900,
	.Data5 = 0x00000114 + 0x00008900, /* See 0x000033B0 */
	.InitForKernelCall = 0x00003C5C,
	.Func1 = 0x00003C78,
	.Func2 = 0x00004414,
	.Func3 = 0x0000596C,
	.Func4 = 0x000036A8,
	.Func5 = 0x00004FEC,
	.Func6 = 0x0000505C,
	.sceIoClose = 0x00007D68,
	.sceKernelCreateThread = 0x000191B4,
	.sceKernelStartThread = 0x00019358,
};
#endif

#ifdef CONFIG_635
PatchOffset g_635_offsets = {
	.fw_version = FW_635,
	.StoreFd = 0x00000184 + 0x00008880, /* See 0x00004D38 */
	.Data1 = 0x00005BB4 - 0x00005BA4 + 0x00000184 + 0x00008880,
	.Data2 = 0x00005BBC - 0x00005BA4 + 0x00000184 + 0x00008880,
	.Data3 = 0x00005BD0 - 0x00005BA4 + 0x00000184 + 0x00008880,
	.Data4 = 0x00005BD8 - 0x00005BA4 + 0x00000184 + 0x00008880,
	.Data5 = 0x00000114 + 0x00008880, /* See 0x00003388 */
	.InitForKernelCall = 0x00003C34,
	.Func1 = 0x00003C4C,
	.Func2 = 0x000043B4,
	.Func3 = 0x0000590C,
	.Func4 = 0x00003680,
	.Func5 = 0x00004F8C,
	.Func6 = 0x00004FFC,
	.sceIoClose = 0x00007D08,
	.sceKernelCreateThread = 0x000191B4,
	.sceKernelStartThread = 0x00019358,
};
#endif

#ifdef CONFIG_620
PatchOffset g_620_offsets = {
	.fw_version = FW_620,
	.StoreFd = 0x00000184 + 0x000087C0, /* See 0x00004C58 */
	.Data1 = 0x00005BB4 - 0x00005BA4 + 0x00000184 + 0x000087C0,
	.Data2 = 0x00005BBC - 0x00005BA4 + 0x00000184 + 0x000087C0,
	.Data3 = 0x00005BD0 - 0x00005BA4 + 0x00000184 + 0x000087C0,
	.Data4 = 0x00005BD8 - 0x00005BA4 + 0x00000184 + 0x000087C0,
	.Data5 = 0x00000114 + 0x000087C0, /* See 0x0000332C */
	.InitForKernelCall = 0x00003BD8,
	.Func1 = 0x00003BF0,
	.Func2 = 0x00004358,
	.Func3 = 0x0000582C,
	.Func4 = 0x00003624,
	.Func5 = 0x00004EAC,
	.Func6 = 0x00004F1C,
	.sceIoClose = 0x00007C28,
	.sceKernelCreateThread = 0x000191B4,
	.sceKernelStartThread = 0x00019358,
};
#endif

PatchOffset *g_offs = NULL;

void setup_patch_offset_table(u32 fw_version)
{
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
