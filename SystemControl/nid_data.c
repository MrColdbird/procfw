#include <pspsdk.h>
#include "nid_resolver.h"
#include "utils.h"

static nid_entry SysMemForKernel_nid[] = {
	{ 0xC7E57B9C, 0x9F154FA1, },
	{ 0xCF4DE78C, 0x208F4820, }, // sceKernelGetUIDcontrolBlock
	{ 0xFC114573, 0xF0E0AB7A, },
	{ 0x3FC9AE6A, 0x5E8DCA05, }, // sceKernelDevkitVersion
	{ 0x536AD5E1, 0xF3C729A6, }, // SysMemForKernel_536AD5E1
	{ 0x636C953B, 0x6D161EE2, }, // sceKernelAllocHeapMemory
	{ 0xC9805775, 0xF9475C1A, }, // sceKernelDeleteHeap 
	{ 0x7B749390, 0xDB836ADB, }, // sceKernelFreeHeapMemory
	{ 0x1C1FBFE7, 0xAF85EB1B, }, // sceKernelCreateHeap
	{ 0x55A40B2C, 0xF5E82409, }, // sceKernelQueryMemoryPartitionInfo
	{ 0xE6581468, 0xE10F21CF, }, // sceKernelPartitionMaxFreeMemSize
	{ 0x9697CD32, 0x7BE9653E, }, // sceKernelPartitionTotalFreeMemSize 0xE10F21CF??? From Davee
	{ 0x237DBD4F, 0x4621A9CC, }, // sceKernelAllocPartitionMemory
	{ 0xB6D61D02, 0x8FDAFC4C, }, // sceKernelFreePartitionMemory
	{ 0x9D9A5BA1, 0xFA5025B5, }, // sceKernelGetBlockHeadAddr
	{ 0xB2C7AA36, 0x00E9A04A, }, // sceKernelSetDdrMemoryProtection
	{ 0x6373995D, 0x458A70B5, }, // sceKernelGetModel
	{ 0xEB7A74DB, UNKNOWNNID, }, // sceKernelAllocHeapMemoryWithOption
	{ 0xA823047E, UNKNOWNNID, }, // sceKernelHeapTotalFreeSize
	{ 0xB2163AA1, UNKNOWNNID, }, // sceKernelGetHeapTypeCB
	{ 0xEFF0C6DD, UNKNOWNNID, }, // SysMemForKernel_EFF0C6DD
	{ 0xEFEEBAC7, UNKNOWNNID, }, // SysMemForKernel_EFEEBAC7
	{ 0x2DB687E9, UNKNOWNNID, }, // sceKernelIsValidHeap
	{ 0xA2A65F0E, UNKNOWNNID, }, // sceKernelFillFreeBlock
	{ 0xEE867074, UNKNOWNNID, }, // sceKernelSizeLockMemoryBlock
	{ 0xCE5544F4, UNKNOWNNID, }, // sceKernelResizeMemoryBlock
	{ 0x915EF4AC, UNKNOWNNID, }, // SysMemForKernel_915EF4AC
	{ 0x5EBE73DE, UNKNOWNNID, }, // sceKernelJointMemoryBlock
	{ 0x2A3E5280, UNKNOWNNID, }, // sceKernelQueryMemoryInfo
	{ 0xEB4C0E1B, UNKNOWNNID, }, // sceKernelQueryBlockSize
	{ 0x82CCA14F, UNKNOWNNID, }, // sceKernelQueryMemoryBlockInfo
	{ 0x2F3B7611, UNKNOWNNID, }, // SysMemForKernel_2F3B7611
	{ 0x7B3E7441, UNKNOWNNID, }, // SysMemForKernel_7B3E7441
	{ 0x6C1DCD41, UNKNOWNNID, }, // sceKernelCallUIDFunction
	{ 0x5367923C, UNKNOWNNID, }, // sceKernelCallUIDObjFunction
	{ 0xCE05CCB7, UNKNOWNNID, }, // SysMemForKernel_CE05CCB7
	{ 0x6CD838EC, UNKNOWNNID, }, // sceKernelLookupUIDFunction
	{ 0xAD09C397, UNKNOWNNID, }, // sceKernelCreateUIDtypeInherit
	{ 0xFEFC8666, UNKNOWNNID, }, // sceKernelCreateUIDtype
	{ 0xD1BAB054, UNKNOWNNID, }, // sceKernelDeleteUIDtype
	{ 0x1C221A08, UNKNOWNNID, }, // sceKernelGetUIDname
	{ 0x2E3402CC, UNKNOWNNID, }, // sceKernelRenameUID
	{ 0x39357F07, UNKNOWNNID, }, // sceKernelGetUIDtype
	{ 0x89A74008, UNKNOWNNID, }, // sceKernelCreateUID
	{ 0x8F20C4C0, UNKNOWNNID, }, // sceKernelDeleteUID
	{ 0x55BFD686, UNKNOWNNID, }, // sceKernelSearchUIDbyName
	{ 0x41FFC7F9, UNKNOWNNID, }, // sceKernelGetUIDcontrolBlockWithType
	{ 0x82D3CEE3, UNKNOWNNID, }, // SysMemForKernel_82D3CEE3
	{ 0xFC207849, UNKNOWNNID, }, // SysMemForKernel_FC207849
	{ 0xDFAEBD5B, UNKNOWNNID, }, // sceKernelIsHold
	{ 0x7BE95FA0, UNKNOWNNID, }, // sceKernelHoldUID
	{ 0xFE8DEBE0, UNKNOWNNID, }, // sceKernelReleaseUID
	{ 0xBD5941B4, UNKNOWNNID, }, // sceKernelSysmemIsValidAccess
	{ 0x80F25772, UNKNOWNNID, }, // sceKernelIsValidUserAccess
	{ 0x3905D956, UNKNOWNNID, }, // sceKernelSysMemCheckCtlBlk
	{ 0x26F96157, UNKNOWNNID, }, // sceKernelSysMemDump
	{ 0x6D6200DD, UNKNOWNNID, }, // sceKernelSysMemDumpBlock
	{ 0x621037F5, UNKNOWNNID, }, // sceKernelSysMemDumpTail
	{ 0xA089ECA4, UNKNOWNNID, }, // sceKernelMemset
	{ 0x2F808748, UNKNOWNNID, }, // SysMemForKernel_2F808748
	{ 0x1C4B1713, UNKNOWNNID, }, // SysMemForKernel_1C4B1713
	{ 0xE400FDB0, UNKNOWNNID, }, // sceKernelSysMemInit
	{ 0x1890BE9C, UNKNOWNNID, }, // sceKernelSysMemMemSize
	{ 0x03072750, UNKNOWNNID, }, // sceKernelSysMemMaxFreeMemSize
	{ 0x811BED79, UNKNOWNNID, }, // sceKernelSysMemTotalFreeMemSize
	{ 0xF6C10E27, UNKNOWNNID, }, // sceKernelGetSysMemoryInfo
	{ 0xCDA3A2F7, UNKNOWNNID, }, // SysMemForKernel_CDA3A2F7
	{ 0x960B888C, UNKNOWNNID, }, // SysMemForKernel_960B888C
	{ 0x452E3696, UNKNOWNNID, }, // SysMemForKernel_452E3696
	{ 0x95F5E8DA, UNKNOWNNID, }, // SysMemForKernel_95F5E8DA
};

static nid_entry LoadCoreForKernel_nid[] = {
	{ 0xCF8A41B1, 0xEF8A0BEA, }, // sceKernelFindModuleByName
	{ 0xCCE4A157, 0xED53894F, }, // sceKernelFindModuleByUID
	{ 0xDD303D79, 0x312CA47E, }, // LoadCoreForKernel_DD303D79 ( TODO: actually in 5.XX)
	{ 0xFB8AE27D, 0x312CA47E, }, // sceKernelFindModuleByAddress (3.xx NID)
	{ 0xACE23476, UNKNOWNNID, }, // sceKernelCheckPspConfig
	{ 0x7BE1421C, UNKNOWNNID, }, // sceKernelCheckExecFile
	{ 0xBF983EF2, UNKNOWNNID, }, // sceKernelProbeExecutableObject
	{ 0x7068E6BA, UNKNOWNNID, }, // sceKernelLoadExecutableObject
	{ 0xB4D6FECC, UNKNOWNNID, }, // sceKernelApplyElfRelSection
	{ 0x54AB2675, UNKNOWNNID, }, // sceKernelApplyPspRelSection
	{ 0x2952F5AC, UNKNOWNNID, }, // sceKernelDcacheWBinvAll
	{ 0xD8779AC6, 0x8D46E9DF, }, // sceKernelIcacheClearAll
	{ 0x99A695F0, UNKNOWNNID, }, // sceKernelRegisterLibrary
	{ 0x5873A31F, UNKNOWNNID, }, // sceKernelRegisterLibraryForUser
	{ 0x0B464512, UNKNOWNNID, }, // sceKernelReleaseLibrary
	{ 0x9BAF90F6, UNKNOWNNID, }, // sceKernelCanReleaseLibrary
	{ 0x0E760DBA, UNKNOWNNID, }, // sceKernelLinkLibraryEntries
	{ 0x0DE1F600, UNKNOWNNID, }, // sceKernelLinkLibraryEntriesForUser
	{ 0xDA1B09AA, UNKNOWNNID, }, // sceKernelUnLinkLibraryEntries
	{ 0xC99DD47A, UNKNOWNNID, }, // sceKernelQueryLoadCoreCB
	{ 0x616FCCCD, UNKNOWNNID, }, // LoadCoreForKernel_616FCCCD
	{ 0xF32A2940, UNKNOWNNID, }, // sceKernelModuleFromUID
	{ 0xCD0F3BAC, UNKNOWNNID, }, // sceKernelCreateModule
	{ 0x6B2371C2, UNKNOWNNID, }, // sceKernelDeleteModule
	{ 0x7320D964, UNKNOWNNID, }, // sceKernelModuleAssign
	{ 0x44B292AB, UNKNOWNNID, }, // sceKernelAllocModule
	{ 0xBD61D4D5, UNKNOWNNID, }, // sceKernelFreeModule
	{ 0xAE7C6E76, UNKNOWNNID, }, // sceKernelRegisterModule
	{ 0x74CF001A, UNKNOWNNID, }, // sceKernelReleaseModule
	{ 0x82CE54ED, UNKNOWNNID, }, // sceKernelModuleCount
	{ 0xC0584F0C, UNKNOWNNID, }, // sceKernelGetModuleList
};

static nid_entry LoadExecForKernel_nid[] = {
	{ 0x6D302D3D, 0xFCD765C9, }, // sceKernelExitVSHKernel
	{ 0x28D0D249, 0x3D805DE6, }, // sceKernelLoadExecVSHMs2
	{ 0xA3D5E142, 0x5AA1A6D2, }, // sceKernelExitVSHVSH
	{ 0x1B97BDB3, 0x45C6125B, }, // sceKernelLoadExecVSHDisc(3.xx)
	{ 0x94A1C627, 0x5AA1A6D2, }, // sceKernelExitVSHVSH
	{ 0xBD2F1094, UNKNOWNNID, }, // sceKernelLoadExec
	{ 0x2AC9954B, UNKNOWNNID, }, // sceKernelExitGameWithStatus
	{ 0x05572A5F, UNKNOWNNID, }, // sceKernelExitGame
	{ 0xAC085B9E, UNKNOWNNID, }, // sceKernelLoadExecVSHFromHost
	{ 0x821BE114, 0x179D905A, }, // sceKernelLoadExecVSHDiscUpdater
	{ 0x015DA036, UNKNOWNNID, }, // sceKernelLoadExecBufferVSHUsbWlan
	{ 0x4F41E75E, UNKNOWNNID, }, // sceKernelLoadExecBufferVSHUsbWlanDebug
	{ 0x31DF42BF, 0x7286CF0B, }, // sceKernelLoadExecVSHMs1
	{ 0x70901231, 0xBAEB4B89, }, // sceKernelLoadExecVSHMs3
	{ 0x62C459E1, UNKNOWNNID, }, // sceKernelLoadExecBufferVSHPlain
	{ 0x918782E8, UNKNOWNNID, }, // sceKernelLoadExecBufferVSHFromHost
	{ 0xBB28E9B7, UNKNOWNNID, }, // sceKernelLoadExecBufferPlain0
	{ 0x71A1D738, UNKNOWNNID, }, // sceKernelLoadExecBufferPlain
	{ 0x4D5C75BE, UNKNOWNNID, }, // sceKernelLoadExecFromHost
	{ 0x4AC57943, 0xE9B45481, }, // sceKernelRegisterExitCallback
	{ 0xD9739B89, UNKNOWNNID, }, // sceKernelUnregisterExitCallback
	{ 0x659188E1, UNKNOWNNID, }, // sceKernelCheckExitCallback
	{ 0x62A27008, UNKNOWNNID, }, // sceKernelInvokeExitCallback
	{ 0x7B7C47EF, UNKNOWNNID, }, // sceKernelLoadExecVSHDiscDebug
	{ 0x061D9514, 0x8EF38192, }, // sceKernelLoadExecVSHMs4
	{ 0xB7AB08DA, UNKNOWNNID, }, // sceKernelLoadExecVSHMs5
	{ 0x11412288, UNKNOWNNID, }, // LoadExecForKernel_11412288
};

static nid_entry ModuleMgrForKernel_nid[] = {
	{ 0x977DE386, 0xFFB9B760, }, // sceKernelLoadModule
	{ 0x50F0C1EC, 0xE6BF3960, }, // sceKernelStartModule
	{ 0xD1FF982A, 0x4848E645, }, // sceKernelStopModule
	{ 0x2E0911AA, 0x0D053026, }, // sceKernelUnloadModule
	{ 0x644395E2, 0xA95C26C8, }, // sceKernelGetModuleIdList
	{ 0xA1A78C58, 0x4986499C, }, // sceKernelLoadModuleDisc
	{ 0x748CBED9, 0xEE3176DD, }, // sceKernelQueryModuleInfo
	{ 0xD675EBB8, 0xD699583C, }, // sceKernelSelfStopUnloadModule
	{ 0xABE84F8A, UNKNOWNNID, }, // sceKernelLoadModuleBufferWithApitype
	{ 0xBA889C07, UNKNOWNNID, }, // sceKernelLoadModuleBuffer
	{ 0xB7F46618, UNKNOWNNID, }, // sceKernelLoadModuleByID
	{ 0x437214AE, UNKNOWNNID, }, // sceKernelLoadModuleWithApitype
	{ 0x710F61B5, UNKNOWNNID, }, // sceKernelLoadModuleMs
	{ 0x91B87FAE, UNKNOWNNID, }, // sceKernelLoadModuleVSHByID
	{ 0xA4370E7C, UNKNOWNNID, }, // sceKernelLoadModuleVSH
	{ 0x23425E93, UNKNOWNNID, }, // sceKernelLoadModuleVSHPlain
	{ 0xF9275D98, UNKNOWNNID, }, // sceKernelLoadModuleBufferUsbWlan
	{ 0xF0CAC59E, UNKNOWNNID, }, // sceKernelLoadModuleBufferVSH
	{ 0xCC1D3699, 0x291CF03E, }, // sceKernelStopUnloadSelfModule
	{ 0x04B7BD22, UNKNOWNNID, }, // sceKernelSearchModuleByName
	{ 0x54D9E02E, UNKNOWNNID, }, // sceKernelSearchModuleByAddress
	{ 0x5F0CC575, UNKNOWNNID, }, // sceKernelRebootBeforeForUser
	{ 0xB49FFB9E, UNKNOWNNID, }, // sceKernelRebootBeforeForKernel
	{ 0xF0A26395, 0x8B19C181, }, // sceKernelGetModuleId
	{ 0xD8B73127, 0xAD633D71, }, // sceKernelGetModuleIdByAddress
};

static nid_entry ExceptionManagerForKernel_nid[] = {
	{ 0x565C0B0E, 0x46EA27C3, }, // sceKernelRegisterDefaultExceptionHandler
	{ 0x3FB264FC, UNKNOWNNID, }, // sceKernelRegisterExceptionHandler
	{ 0x5A837AD4, UNKNOWNNID, }, // sceKernelRegisterPriorityExceptionHandler
	{ 0x1AA6CFFA, UNKNOWNNID, }, // sceKernelReleaseExceptionHandler
	{ 0xDF83875E, UNKNOWNNID, }, // sceKernelGetActiveDefaultExceptionHandler
	{ 0x291FF031, UNKNOWNNID, }, // sceKernelReleaseDefaultExceptionHandler
	{ 0x15ADC862, UNKNOWNNID, }, // sceKernelRegisterNmiHandler
	{ 0xB15357C9, UNKNOWNNID, }, // sceKernelReleaseNmiHandler
};

static nid_entry IoFileMgrForKernel_nid[] = {
	{ 0x3C54E908, 0x421B8EB4, }, // sceIoReopen 
	{ 0x411106BA, 0x74482CE3, }, // sceIoGetThreadCwd
	{ 0xA905B705, 0x08CB99E3, }, // sceIoCloseAll
	{ 0xCB0A151F, UNKNOWNNID, }, // sceIoChangeThreadCwd
};

static nid_entry scePower_driver_nid[] = {
	{ 0xEFD3C963, 0x6C4F9920, }, // scePowerTick
	{ 0x87440F5E, 0xF8C9FAF5, }, // scePowerIsPowerOnline
	{ 0x0AFD0D8B, 0x81DCA5A5, }, // scePowerIsBatteryExist 
	{ 0x1E490401, 0xAB842949, }, // scePowerIsBatteryCharging 
	{ 0xD3075926, 0xDE18E7C0, }, // scePowerIsLowBattery 
	{ 0x2085D15D, 0x2CBFA597, }, // scePowerGetBatteryLifePercent
	{ 0x8EFB3FA2, 0xC8749D1A, }, // scePowerGetBatteryLifeTime 
	{ 0x28E12023, 0xC8324682, }, // scePowerGetBatteryTemp
	{ 0x483CE86B, 0x4A1118E1, }, // scePowerGetBatteryVolt 
	{ 0x2B7C7CF4, 0x334539BA, }, // scePowerRequestStandby 
	{ 0xFEE03A2F, 0xFDB5BFE9, }, // scePowerGetCpuClockFrequency 
	{ 0x478FE6F5, 0x9F53A71F, }, // scePowerGetBusClockFrequency 
	{ 0x737486F2, 0xEBD177D6, }, // scePowerSetClockFrequency 
	{ 0xD6E50D7B, 0x8BA12BAF, }, // scePowerRegisterCallback
	{ 0x1688935C, 0x9F53A71F, }, // scePowerGetBusClockFrequencyInt
	{ 0x6D2CA84B, UNKNOWNNID, }, // scePowerWlanActivate
	{ 0x23BB0A60, UNKNOWNNID, }, // scePowerWlanDeactivate
	{ 0x2B51FE2F, 0x2C6EBF4B, }, // scePower_driver_2B51FE2F
	{ 0x442BFBAC, UNKNOWNNID, }, // scePower_driver_442BFBAC
	{ 0xE8685403, UNKNOWNNID, }, // scePower_driver_E8685403
	{ 0xEDC13FE5, 0x3C0B1828, }, // scePowerGetIdleTimer
	{ 0x7F30B3B1, 0x25C15BDD, }, // scePowerIdleTimerEnable
	{ 0x972CE941, 0x6751719C, }, // scePowerIdleTimerDisable
	{ 0x27F3292C, 0x7810C65D, }, // scePowerBatteryUpdateInfo
	{ 0xE8E4E204, 0xBE3AEA7F, }, // scePower_driver_E8E4E204
	{ 0xB999184C, 0x00B50FFA, }, // scePowerGetLowBatteryCapacity
	{ 0x166922EC, UNKNOWNNID, }, // scePower_driver_166922EC
	{ 0xDD3D4DAC, UNKNOWNNID, }, // scePower_driver_DD3D4DAC
	{ 0xB4432BC8, 0x481F5556, }, // scePowerGetBatteryChargingStatus
	{ 0x78A1A796, UNKNOWNNID, }, // scePower_driver_78A1A796
	{ 0x94F5A53F, 0x08EC482C, }, // scePowerGetBatteryRemainCapacity
	{ 0xFD18A0FF, UNKNOWNNID, }, // scePower_driver_FD18A0FF
	{ 0x862AE1A6, 0xC3EC1F7D, }, // scePowerGetBatteryElec
	{ 0x23436A4A, UNKNOWNNID, }, // scePower_driver_23436A4A
	{ 0x0CD21B1F, UNKNOWNNID, }, // scePower_driver_0CD21B1F
	{ 0x165CE085, UNKNOWNNID, }, // scePower_driver_165CE085
	{ 0xD6D016EF, UNKNOWNNID, }, // scePowerLock
	{ 0xCA3D34C1, UNKNOWNNID, }, // scePowerUnlock
	{ 0x79DB9421, UNKNOWNNID, }, // scePowerRebootStart
	{ 0x7FA406DD, 0x5B459B92, }, // scePowerIsRequest
	{ 0xAC32C9CC, 0x062CFDDC, }, // scePowerRequestSuspend
	{ 0x2875994B, 0x12D8AD47, }, // scePower_driver_2875994B
	{ 0x3951AF53, UNKNOWNNID, }, // scePowerEncodeUBattery
	{ 0x0074EF9B, 0x5B8EBCD9, }, // scePowerGetResumeCount
	{ 0xF535D928, UNKNOWNNID, }, // scePower_driver_F535D928
	{ 0xDFA8BAF8, 0xE51B6170, }, // scePowerUnregisterCallback
	{ 0xDB9D28DD, 0xE51B6170, }, // scePowerUnregitserCallback
	{ 0xD24E6BEB, UNKNOWNNID, }, // scePower_driver_D24E6BEB
	{ 0x35B7662E, UNKNOWNNID, }, // scePowerGetSectionDescriptionEntry
	{ 0xF9B4DEA1, UNKNOWNNID, }, // scePowerLimitPllClock
	{ 0x843FBF43, 0x473DE4F3, }, // scePowerSetCpuClockFrequency
	{ 0xB8D7B3FB, 0xBEA1E507, }, // scePowerSetBusClockFrequency
	{ 0x478FE6F5, 0x9F53A71F, }, // scePowerGetBusClockFrequency
	{ 0xBD681969, 0x9F53A71F, }, // scePowerGetBusClockFrequencyInt
	{ 0xB1A52C83, 0xB7C54B9A, }, // scePowerGetCpuClockFrequencyFloat
	{ 0x9BADB3EB, 0xCC998F67, }, // scePowerGetBusClockFrequencyFloat
	{ 0x737486F2, UNKNOWNNID, }, // scePowerSetClockFrequency
	{ 0xE0B7A95D, UNKNOWNNID, }, // scePower_driver_E0B7A95D
	{ 0xC23AC778, UNKNOWNNID, }, // scePower_driver_C23AC778
	{ 0x23C31FFE, 0x503F08C9, }, // scePowerVolatileMemLock
	{ 0xFA97A599, 0x37DB9C37, }, // scePowerVolatileMemTryLock
	{ 0xB3EDD801, 0x88D4244D, }, // scePowerVolatileMemUnlock
};

static nid_entry InterruptManagerForKernel_nid[] = {
	{ 0x8A389411, 0x359F6F5C, }, // sceKernelDisableSubIntr 
	{ 0xD2E8363F, 0xB1F5E99B, }, // QueryIntrHandlerInfo
	{ 0x8B61808B, 0x399FF74C, }, // sceKernelQuerySystemCall
	{ 0x53991063, UNKNOWNNID, }, // InterruptManagerForKernel_53991063
	{ 0x468BC716, UNKNOWNNID, }, // sceKernelGetInterruptExitCount
	{ 0x43CD40EF, UNKNOWNNID, }, // ReturnToThread
	{ 0x85F7766D, UNKNOWNNID, }, // SaveThreadContext
	{ 0x00B6B0F3, UNKNOWNNID, }, // QueryInterruptManCB
	{ 0x15894D0B, UNKNOWNNID, }, // InterruptManagerForKernel_15894D0B
	{ 0xB5A15B30, UNKNOWNNID, }, // sceKernelSetIntrLevel
	{ 0x43A7BBDC, UNKNOWNNID, }, // InterruptManagerForKernel_43A7BBDC
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
	{ 0xCA04A2B9, 0xD06AA711, }, // sceKernelRegisterSubIntrHandler
	{ 0x5CB5A78B, 0x12B95762, }, // sceKernelSuspendSubIntr
	{ 0x7860E0DC, 0x85E7F044, }, // sceKernelResumeSubIntr
	{ 0xFC4374B8, 0xA811BAF2, }, // sceKernelIsSubInterruptOccurred
	{ 0x35634A64, UNKNOWNNID, }, // sceKernelGetCpuClockCounterWide
	{ 0x2DC9709B, UNKNOWNNID, }, // _sceKernelGetCpuClockCounterLow
	{ 0xE9E652A9, UNKNOWNNID, }, // _sceKernelGetCpuClockCounterHigh
	{ 0x0FC68A56, UNKNOWNNID, }, // sceKernelSetPrimarySyscallHandler
	{ 0xF4D443F3, UNKNOWNNID, }, // sceKernelRegisterSystemCallTable
};

static nid_entry sceCtrl_driver_nid[] = {
	{ 0x454455AC, 0x9F3038AC, },  //sceCtrlReadBufferPositive
	{ 0xC4AAD55F, 0x18654FC0, },  //sceCtrlPeekBufferPositive
	{ 0x28E71A16, 0x6CB49301, },  //sceCtrlSetSamplingMode
	{ 0x3E65A0EA, UNKNOWNNID, }, // sceCtrlInit
	{ 0xE03956E9, UNKNOWNNID, }, // sceCtrlEnd
	{ 0xC3F607F3, UNKNOWNNID, }, // sceCtrlSuspend
	{ 0xC245B57B, UNKNOWNNID, }, // sceCtrlResume
	{ 0xA88E8D22, UNKNOWNNID, }, // sceCtrlSetIdleCancelKey
	{ 0xB7CEAED4, UNKNOWNNID, }, // sceCtrlGetIdleCancelKey
	{ 0x7CA723DC, UNKNOWNNID, }, // sceCtrl_driver_7CA723DC
	{ 0x5E77BC8A, UNKNOWNNID, }, // sceCtrl_driver_5E77BC8A
	{ 0x5C56C779, UNKNOWNNID, }, // sceCtrl_driver_5C56C779
};

static nid_entry sceDisplay_driver_nid[] = {
	{ 0x4AB7497F, 0x37533141, }, // sceDisplaySetFrameBuf
	{ 0xB685BA36, 0xC30D327D, }, // sceDisplayWaitVblankStart
	{ 0xE56B11BA, 0x08A10838, }, // sceDisplayGetFrameBuf
	{ 0x776ADFDB, 0x89FD2128, }, // sceDisplaySetBacklightInternal
	{ 0x31C4BAA8, 0x3A5621E0, }, // sceDisplayGetBrightness
	{ 0x5B5AEFAD, 0xCCEEA6BB, }, // from 1.50
	{ 0xDBA6C4C4, 0x0D3248C1, }, // sceDisplayGetFramePerSec
	{ 0x432D133F, UNKNOWNNID, }, // sceDisplayEnable
	{ 0x681EE6A7, UNKNOWNNID, }, // sceDisplayDisable
	{ 0x7ED59BC4, 0x73CA5F45, }, // sceDisplaySetHoldMode
	{ 0xA544C486, 0x3DABE438, }, // sceDisplaySetResumeMode
	{ 0x63E22A26, UNKNOWNNID, }, // sceDisplay_driver_63E22A26
	{ 0x289D82FE, 0x37533141, }, // sceDisplaySetFrameBuf
	{ 0xEEDA2E54, 0x08A10838, }, // sceDisplayGetFrameBuf
	{ 0xB4F378FA, 0x2D859B86, }, // sceDisplayIsForeground
	{ 0xAC14F1FF, UNKNOWNNID, }, // sceDisplayGetForegroundLevel
	{ 0x9E3C6DC6, UNKNOWNNID, }, // sceDisplaySetBrightness
	{ 0x9C6EAAD7, 0x21F912F4, }, // sceDisplayGetVcount
	{ 0x4D4E10EC, 0x59456A3E, }, // sceDisplayIsVblank
	{ 0x69B53541, 0x2F701A85, }, // sceDisplayGetVblankRest
	{ 0x36CDFADE, 0xB6C8FD75, }, // sceDisplayWaitVblank
	{ 0x8EB9EC49, 0xA10EABE8, }, // sceDisplayWaitVblankCB
	{ 0x984C27E7, 0xC30D327D, }, // sceDisplayWaitVblankStart
	{ 0x46F186C3, 0x3E4B1B28, }, // sceDisplayWaitVblankStartCB
	{ 0x773DD3A3, 0xA87F508D, }, // sceDisplayGetCurrentHcount
	{ 0x210EAB3A, 0x7A3E7402, }, // sceDisplayGetAccumulatedHcount
};

static nid_entry sceAudio_driver_nid[] = {
	{ 0xB282F4B2, 0xA388ABDB, },  //sceAudioGetChannelRestLength
	{ 0x669D93E4, 0xE9BCD519, },  //sceAudioSRCChReserve
	{ 0x138A70F1, 0x0399579B, },  //sceAudioSRCChRelease
	{ 0x43645E69, 0x475F41FE, },  //sceAudioSRCOutputBlocking
	{ 0x80F1F7E0, UNKNOWNNID, }, // sceAudioInit
	{ 0x210567F7, UNKNOWNNID, }, // sceAudioEnd
	{ 0xA2BEAA6C, UNKNOWNNID, }, // sceAudioSetFrequency 
	{ 0xB61595C0, UNKNOWNNID, }, // sceAudioLoopbackTest 
	{ 0x927AC32B, UNKNOWNNID, }, // sceAudioSetVolumeOffset 
	{ 0x8C1009B2, 0xD35EFCD9, }, // sceAudioOutput 
	{ 0x136CAF51, 0xA906D208, }, // sceAudioOutputBlocking 
	{ 0xE2D56B2D, 0xC847973E, }, // sceAudioOutputPanned 
	{ 0x13F592BC, 0xB29D727F, }, // sceAudioOutputPannedBlocking 
	{ 0x5EC81C55, 0xF84226FB, }, // sceAudioChReserve 
	{ 0x41EFADE7, 0x686AC4F4, }, // sceAudioOneshotOutput 
	{ 0x6FC46853, 0xA6FD1A21, }, // sceAudioChRelease 
	{ 0xB011922F, UNKNOWNNID, }, // sceAudio_driver_B011922F 
	{ 0xCB2E439E, 0x3F4D81C1, }, // sceAudioSetChannelDataLen 
	{ 0x95FD0C2D, 0x5A0397C5, }, // sceAudioChangeChannelConfig 
	{ 0xB7E1D8E7, 0xA25A3346, }, // sceAudioChangeChannelVolume 
	{ 0x38553111, UNKNOWNNID, }, // sceAudio_driver_38553111 
	{ 0x5C37C0AE, UNKNOWNNID, }, // sceAudio_driver_5C37C0AE 
	{ 0xE0727056, UNKNOWNNID, }, // sceAudio_driver_E0727056 
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
	{ 0x9F9AE99C, UNKNOWNNID, }, // InitForKernel_9F9AE99C
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
	{ 0x0F341BE4, 0xE8004C90, }, // sceImposeGetHomePopup
	{ 0x116CFF64, UNKNOWNNID, }, // sceImposeCheckVideoOut
	{ 0x116DDED6, UNKNOWNNID, }, // sceImposeSetVideoOutMode
	{ 0x1AEED8FE, UNKNOWNNID, }, // sceImposeSuspend
	{ 0x1B6E3400, UNKNOWNNID, }, // sceImposeGetStatus
	{ 0x24FD7BCF, 0xD33D92AF, }, // sceImposeGetLanguageMode
	{ 0x36AA6E91, 0xBF34567B, }, // sceImposeSetLanguageMode
	{ 0x381BD9E7, 0x90B3DC7F, }, // sceImposeHomeButton
	{ 0x531C9778, UNKNOWNNID, }, // sceImposeGetParam
	{ 0x5595A71A, 0x5EF5BAF2, }, // sceImposeSetHomePopup
	{ 0x7084E72C, 0x6C41D4D6, }, // sceImpose_driver_7084E72C
	{ 0x72189C48, 0x6513E3B5, }, // sceImposeSetUMDPopup
	{ 0x810FB7FB, UNKNOWNNID, }, // sceImposeSetParam
	{ 0x86924032, UNKNOWNNID, }, // sceImposeResume
	{ 0x8C943191, 0xEBC850AC, }, // sceImposeGetBatteryIconStatus
	{ 0x8F6E3518, 0xB658F454, }, // sceImposeGetBacklightOffTime
	{ 0x967F6D4A, 0x8C6BEAF3, }, // sceImposeSetBacklightOffTime
	{ 0x9C8C6C81, UNKNOWNNID, }, // sceImposeSetStatus
	{ 0x9DBCE0C4, 0x08362264, }, // sceImpose_driver_9DBCE0C4
	{ 0xB415FC59, UNKNOWNNID, }, // sceImposeChanges
	{ 0xBDBC42A6, UNKNOWNNID, }, // sceImposeInit
	{ 0xC7E36CC7, UNKNOWNNID, }, // sceImposeEnd
	{ 0xE0887BC8, 0x454CB346, }, // sceImposeGetUMDPopup
};

static nid_entry sceGe_driver_nid[] = {
	{ 0x71FCD1D6, UNKNOWNNID, }, // sceGeInit
	{ 0x9F2C2948, UNKNOWNNID, }, // sceGeEnd
	{ 0x8F185DF7, UNKNOWNNID, }, // sceGeEdramInit
	{ 0x1F6752AD, 0x670F15ED, }, // sceGeEdramGetSize
	{ 0xE47E40E4, 0x40F72852, }, // sceGeEdramGetAddr
	{ 0xB77905EA, 0xD8E03A2B, }, // sceGeEdramSetAddrTranslation
	{ 0xB415364D, UNKNOWNNID, }, // sceGeGetReg
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
	{ 0x9DA4A75F, UNKNOWNNID, }, // sceGe_driver_9DA4A75F
	{ 0x114E1745, UNKNOWNNID, }, // sceGe_driver_114E1745
};

static nid_entry sceRtc_driver_nid[] = {
	{ 0x029CA3B3, UNKNOWNNID, }, // sceRtc_029CA3B3
	{ 0xE7C27D1B, 0x0287B1C2, }, // sceRtcGetCurrentClockLocalTime
	{ 0x34885E0D, 0xB55B2E56, }, // sceRtcConvertUtcToLocalTime
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
	{ 0x6A676D2D, UNKNOWNNID, }, // sceRtc_driver_6A676D2D
	{ 0x759937C5, UNKNOWNNID, }, // sceRtcSetConf
	{ 0x7D1FBED3, UNKNOWNNID, }, // sceRtcSetAlarmTick
	{ 0x81FCDA34, UNKNOWNNID, }, // sceRtc_driver_81FCDA34
	{ 0xC2DDBEB5, UNKNOWNNID, }, // sceRtcGetAlarmTick
	{ 0xC499AF2F, UNKNOWNNID, }, // sceRtc_driver_C499AF2F
	{ 0xE1C93E47, 0x0C8E0D5C, }, // sceRtcGetTime64_t
	{ 0xF0B5571C, UNKNOWNNID, }, // sceRtcSynchronize
	{ 0xFB3B18CD, UNKNOWNNID, }, // sceRtc_driver_FB3B18CD
};

static nid_entry sceMpegbase_driver_nid[] = {
	{ 0x304882E1, UNKNOWNNID, }, // sceMpegbase_driver_304882E1
};

resolver_config nid_fix[] = {
#define NID_ENTRY(libname) \
	{ #libname, NELEMS(libname##_nid), libname##_nid, }
	NID_ENTRY(SysMemForKernel),
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
#undef NID_ENTRY
};

u32 nid_fix_size = NELEMS(nid_fix);
