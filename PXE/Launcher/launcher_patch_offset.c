#include <pspsdk.h>
#include "launcher_patch_offset.h"

PatchOffset g_635_offsets = {
	.fw_version = FW_635,
	.sceKernelIcacheInvalidateAll = 0x88000E98,
	.sceKernelDcacheWritebackInvalidateAll = 0x88000744,
	.sceKernelFindModuleByName = 0x88017000 + 0x000072D8,
	.sceKernelGetModel = 0x8800A13C,
	.loadexec_patch_05g = {
		.LoadReboot = 0x00000000,
		.LoadRebootCall = 0x00002F90,
		.RebootAddress = 0x00002FDC,
	},
	.loadexec_patch_other= {
		.LoadReboot = 0x00000000,
		.LoadRebootCall = 0x00002D44,
		.RebootAddress = 0x00002D90,
	},
	.patchRangeStart = 0x0000A110,
	.patchRangeEnd = 0x0000A1F0,
};

PatchOffset g_620_offsets = {
	.fw_version = FW_620,
	.sceKernelIcacheInvalidateAll = 0x88000E98,
	.sceKernelDcacheWritebackInvalidateAll = 0x88000744,
	.sceKernelFindModuleByName = 0x8801EB78,
	.sceKernelGetModel = 0x8800A1C4,
	.loadexec_patch_05g = {
		.LoadReboot = 0x00000000,
		.LoadRebootCall = 0x00002F28,
		.RebootAddress = 0x00002F74,
	},
	.loadexec_patch_other= {
		.LoadReboot = 0x00000000,
		.LoadRebootCall = 0x00002CD8,
		.RebootAddress = 0x00002D24,
	},
	.patchRangeStart = 0x0000CCB0,
	.patchRangeEnd = 0x0000CCC0,
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
