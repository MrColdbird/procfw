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

#ifndef GALAXY_PATCH_OFFSET_H
#define GALAXY_PATCH_OFFSET_H

#include "utils.h"

typedef struct _PatchOffset {
	u32 fw_version;
	u32 StoreFd;
	u32 Data1;
	u32 Data2;
	u32 Data3;
	u32 Data4;
	u32 Data5;
	u32 InitForKernelCall;
	u32 Func1;
	u32 Func2;
	u32 Func3;
	u32 Func4;
	u32 Func5;
	u32 Func6;
	u32 sceIoClose;
	u32 sceKernelCreateThread;
	u32 sceKernelStartThread;
} PatchOffset;

extern PatchOffset *g_offs;

void setup_patch_offset_table(u32 fw_version);

#endif
