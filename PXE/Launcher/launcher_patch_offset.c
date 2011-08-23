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
#include "launcher_patch_offset.h"

#if !defined(CONFIG_635) && !defined(CONFIG_620) && !defined(CONFIG_639) && !defined(CONFIG_660)
#error You have to define CONFIG_620 or CONFIG_635 or CONFIG_639 or CONFIG_660
#endif

#ifdef CONFIG_660
PatchOffset g_660_offsets = {
	.fw_version = FW_660,
	.sysmem_patch = {
		.sceKernelIcacheInvalidateAll = 0x00000E98,
		.sceKernelDcacheWritebackInvalidateAll = 0x00000744,
		.sceKernelGetModel = 0x0000A0B0,
		.sceKernelPowerLockForUser = 0x0000CBB8,
		.sceKernelPowerLockForUser_data_offset = 0x000040F8,
	},
	.sceKernelFindModuleByName = 0x88017000 + 0x00006F98,
	.loadexec_patch_05g = {
		.LoadReboot = 0x00000000,
		.LoadRebootCall = 0x00002FA8,
		.RebootJump = 0x00002FF4,
	},
	.loadexec_patch_other= {
		.LoadReboot = 0x00000000,
		.LoadRebootCall = 0x00002D5C,
		.RebootJump = 0x00002DA8,
	},
	.patchRangeStart = 0xDEADBEEF,
	.patchRangeEnd = 0xDEADBEEF,
};
#endif

#ifdef CONFIG_639
PatchOffset g_639_offsets = {
	.fw_version = FW_639,
	.sysmem_patch = {
		.sceKernelIcacheInvalidateAll = 0x00000E98,
		.sceKernelDcacheWritebackInvalidateAll = 0x00000744,
		.sceKernelGetModel = 0x0000A13C,
		.sceKernelPowerLockForUser = 0x0000CC34,
		.sceKernelPowerLockForUser_data_offset = 0x000040F4,
	},
	.sceKernelFindModuleByName = 0x88017000 + 0x000072D8,
	.loadexec_patch_05g = {
		.LoadReboot = 0x00000000,
		.LoadRebootCall = 0x00002FA8,
		.RebootJump = 0x00002FF4,
	},
	.loadexec_patch_other= {
		.LoadReboot = 0x00000000,
		.LoadRebootCall = 0x00002D5C,
		.RebootJump = 0x00002DA8,
	},
	.patchRangeStart = 0xDEADBEEF,
	.patchRangeEnd = 0xDEADBEEF,
};
#endif

#ifdef CONFIG_635
PatchOffset g_635_offsets = {
	.fw_version = FW_635,
	.sysmem_patch = {
		.sceKernelIcacheInvalidateAll = 0x00000E98,
		.sceKernelDcacheWritebackInvalidateAll = 0x00000744,
		.sceKernelGetModel = 0x0000A13C,
		.sceKernelPowerLockForUser = 0x0000CC34,
		.sceKernelPowerLockForUser_data_offset = 0x000040F4,
	},
	.sceKernelFindModuleByName = 0x88017000 + 0x000072D8,
	.loadexec_patch_05g = {
		.LoadReboot = 0x00000000,
		.LoadRebootCall = 0x00002F90,
		.RebootJump = 0x00002FDC,
	},
	.loadexec_patch_other= {
		.LoadReboot = 0x00000000,
		.LoadRebootCall = 0x00002D44,
		.RebootJump = 0x00002D90,
	},
	.patchRangeStart = 0x0000A110,
	.patchRangeEnd = 0x0000A1F0,
};
#endif

#ifdef CONFIG_620
PatchOffset g_620_offsets = {
	.fw_version = FW_620,
	.sysmem_patch = {
		.sceKernelIcacheInvalidateAll = 0x00000E98,
		.sceKernelDcacheWritebackInvalidateAll = 0x00000744,
		.sceKernelGetModel = 0x0000A1C4,
		.sceKernelPowerLockForUser = 0x0000CCBC,
		.sceKernelPowerLockForUser_data_offset = 0x00004234,
	},
	.sceKernelFindModuleByName = 0x8801EB78,
	.loadexec_patch_05g = {
		.LoadReboot = 0x00000000,
		.LoadRebootCall = 0x00002F28,
		.RebootJump = 0x00002F74,
	},
	.loadexec_patch_other= {
		.LoadReboot = 0x00000000,
		.LoadRebootCall = 0x00002CD8,
		.RebootJump = 0x00002D24,
	},
	.patchRangeStart = 0x0000CCB0,
	.patchRangeEnd = 0x0000CCC0,
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
