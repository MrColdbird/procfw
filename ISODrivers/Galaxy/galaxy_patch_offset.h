#ifndef GALAXY_PATCH_OFFSET_H
#define GALAXY_PATCH_OFFSET_H

#include "utils.h"

typedef struct _PatchOffset {
	u32 fw_version;
	u32 StoreFd;
	u32 StoreFd2;
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
