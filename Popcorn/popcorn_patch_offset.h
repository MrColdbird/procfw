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

#ifndef POPCORN_PATCH_OFFSET_H
#define POPCORN_PATCH_OFFSET_H

#include "utils.h"

struct PopsMgrPatch {
	u32 get_rif_path;
	u32 get_rif_path_call1;
	u32 get_rif_path_call2;
	u32 sceNpDrmGetVersionKeyCall;
	u32 scePspNpDrm_driver_9A34AC9F_Call;
	u32 scePopsManLoadModuleCheck;
};

struct PopsDecompressFunctionPatch {
	u32 stub_offset;
	u32 patch_offset;
};

struct PopsPatch {
	struct PopsDecompressFunctionPatch decomp[PSP_11000+1];
	u32 ICON0SizeOffset[PSP_11000+1];
	u32 manualNameCheck[PSP_11000+1];
	u32 sceMeAudio_67CD7972_NID;
};

typedef struct _PatchOffset {
	u32 fw_version;
	struct PopsMgrPatch popsman_patch;
	struct PopsPatch pops_patch;
} PatchOffset;

extern PatchOffset *g_offs;

void setup_patch_offset_table(u32 fw_version);

#endif
