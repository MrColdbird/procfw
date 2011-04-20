#include <pspsdk.h>
#include "launcher_patch_offset.h"

#if !defined(CONFIG_635) && !defined(CONFIG_620)
#error You have to define CONFIG_620 or CONFIG_635
#endif

#ifdef CONFIG_635
PatchOffset g_635_offsets = {
	.fw_version = FW_635,
	.sceKernelIcacheInvalidateAll = 0x88000E98,
	.sceKernelDcacheWritebackInvalidateAll = 0x88000744,
	.sceKernelFindModuleByName = 0x88017000 + 0x000072D8,
	.sceKernelGetModel = 0x8800A13C,
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
	.sceKernelIcacheInvalidateAll = 0x88000E98,
	.sceKernelDcacheWritebackInvalidateAll = 0x88000744,
	.sceKernelFindModuleByName = 0x8801EB78,
	.sceKernelGetModel = 0x8800A1C4,
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
