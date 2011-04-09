#ifndef POPCORN_PATCH_OFFSET_H
#define POPCORN_PATCH_OFFSET_H

#include "utils.h"

struct PopsMgrPatch {
	u32 sceIoOpenImport;
	u32 sceIoLseekImport;
	u32 sceIoIoctlImport;
	u32 sceIoReadImport;
	u32 sceIoReadAsyncImport;
	u32 sceIoGetstatImport;
	u32 sceIoCloseImport;
	u32 get_rif_path;
	u32 get_rif_path_call1;
	u32 get_rif_path_call2;
	u32 sceNpDrmGetVersionKeyCall;
	u32 scePspNpDrm_driver_9A34AC9F_Call;
	u32 scePopsManLoadModuleCheck;
	u32 sceDrmBBCipherInitImport;
	u32 sceDrmBBCipherUpdateImport;
	u32 sceDrmBBCipherFinalImport;
	u32 sceDrmBBMacInitImport;
	u32 sceDrmBBMacUpdateImport;
	u32 sceDrmBBMacFinalImport;
	u32 sceDrmBBMacFinal2Import;
};

struct PopsDecompressFunctionPatch {
	u32 stub_offset;
	u32 patch_offset;
};

struct PopsPatch {
	struct PopsDecompressFunctionPatch decomp[9];
	u32 ICON0SizeOffset[9];
	u32 manualNameCheck[9];
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
