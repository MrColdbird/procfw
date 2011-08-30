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

#ifndef LAUNCHER_PATCH_OFFSET_H
#define LAUNCHER_PATCH_OFFSET_H

#include "utils.h"

struct sceLoadExecPatch {
	u32 LoadReboot;
	u32 LoadRebootCall;
	u32 RebootJump;
};

struct SysMemPatch {
	u32 sceKernelIcacheInvalidateAll;
	u32 sceKernelDcacheWritebackInvalidateAll;
	u32 sceKernelGetModel;
	u32 sceKernelPowerLockForUser;
	u16 sceKernelPowerLockForUser_data_offset;
};

typedef struct _PatchOffset {
	u32 fw_version;
	struct SysMemPatch sysmem_patch;
	u32 sceKernelFindModuleByName;
	struct sceLoadExecPatch loadexec_patch_05g;
	struct sceLoadExecPatch loadexec_patch_other;
	u32 patchRangeStart;
	u32 patchRangeEnd;
} PatchOffset;

extern PatchOffset *g_offs;

void setup_patch_offset_table(u32 fw_version);

#endif
