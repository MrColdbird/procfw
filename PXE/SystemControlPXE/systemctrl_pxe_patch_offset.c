#include <pspsdk.h>
#include "systemctrl_pxe_patch_offset.h"

PXEPatchOffset g_pxe_635_offsets = {
	.fw_version = FW_635,
	.vsh_module_patch = {
		.module_start = 0x0000F570,
	},
};

PXEPatchOffset g_pxe_620_offsets = {
	.fw_version = FW_620,
	.vsh_module_patch = {
		.module_start = 0x0000F0D4,
	},
};

PXEPatchOffset *g_pxe_offs = NULL;

void setup_pxe_patch_offset_table(u32 fw_version)
{
	if(fw_version == g_pxe_635_offsets.fw_version) {
		g_pxe_offs = &g_pxe_635_offsets;
	} else if(fw_version == g_pxe_620_offsets.fw_version) {
		g_pxe_offs = &g_pxe_620_offsets;
	}
}
