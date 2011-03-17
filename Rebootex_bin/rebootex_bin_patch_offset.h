#ifndef REBOOTEX_BIN_PATCH_OFFSET_H
#define REBOOTEX_BIN_PATCH_OFFSET_H

#include <pspsdk.h>

struct RebootexPatch {
	u32 sceBootLfatOpen;
	u32 sceBootLfatRead;
	u32 sceBootLfatClose;
	u32 UnpackBootConfig;
	u32 sceBootLfatOpenCall;
	u32 sceBootLfatReadCall;
	u32 sceBootLfatCloseCall;
	u32 UnpackBootConfigCall;
	u32 RebootexCheck1;
	u32 RebootexCheck2;
	u32 RebootexCheck3;
	u32 RebootexCheck4;
	u32 RebootexCheck5;
	u32 LoadCoreModuleStartCall;
	u32 UnpackBootConfigBufferAddress;
};

struct LoadCorePatch {
	u32 sceKernelCheckExecFile;
	u32 memlmd_3F2AC9C6;
	u32 sceKernelCheckExecFileCall1;
	u32 sceKernelCheckExecFileCall2;
	u32 memlmd_3F2AC9C6_call1;
	u32 memlmd_3F2AC9C6_call2;
	u32 memlmd_3F2AC9C6_call3;
};

typedef struct _PatchOffset {
	u32 fw_version;
	u32 iCacheFlushAll;
	struct RebootexPatch rebootex_patch_01g;
	struct RebootexPatch rebootex_patch_other;
	struct LoadCorePatch loadcore_patch;
} PatchOffset;

extern PatchOffset *g_offs;

PatchOffset g_635_offsets = {
	.fw_version = 0x06030510,
	.iCacheFlushAll = 0x00000938,
	.rebootex_patch_01g = {
		.sceBootLfatOpen = 0x00008624,
		.sceBootLfatRead = 0x00008798,
		.sceBootLfatClose = 0x0000873C,
		.UnpackBootConfig = 0x0000588C,
		.sceBootLfatOpenCall = 0x00002764,
		.sceBootLfatReadCall = 0x000027D4,
		.sceBootLfatCloseCall = 0x00002800,
		.UnpackBootConfigCall = 0x00007348,
		.RebootexCheck1 = 0x0000389C,
		.RebootexCheck2 = 0x0000275C,
		.RebootexCheck3 = 0x000027B0,
		.RebootexCheck4 = 0x000027C8,
		.RebootexCheck5 = 0x00007308,
		.LoadCoreModuleStartCall = 0x00005764,
		.UnpackBootConfigBufferAddress = 0x00007308,
	},
	.rebootex_patch_other = {
		.sceBootLfatOpen = 0x000086F0,
		.sceBootLfatRead = 0x00008864,
		.sceBootLfatClose = 0x00008808,
		.UnpackBootConfig = 0x0000595C,
		.sceBootLfatOpenCall = 0x00002834,
		.sceBootLfatReadCall = 0x000028A4,
		.sceBootLfatCloseCall = 0x000028D0,
		.UnpackBootConfigCall = 0x00007438,
		.RebootexCheck1 = 0x0000396C,
		.RebootexCheck2 = 0x0000282C,
		.RebootexCheck3 = 0x00002880,
		.RebootexCheck4 = 0x00002898,
		.RebootexCheck5 = 0x00007714,
		.LoadCoreModuleStartCall = 0x00005834,
		.UnpackBootConfigBufferAddress = 0x000073F8,
	},
	.loadcore_patch = {
		.sceKernelCheckExecFile = 0x00007B08 - 0x00000BBC,
		.memlmd_3F2AC9C6 = 0x00007AE8 - 0x00000BBC,
		.sceKernelCheckExecFileCall1 = 0x000041A4 - 0x00000BBC,
		.sceKernelCheckExecFileCall2 = 0x00005CA4 - 0x00000BBC,
		.memlmd_3F2AC9C6_call1 = 0x00005CC8 - 0x00000BBC,
		.memlmd_3F2AC9C6_call2 = 0x00005CF8 - 0x00000BBC,
		.memlmd_3F2AC9C6_call3 = 0x00005D90 - 0x00000BBC,
	},
};

PatchOffset *g_offs = NULL;

static inline void setup_patch_offset_table(u32 fw_version)
{
	if(fw_version == g_635_offsets.fw_version) {
		g_offs = &g_635_offsets;
	}
}

#endif
