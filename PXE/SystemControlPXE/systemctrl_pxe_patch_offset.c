#include <pspsdk.h>
#include "systemctrl_pxe_patch_offset.h"

#if !defined(CONFIG_635) && !defined(CONFIG_620) && !defined(CONFIG_639)
#error You have to define CONFIG_620 or CONFIG_635 or CONFIG_639
#endif

#ifdef CONFIG_639
PXEPatchOffset g_pxe_639_offsets = {
	.fw_version = FW_639,
	.vsh_module_patch = {
		.module_start = 0x0000F570,
	},
};
#endif

#ifdef CONFIG_635
PXEPatchOffset g_pxe_635_offsets = {
	.fw_version = FW_635,
	.vsh_module_patch = {
		.module_start = 0x0000F570,
	},
};
#endif

#ifdef CONFIG_620
PXEPatchOffset g_pxe_620_offsets = {
	.fw_version = FW_620,
	.vsh_module_patch = {
		.module_start = 0x0000F0D4,
	},
};
#endif

PXEPatchOffset *g_pxe_offs = NULL;

void setup_pxe_patch_offset_table(u32 fw_version)
{
#ifdef CONFIG_639
	if(fw_version == g_pxe_639_offsets.fw_version) {
		g_pxe_offs = &g_pxe_639_offsets;
	}
#endif

#ifdef CONFIG_635
	if(fw_version == g_pxe_635_offsets.fw_version) {
		g_pxe_offs = &g_pxe_635_offsets;
	}
#endif

#ifdef CONFIG_620
	if(fw_version == g_pxe_620_offsets.fw_version) {
		g_pxe_offs = &g_pxe_620_offsets;
	}
#endif
}
