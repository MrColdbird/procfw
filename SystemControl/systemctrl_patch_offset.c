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

#include <pspsdk.h>
#include "systemctrl_patch_offset.h"

#if !defined(CONFIG_635) && !defined(CONFIG_620) && !defined(CONFIG_639)
#error You have to define CONFIG_620 or CONFIG_635 or CONFIG_639
#endif

#ifdef CONFIG_639
PatchOffset g_639_offsets = {
	.fw_version = FW_639,
	.interruptman_patch = {
		.InvalidSyscallCheck1 = 0x00000DE8,
		.InvalidSyscallCheck2 = 0x00000E94,
	},
	.mediasync_patch = {
		.sceSystemFileGetIndex = 0x00000F40,
		.MsCheckMediaCheck = 0x00000744,
		.DiscCheckMediaCheck1 = 0x000003C4,
		.DiscCheckMediaCheck2 = 0x00000DC8,
		.MsSystemFileCheck = 0x000010B4,
		.DiscIDCheck1 = 0x00000FC0,
		.DiscIDCheck2 = 0x00000FDC,
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
		.mesg_decrypt_call = {
			/*0x457B0CF0  0xE71C010B  0xADF305F0  0x457B0AF0  0x380280F0 */
			{ 0x00001DF4, 0x00003914, 0x00003B70, 0x00001E84, 0x00001ADC }, // 01g
			{ 0x00001EEC, 0x00003E84, 0x00004128, 0x00001F7C, 0x00001B70 }, // 02g
			{ 0x0000200C, 0x000043AC, 0x00004698, 0x0000209C, 0x00001BB8 }, // 03g
			{ 0x0000200C, 0x000043AC, 0x00004698, 0x0000209C, 0x00001BB8 }, // 04g
			{ 0x00002134, 0x00004880, 0x00004BB4, 0x000021C4, 0x00001C04 }, // 05g
			{ 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF }, // 06g
			{ 0x0000200C, 0x000043AC, 0x00004698, 0x0000209C, 0x00001BB8 }, // 07g
			{ 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF }, // 08g
			{ 0x0000200C, 0x000043AC, 0x00004698, 0x0000209C, 0x00001BB8 }, // 09g
		},
		.mesgled_decrypt = 0x000000E0,
	},
	.systemctrl_export_patch = {
		.sctrlKernelLoadExecVSHWithApitype = 0x00002384,
		.sctrlKernelLoadExecVSHWithApitype_05g = 0x000025D8,
		.sctrlKernelSetUserLevel = 0x00019E80,
		.sctrlKernelSetDevkitVersion = 0x88011998,
		.sctrlHENFindDriver = 0x00002A44,
		.sctrlKernelSetUMDEmuFile = 0x000099A0 + 0x00000008,   /* See 0x00005C28 */
		.sctrlKernelSetInitFileName = 0x000099A0 + 0x00000004, /* See 0x00004F54 */
	},
	.validate_stub_patch = {
		.StartModule = 0x00007000,
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
		.scePowerGetBacklightMaximumCheck = 0x00000E20,
	},
	.nid_resolver_patch = {
		.sceKernelLinkLibraryEntries = 0x000011D4,
		.sceKernelLinkLibraryEntriesCall = 0x00008448,
		.sceKernelLinkLibraryEntriesForUser = 0x00002924,
		.sceKernelLinkLibraryEntriesForUserCall = 0x00008194,
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
		.ProbeExec3 = 0x00008860,
		.ProbeExec3Call = 0x00007C68,
		.sceKernelCheckExecFileImport = 0x000087E0,
		.PartitionCheck = 0x00007FDC,
		.PartitionCheckCall1 = 0x00006528,
		.PartitionCheckCall2 = 0x000068A4,
		.DeviceCheck1 = 0x00000760,
		.DeviceCheck2 = 0x000007C0,
		.DeviceCheck3 = 0x000030B0,
		.DeviceCheck4 = 0x0000310C,
		.DeviceCheck5 = 0x00003138,
		.DeviceCheck6 = 0x00003444,
		.DeviceCheck7 = 0x0000349C,
		.DeviceCheck8 = 0x000034C8,
		.PrologueModule = 0x00008130,
		.PrologueModuleCall = 0x00007054,
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
		.memlmd_323366CA_NID = 0x3F2AC9C6,
		.memlmd_7CF1CD3E_NID = 0xE42AFE2E,
	},
	.loadexec_patch_other = {
		.LoadReboot = 0x00000000,
		.LoadRebootCall = 0x00002D5C,
		.RebootJump = 0x00002DA8,
		.sceKernelLoadExecWithApiTypeCheck1 = 0x000023D0,
		.sceKernelLoadExecWithApiTypeCheck2 = 0x00002414,
		.sceKernelExitVSHVSHCheck1 = 0x000016A4,
		.sceKernelExitVSHVSHCheck2 = 0x000016D8,
	},
	.loadexec_patch_05g = {
		.LoadReboot = 0x00000000,
		.LoadRebootCall = 0x00002FA8,
		.RebootJump = 0x00002FF4,
		.sceKernelLoadExecWithApiTypeCheck1 = 0x00002624,
		.sceKernelLoadExecWithApiTypeCheck2 = 0x00002668,
		.sceKernelExitVSHVSHCheck1 = 0x000016A4,
		.sceKernelExitVSHVSHCheck2 = 0x000016D8,
	},
	.impose_patch = {
		6,
		0x000005EC,
	},
	.usb_patch = {
		.scePowerBatteryDisableUsbChargingStub = 0x00009050,
		.scePowerBatteryEnableUsbChargingStub = 0x00009048,
	},
};
#endif

#ifdef CONFIG_635
PatchOffset g_635_offsets = {
	.fw_version = FW_635,
	.interruptman_patch = {
		.InvalidSyscallCheck1 = 0x00000DE8,
		.InvalidSyscallCheck2 = 0x00000E94,
	},
	.mediasync_patch = {
		.sceSystemFileGetIndex = 0x00000F40,
		.MsCheckMediaCheck = 0x00000744,
		.DiscCheckMediaCheck1 = 0x000003C4,
		.DiscCheckMediaCheck2 = 0x00000DC8,
		.MsSystemFileCheck = 0x000010B4,
		.DiscIDCheck1 = 0x00000FC0,
		.DiscIDCheck2 = 0x00000FDC,
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
		.mesg_decrypt_call = {
			/*0x457B0CF0  0xE71C010B  0xADF305F0  0x457B0AF0  0x380280F0 */
			{ 0x00001DF4, 0x00003884, 0x00003AE0, 0x00001E84, 0x00001ADC }, // 01g
			{ 0x00001EEC, 0x00003DF4, 0x00004098, 0x00001F7C, 0x00001B70 }, // 02g
			{ 0x0000200C, 0x0000431C, 0x00004608, 0x0000209C, 0x00001BB8 }, // 03g
			{ 0x0000200C, 0x0000431C, 0x00004608, 0x0000209C, 0x00001BB8 }, // 04g
			{ 0x00002134, 0x000047F0, 0x00004B24, 0x000021C4, 0x00001C04 }, // 05g
			{ 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF }, // 06g
			{ 0x0000200C, 0x0000431C, 0x00004608, 0x0000209C, 0x00001BB8 }, // 07g
			{ 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF }, // 08g
			{ 0x0000200C, 0x0000431C, 0x00004608, 0x0000209C, 0x00001BB8 }, // 09g
		},
		.mesgled_decrypt = 0x000000E0,
	},
	.systemctrl_export_patch = {
		.sctrlKernelLoadExecVSHWithApitype = 0x0000236C,
		.sctrlKernelLoadExecVSHWithApitype_05g = 0x000025C0,
		.sctrlKernelSetUserLevel = 0x00019E80,
		.sctrlKernelSetDevkitVersion = 0x88011998,
		.sctrlHENFindDriver = 0x00002A44,
		.sctrlKernelSetUMDEmuFile = 0x000099B0 + 0x00000008,   /* See 0x00005C2C */
		.sctrlKernelSetInitFileName = 0x000099B0 + 0x00000004, /* See 0x00004F58 */
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
		.memlmd_323366CA_NID = 0x3F2AC9C6,
		.memlmd_7CF1CD3E_NID = 0xE42AFE2E,
	},
	.loadexec_patch_other = {
		.LoadReboot = 0x00000000,
		.LoadRebootCall = 0x00002D44,
		.RebootJump = 0x00002D90,
		.sceKernelLoadExecWithApiTypeCheck1 = 0x000023B8,
		.sceKernelLoadExecWithApiTypeCheck2 = 0x000023FC,
		.sceKernelExitVSHVSHCheck1 = 0x0000168C,
		.sceKernelExitVSHVSHCheck2 = 0x000016C0,
	},
	.loadexec_patch_05g = {
		.LoadReboot = 0x00000000,
		.LoadRebootCall = 0x00002F90,
		.RebootJump = 0x00002FDC,
		.sceKernelLoadExecWithApiTypeCheck1 = 0x0000260C,
		.sceKernelLoadExecWithApiTypeCheck2 = 0x00002650,
		.sceKernelExitVSHVSHCheck1 = 0x0000168C,
		.sceKernelExitVSHVSHCheck2 = 0x000016C0,
	},
	.impose_patch = {
		6,
		0x000005EC,
	},
	.usb_patch = {
		.scePowerBatteryDisableUsbChargingStub = 0x00009050,
		.scePowerBatteryEnableUsbChargingStub = 0x00009048,
	},
};
#endif

#ifdef CONFIG_620
PatchOffset g_620_offsets = {
	.fw_version = FW_620,
	.interruptman_patch = {
		.InvalidSyscallCheck1 = 0x00000DE8,
		.InvalidSyscallCheck2 = 0x00000E94,
	},
	.mediasync_patch = {
		.sceSystemFileGetIndex = 0x00000F00,
		.MsCheckMediaCheck = 0x0000071C,
		.DiscCheckMediaCheck1 = 0x0000039C,
		.DiscCheckMediaCheck2 = 0x00000DA0,
		.MsSystemFileCheck = 0x00001074,
		.DiscIDCheck1 = 0x00000F80,
		.DiscIDCheck2 = 0x00000F9C,
		.sceSystemFileGetIndexCall = 0x00000954,
	},
	.memlmd_patch_01g = {
		.memlmd_unsigner = 0x00000F10,
		.memlmd_decrypt = 0x00000134,
		.sceMemlmdInitializeScrambleKey = 0x00001158,
		.memlmd_unsigner_call1 = 0x000010D8,
		.memlmd_unsigner_call2 = 0x0000112C,
		.memlmd_decrypt_call1 = 0x00000E10,
		.memlmd_decrypt_call2 = 0x00000E74,
	},
	.memlmd_patch_other = {
		.memlmd_unsigner = 0x00000FA8,
		.memlmd_decrypt = 0x00000134,
		.sceMemlmdInitializeScrambleKey = 0x000011F0,
		.memlmd_unsigner_call1 = 0x00001170,
		.memlmd_unsigner_call2 = 0x000011C4,
		.memlmd_decrypt_call1 = 0x00000EA8,
		.memlmd_decrypt_call2 = 0x00000F0C,
	},
	.mesgled_patch = {
		.mesg_decrypt_call = {
			/*0x457B0CF0  0xE71C010B  0xADF305F0  0x457B0AF0  0x380208F0 */
			{ 0x00001E3C, 0x00003808, 0x00003BC4, 0x00001ECC, 0x00001908 }, // 01g
			{ 0x00001ECC, 0x00003D10, 0x0000415C, 0x00001F5C, 0x00001950 }, // 02g
			{ 0x00001F5C, 0x000041F0, 0x00004684, 0x00001FEC, 0x00001998 }, // 03g
			{ 0x00001F5C, 0x000041F0, 0x00004684, 0x00001FEC, 0x00001998 }, // 04g
			{ 0x00001FEC, 0x00004674, 0x00004B50, 0x0000207C, 0x000019E0 }, // 05g
			{ 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF }, // 06g
			{ 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF }, // 07g
			{ 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF }, // 08g
			{ 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF, 0xDEADBEEF }, // 09g
		},
		.mesgled_decrypt = 0x000000E0,
	},
	.systemctrl_export_patch = {
		.sctrlKernelLoadExecVSHWithApitype = 0x00002304,
		.sctrlKernelLoadExecVSHWithApitype_05g = 0x00002558,
		.sctrlKernelSetUserLevel = 0x00019E80,
		.sctrlKernelSetDevkitVersion = 0x88011AAC,
		.sctrlHENFindDriver = 0x00002A38,
		.sctrlKernelSetUMDEmuFile = 0x00009990 + 0x00000008,   /* See 0x00005BFC */
		.sctrlKernelSetInitFileName = 0x00009990 + 0x00000004, /* See 0x00004F28 */
	},
	.validate_stub_patch = {
		.StartModule = 0x00006FD4,
		.StartModuleCall = 0x00000290,
	},
	.march33_patch = {
		.MsMediaInsertedCheck = 0x000009E8,
	},
	.psnfix_patch = {
		.NPSignupImageVersion = 0x000331EC,
		.NPSigninCheck = 0x00006C4C,
		.NPSigninImageVersion = 0x000095F0,
		.NPMatchVersionMajor = 0x00004604,
		.NPMatchVersionMinor = 0x0000460C,
	},
	.wlan_driver_patch = {
		.FreqCheck = 0x00002690,
	},
	.power_service_patch = {
		.scePowerGetBacklightMaximumCheck = 0x00000CC8,
	},
	.nid_resolver_patch = {
		.sceKernelLinkLibraryEntries = 0x000011DC,
		.sceKernelLinkLibraryEntriesCall = 0x0000842C,
		.sceKernelLinkLibraryEntriesForUser = 0x0000292C,
		.sceKernelLinkLibraryEntriesForUserCall = 0x00008178,
		.sceKernelIcacheClearAll = 0x00007F6C,
		.sceSysconPowerStandby = 0x00002C64,
	},
	.start_module_patch = {
		.sceInitBootStartCall = 0x00001DB0,
		.sceKernelStartModuleBootStartOffset = 0x00001CC4 - 0x00001A4C,
	},
	.high_memory_patch = {
		.get_partition = 0x88003E2C,
		.umd_cache_module_start = 0x000009C8,
	},
	.sysmemforuser_patch = {
		{ 0x00009B4C, 0x1F }, // sceKernelSetCompiledSdkVersion
		{ 0x00009C6C, 0x12 }, // SysMemUserForUser_342061E5
		{ 0x00009D04, 0x18 }, // SysMemUserForUser_315AD3A0
		{ 0x00009DB4, 0x1C }, // SysMemUserForUser_EBD5C3E6
		{ 0x00009E88, 0x15 }, // SysMemUserForUser_057E7380
		{ 0x00009F2C, 0x15 }, // SysMemUserForUser_91DE343C
		{ 0x00009FD0, 0x12 }, // SysMemUserForUser_7893F79A
		{ 0x0000A068, 0x18 }, // SysMemUserForUser_35669D4C
		{ 0x0000FFFF, 0x12 }, // SysMemUserForUser_1B4217BC Missing in 6.20
	},
	.module_handler_patch = {
		.ProbeExec3 = 0x0000885C,
		.ProbeExec3Call = 0x00007C3C,
		.sceKernelCheckExecFileImport = 0x00008854,
		.PartitionCheck = 0x00007FC0,
		.PartitionCheckCall1 = 0x000064FC,
		.PartitionCheckCall2 = 0x00006878,
		.DeviceCheck1 = 0x00000760,
		.DeviceCheck2 = 0x000007C0,
		.DeviceCheck3 = 0x000030B0,
		.DeviceCheck4 = 0x0000310C,
		.DeviceCheck5 = 0x00003138,
		.DeviceCheck6 = 0x00003444,
		.DeviceCheck7 = 0x0000349C,
		.DeviceCheck8 = 0x000034C8,
		.PrologueModule = 0x00008114,
		.PrologueModuleCall = 0x00007028,
	},
	.loadercore_patch = {
		.sceKernelCheckExecFile = 0x000086B4,
		.sceKernelCheckExecFileCall1 = 0x00001578,
		.sceKernelCheckExecFileCall2 = 0x000015C8,
		.sceKernelCheckExecFileCall3 = 0x00004A18,
		.ReloactionTable = 0x00008B58,
		.ProbeExec1 = 0x000061D4,
		.ProbeExec1Call = 0x000046A4,
		.ProbeExec2 = 0x000060F0,
		.ProbeExec2Call1 = 0x00004878,
		.ProbeExec2Call2 = 0x00004878, /* only one call in 6.20 */
		.EnableSyscallExport = 0x000040A4,
		.LoaderCoreCheck1 = 0x00007E84,
		.LoaderCoreCheck2 = 0x00006880,
		.LoaderCoreCheck3 = 0x00006990,
		.LoaderCoreUndo1Call1 = 0x00006914,
		.LoaderCoreUndo1Call2 = 0x00006944,
		.LoaderCoreUndo1Call3 = 0x000069DC,
		.LoaderCoreUndo2Call1 = 0x000041A4,
		.LoaderCoreUndo2Call2 = 0x000068F0,
		.memlmd_323366CA_NID = 0x2E208358,
		.memlmd_7CF1CD3E_NID = 0xCA560AA6,
	},
	.loadexec_patch_other = {
		.LoadReboot = 0x00000000,
		.LoadRebootCall = 0x00002CD8,
		.RebootJump = 0x00002D24,
		.sceKernelLoadExecWithApiTypeCheck1 = 0x00002350,
		.sceKernelLoadExecWithApiTypeCheck2 = 0x00002394,
		.sceKernelExitVSHVSHCheck1 = 0x00001674,
		.sceKernelExitVSHVSHCheck2 = 0x000016A8,
	},
	.loadexec_patch_05g = {
		.LoadReboot = 0x00000000,
		.LoadRebootCall = 0x00002F28,
		.RebootJump = 0x00002F74,
		.sceKernelLoadExecWithApiTypeCheck1 = 0x000025A4,
		.sceKernelLoadExecWithApiTypeCheck2 = 0x000025E8,
		.sceKernelExitVSHVSHCheck1 = 0x00001674,
		.sceKernelExitVSHVSHCheck2 = 0x000016A8,
	},
	.impose_patch = {
		5,
		0x000005D0,
	},
	.usb_patch = {
		.scePowerBatteryDisableUsbChargingStub = 0x00008FE8,
		.scePowerBatteryEnableUsbChargingStub = 0x00008FF0,
	},
};
#endif

PatchOffset *g_offs = NULL;

void setup_patch_offset_table(u32 fw_version)
{
#ifdef CONFIG_639
	if(fw_version == g_639_offsets.fw_version) {
		g_offs = &g_639_offsets;
	}
#endif

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
