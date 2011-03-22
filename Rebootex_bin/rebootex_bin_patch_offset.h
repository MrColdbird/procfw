#ifndef REBOOTEX_BIN_PATCH_OFFSET_H
#define REBOOTEX_BIN_PATCH_OFFSET_H

#include <pspsdk.h>
#include "utils.h"

#if !defined(CONFIG_635) && !defined(CONFIG_620)
#error You have to define CONFIG_620 or CONFIG_635
#endif

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
	u32 DecryptPSP;
	u32 sceKernelCheckExecFile;
	u32 DecryptPSPCall1;
	u32 DecryptPSPCall2;
	u32 sceKernelCheckExecFileCall1;
	u32 sceKernelCheckExecFileCall2;
	u32 sceKernelCheckExecFileCall3;
};

typedef struct _PatchOffset {
	u32 fw_version;
	u32 iCacheFlushAll;
	u32 dCacheFlushAll;
	struct RebootexPatch rebootex_patch_01g;
	struct RebootexPatch rebootex_patch_other;
	struct LoadCorePatch loadcore_patch;
} PatchOffset;

extern PatchOffset *g_offs;

#ifdef CONFIG_635
PatchOffset g_635_offsets = {
	.fw_version = FW_635,
	.iCacheFlushAll = 0x000001E4,
	.dCacheFlushAll = 0x00000938,
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
		.RebootexCheck5 = 0x00007648,
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
		.DecryptPSP = 0x00007B08 - 0x00000BBC,
		.sceKernelCheckExecFile = 0x00007AE8 - 0x00000BBC,
		.DecryptPSPCall1 = 0x000041A4 - 0x00000BBC,
		.DecryptPSPCall2 = 0x00005CA4 - 0x00000BBC,
		.sceKernelCheckExecFileCall1 = 0x00005CC8 - 0x00000BBC,
		.sceKernelCheckExecFileCall2 = 0x00005CF8 - 0x00000BBC,
		.sceKernelCheckExecFileCall3 = 0x00005D90 - 0x00000BBC,
	},
};
#endif

#ifdef CONFIG_620
PatchOffset g_620_offsets = {
	.fw_version = FW_620,
	.iCacheFlushAll = 0x000001E4,
	.dCacheFlushAll = 0x00000938,
	.rebootex_patch_01g = {
		.sceBootLfatOpen = 0x000082AC,
		.sceBootLfatRead = 0x00008420,
		.sceBootLfatClose = 0x000083C4,
		.UnpackBootConfig = 0x0000565C,
		.sceBootLfatOpenCall = 0x000026DC,
		.sceBootLfatReadCall = 0x0000274C,
		.sceBootLfatCloseCall = 0x00002778,
		.UnpackBootConfigCall = 0x000070F0,
		.RebootexCheck1 = 0x00003798,
		.RebootexCheck2 = 0x000026D4,
		.RebootexCheck3 = 0x00002728,
		.RebootexCheck4 = 0x00002740,
		.RebootexCheck5 = 0x00007388,
		.LoadCoreModuleStartCall = 0x00005554,
		.UnpackBootConfigBufferAddress = 0x000070CC,
	},
	.rebootex_patch_other = {
		.sceBootLfatOpen = 0x00008374,
		.sceBootLfatRead = 0x000084E8,
		.sceBootLfatClose = 0x0000848C,
		.UnpackBootConfig = 0x00005724,
		.sceBootLfatOpenCall = 0x000027A4,
		.sceBootLfatReadCall = 0x00002814,
		.sceBootLfatCloseCall = 0x00002840,
		.UnpackBootConfigCall = 0x000071B8,
		.RebootexCheck1 = 0x00003860,
		.RebootexCheck2 = 0x0000279C,
		.RebootexCheck3 = 0x000027F0,
		.RebootexCheck4 = 0x00002808,
		.RebootexCheck5 = 0x00007450,
		.LoadCoreModuleStartCall = 0x0000561C,
		.UnpackBootConfigBufferAddress = 0x00007194,
	},
	.loadcore_patch = {
		.DecryptPSP = 0x00008374 - 0x00000BC4,
		.sceKernelCheckExecFile = 0x0000835C - 0x00000BC4,
		.DecryptPSPCall1 = 0x000041A4 - 0x00000BC4,
		.DecryptPSPCall2 = 0x000068F0 - 0x00000BC4,
		.sceKernelCheckExecFileCall1 = 0x00006914 - 0x00000BC4,
		.sceKernelCheckExecFileCall2 = 0x00006944 - 0x00000BC4,
		.sceKernelCheckExecFileCall3 = 0x000069DC - 0x00000BC4,
	},
};
#endif

PatchOffset *g_offs = NULL;

static inline void setup_patch_offset_table(u32 fw_version)
{
#ifdef CONFIG_635
	if(fw_version == g_635_offsets.fw_version) {
		g_offs = &g_635_offsets;
	}
#endif

#ifdef CONFIG_620
	if(fw_version == g_620_offsets.fw_version) {
		g_offs = &g_620_offsets;
	}
#endif
}

#endif
