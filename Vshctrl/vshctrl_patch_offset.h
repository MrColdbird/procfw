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
};

struct MsVideoMainPatch {
	u32 checks[10];
};

struct VshModulePatch {
	u32 checks[3];
	u32 loadexecNID1;
	u32 loadexecNID2;
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
