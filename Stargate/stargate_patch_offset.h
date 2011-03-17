#ifndef STARGATE_PATCH_OFFSET_H
#define STARGATE_PATCH_OFFSET_H

typedef struct _PatchOffset {
	u32 fw_version;
	u32 sceMesgLedDecryptGame1[9];
	u32 sceMesgLedDecryptGame2[9];
} PatchOffset;

extern PatchOffset *g_offs;

void setup_patch_offset_table(u32 fw_version);

#endif
