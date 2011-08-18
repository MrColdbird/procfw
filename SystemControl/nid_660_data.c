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

static nid_entry sceDisplay_driver_nid[] = {
	{ 0x4AB7497F, 0xA38B3F89, }, // sceDisplaySetFrameBuf
	{ 0xB685BA36, 0xB0942511, }, // sceDisplayWaitVblankStart
	{ 0xE56B11BA, UNKNOWNNID, }, // sceDisplayGetFrameBuf
	{ 0x776ADFDB, 0x60112E07, }, // sceDisplaySetBacklightInternal
	{ 0x31C4BAA8, 0x0043973F, }, // sceDisplayGetBrightness
	{ 0x5B5AEFAD, UNKNOWNNID, }, // from 1.50
	{ 0xDBA6C4C4, UNKNOWNNID, }, // sceDisplayGetFramePerSec
	{ 0x432D133F, 0x117C3E2C, }, // sceDisplayEnable
	{ 0x681EE6A7, 0x33B620AF, }, // sceDisplayDisable
	{ 0x7ED59BC4, 0x3552AB11, }, // sceDisplaySetHoldMode
	{ 0xA544C486, 0x03F16FD4, }, // sceDisplaySetResumeMode
	{ 0x63E22A26, UNKNOWNNID, }, // sceDisplay_driver_63E22A26
	{ 0x289D82FE, UNKNOWNNID, }, // sceDisplaySetFrameBuf
	{ 0xEEDA2E54, UNKNOWNNID, }, // sceDisplayGetFrameBuf
	{ 0xB4F378FA, 0x99E358F1, }, // sceDisplayIsForeground
	{ 0xAC14F1FF, 0x8059136A, }, // sceDisplayGetForegroundLevel
	{ 0x9E3C6DC6, UNKNOWNNID, }, // sceDisplaySetBrightness
	{ 0x9C6EAAD7, UNKNOWNNID, }, // sceDisplayGetVcount
	{ 0x4D4E10EC, 0x572D7804, }, // sceDisplayIsVblank
	{ 0x69B53541, 0xE7397E7A, }, // sceDisplayGetVblankRest
	{ 0x36CDFADE, 0xFE5884EF, }, // sceDisplayWaitVblank
	{ 0x8EB9EC49, 0x58E8680E, }, // sceDisplayWaitVblankCB
	{ 0x984C27E7, 0xB0942511, }, // sceDisplayWaitVblankStart
	{ 0x46F186C3, 0xE38CA615, }, // sceDisplayWaitVblankStartCB
	{ 0x773DD3A3, 0xEC80D435, }, // sceDisplayGetCurrentHcount
	{ 0x210EAB3A, 0xF84D16CC, }, // sceDisplayGetAccumulatedHcount
	{ 0xBF79F646, 0x03F16FD4, }, // sceDisplayGetResumeMode
	{ 0xA83EF139, 0xD81A8FF9, }, // sceDisplayAdjustAccumulatedHcount
	{ 0x21038913, 0x71695015, }, // sceDisplayIsVsync
	{ 0x92C8F8B7, 0x34516323, }, // sceDisplayIsPseudoField
	{ 0xAE0E8972, UNKNOWNNID, }, // sceDisplaySetPseudoVsync

	{ 0x643F3841, UNKNOWNNID, },
	{ 0x820C6038, 0xF338AAE0, },
	{ 0x1CB8CB47, 0x0043973F, },
	{ 0x13AA96B7, 0x1EAA0BDC, },
	{ 0x66961521, 0x8475E040, },
	{ 0xF5EEEFEF, 0x996881D2, },
	{ 0xE8466BC2, 0x1FBE8856, },
	{ 0x946155FD, 0x117C3E2C, },
	{ 0x32B67781, 0x33B620AF, },
};

static nid_entry sceAudio_driver_nid[] = {
	{ 0xB282F4B2, UNKNOWNNID, }, // sceAudioGetChannelRestLength
	{ 0x669D93E4, UNKNOWNNID, }, // sceAudioSRCChReserve
	{ 0x138A70F1, UNKNOWNNID, }, // sceAudioSRCChRelease
	{ 0x43645E69, UNKNOWNNID, }, // sceAudioSRCOutputBlocking
	{ 0x80F1F7E0, UNKNOWNNID, }, // sceAudioInit
	{ 0x210567F7, UNKNOWNNID, }, // sceAudioEnd
	{ 0xA2BEAA6C, UNKNOWNNID, }, // sceAudioSetFrequency
	{ 0xB61595C0, UNKNOWNNID, }, // sceAudioLoopbackTest
	{ 0x927AC32B, UNKNOWNNID, }, // sceAudioSetVolumeOffset
	{ 0x8C1009B2, UNKNOWNNID, }, // sceAudioOutput
	{ 0x136CAF51, UNKNOWNNID, }, // sceAudioOutputBlocking
	{ 0xE2D56B2D, UNKNOWNNID, }, // sceAudioOutputPanned
	{ 0x13F592BC, UNKNOWNNID, }, // sceAudioOutputPannedBlocking
	{ 0x5EC81C55, UNKNOWNNID, }, // sceAudioChReserve
	{ 0x41EFADE7, UNKNOWNNID, }, // sceAudioOneshotOutput
	{ 0x6FC46853, UNKNOWNNID, }, // sceAudioChRelease
	{ 0xB011922F, UNKNOWNNID, }, // sceAudioGetChannelRestLength
	{ 0xCB2E439E, UNKNOWNNID, }, // sceAudioSetChannelDataLen
	{ 0x95FD0C2D, UNKNOWNNID, }, // sceAudioChangeChannelConfig
	{ 0xB7E1D8E7, UNKNOWNNID, }, // sceAudioChangeChannelVolume
	{ 0x38553111, UNKNOWNNID, }, // sceAudioSRCChReserve
	{ 0x5C37C0AE, UNKNOWNNID, }, // sceAudioSRCChRelease
	{ 0xE0727056, UNKNOWNNID, }, // sceAudioSRCOutputBlocking
	{ 0x086E5895, UNKNOWNNID, }, // sceAudioInputBlocking
	{ 0x6D4BEC68, UNKNOWNNID, }, // sceAudioInput
	{ 0xA708C6A6, UNKNOWNNID, }, // sceAudioGetInputLength
	{ 0x87B2E651, UNKNOWNNID, }, // sceAudioWaitInputEnd
	{ 0x7DE61688, UNKNOWNNID, }, // sceAudioInputInit
	{ 0xE926D3FB, UNKNOWNNID, }, // sceAudioInputInitEx
	{ 0xA633048E, UNKNOWNNID, }, // sceAudioPollInputEnd
	{ 0xE9D97901, UNKNOWNNID, }, // sceAudioGetChannelRestLen
};

static nid_entry sceReg_driver_nid[] = {
	{ 0x98279CF1, UNKNOWNNID, }, // sceRegInit
	{ 0x9B25EDF1, UNKNOWNNID, }, // sceRegExit
	{ 0x92E41280, UNKNOWNNID, }, // sceRegOpenRegistry
	{ 0xFA8A5739, UNKNOWNNID, }, // sceRegCloseRegistry
	{ 0xDEDA92BF, UNKNOWNNID, }, // sceRegRemoveRegistry
	{ 0x1D8A762E, UNKNOWNNID, }, // sceRegOpenCategory
	{ 0x0CAE832B, UNKNOWNNID, }, // sceRegCloseCategory
	{ 0x39461B4D, UNKNOWNNID, }, // sceRegFlushRegistry
	{ 0x0D69BF40, UNKNOWNNID, }, // sceRegFlushCategory
	{ 0x57641A81, UNKNOWNNID, }, // sceRegCreateKey
	{ 0x17768E14, UNKNOWNNID, }, // sceRegSetKeyValue
	{ 0xD4475AA8, UNKNOWNNID, }, // sceRegGetKeyInfo
	{ 0x28A8E98A, UNKNOWNNID, }, // sceRegGetKeyValue
	{ 0x2C0DB9DD, UNKNOWNNID, }, // sceRegGetKeysNum
	{ 0x2D211135, UNKNOWNNID, }, // sceRegGetKeys
	{ 0x4CA16893, UNKNOWNNID, }, // sceRegRemoveCategory
	{ 0x3615BC87, UNKNOWNNID, }, // sceRegRemoveKey
	{ 0xC5768D02, UNKNOWNNID, }, // sceRegGetKeyInfoByName
	{ 0x30BE0259, UNKNOWNNID, }, // sceRegGetKeyValueByName
	{ 0xBE8C1263, UNKNOWNNID, }, // sceReg_BE8C1263
	{ 0x835ECE6F, UNKNOWNNID, }, // sceReg_835ECE6F
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
	{ 0xDD55A192, UNKNOWNNID, }, // sceKernelGetSyscallRA
};

static nid_entry InitForKernel_nid[] = {
	{ 0x33D30F49, UNKNOWNNID, }, // InitForKernel_33D30F49
	{ 0x9F9AE99C, UNKNOWNNID, }, // sceKernelSetInitCallback
	{ 0xC4F1BA33, UNKNOWNNID, }, // InitForKernel_C4F1BA33
	{ 0xFD0F25AD, UNKNOWNNID, }, // InitForKernel_FD0F25AD
};

static nid_entry SysTimerForKernel_nid[] = {
	{ 0xC99073E3, UNKNOWNNID, }, // sceSTimerAlloc
	{ 0xC105CF38, UNKNOWNNID, }, // sceSTimerFree
	{ 0xB53534B4, UNKNOWNNID, }, // SysTimerForKernel_B53534B4
	{ 0x975D8E84, UNKNOWNNID, }, // sceSTimerSetHandler
	{ 0xA95143E2, UNKNOWNNID, }, // sceSTimerStartCount
	{ 0x4A01F9D3, UNKNOWNNID, }, // sceSTimerStopCount
	{ 0x54BB5DB4, UNKNOWNNID, }, // sceSTimerResetCount
	{ 0x228EDAE4, UNKNOWNNID, }, // sceSTimerGetCount
	{ 0x53231A15, UNKNOWNNID, }, // SysTimerForKernel_53231A15
};

static nid_entry sceImpose_driver_nid[] = {
	{ 0x0F341BE4, 0xE9A42056, }, // sceImposeGetHomePopup
	{ 0x116CFF64, 0xE9B0E008, }, // sceImposeCheckVideoOut
	{ 0x116DDED6, 0x28B97035, }, // sceImposeSetVideoOutMode
	{ 0x1AEED8FE, 0x686B040E, }, // sceImposeSuspend
	{ 0x1B6E3400, 0xB3EAE8F0, }, // sceImposeGetStatus
	{ 0x24FD7BCF, 0x842309DD, }, // sceImposeGetLanguageMode
	{ 0x36AA6E91, 0xB376EF7F, }, // sceImposeSetLanguageMode
	{ 0x381BD9E7, 0xEFB54A28, }, // sceImposeHomeButton
	{ 0x531C9778, 0xDC3BECFF, }, // sceImposeGetParam
	{ 0x5595A71A, 0xC08C41EF, }, // sceImposeSetHomePopup
	{ 0x7084E72C, 0x0D5F811D, }, // sceImpose_driver_7084E72C
	{ 0x72189C48, 0x9FF64142, }, // sceImposeSetUMDPopup
	{ 0x810FB7FB, 0x3C318569, }, // sceImposeSetParam
	{ 0x86924032, 0x9E921836, }, // sceImposeResume
	{ 0x8C943191, 0x5557F4E2, }, // sceImposeGetBatteryIconStatus
	{ 0x8F6E3518, 0xE1A76241, }, // sceImposeGetBacklightOffTime
	{ 0x967F6D4A, 0x37F3F1D2, }, // sceImposeSetBacklightOffTime
	{ 0x9C8C6C81, 0xBB12F974, }, // sceImposeSetStatus
	{ 0x9DBCE0C4, 0xBCF1D254, }, // sceImpose_driver_9DBCE0C4
	{ 0xB415FC59, 0x0F067E16, }, // sceImposeChanges
	{ 0xBDBC42A6, 0xD067235E, }, // sceImposeInit
	{ 0xC7E36CC7, 0xAABEE36A, }, // sceImposeEnd
	{ 0xE0887BC8, 0xEF4D1BC5, }, // sceImposeGetUMDPopup

	{ 0xFF1A2F07, 0xCE16DF6D, },
};

static nid_entry sceGe_driver_nid[] = {
	{ 0x71FCD1D6, UNKNOWNNID, }, // sceGeInit
	{ 0x9F2C2948, UNKNOWNNID, }, // sceGeEnd
	{ 0x8F185DF7, UNKNOWNNID, }, // sceGeEdramInit
	{ 0x1F6752AD, UNKNOWNNID, }, // sceGeEdramGetSize
	{ 0xE47E40E4, UNKNOWNNID, }, // sceGeEdramGetAddr
	{ 0xB77905EA, UNKNOWNNID, }, // sceGeEdramSetAddrTranslation
	{ 0xB415364D, UNKNOWNNID, }, // sceGeGetReg
	{ 0xDC93CFEF, UNKNOWNNID, }, // sceGeGetCmd
	{ 0x57C8945B, UNKNOWNNID, }, // sceGeGetMtx
	{ 0x438A385A, UNKNOWNNID, }, // sceGeSaveContext
	{ 0x0BF608FB, UNKNOWNNID, }, // sceGeRestoreContext
	{ 0xAB49E76A, UNKNOWNNID, }, // sceGeListEnQueue
	{ 0x1C0D95A6, UNKNOWNNID, }, // sceGeListEnQueueHead
	{ 0x5FB86AB0, UNKNOWNNID, }, // sceGeListDeQueue
	{ 0xE0D68148, UNKNOWNNID, }, // sceGeListUpdateStallAddr
	{ 0x03444EB4, UNKNOWNNID, }, // sceGeListSync
	{ 0xB287BD61, UNKNOWNNID, }, // sceGeDrawSync
	{ 0xB448EC0D, UNKNOWNNID, }, // sceGeBreak
	{ 0x4C06E472, UNKNOWNNID, }, // sceGeContinue
	{ 0xA4FC06A4, UNKNOWNNID, }, // sceGeSetCallback
	{ 0x05DB22CE, UNKNOWNNID, }, // sceGeUnsetCallback
	{ 0x9DA4A75F, UNKNOWNNID, }, // sceGe_driver_9DA4A75F
	{ 0x114E1745, UNKNOWNNID, }, // sceGe_driver_114E1745
	{ 0x51C8BB60, UNKNOWNNID, }, // sceGeSetReg
	{ 0x51D44C58, UNKNOWNNID, }, // sceGeSetCmd
	{ 0x5A0103E6, UNKNOWNNID, }, // sceGeSetMtx
	{ 0xAEC21518, UNKNOWNNID, }, // sceGe_driver_AEC21518
	{ 0x7B481502, UNKNOWNNID, }, // sceGe_driver_7B481502
	{ 0x9ACFF59D, UNKNOWNNID, }, // sceGeGetListIdList
	{ 0x67B01D8E, UNKNOWNNID, }, // sceGeGetList
	{ 0xE66CB92E, UNKNOWNNID, }, // sceGeGetStack
	{ 0x5BAA5439, UNKNOWNNID, }, // sceGeEdramSetSize

	{ 0xBAD6E1CA, UNKNOWNNID, },
	{ 0xC576E897, UNKNOWNNID, },
};

static nid_entry sceRtc_driver_nid[] = {
	{ 0x029CA3B3, UNKNOWNNID, }, // sceRtc_029CA3B3
	{ 0xE7C27D1B, UNKNOWNNID, }, // sceRtcGetCurrentClockLocalTime
	{ 0x34885E0D, UNKNOWNNID, }, // sceRtcConvertUtcToLocalTime
	{ 0x779242A2, UNKNOWNNID, }, // sceRtcConvertLocalTimeToUTC
	{ 0x42307A17, UNKNOWNNID, }, // sceRtcIsLeapYear
	{ 0x05EF322C, UNKNOWNNID, }, // sceRtcGetDaysInMonth
	{ 0x57726BC1, UNKNOWNNID, }, // sceRtcGetDayOfWeek
	{ 0x3A807CC8, UNKNOWNNID, }, // sceRtcSetTime_t
	{ 0x27C4594C, UNKNOWNNID, }, // sceRtcGetTime_t
	{ 0xF006F264, UNKNOWNNID, }, // sceRtcSetDosTime
	{ 0x36075567, UNKNOWNNID, }, // sceRtcGetDosTime
	{ 0x7ACE4C04, UNKNOWNNID, }, // sceRtcSetWin32FileTime
	{ 0xCF561893, UNKNOWNNID, }, // sceRtcGetWin32FileTime
	{ 0x9ED0AE87, UNKNOWNNID, }, // sceRtcCompareTick
	{ 0x26D25A5D, UNKNOWNNID, }, // sceRtcTickAddMicroseconds
	{ 0xE6605BCA, UNKNOWNNID, }, // sceRtcTickAddMinutes
	{ 0x26D7A24A, UNKNOWNNID, }, // sceRtcTickAddHours
	{ 0xE51B4B7A, UNKNOWNNID, }, // sceRtcTickAddDays
	{ 0xCF3A2CA8, UNKNOWNNID, }, // sceRtcTickAddWeeks
	{ 0xDBF74F1B, UNKNOWNNID, }, // sceRtcTickAddMonths
	{ 0x42842C77, UNKNOWNNID, }, // sceRtcTickAddYears
	{ 0xC663B3B9, UNKNOWNNID, }, // sceRtcFormatRFC2822
	{ 0x7DE6711B, UNKNOWNNID, }, // sceRtcFormatRFC2822LocalTime
	{ 0x0498FB3C, UNKNOWNNID, }, // sceRtcFormatRFC3339
	{ 0x27F98543, UNKNOWNNID, }, // sceRtcFormatRFC3339LocalTime
	{ 0xDFBC5F16, UNKNOWNNID, }, // sceRtcParseDateTime
	{ 0x28E1E988, UNKNOWNNID, }, // sceRtcParseRFC3339
	{ 0x011F03C1, UNKNOWNNID, }, // sceRtcGetAccumulativeTime
	{ 0x17C26C00, UNKNOWNNID, }, // sceRtc_driver_17C26C00
	{ 0x1909C99B, UNKNOWNNID, }, // sceRtcSetTime64_t
	{ 0x203CEB0D, UNKNOWNNID, }, // sceRtcGetLastReincarnatedTime
	{ 0x62685E98, UNKNOWNNID, }, // sceRtcGetLastAdjustedTime
	{ 0x6A676D2D, UNKNOWNNID, }, // sceRtcUnregisterCallback
	{ 0x759937C5, UNKNOWNNID, }, // sceRtcSetConf
	{ 0x7D1FBED3, UNKNOWNNID, }, // sceRtcSetAlarmTick
	{ 0x81FCDA34, UNKNOWNNID, }, // sceRtcIsAlarmed
	{ 0xC2DDBEB5, UNKNOWNNID, }, // sceRtcGetAlarmTick
	{ 0xC499AF2F, UNKNOWNNID, }, // sceRtcReset
	{ 0xE1C93E47, UNKNOWNNID, }, // sceRtcGetTime64_t
	{ 0xF0B5571C, UNKNOWNNID, }, // sceRtcSynchronize
	{ 0xFB3B18CD, UNKNOWNNID, }, // sceRtcRegisterCallback
	{ 0x9763C138, UNKNOWNNID, }, // sceRtcSetCurrentTick
	{ 0xF2A4AFE5, UNKNOWNNID, }, // sceRtcTickAddSeconds

	{ 0xD24DF719, UNKNOWNNID, },
};

static nid_entry sceMpegbase_driver_nid[] = {
	{ 0x304882E1, UNKNOWNNID, }, // sceMpegbase_driver_304882E1
};

static nid_entry sceSyscon_driver_nid[] = {
	{ 0x58531E69, 0xF8EC18BD, }, // sceSysconSetGSensorCallback
	{ 0xAD555CE5, 0x599EB8A0, }, // sceSysconSetLowBatteryCallback
	{ 0xF281805D, 0xD76A105E, }, // sceSysconSetPowerSwitchCallback
	{ 0xA068B3D0, 0x2B7A0D32, }, // sceSysconSetAlarmCallback
	{ 0xE540E532, 0x657DCEF7, }, // sceSysconSetAcSupplyCallback
	{ 0xBBFB70C0, 0x672B79E8, }, // sceSysconSetHPConnectCallback
	{ 0x805180D1, 0x21AC8621, }, // sceSysconSetHRPowerCallback
	{ 0xA8E34316, 0x39456DE1, }, // sceSysconSetHRWakeupCallback
	{ 0x53072985, 0x50446BE5, }, // sceSysconSetWlanSwitchCallback
	{ 0xF9193EC3, 0xD2C053E7, }, // sceSysconSetWlanPowerCallback
	{ 0x88FDB279, 0xE19BC2DF, }, // sceSysconSetBtSwitchCallback
	{ 0xBAAECDF8, 0xB1F1AA4F, }, // sceSysconSetBtPowerCallback
	{ 0x7479DB05, 0x5C4C1130, }, // sceSysconSetHoldSwitchCallback
	{ 0x6848D817, 0xD6C2FD5F, }, // sceSysconSetUmdSwitchCallback
	{ 0xFC32141A, 0x22240B41, }, // sceSysconGetPowerSupplyStatus
	{ 0xF775BC34, 0xF33E1B14, }, // sceSysconGetWakeUpFactor
	{ 0xA9AEF39F, 0x9D88A8DE, }, // sceSysconGetWakeUpReq
	{ 0x25F47F96, 0x9FB6B045, }, // sceSysconGetVideoCable
	{ 0xC4D66C1D, 0xF436BB12, }, // sceSysconReadClock
	{ 0xC7634A7A, 0x3FB3FD08, }, // sceSysconWriteClock
	{ 0x7A805EE4, 0xF2AE6D5E, }, // sceSysconReadAlarm
	{ 0x6C911742, 0x80711575, }, // sceSysconWriteAlarm
	{ 0xE9FF8226, 0x2BE8EBC2, }, // sceSysconSetUSBStatus
	{ 0x882F0AAB, 0x38CBE06E, }, // sceSysconGetTachyonWDTStatus
	{ 0x00E7B6C2, 0x327A82F2, }, // sceSysconCtrlAnalogXYPolling
	{ 0x44439604, 0xBB7260C8, }, // sceSysconCtrlHRPower
	{ 0x91E183CB, 0x17E7753D, }, // sceSysconPowerSuspend
	{ 0xBE27FE66, 0xEDD3AB8B, }, // sceSysconCtrlPower
	{ 0x1A08CD66, 0x88EAAB07, }, // sceSysconCtrlDvePower
	{ 0x19D5A97A, 0x89A2024D, }, // sceSysconCtrlCharge
	{ 0xE5E35721, 0x9BC5E33B, }, // sceSysconCtrlTachyonAvcPower
	{ 0x01677F91, 0xF7BCD2A6, }, // sceSysconCtrlVoltage
	{ 0x09721F7F, 0x28363C97, }, // sceSysconGetPowerStatus
	{ 0x99BBB24C, 0x1088ABA8, }, // sceSysconCtrlMsPower
	{ 0xF0ED3255, 0x48448373, }, // sceSysconCtrlWlanPower
	{ 0xA975F224, 0xA0FA8CF7, }, // sceSysconCtrlHddPower
	{ 0x9C4266FC, 0x7E3A82AF, }, // sceSysconCtrlBtPower
	{ 0x3987FEA3, 0x47378317, }, // sceSysconCtrlUsbPower
	{ 0xEAB13FBE, 0x5EDEDED6, }, // sceSysconPermitChargeBattery
	{ 0xC5075828, 0x27F94EE6, }, // sceSysconForbidChargeBattery
	{ 0x3C6DB1C5, 0x1C5D4229, }, // sceSysconCtrlTachyonVmePower
	{ 0xB2558E37, 0x9F39BDC8, }, // sceSysconCtrlTachyonAwPower
	{ 0x9478F399, 0x457D8D7C, }, // sceSysconCtrlLcdPower
	{ 0x6A53F3F8, 0x85F5F601, }, // sceSysconBatteryGetStatusCap
	{ 0xFA4C4518, 0x7686E7A7, }, // sceSysconBatteryGetInfo
	{ 0xE448FD3F, 0x8F4AD2CA, }, // sceSysconBatteryNop
	{ 0x70C10E61, 0xCE8B6633, }, // sceSysconBatteryGetTemp
	{ 0x8BDEBB1E, 0xA7DB34BB, }, // sceSysconBatteryGetVolt
	{ 0x373EC933, 0x483088B0, }, // sceSysconBatteryGetElec
	{ 0x82861DE2, 0x8E54A128, }, // sceSysconBatteryGetRCap
	{ 0x876CA580, 0x3E16A759, }, // sceSysconBatteryGetCap
	{ 0x71135D7D, 0x4C871BEA, }, // sceSysconBatteryGetFullCap
	{ 0x7CBD4522, 0x8DD190A1, }, // sceSysconBatteryGetIFC
	{ 0x284FE366, 0x4D5A19BB, }, // sceSysconBatteryGetLimitTime
	{ 0x75025299, 0x3FA9F842, }, // sceSysconBatteryGetStatus
	{ 0xB5105D51, 0x68AF19F1, }, // sceSysconBatteryGetCycle
	{ 0xD5340103, 0x031CCDD7, }, // sceSysconBatteryGetSerial
	{ 0xB71B98A8, 0xD06FA2C6, }, // sceSysconBatteryGetTempAD
	{ 0x87671B18, 0x2B6BA4AB, }, // sceSysconBatteryGetVoltAD
	{ 0x75D22BF8, 0x011AC062, }, // sceSysconBatteryGetElecAD
	{ 0x4C539345, 0xF8F6E1F4, }, // sceSysconBatteryGetTotalElec
	{ 0x4C0EE2FA, 0xF87A1D11, }, // sceSysconBatteryGetChargeTime
	{ 0x08DA3752, 0x82D9F1BB, }, // sceSysconCtrlTachyonVoltage
	{ 0x34C36FF9, 0x3E4BD909, }, // sceSysconGetDigitalKey
	{ 0x565EF519, 0x0E5FA7EA, }, // sceSysconGetFallingDetectTime
	{ 0x2B9E6A06, 0xF0D1443F, }, // sceSysconGetPowerError
	{ 0x5FF1D610, 0x0E4FC766, }, // sceSysconSetPollingMode
	{ 0x3357EE5C, 0x05734D21, }, // sceSysconIsFalling
	{ 0x32CFD20F, 0x1605847F, }, // sceSysconIsLowBattery
	{ 0xEC0DE439, 0x47C04A04, }, // sceSysconGetPowerSwitch
	{ 0xEA5B9823, 0xC8DB7B74, }, // sceSysconIsAlarmed
	{ 0xE0DDFE18, 0x2D6F2728, }, // sceSysconGetHPConnect
	{ 0xBDA16E46, 0x2D510164, }, // sceSysconGetWlanSwitch
	{ 0xA6776FB4, 0x579A30EA, }, // sceSysconGetBtSwitch
	{ 0xF6BB4649, 0x01792223, }, // sceSysconGetHoldSwitch
	{ 0x71AE1BCE, 0xE58B9388, }, // sceSysconGetHRPowerStatus
	{ 0xF953EF93, 0xEF31EF4E, }, // sceSysconGetHRWakeupStatus
	{ 0x7016161C, 0x7216917F, }, // sceSysconGetWlanPowerStatus
	{ 0x56A73EDD, 0x67B66898, }, // sceSysconGetBtPowerStatus
	{ 0x48AB0E44, 0x577C5771, }, // sceSysconGetLeptonPowerCtrl
	{ 0x628F2351, 0x7672103B, }, // sceSysconGetMsPowerCtrl
	{ 0xEC37C549, 0xB5B06B81, }, // sceSysconGetWlanPowerCtrl
	{ 0x051186F3, 0x64257B5C, }, // sceSysconGetHddPowerCtrl
	{ 0xCE76F93E, 0x833017E5, }, // sceSysconGetDvePowerCtrl
	{ 0x8EDF1AD7, 0x3032943A, }, // sceSysconGetBtPowerCtrl
	{ 0xEE81C5B1, 0xEB11E9DE, }, // sceSysconGetUsbPowerCtrl
	{ 0x8DDA4CA6, 0x9E82A08C, }, // sceSysconGetTachyonVmePowerCtrl
	{ 0x52B74B6C, 0xDEB91FF2, }, // sceSysconGetTachyonAwPowerCtrl
	{ 0x1B17D3E3, 0x3932315D, }, // sceSysconGetTachyonAvcPowerCtrl
	{ 0x5F19C00F, 0x9497E906, }, // sceSysconGetLcdPowerCtrl
	{ 0x3C739F57, 0xD38A3708, }, // sceSysconGetHRPowerCtrl
	{ 0x72EDA9AF, 0xB5FA7580, }, // sceSysconGetGSensorVersion
	{ 0xC68F1573, 0xC56D0B5A, }, // sceSysconCtrlGSensor
	{ 0x7C8A5503, 0x93A3B23E, }, // sceSysconGetScvCode
//	{ 0x2EE82492, 0x2EE82492, }, // sceSysconCtrlTachyonWDT

	{ 0x64FA0B22, 0x28F7032E, },
	{ 0x4AB44BFC, 0xAA1B32D4, },
	{ 0xD653E02E, 0xE8C20DB5, },
	{ 0x9E6E060F, 0xB72DDFD2, },
	{ 0xE00BFC9E, 0xFD5C58CB, },
	{ 0xC8D97773, 0x862A93DE, },
	{ 0xE20D08FE, 0xA3406117, },
	{ 0x14909C63, 0x1083C71D, },
	{ 0x05727A0B, 0x294A7ED9, },
	{ 0x2C03E53E, 0xA2DAACB4, },
	{ 0xBF458F40, 0xD1216838, },
};

static nid_entry sceUmd_nid[] = {
	{ 0xAE53DC2D, UNKNOWNNID, }, // sceUmdClearDriveStatus
	{ 0x230666E3, UNKNOWNNID, }, // sceUmdSetDriveStatus
	{ 0x319ED97C, UNKNOWNNID, }, // sceUmdUnRegisterActivateCallBack
	{ 0x086DDC0D, UNKNOWNNID, }, // sceUmdRegisterActivateCallBack
	{ 0x4832ABF3, UNKNOWNNID, }, // sceUmdRegisterReplaceCallBack
	{ 0x4BA25F4A, UNKNOWNNID, }, // sceUmdUnRegisterReplaceCallBack
	{ 0xBD2BDE07, UNKNOWNNID, }, // sceUmdUnRegisterUMDCallBack
	{ 0xAEE7404D, UNKNOWNNID, }, // sceUmdRegisterUMDCallBack
	{ 0x46EBB729, UNKNOWNNID, }, // sceUmdCheckMedium
	{ 0x6AF9B50A, UNKNOWNNID, }, // sceUmdCancelWaitDriveStat
	{ 0x87533940, UNKNOWNNID, }, // sceUmdReplaceProhibit
	{ 0xCBE9F02A, UNKNOWNNID, }, // sceUmdReplacePermit
	{ 0x3D0DECD5, UNKNOWNNID, }, // sceUmdGetUserEventFlagId
	{ 0xD45D1FE6, UNKNOWNNID, }, // sceUmdGetDriveStatus
	{ 0xD01B2DC6, UNKNOWNNID, }, // sceUmdGetAssignedFlag
	{ 0x3925CBD8, UNKNOWNNID, }, // sceUmdSetAssignedFlag
	{ 0x9B22AED7, UNKNOWNNID, }, // sceUmdSetErrorStatus
	{ 0x71F81482, UNKNOWNNID, }, // sceUmdGetErrorStatus
	{ 0x7850F057, UNKNOWNNID, }, // sceUmdRegisterGetUMDInfoCallBack
	{ 0x27A764A1, UNKNOWNNID, }, // sceUmdUnRegisterGetUMDInfoCallBack
	{ 0x2D81508D, UNKNOWNNID, }, // sceUmdRegisterDeactivateCallBack
	{ 0x56202973, UNKNOWNNID, }, // sceUmdWaitDriveStatWithTimer
	{ 0x4A9E5E29, UNKNOWNNID, }, // sceUmdWaitDriveStatCB
	{ 0x075F1E0B, UNKNOWNNID, }, // sceUmdSetDetectUMDCallBackId
	{ 0x340B7686, UNKNOWNNID, }, // sceUmdGetDiscInfo
	{ 0xEB56097E, UNKNOWNNID, }, // sceUmdGetDetectUMDCallBackId
	{ 0x6A41ED25, UNKNOWNNID, }, // sceUmdGetSuspendResumeMode
	{ 0x4C952ACF, UNKNOWNNID, }, // sceUmdSetSuspendResumeMode
	{ 0xF2D0EDFB, UNKNOWNNID, }, // sceUmdCheckMedium

	{ 0x659587F7, UNKNOWNNID, },
	{ 0x38503380, UNKNOWNNID, },
	{ 0xF8352373, UNKNOWNNID, },
	{ 0x5469DC37, UNKNOWNNID, },
};

static nid_entry memlmd_nid[] = {
	{ 0x323366CA, UNKNOWNNID, },
	{ 0x1570BAB4, UNKNOWNNID, },
	{ 0x7CF1CD3E, UNKNOWNNID, },
};

static nid_entry sceDdr_driver_nid[] = {
	{ 0x286E1372, 0xE0A39D3E, }, // sceDdrSetPowerDownCounter
	{ 0xB5694ECD, 0x77CD1FB3, }, // sceDdrExecSeqCmd
	{ 0x655A9AB0, 0x4F30BFE8, }, // sceDdrWriteMaxAllocate
	{ 0x0DC43DE4, 0x00E36648, }, // sceDdrGetPowerDownCounter
	{ 0xD7C6C313, 0x9F882141, }, // sceDdrSetup
	{ 0x91CD8F94, 0x397756C0, }, // sceDdrResetDevice
	{ 0x4CE55E76, 0xCF07B9E2, }, // sceDdrChangePllClock

	{ 0x6078F911, 0x95B9A692, },
	{ 0x5B5831E5, 0x3D50DEC9, },
	{ 0xF23B7983, 0x2AA39A80, },
	{ 0x6955346C, 0xF1005384, },
	{ 0x888129E8, 0x0BAAE4C5, },
	{ 0x7251F5AB, 0x6DE74D80, },
};

static nid_entry sceDmacplus_driver_nid[] = {
	{ 0x29B50A82, UNKNOWNNID, }, // sceDmacplusLcdcSetBaseAddr
	{ 0x8AE579CD, UNKNOWNNID, }, // sceDmacplusLcdcSetFormat
	{ 0x241601AE, UNKNOWNNID, }, // sceDmacplusLcdcGetFormat
	{ 0xF8C7C180, UNKNOWNNID, }, // sceDmacplusLcdcEnable
	{ 0x576522BC, UNKNOWNNID, }, // sceDmacplusLcdcDisable
	{ 0x3A98EE05, UNKNOWNNID, }, // sceDmacplusAvcSync
	{ 0x4B980588, UNKNOWNNID, }, // sceDmacplusSc2MeNormal16
	{ 0x3438DA0B, UNKNOWNNID, }, // sceDmacplusSc2MeLLI
	{ 0x58DE4914, UNKNOWNNID, }, // sceDmacplusSc2MeSync
	{ 0xBE693828, UNKNOWNNID, }, // sceDmacplusMe2ScNormal16
	{ 0xAB49D2CB, UNKNOWNNID, }, // sceDmacplusMe2ScSync
	{ 0x2D5940FF, UNKNOWNNID, }, // sceDmacplusMe2ScLLI
	{ 0x28558DBF, UNKNOWNNID, }, // sceDmacTryMemcpy
	{ 0xFD183BCF, UNKNOWNNID, }, // sceDmacplusSc128LLI
	{ 0x58C380BB, UNKNOWNNID, }, // sceDmacplusLcdcGetBaseAddr
	{ 0xC7D33466, UNKNOWNNID, }, // sceDmacMemcpy

	{ 0x1D73DDA8, UNKNOWNNID, },
	{ 0x74323807, UNKNOWNNID, },
	{ 0xB2EFA681, UNKNOWNNID, },
};

static nid_entry sceGpio_driver_nid[] = {
	{ 0xCA8BE2EA, UNKNOWNNID, }, // sceGpioGetPortMode
	{ 0x37C8DADC, UNKNOWNNID, }, // sceGpioSetIntrMode
	{ 0xF856CE46, UNKNOWNNID, }, // sceGpioGetIntrMode
	{ 0x95135905, UNKNOWNNID, }, // sceGpioPortInvert
	{ 0x785206CD, UNKNOWNNID, }, // sceGpioEnableIntr
	{ 0x31F34AE6, UNKNOWNNID, }, // sceGpioQueryIntr
	{ 0xBE77D1D0, UNKNOWNNID, }, // sceGpioAcquireIntr
	{ 0xC6928224, UNKNOWNNID, }, // sceGpioGetCapturePort
	{ 0x6B38B826, UNKNOWNNID, }, // sceGpioSetCapturePort
	{ 0x5691CEFA, UNKNOWNNID, }, // sceGpioEnableTimerCapture
	{ 0x2CDC8EDC, UNKNOWNNID, }, // sceGpioDisableTimerCapture
};

static nid_entry sceSysreg_driver_nid[] = {
	{ 0xC29D614E, UNKNOWNNID, }, // sceSysregTopResetEnable
	{ 0xDC6139A4, UNKNOWNNID, }, // sceSysregScResetEnable
	{ 0xDE59DACB, UNKNOWNNID, }, // sceSysregMeResetEnable
	{ 0x2DB0EB28, UNKNOWNNID, }, // sceSysregMeResetDisable
	{ 0x26283A6F, UNKNOWNNID, }, // sceSysregAwResetEnable
	{ 0xA374195E, UNKNOWNNID, }, // sceSysregAwResetDisable
	{ 0xD20581EA, UNKNOWNNID, }, // sceSysregVmeResetEnable
	{ 0x7558064A, UNKNOWNNID, }, // sceSysregVmeResetDisable
	{ 0x9BB70D34, UNKNOWNNID, }, // sceSysregAvcResetEnable
	{ 0xFD6C562B, UNKNOWNNID, }, // sceSysregAvcResetDisable
	{ 0xCD4FB614, UNKNOWNNID, }, // sceSysregUsbResetEnable
	{ 0x69EECBE5, UNKNOWNNID, }, // sceSysregUsbResetDisable
	{ 0xF5B80837, UNKNOWNNID, }, // sceSysregAtaResetEnable
	{ 0x8075303F, UNKNOWNNID, }, // sceSysregAtaResetDisable
	{ 0x00C2628E, UNKNOWNNID, }, // sceSysregMsifResetEnable
	{ 0xEC4BF81F, UNKNOWNNID, }, // sceSysregMsifResetDisable
	{ 0x8A7F9EB4, UNKNOWNNID, }, // sceSysregKirkResetEnable
	{ 0xC32F2491, UNKNOWNNID, }, // sceSysregKirkResetDisable
	{ 0xA23BC2C4, UNKNOWNNID, }, // sceSysregAtahddResetEnable
	{ 0xF5EA8570, UNKNOWNNID, }, // sceSysregAtahddResetDisable
	{ 0x3E961C02, UNKNOWNNID, }, // sceSysregUsbhostResetEnable
	{ 0xACFA3764, UNKNOWNNID, }, // sceSysregUsbhostResetDisable
	{ 0x44F6CDA7, UNKNOWNNID, }, // sceSysregMeBusClockEnable
	{ 0x158AD4FC, UNKNOWNNID, }, // sceSysregMeBusClockDisable
	{ 0x4D4CE2B8, UNKNOWNNID, }, // sceSysregAwRegABusClockEnable
	{ 0x789BD609, UNKNOWNNID, }, // sceSysregAwRegABusClockDisable
	{ 0x391CE1C0, UNKNOWNNID, }, // sceSysregAwRegBBusClockEnable
	{ 0x82D35024, UNKNOWNNID, }, // sceSysregAwRegBBusClockDisable
	{ 0xAF904657, UNKNOWNNID, }, // sceSysregAwEdramBusClockEnable
	{ 0x438AECE9, UNKNOWNNID, }, // sceSysregAwEdramBusClockDisable
	{ 0x4F5AFBBE, UNKNOWNNID, }, // sceSysregKirkBusClockEnable
	{ 0x845DD1A6, UNKNOWNNID, }, // sceSysregKirkBusClockDisable
	{ 0x3E216017, UNKNOWNNID, }, // sceSysregUsbBusClockEnable
	{ 0xBFBABB63, UNKNOWNNID, }, // sceSysregUsbBusClockDisable
	{ 0x4716E71E, UNKNOWNNID, }, // sceSysregMsifBusClockEnable
	{ 0x826430C0, UNKNOWNNID, }, // sceSysregMsifBusClockDisable
	{ 0x7CC6CBFD, UNKNOWNNID, }, // sceSysregEmcddrBusClockEnable
	{ 0xEE6B9411, UNKNOWNNID, }, // sceSysregEmcddrBusClockDisable
	{ 0x63B9EB65, UNKNOWNNID, }, // sceSysregApbBusClockEnable
	{ 0xE1AA9788, UNKNOWNNID, }, // sceSysregApbBusClockDisable
	{ 0xAA63C8BD, UNKNOWNNID, }, // sceSysregAudioBusClockEnable
	{ 0x054AC8C6, UNKNOWNNID, }, // sceSysregAudioBusClockDisable
	{ 0xE45BED6A, UNKNOWNNID, }, // sceSysregAtahddBusClockEnable
	{ 0x681B35C4, UNKNOWNNID, }, // sceSysregAtahddBusClockDisable
	{ 0xE321F41A, UNKNOWNNID, }, // sceSysregUsbhostBusClockEnable
	{ 0x4D2FFC60, UNKNOWNNID, }, // sceSysregUsbhostBusClockDisable
	{ 0x6B01D71B, UNKNOWNNID, }, // sceSysregAtaClkEnable
	{ 0xFC186A83, UNKNOWNNID, }, // sceSysregAtaClkDisable
	{ 0xB59DB832, UNKNOWNNID, }, // sceSysregAtahddClkEnable
	{ 0x9155812C, UNKNOWNNID, }, // sceSysregAtahddClkDisable
	{ 0xDA4FCA1D, UNKNOWNNID, }, // sceSysregUsbhostClkEnable
	{ 0x228A73E1, UNKNOWNNID, }, // sceSysregUsbhostClkDisable
	{ 0x31154490, UNKNOWNNID, }, // sceSysregMsifClkEnable
	{ 0x8E2D835D, UNKNOWNNID, }, // sceSysregMsifClkDisable
	{ 0xE8533DCA, UNKNOWNNID, }, // sceSysregApbTimerClkEnable
	{ 0xF6D83AD0, UNKNOWNNID, }, // sceSysregApbTimerClkDisable
	{ 0xA9CD1C1F, UNKNOWNNID, }, // sceSysregAudioClkEnable
	{ 0x2F216F38, UNKNOWNNID, }, // sceSysregAudioClkDisable
	{ 0xE3AECFFA, UNKNOWNNID, }, // sceSysregGpioClkEnable
	{ 0x3BBD0C0C, UNKNOWNNID, }, // sceSysregGpioClkDisable
	{ 0xC1DA05D2, UNKNOWNNID, }, // sceSysregAudioClkoutClkEnable
	{ 0xDE170397, UNKNOWNNID, }, // sceSysregAudioClkoutClkDisable
	{ 0x7234EA80, UNKNOWNNID, }, // sceSysregUsbClkEnable
	{ 0x38EC3281, UNKNOWNNID, }, // sceSysregUsbClkDisable
	{ 0xBC68D9B6, UNKNOWNNID, }, // sceSysregUsbIoEnable
	{ 0xA3C8E075, UNKNOWNNID, }, // sceSysregUsbIoDisable
	{ 0x79338EA3, UNKNOWNNID, }, // sceSysregAtaIoEnable
	{ 0xCADB92AA, UNKNOWNNID, }, // sceSysregAtaIoDisable
	{ 0xD74F1D48, UNKNOWNNID, }, // sceSysregMsifIoEnable
	{ 0x18172C6A, UNKNOWNNID, }, // sceSysregMsifIoDisable
	{ 0xBB26CF1F, UNKNOWNNID, }, // sceSysregAudioIoEnable
	{ 0x8E2FB536, UNKNOWNNID, }, // sceSysregAudioIoDisable
	{ 0xF844DDF3, UNKNOWNNID, }, // sceSysregAudioClkoutIoEnable
	{ 0x29A119A1, UNKNOWNNID, }, // sceSysregAudioClkoutIoDisable
	{ 0x8B95C17F, UNKNOWNNID, }, // sceSysregAtahddIoEnable
	{ 0xCCF911C0, UNKNOWNNID, }, // sceSysregAtahddIoDisable
	{ 0xB627582E, UNKNOWNNID, }, // sceSysregGpioIoEnable
	{ 0x1E9C3607, UNKNOWNNID, }, // sceSysregGpioIoDisable
	{ 0x633595F2, UNKNOWNNID, }, // sceSysregUsbGetConnectStatus
	{ 0x32471457, UNKNOWNNID, }, // sceSysregUsbQueryIntr
	{ 0x692F31FF, UNKNOWNNID, }, // sceSysregUsbAcquireIntr
	{ 0xFFEB6E00, UNKNOWNNID, }, // sceSysregUsbhostQueryIntr
	{ 0x87C2BA20, UNKNOWNNID, }, // sceSysregUsbhostAcquireIntr
	{ 0xD43E98F6, UNKNOWNNID, }, // sceSysregMsifGetConnectStatus
	{ 0xBF91FBDA, UNKNOWNNID, }, // sceSysregMsifQueryConnectIntr
	{ 0x36A75390, UNKNOWNNID, }, // sceSysregMsifAcquireConnectIntr
	{ 0x61BFF85F, UNKNOWNNID, }, // sceSysregInterruptToOther
	{ 0x9FC87ED4, UNKNOWNNID, }, // sceSysregSemaTryLock
	{ 0x8BE2D520, UNKNOWNNID, }, // sceSysregSemaUnlock
	{ 0xB21B6CBF, UNKNOWNNID, }, // sceSysregPllGetFrequency
	{ 0xB4560C45, UNKNOWNNID, }, // sceSysregPllGetOutSelect
	{ 0xDCA57573, UNKNOWNNID, }, // sceSysregPllSetOutSelect
	{ 0x1969E840, UNKNOWNNID, }, // sceSysregMsifClkSelect
	{ 0x1D382514, UNKNOWNNID, }, // sceSysregMsifDelaySelect
	{ 0x03340297, UNKNOWNNID, }, // sceSysregApbTimerClkSelect
	{ 0x9100B4E5, UNKNOWNNID, }, // sceSysregAudioClkSelect
	{ 0x833E6FB1, UNKNOWNNID, }, // sceSysregAtaClkSelect
	{ 0x8CE2F97A, UNKNOWNNID, }, // sceSysregAtahddClkSelect
	{ 0x0A83FC7B, UNKNOWNNID, }, // sceSysregAudioClkoutClkSelect
	{ 0x3EA188AD, UNKNOWNNID, }, // sceSysregRequestIntr
	{ 0xCD0F6715, UNKNOWNNID, }, // sceSysregDoTimerEvent
	{ 0xF3443B9A, UNKNOWNNID, }, // sceSysregSetAwEdramSize
	{ 0x53A6838B, UNKNOWNNID, }, // sceSysregPllGetBaseFrequency
	{ 0xBB3623DF, UNKNOWNNID, }, // sceSysregPllUpdateFrequency
	{ 0x25673620, UNKNOWNNID, }, // sceSysregIntrInit
	{ 0x4EE8E2C8, UNKNOWNNID, }, // sceSysregIntrEnd
	{ 0x083F56E2, UNKNOWNNID, }, // sceSysregEnableIntr
	{ 0x7C5B543C, UNKNOWNNID, }, // sceSysregDisableIntr
	{ 0xA9997109, UNKNOWNNID, }, // sceSysregMeResetEnable(371)
	{ 0x76220E94, UNKNOWNNID, }, // sceSysregMeResetDisable(371)
	{ 0x3199CF1C, UNKNOWNNID, }, // sceSysregMeBusClockEnable(371)
	{ 0x07881A0B, UNKNOWNNID, }, // sceSysregMeBusClockDisable(371)

	{ 0x55B18B84, UNKNOWNNID, },
	{ 0x2112E686, UNKNOWNNID, },
	{ 0x7B9E9A53, UNKNOWNNID, },
	{ 0x7BDF0556, UNKNOWNNID, },
	{ 0x8D0FED1E, UNKNOWNNID, },
	{ 0xA46E9CA8, UNKNOWNNID, },
	{ 0xB08A4E85, UNKNOWNNID, },
	{ 0xA41929DA, UNKNOWNNID, },
	{ 0xE88B77ED, UNKNOWNNID, },
	{ 0x312F9419, UNKNOWNNID, },
	{ 0x844AF6BD, UNKNOWNNID, },
	{ 0xEE8760C6, UNKNOWNNID, },
	{ 0x5664F8B5, UNKNOWNNID, },
	{ 0x44704E1D, UNKNOWNNID, },
	{ 0x584AD989, UNKNOWNNID, },
	{ 0x377F035F, UNKNOWNNID, },
	{ 0xAB3185FD, UNKNOWNNID, },
	{ 0x0EA487FA, UNKNOWNNID, },
	{ 0x136E8F5A, UNKNOWNNID, },
	{ 0xF4811E00, UNKNOWNNID, },
};

static nid_entry sceDve_driver_nid[] = {
	{ 0x0836B2B5, UNKNOWNNID, }, // sceDvePowerOn
	{ 0x17BC42E4, UNKNOWNNID, }, // sceDvePowerOff
	{ 0xADE8C471, UNKNOWNNID, }, // sceDveResetEnable
	{ 0xD986F31B, UNKNOWNNID, }, // sceDveResetDisable
	{ 0xB7827F16, UNKNOWNNID, }, // sceDveUnk1
	{ 0x34E9274D, UNKNOWNNID, }, // sceDveUnk2
	{ 0xE84E975E, UNKNOWNNID, }, // sceDveUnk3
	{ 0x0B85524C, UNKNOWNNID, }, // sceDveUnk4
	{ 0xA265B504, UNKNOWNNID, }, // sceDveUnk5
	{ 0xAE2A6570, UNKNOWNNID, }, // sceDveDisplayOn
	{ 0xD8863512, UNKNOWNNID, }, // sceDveUnk10
	{ 0xA2D53F99, UNKNOWNNID, }, // sceDveDisplayOff
	{ 0xF0CF5A67, UNKNOWNNID, }, // sceDveUnk12
	{ 0xBF910381, UNKNOWNNID, }, // sceDveUnk11
	{ 0x36970008, UNKNOWNNID, }, // sceDveUnk7
	{ 0x59E1B113, UNKNOWNNID, }, // sceDveUnk8
	{ 0xC9362C91, UNKNOWNNID, }, // sceDveGetDisplayMode
	{ 0x22D50DA7, UNKNOWNNID, }, // sceDveInit
	{ 0x292F15DA, UNKNOWNNID, }, // sceDveUnk9

	{ 0xB2E4E437, UNKNOWNNID, },
	{ 0xDEB2F80C, UNKNOWNNID, },
	{ 0x93828323, UNKNOWNNID, },
};

static nid_entry sceHprm_driver_nid[] = {
	{ 0xC7154136, 0xDFC57C88, }, // sceHprmRegisterCallback
	{ 0xFD7DE6CD, 0xEB0CFCCC, }, // sceHprmUnregitserCallback
	{ 0x8EC787E0, 0xA0B1A19B, }, // sceHprmUpdateCableType
	{ 0xBAD0828E, 0x0B83352B, }, // sceHprmGetModel
	{ 0x4D1E622C, 0xC00DE13F, }, // sceHprmReset
	{ 0x2BCEC83E, 0x1F64B227, }, // sceHprmPeekLatch
	{ 0x40D2F9F0, 0xE9B776BE, }, // sceHprmReadLatch
	{ 0x7B038374, 0xB6895C1C, }, // sceHprmGetInternalState
	{ 0xF04591FA, 0x2F17A548, }, // sceHprmSetIdleCancelKey
	{ 0x971AE8FB, 0xD3F17366, }, // sceHprmGetIdleCancelKey
	{ 0x71B5FB67, 0x370DDC12, }, // sceHprmGetHpDetect
	{ 0x7E69EDA4, 0xFA4A25A7, }, // sceHprmIsHeadphoneExist
	{ 0x208DB1BD, 0xEFCFD0C5, }, // sceHprmIsRemoteExist
	{ 0x219C58F1, 0xAD158331, }, // sceHprmIsMicrophoneExist
	{ 0xC1777D84, 0xB2BEADB8, }, // sceHprmGetCableType
	{ 0xD22913DB, 0x8AEC8EE9, }, // sceHprmSetConnectCallback

	{ 0x9C1D9ADB, 0x301063CC, },
	{ 0xB6B6786B, 0x06C478D4, },
	{ 0x21E8AFD5, 0x602D5955, },
	{ 0x6BC7BEE9, 0x2903130A, },
	{ 0xC0FE0FAD, 0xDC895B2B, },
	{ 0x7B1A14B8, 0x8C728BB4, },
	{ 0x1528D408, 0xB2BEADB8, },
	{ 0x133DE8FA, 0x2E1A7B7C, },
};

static nid_entry sceMesgLed_driver_nid[] = {
	{ 0xDFF0F308, UNKNOWNNID, },
};

static nid_entry sceClockgen_driver_nid[] = {
	{ 0x4EB657D5, UNKNOWNNID, }, // sceClockgenAudioClkSetFreq
};

static nid_entry sceCodec_driver_nid[] = {
	{ 0x20C61103, UNKNOWNNID, }, // sceCodecSelectVolumeTable
};

static nid_entry scePaf_nid[] = {
	{ 0x2BE8DDBB, UNKNOWNNID, },
	{ 0xE8CCC611, UNKNOWNNID, },
	{ 0xCDDCFFB3, UNKNOWNNID, },
	{ 0x48BB05D5, UNKNOWNNID, },
	{ 0x22FB4177, UNKNOWNNID, },
	{ 0xBC8DC92B, UNKNOWNNID, },
	{ 0xE3D530AE, UNKNOWNNID, },
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
