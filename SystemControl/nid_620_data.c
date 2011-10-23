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
	{ 0xC7E57B9C, 0xB00D1EC2, }, // sceKernelSetQTGP3
	{ 0xCF4DE78C, 0xF5780DAA, }, // sceKernelGetUIDcontrolBlock
	{ 0xFC114573, 0xB9796F69, }, // sceKernelGetCompiledSdkVersion
	{ 0x3FC9AE6A, 0xFE9BC18B, }, // sceKernelDevkitVersion
	{ 0x536AD5E1, 0x6DD87F6B, }, // SysMemForKernel_536AD5E1
	{ 0x636C953B, 0x96BFE779, }, // sceKernelAllocHeapMemory
	{ 0xC9805775, 0x9BAA857E, }, // sceKernelDeleteHeap
	{ 0x7B749390, 0xC2A8C491, }, // sceKernelFreeHeapMemory
	{ 0x1C1FBFE7, 0xC6A782DA, }, // sceKernelCreateHeap
	{ 0x55A40B2C, 0xE057A674, }, // sceKernelQueryMemoryPartitionInfo
	{ 0xE6581468, 0xA25BF7A1, }, // sceKernelPartitionMaxFreeMemSize
	{ 0x9697CD32, 0x35588461, }, // sceKernelPartitionTotalFreeMemSize
	{ 0x237DBD4F, 0xE5E18A99, }, // sceKernelAllocPartitionMemory
	{ 0xB6D61D02, 0x093DE56A, }, // sceKernelFreePartitionMemory
	{ 0x9D9A5BA1, 0xFEB5C72B, }, // sceKernelGetBlockHeadAddr
	{ 0xB2C7AA36, 0x31DFE03F, }, // sceKernelSetDdrMemoryProtection
	{ 0x6373995D, 0x864EBFD7, }, // sceKernelGetModel
	{ 0xEB7A74DB, 0x191C6F98, }, // sceKernelAllocHeapMemoryWithOption
	{ 0xA823047E, 0xA60B6316, }, // sceKernelHeapTotalFreeSize
	{ 0xA2A65F0E, 0x22CD6BE7, }, // sceKernelFillFreeBlock
	{ 0xEE867074, 0x5FBC6777, }, // sceKernelSizeLockMemoryBlock
	{ 0xCE5544F4, 0x6467A511, }, // sceKernelResizeMemoryBlock
	{ 0x915EF4AC, 0x7BC702B2, }, // SysMemForKernel_915EF4AC
	{ 0x5EBE73DE, 0x138082C6, }, // sceKernelJointMemoryBlock
	{ 0x2A3E5280, 0x442F5709, }, // sceKernelQueryMemoryInfo
	{ 0x82CCA14F, 0x51674BC8, }, // sceKernelQueryMemoryBlockInfo
	{ 0x7B3E7441, 0xFE305C1C, }, // sceKernelMemoryExtendSize
	{ 0x6C1DCD41, 0x455CF575, }, // sceKernelCallUIDFunction
	{ 0x5367923C, 0xCBA3E53A, }, // sceKernelCallUIDObjFunction
	{ 0xCE05CCB7, 0x3699EF87, }, // SysMemForKernel_CE05CCB7
	{ 0x6CD838EC, 0xAB8A8028, }, // sceKernelLookupUIDFunction
	{ 0xAD09C397, 0x146C2BA2, }, // sceKernelCreateUIDtypeInherit
	{ 0xFEFC8666, 0x483237E4, }, // sceKernelCreateUIDtype
	{ 0xD1BAB054, 0x85AE7A69, }, // sceKernelDeleteUIDtype
	{ 0x1C221A08, 0x4DF843CC, }, // sceKernelGetUIDname
	{ 0x2E3402CC, 0x4AA435F6, }, // sceKernelRenameUID
	{ 0x39357F07, 0xFC3E85D6, }, // sceKernelGetUIDtype
	{ 0x89A74008, 0xB8D6DC38, }, // sceKernelCreateUID
	{ 0x8F20C4C0, 0x6C00F0CC, }, // sceKernelDeleteUID
	{ 0x55BFD686, 0xBDD1D428, }, // sceKernelSearchUIDbyName
	{ 0x41FFC7F9, 0xD17C6005, }, // sceKernelGetUIDcontrolBlockWithType
	{ 0x82D3CEE3, 0x9DABAF4E, }, // SysMemForKernel_82D3CEE3
	{ 0xFC207849, 0x491942DC, }, // SysMemForKernel_FC207849
	{ 0xDFAEBD5B, 0xD0DBFFE7, }, // sceKernelIsHold
	{ 0x7BE95FA0, 0xFEA10418, }, // sceKernelHoldUID
	{ 0xFE8DEBE0, 0xCCD58B8B, }, // sceKernelReleaseUID
	{ 0xA089ECA4, 0x74F3DC82, }, // sceKernelMemset
	{ 0x2F808748, 0x18D7F644, }, // sceKernelMemset32
	{ 0x1C4B1713, 0x08F96A1B, }, // sceKernelMemmove
	{ 0x1890BE9C, 0xF4C96DB4, }, // sceKernelSysMemMemSize
	{ 0x03072750, 0xAF8CBF62, }, // sceKernelSysMemMaxFreeMemSize
	{ 0x811BED79, 0x17BF724F, }, // sceKernelSysMemTotalFreeMemSize
	{ 0xF6C10E27, 0xA75DC8CE, }, // sceKernelGetSysMemoryInfo
	{ 0xCDA3A2F7, 0xA4AC3934, }, // SysMemForKernel_CDA3A2F7
	{ 0x960B888C, 0x55C21972, }, // SysMemForKernel_960B888C
	{ 0x452E3696, 0x957A966D, }, // sceKernelGetSystemStatus
	{ 0x95F5E8DA, 0x98ACDE01, }, // sceKernelSetSystemStatus
	{ 0x0D547E7F, 0xAB5E85E5, }, // sceKernelGetUMDData
	{ 0x3C4C5630, 0xAB5E85E5, }, // sceKernelGetUMDData (635 to 620)
	{ 0x419DB8F4, 0x02AEA33F, }, // sceKernelRegisterGetIdFunc
	{ 0x002BA296, 0x6FDEF86F, }, // sceKernelQueryHeapInfo
	{ 0x03808C51, 0x3988900F, }, // SysMemForKernel_03808C51
	{ 0xBB90D841, 0x20001B3C, }, // SysMemForKernel_BB90D841
	{ 0x0D1D8AC8, 0x93EAD67C, }, // sceKernelMemmoveWithFill
	{ 0x97B18FA8, 0xCA852144, }, // sceKernelCopyGameInfo
	{ 0xE40D3A1D, 0x1DFFBF56, }, // sceKernelSetUmdData
	{ 0xCE8D3DB3, 0x040CDA4C, }, // sceKernelGetQTGP2
	{ 0x55E4719C, 0xAAE557BF, }, // sceKernelSetQTGP2
	{ 0x6D8E0CDF, 0x2F756CC8, }, // sceKernelGetQTGP3
	{ 0xA262FEF0, 0x1205AE0C, }, // sceKernelGetAllowReplaceUmd
	{ 0xF91FE6AA, 0x4BFF26D9, }, // sceKernelSetParamSfo
	{ 0xDEC0A0D8, 0x031563B1, }, // sceKernelGetCompilerVersion
	{ 0x7ECBDBD9, 0xBD71F23A, }, // sceKernelGetDNAS
	{ 0x9C304ED7, 0xDCFF8AE5, }, // sceKernelSetDNAS
	{ 0xF4390489, 0xB0ED0CCD, }, // sceKernelRebootKernel
	{ 0xA1ACEA31, 0xC10C3C26, }, // sceKernelGetId
	{ 0xE0058030, 0xEA1C5190, }, // sceKernelMemoryShrinkSize
	{ 0x719792F1, 0x94AED0B5, }, // sceKernelMemoryOpenSize
	{ 0x101C3E59, 0x51963398, }, // sceKernelMemoryCloseSize
	{ 0xE0645895, 0x75A7EA2D, }, // sceKernelGetSysmemIdList
	{ 0xCBB05241, 0x7EF1DC3C, }, // sceKernelSetAllowReplaceUmd
	{ 0x2269BFA2, 0x2DC2E222, }, // SysMemForKernel_2269BFA2
	{ 0xCD617A94, 0xAB5E85E5, }, // sceKernelGetGameInfo
	{ 0xF77D77CB, 0x374DF838, }, // sceKernelSetCompilerVersion
	{ 0x38495D84, 0x2E0F38E5, }, // sceKernelGetInitialRandomValue
	{ 0x29A5899B, 0xC5485286, }, // sceKernelSetRebootKernel
	{ 0x071D9804, 0x13DC1D8E, }, // sceKernelApiEvaluationInit
	{ 0x02786087, 0x00E0050D, }, // sceKernelRegisterApiEvaluation
	{ 0x1F7F7F40, 0x00E0050D, }, // SysMemForKernel_1F7F7F40
	{ 0x049CC735, 0x1B03DBB9, }, // sceKernelApiEvaluationReport
	{ 0x39351245, 0x5AE912B5, }, // SysMemForKernel_39351245
	{ 0xD2E3A399, 0xEEB65C76, }, // SysMemForKernel_D2E3A399
	{ 0x4852F8DD, 0xEF66EEC6, }, // SysMemForKernel_4852F8DD
	{ 0x4EC43DC4, 0x906D863A, }, // SysMemForKernel_4EC43DC4
	{ 0x1F01A9E2, 0x3B560EE6, }, // SysMemForKernel_1F01A9E2
	{ 0xB9F8561C, 0xB0CC8D87, }, // SysMemForKernel_B9F8561C
	{ 0x9452B542, 0x361DC8C0, }, // SysMemForKernel_9452B542
	{ 0xEF2EE8C1, 0x2F15F149, }, // SysMemForKernel_EF2EE8C1
};

static nid_entry KDebugForKernel_nid[] = {
	{ 0xA2983280, 0x56B63535, }, // sceKernelCheckDebugHandler
	{ 0x2FF4E9F9, 0xEC5AE038, }, // sceKernelAssert
	{ 0x5282DD5E, 0x44599648, }, // sceKernelDipswSet
	{ 0xEE75658D, 0x9023AB67, }, // sceKernelDipswClear
	{ 0x428A8DA3, 0x98A23ABC, }, // sceKernelIsToolMode
	{ 0x72E6B3B8, 0x274F1548, }, // sceKernelIsUMDMode
	{ 0xA216AE06, 0xD9DB4F02, }, // sceKernelIsDVDMode
	{ 0x9B868276, 0xE50EB8AE, }, // sceKernelGetDebugPutchar
	{ 0xE7A3874D, 0x8F99B08A, }, // sceKernelRegisterAssertHandler
	{ 0xAFB8FC80, 0x959D4C09, }, // sceKernelDipswLow32
	{ 0x23906FB2, 0x2F753063, }, // sceKernelDipswHigh32
	{ 0xE146606D, 0x8F7CE42D, }, // sceKernelRegisterDebugPutchar
	{ 0x7CEB2C09, 0x681C1EEA, }, // sceKernelRegisterKprintfHandler
	{ 0x5CE9838B, 0x93701FDB, }, // sceKernelDebugWrite
	{ 0x66253C4E, 0xAA33F12F, }, // sceKernelRegisterDebugWrite
	{ 0xDBB5597F, 0x46750633, }, // sceKernelDebugRead
	{ 0xE6554FDA, 0xC741642A, }, // sceKernelRegisterDebugRead
	{ 0xB9C643C9, 0xEB0759EF, }, // sceKernelDebugEcho
	{ 0x7D1C74F0, 0x295865B1, }, // sceKernelDebugEchoSet
	{ 0x24C32559, 0x93F5D2A6, }, // sceKernelDipsw

	{ 0x333DCEC7, 0x67AAE03C, },
	{ 0x8B041DFB, 0xAED9BE14, },
	{ 0x9F8703E4, 0xC87C49CF, },
	{ 0xFFD2F2B9, 0xAFDEF5C1, },
	{ 0xA126F497, 0x62469B57, },
	{ 0xB7251823, 0x715A5348, },
};

static nid_entry LoadCoreForKernel_nid[] = {
	{ 0xCF8A41B1, 0xBEF0A05E, }, // sceKernelFindModuleByName
	{ 0xCCE4A157, 0x77E8F516, }, // sceKernelFindModuleByUID
	{ 0xDD303D79, 0x452C5524, }, // sceKernelFindModuleByAddress (5.XX NID)
	{ 0xFB8AE27D, 0x452C5524, }, // sceKernelFindModuleByAddress (3.xx NID)
	{ 0xACE23476, 0xB6F01A3C, }, // sceKernelCheckPspConfig
	{ 0x7BE1421C, 0xB0466E46, }, // sceKernelCheckExecFile
	{ 0xBF983EF2, 0xB95FA50D, }, // sceKernelProbeExecutableObject
	{ 0x7068E6BA, 0x6E8B6AB6, }, // sceKernelLoadExecutableObject
	{ 0x99A695F0, 0xCD1D28E3, }, // sceKernelRegisterLibrary
	{ 0x5873A31F, 0x79970B0B, }, // sceKernelRegisterLibraryForUser
	{ 0x0B464512, 0x4CF715C5, }, // sceKernelReleaseLibrary
	{ 0x9BAF90F6, 0x3E5F5629, }, // sceKernelCanReleaseLibrary
	{ 0x0E760DBA, 0x8C712A03, }, // sceKernelLinkLibraryEntries
	{ 0x0DE1F600, 0x3922A067, }, // sceKernelLinkLibraryEntriesForUser
	{ 0xC99DD47A, 0x1B107A36, }, // sceKernelQueryLoadCoreCB
	{ 0x616FCCCD, 0x371962BE, }, // sceKernelSetBootCallbackLevel
	{ 0x6B2371C2, 0xA3035783, }, // sceKernelDeleteModule
	{ 0xAE7C6E76, 0x602BF42A, }, // sceKernelRegisterModule
	{ 0x7E63F86D, 0xF62D0F30, }, // sceKernelGetLibraryLinkInfo
	{ 0xB370DF29, 0xA5E8A07F, }, // sceKernelGetLibraryList
	{ 0x5248A98F, 0xD5B046F0, }, // sceKernelLoadModuleBootLoadCore
	{ 0xC0913394, 0x1D665D6D, }, // sceKernelLinkLibraryEntriesWithModule
	{ 0xEC20C661, 0x593A3FD9, }, // LoadCoreForKernel_EC20C661
	{ 0xCDCAA8B2, 0x8F8F99D1, }, // LoadCoreForKernel_CDCAA8B2
	{ 0xCD0F3BAC, 0x7A0FB6D6, }, // sceKernelCreateModule
	{ 0x8D8A8ACE, 0x1591CFB1, }, // sceKernelAssignModule
	{ 0x74CF001A, 0x631EB12B, }, // sceKernelReleaseModule
	{ 0x05D915DB, 0x0FEC2F68, }, // sceKernelGetModuleIdListForKernel
	{ 0x52A86C21, 0xF9FD7790, }, // sceKernelGetModuleFromUID
	{ 0xAFF947D4, 0x24F00493, }, // sceKernelCreateAssignModule
	{ 0xA6D40F56, 0x141827CE, }, // LoadCoreForKernel_A6D40F56
	{ 0x929B5C69, 0xFB770EC0, }, // sceKernelGetModuleListWithAlloc
	{ 0x4440853B, 0x4B4306C2, }, // LoadCoreForKernel_4440853B
	{ 0xFA3101A4, 0x8409BAA8, }, // LoadCoreForKernel_FA3101A4
	{ 0x0C0D8B7F, 0x7A0890AD, }, // LoadCoreForKernel_0C0D8B7F
	{ 0x3BB7AC18, 0x60BC436F, }, // sceKernelLoadCoreMode
};

static nid_entry LoadExecForKernel_nid[] = {
	{ 0x6D302D3D, 0x29413885, }, // sceKernelExitVSHKernel
	{ 0x28D0D249, 0x254F01CB, }, // sceKernelLoadExecVSHMs2
	{ 0xA3D5E142, 0xAEFEF0F4, }, // sceKernelExitVSHVSH
	{ 0x1B97BDB3, 0x73BAD4F8, }, // sceKernelLoadExecVSHDisc(3.xx)
	{ 0x94A1C627, 0xAEFEF0F4, }, // sceKernelExitVSHVSH
	{ 0x821BE114, 0xD364D324, }, // sceKernelLoadExecVSHDiscUpdater
	{ 0x015DA036, 0x32224187, }, // sceKernelLoadExecBufferVSHUsbWlan
	{ 0x4F41E75E, 0x0C12D5DC, }, // sceKernelLoadExecBufferVSHUsbWlanDebug
	{ 0x31DF42BF, 0x783EA19F, }, // sceKernelLoadExecVSHMs1
	{ 0x70901231, 0xE09DAFDC, }, // sceKernelLoadExecVSHMs3
	{ 0x4AC57943, 0xB5CE3204, }, // sceKernelRegisterExitCallback
	{ 0xD9739B89, 0x71EF3793, }, // sceKernelUnregisterExitCallback
	{ 0x659188E1, 0xB848CC2C, }, // sceKernelCheckExitCallback
	{ 0x62A27008, 0x63BB2F2C, }, // sceKernelInvokeExitCallback
	{ 0x7B7C47EF, 0x00FC0987, }, // sceKernelLoadExecVSHDiscDebug
	{ 0x061D9514, 0xEFA048F1, }, // sceKernelLoadExecVSHMs4
	{ 0xB7AB08DA, 0xD1DFD7A6, }, // sceKernelLoadExecVSHMs5
	{ 0x11412288, 0xFABD21E5, }, // LoadExecForKernel_11412288
	{ 0x6D8D3A3A, 0x9D1ED0DF, }, // LoadExecForKernel_6D8D3A3A

	{ 0xD3D13800, 0x596ABD6B, },
	{ 0x15EB8575, 0xE717E255, },
};

static nid_entry ModuleMgrForKernel_nid[] = {
	{ 0x977DE386, 0xE3CCC6EA, }, // sceKernelLoadModule
	{ 0x50F0C1EC, 0xDF8FFFAB, }, // sceKernelStartModule
	{ 0xD1FF982A, 0xAAFA90C2, }, // sceKernelStopModule
	{ 0x2E0911AA, 0x9CEB18C4, }, // sceKernelUnloadModule
	{ 0x644395E2, 0x4392903C, }, // sceKernelGetModuleIdList
	{ 0xA1A78C58, 0x7F9670CF, }, // sceKernelLoadModuleDisc
	{ 0x748CBED9, 0x8F38EBE9, }, // sceKernelQueryModuleInfo
	{ 0xD675EBB8, 0x75ECB8BA, }, // sceKernelSelfStopUnloadModule
	{ 0xBA889C07, 0x5352C26C, }, // sceKernelLoadModuleBuffer
	{ 0xB7F46618, 0x3E9AA3D1, }, // sceKernelLoadModuleByID
	{ 0x91B87FAE, 0xB0752D7D, }, // sceKernelLoadModuleVSHByID
	{ 0xA4370E7C, 0x329C89DB, }, // sceKernelLoadModuleVSH
	{ 0xF0CAC59E, 0xBB6E2BCA, }, // sceKernelLoadModuleBufferVSH
	{ 0xCC1D3699, 0xB751A9DE, }, // sceKernelStopUnloadSelfModule
	{ 0x04B7BD22, 0x6620C340, }, // sceKernelSearchModuleByName
	{ 0x54D9E02E, 0xB490529C, }, // sceKernelSearchModuleByAddress
	{ 0x5F0CC575, 0x6100F075, }, // sceKernelRebootBeforeForUser
	{ 0xB49FFB9E, 0xCE234D4A, }, // sceKernelRebootBeforeForKernel
	{ 0xF0A26395, 0xB7CFD5AD, }, // sceKernelGetModuleId
	{ 0xD8B73127, 0x0F3F0B4C, }, // sceKernelGetModuleIdByAddress
	{ 0x6DE9FF11, 0xB691CB9F, }, // sceKernelLoadModuleWithApitype2
	{ 0x896C15B6, 0xD2FB2722, }, // sceKernelLoadModuleDiscUpdater
	{ 0x92EDEBD0, 0xB029065E, }, // sceKernelLoadModuleDiscDebug
	{ 0x6723BBFF, 0xF8CA0D9D, }, // sceKernelLoadModuleMs1
	{ 0x49C5B9E1, 0x9D790CD9, }, // sceKernelLoadModuleMs2/sceKernelLoadModuleForLoadExecVSHMs2
	{ 0xECC2EAA9, 0x42C48848, }, // sceKernelLoadModuleMs3
	{ 0xF07E1A2F, 0x7933CC33, }, // sceKernelLoadModuleMs4
	{ 0x0CE03872, 0xD293E707, }, // sceKernelLoadModuleForLoadExecVSHMs5
	{ 0x9F3AD1E9, 0x8E9C873C, }, // sceKernelLoadModuleBufferForExitGame
	{ 0x667C1174, 0x5EBE54F1, }, // sceKernelLoadModuleBufferForExitVSHVSH
	{ 0x5799EE32, 0x9E4C362D, }, // sceKernelLoadModuleBufferForExitVSHKernel
	{ 0x53CA70EE, 0x90F5533C, }, // sceKernelLoadModuleBufferForRebootKernel
	{ 0x2BE4807D, 0xB9FAD518, }, // sceKernelStopUnloadSelfModuleWithStatusKernel
	{ 0xA6E8C1F5, 0x0D4B0221, }, // sceKernelRebootPhaseForKernel
	{ 0x74690AEB, 0x748EC4A6, }, // sceKernelLoadModuleByIDBootInitConfig
	{ 0xF80F3C96, 0xF4B7909C, }, // sceKernelLoadModuleBufferBootInitConfig
	{ 0x61EC6AB0, 0xDBE7524B, }, // sceKernelModuleMgrMode
	{ 0x3E4FEDDC, 0x07290699, }, // sceKernelLoadModuleToBlock
	{ 0xAB77DC5A, 0xA035AADA, }, // sceKernelLoadModuleBootInitConfig
	{ 0x5C88479B, 0x2124FEC7, }, // sceKernelLoadModuleDeci
	{ 0xCE70664B, 0x5226F362, }, // sceKernelLoadModuleBufferForLoadExecBufferVSHUsbWlan
	{ 0xB5DDABD3, 0xDEE14448, }, // sceKernelLoadModuleBufferForLoadExecBufferVSHUsbWlanDebug

	{ 0xEF7A7F02, 0x76D0B981, },
	{ 0x2BE2B17E, 0x42F022E4, },
	{ 0x25E1F458, 0xDAC8A7B3, },
	{ 0xF7E46A72, 0xA63A565A, },
	{ 0x1B91F6EC, 0xADAE7610, },
	{ 0x245B698D, 0xED5094E2, },
};

static nid_entry ExceptionManagerForKernel_nid[] = {
	{ 0x565C0B0E, 0x557FBABB, }, // sceKernelRegisterDefaultExceptionHandler
	{ 0x3FB264FC, 0x56198384, }, // sceKernelRegisterExceptionHandler
	{ 0x5A837AD4, 0x71286CF4, }, // sceKernelRegisterPriorityExceptionHandler
	{ 0x1AA6CFFA, 0xC0BC60E9, }, // sceKernelReleaseExceptionHandler
	{ 0xDF83875E, 0x014FD1D1, }, // sceKernelGetActiveDefaultExceptionHandler
	{ 0x291FF031, 0x85F02133, }, // sceKernelReleaseDefaultExceptionHandler
	{ 0xFC26C354, 0x7DDFBCC7, },
};

static nid_entry IoFileMgrForKernel_nid[] = {
	{ 0x3C54E908, 0xF0AD3C72, }, // sceIoReopen
	{ 0x411106BA, 0x0AB86E5D, }, // sceIoGetThreadCwd
	{ 0xA905B705, 0xEB5BE7EC, }, // sceIoCloseAll
	{ 0xCB0A151F, 0x9D5CDC31, }, // sceIoChangeThreadCwd
	{ 0x73B7671F, 0xDC30BB19, }, // sceIoGetFdDebugInfo(371)
	{ 0x9B86630B, 0x4F663EBF, }, // sceIoGetUID
	{ 0x30E8ABB3, 0xF2990AC6, }, // IoFileMgrForKernel_30E8ABB3
	{ 0x42F954D4, 0xB2C5D86F, }, // sceIoAddHook
	{ 0xBD17474F, 0x6CE0E5F0, }, // IoFileMgrForKernel_BD17474F
};

static nid_entry scePower_driver_nid[] = {
	{ 0xEFD3C963, 0xE59EF335, }, // scePowerTick
	{ 0x87440F5E, 0x7F480684, }, // scePowerIsPowerOnline
	{ 0x0AFD0D8B, 0xBC8823E8, }, // scePowerIsBatteryExist
	{ 0x1E490401, 0x5202A826, }, // scePowerIsBatteryCharging
	{ 0xD3075926, 0x279492B3, }, // scePowerIsLowBattery
	{ 0x2085D15D, 0xE5F8596E, }, // scePowerGetBatteryLifePercent
	{ 0x8EFB3FA2, 0xB3C30947, }, // scePowerGetBatteryLifeTime
	{ 0x28E12023, 0xE66978D7, }, // scePowerGetBatteryTemp
	{ 0x483CE86B, 0x3C7B0B84, }, // scePowerGetBatteryVolt
	{ 0x2B7C7CF4, 0x47F4E1A8, }, // scePowerRequestStandby
	{ 0xFEE03A2F, 0xFDB5BFE9, }, // scePowerGetCpuClockFrequency
	{ 0x478FE6F5, 0xBF5BA7FC, }, // scePowerGetBusClockFrequency
	{ 0x737486F2, 0xEBD177D6, }, // scePowerSetClockFrequency
	{ 0xD6E50D7B, 0x1A41E0ED, }, // scePowerRegisterCallback(3.71)
	{ 0x04B7766E, 0x1A41E0ED, }, // scePowerRegisterCallback(1.50)
	{ 0x1688935C, 0xBF5BA7FC, }, // scePowerGetBusClockFrequencyInt
	{ 0x6D2CA84B, 0xA8909AE1, }, // scePowerWlanActivate
	{ 0x23BB0A60, 0x5D311801, }, // scePowerWlanDeactivate
	{ 0x2B51FE2F, 0xCE9C5F0E, }, // scePower_driver_2B51FE2F
	{ 0x442BFBAC, 0x4084E678, }, // scePowerGetBacklightMaximum
	{ 0xEDC13FE5, 0xB361215A, }, // scePowerGetIdleTimer
	{ 0x7F30B3B1, 0x69513373, }, // scePowerIdleTimerEnable
	{ 0x972CE941, 0x21361196, }, // scePowerIdleTimerDisable
	{ 0x27F3292C, 0x79CC68FC, }, // scePowerBatteryUpdateInfo
	{ 0xE8E4E204, 0x9E77DDBA, }, // scePower_driver_E8E4E204
	{ 0xB999184C, 0x8BDEC501, }, // scePowerGetLowBatteryCapacity
	{ 0x166922EC, 0xA247240B, }, // scePowerBatteryForbidCharging
	{ 0xDD3D4DAC, 0xCA1B6E91, }, // scePowerBatteryPermitCharging
	{ 0xB4432BC8, 0x5D9E954F, }, // scePowerGetBatteryChargingStatus
	{ 0x78A1A796, 0x9E09D19B, }, // scePowerIsSuspendRequired
	{ 0x94F5A53F, 0xB6782EAE, }, // scePowerGetBatteryRemainCapacity
	{ 0xFD18A0FF, 0x6E7E0056, }, // scePowerGetBatteryFullCapacity
	{ 0x862AE1A6, 0xDC73B503, }, // scePowerGetBatteryElec
	{ 0x23436A4A, 0x4AA479BE, }, // scePowerGetInnerTemp
	{ 0x0CD21B1F, 0x9E08030B, }, // scePowerSetPowerSwMode
	{ 0x165CE085, 0x92030113, }, // scePowerGetPowerSwMode
	{ 0xD6D016EF, 0x534D471F, }, // scePowerLock
	{ 0xCA3D34C1, 0xD80E403B, }, // scePowerUnlock
	{ 0x79DB9421, 0x86B475A4, }, // scePowerRebootStart
	{ 0x7FA406DD, 0xE5C40F85, }, // scePowerIsRequest
	{ 0xAC32C9CC, 0xC9885394, }, // scePowerRequestSuspend
	{ 0x2875994B, 0x8AFD31BC, }, // scePower_driver_2875994B
	{ 0x3951AF53, 0x39274F61, }, // scePowerEncodeUBattery
	{ 0x0074EF9B, 0x0633D9DC, }, // scePowerGetResumeCount
	{ 0xF535D928, 0x9A27EFA6, }, // scePowerSetWakeupCondition
	{ 0xDFA8BAF8, 0xCE5D389B, }, // scePowerUnregisterCallback
	{ 0xDB9D28DD, 0xCE5D389B, }, // scePowerUnregitserCallback
	{ 0xD24E6BEB, 0xA30B558A, }, // scePowerLimitScCpuClock
	{ 0x35B7662E, 0x2F67557D, }, // scePowerGetSectionDescriptionEntry
	{ 0xF9B4DEA1, 0xE3A5CA86, }, // scePowerLimitPllClock
	{ 0x843FBF43, 0x37DCC2DA, }, // scePowerSetCpuClockFrequency
	{ 0xB8D7B3FB, 0xC5371DAD, }, // scePowerSetBusClockFrequency
	{ 0x478FE6F5, 0xBF5BA7FC, }, // scePowerGetBusClockFrequency
	{ 0xBD681969, 0xBF5BA7FC, }, // scePowerGetBusClockFrequencyInt
	{ 0xB1A52C83, 0x4CAE06EF, }, // scePowerGetCpuClockFrequencyFloat
	{ 0x9BADB3EB, 0x279FD567, }, // scePowerGetBusClockFrequencyFloat
	{ 0xE0B7A95D, 0x08354E58, }, // scePowerSetGeEdramRefreshMode
	{ 0xC23AC778, 0xA4ABE2C5, }, // scePowerGetGeEdramRefreshMode
	{ 0x23C31FFE, 0x12FFFD34, }, // scePowerVolatileMemLock
	{ 0xFA97A599, 0x74035D33, }, // scePowerVolatileMemTryLock
	{ 0xB3EDD801, 0x8E71E273, }, // scePowerVolatileMemUnlock
	{ 0xA9D22232, 0xDB086E2B, }, // scePowerSetCallbackMode
	{ 0xBAFA3DF0, 0x75E2A155, }, // scePowerGetCallbackMode
	{ 0x57A098B4, 0x9E1FB9FE, }, // scePowerGetCurrentTachyonVoltage(2.80)
	{ 0x55D2D789, 0x071F03B8, }, // scePowerGetTachyonVoltage
	{ 0xDD27F119, 0x45E2AEB9, }, // scePowerGetTachyonVoltage
	{ 0x75F91FE4, 0xE4C917A5, }, // scePowerGetCurrentDdrVoltage
	{ 0x06C83A4F, 0x6DE6166A, }, // scePowerGetDdrVoltage
	{ 0x4E759486, 0x37C9C3FE, }, // scePowerSetDdrVoltage
	{ 0x53C1255D, 0xDDBB64D9, }, // scePower_driver_53C1255D
	{ 0x8762BCE6, 0x5D3CE741, }, // scePower_driver_8762BCE6
	{ 0xCAE2A4CA, 0x7E30F280, }, // scePower_driver_CAE2A4CA
	{ 0xF06B96D4, 0x5996754F, }, // scePower_driver_F06B96D4
	{ 0x34F9C463, 0xBA93F79B, }, // scePowerGetPllClockFrequencyInt
	{ 0xEA382A27, 0x666E6BE3, }, // scePowerGetPllClockFrequencyFloat
	{ 0x3234844A, 0x284A0F59, }, // scePower_driver_3234844A
	{ 0xA4D02055, 0x5BB0D018, }, // scePower_driver_A4D02055
	{ 0x9C40E184, 0xE59EF335, }, // scePowerTick(371)
	{ 0xCB49F5CE, 0xA73D777D, }, // scePowerGetBatteryChargeCycle
	{ 0x0442D852, 0x599A2901, }, // scePowerRequestColdReset
	{ 0x733F973B, 0xF52F66EB, }, // scePowerBatteryEnableUsbCharging
	{ 0x90285886, 0xC2B7CD0C, }, // scePowerBatteryDisableUsbCharging
	{ 0xD66EF08D, 0x9CDB22B5, }, // scePowerCheckWlanCondition

	{ 0x9B1A9C5F, 0x47F4E1A8, },
	{ 0xE65F00BD, 0x599A2901, },
	{ 0xD7B9C925, 0x2D5A880D, },
	{ 0xC71EE866, 0x207119B3, },
	{ 0xC3C33306, 0x756261FC, },
};

static nid_entry InterruptManagerForKernel_nid[] = {
	{ 0x8A389411, 0xC03C8D51, }, // sceKernelDisableSubIntr
	{ 0xD2E8363F, 0x2BF441EF, }, // QueryIntrHandlerInfo
	{ 0x8B61808B, 0xAC9306F0, }, // sceKernelQuerySystemCall
	{ 0x85F7766D, 0xD24AE49D, }, // SaveThreadContext
	{ 0x00B6B0F3, 0x5F3525D5, }, // QueryInterruptManCB
	{ 0x15894D0B, 0xD2274728, }, // InterruptManagerForKernel_15894D0B
	{ 0xB5A15B30, 0x54ACF6F8, }, // sceKernelSetIntrLevel
	{ 0x43A7BBDC, 0xCCC5A332, }, // InterruptManagerForKernel_43A7BBDC
	{ 0x750E2507, 0xEC5485ED, }, // sceKernelSuspendIntr
	{ 0x494D6D2B, 0xEC5485ED, }, // sceKernelResumeIntr
	{ 0x55242A8B, 0xE3CDA797, }, // ReleaseContextHooks
	{ 0x27BC9A45, 0x47B6E6B7, }, // UnSupportIntr
	{ 0x0E224D66, 0x3AB4955A, }, // SupportIntr
	{ 0xCDC86B64, 0xEC51802D, }, // sceKernelCallSubIntrHandler
	{ 0xD6878EB6, 0xFEE218CE, }, // sceKernelGetUserIntrStack
	{ 0xF4454E44, 0xD157AF51, }, // sceKernelCallUserIntrHandler
	{ 0xCA04A2B9, 0x3C161DFB, }, // sceKernelRegisterSubIntrHandler
	{ 0x5CB5A78B, 0x25F676D4, }, // sceKernelSuspendSubIntr
	{ 0x7860E0DC, 0x85E7F044, }, // sceKernelResumeSubIntr
	{ 0xFC4374B8, 0x2D70650B, }, // sceKernelIsSubInterruptOccurred
	{ 0x35634A64, 0x0371FA94, }, // sceKernelGetCpuClockCounterWide
	{ 0x2DC9709B, 0x53F5A3F8, }, // _sceKernelGetCpuClockCounterLow
	{ 0xE9E652A9, 0x247EBFCB, }, // _sceKernelGetCpuClockCounterHigh
	{ 0x0FC68A56, 0x667D2693, }, // sceKernelSetPrimarySyscallHandler
	{ 0xF4D443F3, 0x580DDADD, }, // sceKernelRegisterSystemCallTable
	{ 0x36B1EF81, 0x2BF441EF, }, // sceKernelQueryIntrHandlerInfo
	{ 0xEB988556, 0xAC9306F0, }, // sceKernelQuerySystemCall(5xx)

	{ 0x07E138EE, 0x89081386, },
	{ 0xD8410DE6, 0x79EE281A, },
	{ 0x8DC2BAEF, 0x114435E5, },
	{ 0x0B907DBC, 0xF952C56F, },
	{ 0x7A05D976, 0x80EE1E2E, },
	{ 0x7EF7500F, 0xB8D11C47, },
	{ 0x05E9D6C4, 0x36A721FB, },
	{ 0xD63F3B31, 0xA359F4BA, },
	{ 0x72CE22E8, 0x157ADE6A, },
};

static nid_entry sceCtrl_driver_nid[] = {
	{ 0x454455AC, 0xD073ECA4, }, //sceCtrlReadBufferPositive
	{ 0xC4AAD55F, 0x3CA6922B, }, //sceCtrlPeekBufferPositive
	{ 0x28E71A16, 0x262DD0DC, }, //sceCtrlSetSamplingMode
	{ 0x3E65A0EA, 0x04A88056, }, // sceCtrlInit
	{ 0xE03956E9, 0x8FDE3DD5, }, // sceCtrlEnd
	{ 0xC3F607F3, 0xA0798B29, }, // sceCtrlSuspend
	{ 0xC245B57B, 0x84793F3E, }, // sceCtrlResume
	{ 0xA88E8D22, 0x0DCB5BD2, }, // sceCtrlSetIdleCancelKey
	{ 0xB7CEAED4, 0x333872B7, }, // sceCtrlGetIdleCancelKey
	{ 0x7CA723DC, 0xF3132A07, }, // sceCtrl_driver_7CA723DC
	{ 0x5E77BC8A, 0x063D8197, }, // sceCtrl_driver_5E77BC8A
	{ 0x5C56C779, 0xEB6CDD17, }, // sceCtrl_driver_5C56C779
	{ 0x1F4011E6, 0x262DD0DC, }, // sceCtrlSetSamplingMode(150)
	{ 0x3A622550, 0x3CA6922B, }, // sceCtrlPeekBufferPositive(150)
	{ 0xC152080A, 0x2468E1F3, }, // sceCtrlPeekBufferNegative(150)
	{ 0x1F803938, 0xD073ECA4, }, // sceCtrlReadBufferPositive(150)
	{ 0x60B81F86, 0x5E758582, }, // sceCtrlReadBufferNegative(150)
	{ 0xDA6B76A1, 0xC44ED01B, }, // sceCtrlGetSamplingMode
	{ 0x6A2774F3, 0xE4758982, }, // sceCtrlSetSamplingCycle
	{ 0xB1D0E5CD, 0x8440BBFA, }, // sceCtrlPeekLatch
	{ 0x0B588501, 0x821F0F79, }, // sceCtrlReadLatch
	{ 0xA7144800, 0x32999159, }, // sceCtrlSetIdleCancelThreshold
	{ 0x687660FA, 0xD821DF71, }, // sceCtrlGetIdleCancelThreshold
	{ 0x6841BE1A, 0xA2FF2213, }, // sceCtrlSetRapidFire
	{ 0x02BAAD91, 0xFE15E7BA, }, // sceCtrlGetSamplingCycle
	{ 0xA68FD260, 0xF7E92CE5, }, // sceCtrlClearRapidFire
	{ 0x252D3A8D, 0x5B2E5299, }, // sceCtrlSetPollingMode

	{ 0x6D74BF08, 0xE4758982, },
	{ 0xFF847C31, 0x5E758582, },
	{ 0x348D99D4, 0xB7D6332B, },
	{ 0xAF5960F3, 0xD2EC6240, },
};

static nid_entry sceDisplay_driver_nid[] = {
	{ 0x4AB7497F, 0xA231DB56, }, // sceDisplaySetFrameBuf
	{ 0xB685BA36, 0x2B4FF881, }, // sceDisplayWaitVblankStart
	{ 0xE56B11BA, 0x64EE9BF2, }, // sceDisplayGetFrameBuf
	{ 0x776ADFDB, 0xFF5A5D52, }, // sceDisplaySetBacklightInternal
	{ 0x31C4BAA8, 0x43BA0974, }, // sceDisplayGetBrightness
	{ 0x5B5AEFAD, 0x452B3820, }, // from 1.50
	{ 0xDBA6C4C4, 0xD41F8847, }, // sceDisplayGetFramePerSec
	{ 0x432D133F, 0xA20A245E, }, // sceDisplayEnable
	{ 0x681EE6A7, 0x2A1826A2, }, // sceDisplayDisable
	{ 0x7ED59BC4, 0xC7161CF7, }, // sceDisplaySetHoldMode
	{ 0xA544C486, 0xAAC50D73, }, // sceDisplaySetResumeMode
	{ 0x63E22A26, 0xDC68E0EF, }, // sceDisplay_driver_63E22A26
	{ 0x289D82FE, 0xA231DB56, }, // sceDisplaySetFrameBuf
	{ 0xEEDA2E54, 0x64EE9BF2, }, // sceDisplayGetFrameBuf
	{ 0xB4F378FA, 0xB550528B, }, // sceDisplayIsForeground
	{ 0xAC14F1FF, 0x4E7E51FC, }, // sceDisplayGetForegroundLevel
	{ 0x9E3C6DC6, 0xFF5A5D52, }, // sceDisplaySetBrightness
	{ 0x9C6EAAD7, 0x82DE8125, }, // sceDisplayGetVcount
	{ 0x4D4E10EC, 0xBB8FD5C9, }, // sceDisplayIsVblank
	{ 0x69B53541, 0x3F945436, }, // sceDisplayGetVblankRest
	{ 0x36CDFADE, 0xECFADEAB, }, // sceDisplayWaitVblank
	{ 0x8EB9EC49, 0x770DF790, }, // sceDisplayWaitVblankCB
	{ 0x984C27E7, 0x2B4FF881, }, // sceDisplayWaitVblankStart
	{ 0x46F186C3, 0x0F82D90A, }, // sceDisplayWaitVblankStartCB
	{ 0x773DD3A3, 0x1C93147A, }, // sceDisplayGetCurrentHcount
	{ 0x210EAB3A, 0x2F06E30B, }, // sceDisplayGetAccumulatedHcount
	{ 0xBF79F646, 0xFAFF79B3, }, // sceDisplayGetResumeMode
	{ 0xA83EF139, 0x2F4E7DF1, }, // sceDisplayAdjustAccumulatedHcount
	{ 0x21038913, 0x800E01C6, }, // sceDisplayIsVsync
	{ 0x92C8F8B7, 0x2DFD5B55, }, // sceDisplayIsPseudoField
	{ 0xAE0E8972, 0xF4B7B531, }, // sceDisplaySetPseudoVsync

	{ 0x643F3841, 0xDC68E0EF, },
	{ 0x820C6038, 0x452B3820, },
	{ 0x1CB8CB47, 0x43BA0974, },
	{ 0x13AA96B7, 0xD41F8847, },
	{ 0x66961521, 0x0AAF242A, },
	{ 0xF5EEEFEF, 0x6C42016A, },
	{ 0xE8466BC2, 0x82DE8125, },
	{ 0x946155FD, 0xA20A245E, },
	{ 0x32B67781, 0x2A1826A2, },
};

static nid_entry sceAudio_driver_nid[] = {
	{ 0xB282F4B2, 0x77EBE4A2, }, // sceAudioGetChannelRestLength
	{ 0x669D93E4, 0xA502088E, }, // sceAudioSRCChReserve
	{ 0x138A70F1, 0xF462A7B9, }, // sceAudioSRCChRelease
	{ 0x43645E69, 0xE26E49E7, }, // sceAudioSRCOutputBlocking
	{ 0x80F1F7E0, 0x0C4213E3, }, // sceAudioInit
	{ 0x210567F7, 0x3C20F1F3, }, // sceAudioEnd
	{ 0xA2BEAA6C, 0x45543440, }, // sceAudioSetFrequency
	{ 0xB61595C0, 0x6354634C, }, // sceAudioLoopbackTest
	{ 0x927AC32B, 0xBF1E8C42, }, // sceAudioSetVolumeOffset
	{ 0x8C1009B2, 0xA5C0603B, }, // sceAudioOutput
	{ 0x136CAF51, 0x2FD643D0, }, // sceAudioOutputBlocking
	{ 0xE2D56B2D, 0x548EEFD1, }, // sceAudioOutputPanned
	{ 0x13F592BC, 0x23F19798, }, // sceAudioOutputPannedBlocking
	{ 0x5EC81C55, 0x64B9E089, }, // sceAudioChReserve
	{ 0x41EFADE7, 0x45457EE5, }, // sceAudioOneshotOutput
	{ 0x6FC46853, 0xA05BAA21, }, // sceAudioChRelease
	{ 0xB011922F, 0x77EBE4A2, }, // sceAudioGetChannelRestLength
	{ 0xCB2E439E, 0x6F968893, }, // sceAudioSetChannelDataLen
	{ 0x95FD0C2D, 0xCD62C216, }, // sceAudioChangeChannelConfig
	{ 0xB7E1D8E7, 0x57AB835D, }, // sceAudioChangeChannelVolume
	{ 0x38553111, 0xA502088E, }, // sceAudioSRCChReserve
	{ 0x5C37C0AE, 0xF462A7B9, }, // sceAudioSRCChRelease
	{ 0xE0727056, 0xE26E49E7, }, // sceAudioSRCOutputBlocking
	{ 0x086E5895, 0x7F93FE1C, }, // sceAudioInputBlocking
	{ 0x6D4BEC68, 0xEC8B87CD, }, // sceAudioInput
	{ 0xA708C6A6, 0xFA700131, }, // sceAudioGetInputLength
	{ 0x87B2E651, 0x6B9C59FE, }, // sceAudioWaitInputEnd
	{ 0x7DE61688, 0xB82D416A, }, // sceAudioInputInit
	{ 0xE926D3FB, 0xB82D416A, }, // sceAudioInputInitEx
	{ 0xA633048E, 0x3076A373, }, // sceAudioPollInputEnd
	{ 0xE9D97901, 0x79AF2DA3, }, // sceAudioGetChannelRestLen
	{ 0x647CEF33, 0xA500F7F2, }, // sceAudioOutput2GetRestSample
};

static nid_entry sceReg_driver_nid[] = {
	{ 0x98279CF1, 0x10808BB5, }, // sceRegInit
	{ 0x9B25EDF1, 0x5B06A7B9, }, // sceRegExit
	{ 0x92E41280, 0x873EB4E1, }, // sceRegOpenRegistry
	{ 0xFA8A5739, 0xF3300EBF, }, // sceRegCloseRegistry
	{ 0xDEDA92BF, 0x632E987D, }, // sceRegRemoveRegistry
	{ 0x1D8A762E, 0x8A0F5133, }, // sceRegOpenCategory
	{ 0x0CAE832B, 0x21928F15, }, // sceRegCloseCategory
	{ 0x39461B4D, 0x31F9DB91, }, // sceRegFlushRegistry
	{ 0x0D69BF40, 0x0404652C, }, // sceRegFlushCategory
	{ 0x57641A81, 0x762C35DC, }, // sceRegCreateKey
	{ 0x17768E14, 0xDC7EF81C, }, // sceRegSetKeyValue
	{ 0xD4475AA8, 0x400BB753, }, // sceRegGetKeyInfo
	{ 0x28A8E98A, 0x08BEB24C, }, // sceRegGetKeyValue
	{ 0x2C0DB9DD, 0xF8E1F5A1, }, // sceRegGetKeysNum
	{ 0x2D211135, 0x73B8B70E, }, // sceRegGetKeys
	{ 0x4CA16893, 0xAF9FBE8E, }, // sceRegRemoveCategory
	{ 0x3615BC87, 0x89DBFC66, }, // sceRegRemoveKey
	{ 0xC5768D02, 0xB4E27416, }, // sceRegGetKeyInfoByName
	{ 0x30BE0259, 0x4D81184D, }, // sceRegGetKeyValueByName
	{ 0xBE8C1263, 0x675482AD, }, // sceReg_BE8C1263
	{ 0x835ECE6F, 0x3D3A1014, }, // sceReg_835ECE6F
};

static nid_entry SysTimerForKernel_nid[] = {
	{ 0xC99073E3, 0x8758ACCD, }, // sceSTimerAlloc
	{ 0xC105CF38, 0x4ECB4A4C, }, // sceSTimerFree
	{ 0xB53534B4, 0xCCC7A9E4, }, // SysTimerForKernel_B53534B4
	{ 0x975D8E84, 0xCA553E9C, }, // sceSTimerSetHandler
	{ 0xA95143E2, 0xE7BD0E48, }, // sceSTimerStartCount
	{ 0x4A01F9D3, 0x9D20D194, }, // sceSTimerStopCount
	{ 0x54BB5DB4, 0x30D5FF7A, }, // sceSTimerResetCount
	{ 0x228EDAE4, 0xC01A8A7E, }, // sceSTimerGetCount
	{ 0x53231A15, 0xD945310D, }, // SysTimerForKernel_53231A15
};

static nid_entry sceImpose_driver_nid[] = {
	{ 0x0F341BE4, 0x67544934, }, // sceImposeGetHomePopup
	{ 0x116CFF64, 0x2700B882, }, // sceImposeCheckVideoOut
	{ 0x116DDED6, 0x25011AAB, }, // sceImposeSetVideoOutMode
	{ 0x1AEED8FE, 0x39B67435, }, // sceImposeSuspend
	{ 0x1B6E3400, 0xDC6B1382, }, // sceImposeGetStatus
	{ 0x24FD7BCF, 0x7C972622, }, // sceImposeGetLanguageMode
	{ 0x36AA6E91, 0xEC3F0727, }, // sceImposeSetLanguageMode
	{ 0x381BD9E7, 0xFD250093, }, // sceImposeHomeButton
	{ 0x531C9778, 0xC94AC8E2, }, // sceImposeGetParam
	{ 0x5595A71A, 0x5D93BBE4, }, // sceImposeSetHomePopup
	{ 0x7084E72C, 0x90AB6874, }, // sceImpose_driver_7084E72C
	{ 0x72189C48, 0xD8CCF6D2, }, // sceImposeSetUMDPopup
	{ 0x810FB7FB, 0xC5EA0BAC, }, // sceImposeSetParam
	{ 0x86924032, 0x80B0A780, }, // sceImposeResume
	{ 0x8C943191, 0x4D849B71, }, // sceImposeGetBatteryIconStatus
	{ 0x8F6E3518, 0xC54F98F6, }, // sceImposeGetBacklightOffTime
	{ 0x967F6D4A, 0x9954A241, }, // sceImposeSetBacklightOffTime
	{ 0x9C8C6C81, 0x01EF0650, }, // sceImposeSetStatus
	{ 0x9DBCE0C4, 0x6A35E462, }, // sceImpose_driver_9DBCE0C4
	{ 0xB415FC59, 0x55864BD7, }, // sceImposeChanges
	{ 0xBDBC42A6, 0x76D7E4D4, }, // sceImposeInit
	{ 0xC7E36CC7, 0x3745B423, }, // sceImposeEnd
	{ 0xE0887BC8, 0xCFA708BD, }, // sceImposeGetUMDPopup

	{ 0xFF1A2F07, 0xBFB8473E, },
};

static nid_entry sceGe_driver_nid[] = {
	{ 0x71FCD1D6, 0x8AC047D1, }, // sceGeInit
	{ 0x9F2C2948, 0x6175E7FC, }, // sceGeEnd
	{ 0x8F185DF7, 0x9F724333, }, // sceGeEdramInit
	{ 0x1F6752AD, 0xF489E74B, }, // sceGeEdramGetSize
	{ 0xE47E40E4, 0x9919BBE3, }, // sceGeEdramGetAddr
	{ 0xB77905EA, 0x8F1E3AF7, }, // sceGeEdramSetAddrTranslation
	{ 0xB415364D, 0x87C1C34C, }, // sceGeGetReg
	{ 0xDC93CFEF, 0x2B37561E, }, // sceGeGetCmd
	{ 0x57C8945B, 0xA23C2482, }, // sceGeGetMtx
	{ 0x438A385A, 0x36585054, }, // sceGeSaveContext
	{ 0x0BF608FB, 0x87A22519, }, // sceGeRestoreContext
	{ 0xAB49E76A, 0x7E2381D2, }, // sceGeListEnQueue
	{ 0x1C0D95A6, 0x362B73C5, }, // sceGeListEnQueueHead
	{ 0x5FB86AB0, 0x7845ABFC, }, // sceGeListDeQueue
	{ 0xE0D68148, 0x862248FD, }, // sceGeListUpdateStallAddr
	{ 0x03444EB4, 0x23E9F5FA, }, // sceGeListSync
	{ 0xB287BD61, 0xAC348640, }, // sceGeDrawSync
	{ 0xB448EC0D, 0x3351EE04, }, // sceGeBreak
	{ 0x4C06E472, 0x32A76BF7, }, // sceGeContinue
	{ 0xA4FC06A4, 0xD57B9873, }, // sceGeSetCallback
	{ 0x05DB22CE, 0xFFD33EAC, }, // sceGeUnsetCallback
	{ 0x9DA4A75F, 0x75CCB8EF, }, // sceGe_driver_9DA4A75F
	{ 0x114E1745, 0xE3BCEAB6, }, // sceGe_driver_114E1745
	{ 0x51C8BB60, 0x8928D81B, }, // sceGeSetReg
	{ 0x51D44C58, 0xE18D6B57, }, // sceGeSetCmd
	{ 0x5A0103E6, 0x56A5110E, }, // sceGeSetMtx
	{ 0xAEC21518, 0x3832B8F7, }, // sceGe_driver_AEC21518
	{ 0x7B481502, 0xDABC7000, }, // sceGe_driver_7B481502
	{ 0x9ACFF59D, 0x002E0226, }, // sceGeGetListIdList
	{ 0x67B01D8E, 0x7FD1EBEF, }, // sceGeGetList
	{ 0xE66CB92E, 0x58935A8B, }, // sceGeGetStack
	{ 0x5BAA5439, 0xD4D665C9, }, // sceGeEdramSetSize

	{ 0xBAD6E1CA, 0xE08CA161, },
	{ 0xC576E897, 0xC774B373, },
};

static nid_entry sceRtc_driver_nid[] = {
	{ 0xE7C27D1B, 0xA0B3BF0F, }, // sceRtcGetCurrentClockLocalTime
	{ 0x34885E0D, 0x2F20DAF0, }, // sceRtcConvertUtcToLocalTime
	{ 0x779242A2, 0x6A41FE5E, }, // sceRtcConvertLocalTimeToUTC
	{ 0x42307A17, 0x2510903A, }, // sceRtcIsLeapYear
	{ 0x05EF322C, 0xA99453A4, }, // sceRtcGetDaysInMonth
	{ 0x57726BC1, 0x6F2FC44B, }, // sceRtcGetDayOfWeek
	{ 0x3A807CC8, 0x4A317519, }, // sceRtcSetTime_t
	{ 0x27C4594C, 0x7B463901, }, // sceRtcGetTime_t
	{ 0xF006F264, 0xB9B1CD57, }, // sceRtcSetDosTime
	{ 0x36075567, 0xE9B6C08A, }, // sceRtcGetDosTime
	{ 0x7ACE4C04, 0x36CB9E04, }, // sceRtcSetWin32FileTime
	{ 0xCF561893, 0x120ED794, }, // sceRtcGetWin32FileTime
	{ 0x9ED0AE87, 0xE2BE9595, }, // sceRtcCompareTick
	{ 0x26D25A5D, 0x3BFB5E74, }, // sceRtcTickAddMicroseconds
	{ 0xE6605BCA, 0xC59704E1, }, // sceRtcTickAddMinutes
	{ 0x26D7A24A, 0x63EF93F0, }, // sceRtcTickAddHours
	{ 0xE51B4B7A, 0x258213AF, }, // sceRtcTickAddDays
	{ 0xCF3A2CA8, 0xC4A12BFC, }, // sceRtcTickAddWeeks
	{ 0xDBF74F1B, 0xEB013962, }, // sceRtcTickAddMonths
	{ 0x42842C77, 0x76C60BEB, }, // sceRtcTickAddYears
	{ 0xC663B3B9, 0x108E9C68, }, // sceRtcFormatRFC2822
	{ 0x7DE6711B, 0xB1E3C114, }, // sceRtcFormatRFC2822LocalTime
	{ 0x0498FB3C, 0x05487F6A, }, // sceRtcFormatRFC3339
	{ 0x27F98543, 0x5D6735B0, }, // sceRtcFormatRFC3339LocalTime
	{ 0xDFBC5F16, 0xFC555073, }, // sceRtcParseDateTime
	{ 0x28E1E988, 0xDDC10EEE, }, // sceRtcParseRFC3339
	{ 0x029CA3B3, 0x011F03C1, }, // sceRtcGetAccumulativeTime
	{ 0x17C26C00, 0x4F970DF9, }, // sceRtcSetCurrentSecureTick
	{ 0x1909C99B, 0xCD653A7E, }, // sceRtcSetTime64_t
	{ 0x203CEB0D, 0xC55A5A0C, }, // sceRtcGetLastReincarnatedTime
	{ 0x62685E98, 0x7D70D63F, }, // sceRtcGetLastAdjustedTime
	{ 0x6A676D2D, 0x49FF9A51, }, // sceRtcUnregisterCallback
	{ 0x759937C5, 0xCFBE4A68, }, // sceRtcSetConf
	{ 0x7D1FBED3, 0x68AED59A, }, // sceRtcSetAlarmTick
	{ 0x81FCDA34, 0x46C695AA, }, // sceRtcIsAlarmed
	{ 0xC2DDBEB5, 0x19C3C426, }, // sceRtcGetAlarmTick
	{ 0xC499AF2F, 0x2D37DC1E, }, // sceRtcReset
	{ 0xE1C93E47, 0x4F9F0E2C, }, // sceRtcGetTime64_t
	{ 0xF0B5571C, 0xB859041B, }, // sceRtcSynchronize
	{ 0xFB3B18CD, 0xE1F7B409, }, // sceRtcRegisterCallback
	{ 0x9763C138, 0x8FD3886D, }, // sceRtcSetCurrentTick
	{ 0xF2A4AFE5, 0xF6E4C8A6, }, // sceRtcTickAddSeconds

	{ 0xD24DF719, 0xF2FCB0F3, },
	{ 0xF5FCC995, 0xF2FCB0F3, },
};

static nid_entry sceSyscon_driver_nid[] = {
	{ 0x58531E69, 0x70B4B7D9, }, // sceSysconSetGSensorCallback
	{ 0xAD555CE5, 0x17B5F203, }, // sceSysconSetLowBatteryCallback
	{ 0xF281805D, 0x235C7D1B, }, // sceSysconSetPowerSwitchCallback
	{ 0xA068B3D0, 0x4BAEC2F6, }, // sceSysconSetAlarmCallback
	{ 0xE540E532, 0x76986BED, }, // sceSysconSetAcSupplyCallback
	{ 0xBBFB70C0, 0x9EC42912, }, // sceSysconSetHPConnectCallback
	{ 0x805180D1, 0x75867DAD, }, // sceSysconSetHRPowerCallback
	{ 0xA8E34316, 0x97E67774, }, // sceSysconSetHRWakeupCallback
	{ 0x53072985, 0xFDEA1FB6, }, // sceSysconSetWlanSwitchCallback
	{ 0xF9193EC3, 0xE5087D50, }, // sceSysconSetWlanPowerCallback
	{ 0x88FDB279, 0x195C1E27, }, // sceSysconSetBtSwitchCallback
	{ 0xBAAECDF8, 0xB1A756B3, }, // sceSysconSetBtPowerCallback
	{ 0x7479DB05, 0xA6E2325E, }, // sceSysconSetHoldSwitchCallback
	{ 0x6848D817, 0x6E7D63DD, }, // sceSysconSetUmdSwitchCallback
	{ 0xFC32141A, 0xAE3CD6FD, }, // sceSysconGetPowerSupplyStatus
	{ 0xF775BC34, 0xDCA038A1, }, // sceSysconGetWakeUpFactor
	{ 0xA9AEF39F, 0xC25BB6C4, }, // sceSysconGetWakeUpReq
	{ 0x25F47F96, 0xE143D30E, }, // sceSysconGetVideoCable
	{ 0xC4D66C1D, 0x83CB6732, }, // sceSysconReadClock
	{ 0xC7634A7A, 0x23EC219F, }, // sceSysconWriteClock
	{ 0x7A805EE4, 0x77877E75, }, // sceSysconReadAlarm
	{ 0x6C911742, 0x4B3EC953, }, // sceSysconWriteAlarm
	{ 0xE9FF8226, 0x49CA079D, }, // sceSysconSetUSBStatus
	{ 0x882F0AAB, 0x2675D6DA, }, // sceSysconGetTachyonWDTStatus
	{ 0x00E7B6C2, 0x0FD370D9, }, // sceSysconCtrlAnalogXYPolling
	{ 0x44439604, 0x76294748, }, // sceSysconCtrlHRPower
	{ 0x91E183CB, 0x9E5CAC98, }, // sceSysconPowerSuspend
	{ 0xBE27FE66, 0x1F27CA6D, }, // sceSysconCtrlPower
	{ 0x1A08CD66, 0x4ADD8A39, }, // sceSysconCtrlDvePower
	{ 0x19D5A97A, 0x14400A9A, }, // sceSysconCtrlCharge
	{ 0xE5E35721, 0xF7E9487A, }, // sceSysconCtrlTachyonAvcPower
	{ 0x01677F91, 0x4F3581B0, }, // sceSysconCtrlVoltage
	{ 0x09721F7F, 0x3D83B935, }, // sceSysconGetPowerStatus
	{ 0x99BBB24C, 0x3B4D7F65, }, // sceSysconCtrlMsPower
	{ 0xF0ED3255, 0xB757C11D, }, // sceSysconCtrlWlanPower
	{ 0xA975F224, 0x9F5E95F1, }, // sceSysconCtrlHddPower
	{ 0x9C4266FC, 0x53CB4AAB, }, // sceSysconCtrlBtPower
	{ 0x3987FEA3, 0x91167903, }, // sceSysconCtrlUsbPower
	{ 0xEAB13FBE, 0x4EB03528, }, // sceSysconPermitChargeBattery
	{ 0xC5075828, 0x027785AD, }, // sceSysconForbidChargeBattery
	{ 0x3C6DB1C5, 0x3ACF24BE, }, // sceSysconCtrlTachyonVmePower
	{ 0xB2558E37, 0x2404C0E1, }, // sceSysconCtrlTachyonAwPower
	{ 0x9478F399, 0xDAE67501, }, // sceSysconCtrlLcdPower
	{ 0x6A53F3F8, 0xC7D9E65C, }, // sceSysconBatteryGetStatusCap
	{ 0xFA4C4518, 0x7EC5EA10, }, // sceSysconBatteryGetInfo
	{ 0xE448FD3F, 0x6BC89A77, }, // sceSysconBatteryNop
	{ 0x70C10E61, 0xCAF3198C, }, // sceSysconBatteryGetTemp
	{ 0x8BDEBB1E, 0x420B593D, }, // sceSysconBatteryGetVolt
	{ 0x373EC933, 0x067EA5AF, }, // sceSysconBatteryGetElec
	{ 0x82861DE2, 0xDB5D6CDE, }, // sceSysconBatteryGetRCap
	{ 0x876CA580, 0x86780127, }, // sceSysconBatteryGetCap
	{ 0x71135D7D, 0x4D31FDB9, }, // sceSysconBatteryGetFullCap
	{ 0x7CBD4522, 0xA9C193D4, }, // sceSysconBatteryGetIFC
	{ 0x284FE366, 0x8AD2A537, }, // sceSysconBatteryGetLimitTime
	{ 0x75025299, 0xD6365013, }, // sceSysconBatteryGetStatus
	{ 0xB5105D51, 0x3250A1E1, }, // sceSysconBatteryGetCycle
	{ 0xD5340103, 0x9CC9302D, }, // sceSysconBatteryGetSerial
	{ 0xB71B98A8, 0xC9E8A328, }, // sceSysconBatteryGetTempAD
	{ 0x87671B18, 0xB593267E, }, // sceSysconBatteryGetVoltAD
	{ 0x75D22BF8, 0xEE57D957, }, // sceSysconBatteryGetElecAD
	{ 0x4C539345, 0xEABC0F0A, }, // sceSysconBatteryGetTotalElec
	{ 0x4C0EE2FA, 0xD617DFDB, }, // sceSysconBatteryGetChargeTime
	{ 0x08DA3752, 0x68C6A2D6, }, // sceSysconCtrlTachyonVoltage
	{ 0x34C36FF9, 0xE30280DA, }, // sceSysconGetDigitalKey
	{ 0x565EF519, 0xE871BDE9, }, // sceSysconGetFallingDetectTime
	{ 0x2B9E6A06, 0x23DA2802, }, // sceSysconGetPowerError
	{ 0x5FF1D610, 0xDC37B6FF, }, // sceSysconSetPollingMode
	{ 0x3357EE5C, 0x6E9ABA7F, }, // sceSysconIsFalling
	{ 0x32CFD20F, 0xA5F9FCB1, }, // sceSysconIsLowBattery
	{ 0xEC0DE439, 0xC01768C4, }, // sceSysconGetPowerSwitch
	{ 0xEA5B9823, 0x0D7477A5, }, // sceSysconIsAlarmed
	{ 0xE0DDFE18, 0x5DED813F, }, // sceSysconGetHPConnect
	{ 0xBDA16E46, 0x801A15E4, }, // sceSysconGetWlanSwitch
	{ 0xA6776FB4, 0xB1386840, }, // sceSysconGetBtSwitch
	{ 0xF6BB4649, 0x475743E5, }, // sceSysconGetHoldSwitch
	{ 0x71AE1BCE, 0x8844B5EE, }, // sceSysconGetHRPowerStatus
	{ 0xF953EF93, 0x8203D89D, }, // sceSysconGetHRWakeupStatus
	{ 0x7016161C, 0xBEDA2264, }, // sceSysconGetWlanPowerStatus
	{ 0x56A73EDD, 0xA2840D70, }, // sceSysconGetBtPowerStatus
	{ 0x48AB0E44, 0x8356229C, }, // sceSysconGetLeptonPowerCtrl
	{ 0x628F2351, 0x9141E498, }, // sceSysconGetMsPowerCtrl
	{ 0xEC37C549, 0xBB56F7CD, }, // sceSysconGetWlanPowerCtrl
	{ 0x051186F3, 0x71C7E788, }, // sceSysconGetHddPowerCtrl
	{ 0xCE76F93E, 0xA85C2DA9, }, // sceSysconGetDvePowerCtrl
	{ 0x8EDF1AD7, 0xCD4741CB, }, // sceSysconGetBtPowerCtrl
	{ 0xEE81C5B1, 0x33F0AEDF, }, // sceSysconGetUsbPowerCtrl
	{ 0x8DDA4CA6, 0x2CE5E02E, }, // sceSysconGetTachyonVmePowerCtrl
	{ 0x52B74B6C, 0x9301E1B1, }, // sceSysconGetTachyonAwPowerCtrl
	{ 0x1B17D3E3, 0xDF5E7FA8, }, // sceSysconGetTachyonAvcPowerCtrl
	{ 0x5F19C00F, 0xF09C2BF3, }, // sceSysconGetLcdPowerCtrl
	{ 0x3C739F57, 0x89C87536, }, // sceSysconGetHRPowerCtrl
	{ 0x72EDA9AF, 0x90DDB37B, }, // sceSysconGetGSensorVersion
	{ 0xC68F1573, 0x12AFA8CB, }, // sceSysconCtrlGSensor
	{ 0x7C8A5503, 0xC0CA6581, }, // sceSysconGetScvCode
//	{ 0x2EE82492, 0x2EE82492, }, // sceSysconCtrlTachyonWDT

	{ 0x64FA0B22, 0xA0BA861F, },
	{ 0x4AB44BFC, 0x6B8C9E3F, },
	{ 0xD653E02E, 0x5B3E7272, },
	{ 0x9E6E060F, 0x62012EAF, },
	{ 0xE00BFC9E, 0x5FD04874, },
	{ 0xC8D97773, 0x64184899, },
	{ 0xE20D08FE, 0xBD0257BB, },
	{ 0x14909C63, 0xFB155B49, },
	{ 0x05727A0B, 0xA53A997C, },
	{ 0x2C03E53E, 0xA75E169B, },
	{ 0xBF458F40, 0x0FAAAA72, },
};

static nid_entry sceUmd_nid[] = {
	{ 0xAE53DC2D, 0xB826BD59, }, // sceUmdClearDriveStatus
	{ 0x230666E3, 0xF60013F8, }, // sceUmdSetDriveStatus
	{ 0x319ED97C, 0x51046712, }, // sceUmdUnRegisterActivateCallBack
	{ 0x086DDC0D, 0xB2AFFC75, }, // sceUmdRegisterActivateCallBack
	{ 0x4832ABF3, 0x4E92D55C, }, // sceUmdRegisterReplaceCallBack
	{ 0x4BA25F4A, 0xDB179672, }, // sceUmdUnRegisterReplaceCallBack
	{ 0xBD2BDE07, 0x51C95C02, }, // sceUmdUnRegisterUMDCallBack
	{ 0xAEE7404D, 0x5083C012, }, // sceUmdRegisterUMDCallBack
	{ 0x46EBB729, 0xE70E8FFB, }, // sceUmdCheckMedium
	{ 0x6AF9B50A, 0x35B13E16, }, // sceUmdCancelWaitDriveStat
	{ 0x87533940, 0xB1641203, }, // sceUmdReplaceProhibit
	{ 0xCBE9F02A, 0x70D9B731, }, // sceUmdReplacePermit
	{ 0x3D0DECD5, 0x6664E727, }, // sceUmdGetUserEventFlagId
	{ 0xD45D1FE6, 0x2D5CB292, }, // sceUmdGetDriveStatus
	{ 0xD01B2DC6, 0xAB586818, }, // sceUmdGetAssignedFlag
	{ 0x3925CBD8, 0x4ECC8FC2, }, // sceUmdSetAssignedFlag
	{ 0x9B22AED7, 0x7E6182C7, }, // sceUmdSetErrorStatus
	{ 0x71F81482, 0x1BA5BAFB, }, // sceUmdGetErrorStatus
	{ 0x7850F057, 0xDA980D66, }, // sceUmdRegisterGetUMDInfoCallBack
	{ 0x27A764A1, 0x569DEE7C, }, // sceUmdUnRegisterGetUMDInfoCallBack
	{ 0x2D81508D, 0x6CA782BF, }, // sceUmdRegisterDeactivateCallBack
	{ 0x56202973, 0x6592E954, }, // sceUmdWaitDriveStatWithTimer
	{ 0x4A9E5E29, 0x74145046, }, // sceUmdWaitDriveStatCB
	{ 0x075F1E0B, 0x63E209AC, }, // sceUmdSetDetectUMDCallBackId
	{ 0x340B7686, 0x9F53CFA1, }, // sceUmdGetDiscInfo
	{ 0xEB56097E, 0x69801EEC, }, // sceUmdGetDetectUMDCallBackId
	{ 0x6A41ED25, 0xE9101A0E, }, // sceUmdGetSuspendResumeMode
	{ 0x4C952ACF, 0x259BDD2E, }, // sceUmdSetSuspendResumeMode
	{ 0xF2D0EDFB, 0xE70E8FFB, }, // sceUmdCheckMedium

	{ 0x659587F7, 0x004F4BE5, },
	{ 0x38503380, 0x31303428, },
	{ 0xF8352373, 0x243C95B5, },
	{ 0x5469DC37, 0xFACF102F, },
};

static nid_entry memlmd_nid[] = {
	{ 0x323366CA, 0x2E208358, },
	{ 0x1570BAB4, 0x8450109F, },
	{ 0x7CF1CD3E, 0xCA560AA6, },
};

static nid_entry sceDdr_driver_nid[] = {
	{ 0x286E1372, 0x37B1BD46, }, // sceDdrSetPowerDownCounter
	{ 0xB5694ECD, 0xFAC28535, }, // sceDdrExecSeqCmd
	{ 0x655A9AB0, 0xCDBED410, }, // sceDdrWriteMaxAllocate
	{ 0x0DC43DE4, 0xF19FD587, }, // sceDdrGetPowerDownCounter
	{ 0xD7C6C313, 0x0B4E8A52, }, // sceDdrSetup
	{ 0x91CD8F94, 0x9B29E314, }, // sceDdrResetDevice
	{ 0x4CE55E76, 0x25AB44F3, }, // sceDdrChangePllClock

	{ 0x6078F911, 0xEF5E4465, },
	{ 0x5B5831E5, 0xD48CC758, },
	{ 0xF23B7983, 0xBD40FC26, },
	{ 0x6955346C, 0x8D5170F3, },
	{ 0x888129E8, 0xB8D2E273, },
	{ 0x7251F5AB, 0xD8910A05, },
};

static nid_entry sceDmacplus_driver_nid[] = {
	{ 0x29B50A82, 0x6CDDF704, }, // sceDmacplusLcdcSetBaseAddr
	{ 0x8AE579CD, 0x82DC94B2, }, // sceDmacplusLcdcSetFormat
	{ 0x241601AE, 0x80374042, }, // sceDmacplusLcdcGetFormat
	{ 0xF8C7C180, 0xD9003581, }, // sceDmacplusLcdcEnable
	{ 0x576522BC, 0x153E82A4, }, // sceDmacplusLcdcDisable
	{ 0x3A98EE05, 0xD455CC19, }, // sceDmacplusAvcSync
	{ 0x4B980588, 0xF760E230, }, // sceDmacplusSc2MeNormal16
	{ 0x3438DA0B, 0x4786526C, }, // sceDmacplusSc2MeLLI
	{ 0x58DE4914, 0x1110A88F, }, // sceDmacplusSc2MeSync
	{ 0xBE693828, 0xE94BCE36, }, // sceDmacplusMe2ScNormal16
	{ 0xAB49D2CB, 0x148E1147, }, // sceDmacplusMe2ScSync
	{ 0x2D5940FF, 0xDC0A1A1D, }, // sceDmacplusMe2ScLLI
	{ 0x28558DBF, 0x3FD8340C, }, // sceDmacTryMemcpy
	{ 0xFD183BCF, 0x1801A63E, }, // sceDmacplusSc128LLI
	{ 0x58C380BB, 0x8E39FD88, }, // sceDmacplusLcdcGetBaseAddr
	{ 0xC7D33466, 0x558CDFAB, }, // sceDmacMemcpy

	{ 0x1D73DDA8, 0x7CD63CBE, },
	{ 0x74323807, 0x4E875FE8, },
	{ 0xB2EFA681, 0x23452082, },
};

static nid_entry sceGpio_driver_nid[] = {
	{ 0xCA8BE2EA, 0xD5469E56, }, // sceGpioGetPortMode
	{ 0x37C8DADC, 0x317D9D2C, }, // sceGpioSetIntrMode
	{ 0xF856CE46, 0xDB4F5CE2, }, // sceGpioGetIntrMode
	{ 0x95135905, 0xE7BDAF50, }, // sceGpioPortInvert
	{ 0x785206CD, 0xB0D23234, }, // sceGpioEnableIntr
	{ 0x31F34AE6, 0x856D950A, }, // sceGpioQueryIntr
	{ 0xBE77D1D0, 0x7AD284FD, }, // sceGpioAcquireIntr
	{ 0xC6928224, 0xE8A497DE, }, // sceGpioGetCapturePort
	{ 0x6B38B826, 0xEBF28F41, }, // sceGpioSetCapturePort
	{ 0x5691CEFA, 0xD2A8B1B5, }, // sceGpioEnableTimerCapture
	{ 0x2CDC8EDC, 0xEF344659, }, // sceGpioDisableTimerCapture
};

static nid_entry sceSysreg_driver_nid[] = {
	{ 0xC29D614E, 0x9EF98522, }, // sceSysregTopResetEnable
	{ 0xDC6139A4, 0x3B286D7A, }, // sceSysregScResetEnable
	{ 0xDE59DACB, 0x97EA6913, }, // sceSysregMeResetEnable
	{ 0x2DB0EB28, 0x2D858336, }, // sceSysregMeResetDisable
	{ 0x26283A6F, 0x3CC1D73F, }, // sceSysregAwResetEnable
	{ 0xA374195E, 0x2F353BF5, }, // sceSysregAwResetDisable
	{ 0xD20581EA, 0xA9A2F237, }, // sceSysregVmeResetEnable
	{ 0x7558064A, 0x67557011, }, // sceSysregVmeResetDisable
	{ 0x9BB70D34, 0x119647A7, }, // sceSysregAvcResetEnable
	{ 0xFD6C562B, 0x85C424F9, }, // sceSysregAvcResetDisable
	{ 0xCD4FB614, 0xDA2E7D06, }, // sceSysregUsbResetEnable
	{ 0x69EECBE5, 0xED6EF130, }, // sceSysregUsbResetDisable
	{ 0xF5B80837, 0xC572A362, }, // sceSysregAtaResetEnable
	{ 0x8075303F, 0x5F01289B, }, // sceSysregAtaResetDisable
	{ 0x00C2628E, 0xE6A812AA, }, // sceSysregMsifResetEnable
	{ 0xEC4BF81F, 0xF81EC5B9, }, // sceSysregMsifResetDisable
	{ 0x8A7F9EB4, 0xF7A8BFA5, }, // sceSysregKirkResetEnable
	{ 0xC32F2491, 0x43EEDE50, }, // sceSysregKirkResetDisable
	{ 0xA23BC2C4, 0xE40DC039, }, // sceSysregAtahddResetEnable
	{ 0xF5EA8570, 0x1CAB8282, }, // sceSysregAtahddResetDisable
	{ 0x3E961C02, 0x48546F11, }, // sceSysregUsbhostResetEnable
	{ 0xACFA3764, 0xB03747DB, }, // sceSysregUsbhostResetDisable
	{ 0x44F6CDA7, 0x2B3BE2C8, }, // sceSysregMeBusClockEnable
	{ 0x158AD4FC, 0x774775CA, }, // sceSysregMeBusClockDisable
	{ 0x4D4CE2B8, 0xDF06373E, }, // sceSysregAwRegABusClockEnable
	{ 0x789BD609, 0x9EBD0CD2, }, // sceSysregAwRegABusClockDisable
	{ 0x391CE1C0, 0x6637667D, }, // sceSysregAwRegBBusClockEnable
	{ 0x82D35024, 0x5AB2CF90, }, // sceSysregAwRegBBusClockDisable
	{ 0xAF904657, 0x739A33F0, }, // sceSysregAwEdramBusClockEnable
	{ 0x438AECE9, 0xA3C7B0C1, }, // sceSysregAwEdramBusClockDisable
	{ 0x4F5AFBBE, 0x0FE3B0D7, }, // sceSysregKirkBusClockEnable
	{ 0x845DD1A6, 0x2AF6987B, }, // sceSysregKirkBusClockDisable
	{ 0x3E216017, 0xBA3FADB4, }, // sceSysregUsbBusClockEnable
	{ 0xBFBABB63, 0x313DD64A, }, // sceSysregUsbBusClockDisable
	{ 0x4716E71E, 0x3F36F3B1, }, // sceSysregMsifBusClockEnable
	{ 0x826430C0, 0x7F2571B7, }, // sceSysregMsifBusClockDisable
	{ 0x7CC6CBFD, 0x1B6F77C0, }, // sceSysregEmcddrBusClockEnable
	{ 0xEE6B9411, 0xE95E59A1, }, // sceSysregEmcddrBusClockDisable
	{ 0x63B9EB65, 0xA8235369, }, // sceSysregApbBusClockEnable
	{ 0xE1AA9788, 0x1663DA05, }, // sceSysregApbBusClockDisable
	{ 0xAA63C8BD, 0xE7849809, }, // sceSysregAudioBusClockEnable
	{ 0x054AC8C6, 0x44C4DA5F, }, // sceSysregAudioBusClockDisable
	{ 0xE45BED6A, 0x77831D41, }, // sceSysregAtahddBusClockEnable
	{ 0x681B35C4, 0xB2B17B0F, }, // sceSysregAtahddBusClockDisable
	{ 0xE321F41A, 0xB85674BC, }, // sceSysregUsbhostBusClockEnable
	{ 0x4D2FFC60, 0x5EAFED61, }, // sceSysregUsbhostBusClockDisable
	{ 0x6B01D71B, 0xEF64EFDD, }, // sceSysregAtaClkEnable
	{ 0xFC186A83, 0xF6922705, }, // sceSysregAtaClkDisable
	{ 0xB59DB832, 0x0E441BEB, }, // sceSysregAtahddClkEnable
	{ 0x9155812C, 0x91616908, }, // sceSysregAtahddClkDisable
	{ 0xDA4FCA1D, 0x39FF613D, }, // sceSysregUsbhostClkEnable
	{ 0x228A73E1, 0x53F6D550, }, // sceSysregUsbhostClkDisable
	{ 0x31154490, 0xCA901512, }, // sceSysregMsifClkEnable
	{ 0x8E2D835D, 0x046FE30E, }, // sceSysregMsifClkDisable
	{ 0xE8533DCA, 0x565B8650, }, // sceSysregApbTimerClkEnable
	{ 0xF6D83AD0, 0xC4C1F135, }, // sceSysregApbTimerClkDisable
	{ 0xA9CD1C1F, 0xB733A263, }, // sceSysregAudioClkEnable
	{ 0x2F216F38, 0x0DD07A50, }, // sceSysregAudioClkDisable
	{ 0xE3AECFFA, 0x14D3ACBC, }, // sceSysregGpioClkEnable
	{ 0x3BBD0C0C, 0x7BFA7EEA, }, // sceSysregGpioClkDisable
	{ 0xC1DA05D2, 0x34EED467, }, // sceSysregAudioClkoutClkEnable
	{ 0xDE170397, 0x5122CA96, }, // sceSysregAudioClkoutClkDisable
	{ 0x7234EA80, 0xEB5ADBE4, }, // sceSysregUsbClkEnable
	{ 0x38EC3281, 0x4D23058C, }, // sceSysregUsbClkDisable
	{ 0xBC68D9B6, 0xF5509B44, }, // sceSysregUsbIoEnable
	{ 0xA3C8E075, 0x14E314B0, }, // sceSysregUsbIoDisable
	{ 0x79338EA3, 0x2C0E5FA5, }, // sceSysregAtaIoEnable
	{ 0xCADB92AA, 0x25EA91E5, }, // sceSysregAtaIoDisable
	{ 0xD74F1D48, 0xE4D3F01F, }, // sceSysregMsifIoEnable
	{ 0x18172C6A, 0xEBBD8E0C, }, // sceSysregMsifIoDisable
	{ 0xBB26CF1F, 0x5D3DD0B3, }, // sceSysregAudioIoEnable
	{ 0x8E2FB536, 0x266F1C65, }, // sceSysregAudioIoDisable
	{ 0xF844DDF3, 0x94914740, }, // sceSysregAudioClkoutIoEnable
	{ 0x29A119A1, 0xA25740B8, }, // sceSysregAudioClkoutIoDisable
	{ 0x8B95C17F, 0x87176F00, }, // sceSysregAtahddIoEnable
	{ 0xCCF911C0, 0xB50B1767, }, // sceSysregAtahddIoDisable
	{ 0xB627582E, 0xE932D752, }, // sceSysregGpioIoEnable
	{ 0x1E9C3607, 0x73B789C9, }, // sceSysregGpioIoDisable
	{ 0x633595F2, 0x13E2ACB2, }, // sceSysregUsbGetConnectStatus
	{ 0x32471457, 0xECFBBCB5, }, // sceSysregUsbQueryIntr
	{ 0x692F31FF, 0x4100075B, }, // sceSysregUsbAcquireIntr
	{ 0xFFEB6E00, 0x8E51EE5A, }, // sceSysregUsbhostQueryIntr
	{ 0x87C2BA20, 0x26BBB4C8, }, // sceSysregUsbhostAcquireIntr
	{ 0xD43E98F6, 0x6EFCF152, }, // sceSysregMsifGetConnectStatus
	{ 0xBF91FBDA, 0x2B3033AD, }, // sceSysregMsifQueryConnectIntr
	{ 0x36A75390, 0x68F3EA50, }, // sceSysregMsifAcquireConnectIntr
	{ 0x61BFF85F, 0x3620CA9E, }, // sceSysregInterruptToOther
	{ 0x9FC87ED4, 0x4643F826, }, // sceSysregSemaTryLock
	{ 0x8BE2D520, 0x314A489C, }, // sceSysregSemaUnlock
	{ 0xB21B6CBF, 0x181F6AFF, }, // sceSysregPllGetFrequency
	{ 0xB4560C45, 0xD47B7BE0, }, // sceSysregPllGetOutSelect
	{ 0xDCA57573, 0x81667490, }, // sceSysregPllSetOutSelect
	{ 0x1969E840, 0xBDB91784, }, // sceSysregMsifClkSelect
	{ 0x1D382514, 0x8F046FDE, }, // sceSysregMsifDelaySelect
	{ 0x03340297, 0x35AF8BE3, }, // sceSysregApbTimerClkSelect
	{ 0x9100B4E5, 0x9EBB3C43, }, // sceSysregAudioClkSelect
	{ 0x833E6FB1, 0x55528C0B, }, // sceSysregAtaClkSelect
	{ 0x8CE2F97A, 0x5E45F850, }, // sceSysregAtahddClkSelect
	{ 0x0A83FC7B, 0x47966D28, }, // sceSysregAudioClkoutClkSelect
	{ 0x3EA188AD, 0xB13D48CD, }, // sceSysregRequestIntr
	{ 0xCD0F6715, 0x2737D937, }, // sceSysregDoTimerEvent
	{ 0xF3443B9A, 0x0D0AE159, }, // sceSysregSetAwEdramSize
	{ 0x53A6838B, 0xA684D1B0, }, // sceSysregPllGetBaseFrequency
	{ 0xBB3623DF, 0xA0F3053B, }, // sceSysregPllUpdateFrequency
	{ 0x25673620, 0x7ADF93F1, }, // sceSysregIntrInit
	{ 0x4EE8E2C8, 0x8F224A0F, }, // sceSysregIntrEnd
	{ 0x083F56E2, 0x787CCE85, }, // sceSysregEnableIntr
	{ 0x7C5B543C, 0xBBE75ABD, }, // sceSysregDisableIntr
	{ 0xA9997109, 0x97EA6913, }, // sceSysregMeResetEnable(371)
	{ 0x76220E94, 0x2D858336, }, // sceSysregMeResetDisable(371)
	{ 0x3199CF1C, 0x2B3BE2C8, }, // sceSysregMeBusClockEnable(371)
	{ 0x07881A0B, 0x774775CA, }, // sceSysregMeBusClockDisable(371)

	{ 0x55B18B84, 0x75857874, },
	{ 0x2112E686, 0xF450CD9F, },
	{ 0x7B9E9A53, 0x1CB5AE24, },
	{ 0x7BDF0556, 0x18D9BFAB, },
	{ 0x8D0FED1E, 0x371C2599, },
	{ 0xA46E9CA8, 0xC4330C30, },
	{ 0xB08A4E85, 0x5178FB81, },
	{ 0xA41929DA, 0x290BE4E7, },
	{ 0xE88B77ED, 0x73CC040F, },
	{ 0x312F9419, 0x0A492798, },
	{ 0x844AF6BD, 0xE58D8630, },
	{ 0xEE8760C6, 0x91143B9B, },
	{ 0x5664F8B5, 0xE4CB1700, },
	{ 0x44704E1D, 0xDC17E0E9, },
	{ 0x584AD989, 0x22EB25C4, },
	{ 0x377F035F, 0x51938C20, },
	{ 0xAB3185FD, 0x73DF64ED, },
	{ 0x0EA487FA, 0x175283DF, },
	{ 0x136E8F5A, 0xC5076887, },
	{ 0xF4811E00, 0xE977730A, },
};

static nid_entry sceDve_driver_nid[] = {
	{ 0x0836B2B5, 0x9E03BCF9, }, // sceDvePowerOn
	{ 0x17BC42E4, 0x5BC934E9, }, // sceDvePowerOff
	{ 0xADE8C471, 0xE5EE11B3, }, // sceDveResetEnable
	{ 0xD986F31B, 0x50005CD3, }, // sceDveResetDisable
	{ 0xB7827F16, 0x46C595A3, }, // sceDveUnk1
	{ 0x34E9274D, 0xE926B3B8, }, // sceDveUnk2
	{ 0xE84E975E, 0xAB7574D1, }, // sceDveUnk3
	{ 0x0B85524C, 0x194FE107, }, // sceDveUnk4
	{ 0xA265B504, 0x024D7064, }, // sceDveUnk5
	{ 0xAE2A6570, 0x7C77EDA7, }, // sceDveDisplayOn
	{ 0xD8863512, 0xDB6BCBD6, }, // sceDveUnk10
	{ 0xA2D53F99, 0x46E070DD, }, // sceDveDisplayOff
	{ 0xF0CF5A67, 0x0F748EAC, }, // sceDveUnk12
	{ 0xBF910381, 0xAA503020, }, // sceDveUnk11
	{ 0x36970008, 0x096FA2AE, }, // sceDveUnk7
	{ 0x59E1B113, 0xF1175122, }, // sceDveUnk8
	{ 0xC9362C91, 0x9FC8CDA7, }, // sceDveGetDisplayMode
	{ 0x22D50DA7, 0xBD65CFAE, }, // sceDveInit
	{ 0x292F15DA, 0x762C12FE, }, // sceDveUnk9

	{ 0xB2E4E437, 0x4210EB3C, },
	{ 0xDEB2F80C, 0xA1401BA2, },
	{ 0x93828323, 0x2F863B65, },
};

static nid_entry sceHprm_driver_nid[] = {
	{ 0xC7154136, 0xE611E89B, }, // sceHprmRegisterCallback
	{ 0xFD7DE6CD, 0xBF4C183C, }, // sceHprmUnregitserCallback
	{ 0x8EC787E0, 0x0CB055CC, }, // sceHprmUpdateCableType
	{ 0xBAD0828E, 0x732EC325, }, // sceHprmGetModel
	{ 0x4D1E622C, 0x1F631BD7, }, // sceHprmReset
	{ 0x2BCEC83E, 0x00CFE490, }, // sceHprmPeekLatch
	{ 0x40D2F9F0, 0x5FC5E53B, }, // sceHprmReadLatch
	{ 0x7B038374, 0x34906CFF, }, // sceHprmGetInternalState
	{ 0xF04591FA, 0x5CE3A57B, }, // sceHprmSetIdleCancelKey
	{ 0x971AE8FB, 0xEC254B64, }, // sceHprmGetIdleCancelKey
	{ 0x71B5FB67, 0x8A898B38, }, // sceHprmGetHpDetect
	{ 0x7E69EDA4, 0x2E49469C, }, // sceHprmIsHeadphoneExist
	{ 0x208DB1BD, 0x4C9BEDA4, }, // sceHprmIsRemoteExist
	{ 0x219C58F1, 0xE0F76A26, }, // sceHprmIsMicrophoneExist
	{ 0xC1777D84, 0x62CDFB12, }, // sceHprmGetCableType
	{ 0xD22913DB, 0x1EECCA1A, }, // sceHprmSetConnectCallback

	{ 0x9C1D9ADB, 0xDA01152D, },
	{ 0xB6B6786B, 0x61650073, },
	{ 0x21E8AFD5, 0xEAE19005, },
	{ 0x6BC7BEE9, 0xCDACC031, },
	{ 0xC0FE0FAD, 0xC61FC867, },
	{ 0x7B1A14B8, 0x2F028F97, },
	{ 0x1528D408, 0x61650073, },
	{ 0x133DE8FA, 0x74B54AEA, },
};

static nid_entry sceMesgLed_driver_nid[] = {
	{ 0xDFF0F308, 0x5D799BBF, },
};

static nid_entry sceClockgen_driver_nid[] = {
	{ 0x4EB657D5, 0xACC30ECE, }, // sceClockgenAudioClkSetFreq
};

static nid_entry sceCodec_driver_nid[] = {
	{ 0x20C61103, 0xBCE58DC4, }, // sceCodecSelectVolumeTable
	{ 0xEEB91526, 0x70C56A98, }, // sceCodecSetVolumeOffset
	{ 0x6D945509, 0x0BA1AF49, }, // sceCodecSetHeadphoneVolume
	{ 0x40D5C897, 0x8E11D696, }, // sceCodecSetSpeakerVolume
};

static nid_entry scePaf_nid[] = {
	{ 0x2BE8DDBB, 0xE9411837, },
	{ 0xE8CCC611, 0x85D1CB6B, },
	{ 0xCDDCFFB3, 0x4B2F9A4F, },
	{ 0x48BB05D5, 0xF0D98BD1, },
	{ 0x22FB4177, 0xC9B72C40, },
	{ 0xBC8DC92B, 0x5F73A09B, },
	{ 0xE3D530AE, 0x4900119B, },
};

resolver_config nid_620_fix[] = {
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
	NID_ENTRY(SysTimerForKernel),
	NID_ENTRY(sceImpose_driver),
	NID_ENTRY(sceGe_driver),
	NID_ENTRY(sceRtc_driver),
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

u32 nid_620_fix_size = NELEMS(nid_620_fix);
