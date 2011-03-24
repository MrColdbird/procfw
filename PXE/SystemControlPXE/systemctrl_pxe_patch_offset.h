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
