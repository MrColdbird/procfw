#ifndef REBOOTEX_PATCH_OFFSET_H
#define REBOOTEX_PATCH_OFFSET_H

struct LoadExecPatch {
	u32 LoadRebootCall;
	u32 RebootJump;
};

typedef struct _PatchOffset {
	u32 fw_version;
	struct LoadExecPatch loadexec_patch_05g;
	struct LoadExecPatch loadexec_patch_other;
} PatchOffset;

extern PatchOffset *g_offs;

void setup_patch_offset_table(u32 fw_version);

#endif
