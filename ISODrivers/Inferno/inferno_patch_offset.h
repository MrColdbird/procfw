#ifndef STARGATE_PATCH_OFFSET_H
#define STARGATE_PATCH_OFFSET_H

#include "utils.h"

typedef struct _PatchOffset {
	u32 fw_version;
	u32 patches[4];
} PatchOffset;

extern PatchOffset *g_offs;

void setup_patch_offset_table(u32 fw_version);

#endif
