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
#include "systemctrl_pxe_patch_offset.h"

#if !defined(CONFIG_635) && !defined(CONFIG_620) && !defined(CONFIG_639) && !defined(CONFIG_660) && !defined(CONFIG_661)
#error You have to define CONFIG_620 or CONFIG_635 or CONFIG_639 or CONFIG_660 or CONFIG_661
#endif

#if defined(CONFIG_660) || defined(CONFIG_661)
#ifdef CONFIG_660
PXEPatchOffset g_pxe_660_offsets = {
	.fw_version = FW_660,
#endif
#ifdef CONFIG_661
PXEPatchOffset g_pxe_661_offsets = {
	.fw_version = FW_661,
#endif
	.vsh_module_patch = {
		.module_start = 0x0000F5F0,
	},
};
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
#ifdef CONFIG_661
	if(fw_version == g_pxe_661_offsets.fw_version) {
		g_pxe_offs = &g_pxe_661_offsets;
	}
#endif

#ifdef CONFIG_660
	if(fw_version == g_pxe_660_offsets.fw_version) {
		g_pxe_offs = &g_pxe_660_offsets;
	}
#endif

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
