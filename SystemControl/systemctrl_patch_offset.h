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
	u32 memlmd_unsigner;
	u32 memlmd_decrypt;
	u32 sceMemlmdInitializeScrambleKey;
	u32 memlmd_unsigner_call1, memlmd_unsigner_call2;
	u32 memlmd_decrypt_call1, memlmd_decrypt_call2;
};

struct MesgledPatch {
	u32 mesg_decrypt_call_common;
	u32 mesg_decrypt_call[9][4];
	u32 mesgled_decrypt;
};

struct SystemCtrlExportPatch {
	u32 sctrlKernelLoadExecVSHWithApitype, sctrlKernelLoadExecVSHWithApitype_05g;
	u32 sctrlKernelSetUserLevel;
	u32 sctrlKernelSetDevkitVersion;
	u32 sctrlHENFindDriver;
	u32 sctrlKernelSetUMDEmuFile;
	u32 sctrlKernelSetInitFileName;
};

struct ValidateStubPatch {
	u32 StartModule;
	u32 StartModuleCall;
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

struct NidResolverPatch {
	u32 sceKernelLinkLibraryEntries;
	u32 sceKernelLinkLibraryEntriesCall;
	u32 sceKernelLinkLibraryEntriesForUser;
	u32 sceKernelLinkLibraryEntriesForUserCall;
	u32 sceKernelIcacheClearAll;
	u32 sceSysconPowerStandby;
};

struct StartModulePatch {
	u32 sceInitBootStartCall;
	u32 sceKernelStartModuleBootStartOffset;
};

struct HighMemoryPatch {
	u32 get_partition;
	u32 umd_cache_module_start;
};

struct SysMemForUserPatch {
	u16 offset;
	u8 value;
};

struct ModuleHandlerPatch {
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
};

struct sceLoaderCorePatch {
	u32 sceKernelCheckExecFile;
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
};

struct sceLoadExecPatch {
	u32 LoadReboot;
	u32 LoadRebootCall;
	u32 RebootJump;
	u32 sceKernelLoadExecWithApiTypeCheck1;
	u32 sceKernelLoadExecWithApiTypeCheck2;
	u32 sceKernelExitVSHVSHCheck1;
	u32 sceKernelExitVSHVSHCheck2;
};

struct sceImposeDriverPatch {
	int nr_nop;
	u32 offset;
};

struct sceUSBDriverPatch {
	u32 scePowerBatteryDisableUsbChargingStub;
	u32 scePowerBatteryEnableUsbChargingStub;
};

typedef struct _PatchOffset {
	u32 fw_version;
	struct InterruptManPatch interruptman_patch;
	struct MediaSyncPatch mediasync_patch;
	struct MemlmdPatch memlmd_patch_01g;
	struct MemlmdPatch memlmd_patch_other;
	struct MesgledPatch mesgled_patch;
	struct SystemCtrlExportPatch systemctrl_export_patch;
	struct ValidateStubPatch validate_stub_patch;
	struct March33Patch march33_patch;
	struct PsnFixPatch psnfix_patch;
	struct sceWlanDriverPatch wlan_driver_patch;
	struct scePowerServicePatch power_service_patch;
	struct NidResolverPatch nid_resolver_patch;
	struct StartModulePatch start_module_patch;
	struct HighMemoryPatch high_memory_patch;
	struct SysMemForUserPatch sysmemforuser_patch[9];
	struct ModuleHandlerPatch module_handler_patch;
	struct sceLoaderCorePatch loadercore_patch;
	struct sceLoadExecPatch loadexec_patch_other;
	struct sceLoadExecPatch loadexec_patch_05g;
	struct sceImposeDriverPatch impose_patch;
	struct sceUSBDriverPatch usb_patch;
} PatchOffset;

extern PatchOffset *g_offs;

void setup_patch_offset_table(u32 fw_version);

#endif
