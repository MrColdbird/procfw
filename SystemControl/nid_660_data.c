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
#include "systemctrl.h"
#include "nid_resolver.h"
#include "utils.h"

static nid_entry SysMemForKernel_nid[] = {
	{ 0xC7E57B9C, 0xFC639A2B, }, // sceKernelSetQTGP3
	{ 0xCF4DE78C, 0xC90B0992, }, // sceKernelGetUIDcontrolBlock
	{ 0xFC114573, 0xB4F00CB5, }, // sceKernelGetCompiledSdkVersion
	{ 0x3FC9AE6A, 0xC886B169, }, // sceKernelDevkitVersion
	{ 0x536AD5E1, 0xAC12F678, }, // SysMemForKernel_536AD5E1
	{ 0x636C953B, 0x23D81675, }, // sceKernelAllocHeapMemory
	{ 0xC9805775, 0xDD6512D0, }, // sceKernelDeleteHeap
	{ 0x7B749390, 0x87C2AB85, }, // sceKernelFreeHeapMemory
	{ 0x1C1FBFE7, 0x58148F07, }, // sceKernelCreateHeap
	{ 0x55A40B2C, 0xC4EEAF20, }, // sceKernelQueryMemoryPartitionInfo
	{ 0xE6581468, 0x13F4A0DE, }, // sceKernelPartitionMaxFreeMemSize
	{ 0x9697CD32, 0x0115B0F8, }, // sceKernelPartitionTotalFreeMemSize
	{ 0x237DBD4F, 0x7158CE7E, }, // sceKernelAllocPartitionMemory
	{ 0xB6D61D02, 0xC1A26C6F, }, // sceKernelFreePartitionMemory
	{ 0x9D9A5BA1, 0xF12A62F7, }, // sceKernelGetBlockHeadAddr
	{ 0xB2C7AA36, 0x83B5226D, }, // sceKernelSetDdrMemoryProtection
	{ 0x6373995D, 0x07C586A1, }, // sceKernelGetModel
	{ 0xEB7A74DB, 0xF2284ECC, }, // sceKernelAllocHeapMemoryWithOption
	{ 0xA823047E, 0x01810023, }, // sceKernelHeapTotalFreeSize
	{ 0xB2163AA1, UNKNOWNNID, }, // sceKernelGetHeapTypeCB
	{ 0xEFF0C6DD, UNKNOWNNID, }, // SysMemForKernel_EFF0C6DD
	{ 0xEFEEBAC7, UNKNOWNNID, }, // SysMemForKernel_EFEEBAC7
	{ 0x2DB687E9, UNKNOWNNID, }, // sceKernelIsValidHeap
	{ 0xA2A65F0E, 0xEA1CABF1, }, // sceKernelFillFreeBlock
	{ 0xEE867074, 0x8E742787, }, // sceKernelSizeLockMemoryBlock
	{ 0xCE5544F4, 0xFB5BEB66, }, // sceKernelResizeMemoryBlock
	{ 0x915EF4AC, 0xF7E78B33, }, // SysMemForKernel_915EF4AC
	{ 0x5EBE73DE, 0x1AB50974, }, // sceKernelJointMemoryBlock
	{ 0x2A3E5280, 0xFAF29F34, }, // sceKernelQueryMemoryInfo
	{ 0xEB4C0E1B, UNKNOWNNID, }, // sceKernelQueryBlockSize
	{ 0x82CCA14F, 0xE860BE8F, }, // sceKernelQueryMemoryBlockInfo
	{ 0x2F3B7611, UNKNOWNNID, }, // SysMemForKernel_2F3B7611
	{ 0x7B3E7441, 0x1E6BB8E8, }, // sceKernelMemoryExtendSize
	{ 0x6C1DCD41, 0x757EAC7D, }, // sceKernelCallUIDFunction
	{ 0x5367923C, 0x7D50CE82, }, // sceKernelCallUIDObjFunction
	{ 0xCE05CCB7, 0x235C2646, }, // SysMemForKernel_CE05CCB7
	{ 0x6CD838EC, 0x21487E5B, }, // sceKernelLookupUIDFunction
	{ 0xAD09C397, 0xD222DAA7, }, // sceKernelCreateUIDtypeInherit
	{ 0xFEFC8666, 0x034129FB, }, // sceKernelCreateUIDtype
	{ 0xD1BAB054, 0x1ECAB5C4, }, // sceKernelDeleteUIDtype
	{ 0x1C221A08, 0x3EC451EB, }, // sceKernelGetUIDname
	{ 0x2E3402CC, 0xA7622297, }, // sceKernelRenameUID
	{ 0x39357F07, 0x9F205D3E, }, // sceKernelGetUIDtype
	{ 0x89A74008, 0x0A34C078, }, // sceKernelCreateUID
	{ 0x8F20C4C0, 0x361F0F88, }, // sceKernelDeleteUID
	{ 0x55BFD686, 0xE3F9C38E, }, // sceKernelSearchUIDbyName
	{ 0x41FFC7F9, 0x44BDF332, }, // sceKernelGetUIDcontrolBlockWithType
	{ 0x82D3CEE3, 0xFFC63884, }, // SysMemForKernel_82D3CEE3
	{ 0xFC207849, 0x3E384043, }, // SysMemForKernel_FC207849
	{ 0xDFAEBD5B, 0xE13DDC87, }, // sceKernelIsHold
	{ 0x7BE95FA0, 0x7DDA0E01, }, // sceKernelHoldUID
	{ 0xFE8DEBE0, 0xB0CA8A4E, }, // sceKernelReleaseUID
	{ 0xBD5941B4, UNKNOWNNID, }, // sceKernelSysmemIsValidAccess
	{ 0x80F25772, UNKNOWNNID, }, // sceKernelIsValidUserAccess
	{ 0x3905D956, UNKNOWNNID, }, // sceKernelSysMemCheckCtlBlk
	{ 0x26F96157, UNKNOWNNID, }, // sceKernelSysMemDump
	{ 0x6D6200DD, UNKNOWNNID, }, // sceKernelSysMemDumpBlock
	{ 0x621037F5, UNKNOWNNID, }, // sceKernelSysMemDumpTail
	{ 0xA089ECA4, UNKNOWNNID, }, // sceKernelMemset
	{ 0x2F808748, 0x22A114DC, }, // sceKernelMemset32
	{ 0x1C4B1713, 0x9BAC123D, }, // sceKernelMemmove
	{ 0xE400FDB0, UNKNOWNNID, }, // sceKernelSysMemInit
	{ 0x1890BE9C, 0xFDBDEB5C, }, // sceKernelSysMemMemSize
	{ 0x03072750, 0x00154B6D, }, // sceKernelSysMemMaxFreeMemSize
	{ 0x811BED79, 0x45332D44, }, // sceKernelSysMemTotalFreeMemSize
	{ 0xF6C10E27, 0x57FA5AF4, }, // sceKernelGetSysMemoryInfo
	{ 0xCDA3A2F7, 0xFDC97D28, }, // SysMemForKernel_CDA3A2F7
	{ 0x960B888C, 0xE0EB8CBE, }, // SysMemForKernel_960B888C
	{ 0x452E3696, 0x36C503A9, }, // sceKernelGetSystemStatus
	{ 0x95F5E8DA, 0x521AC5A4, }, // sceKernelSetSystemStatus
	{ 0x0D547E7F, 0xEF29061C, }, // sceKernelGetUMDData
	{ 0xAB5E85E5, 0xEF29061C, }, // sceKernelGetUMDData (620 to 639)
	{ 0x419DB8F4, 0x310802A9, }, // sceKernelRegisterGetIdFunc
	{ 0x002BA296, 0x79BD1975, }, // sceKernelQueryHeapInfo
	{ 0x03808C51, 0x476B244F, }, // SysMemForKernel_03808C51
	{ 0xBB90D841, 0x53D50AC2, }, // SysMemForKernel_BB90D841
	{ 0x0D1D8AC8, 0x9B20ACEF, }, // sceKernelMemmoveWithFill
	{ 0x97B18FA8, 0xB9B7281A, }, // sceKernelCopyGameInfo
	{ 0xE40D3A1D, 0xF3BDB718, }, // sceKernelSetUmdData
	{ 0xCE8D3DB3, 0x474CA24F, }, // sceKernelGetQTGP2
	{ 0x55E4719C, 0x9E5B1ACB, }, // sceKernelSetQTGP2
	{ 0x6D8E0CDF, 0x43E0A861, }, // sceKernelGetQTGP3
	{ 0xA262FEF0, 0x4972F9D1, }, // sceKernelGetAllowReplaceUmd
	{ 0xF91FE6AA, 0x807179E7, }, // sceKernelSetParamSfo
	{ 0xDEC0A0D8, 0x6E4F9F62, }, // sceKernelGetCompilerVersion
	{ 0x7ECBDBD9, 0xBFD53FB7, }, // sceKernelGetDNAS
	{ 0x9C304ED7, 0x982A4779, }, // sceKernelSetDNAS
	{ 0xF4390489, 0xE73FBC0B, }, // sceKernelRebootKernel
	{ 0xA1ACEA31, 0xD0C1460D, }, // sceKernelGetId
	{ 0xE0058030, 0x7A7CD7BC, }, // sceKernelMemoryShrinkSize
	{ 0x719792F1, 0x35170484, }, // sceKernelMemoryOpenSize
	{ 0x101C3E59, 0xE5FB2EE5, }, // sceKernelMemoryCloseSize
	{ 0xE0645895, 0xF5228C40, }, // sceKernelGetSysmemIdList
	{ 0xCBB05241, 0xF19BA38D, }, // sceKernelSetAllowReplaceUmd
	{ 0x2269BFA2, 0x1404C1AA, }, // SysMemForKernel_2269BFA2
	{ 0xCD617A94, 0xEF29061C, }, // sceKernelGetGameInfo
	{ 0xF77D77CB, 0x476F687A, }, // sceKernelSetCompilerVersion
	{ 0x38495D84, 0x4A325AA0, }, // sceKernelGetInitialRandomValue
	{ 0x29A5899B, 0x96A3CE2C, }, // sceKernelSetRebootKernel
	{ 0x071D9804, 0xC5EC364C, }, // sceKernelApiEvaluationInit
	{ 0x02786087, 0xF5153060, }, // sceKernelRegisterApiEvaluation
	{ 0x1F7F7F40, 0xF8F54A95, }, // SysMemForKernel_1F7F7F40
	{ 0x049CC735, 0x845310CF, }, // sceKernelApiEvaluationReport
	{ 0x39351245, 0xFD15D334, }, // SysMemForKernel_39351245
	{ 0xD2E3A399, 0x5FE712E9, }, // SysMemForKernel_D2E3A399
	{ 0x4852F8DD, 0x1A726C89, }, // SysMemForKernel_4852F8DD
	{ 0x4EC43DC4, 0xE7A3F4D5, }, // SysMemForKernel_4EC43DC4
	{ 0x1F01A9E2, 0x6D9E2DD6, }, // SysMemForKernel_1F01A9E2
	{ 0xB9F8561C, 0xD3CA555C, }, // SysMemForKernel_B9F8561C
	{ 0x9452B542, 0xC4D3A378, }, // SysMemForKernel_9452B542
	{ 0xEF2EE8C1, 0xFE3CF2BC, }, // SysMemForKernel_EF2EE8C1
};

static nid_entry KDebugForKernel_nid[] = {
	{ 0xA2983280, 0x2C7D85FA, }, // sceKernelCheckDebugHandler
	{ 0x2FF4E9F9, 0xAA9B2F07, }, // sceKernelAssert
	{ 0x5282DD5E, 0xB20CABDB, }, // sceKernelDipswSet
	{ 0xEE75658D, 0xEFF672D1, }, // sceKernelDipswClear
	{ 0x428A8DA3, 0x47570AC5, }, // sceKernelIsToolMode
	{ 0x72E6B3B8, 0x27B23800, }, // sceKernelIsUMDMode
	{ 0xA216AE06, 0xB41E2430, }, // sceKernelIsDVDMode
	{ 0x9B868276, 0x276C02E3, }, // sceKernelGetDebugPutchar
	{ 0xE7A3874D, 0x0C56F87A, }, // sceKernelRegisterAssertHandler
	{ 0xAFB8FC80, 0x43F0F8AB, }, // sceKernelDipswLow32
	{ 0x23906FB2, 0x6CB0BDA4, }, // sceKernelDipswHigh32
	{ 0xE146606D, 0x01FFE328, }, // sceKernelRegisterDebugPutchar
	{ 0x7CEB2C09, 0xE201A0D8, }, // sceKernelRegisterKprintfHandler
	{ 0x5CE9838B, 0x2214799B, }, // sceKernelDebugWrite
	{ 0x66253C4E, 0x4A74A9D4, }, // sceKernelRegisterDebugWrite
	{ 0xDBB5597F, 0xD4EC38C1, }, // sceKernelDebugRead
	{ 0xE6554FDA, 0x48BE5AA9, }, // sceKernelRegisterDebugRead
	{ 0xB9C643C9, 0xE8FE3EE3, }, // sceKernelDebugEcho
	{ 0x7D1C74F0, 0xA1C5B42F, }, // sceKernelDebugEchoSet
	{ 0x24C32559, 0x86010FCB, }, // sceKernelDipsw

	{ 0x333DCEC7, 0x02668C61, },
	{ 0x8B041DFB, 0x8A0F4E94, },
	{ 0x9F8703E4, 0x568DCD25, },
	{ 0xFFD2F2B9, 0xACF427DC, },
	{ 0xA126F497, UNKNOWNNID, },
	{ 0xB7251823, UNKNOWNNID, },
};

static nid_entry LoadCoreForKernel_nid[] = {
	{ 0xCF8A41B1, 0xF6B1BF0F, }, // sceKernelFindModuleByName
	{ 0xCCE4A157, 0x40972E6E, }, // sceKernelFindModuleByUID
	{ 0xDD303D79, 0xBC99C625, }, // sceKernelFindModuleByAddress (5.XX NID)
	{ 0xFB8AE27D, 0xBC99C625, }, // sceKernelFindModuleByAddress (3.xx NID)
	{ 0xACE23476, 0xFC47F93A, }, // sceKernelCheckPspConfig
	{ 0x7BE1421C, 0xD3353EC4, }, // sceKernelCheckExecFile
	{ 0xBF983EF2, 0x41D10899, }, // sceKernelProbeExecutableObject
	{ 0x7068E6BA, 0x1C394885, }, // sceKernelLoadExecutableObject
	{ 0xB4D6FECC, UNKNOWNNID, }, // sceKernelApplyElfRelSection
	{ 0x54AB2675, UNKNOWNNID, }, // sceKernelApplyPspRelSection
	{ 0x2952F5AC, UNKNOWNNID, }, // sceKernelDcacheWBinvAll
	{ 0xD8779AC6, UNKNOWNNID, }, // sceKernelIcacheClearAll not exported any more
	{ 0x99A695F0, 0x48AF96A9, }, // sceKernelRegisterLibrary
	{ 0x5873A31F, 0x2C60CCB8, }, // sceKernelRegisterLibraryForUser
	{ 0x0B464512, 0xCB636A90, }, // sceKernelReleaseLibrary
	{ 0x9BAF90F6, 0x538129F8, }, // sceKernelCanReleaseLibrary
	{ 0x0E760DBA, 0x8EAE9534, }, // sceKernelLinkLibraryEntries
	{ 0x0DE1F600, 0x6ECFFFBA, }, // sceKernelLinkLibraryEntriesForUser
	{ 0xDA1B09AA, UNKNOWNNID, }, // sceKernelUnLinkLibraryEntries
	{ 0xC99DD47A, 0x696594C8, }, // sceKernelQueryLoadCoreCB
	{ 0x616FCCCD, 0xF976EF41, }, // sceKernelSetBootCallbackLevel
	{ 0xF32A2940, UNKNOWNNID, }, // sceKernelModuleFromUID
	{ 0x6B2371C2, 0x001B57BB, }, // sceKernelDeleteModule
	{ 0x7320D964, UNKNOWNNID, }, // sceKernelModuleAssign
	{ 0x44B292AB, UNKNOWNNID, }, // sceKernelAllocModule
	{ 0xBD61D4D5, UNKNOWNNID, }, // sceKernelFreeModule
	{ 0xAE7C6E76, 0xBF2E388C, }, // sceKernelRegisterModule
	{ 0x82CE54ED, UNKNOWNNID, }, // sceKernelModuleCount
	{ 0xC0584F0C, UNKNOWNNID, }, // sceKernelGetModuleList
	{ 0x7E63F86D, UNKNOWNNID, }, // sceKernelGetLibraryLinkInfo
	{ 0xB370DF29, UNKNOWNNID, }, // sceKernelGetLibraryList
	{ 0x5248A98F, 0x493EE781, }, // sceKernelLoadModuleBootLoadCore
	{ 0xC0913394, 0xA481E30E, }, // sceKernelLinkLibraryEntriesWithModule
	{ 0xEC20C661, 0xB27CC244, }, // LoadCoreForKernel_EC20C661
	{ 0xCDCAA8B2, 0x5FDDB07A, }, // LoadCoreForKernel_CDCAA8B2
	{ 0xCD0F3BAC, 0x2C44F793, }, // sceKernelCreateModule
	{ 0x8D8A8ACE, 0xF3DD4808, }, // sceKernelAssignModule
	{ 0x74CF001A, 0xB17F5075, }, // sceKernelReleaseModule
	{ 0x05D915DB, 0x37E6F41B, }, // sceKernelGetModuleIdListForKernel
	{ 0x52A86C21, 0xCD26E0CA, }, // sceKernelGetModuleFromUID
	{ 0xAFF947D4, 0x84D5C971, }, // sceKernelCreateAssignModule
	{ 0xA6D40F56, 0x410084F9, }, // LoadCoreForKernel_A6D40F56
	{ 0x929B5C69, 0x3FE631F0, }, // sceKernelGetModuleListWithAlloc
	{ 0x4440853B, 0x1999032F, }, // LoadCoreForKernel_4440853B
	{ 0xFA3101A4, 0xB6C037EA, }, // LoadCoreForKernel_FA3101A4
	{ 0x0C0D8B7F, 0x1915737F, }, // LoadCoreForKernel_0C0D8B7F
	{ 0x3BB7AC18, 0xC8FF5EE5, }, // sceKernelLoadCoreMode
};

static nid_entry LoadExecForKernel_nid[] = {
	{ 0x6D302D3D, 0xC3474C2A, }, // sceKernelExitVSHKernel
	{ 0x28D0D249, 0xD940C83C, }, // sceKernelLoadExecVSHMs2
	{ 0xA3D5E142, 0x08F7166C, }, // sceKernelExitVSHVSH
	{ 0x1B97BDB3, 0xD8320A28, }, // sceKernelLoadExecVSHDisc(3.xx)
	{ 0x94A1C627, 0x08F7166C, }, // sceKernelExitVSHVSH
	{ 0xBD2F1094, UNKNOWNNID, }, // sceKernelLoadExec
	{ 0x2AC9954B, UNKNOWNNID, }, // sceKernelExitGameWithStatus
	{ 0x05572A5F, UNKNOWNNID, }, // sceKernelExitGame
	{ 0xAC085B9E, UNKNOWNNID, }, // sceKernelLoadExecVSHFromHost
	{ 0x821BE114, 0xD4B49C4B, }, // sceKernelLoadExecVSHDiscUpdater
	{ 0x015DA036, 0xBEF585EC, }, // sceKernelLoadExecBufferVSHUsbWlan
	{ 0x4F41E75E, 0x2B8813AF, }, // sceKernelLoadExecBufferVSHUsbWlanDebug
	{ 0x31DF42BF, 0x4FB44D27, }, // sceKernelLoadExecVSHMs1
	{ 0x70901231, 0xCC6A47D2, }, // sceKernelLoadExecVSHMs3
	{ 0x62C459E1, UNKNOWNNID, }, // sceKernelLoadExecBufferVSHPlain
	{ 0x918782E8, UNKNOWNNID, }, // sceKernelLoadExecBufferVSHFromHost
	{ 0xBB28E9B7, UNKNOWNNID, }, // sceKernelLoadExecBufferPlain0
	{ 0x71A1D738, UNKNOWNNID, }, // sceKernelLoadExecBufferPlain
	{ 0x4D5C75BE, UNKNOWNNID, }, // sceKernelLoadExecFromHost
	{ 0x4AC57943, 0x1F88A490, }, // sceKernelRegisterExitCallback
	{ 0xD9739B89, 0x24114598, }, // sceKernelUnregisterExitCallback
	{ 0x659188E1, 0xB57D0DEC, }, // sceKernelCheckExitCallback
	{ 0x62A27008, 0x1F08547A, }, // sceKernelInvokeExitCallback
	{ 0x7B7C47EF, 0x1B305B09, }, // sceKernelLoadExecVSHDiscDebug
	{ 0x061D9514, 0x00745486, }, // sceKernelLoadExecVSHMs4
	{ 0xB7AB08DA, 0x7CABED9B, }, // sceKernelLoadExecVSHMs5
	{ 0x11412288, 0xA5ECA6E3, }, // LoadExecForKernel_11412288
	{ 0x6D8D3A3A, 0xF9CFCF2F, }, // LoadExecForKernel_6D8D3A3A

	{ 0xD3D13800, 0xDBD0CF1B, },
	{ 0x15EB8575, 0xBC26BEEF, },
};

static nid_entry ModuleMgrForKernel_nid[] = {
	{ 0x977DE386, 0x939E4270, }, // sceKernelLoadModule
	{ 0x50F0C1EC, 0x3FF74DF1, }, // sceKernelStartModule
	{ 0xD1FF982A, 0xE5D6087B, }, // sceKernelStopModule
	{ 0x2E0911AA, 0x387E3CA9, }, // sceKernelUnloadModule
	{ 0x644395E2, 0x303FAB7F, }, // sceKernelGetModuleIdList
	{ 0xA1A78C58, 0xCE0A74A5, }, // sceKernelLoadModuleDisc
	{ 0x748CBED9, 0x22BDBEFF, }, // sceKernelQueryModuleInfo
	{ 0xD675EBB8, 0x5805C1CA, }, // sceKernelSelfStopUnloadModule
	{ 0xABE84F8A, UNKNOWNNID, }, // sceKernelLoadModuleBufferWithApitype
	{ 0xBA889C07, 0x4E62C48A, }, // sceKernelLoadModuleBuffer
	{ 0xB7F46618, UNKNOWNNID, }, // sceKernelLoadModuleByID
	{ 0x437214AE, UNKNOWNNID, }, // sceKernelLoadModuleWithApitype
	{ 0x710F61B5, UNKNOWNNID, }, // sceKernelLoadModuleMs
	{ 0x91B87FAE, 0xCBA02988, }, // sceKernelLoadModuleVSHByID
	{ 0xA4370E7C, 0xD5DDAB1F, }, // sceKernelLoadModuleVSH
	{ 0x23425E93, UNKNOWNNID, }, // sceKernelLoadModuleVSHPlain
	{ 0xF9275D98, UNKNOWNNID, }, // sceKernelLoadModuleBufferUsbWlan
	{ 0xF0CAC59E, 0xC6DE0B9C, }, // sceKernelLoadModuleBufferVSH
	{ 0xCC1D3699, 0xE97E0DB7, }, // sceKernelStopUnloadSelfModule
	{ 0x04B7BD22, 0xD86DD11B, }, // sceKernelSearchModuleByName
	{ 0x54D9E02E, 0x12F99392, }, // sceKernelSearchModuleByAddress
	{ 0x5F0CC575, 0xCC873DFA, }, // sceKernelRebootBeforeForUser
	{ 0xB49FFB9E, 0x5FC3B3DA, }, // sceKernelRebootBeforeForKernel
	{ 0xF0A26395, 0xCAB06D30, }, // sceKernelGetModuleId
	{ 0xD8B73127, 0x433D5287, }, // sceKernelGetModuleIdByAddress
	{ 0x6DE9FF11, 0x2B7FC10D, }, // sceKernelLoadModuleWithApitype2
	{ 0x896C15B6, 0xCAE8E169, }, // sceKernelLoadModuleDiscUpdater
	{ 0x92EDEBD0, 0x2C4F270D, }, // sceKernelLoadModuleDiscDebug
	{ 0x6723BBFF, 0xFE61F16D, }, // sceKernelLoadModuleMs1
	{ 0x49C5B9E1, 0x7BD53193, }, // sceKernelLoadModuleMs2/sceKernelLoadModuleForLoadExecVSHMs2
	{ 0xECC2EAA9, 0xD60AB6CC, }, // sceKernelLoadModuleMs3
	{ 0xF07E1A2F, 0x76F0E956, }, // sceKernelLoadModuleMs4
	{ 0x0CE03872, 0x4E8A2C9D, }, // sceKernelLoadModuleForLoadExecVSHMs5
	{ 0x9F3AD1E9, 0x61E3EC69, }, // sceKernelLoadModuleBufferForExitGame
	{ 0x667C1174, 0x9236B422, }, // sceKernelLoadModuleBufferForExitVSHVSH
	{ 0x5799EE32, 0x253AA17C, }, // sceKernelLoadModuleBufferForExitVSHKernel
	{ 0x53CA70EE, 0x4E38EA1D, }, // sceKernelLoadModuleBufferForRebootKernel
	{ 0x2BE4807D, UNKNOWNNID, }, // sceKernelStopUnloadSelfModuleWithStatusKernel
	{ 0xA6E8C1F5, 0x9B7102E2, }, // sceKernelRebootPhaseForKernel
	{ 0x74690AEB, 0x74690AEB, }, // sceKernelLoadModuleByIDBootInitConfig
	{ 0xF80F3C96, 0xE8B9D19D, }, // sceKernelLoadModuleBufferBootInitConfig
	{ 0x61EC6AB0, 0x1CFFC5DE, }, // sceKernelModuleMgrMode

	{ 0xEF7A7F02, 0x1CF0B794, },
	{ 0x2BE2B17E, 0xC3DDABEF, },
	{ 0x25E1F458, 0x955D6CB2, },
	{ 0xF7E46A72, 0xA40EC254, },
	{ 0x1B91F6EC, 0x853A6C16, },
};

static nid_entry ExceptionManagerForKernel_nid[] = {
	{ 0x565C0B0E, 0x0A998599, }, // sceKernelRegisterDefaultExceptionHandler
	{ 0x3FB264FC, 0xFBC2353A, }, // sceKernelRegisterExceptionHandler
	{ 0x5A837AD4, 0x7C11759D, }, // sceKernelRegisterPriorityExceptionHandler
	{ 0x1AA6CFFA, 0x54B0CAD2, }, // sceKernelReleaseExceptionHandler
	{ 0xDF83875E, 0x27E6A663, }, // sceKernelGetActiveDefaultExceptionHandler
	{ 0x291FF031, 0x409DBB53, }, // sceKernelReleaseDefaultExceptionHandler
	{ 0x15ADC862, UNKNOWNNID, }, // sceKernelRegisterNmiHandler
	{ 0xB15357C9, UNKNOWNNID, }, // sceKernelReleaseNmiHandler

	{ 0xFC26C354, 0x79454858, },
};

static nid_entry IoFileMgrForKernel_nid[] = {
	{ 0x3C54E908, 0x947D7A06, }, // sceIoReopen
	{ 0x411106BA, 0x1FC0620B, }, // sceIoGetThreadCwd
	{ 0xA905B705, 0xC658603A, }, // sceIoCloseAll
	{ 0xCB0A151F, 0xC30581F4, }, // sceIoChangeThreadCwd
	{ 0x73B7671F, 0x18881E58, }, // sceIoGetFdDebugInfo(371)
	{ 0x9B86630B, 0xDCCD6185, }, // sceIoGetUID
	{ 0x30E8ABB3, 0x2B6A9B21, }, // IoFileMgrForKernel_30E8ABB3
	{ 0x42F954D4, 0x22F15793, }, // sceIoAddHook
	{ 0xBD17474F, 0x49356C12, }, // IoFileMgrForKernel_BD17474F
};

static nid_entry scePower_driver_nid[] = {
	{ 0xEFD3C963, 0x0EFEE60E, }, // scePowerTick
	{ 0x87440F5E, 0x872F4ECE, }, // scePowerIsPowerOnline
	{ 0x0AFD0D8B, 0x8C873AA7, }, // scePowerIsBatteryExist
	{ 0x1E490401, 0x7A9EA6DE, }, // scePowerIsBatteryCharging
	{ 0xD3075926, 0xFA651CE1, }, // scePowerIsLowBattery
	{ 0x2085D15D, 0x31AEA94C, }, // scePowerGetBatteryLifePercent
	{ 0x8EFB3FA2, 0xC79F9157, }, // scePowerGetBatteryLifeTime
	{ 0x28E12023, 0x40870DAC, }, // scePowerGetBatteryTemp
	{ 0x483CE86B, 0xF7DE0E81, }, // scePowerGetBatteryVolt
	{ 0x2B7C7CF4, 0x9B44CFD9, }, // scePowerRequestStandby
	{ 0xFEE03A2F, 0xFDB5BFE9, }, // scePowerGetCpuClockFrequency
	{ 0x478FE6F5, 0x04711DFB, }, // scePowerGetBusClockFrequency
	{ 0x737486F2, UNKNOWNNID, }, // scePowerSetClockFrequency
	{ 0xD6E50D7B, UNKNOWNNID, }, // scePowerRegisterCallback(3.71)
	{ 0x04B7766E, 0x766CD857, }, // scePowerRegisterCallback(1.50)
	{ 0x1688935C, 0x04711DFB, }, // scePowerGetBusClockFrequencyInt
	{ 0x6D2CA84B, UNKNOWNNID, }, // scePowerWlanActivate
	{ 0x23BB0A60, UNKNOWNNID, }, // scePowerWlanDeactivate
	{ 0x2B51FE2F, 0xCE2032CD, }, // scePower_driver_2B51FE2F
	{ 0x442BFBAC, 0x2509FF3B, }, // scePowerGetBacklightMaximum
	{ 0xE8685403, UNKNOWNNID, }, // scePower_driver_E8685403
	{ 0xEDC13FE5, 0xDF336CDE, }, // scePowerGetIdleTimer
	{ 0x7F30B3B1, 0x1E3B1FAE, }, // scePowerIdleTimerEnable
	{ 0x972CE941, 0x961A06A5, }, // scePowerIdleTimerDisable
	{ 0x27F3292C, 0x0DA940D2, }, // scePowerBatteryUpdateInfo
	{ 0xE8E4E204, 0xA641CF3F, }, // scePower_driver_E8E4E204
	{ 0xB999184C, 0x7B908CAA, }, // scePowerGetLowBatteryCapacity
	{ 0x166922EC, UNKNOWNNID, }, // scePowerBatteryForbidCharging
	{ 0xDD3D4DAC, UNKNOWNNID, }, // scePowerBatteryPermitCharging
	{ 0xB4432BC8, 0x67492C52, }, // scePowerGetBatteryChargingStatus
	{ 0x78A1A796, 0x88C79735, }, // scePowerIsSuspendRequired
	{ 0x94F5A53F, 0x41ADFF48, }, // scePowerGetBatteryRemainCapacity
	{ 0xFD18A0FF, 0x003B1E03, }, // scePowerGetBatteryFullCapacity
	{ 0x862AE1A6, 0x993B8C4A, }, // scePowerGetBatteryElec
	{ 0x23436A4A, 0xC730F432, }, // scePowerGetInnerTemp
	{ 0x0CD21B1F, 0x73785D34, }, // scePowerSetPowerSwMode
	{ 0x165CE085, 0xE11CDFFA, }, // scePowerGetPowerSwMode
	{ 0xD6D016EF, UNKNOWNNID, }, // scePowerLock
	{ 0xCA3D34C1, UNKNOWNNID, }, // scePowerUnlock
	{ 0x79DB9421, UNKNOWNNID, }, // scePowerRebootStart
	{ 0x7FA406DD, 0x566B8353, }, // scePowerIsRequest
	{ 0xAC32C9CC, 0x5C1333B7, }, // scePowerRequestSuspend
	{ 0x2875994B, 0xD1FFF513, }, // scePower_driver_2875994B
	{ 0x3951AF53, 0x3300D85A, }, // scePowerEncodeUBattery
	{ 0x0074EF9B, 0xB45C9066, }, // scePowerGetResumeCount
	{ 0xF535D928, UNKNOWNNID, }, // scePowerSetWakeupCondition
	{ 0xDFA8BAF8, 0x315B8CB6, }, // scePowerUnregisterCallback
	{ 0xDB9D28DD, UNKNOWNNID, }, // scePowerUnregitserCallback
	{ 0xD24E6BEB, UNKNOWNNID, }, // scePowerLimitScCpuClock
	{ 0x35B7662E, UNKNOWNNID, }, // scePowerGetSectionDescriptionEntry
	{ 0xF9B4DEA1, UNKNOWNNID, }, // scePowerLimitPllClock
	{ 0x843FBF43, 0x53808CBB, }, // scePowerSetCpuClockFrequency
	{ 0xB8D7B3FB, 0xB71A8B2F, }, // scePowerSetBusClockFrequency
	{ 0x478FE6F5, 0x04711DFB, }, // scePowerGetBusClockFrequency
	{ 0xBD681969, 0x04711DFB, }, // scePowerGetBusClockFrequencyInt
	{ 0xB1A52C83, 0xDC4395E2, }, // scePowerGetCpuClockFrequencyFloat
	{ 0x9BADB3EB, 0x1FF8DA3B, }, // scePowerGetBusClockFrequencyFloat
	{ 0xE0B7A95D, UNKNOWNNID, }, // scePowerSetGeEdramRefreshMode
	{ 0xC23AC778, UNKNOWNNID, }, // scePowerGetGeEdramRefreshMode
	{ 0x23C31FFE, 0x70F42744, }, // scePowerVolatileMemLock
	{ 0xFA97A599, 0xA882AEB7, }, // scePowerVolatileMemTryLock
	{ 0xB3EDD801, 0x5978B1C2, }, // scePowerVolatileMemUnlock
	{ 0xA9D22232, 0x29E23416, }, // scePowerSetCallbackMode
	{ 0xBAFA3DF0, 0x17EEA285, }, // scePowerGetCallbackMode
	{ 0x57A098B4, UNKNOWNNID, }, // scePowerGetCurrentTachyonVoltage(2.80)
	{ 0x55D2D789, UNKNOWNNID, }, // scePowerGetTachyonVoltage
	{ 0xDD27F119, UNKNOWNNID, }, // scePowerGetTachyonVoltage
	{ 0x75F91FE4, UNKNOWNNID, }, // scePowerGetCurrentDdrVoltage
	{ 0x06C83A4F, UNKNOWNNID, }, // scePowerGetDdrVoltage
	{ 0x4E759486, UNKNOWNNID, }, // scePowerSetDdrVoltage
	{ 0x53C1255D, UNKNOWNNID, }, // scePower_driver_53C1255D
	{ 0x8762BCE6, UNKNOWNNID, }, // scePower_driver_8762BCE6
	{ 0xCAE2A4CA, UNKNOWNNID, }, // scePower_driver_CAE2A4CA
	{ 0xF06B96D4, UNKNOWNNID, }, // scePower_driver_F06B96D4
	{ 0x34F9C463, 0x67BD889B, }, // scePowerGetPllClockFrequencyInt
	{ 0xEA382A27, 0xBA8CBCBF, }, // scePowerGetPllClockFrequencyFloat
	{ 0x3234844A, UNKNOWNNID, }, // scePower_driver_3234844A
	{ 0xA4D02055, UNKNOWNNID, }, // scePower_driver_A4D02055
	{ 0x9C40E184, 0x0EFEE60E, }, // scePowerTick(371)
	{ 0xCB49F5CE, 0x8432901E, }, // scePowerGetBatteryChargeCycle
	{ 0x0442D852, 0x9DAF25A0, }, // scePowerRequestColdReset
	{ 0x733F973B, UNKNOWNNID, }, // scePowerBatteryEnableUsbCharging
	{ 0x90285886, UNKNOWNNID, }, // scePowerBatteryDisableUsbCharging

	{ 0x9B1A9C5F, 0x9B44CFD9, },
	{ 0xE65F00BD, 0x9DAF25A0, },
	{ 0xD7B9C925, UNKNOWNNID, },
};

// TODO
static nid_entry InterruptManagerForKernel_nid[] = {
	{ 0x8A389411, 0x4D6E7305, }, // sceKernelDisableSubIntr
	{ 0xD2E8363F, UNKNOWNNID, }, // QueryIntrHandlerInfo
	{ 0x8B61808B, UNKNOWNNID, }, // sceKernelQuerySystemCall
	{ 0x53991063, UNKNOWNNID, }, // InterruptManagerForKernel_53991063
	{ 0x468BC716, UNKNOWNNID, }, // sceKernelGetInterruptExitCount
	{ 0x43CD40EF, UNKNOWNNID, }, // ReturnToThread
	{ 0x85F7766D, UNKNOWNNID, }, // SaveThreadContext
	{ 0x00B6B0F3, UNKNOWNNID, }, // QueryInterruptManCB
	{ 0x15894D0B, 0xA1B88367, }, // InterruptManagerForKernel_15894D0B
	{ 0xB5A15B30, 0xB941600E, }, // sceKernelSetIntrLevel
	{ 0x43A7BBDC, 0xAB1FC793, }, // InterruptManagerForKernel_43A7BBDC
	{ 0x02475AAF, UNKNOWNNID, }, // sceKernelIsInterruptOccurred
	{ 0x750E2507, UNKNOWNNID, }, // sceKernelSuspendIntr
	{ 0x494D6D2B, UNKNOWNNID, }, // sceKernelResumeIntr
	{ 0x2CD783A1, UNKNOWNNID, }, // RegisterContextHooks
	{ 0x55242A8B, UNKNOWNNID, }, // ReleaseContextHooks
	{ 0x27BC9A45, UNKNOWNNID, }, // UnSupportIntr
	{ 0x0E224D66, UNKNOWNNID, }, // SupportIntr
	{ 0x272766F8, UNKNOWNNID, }, // sceKernelRegisterDebuggerIntrHandler
	{ 0xB386A459, UNKNOWNNID, }, // sceKernelReleaseDebuggerIntrHandler
	{ 0xCDC86B64, UNKNOWNNID, }, // sceKernelCallSubIntrHandler
	{ 0xD6878EB6, UNKNOWNNID, }, // sceKernelGetUserIntrStack
	{ 0xF4454E44, UNKNOWNNID, }, // sceKernelCallUserIntrHandler
	{ 0xCA04A2B9, UNKNOWNNID, }, // sceKernelRegisterSubIntrHandler
	{ 0x5CB5A78B, UNKNOWNNID, }, // sceKernelSuspendSubIntr
	{ 0x7860E0DC, UNKNOWNNID, }, // sceKernelResumeSubIntr
	{ 0xFC4374B8, 0x4D6E7305, }, // sceKernelIsSubInterruptOccurred
	{ 0x35634A64, 0xF9E06DF1, }, // sceKernelGetCpuClockCounterWide
	{ 0x2DC9709B, 0x6DDA4D7B, }, // _sceKernelGetCpuClockCounterLow
	{ 0xE9E652A9, 0xE6FB16E3, }, // _sceKernelGetCpuClockCounterHigh
	{ 0x0FC68A56, UNKNOWNNID, }, // sceKernelSetPrimarySyscallHandler
	{ 0xF4D443F3, UNKNOWNNID, }, // sceKernelRegisterSystemCallTable
	{ 0x36B1EF81, 0x68B5CA51, }, // sceKernelQueryIntrHandlerInfo
	{ 0xEB988556, UNKNOWNNID, }, // sceKernelQuerySystemCall(5xx)

	{ 0x07E138EE, UNKNOWNNID, },
	{ 0xD8410DE6, UNKNOWNNID, },
	{ 0x8DC2BAEF, 0x1373F20E, },
	{ 0x0B907DBC, UNKNOWNNID, },
	{ 0x7A05D976, 0xDBD52A5D, },
	{ 0x7EF7500F, 0x19596CD3, },
	{ 0x05E9D6C4, 0x2F3C4DCD, },
	{ 0xD63F3B31, 0x2F3C4DCD, },
	{ 0x72CE22E8, UNKNOWNNID, },
};

static nid_entry sceCtrl_driver_nid[] = {
	{ 0x454455AC, 0xBE30CED0, }, // sceCtrlReadBufferPositive
	{ 0xC4AAD55F, 0x2BA616AF, }, // sceCtrlPeekBufferPositive
	{ 0x28E71A16, 0xF6E94EA3, }, // sceCtrlSetSamplingMode
	{ 0x3E65A0EA, 0x121097D5, }, // sceCtrlInit
	{ 0xE03956E9, 0x1A1A7D40, }, // sceCtrlEnd
	{ 0xC3F607F3, 0x55497589, }, // sceCtrlSuspend
	{ 0xC245B57B, 0x33D03FD5, }, // sceCtrlResume
	{ 0xA88E8D22, 0x6A1DF4CB, }, // sceCtrlSetIdleCancelKey
	{ 0xB7CEAED4, 0x7511CCFE, }, // sceCtrlGetIdleCancelKey
	{ 0x7CA723DC, 0xF8346777, }, // sceCtrl_driver_7CA723DC
	{ 0x5E77BC8A, 0x1809B9FC, }, // sceCtrl_driver_5E77BC8A
	{ 0x5C56C779, 0xDF53E160, }, // sceCtrl_driver_5C56C779
	{ 0x1F4011E6, 0xF6E94EA3, }, // sceCtrlSetSamplingMode(150)
	{ 0x3A622550, 0x2BA616AF, }, // sceCtrlPeekBufferPositive(150)
	{ 0xC152080A, 0xE6085C33, }, // sceCtrlPeekBufferNegative(150)
	{ 0x1F803938, 0xBE30CED0, }, // sceCtrlReadBufferPositive(150)
	{ 0x60B81F86, 0x3A6A612A, }, // sceCtrlReadBufferNegative(150)
	{ 0xDA6B76A1, 0xF8EC18BD, }, // sceCtrlGetSamplingMode
	{ 0x6A2774F3, 0x83B15A81, }, // sceCtrlSetSamplingCycle
	{ 0xB1D0E5CD, 0x637CB76C, }, // sceCtrlPeekLatch
	{ 0x0B588501, 0x7F7C4E0A, }, // sceCtrlReadLatch
	{ 0xA7144800, 0x37533267, }, // sceCtrlSetIdleCancelThreshold
	{ 0x687660FA, 0xE54253E7, }, // sceCtrlGetIdleCancelThreshold
	{ 0x6841BE1A, 0x89438C13, }, // sceCtrlSetRapidFire
	{ 0x02BAAD91, 0x501E0C70, }, // sceCtrlGetSamplingCycle
	{ 0xA68FD260, 0x994488EC, }, // sceCtrlClearRapidFire
	{ 0x252D3A8D, 0xF0074903, }, // sceCtrlSetPollingMode

	{ 0x6D74BF08, 0x83B15A81, },
	{ 0xFF847C31, 0x3A6A612A, },
	{ 0x348D99D4, 0x547F89D3, },
	{ 0xAF5960F3, 0xBC8D1A3B, },
};

// TODO BELOW
static nid_entry sceDisplay_driver_nid[] = {
	{ 0x4AB7497F, 0x37533141, }, // sceDisplaySetFrameBuf
	{ 0xB685BA36, 0xC30D327D, }, // sceDisplayWaitVblankStart
	{ 0xE56B11BA, 0x08A10838, }, // sceDisplayGetFrameBuf
	{ 0x776ADFDB, 0x89FD2128, }, // sceDisplaySetBacklightInternal
	{ 0x31C4BAA8, 0x3A5621E0, }, // sceDisplayGetBrightness
	{ 0x5B5AEFAD, 0xCCEEA6BB, }, // from 1.50
	{ 0xDBA6C4C4, 0x0D3248C1, }, // sceDisplayGetFramePerSec
	{ 0x432D133F, 0xA70E2B3C, }, // sceDisplayEnable
	{ 0x681EE6A7, 0xD4EF6579, }, // sceDisplayDisable
	{ 0x7ED59BC4, 0x73CA5F45, }, // sceDisplaySetHoldMode
	{ 0xA544C486, 0x3DABE438, }, // sceDisplaySetResumeMode
	{ 0x63E22A26, 0xDF998667, }, // sceDisplay_driver_63E22A26
	{ 0x289D82FE, 0x37533141, }, // sceDisplaySetFrameBuf
	{ 0xEEDA2E54, 0x08A10838, }, // sceDisplayGetFrameBuf
	{ 0xB4F378FA, 0x2D859B86, }, // sceDisplayIsForeground
	{ 0xAC14F1FF, 0x45FC50F8, }, // sceDisplayGetForegroundLevel
	{ 0x9E3C6DC6, 0x89FD2128, }, // sceDisplaySetBrightness
	{ 0x9C6EAAD7, 0x21F912F4, }, // sceDisplayGetVcount
	{ 0x4D4E10EC, 0x59456A3E, }, // sceDisplayIsVblank
	{ 0x69B53541, 0x2F701A85, }, // sceDisplayGetVblankRest
	{ 0x36CDFADE, 0xB6C8FD75, }, // sceDisplayWaitVblank
	{ 0x8EB9EC49, 0xA10EABE8, }, // sceDisplayWaitVblankCB
	{ 0x984C27E7, 0xC30D327D, }, // sceDisplayWaitVblankStart
	{ 0x46F186C3, 0x3E4B1B28, }, // sceDisplayWaitVblankStartCB
	{ 0x773DD3A3, 0xA87F508D, }, // sceDisplayGetCurrentHcount
	{ 0x210EAB3A, 0x7A3E7402, }, // sceDisplayGetAccumulatedHcount
	{ 0xBF79F646, 0xE1B2D7D3, }, // sceDisplayGetResumeMode
	{ 0xA83EF139, 0x575B0CC5, }, // sceDisplayAdjustAccumulatedHcount
	{ 0x21038913, 0x4E6526A4, }, // sceDisplayIsVsync
	{ 0x92C8F8B7, 0x823F72B2, }, // sceDisplayIsPseudoField
	{ 0xAE0E8972, 0x8930B3A9, }, // sceDisplaySetPseudoVsync

	{ 0x643F3841, 0xDF998667, },
	{ 0x820C6038, 0xCCEEA6BB, },
	{ 0x1CB8CB47, 0x3A5621E0, },
	{ 0x13AA96B7, 0x0D3248C1, },
	{ 0x66961521, 0x0163AE99, },
	{ 0xF5EEEFEF, 0x01F6B0D5, },
	{ 0xE8466BC2, 0x21F912F4, },
	{ 0x946155FD, 0xA70E2B3C, },
	{ 0x32B67781, 0xD4EF6579, },
};

static nid_entry sceAudio_driver_nid[] = {
	{ 0xB282F4B2, 0xA388ABDB, }, // sceAudioGetChannelRestLength
	{ 0x669D93E4, 0xE9BCD519, }, // sceAudioSRCChReserve
	{ 0x138A70F1, 0x0399579B, }, // sceAudioSRCChRelease
	{ 0x43645E69, 0x475F41FE, }, // sceAudioSRCOutputBlocking
	{ 0x80F1F7E0, 0x65BED4D9, }, // sceAudioInit
	{ 0x210567F7, 0xFADC7CBB, }, // sceAudioEnd
	{ 0xA2BEAA6C, 0x0BF0FA03, }, // sceAudioSetFrequency
	{ 0xB61595C0, 0x4FFADD67, }, // sceAudioLoopbackTest
	{ 0x927AC32B, 0xED4A5679, }, // sceAudioSetVolumeOffset
	{ 0x8C1009B2, 0xD35EFCD9, }, // sceAudioOutput
	{ 0x136CAF51, 0xA906D208, }, // sceAudioOutputBlocking
	{ 0xE2D56B2D, 0xC847973E, }, // sceAudioOutputPanned
	{ 0x13F592BC, 0xB29D727F, }, // sceAudioOutputPannedBlocking
	{ 0x5EC81C55, 0xF84226FB, }, // sceAudioChReserve
	{ 0x41EFADE7, 0x686AC4F4, }, // sceAudioOneshotOutput
	{ 0x6FC46853, 0xA6FD1A21, }, // sceAudioChRelease
	{ 0xB011922F, 0xA388ABDB, }, // sceAudioGetChannelRestLength
	{ 0xCB2E439E, 0x3F4D81C1, }, // sceAudioSetChannelDataLen
	{ 0x95FD0C2D, 0x5A0397C5, }, // sceAudioChangeChannelConfig
	{ 0xB7E1D8E7, 0xA25A3346, }, // sceAudioChangeChannelVolume
	{ 0x38553111, 0xE9BCD519, }, // sceAudioSRCChReserve
	{ 0x5C37C0AE, 0x0399579B, }, // sceAudioSRCChRelease
	{ 0xE0727056, 0x475F41FE, }, // sceAudioSRCOutputBlocking
	{ 0x086E5895, 0x459D3B55, }, // sceAudioInputBlocking
	{ 0x6D4BEC68, 0x84FA80C4, }, // sceAudioInput
	{ 0xA708C6A6, 0xE071AB41, }, // sceAudioGetInputLength
	{ 0x87B2E651, 0x8FB1537B, }, // sceAudioWaitInputEnd
	{ 0x7DE61688, 0xFE5A8DD1, }, // sceAudioInputInit
	{ 0xE926D3FB, 0x2E5E3227, }, // sceAudioInputInitEx
	{ 0xA633048E, 0x83C00002, }, // sceAudioPollInputEnd
	{ 0xE9D97901, 0x52F5804E, }, // sceAudioGetChannelRestLen
};

static nid_entry sceReg_driver_nid[] = {
	{ 0x98279CF1, 0xAEBFD786, }, // sceRegInit
	{ 0x9B25EDF1, 0x746E6D70, }, // sceRegExit
	{ 0x92E41280, 0x7F17123E, }, // sceRegOpenRegistry
	{ 0xFA8A5739, 0xA349B88E, }, // sceRegCloseRegistry
	{ 0xDEDA92BF, 0x079810CF, }, // sceRegRemoveRegistry
	{ 0x1D8A762E, 0xBA1A6A86, }, // sceRegOpenCategory
	{ 0x0CAE832B, 0xA1671F90, }, // sceRegCloseCategory
	{ 0x39461B4D, 0xAEF5B913, }, // sceRegFlushRegistry
	{ 0x0D69BF40, 0x55C9A620, }, // sceRegFlushCategory
	{ 0x57641A81, 0x79C81DCA, }, // sceRegCreateKey
	{ 0x17768E14, 0x5476A667, }, // sceRegSetKeyValue
	{ 0xD4475AA8, 0xA81437D0, }, // sceRegGetKeyInfo
	{ 0x28A8E98A, 0x693B0D7F, }, // sceRegGetKeyValue
	{ 0x2C0DB9DD, 0x38BC9D01, }, // sceRegGetKeysNum
	{ 0x2D211135, 0x4E285673, }, // sceRegGetKeys
	{ 0x4CA16893, 0x3F8C163C, }, // sceRegRemoveCategory
	{ 0x3615BC87, 0xECE409B6, }, // sceRegRemoveKey
	{ 0xC5768D02, 0x7149BA4A, }, // sceRegGetKeyInfoByName
	{ 0x30BE0259, 0x23B685FA, }, // sceRegGetKeyValueByName
	{ 0xBE8C1263, 0xC1881EB5, }, // sceReg_BE8C1263
	{ 0x835ECE6F, 0x44242CEB, }, // sceReg_835ECE6F
};

static nid_entry UtilsForKernel_nid[] = {
	{ 0x80FE032E, UNKNOWNNID, }, // sceUtilsKernelDcacheWBinvRange
	{ 0x7B7ED3FD, UNKNOWNNID, }, // sceKernelRegisterLibcRtcFunc
	{ 0x6151A7C3, UNKNOWNNID, }, // sceKernelReleaseLibcRtcFunc
};

static nid_entry SysclibForKernel_nid[] = {
//	{ 0x89B79CB1, UNKNOWNNID, }, // strcspn MISSING
//	{ 0x62AE052F, UNKNOWNNID, }, // strspn  MISSING
//	{ 0x87F8D2DA, UNKNOWNNID, }, // strtok  MISSING
//	{ 0x1AB53A58, UNKNOWNNID, }, // strtok_r  MISSING
	{ 0x1D83F344, UNKNOWNNID, }, // atob
	{ 0x8AF6B8F8, UNKNOWNNID, }, // SysclibForKernel_8AF6B8F8
//	{ 0xD3D1A3B9, UNKNOWNNID, }, // strncat  MISSING
	{ 0xD887CACD, UNKNOWNNID, }, // get_ctype_table
//	{ 0x909C228B, UNKNOWNNID, }, // setjmp  MISSING
//	{ 0x18FE80DB, UNKNOWNNID, }, // longjmp  MISSING
};

static nid_entry ThreadManForKernel_nid[] = {
	{ 0xDD55A192, 0x37BD0C9C, }, // sceKernelGetSyscallRA
};

static nid_entry InitForKernel_nid[] = {
	{ 0x33D30F49, UNKNOWNNID, }, // InitForKernel_33D30F49
	{ 0x9F9AE99C, UNKNOWNNID, }, // sceKernelSetInitCallback
	{ 0xC4F1BA33, UNKNOWNNID, }, // InitForKernel_C4F1BA33
	{ 0xFD0F25AD, UNKNOWNNID, }, // InitForKernel_FD0F25AD
};

static nid_entry SysTimerForKernel_nid[] = {
	{ 0xC99073E3, 0x71059CBF, }, // sceSTimerAlloc
	{ 0xC105CF38, 0xF03AE143, }, // sceSTimerFree
	{ 0xB53534B4, 0x4467BD60, }, // SysTimerForKernel_B53534B4
	{ 0x975D8E84, 0x847D785B, }, // sceSTimerSetHandler
	{ 0xA95143E2, 0x8FB264FB, }, // sceSTimerStartCount
	{ 0x4A01F9D3, 0xD01C6E08, }, // sceSTimerStopCount
	{ 0x54BB5DB4, 0x964F73FD, }, // sceSTimerResetCount
	{ 0x228EDAE4, 0x94BA6594, }, // sceSTimerGetCount
	{ 0x53231A15, 0xCE5A60D8, }, // SysTimerForKernel_53231A15
};

static nid_entry sceImpose_driver_nid[] = {
	{ 0x0F341BE4, 0xE8004C90, }, // sceImposeGetHomePopup
	{ 0x116CFF64, 0x4459932F, }, // sceImposeCheckVideoOut
	{ 0x116DDED6, 0x027F21C6, }, // sceImposeSetVideoOutMode
	{ 0x1AEED8FE, 0x375A2913, }, // sceImposeSuspend
	{ 0x1B6E3400, 0x6886A3B9, }, // sceImposeGetStatus
	{ 0x24FD7BCF, 0xD33D92AF, }, // sceImposeGetLanguageMode
	{ 0x36AA6E91, 0xBF34567B, }, // sceImposeSetLanguageMode
	{ 0x381BD9E7, 0x90B3DC7F, }, // sceImposeHomeButton
	{ 0x531C9778, 0x4C4DF719, }, // sceImposeGetParam
	{ 0x5595A71A, 0x5EF5BAF2, }, // sceImposeSetHomePopup
	{ 0x7084E72C, 0x6C41D4D6, }, // sceImpose_driver_7084E72C
	{ 0x72189C48, 0x6513E3B5, }, // sceImposeSetUMDPopup
	{ 0x810FB7FB, 0x72524BDB, }, // sceImposeSetParam
	{ 0x86924032, 0x6DDBA601, }, // sceImposeResume
	{ 0x8C943191, 0xEBC850AC, }, // sceImposeGetBatteryIconStatus
	{ 0x8F6E3518, 0xB658F454, }, // sceImposeGetBacklightOffTime
	{ 0x967F6D4A, 0x8C6BEAF3, }, // sceImposeSetBacklightOffTime
	{ 0x9C8C6C81, 0x2462EFE4, }, // sceImposeSetStatus
	{ 0x9DBCE0C4, 0x08362264, }, // sceImpose_driver_9DBCE0C4
	{ 0xB415FC59, 0x44F17A7A, }, // sceImposeChanges
	{ 0xBDBC42A6, 0xAF2CB30B, }, // sceImposeInit
	{ 0xC7E36CC7, 0x8AEDAD79, }, // sceImposeEnd
	{ 0xE0887BC8, 0x454CB346, }, // sceImposeGetUMDPopup

	{ 0xFF1A2F07, 0x418EF9D9, },
};

static nid_entry sceGe_driver_nid[] = {
	{ 0x71FCD1D6, 0x632DD78F, }, // sceGeInit
	{ 0x9F2C2948, 0xB85BCB96, }, // sceGeEnd
	{ 0x8F185DF7, 0xBDA9D2F5, }, // sceGeEdramInit
	{ 0x1F6752AD, 0x670F15ED, }, // sceGeEdramGetSize
	{ 0xE47E40E4, 0x40F72852, }, // sceGeEdramGetAddr
	{ 0xB77905EA, 0xD8E03A2B, }, // sceGeEdramSetAddrTranslation
	{ 0xB415364D, 0xE0E43511, }, // sceGeGetReg
	{ 0xDC93CFEF, 0xED34CF92, }, // sceGeGetCmd
	{ 0x57C8945B, 0x7FDFE7D4, }, // sceGeGetMtx
	{ 0x438A385A, 0x04B53579, }, // sceGeSaveContext
	{ 0x0BF608FB, 0xB4E22A53, }, // sceGeRestoreContext
	{ 0xAB49E76A, 0xD8A53104, }, // sceGeListEnQueue
	{ 0x1C0D95A6, 0xBB302D0B, }, // sceGeListEnQueueHead
	{ 0x5FB86AB0, 0x0BC8BCED, }, // sceGeListDeQueue
	{ 0xE0D68148, 0xEF1B48C6, }, // sceGeListUpdateStallAddr
	{ 0x03444EB4, 0xFEEC36F7, }, // sceGeListSync
	{ 0xB287BD61, 0xF490E8A0, }, // sceGeDrawSync
	{ 0xB448EC0D, 0x34A061D2, }, // sceGeBreak
	{ 0x4C06E472, 0xA462A747, }, // sceGeContinue
	{ 0xA4FC06A4, 0x1709686F, }, // sceGeSetCallback
	{ 0x05DB22CE, 0x2994C7F0, }, // sceGeUnsetCallback
	{ 0x9DA4A75F, 0x86551EBA, }, // sceGe_driver_9DA4A75F
	{ 0x114E1745, 0x6338386D, }, // sceGe_driver_114E1745
	{ 0x51C8BB60, 0xCD3DA558, }, // sceGeSetReg
	{ 0x51D44C58, 0x1DA54C86, }, // sceGeSetCmd
	{ 0x5A0103E6, 0x8083DAFC, }, // sceGeSetMtx
	{ 0xAEC21518, 0x39F15186, }, // sceGe_driver_AEC21518
	{ 0x7B481502, 0x4508BF79, }, // sceGe_driver_7B481502
	{ 0x9ACFF59D, 0x20EF3AC2, }, // sceGeGetListIdList
	{ 0x67B01D8E, 0x4C89C4A7, }, // sceGeGetList
	{ 0xE66CB92E, 0x97CA08A2, }, // sceGeGetStack
	{ 0x5BAA5439, 0x2444EC4D, }, // sceGeEdramSetSize

	{ 0xBAD6E1CA, 0xBF3A8761, },
	{ 0xC576E897, 0x35AF4E6C, },
};

static nid_entry sceRtc_driver_nid[] = {
	{ 0x029CA3B3, UNKNOWNNID, }, // sceRtc_029CA3B3
	{ 0xE7C27D1B, 0x0287B1C2, }, // sceRtcGetCurrentClockLocalTime
	{ 0x34885E0D, 0xFF530D9F, }, // sceRtcConvertUtcToLocalTime
	{ 0x779242A2, 0x9FAADB5A, }, // sceRtcConvertLocalTimeToUTC
	{ 0x42307A17, 0x2894B167, }, // sceRtcIsLeapYear
	{ 0x05EF322C, 0x47E66184, }, // sceRtcGetDaysInMonth
	{ 0x57726BC1, 0x1D887B2E, }, // sceRtcGetDayOfWeek
	{ 0x3A807CC8, 0x6D49A806, }, // sceRtcSetTime_t
	{ 0x27C4594C, 0xD2CCB1A6, }, // sceRtcGetTime_t
	{ 0xF006F264, 0xC34F73EB, }, // sceRtcSetDosTime
	{ 0x36075567, 0xDA941BC2, }, // sceRtcGetDosTime
	{ 0x7ACE4C04, 0xE8E3D9CB, }, // sceRtcSetWin32FileTime
	{ 0xCF561893, 0x9C7EC49F, }, // sceRtcGetWin32FileTime
	{ 0x9ED0AE87, 0x797B5418, }, // sceRtcCompareTick
	{ 0x26D25A5D, 0x5EF1D882, }, // sceRtcTickAddMicroseconds
	{ 0xE6605BCA, 0x4A2C0756, }, // sceRtcTickAddMinutes
	{ 0x26D7A24A, 0x994BA17E, }, // sceRtcTickAddHours
	{ 0xE51B4B7A, 0x405A9884, }, // sceRtcTickAddDays
	{ 0xCF3A2CA8, 0x0C47B23C, }, // sceRtcTickAddWeeks
	{ 0xDBF74F1B, 0x383685EA, }, // sceRtcTickAddMonths
	{ 0x42842C77, 0xE25D31B3, }, // sceRtcTickAddYears
	{ 0xC663B3B9, 0x2CF257D1, }, // sceRtcFormatRFC2822
	{ 0x7DE6711B, 0x23A51249, }, // sceRtcFormatRFC2822LocalTime
	{ 0x0498FB3C, 0x8FFE941C, }, // sceRtcFormatRFC3339
	{ 0x27F98543, 0x1F0BED41, }, // sceRtcFormatRFC3339LocalTime
	{ 0xDFBC5F16, 0x2A7A5D67, }, // sceRtcParseDateTime
	{ 0x28E1E988, 0x87EA3D9F, }, // sceRtcParseRFC3339
	{ 0x011F03C1, 0xC6E489D5, }, // sceRtcGetAccumulativeTime
	{ 0x17C26C00, UNKNOWNNID, }, // sceRtc_driver_17C26C00
	{ 0x1909C99B, 0xDACE3710, }, // sceRtcSetTime64_t
	{ 0x203CEB0D, 0xA01B48AC, }, // sceRtcGetLastReincarnatedTime
	{ 0x62685E98, 0x65725896, }, // sceRtcGetLastAdjustedTime
	{ 0x6A676D2D, 0xAB8F477C, }, // sceRtcUnregisterCallback
	{ 0x759937C5, 0xF3B8D574, }, // sceRtcSetConf
	{ 0x7D1FBED3, 0x54B9C589, }, // sceRtcSetAlarmTick
	{ 0x81FCDA34, 0xBEBE31B8, }, // sceRtcIsAlarmed
	{ 0xC2DDBEB5, 0x642544CD, }, // sceRtcGetAlarmTick
	{ 0xC499AF2F, 0x0686868F, }, // sceRtcReset
	{ 0xE1C93E47, 0x0C8E0D5C, }, // sceRtcGetTime64_t
	{ 0xF0B5571C, 0xD0AEDBD2, }, // sceRtcSynchronize
	{ 0xFB3B18CD, 0x12DDA3D7, }, // sceRtcRegisterCallback
	{ 0x9763C138, 0x6E69ED54, }, // sceRtcSetCurrentTick
	{ 0xF2A4AFE5, 0xC2C55D5C, }, // sceRtcTickAddSeconds

	{ 0xD24DF719, 0xD6335482, },
};

static nid_entry sceMpegbase_driver_nid[] = {
	{ 0x304882E1, UNKNOWNNID, }, // sceMpegbase_driver_304882E1
};

static nid_entry sceSyscon_driver_nid[] = {
	{ 0x58531E69, 0x76D41EEF, }, // sceSysconSetGSensorCallback
	{ 0xAD555CE5, 0x6552B9B1, }, // sceSysconSetLowBatteryCallback
	{ 0xF281805D, 0x77C8C60A, }, // sceSysconSetPowerSwitchCallback
	{ 0xA068B3D0, 0x1E9F1794, }, // sceSysconSetAlarmCallback
	{ 0xE540E532, 0x6AB36FE6, }, // sceSysconSetAcSupplyCallback
	{ 0xBBFB70C0, 0xCFFF5E1A, }, // sceSysconSetHPConnectCallback
	{ 0x805180D1, 0x8F53A3F8, }, // sceSysconSetHRPowerCallback
	{ 0xA8E34316, 0x3AE06081, }, // sceSysconSetHRWakeupCallback
	{ 0x53072985, 0xF1D75F96, }, // sceSysconSetWlanSwitchCallback
	{ 0xF9193EC3, 0x3FD3C02D, }, // sceSysconSetWlanPowerCallback
	{ 0x88FDB279, 0xBB0CB029, }, // sceSysconSetBtSwitchCallback
	{ 0xBAAECDF8, 0xB1F1AA4F, }, // sceSysconSetBtPowerCallback
	{ 0x7479DB05, 0xC15ECA50, }, // sceSysconSetHoldSwitchCallback
	{ 0x6848D817, 0xCF14E5F9, }, // sceSysconSetUmdSwitchCallback
	{ 0xFC32141A, 0x035EA6E4, }, // sceSysconGetPowerSupplyStatus
	{ 0xF775BC34, 0x8278F12B, }, // sceSysconGetWakeUpFactor
	{ 0xA9AEF39F, 0xE5E76E20, }, // sceSysconGetWakeUpReq
	{ 0x25F47F96, 0x085A0515, }, // sceSysconGetVideoCable
	{ 0xC4D66C1D, 0x447ACF0C, }, // sceSysconReadClock
	{ 0xC7634A7A, 0x143A8E3B, }, // sceSysconWriteClock
	{ 0x7A805EE4, 0x5E866CFD, }, // sceSysconReadAlarm
	{ 0x6C911742, 0x5E4EED46, }, // sceSysconWriteAlarm
	{ 0xE9FF8226, 0xC7661272, }, // sceSysconSetUSBStatus
	{ 0x882F0AAB, 0xB103FA82, }, // sceSysconGetTachyonWDTStatus
	{ 0x00E7B6C2, 0x4F4191D2, }, // sceSysconCtrlAnalogXYPolling
	{ 0x44439604, 0xD0BD786F, }, // sceSysconCtrlHRPower
	{ 0x91E183CB, 0x21A2F163, }, // sceSysconPowerSuspend
	{ 0xBE27FE66, 0x60EA8627, }, // sceSysconCtrlPower
	{ 0x1A08CD66, 0x162B2559, }, // sceSysconCtrlDvePower
	{ 0x19D5A97A, 0x7EEB4188, }, // sceSysconCtrlCharge
	{ 0xE5E35721, 0xC4931159, }, // sceSysconCtrlTachyonAvcPower
	{ 0x01677F91, 0x80ADDE16, }, // sceSysconCtrlVoltage
	{ 0x09721F7F, 0x9805EFA4, }, // sceSysconGetPowerStatus
	{ 0x99BBB24C, 0x62EAAA17, }, // sceSysconCtrlMsPower
	{ 0xF0ED3255, 0x741110C5, }, // sceSysconCtrlWlanPower
	{ 0xA975F224, 0x87FF42E5, }, // sceSysconCtrlHddPower
	{ 0x9C4266FC, 0x1516FEBC, }, // sceSysconCtrlBtPower
	{ 0x3987FEA3, 0x8BED6A5F, }, // sceSysconCtrlUsbPower
	{ 0xEAB13FBE, 0x97745810, }, // sceSysconPermitChargeBattery
	{ 0xC5075828, 0x95B64473, }, // sceSysconForbidChargeBattery
	{ 0x3C6DB1C5, 0xD501125F, }, // sceSysconCtrlTachyonVmePower
	{ 0xB2558E37, 0x17B0AC09, }, // sceSysconCtrlTachyonAwPower
	{ 0x9478F399, 0x62686235, }, // sceSysconCtrlLcdPower
	{ 0x6A53F3F8, 0xCC0AACB9, }, // sceSysconBatteryGetStatusCap
	{ 0xFA4C4518, 0xD62BB5B7, }, // sceSysconBatteryGetInfo
	{ 0xE448FD3F, 0x26D8C007, }, // sceSysconBatteryNop
	{ 0x70C10E61, 0x89468062, }, // sceSysconBatteryGetTemp
	{ 0x8BDEBB1E, 0xD84B0A9A, }, // sceSysconBatteryGetVolt
	{ 0x373EC933, 0xB72CEA5A, }, // sceSysconBatteryGetElec
	{ 0x82861DE2, 0xB039548D, }, // sceSysconBatteryGetRCap
	{ 0x876CA580, 0x53181AA1, }, // sceSysconBatteryGetCap
	{ 0x71135D7D, 0x0FDDB500, }, // sceSysconBatteryGetFullCap
	{ 0x7CBD4522, 0xEA23A8CB, }, // sceSysconBatteryGetIFC
	{ 0x284FE366, 0xD1241F0E, }, // sceSysconBatteryGetLimitTime
	{ 0x75025299, 0x05FA7FA3, }, // sceSysconBatteryGetStatus
	{ 0xB5105D51, 0x3182D1D3, }, // sceSysconBatteryGetCycle
	{ 0xD5340103, 0xE3D4ACBA, }, // sceSysconBatteryGetSerial
	{ 0xB71B98A8, 0xDAFF12A6, }, // sceSysconBatteryGetTempAD
	{ 0x87671B18, 0x7BD57A97, }, // sceSysconBatteryGetVoltAD
	{ 0x75D22BF8, 0xF746CC17, }, // sceSysconBatteryGetElecAD
	{ 0x4C539345, 0x12AF1EAF, }, // sceSysconBatteryGetTotalElec
	{ 0x4C0EE2FA, 0xC203B3D1, }, // sceSysconBatteryGetChargeTime
	{ 0x08DA3752, 0x22E6CDAB, }, // sceSysconCtrlTachyonVoltage
	{ 0x34C36FF9, 0x691779D8, }, // sceSysconGetDigitalKey
	{ 0x565EF519, 0xB94B011A, }, // sceSysconGetFallingDetectTime
	{ 0x2B9E6A06, 0xB9F72312, }, // sceSysconGetPowerError
	{ 0x5FF1D610, 0x78E0EEAC, }, // sceSysconSetPollingMode
	{ 0x3357EE5C, 0x05DDCF97, }, // sceSysconIsFalling
	{ 0x32CFD20F, 0x064CBEFA, }, // sceSysconIsLowBattery
	{ 0xEC0DE439, 0x43CAE8DB, }, // sceSysconGetPowerSwitch
	{ 0xEA5B9823, 0xA2B95839, }, // sceSysconIsAlarmed
	{ 0xE0DDFE18, 0x14887DD6, }, // sceSysconGetHPConnect
	{ 0xBDA16E46, 0xF3186284, }, // sceSysconGetWlanSwitch
	{ 0xA6776FB4, 0x6FCB027D, }, // sceSysconGetBtSwitch
	{ 0xF6BB4649, 0xB2C3A628, }, // sceSysconGetHoldSwitch
	{ 0x71AE1BCE, 0xD3BAD761, }, // sceSysconGetHRPowerStatus
	{ 0xF953EF93, 0xC38B6F9B, }, // sceSysconGetHRWakeupStatus
	{ 0x7016161C, 0xE42768B0, }, // sceSysconGetWlanPowerStatus
	{ 0x56A73EDD, 0xEC6915C2, }, // sceSysconGetBtPowerStatus
	{ 0x48AB0E44, 0xCCE484C6, }, // sceSysconGetLeptonPowerCtrl
	{ 0x628F2351, 0x3A67BD9B, }, // sceSysconGetMsPowerCtrl
	{ 0xEC37C549, 0xB0151DFD, }, // sceSysconGetWlanPowerCtrl
	{ 0x051186F3, 0xDD182ADD, }, // sceSysconGetHddPowerCtrl
	{ 0xCE76F93E, 0xC05FEEA3, }, // sceSysconGetDvePowerCtrl
	{ 0x8EDF1AD7, 0x6E2C46EC, }, // sceSysconGetBtPowerCtrl
	{ 0xEE81C5B1, 0x61EB0F0D, }, // sceSysconGetUsbPowerCtrl
	{ 0x8DDA4CA6, 0xB4F2E7AB, }, // sceSysconGetTachyonVmePowerCtrl
	{ 0x52B74B6C, 0x4792EFF0, }, // sceSysconGetTachyonAwPowerCtrl
	{ 0x1B17D3E3, 0x725DEB89, }, // sceSysconGetTachyonAvcPowerCtrl
	{ 0x5F19C00F, 0xC783AEFA, }, // sceSysconGetLcdPowerCtrl
	{ 0x3C739F57, 0xC146C4CF, }, // sceSysconGetHRPowerCtrl
	{ 0x72EDA9AF, 0xA33EA207, }, // sceSysconGetGSensorVersion
	{ 0xC68F1573, 0x315CABD2, }, // sceSysconCtrlGSensor
	{ 0x7C8A5503, 0xDCA40C65, }, // sceSysconGetScvCode
//	{ 0x2EE82492, 0x2EE82492, }, // sceSysconCtrlTachyonWDT

	{ 0x64FA0B22, 0x716FEABC, },
	{ 0x4AB44BFC, 0x9C99FD01, },
	{ 0xD653E02E, 0xF24CC87A, },
	{ 0x9E6E060F, 0xC325BF4B, },
	{ 0xE00BFC9E, 0x3029639F, },
	{ 0xC8D97773, 0x61A25BB7, },
	{ 0xE20D08FE, 0xB85D3405, },
	{ 0x14909C63, 0x43A7052A, },
	{ 0x05727A0B, 0x729C6178, },
	{ 0x2C03E53E, 0x986F0E02, },
	{ 0xBF458F40, 0xDBCEDFE9, },
};

static nid_entry sceUmd_nid[] = {
	{ 0xAE53DC2D, 0x507AB2B8, }, // sceUmdClearDriveStatus
	{ 0x230666E3, 0x7E185F98, }, // sceUmdSetDriveStatus
	{ 0x319ED97C, 0xE42F8D12, }, // sceUmdUnRegisterActivateCallBack
	{ 0x086DDC0D, 0xE8909D7B, }, // sceUmdRegisterActivateCallBack
	{ 0x4832ABF3, 0x5BB22C42, }, // sceUmdRegisterReplaceCallBack
	{ 0x4BA25F4A, 0xF80E731B, }, // sceUmdUnRegisterReplaceCallBack
	{ 0xBD2BDE07, 0x6E350FE4, }, // sceUmdUnRegisterUMDCallBack
	{ 0xAEE7404D, 0x58708431, }, // sceUmdRegisterUMDCallBack
	{ 0x46EBB729, 0x8213F674, }, // sceUmdCheckMedium
	{ 0x6AF9B50A, 0x145DBD8C, }, // sceUmdCancelWaitDriveStat
	{ 0x87533940, 0xB1E889EB, }, // sceUmdReplaceProhibit
	{ 0xCBE9F02A, 0xE34F613F, }, // sceUmdReplacePermit
	{ 0x3D0DECD5, 0x8C486A4F, }, // sceUmdGetUserEventFlagId
	{ 0xD45D1FE6, 0x7E45DF26, }, // sceUmdGetDriveStatus
	{ 0xD01B2DC6, 0x80CA9429, }, // sceUmdGetAssignedFlag
	{ 0x3925CBD8, 0xD0F6CBDD, }, // sceUmdSetAssignedFlag
	{ 0x9B22AED7, 0x3F826E9B, }, // sceUmdSetErrorStatus
	{ 0x71F81482, 0x749B5538, }, // sceUmdGetErrorStatus
	{ 0x7850F057, 0x04349047, }, // sceUmdRegisterGetUMDInfoCallBack
	{ 0x27A764A1, 0x3C650E45, }, // sceUmdUnRegisterGetUMDInfoCallBack
	{ 0x2D81508D, 0xFD058E55, }, // sceUmdRegisterDeactivateCallBack
	{ 0x56202973, 0xF0E95430, }, // sceUmdWaitDriveStatWithTimer
	{ 0x4A9E5E29, 0xFAAA2DC6, }, // sceUmdWaitDriveStatCB
	{ 0x075F1E0B, 0x8799D350, }, // sceUmdSetDetectUMDCallBackId
	{ 0x340B7686, 0x47B447E5, }, // sceUmdGetDiscInfo
	{ 0xEB56097E, 0xF4DF9EDE, }, // sceUmdGetDetectUMDCallBackId
	{ 0x6A41ED25, 0x107064CC, }, // sceUmdGetSuspendResumeMode
	{ 0x4C952ACF, 0xC886430B, }, // sceUmdSetSuspendResumeMode
	{ 0xF2D0EDFB, 0x8213F674, }, // sceUmdCheckMedium

	{ 0x659587F7, 0x24E838BA, },
	{ 0x38503380, 0x1228C54A, },
	{ 0xF8352373, 0xAA66D818, },
	{ 0x5469DC37, 0xAC4709C0, },
};

static nid_entry memlmd_nid[] = {
	{ 0x323366CA, 0x3F2AC9C6, },
	{ 0x1570BAB4, 0xC071E9CA, },
	{ 0x7CF1CD3E, 0xE42AFE2E, },
};

static nid_entry sceDdr_driver_nid[] = {
	{ 0x286E1372, 0x6835D115, }, // sceDdrSetPowerDownCounter
	{ 0xB5694ECD, 0x52C31D02, }, // sceDdrExecSeqCmd
	{ 0x655A9AB0, 0xA786CD84, }, // sceDdrWriteMaxAllocate
	{ 0x0DC43DE4, 0xFDC569B7, }, // sceDdrGetPowerDownCounter
	{ 0xD7C6C313, 0x64CF91A8, }, // sceDdrSetup
	{ 0x91CD8F94, 0x8AB4AC05, }, // sceDdrResetDevice
	{ 0x4CE55E76, 0x2E025581, }, // sceDdrChangePllClock

	{ 0x6078F911, 0x1970E996, },
	{ 0x5B5831E5, 0x4082BC9E, },
	{ 0xF23B7983, 0x16AAFDCA, },
	{ 0x6955346C, 0xB358CAF5, },
	{ 0x888129E8, 0xE8D58A1A, },
	{ 0x7251F5AB, 0x6627B052, },
};

static nid_entry sceDmacplus_driver_nid[] = {
	{ 0x29B50A82, 0x8A0E1D73, }, // sceDmacplusLcdcSetBaseAddr
	{ 0x8AE579CD, 0xC9DFF0CB, }, // sceDmacplusLcdcSetFormat
	{ 0x241601AE, 0x843241D0, }, // sceDmacplusLcdcGetFormat
	{ 0xF8C7C180, 0x0AB8C245, }, // sceDmacplusLcdcEnable
	{ 0x576522BC, 0x7CE02CDC, }, // sceDmacplusLcdcDisable
	{ 0x3A98EE05, 0xF2761EEB, }, // sceDmacplusAvcSync
	{ 0x4B980588, 0x5B73607B, }, // sceDmacplusSc2MeNormal16
	{ 0x3438DA0B, 0x639DFB43, }, // sceDmacplusSc2MeLLI
	{ 0x58DE4914, 0x1272EA9B, }, // sceDmacplusSc2MeSync
	{ 0xBE693828, 0xEFF77700, }, // sceDmacplusMe2ScNormal16
	{ 0xAB49D2CB, 0xA049D27E, }, // sceDmacplusMe2ScSync
	{ 0x2D5940FF, 0x1D67496B, }, // sceDmacplusMe2ScLLI
	{ 0x28558DBF, 0x9C38787B, }, // sceDmacTryMemcpy
	{ 0xFD183BCF, 0x7DD37DF5, }, // sceDmacplusSc128LLI
	{ 0x58C380BB, 0xA01D320B, }, // sceDmacplusLcdcGetBaseAddr
	{ 0xC7D33466, 0x70DB6434, }, // sceDmacMemcpy

	{ 0x1D73DDA8, 0xAF5B0967, },
	{ 0x74323807, 0xF634154D, },
	{ 0xB2EFA681, 0xABF36494, },
};

static nid_entry sceGpio_driver_nid[] = {
	{ 0xCA8BE2EA, 0x8137599B, }, // sceGpioGetPortMode
	{ 0x37C8DADC, 0x7E4F5CAD, }, // sceGpioSetIntrMode
	{ 0xF856CE46, 0xF2A11746, }, // sceGpioGetIntrMode
	{ 0x95135905, 0x7BB3FD6B, }, // sceGpioPortInvert
	{ 0x785206CD, 0xDDAFD05C, }, // sceGpioEnableIntr
	{ 0x31F34AE6, 0xFCC28150, }, // sceGpioQueryIntr
	{ 0xBE77D1D0, 0x77A979FB, }, // sceGpioAcquireIntr
	{ 0xC6928224, 0x6446C968, }, // sceGpioGetCapturePort
	{ 0x6B38B826, 0x127C8C8D, }, // sceGpioSetCapturePort
	{ 0x5691CEFA, 0x50540953, }, // sceGpioEnableTimerCapture
	{ 0x2CDC8EDC, 0x3775BB6E, }, // sceGpioDisableTimerCapture
};

static nid_entry sceSysreg_driver_nid[] = {
	{ 0xC29D614E, 0xDD81E650, }, // sceSysregTopResetEnable
	{ 0xDC6139A4, 0x2B987503, }, // sceSysregScResetEnable
	{ 0xDE59DACB, 0xB70793D7, }, // sceSysregMeResetEnable
	{ 0x2DB0EB28, 0xE2606FCB, }, // sceSysregMeResetDisable
	{ 0x26283A6F, 0x2DDD6900, }, // sceSysregAwResetEnable
	{ 0xA374195E, 0x5C431485, }, // sceSysregAwResetDisable
	{ 0xD20581EA, 0x0C207E1B, }, // sceSysregVmeResetEnable
	{ 0x7558064A, 0x381A7DEF, }, // sceSysregVmeResetDisable
	{ 0x9BB70D34, 0x86FFE348, }, // sceSysregAvcResetEnable
	{ 0xFD6C562B, 0xC244D673, }, // sceSysregAvcResetDisable
	{ 0xCD4FB614, 0x589C265C, }, // sceSysregUsbResetEnable
	{ 0x69EECBE5, 0x39D70582, }, // sceSysregUsbResetDisable
	{ 0xF5B80837, 0x1DDD5251, }, // sceSysregAtaResetEnable
	{ 0x8075303F, 0xD01D14CA, }, // sceSysregAtaResetDisable
	{ 0x00C2628E, 0xA5ADA1B9, }, // sceSysregMsifResetEnable
	{ 0xEC4BF81F, 0xB457532C, }, // sceSysregMsifResetDisable
	{ 0x8A7F9EB4, 0x519942D9, }, // sceSysregKirkResetEnable
	{ 0xC32F2491, 0x0776E51D, }, // sceSysregKirkResetDisable
	{ 0xA23BC2C4, 0xFAC5A294, }, // sceSysregAtahddResetEnable
	{ 0xF5EA8570, 0x5084432C, }, // sceSysregAtahddResetDisable
	{ 0x3E961C02, 0xA898A421, }, // sceSysregUsbhostResetEnable
	{ 0xACFA3764, 0x5CB2DF0E, }, // sceSysregUsbhostResetDisable
	{ 0x44F6CDA7, 0xD74D9041, }, // sceSysregMeBusClockEnable
	{ 0x158AD4FC, 0x64FA9D42, }, // sceSysregMeBusClockDisable
	{ 0x4D4CE2B8, 0x78C77639, }, // sceSysregAwRegABusClockEnable
	{ 0x789BD609, 0x780DA912, }, // sceSysregAwRegABusClockDisable
	{ 0x391CE1C0, 0x096A438F, }, // sceSysregAwRegBBusClockEnable
	{ 0x82D35024, 0x460B35AD, }, // sceSysregAwRegBBusClockDisable
	{ 0xAF904657, 0x90DD91C7, }, // sceSysregAwEdramBusClockEnable
	{ 0x438AECE9, 0x296B0011, }, // sceSysregAwEdramBusClockDisable
	{ 0x4F5AFBBE, 0x8C722FED, }, // sceSysregKirkBusClockEnable
	{ 0x845DD1A6, 0xF2123675, }, // sceSysregKirkBusClockDisable
	{ 0x3E216017, 0x5A2F2873, }, // sceSysregUsbBusClockEnable
	{ 0xBFBABB63, 0xB1654EF2, }, // sceSysregUsbBusClockDisable
	{ 0x4716E71E, 0xAAF010B3, }, // sceSysregMsifBusClockEnable
	{ 0x826430C0, 0xE179B80C, }, // sceSysregMsifBusClockDisable
	{ 0x7CC6CBFD, 0xECFCCC18, }, // sceSysregEmcddrBusClockEnable
	{ 0xEE6B9411, 0x296DC1E1, }, // sceSysregEmcddrBusClockDisable
	{ 0x63B9EB65, 0x4B9663E0, }, // sceSysregApbBusClockEnable
	{ 0xE1AA9788, 0x64F213C0, }, // sceSysregApbBusClockDisable
	{ 0xAA63C8BD, 0x789597AB, }, // sceSysregAudioBusClockEnable
	{ 0x054AC8C6, 0x5B23A351, }, // sceSysregAudioBusClockDisable
	{ 0xE45BED6A, 0xA7050245, }, // sceSysregAtahddBusClockEnable
	{ 0x681B35C4, 0x1BF3B5F7, }, // sceSysregAtahddBusClockDisable
	{ 0xE321F41A, 0x16B660BF, }, // sceSysregUsbhostBusClockEnable
	{ 0x4D2FFC60, 0x1B5A6BF5, }, // sceSysregUsbhostBusClockDisable
	{ 0x6B01D71B, 0x818819C6, }, // sceSysregAtaClkEnable
	{ 0xFC186A83, 0x68F37D82, }, // sceSysregAtaClkDisable
	{ 0xB59DB832, 0x89CDC923, }, // sceSysregAtahddClkEnable
	{ 0x9155812C, 0x2A19E0D8, }, // sceSysregAtahddClkDisable
	{ 0xDA4FCA1D, 0x10735602, }, // sceSysregUsbhostClkEnable
	{ 0x228A73E1, 0xC022F8DD, }, // sceSysregUsbhostClkDisable
	{ 0x31154490, 0x9BBB19AE, }, // sceSysregMsifClkEnable
	{ 0x8E2D835D, 0x3CF9F018, }, // sceSysregMsifClkDisable
	{ 0xE8533DCA, 0x69CC777F, }, // sceSysregApbTimerClkEnable
	{ 0xF6D83AD0, 0x24FE340D, }, // sceSysregApbTimerClkDisable
	{ 0xA9CD1C1F, 0x8E9E76AE, }, // sceSysregAudioClkEnable
	{ 0x2F216F38, 0x88DB6B25, }, // sceSysregAudioClkDisable
	{ 0xE3AECFFA, 0xF79E0DF3, }, // sceSysregGpioClkEnable
	{ 0x3BBD0C0C, 0x495BBD59, }, // sceSysregGpioClkDisable
	{ 0xC1DA05D2, 0x035BD7AA, }, // sceSysregAudioClkoutClkEnable
	{ 0xDE170397, 0x23EC68E8, }, // sceSysregAudioClkoutClkDisable
	{ 0x7234EA80, 0x6C4D9F5A, }, // sceSysregUsbClkEnable
	{ 0x38EC3281, 0x81856987, }, // sceSysregUsbClkDisable
	{ 0xBC68D9B6, 0x5E118FA3, }, // sceSysregUsbIoEnable
	{ 0xA3C8E075, 0x16859359, }, // sceSysregUsbIoDisable
	{ 0x79338EA3, 0x5972F148, }, // sceSysregAtaIoEnable
	{ 0xCADB92AA, 0xFEC035A0, }, // sceSysregAtaIoDisable
	{ 0xD74F1D48, 0xBA3E983B, }, // sceSysregMsifIoEnable
	{ 0x18172C6A, 0x767D0E1A, }, // sceSysregMsifIoDisable
	{ 0xBB26CF1F, 0xD1999F94, }, // sceSysregAudioIoEnable
	{ 0x8E2FB536, 0x7CE56D54, }, // sceSysregAudioIoDisable
	{ 0xF844DDF3, 0x9984A972, }, // sceSysregAudioClkoutIoEnable
	{ 0x29A119A1, 0x3835F9D9, }, // sceSysregAudioClkoutIoDisable
	{ 0x8B95C17F, 0x021D76FC, }, // sceSysregAtahddIoEnable
	{ 0xCCF911C0, 0x4554E89E, }, // sceSysregAtahddIoDisable
	{ 0xB627582E, 0xE7748A19, }, // sceSysregGpioIoEnable
	{ 0x1E9C3607, 0x18036C7C, }, // sceSysregGpioIoDisable
	{ 0x633595F2, 0x39BAA610, }, // sceSysregUsbGetConnectStatus
	{ 0x32471457, 0xEAD4AA62, }, // sceSysregUsbQueryIntr
	{ 0x692F31FF, 0xFE2A72C1, }, // sceSysregUsbAcquireIntr
	{ 0xFFEB6E00, 0xC86847C6, }, // sceSysregUsbhostQueryIntr
	{ 0x87C2BA20, 0x55B2A1F1, }, // sceSysregUsbhostAcquireIntr
	{ 0xD43E98F6, 0x88A2F907, }, // sceSysregMsifGetConnectStatus
	{ 0xBF91FBDA, 0x94DE6FAA, }, // sceSysregMsifQueryConnectIntr
	{ 0x36A75390, 0xA7833991, }, // sceSysregMsifAcquireConnectIntr
	{ 0x61BFF85F, 0x0344F228, }, // sceSysregInterruptToOther
	{ 0x9FC87ED4, 0x45596E3F, }, // sceSysregSemaTryLock
	{ 0x8BE2D520, 0x0CFD5A8A, }, // sceSysregSemaUnlock
	{ 0xB21B6CBF, 0xA810DD49, }, // sceSysregPllGetFrequency
	{ 0xB4560C45, 0x5DB6670C, }, // sceSysregPllGetOutSelect
	{ 0xDCA57573, 0x6C8249A5, }, // sceSysregPllSetOutSelect
	{ 0x1969E840, 0xDAD55B3D, }, // sceSysregMsifClkSelect
	{ 0x1D382514, 0xED38F76C, }, // sceSysregMsifDelaySelect
	{ 0x03340297, 0x325B9873, }, // sceSysregApbTimerClkSelect
	{ 0x9100B4E5, 0x3A98CABB, }, // sceSysregAudioClkSelect
	{ 0x833E6FB1, 0x7D40269F, }, // sceSysregAtaClkSelect
	{ 0x8CE2F97A, 0x88BB9B70, }, // sceSysregAtahddClkSelect
	{ 0x0A83FC7B, 0x9E80B4E2, }, // sceSysregAudioClkoutClkSelect
	{ 0x3EA188AD, 0xDAEB80C9, }, // sceSysregRequestIntr
	{ 0xCD0F6715, 0xAA8C6DEE, }, // sceSysregDoTimerEvent
	{ 0xF3443B9A, 0x2D5A9EE1, }, // sceSysregSetAwEdramSize
	{ 0x53A6838B, 0xE86442A0, }, // sceSysregPllGetBaseFrequency
	{ 0xBB3623DF, 0x1FF17642, }, // sceSysregPllUpdateFrequency
	{ 0x25673620, 0x06E4C1A1, }, // sceSysregIntrInit
	{ 0x4EE8E2C8, 0x2ECAA478, }, // sceSysregIntrEnd
	{ 0x083F56E2, 0x0CBB0657, }, // sceSysregEnableIntr
	{ 0x7C5B543C, 0x8B0E4451, }, // sceSysregDisableIntr
	{ 0xA9997109, 0xB70793D7, }, // sceSysregMeResetEnable(371)
	{ 0x76220E94, 0xE2606FCB, }, // sceSysregMeResetDisable(371)
	{ 0x3199CF1C, 0xD74D9041, }, // sceSysregMeBusClockEnable(371)
	{ 0x07881A0B, 0x64FA9D42, }, // sceSysregMeBusClockDisable(371)

	{ 0x55B18B84, 0xA07C29A9, },
	{ 0x2112E686, 0x47E9B7BC, },
	{ 0x7B9E9A53, 0x3440E941, },
	{ 0x7BDF0556, 0x0AE7EB33, },
	{ 0x8D0FED1E, 0x2841F98A, },
	{ 0xA46E9CA8, 0xBF0A535F, },
	{ 0xB08A4E85, 0x4978EA7F, },
	{ 0xA41929DA, 0xE160C581, },
	{ 0xE88B77ED, 0xCD852877, },
	{ 0x312F9419, 0x40FD846E, },
	{ 0x844AF6BD, 0x7DA92F37, },
	{ 0xEE8760C6, 0xB31774D7, },
	{ 0x5664F8B5, 0xF22C6FBB, },
	{ 0x44704E1D, 0x7EC19A9A, },
	{ 0x584AD989, 0xE392C8AD, },
	{ 0x377F035F, 0xAF32F771, },
	{ 0xAB3185FD, 0x2AFB7135, },
	{ 0x0EA487FA, 0xE0B6CBFF, },
	{ 0x136E8F5A, 0x99C319F7, },
	{ 0xF4811E00, 0xC0D9A9E6, },
};

static nid_entry sceDve_driver_nid[] = {
	{ 0x0836B2B5, 0x1C746CD2, }, // sceDvePowerOn
	{ 0x17BC42E4, 0x6E0F0A5A, }, // sceDvePowerOff
	{ 0xADE8C471, 0x01A3B89B, }, // sceDveResetEnable
	{ 0xD986F31B, 0x7392121D, }, // sceDveResetDisable
	{ 0xB7827F16, 0x8ED0BC91, }, // sceDveUnk1
	{ 0x34E9274D, 0x2E4D6C23, }, // sceDveUnk2
	{ 0xE84E975E, 0xD3AF8097, }, // sceDveUnk3
	{ 0x0B85524C, 0x9DAE67BC, }, // sceDveUnk4
	{ 0xA265B504, 0x1C67572C, }, // sceDveUnk5
	{ 0xAE2A6570, 0xD7CA3CCE, }, // sceDveDisplayOn
	{ 0xD8863512, 0xFA89E42C, }, // sceDveUnk10
	{ 0xA2D53F99, 0xB51F7D86, }, // sceDveDisplayOff
	{ 0xF0CF5A67, 0x3FFA90A3, }, // sceDveUnk12
	{ 0xBF910381, 0x3AA978BB, }, // sceDveUnk11
	{ 0x36970008, 0x578C8A6C, }, // sceDveUnk7
	{ 0x59E1B113, 0xB22FBD95, }, // sceDveUnk8
	{ 0xC9362C91, 0xEDD3BF59, }, // sceDveGetDisplayMode
	{ 0x22D50DA7, 0xE2E84747, }, // sceDveInit
	{ 0x292F15DA, 0xB6609201, }, // sceDveUnk9

	{ 0xB2E4E437, 0x92830E89, },
	{ 0xDEB2F80C, 0xAA085C2A, },
	{ 0x93828323, 0x0DD3C1B4, },
};

static nid_entry sceHprm_driver_nid[] = {
	{ 0xC7154136, 0x2717E848, }, // sceHprmRegisterCallback
	{ 0xFD7DE6CD, 0x03145C9F, }, // sceHprmUnregitserCallback
	{ 0x8EC787E0, 0x2BE94442, }, // sceHprmUpdateCableType
	{ 0xBAD0828E, 0x73587D56, }, // sceHprmGetModel
	{ 0x4D1E622C, 0x47C9A7FF, }, // sceHprmReset
	{ 0x2BCEC83E, 0x0611587F, }, // sceHprmPeekLatch
	{ 0x40D2F9F0, 0xA3A87975, }, // sceHprmReadLatch
	{ 0x7B038374, 0x83B75776, }, // sceHprmGetInternalState
	{ 0xF04591FA, 0x4A1FDBBE, }, // sceHprmSetIdleCancelKey
	{ 0x971AE8FB, 0xA4396E9B, }, // sceHprmGetIdleCancelKey
	{ 0x71B5FB67, 0xB05CD4A2, }, // sceHprmGetHpDetect
	{ 0x7E69EDA4, 0x8F2559DE, }, // sceHprmIsHeadphoneExist
	{ 0x208DB1BD, 0x0682085A, }, // sceHprmIsRemoteExist
	{ 0x219C58F1, 0x9B916290, }, // sceHprmIsMicrophoneExist
	{ 0xC1777D84, 0xADE4B1F4, }, // sceHprmGetCableType
	{ 0xD22913DB, 0x4C1F35A1, }, // sceHprmSetConnectCallback

	{ 0x9C1D9ADB, 0x38FB84B5, },
	{ 0xB6B6786B, 0x183E2F8D, },
	{ 0x21E8AFD5, 0xB866E93F, },
	{ 0x6BC7BEE9, 0xC69CB776, },
	{ 0xC0FE0FAD, 0x748FC3C8, },
	{ 0x7B1A14B8, 0xA971659D, },
	{ 0x1528D408, 0xADE4B1F4, },
	{ 0x133DE8FA, 0xF0A5DC5E, },
};

static nid_entry sceMesgLed_driver_nid[] = {
	{ 0xDFF0F308, 0x11B29654, },
};

static nid_entry sceClockgen_driver_nid[] = {
	{ 0x4EB657D5, 0xA02BB1D3, }, // sceClockgenAudioClkSetFreq
};

static nid_entry sceCodec_driver_nid[] = {
	{ 0x20C61103, 0xD26B6CCE, }, // sceCodecSelectVolumeTable
};

static nid_entry scePaf_nid[] = {
	{ 0x2BE8DDBB, 0x4DCF9203, },
	{ 0xE8CCC611, 0xB70C91FD, },
	{ 0xCDDCFFB3, 0xAEF47B29, },
	{ 0x48BB05D5, 0x9E9FFBFB, },
	{ 0x22FB4177, 0x1F538758, },
	{ 0xBC8DC92B, 0xCDE9615E, },
	{ 0xE3D530AE, 0x1B952318, },
};

resolver_config nid_660_fix[] = {
	NID_ENTRY(SysMemForKernel),
	NID_ENTRY(KDebugForKernel),
	NID_ENTRY(LoadCoreForKernel),
	NID_ENTRY(LoadExecForKernel),
	NID_ENTRY(ModuleMgrForKernel),
	NID_ENTRY(ExceptionManagerForKernel),
	NID_ENTRY(IoFileMgrForKernel),
	NID_ENTRY(scePower_driver),
	NID_ENTRY(InterruptManagerForKernel),
	NID_ENTRY(sceReg_driver),
	NID_ENTRY(sceCtrl_driver),
	NID_ENTRY(sceDisplay_driver),
	NID_ENTRY(sceAudio_driver),
	NID_ENTRY(UtilsForKernel),
	NID_ENTRY(SysclibForKernel),
	NID_ENTRY(ThreadManForKernel),
	NID_ENTRY(InitForKernel),
	NID_ENTRY(SysTimerForKernel),
	NID_ENTRY(sceImpose_driver),
	NID_ENTRY(sceGe_driver),
	NID_ENTRY(sceRtc_driver),
	NID_ENTRY(sceMpegbase_driver),
	NID_ENTRY(memlmd),
	NID_ENTRY(sceSyscon_driver),
	NID_ENTRY(sceUmd),
	NID_ENTRY(sceDdr_driver),
	NID_ENTRY(sceDmacplus_driver),
	NID_ENTRY(sceGpio_driver),
	NID_ENTRY(sceSysreg_driver),
	NID_ENTRY(sceDve_driver),
	NID_ENTRY(sceHprm_driver),
	NID_ENTRY(sceMesgLed_driver),
	NID_ENTRY(sceClockgen_driver),
	NID_ENTRY(sceCodec_driver),
	NID_ENTRY(scePaf),
};

u32 nid_660_fix_size = NELEMS(nid_660_fix);
