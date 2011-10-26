/*
 * This file is part of PRO CFW.

 * PRO CFW is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * PRO CFW is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PRO CFW. If not, see <http://www.gnu.org/licenses/ .
 */

#ifndef SYSTEMCTRL_PATCH_OFFSET_H
#define SYSTEMCTRL_PATCH_OFFSET_H

#include "utils.h"

struct InterruptManPatch {
	u32 InvalidSyscallCheck1, InvalidSyscallCheck2;
};

struct MediaSyncPatch {
	u32 sceSystemFileGetIndex;
	u32 MsCheckMediaCheck;
	u32 DiscCheckMediaCheck1;
	u32 DiscCheckMediaCheck2;
	u32 MsSystemFileCheck;
	u32 DiscIDCheck1;
	u32 DiscIDCheck2;
	u32 sceSystemFileGetIndexCall;
};

struct MemlmdPatch {
	u32 memlmd_TagPatch;
	u32 memlmd_unsigner;
	u32 memlmd_decrypt;
	u32 sceMemlmdInitializeScrambleKey;
	u32 memlmd_unsigner_call1, memlmd_unsigner_call2;
	u32 memlmd_decrypt_call1, memlmd_decrypt_call2;
};

struct MesgledPatch {
	u32 mesg_decrypt_call[PSP_11000+1][5];
	u32 mesgled_decrypt;
};

struct ThreadMgrPatch {
	u32 sctrlKernelSetUserLevel;
};

struct IOFileMgrPatch {
	u32 sctrlHENFindDriver;
};

struct March33Patch {
	u32 MsMediaInsertedCheck;
};

struct PsnFixPatch {
	u32 NPSignupImageVersion;
	u32 NPSigninCheck;
	u32 NPSigninImageVersion;
	u32 NPMatchVersionMajor;
	u32 NPMatchVersionMinor;
};

struct sceWlanDriverPatch {
	u32 FreqCheck;
};

struct scePowerServicePatch {
	u32 scePowerGetBacklightMaximumCheck;
};

struct SysconPatch {
	u32 sceSysconPowerStandby;
};

struct InitPatch {
	u32 sceKernelStartModuleImport;
	u32 module_bootstart;
};

struct UmdCachePatch {
	u32 module_start;
};

struct SysMemForUserPatch {
	u16 offset;
	u8 value;
};

struct SysMemPatch {
	struct SysMemForUserPatch sysmemforuser_patch[10];
	u32 sctrlKernelSetDevkitVersion;
	u32 get_partition;
};

struct sceLoaderCorePatch {
	u32 sceKernelCheckExecFilePtr;
	u32 sceKernelCheckExecFileCall1;
	u32 sceKernelCheckExecFileCall2;
	u32 sceKernelCheckExecFileCall3;
	u32 ReloactionTable;
	u32 ProbeExec1;
	u32 ProbeExec1Call;
	u32 ProbeExec2;
	u32 ProbeExec2Call1;
	u32 ProbeExec2Call2;
	u32 EnableSyscallExport;
	u32 LoaderCoreCheck1;
	u32 LoaderCoreCheck2;
	u32 LoaderCoreCheck3;
	u32 LoaderCoreUndo1Call1;
	u32 LoaderCoreUndo1Call2;
	u32 LoaderCoreUndo1Call3;
	u32 LoaderCoreUndo2Call1;
	u32 LoaderCoreUndo2Call2;
	u32 memlmd_323366CA_NID;
	u32 memlmd_7CF1CD3E_NID;
	u32 pops_version_check;
	u32 sceInitBootStartCall;
	u32 sceKernelLinkLibraryEntries;
	u32 sceKernelLinkLibraryEntriesForUser;
	u32 sceKernelIcacheClearAll;
};

struct sceLoadExecPatch {
	u32 LoadReboot;
	u32 LoadRebootCall;
	u32 RebootJump;
	u32 sceKernelLoadExecWithApiTypeCheck1;
	u32 sceKernelLoadExecWithApiTypeCheck2;
	u32 sceKernelExitVSHVSHCheck1;
	u32 sceKernelExitVSHVSHCheck2;
	u32 sctrlKernelLoadExecVSHWithApitype;
};

struct sceImposeDriverPatch {
	int nr_nop;
	u32 offset;
};

struct sceUSBDriverPatch {
	u32 scePowerBatteryDisableUsbChargingStub;
	u32 scePowerBatteryEnableUsbChargingStub;
};

struct ModuleMgrPatch {
	u32 sctrlKernelSetUMDEmuFile;
	u32 sctrlKernelSetInitFileName;
	u32 ProbeExec3;
	u32 ProbeExec3Call;
	u32 sceKernelCheckExecFileImport;
	u32 PartitionCheck;
	u32 PartitionCheckCall1;
	u32 PartitionCheckCall2;
	u32 DeviceCheck1;
	u32 DeviceCheck2;
	u32 DeviceCheck3;
	u32 DeviceCheck4;
	u32 DeviceCheck5;
	u32 DeviceCheck6;
	u32 DeviceCheck7;
	u32 DeviceCheck8;
	u32 PrologueModule;
	u32 PrologueModuleCall;
	u32 StartModule;
	u32 StartModuleCall;
	u32 sceKernelLinkLibraryEntriesCall;
	u32 sceKernelLinkLibraryEntriesForUserCall;
};

typedef struct _PatchOffset {
	u32 fw_version;
	struct InterruptManPatch interruptman_patch;
	struct ModuleMgrPatch modulemgr_patch;
	struct ThreadMgrPatch threadmgr_patch;
	struct MediaSyncPatch mediasync_patch;
	struct MemlmdPatch memlmd_patch_01g;
	struct MemlmdPatch memlmd_patch_other;
	struct MesgledPatch mesgled_patch;
	struct SysMemPatch sysmem_patch;
	struct IOFileMgrPatch iofilemgr_patch;
	struct March33Patch march33_patch;
	struct PsnFixPatch psnfix_patch;
	struct sceWlanDriverPatch wlan_driver_patch;
	struct scePowerServicePatch power_service_patch;
	struct SysconPatch syscon_patch;
	struct InitPatch init_patch;
	struct UmdCachePatch umdcache_patch;
	struct sceLoaderCorePatch loadercore_patch;
	struct sceLoadExecPatch loadexec_patch_other;
	struct sceLoadExecPatch loadexec_patch_05g;
	struct sceImposeDriverPatch impose_patch;
	struct sceUSBDriverPatch usb_patch;
} PatchOffset;

extern PatchOffset *g_offs;

void setup_patch_offset_table(u32 fw_version);

#endif
