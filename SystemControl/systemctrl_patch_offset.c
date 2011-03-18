#include <pspsdk.h>
#include "systemctrl_patch_offset.h"

PatchOffset g_635_offsets = {
	.fw_version = 0x06030510,
	.interruptman_patch = {
		.InvalidSyscallCheck1 = 0x00000DE8,
		.InvalidSyscallCheck2 = 0x00000E94,
	},
	.mediasync_patch = {
		.sceSystemFileGetIndex = 0x00000F40,
		.MsCheckMediaCheck = 0x00000744,
		.DiscCheckMediaCheck = 0x000003C4,
		.MsSystemFileCheck = 0x000010B4,
		.DiscIDCheck = 0x00000FC0,
		.sceSystemFileGetIndexCall = 0x0000097C,
	},
	.memlmd_patch_01g = {
		.memlmd_unsigner = 0x00000F88,
		.memlmd_decrypt = 0x00000134,
		.sceMemlmdInitializeScrambleKey = 0x000011D0,
		.memlmd_unsigner_call1 = 0x00001150,
		.memlmd_unsigner_call2 = 0x000011A4,
		.memlmd_decrypt_call1 = 0x00000E88,
		.memlmd_decrypt_call2 = 0x00000EEC,
	},
	.memlmd_patch_other = {
		.memlmd_unsigner = 0x00001078,
		.memlmd_decrypt = 0x00000134,
		.sceMemlmdInitializeScrambleKey = 0x000012C0,
		.memlmd_unsigner_call1 = 0x00001240,
		.memlmd_unsigner_call2 = 0x00001294,
		.memlmd_decrypt_call1 = 0x00000F78,
		.memlmd_decrypt_call2 = 0x00000FDC,
	},
	.mesgled_patch = {
		.mesg_decrypt_homebrew = {
			0x00001D20, // 01g
			0x00001DD0, // 02g
			0x00001E60, // 03g
			0x00001E60, // 04g
			0x00001EF8, // 05g
			0xDEADBEEF, // 06g
			0x00001E60, // 07g
			0xDEADBEEF, // 08g
			0x00001E60, // 09g
		},
	},
	.systemctrl_export_patch = {
		.sctrlKernelLoadExecVSHWithApitype = 0x0000236C,
		.sctrlKernelLoadExecVSHWithApitype_05g = 0x000025C0,
		.sctrlKernelSetUserLevel = 0x00019E80,
		.sctrlKernelSetDevkitVersion = 0x88011998,
		.sctrlHENFindDriver = 0x00002A44,
		.sctrlKernelSetUMDEmuFile = 0x000099B8,
		.sctrlKernelSetInitFileName = 0x000099B4,
	},
	.validate_stub_patch = {
		.StartModule = 0x00007004,
		.StartModuleCall = 0x00000290,
	},
	.march33_patch = {
		.MsMediaInsertedCheck = 0x000009E8,
	},
	.psnfix_patch = {
		.NPSignupImageVersion = 0x000370D0,
		.NPSigninCheck = 0x00006C94,
		.NPSigninImageVersion = 0x00009664,
		.NPMatchVersionMajor = 0x00004604,
		.NPMatchVersionMinor = 0x0000460C,
	},
	.wlan_driver_patch = {
		.FreqCheck = 0x000026C0,
	},
	.power_service_patch = {
		.scePowerGetBacklightMaximumCheck = 0x00000E10,
	},
	.nid_resolver_patch = {
		.sceKernelLinkLibraryEntries = 0x000011D4,
		.sceKernelLinkLibraryEntriesCall = 0x0000844C,
		.sceKernelLinkLibraryEntriesForUser = 0x00002924,
		.sceKernelLinkLibraryEntriesForUserCall = 0x00008198,
		.sceKernelIcacheClearAll = 0x000077CC,
		.sceSysconPowerStandby = 0x00002C6C,
	},
	.start_module_patch = {
		.sceInitBootStartCall = 0x00001DA8,
		.sceKernelStartModuleBootStartOffset = 0x00001CBC - 0x00001A4C,
	},
	.high_memory_patch = {
		.get_partition = 0x88003E34,
		.umd_cache_module_start = 0x000009C8,
	},
	.sysmemforuser_patch = {
		{ 0x00009A2C, 0x1F }, // sceKernelSetCompiledSdkVersion
		{ 0x00009B4C, 0x12 }, // SysMemUserForUser_342061E5
		{ 0x00009BE4, 0x18 }, // SysMemUserForUser_315AD3A0
		{ 0x00009C94, 0x1C }, // SysMemUserForUser_EBD5C3E6
		{ 0x00009D68, 0x15 }, // SysMemUserForUser_057E7380
		{ 0x00009E0C, 0x15 }, // SysMemUserForUser_91DE343C
		{ 0x00009EB0, 0x12 }, // SysMemUserForUser_7893F79A
		{ 0x00009F48, 0x18 }, // SysMemUserForUser_35669D4C
		{ 0x00009FF8, 0x12 }, // SysMemUserForUser_1B4217BC
	},
	.module_handler_patch = {
		.ProbeExec3 = 0x00008864,
		.ProbeExec3Call = 0x00007C6C,
		.sceKernelCheckExecFileImport = 0x000087E4,
		.PartitionCheck = 0x00007FE0,
		.PartitionCheckCall1 = 0x0000652C,
		.PartitionCheckCall2 = 0x000068A8,
		.DeviceCheck1 = 0x00000760,
		.DeviceCheck2 = 0x000007C0,
		.DeviceCheck3 = 0x000030B0,
		.DeviceCheck4 = 0x0000310C,
		.DeviceCheck5 = 0x00003138,
		.DeviceCheck6 = 0x00003444,
		.DeviceCheck7 = 0x0000349C,
		.DeviceCheck8 = 0x000034C8,
		.PrologueModule = 0x00008134,
		.PrologueModuleCall = 0x00007058,
	},
	.loadercore_patch = {
		.sceKernelCheckExecFile = 0x00007DC0,
		.sceKernelCheckExecFileCall1 = 0x00001570,
		.sceKernelCheckExecFileCall2 = 0x000015C0,
		.sceKernelCheckExecFileCall3 = 0x00004BE8,
		.ReloactionTable = 0x00008274,
		.ProbeExec1 = 0x0000679C,
		.ProbeExec1Call = 0x000047E4,
		.ProbeExec2 = 0x000066F4,
		.ProbeExec2Call1 = 0x000049E4,
		.ProbeExec2Call2 = 0x00006A14,
		.EnableSyscallExport = 0x000040A4,
		.LoaderCoreCheck1 = 0x000076E4,
		.LoaderCoreCheck2 = 0x00005C34,
		.LoaderCoreCheck3 = 0x00005D44,
		.LoaderCoreUndo1Call1 = 0x00005CC8,
		.LoaderCoreUndo1Call2 = 0x00005CF8,
		.LoaderCoreUndo1Call3 = 0x00005D90,
		.LoaderCoreUndo2Call1 = 0x000041A4,
		.LoaderCoreUndo2Call2 = 0x00005CA4,
	},
	.loadexec_patch_other = {
		.LoadReboot = 0x00000000,
		.LoadRebootCall = 0x00002D44,
		.RebootAddress = 0x00002D90,
		.sceKernelLoadExecWithApiTypeCheck1 = 0x000023B8,
		.sceKernelLoadExecWithApiTypeCheck2 = 0x000023FC,
		.sceKernelExitVSHVSHCheck1 = 0x0000168C,
		.sceKernelExitVSHVSHCheck2 = 0x000016C0,
	},
	.loadexec_patch_05g = {
		.LoadReboot = 0x00000000,
		.LoadRebootCall = 0x00002F90,
		.RebootAddress = 0x00002FDC,
		.sceKernelLoadExecWithApiTypeCheck1 = 0x0000260C,
		.sceKernelLoadExecWithApiTypeCheck2 = 0x00002650,
		.sceKernelExitVSHVSHCheck1 = 0x0000168C,
		.sceKernelExitVSHVSHCheck2 = 0x000016C0,
	},
};

PatchOffset *g_offs = NULL;

void setup_patch_offset_table(u32 fw_version)
{
	// for now fw_version cannot be used because sceKernelDevkitVersion's NID goes randomly

	// assumed it's 635
	g_offs = &g_635_offsets;
}
