#include <pspsdk.h>
#include "rebootex_patch_offset.h"

PatchOffset g_635_offsets = {
	.fw_version = FW_635,
	.loadexec_patch_05g = {
		.LoadRebootCall = 0x00002F90,
		.RebootJump = 0x00002FDC,
	},
	.loadexec_patch_other = {
		.LoadRebootCall = 0x00002D44,
		.RebootJump = 0x00002D90,
	},
};

PatchOffset g_620_offsets = {
	.fw_version = FW_620,
	.loadexec_patch_05g = {
		.LoadRebootCall = 0x00002F28,
		.RebootJump = 0x00002F74,
	},
	.loadexec_patch_other = {
		.LoadRebootCall = 0x00002CD8,
		.RebootJump = 0x00002D24,
	},
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
