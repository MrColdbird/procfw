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

#ifndef VSHCTRL_PATCH_OFFSET_H
#define VSHCTRL_PATCH_OFFSET_H

#include "utils.h"

struct VshctrlPatch {
	u32 sceDisplaySetHoldMode;
	u32 sceDisplaySetHoldModeCall;
	u32 HibBlockCheck;
	u32 SystemVersionStr;
	const char *SystemVersionMessage;
	u32 SystemVersion;
	u32 MacAddressStr;
	u32 HomebrewCheck;
	u32 PopsCheck;
	u32 MultiDiscPopsCheck;
	u32 HidePicCheck1;
	u32 HidePicCheck2;
	u32 SkipGameBootSubroute;
	u32 SkipGameBoot;
	u32 RifFileCheck;
	u32 RifCompareCheck;
	u32 RifTypeCheck;
	u32 RifNpDRMCheck;
	u32 sceCtrlReadBufferPositiveNID;
	u32 htmlviewer_save_location;
};

struct MsVideoMainPatch {
	u32 checks[10];
};

struct VshModulePatch {
	u32 checks[3];
	u32 loadexecNID1;
	u32 loadexecNID2;
	u32 PBPFWCheck[4];
};

struct CustomUpdatePatch {
	u32 UpdatePluginImageVersion1;
	u32 UpdatePluginImageVersion2;
	u32 SceUpdateDL_UpdateListStr;
	u32 SceUpdateDL_UpdateListCall1;
	u32 SceUpdateDL_UpdateListCall2;
	u32 SceUpdateDL_UpdateListCall3;
	u32 SceUpdateDL_UpdateListCall4;
};

typedef struct _PatchOffset {
	u32 fw_version;
	struct VshctrlPatch vshctrl_patch;
	struct MsVideoMainPatch msvideo_main_patch;
	struct VshModulePatch vsh_module_patch;
	struct CustomUpdatePatch custom_update_patch;
} PatchOffset;

extern PatchOffset *g_offs;

void setup_patch_offset_table(u32 fw_version);

#endif
