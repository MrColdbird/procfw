#ifndef LAUNCHER_PATCH_OFFSET_H
#define LAUNCHER_PATCH_OFFSET_H

#include "utils.h"

struct sceLoadExecPatch {
	u32 LoadReboot;
	u32 LoadRebootCall;
	u32 RebootJump;
};

typedef struct _PatchOffset {
	u32 fw_version;
	u32 sceKernelIcacheInvalidateAll;
	u32 sceKernelDcacheWritebackInvalidateAll;
	u32 sceKernelFindModuleByName;
	u32 sceKernelGetModel;
	struct sceLoadExecPatch loadexec_patch_05g;
	struct sceLoadExecPatch loadexec_patch_other;
	u32 patchRangeStart;
	u32 patchRangeEnd;
} PatchOffset;

extern PatchOffset *g_offs;

void setup_patch_offset_table(u32 fw_version);

#endif
