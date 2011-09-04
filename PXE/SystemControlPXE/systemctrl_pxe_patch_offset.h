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

#ifndef SYSTEMCTRL_PXE_PATCH_OFFSET_H
#define SYSTEMCTRL_PXE_PATCH_OFFSET_H

#include "utils.h"

struct VshModulePatch {
	u32 module_start;
};

typedef struct _PXEPatchOffset {
	u32 fw_version;
	struct VshModulePatch vsh_module_patch;
} PXEPatchOffset;

extern PXEPatchOffset *g_pxe_offs;

void setup_pxe_patch_offset_table(u32 fw_version);

#endif
