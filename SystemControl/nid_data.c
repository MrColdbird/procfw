#include <pspsdk.h>
#include "nid_resolver.h"
#include "utils.h"

static nid_entry sceNand_driver_nid[] = {
	{ 0xA513BB12, UNKNOWNNID, }, // sceNandInit
	{ 0xD305870E, UNKNOWNNID, }, // sceNandEnd
	{ 0x73A68408, UNKNOWNNID, }, // sceNandSuspend
	{ 0x0F9BBBBD, UNKNOWNNID, }, // sceNandResume
	{ 0x84EE5D76, UNKNOWNNID, }, // sceNandSetWriteProtect
	{ 0xAE4438C7, UNKNOWNNID, }, // sceNandLock
	{ 0x41FFA822, UNKNOWNNID, }, // sceNandUnlock
	{ 0xE41A11DE, UNKNOWNNID, }, // sceNandReadStatus
	{ 0x7AF7B77A, UNKNOWNNID, }, // sceNandReset
	{ 0xFCDF7610, UNKNOWNNID, }, // sceNandReadId
	{ 0x89BDCA08, UNKNOWNNID, }, // sceNandReadPages
	{ 0x8AF0AB9F, UNKNOWNNID, }, // sceNandWritePages
	{ 0xE05AE88D, UNKNOWNNID, }, // sceNand_driver_E05AE88D
	{ 0x8932166A, UNKNOWNNID, }, // sceNand_driver_8932166A
	{ 0xC478C1DE, UNKNOWNNID, }, // sceNand_driver_C478C1DE
	{ 0xBADD5D46, UNKNOWNNID, }, // sceNand_driver_BADD5D46
	{ 0x766756EF, UNKNOWNNID, }, // sceNandReadAccess
	{ 0x0ADC8686, UNKNOWNNID, }, // sceNandWriteAccess
	{ 0xEB0A0022, UNKNOWNNID, }, // sceNandEraseBlock
	{ 0x5182C394, UNKNOWNNID, }, // sceNandReadExtraOnly
	{ 0xEF55F193, UNKNOWNNID, }, // sceNandCalcEcc
	{ 0x18B78661, UNKNOWNNID, }, // sceNandVerifyEcc
	{ 0xB795D2ED, UNKNOWNNID, }, // sceNandCollectEcc
	{ 0xD897C343, UNKNOWNNID, }, // sceNand_driver_D897C343
	{ 0xCE9843E6, UNKNOWNNID, }, // sceNandGetPageSize
	{ 0xB07C41D4, UNKNOWNNID, }, // sceNandGetPagesPerBlock
	{ 0xC1376222, UNKNOWNNID, }, // sceNandGetTotalBlocks
	{ 0x716CD2B2, UNKNOWNNID, }, // sceNandWriteBlock
	{ 0xB2B021E5, UNKNOWNNID, }, // sceNandWriteBlockWithVerify
	{ 0xC32EA051, UNKNOWNNID, }, // sceNandReadBlockWithRetry
	{ 0x5AC02755, UNKNOWNNID, }, // sceNandVerifyBlockWithRetry
	{ 0x8933B2E0, UNKNOWNNID, }, // sceNandEraseBlockWithRetry
	{ 0x01F09203, UNKNOWNNID, }, // sceNandIsBadBlock
	{ 0xC29DA136, UNKNOWNNID, }, // sceNand_driver_C29DA136
	{ 0x3F76BC21, UNKNOWNNID, }, // sceNand_driver_3F76BC21
	{ 0xEBA0E6C6, UNKNOWNNID, }, // sceNand_driver_EBA0E6C6
	{ 0x2FF6081B, UNKNOWNNID, }, // sceNand_driver_2FF6081B
	{ 0x2674CFFE, UNKNOWNNID, }, // sceNandEraseAllBlock
	{ 0x9B2AC433, UNKNOWNNID, }, // sceNandTestBlock
};

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
	{ 0x9697CD32, 0x7BE9653E, }, // sceKernelPartitionTotalFreeMemSize
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
	{ 0xD8779AC6, UNKNOWNNID, }, // sceKernelIcacheClearAll
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
	{ 0x821BE114, UNKNOWNNID, }, // sceKernelLoadExecVSHDiscUpdater
	{ 0x015DA036, UNKNOWNNID, }, // sceKernelLoadExecBufferVSHUsbWlan
	{ 0x4F41E75E, UNKNOWNNID, }, // sceKernelLoadExecBufferVSHUsbWlanDebug
	{ 0x31DF42BF, UNKNOWNNID, }, // sceKernelLoadExecVSHMs1
	{ 0x70901231, UNKNOWNNID, }, // sceKernelLoadExecVSHMs3
	{ 0x62C459E1, UNKNOWNNID, }, // sceKernelLoadExecBufferVSHPlain
	{ 0x918782E8, UNKNOWNNID, }, // sceKernelLoadExecBufferVSHFromHost
	{ 0xBB28E9B7, UNKNOWNNID, }, // sceKernelLoadExecBufferPlain0
	{ 0x71A1D738, UNKNOWNNID, }, // sceKernelLoadExecBufferPlain
	{ 0x4D5C75BE, UNKNOWNNID, }, // sceKernelLoadExecFromHost
	{ 0x4AC57943, UNKNOWNNID, }, // sceKernelRegisterExitCallback
	{ 0xD9739B89, UNKNOWNNID, }, // sceKernelUnregisterExitCallback
	{ 0x659188E1, UNKNOWNNID, }, // sceKernelCheckExitCallback
	{ 0x62A27008, UNKNOWNNID, }, // sceKernelInvokeExitCallback
	{ 0x7B7C47EF, UNKNOWNNID, }, // sceKernelLoadExecVSHDiscDebug
	{ 0x061D9514, UNKNOWNNID, }, // sceKernelLoadExecVSHMs4
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
	{ 0xCC1D3699, UNKNOWNNID, }, // sceKernelStopUnloadSelfModule
	{ 0x04B7BD22, UNKNOWNNID, }, // sceKernelSearchModuleByName
	{ 0x54D9E02E, UNKNOWNNID, }, // sceKernelSearchModuleByAddress
	{ 0x5F0CC575, UNKNOWNNID, }, // sceKernelRebootBeforeForUser
	{ 0xB49FFB9E, UNKNOWNNID, }, // sceKernelRebootBeforeForKernel
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
	{ 0x3251EA56, UNKNOWNNID, }, // sceIoPollAsync
	{ 0xE23EEC33, UNKNOWNNID, }, // sceIoWaitAsync
	{ 0x35DBD746, UNKNOWNNID, }, // sceIoWaitAsyncCB
	{ 0xCB05F8D6, UNKNOWNNID, }, // sceIoGetAsyncStat
	{ 0xB293727F, UNKNOWNNID, }, // sceIoChangeAsyncPriority
	{ 0xA12A0514, UNKNOWNNID, }, // sceIoSetAsyncCallback
	{ 0x810C4BC3, UNKNOWNNID, }, // sceIoClose
	{ 0xFF5940B6, UNKNOWNNID, }, // sceIoCloseAsync
	{ 0xA905B705, UNKNOWNNID, }, // sceIoCloseAll
	{ 0x109F50BC, UNKNOWNNID, }, // sceIoOpen
	{ 0x89AA9906, UNKNOWNNID, }, // sceIoOpenAsync
	{ 0x6A638D83, UNKNOWNNID, }, // sceIoRead
	{ 0xA0B5A7C2, UNKNOWNNID, }, // sceIoReadAsync
	{ 0x42EC03AC, UNKNOWNNID, }, // sceIoWrite
	{ 0x0FACAB19, UNKNOWNNID, }, // sceIoWriteAsync
	{ 0x27EB27B8, UNKNOWNNID, }, // sceIoLseek
	{ 0x71B19E77, UNKNOWNNID, }, // sceIoLseekAsync
	{ 0x68963324, UNKNOWNNID, }, // sceIoLseek32
	{ 0x1B385D8F, UNKNOWNNID, }, // sceIoLseek32Async
	{ 0x63632449, UNKNOWNNID, }, // sceIoIoctl
	{ 0xE95A012B, UNKNOWNNID, }, // sceIoIoctlAsync
	{ 0xB29DDF9C, UNKNOWNNID, }, // sceIoDopen
	{ 0xE3EB004C, UNKNOWNNID, }, // sceIoDread
	{ 0xEB092469, UNKNOWNNID, }, // sceIoDclose
	{ 0xF27A9C51, UNKNOWNNID, }, // sceIoRemove
	{ 0x06A70004, UNKNOWNNID, }, // sceIoMkdir
	{ 0x1117C65F, UNKNOWNNID, }, // sceIoRmdir
	{ 0x55F4717D, UNKNOWNNID, }, // sceIoChdir
	{ 0xAB96437F, UNKNOWNNID, }, // sceIoSync
	{ 0xACE946E8, UNKNOWNNID, }, // sceIoGetstat
	{ 0xB8A740F4, UNKNOWNNID, }, // sceIoChstat
	{ 0x779103A0, UNKNOWNNID, }, // sceIoRename
	{ 0x54F5FB11, UNKNOWNNID, }, // sceIoDevctl
	{ 0x08BD7374, UNKNOWNNID, }, // sceIoGetDevType
	{ 0xB2A628C1, UNKNOWNNID, }, // sceIoAssign
	{ 0x6D08A871, UNKNOWNNID, }, // sceIoUnassign
	{ 0xCB0A151F, UNKNOWNNID, }, // sceIoChangeThreadCwd
	{ 0xE8BC6571, UNKNOWNNID, }, // sceIoCancel
	{ 0x8E982A74, UNKNOWNNID, }, // sceIoAddDrv
	{ 0xC7F35804, UNKNOWNNID, }, // sceIoDelDrv
};

static nid_entry KDebugForKernel_nid[] = {
	{ 0xE146606D, 0xAAC71263, }, // sceKernelRegisterDebugPutchar 
	{ 0xE7A3874D, UNKNOWNNID, }, // sceKernelRegisterAssertHandler
	{ 0x2FF4E9F9, UNKNOWNNID, }, // sceKernelAssert
	{ 0x9B868276, UNKNOWNNID, }, // sceKernelGetDebugPutchar
	{ 0x7CEB2C09, UNKNOWNNID, }, // sceKernelRegisterKprintfHandler
	{ 0x84F370BC, UNKNOWNNID, }, // Kprintf
	{ 0x5CE9838B, UNKNOWNNID, }, // sceKernelDebugWrite
	{ 0x66253C4E, UNKNOWNNID, }, // sceKernelRegisterDebugWrite
	{ 0xDBB5597F, UNKNOWNNID, }, // sceKernelDebugRead
	{ 0xE6554FDA, UNKNOWNNID, }, // sceKernelRegisterDebugRead
	{ 0xB9C643C9, UNKNOWNNID, }, // sceKernelDebugEcho
	{ 0x7D1C74F0, UNKNOWNNID, }, // sceKernelDebugEchoSet
	{ 0x24C32559, UNKNOWNNID, }, // KDebugForKernel_24C32559
	{ 0xD636B827, UNKNOWNNID, }, // sceKernelRemoveByDebugSection
	{ 0x5282DD5E, UNKNOWNNID, }, // KDebugForKernel_5282DD5E
	{ 0x9F8703E4, UNKNOWNNID, }, // KDebugForKernel_9F8703E4
	{ 0x333DCEC7, UNKNOWNNID, }, // KDebugForKernel_333DCEC7
	{ 0xE892D9A1, UNKNOWNNID, }, // KDebugForKernel_E892D9A1
	{ 0xA126F497, UNKNOWNNID, }, // KDebugForKernel_A126F497
	{ 0xB7251823, UNKNOWNNID, }, // KDebugForKernel_B7251823
};

static nid_entry StdioForKernel_nid[] = {
	{ 0x2CCF071A, UNKNOWNNID, }, // fdprintf
	{ 0xCAB439DF, UNKNOWNNID, }, // printf
	{ 0x4F78930A, UNKNOWNNID, }, // fdputc
	{ 0xD768752A, UNKNOWNNID, }, // putchar
	{ 0x36B23B8B, UNKNOWNNID, }, // fdputs
	{ 0xD97C8CB9, UNKNOWNNID, }, // puts
	{ 0xD2B2A2A7, UNKNOWNNID, }, // fdgetc
	{ 0x7E338487, UNKNOWNNID, }, // getchar
	{ 0x11A5127A, UNKNOWNNID, }, // fdgets
	{ 0xBFF7E760, UNKNOWNNID, }, // gets
	{ 0x172D316E, UNKNOWNNID, }, // sceKernelStdin
	{ 0xA6BAB2E9, UNKNOWNNID, }, // sceKernelStdout
	{ 0xF78BA90A, UNKNOWNNID, }, // sceKernelStderr
	{ 0x98220F3E, UNKNOWNNID, }, // sceKernelStdoutReopen
	{ 0xFB5380C5, UNKNOWNNID, }, // sceKernelStderrReopen
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
	{ 0x9CE06934, UNKNOWNNID, }, // scePowerInit
	{ 0xAD5BB433, UNKNOWNNID, }, // scePowerEnd
	{ 0x6D2CA84B, UNKNOWNNID, }, // scePowerWlanActivate
	{ 0x23BB0A60, UNKNOWNNID, }, // scePowerWlanDeactivate
	{ 0x2B51FE2F, UNKNOWNNID, }, // scePower_driver_2B51FE2F
	{ 0x442BFBAC, UNKNOWNNID, }, // scePower_driver_442BFBAC
	{ 0xE8685403, UNKNOWNNID, }, // scePower_driver_E8685403
	{ 0xEDC13FE5, UNKNOWNNID, }, // scePowerGetIdleTimer
	{ 0x1BA2FCAE, UNKNOWNNID, }, // scePowerSetIdleCallback
	{ 0x7F30B3B1, UNKNOWNNID, }, // scePowerIdleTimerEnable
	{ 0x972CE941, UNKNOWNNID, }, // scePowerIdleTimerDisable
	{ 0x27F3292C, UNKNOWNNID, }, // scePowerBatteryUpdateInfo
	{ 0xE8E4E204, UNKNOWNNID, }, // scePower_driver_E8E4E204
	{ 0xB999184C, UNKNOWNNID, }, // scePowerGetLowBatteryCapacity
	{ 0x166922EC, UNKNOWNNID, }, // scePower_driver_166922EC
	{ 0xDD3D4DAC, UNKNOWNNID, }, // scePower_driver_DD3D4DAC
	{ 0xB4432BC8, UNKNOWNNID, }, // scePowerGetBatteryChargingStatus
	{ 0x78A1A796, UNKNOWNNID, }, // scePower_driver_78A1A796
	{ 0x94F5A53F, UNKNOWNNID, }, // scePowerGetBatteryRemainCapacity
	{ 0xFD18A0FF, UNKNOWNNID, }, // scePower_driver_FD18A0FF
	{ 0x862AE1A6, UNKNOWNNID, }, // scePowerGetBatteryElec
	{ 0x23436A4A, UNKNOWNNID, }, // scePower_driver_23436A4A
	{ 0x0CD21B1F, UNKNOWNNID, }, // scePower_driver_0CD21B1F
	{ 0x165CE085, UNKNOWNNID, }, // scePower_driver_165CE085
	{ 0xD6D016EF, UNKNOWNNID, }, // scePowerLock
	{ 0xCA3D34C1, UNKNOWNNID, }, // scePowerUnlock
	{ 0x79DB9421, UNKNOWNNID, }, // scePowerRebootStart
	{ 0xDB62C9CF, UNKNOWNNID, }, // scePowerCancelRequest
	{ 0x7FA406DD, UNKNOWNNID, }, // scePowerIsRequest
	{ 0x2B7C7CF4, UNKNOWNNID, }, // scePowerRequestStandby
	{ 0xAC32C9CC, UNKNOWNNID, }, // scePowerRequestSuspend
	{ 0x2875994B, UNKNOWNNID, }, // scePower_driver_2875994B
	{ 0x3951AF53, UNKNOWNNID, }, // scePowerEncodeUBattery
	{ 0x0074EF9B, UNKNOWNNID, }, // scePowerGetResumeCount
	{ 0xF535D928, UNKNOWNNID, }, // scePower_driver_F535D928
	{ 0x04B7766E, UNKNOWNNID, }, // scePowerRegisterCallback
	{ 0xDFA8BAF8, UNKNOWNNID, }, // scePowerUnregisterCallback
	{ 0xDB9D28DD, UNKNOWNNID, }, // scePowerUnregitserCallback
	{ 0xD24E6BEB, UNKNOWNNID, }, // scePower_driver_D24E6BEB
	{ 0x35B7662E, UNKNOWNNID, }, // scePowerGetSectionDescriptionEntry
	{ 0xF9B4DEA1, UNKNOWNNID, }, // scePowerLimitPllClock
	{ 0x843FBF43, UNKNOWNNID, }, // scePowerSetCpuClockFrequency
	{ 0xB8D7B3FB, UNKNOWNNID, }, // scePowerSetBusClockFrequency
	{ 0xFEE03A2F, UNKNOWNNID, }, // scePowerGetCpuClockFrequency
	{ 0x478FE6F5, UNKNOWNNID, }, // scePowerGetBusClockFrequency
	{ 0xFDB5BFE9, UNKNOWNNID, }, // scePowerGetCpuClockFrequencyInt
	{ 0xBD681969, UNKNOWNNID, }, // scePowerGetBusClockFrequencyInt
	{ 0xB1A52C83, UNKNOWNNID, }, // scePowerGetCpuClockFrequencyFloat
	{ 0x9BADB3EB, UNKNOWNNID, }, // scePowerGetBusClockFrequencyFloat
	{ 0x737486F2, UNKNOWNNID, }, // scePowerSetClockFrequency
	{ 0xE0B7A95D, UNKNOWNNID, }, // scePower_driver_E0B7A95D
	{ 0xC23AC778, UNKNOWNNID, }, // scePower_driver_C23AC778
	{ 0x23C31FFE, UNKNOWNNID, }, // scePowerVolatileMemLock
	{ 0xFA97A599, UNKNOWNNID, }, // scePowerVolatileMemTryLock
	{ 0xB3EDD801, UNKNOWNNID, }, // scePowerVolatileMemUnlock
};

static nid_entry InterruptManagerForKernel_nid[] = {
	{ 0x8A389411, 0x359F6F5C, }, // sceKernelDisableSubIntr 
	{ 0xD2E8363F, 0xB1F5E99B, }, // QueryIntrHandlerInfo
	{ 0x8B61808B, 0x399FF74C, }, // sceKernelQuerySystemCall
	{ 0x092968F4, UNKNOWNNID, }, // sceKernelCpuSuspendIntr
	{ 0x5F10D406, UNKNOWNNID, }, // sceKernelCpuResumeIntr
	{ 0x3B84732D, UNKNOWNNID, }, // sceKernelCpuResumeIntrWithSync
	{ 0xFE28C6D9, UNKNOWNNID, }, // sceKernelIsIntrContext
	{ 0x53991063, UNKNOWNNID, }, // InterruptManagerForKernel_53991063
	{ 0x468BC716, UNKNOWNNID, }, // sceKernelGetInterruptExitCount
	{ 0x43CD40EF, UNKNOWNNID, }, // ReturnToThread
	{ 0x85F7766D, UNKNOWNNID, }, // SaveThreadContext
	{ 0x02314986, UNKNOWNNID, }, // sceKernelCpuEnableIntr
	{ 0x00B6B0F3, UNKNOWNNID, }, // QueryInterruptManCB
	{ 0x58DD8978, UNKNOWNNID, }, // sceKernelRegisterIntrHandler
	{ 0x15894D0B, UNKNOWNNID, }, // InterruptManagerForKernel_15894D0B
	{ 0xF987B1F0, UNKNOWNNID, }, // sceKernelReleaseIntrHandler
	{ 0xB5A15B30, UNKNOWNNID, }, // sceKernelSetIntrLevel
	{ 0x43A7BBDC, UNKNOWNNID, }, // InterruptManagerForKernel_43A7BBDC
	{ 0x02475AAF, UNKNOWNNID, }, // sceKernelIsInterruptOccurred
	{ 0x4D6E7305, UNKNOWNNID, }, // sceKernelEnableIntr
	{ 0x750E2507, UNKNOWNNID, }, // sceKernelSuspendIntr
	{ 0xD774BA45, UNKNOWNNID, }, // sceKernelDisableIntr
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
	{ 0xD61E6961, UNKNOWNNID, }, // sceKernelReleaseSubIntrHandler
	{ 0xFB8E22EC, UNKNOWNNID, }, // sceKernelEnableSubIntr
	{ 0x5CB5A78B, UNKNOWNNID, }, // sceKernelSuspendSubIntr
	{ 0x7860E0DC, UNKNOWNNID, }, // sceKernelResumeSubIntr
	{ 0xFC4374B8, UNKNOWNNID, }, // sceKernelIsSubInterruptOccurred
	{ 0x30C08374, UNKNOWNNID, }, // sceKernelGetCpuClockCounter
	{ 0x35634A64, UNKNOWNNID, }, // sceKernelGetCpuClockCounterWide
	{ 0x2DC9709B, UNKNOWNNID, }, // _sceKernelGetCpuClockCounterLow
	{ 0xE9E652A9, UNKNOWNNID, }, // _sceKernelGetCpuClockCounterHigh
	{ 0x0FC68A56, UNKNOWNNID, }, // sceKernelSetPrimarySyscallHandler
	{ 0xF4D443F3, UNKNOWNNID, }, // sceKernelRegisterSystemCallTable
};

static nid_entry sceHprm_driver_nid[] = {
	{ 0x103DF40C, 0x0682085A, }, // sceHprmIsRemoteExist
	{ 0x1C5BC5A0, UNKNOWNNID, }, // sceHprmInit
	{ 0x588845DA, UNKNOWNNID, }, // sceHprmEnd
	{ 0x526BB7F4, UNKNOWNNID, }, // sceHprmSuspend
	{ 0x2C7B8B05, UNKNOWNNID, }, // sceHprmResume
	{ 0xD22913DB, UNKNOWNNID, }, // sceHprmSetConnectCallback
	{ 0xC7154136, UNKNOWNNID, }, // sceHprmRegisterCallback
	{ 0x444ED0B7, UNKNOWNNID, }, // sceHprmUnregisterCallback
	{ 0x208DB1BD, UNKNOWNNID, }, // sceHprmIsRemoteExist
	{ 0x7E69EDA4, UNKNOWNNID, }, // sceHprmIsHeadphoneExist
	{ 0x219C58F1, UNKNOWNNID, }, // sceHprmIsMicrophoneExist
	{ 0x4D1E622C, UNKNOWNNID, }, // sceHprmReset
	{ 0x7B038374, UNKNOWNNID, }, // sceHprmGetInternalState
	{ 0xF04591FA, UNKNOWNNID, }, // sceHprm_driver_F04591FA
	{ 0x971AE8FB, UNKNOWNNID, }, // sceHprm_driver_971AE8FB
	{ 0xBAD0828E, UNKNOWNNID, }, // sceHprmGetModel
	{ 0x1910B327, UNKNOWNNID, }, // sceHprmPeekCurrentKey
	{ 0x2BCEC83E, UNKNOWNNID, }, // sceHprmPeekLatch
	{ 0x40D2F9F0, UNKNOWNNID, }, // sceHprmReadLatch
};

static nid_entry sceCtrl_driver_nid[] = {
	{ 0x454455AC, 0x9F3038AC, },  //sceCtrlReadBufferPositive
	{ 0xC4AAD55F, 0x18654FC0, },  //sceCtrlPeekBufferPositive
	{ 0x28E71A16, 0x6CB49301, },  //sceCtrlSetSamplingMode
	{ 0x3E65A0EA, UNKNOWNNID, }, // sceCtrlInit
	{ 0xE03956E9, UNKNOWNNID, }, // sceCtrlEnd
	{ 0xC3F607F3, UNKNOWNNID, }, // sceCtrlSuspend
	{ 0xC245B57B, UNKNOWNNID, }, // sceCtrlResume
	{ 0x6A2774F3, UNKNOWNNID, }, // sceCtrlSetSamplingCycle
	{ 0x02BAAD91, UNKNOWNNID, }, // sceCtrlGetSamplingCycle
	{ 0x1F4011E6, UNKNOWNNID, }, // sceCtrlSetSamplingMode
	{ 0xDA6B76A1, UNKNOWNNID, }, // sceCtrlGetSamplingMode
	{ 0x3A622550, UNKNOWNNID, }, // sceCtrlPeekBufferPositive
	{ 0xC152080A, UNKNOWNNID, }, // sceCtrlPeekBufferNegative
	{ 0x1F803938, UNKNOWNNID, }, // sceCtrlReadBufferPositive
	{ 0x60B81F86, UNKNOWNNID, }, // sceCtrlReadBufferNegative
	{ 0xB1D0E5CD, UNKNOWNNID, }, // sceCtrlPeekLatch
	{ 0x0B588501, UNKNOWNNID, }, // sceCtrlReadLatch
	{ 0xA88E8D22, UNKNOWNNID, }, // sceCtrlSetIdleCancelKey
	{ 0xB7CEAED4, UNKNOWNNID, }, // sceCtrlGetIdleCancelKey
	{ 0x348D99D4, UNKNOWNNID, }, // sceCtrl_driver_348D99D4
	{ 0xAF5960F3, UNKNOWNNID, }, // sceCtrl_driver_AF5960F3
	{ 0xA68FD260, UNKNOWNNID, }, // sceCtrlClearRapidFire
	{ 0x6841BE1A, UNKNOWNNID, }, // sceCtrlSetRapidFire
	{ 0x7CA723DC, UNKNOWNNID, }, // sceCtrl_driver_7CA723DC
	{ 0x5E77BC8A, UNKNOWNNID, }, // sceCtrl_driver_5E77BC8A
	{ 0x5C56C779, UNKNOWNNID, }, // sceCtrl_driver_5C56C779
	{ 0xA7144800, UNKNOWNNID, }, // sceCtrlSetIdleCancelThreshold
	{ 0x687660FA, UNKNOWNNID, }, // sceCtrlGetIdleCancelThreshold
};

static nid_entry sceDisplay_driver_nid[] = {
	{ 0x4AB7497F, 0x37533141, },  //sceDisplaySetFrameBuf
	{ 0xB685BA36, 0xC30D327D, },  //sceDisplayWaitVblankStart
	{ 0xE56B11BA, 0x08A10838, },  //sceDisplayGetFrameBuf
	{ 0x776ADFDB, 0x89FD2128, },  //sceDisplaySetBacklightInternal
	{ 0x31C4BAA8, 0x3A5621E0, },  //sceDisplayGetBrightness
	{ 0x5B5AEFAD, 0xCCEEA6BB, },  //from 1.50
	{ 0x206276C2, UNKNOWNNID, }, // sceDisplayInit
	{ 0x7A10289D, UNKNOWNNID, }, // sceDisplayEnd
	{ 0x0E20F177, UNKNOWNNID, }, // sceDisplaySetMode
	{ 0xDEA197D4, UNKNOWNNID, }, // sceDisplayGetMode
	{ 0xDBA6C4C4, UNKNOWNNID, }, // sceDisplayGetFramePerSec
	{ 0x432D133F, UNKNOWNNID, }, // sceDisplayEnable
	{ 0x681EE6A7, UNKNOWNNID, }, // sceDisplayDisable
	{ 0x7ED59BC4, UNKNOWNNID, }, // sceDisplaySetHoldMode
	{ 0xA544C486, UNKNOWNNID, }, // sceDisplaySetResumeMode
	{ 0x63E22A26, UNKNOWNNID, }, // sceDisplay_driver_63E22A26
	{ 0x289D82FE, UNKNOWNNID, }, // sceDisplaySetFrameBuf
	{ 0xEEDA2E54, UNKNOWNNID, }, // sceDisplayGetFrameBuf
	{ 0xB4F378FA, UNKNOWNNID, }, // sceDisplayIsForeground
	{ 0xAC14F1FF, UNKNOWNNID, }, // sceDisplayGetForegroundLevel
	{ 0x9E3C6DC6, UNKNOWNNID, }, // sceDisplaySetBrightness
	{ 0x9C6EAAD7, UNKNOWNNID, }, // sceDisplayGetVcount
	{ 0x4D4E10EC, UNKNOWNNID, }, // sceDisplayIsVblank
	{ 0x69B53541, UNKNOWNNID, }, // sceDisplayGetVblankRest
	{ 0x36CDFADE, UNKNOWNNID, }, // sceDisplayWaitVblank
	{ 0x8EB9EC49, UNKNOWNNID, }, // sceDisplayWaitVblankCB
	{ 0x984C27E7, UNKNOWNNID, }, // sceDisplayWaitVblankStart
	{ 0x46F186C3, UNKNOWNNID, }, // sceDisplayWaitVblankStartCB
	{ 0x773DD3A3, UNKNOWNNID, }, // sceDisplayGetCurrentHcount
	{ 0x210EAB3A, UNKNOWNNID, }, // sceDisplayGetAccumulatedHcount
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
	{ 0x8C1009B2, UNKNOWNNID, }, // sceAudioOutput 
	{ 0x136CAF51, UNKNOWNNID, }, // sceAudioOutputBlocking 
	{ 0xE2D56B2D, UNKNOWNNID, }, // sceAudioOutputPanned 
	{ 0x13F592BC, UNKNOWNNID, }, // sceAudioOutputPannedBlocking 
	{ 0x5EC81C55, UNKNOWNNID, }, // sceAudioChReserve 
	{ 0x41EFADE7, UNKNOWNNID, }, // sceAudioOneshotOutput 
	{ 0x6FC46853, UNKNOWNNID, }, // sceAudioChRelease 
	{ 0xB011922F, UNKNOWNNID, }, // sceAudio_driver_B011922F 
	{ 0xCB2E439E, UNKNOWNNID, }, // sceAudioSetChannelDataLen 
	{ 0x95FD0C2D, UNKNOWNNID, }, // sceAudioChangeChannelConfig 
	{ 0xB7E1D8E7, UNKNOWNNID, }, // sceAudioChangeChannelVolume 
	{ 0x38553111, UNKNOWNNID, }, // sceAudio_driver_38553111 
	{ 0x5C37C0AE, UNKNOWNNID, }, // sceAudio_driver_5C37C0AE 
	{ 0xE0727056, UNKNOWNNID, }, // sceAudio_driver_E0727056 
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
};

static nid_entry sceSyscon_driver_nid[] = {
	{ 0x0A771482, UNKNOWNNID, }, // sceSysconInit
	{ 0x92D16FC7, UNKNOWNNID, }, // sceSysconEnd
	{ 0x081826B4, UNKNOWNNID, }, // sceSysconSuspend
	{ 0x56931095, UNKNOWNNID, }, // sceSysconResume
	{ 0x5EE92F3C, UNKNOWNNID, }, // sceSyscon_driver_5EE92F3C
	{ 0x5FF1D610, UNKNOWNNID, }, // sceSyscon_driver_5FF1D610
	{ 0xAD555CE5, UNKNOWNNID, }, // sceSyscon_driver_AD555CE5
	{ 0xF281805D, UNKNOWNNID, }, // sceSyscon_driver_F281805D
	{ 0xA068B3D0, UNKNOWNNID, }, // sceSysconSetAlarmCallback
	{ 0xE540E532, UNKNOWNNID, }, // sceSyscon_driver_E540E532
	{ 0xBBFB70C0, UNKNOWNNID, }, // sceSyscon_driver_BBFB70C0
	{ 0x805180D1, UNKNOWNNID, }, // sceSyscon_driver_805180D1
	{ 0x53072985, UNKNOWNNID, }, // sceSyscon_driver_53072985
	{ 0xF9193EC3, UNKNOWNNID, }, // sceSyscon_driver_F9193EC3
	{ 0x7479DB05, UNKNOWNNID, }, // sceSyscon_driver_7479DB05
	{ 0x6848D817, UNKNOWNNID, }, // sceSyscon_driver_6848D817
	{ 0x5B9ACC97, UNKNOWNNID, }, // sceSysconCmdExec
	{ 0x3AC3D2A4, UNKNOWNNID, }, // sceSysconCmdExecAsync
	{ 0x1602ED0D, UNKNOWNNID, }, // sceSysconCmdCancel
	{ 0xF350F666, UNKNOWNNID, }, // sceSysconCmdSync
	{ 0x86D4CAD8, UNKNOWNNID, }, // sceSyscon_driver_86D4CAD8
	{ 0x32CFD20F, UNKNOWNNID, }, // sceSysconIsLowBattery
	{ 0xEC0DE439, UNKNOWNNID, }, // sceSysconGetPowerSwitch
	{ 0xEA5B9823, UNKNOWNNID, }, // sceSyscon_driver_EA5B9823
	{ 0xE20D08FE, UNKNOWNNID, }, // sceSyscon_driver_E20D08FE
	{ 0xE0DDFE18, UNKNOWNNID, }, // sceSysconGetHPConnect
	{ 0xBDA16E46, UNKNOWNNID, }, // sceSysconGetWlanSwitch
	{ 0xF6BB4649, UNKNOWNNID, }, // sceSysconGetHoldSwitch
	{ 0x138747DE, UNKNOWNNID, }, // sceSysconGetUmdSwitch
	{ 0x71AE1BCE, UNKNOWNNID, }, // sceSyscon_driver_71AE1BCE
	{ 0x7016161C, UNKNOWNNID, }, // sceSysconGetWlanPowerStatus
	{ 0x48AB0E44, UNKNOWNNID, }, // sceSysconGetLeptonPowerCtrl
	{ 0x628F2351, UNKNOWNNID, }, // sceSysconGetMsPowerCtrl
	{ 0x3C739F57, UNKNOWNNID, }, // sceSyscon_driver_3C739F57
	{ 0xEC37C549, UNKNOWNNID, }, // sceSysconGetWlanPowerCtrl
	{ 0x8DDA4CA6, UNKNOWNNID, }, // sceSyscon_driver_8DDA4CA6
	{ 0x52B74B6C, UNKNOWNNID, }, // sceSyscon_driver_52B74B6C
	{ 0x1B17D3E3, UNKNOWNNID, }, // sceSyscon_driver_1B17D3E3
	{ 0x5F19C00F, UNKNOWNNID, }, // sceSyscon_driver_5F19C00F
	{ 0xCC04A978, UNKNOWNNID, }, // sceSysconGetWlanLedCtrl
	{ 0xE6B74CB9, UNKNOWNNID, }, // sceSysconNop
	{ 0x7EC5A957, UNKNOWNNID, }, // sceSyscon_driver_7EC5A957
	{ 0x7BCC5EAE, UNKNOWNNID, }, // sceSyscon_driver_7BCC5EAE
	{ 0x3B657A27, UNKNOWNNID, }, // sceSyscon_driver_3B657A27
	{ 0xFC32141A, UNKNOWNNID, }, // sceSyscon_driver_FC32141A
	{ 0xF775BC34, UNKNOWNNID, }, // sceSyscon_driver_F775BC34
	{ 0xA9AEF39F, UNKNOWNNID, }, // sceSyscon_driver_A9AEF39F
	{ 0xC4D66C1D, UNKNOWNNID, }, // sceSysconReadClock
	{ 0xC7634A7A, UNKNOWNNID, }, // sceSysconWriteClock
	{ 0x7A805EE4, UNKNOWNNID, }, // sceSysconReadAlarm
	{ 0x6C911742, UNKNOWNNID, }, // sceSysconWriteAlarm
	{ 0x65EB6096, UNKNOWNNID, }, // sceSyscon_driver_65EB6096
	{ 0xEB277C88, UNKNOWNNID, }, // sceSyscon_driver_EB277C88
	{ 0x992C22C2, UNKNOWNNID, }, // sceSysconSendSetParam
	{ 0x08234E6D, UNKNOWNNID, }, // sceSysconReceiveSetParam
	{ 0x882F0AAB, UNKNOWNNID, }, // sceSyscon_driver_882F0AAB
	{ 0x2EE82492, UNKNOWNNID, }, // sceSyscon_driver_2EE82492
	{ 0x8CBC7987, UNKNOWNNID, }, // sceSysconResetDevice
	{ 0x00E7B6C2, UNKNOWNNID, }, // sceSyscon_driver_00E7B6C2
	{ 0x44439604, UNKNOWNNID, }, // sceSysconCtrlHRPower
	{ 0xC8439C57, UNKNOWNNID, }, // sceSysconPowerStandby
	{ 0x91E183CB, UNKNOWNNID, }, // sceSysconPowerSuspend
	{ 0xE7E87741, UNKNOWNNID, }, // sceSyscon_driver_E7E87741
	{ 0xFB148FB6, UNKNOWNNID, }, // sceSyscon_driver_FB148FB6
	{ 0x01677F91, UNKNOWNNID, }, // sceSyscon_driver_01677F91
	{ 0xBE27FE66, UNKNOWNNID, }, // sceSysconCtrlPower
	{ 0x09721F7F, UNKNOWNNID, }, // sceSysconGetPowerStatus
	{ 0x18BFBE65, UNKNOWNNID, }, // sceSysconCtrlLED
	{ 0xD1B501E8, UNKNOWNNID, }, // sceSyscon_driver_D1B501E8
	{ 0x3DE38336, UNKNOWNNID, }, // sceSyscon_driver_3DE38336
	{ 0x2B9E6A06, UNKNOWNNID, }, // sceSysconGetPowerError
	{ 0x8A4519F5, UNKNOWNNID, }, // sceSysconCtrlLeptonPower
	{ 0x99BBB24C, UNKNOWNNID, }, // sceSysconCtrlMsPower
	{ 0xF0ED3255, UNKNOWNNID, }, // sceSysconCtrlWlanPower
	{ 0x3C6DB1C5, UNKNOWNNID, }, // sceSyscon_driver_3C6DB1C5
	{ 0xB2558E37, UNKNOWNNID, }, // sceSyscon_driver_B2558E37
	{ 0xE5E35721, UNKNOWNNID, }, // sceSyscon_driver_E5E35721
	{ 0x9478F399, UNKNOWNNID, }, // sceSyscon_driver_9478F399
	{ 0x806D4D6C, UNKNOWNNID, }, // sceSyscon_driver_806D4D6C
	{ 0xD8471760, UNKNOWNNID, }, // sceSyscon_driver_D8471760
	{ 0xEAB13FBE, UNKNOWNNID, }, // sceSyscon_driver_EAB13FBE
	{ 0xC5075828, UNKNOWNNID, }, // sceSyscon_driver_C5075828
	{ 0xE448FD3F, UNKNOWNNID, }, // sceSysconBatteryNop
	{ 0x6A53F3F8, UNKNOWNNID, }, // sceSysconBatteryGetStatusCap
	{ 0x70C10E61, UNKNOWNNID, }, // sceSysconBatteryGetTemp
	{ 0x8BDEBB1E, UNKNOWNNID, }, // sceSysconBatteryGetVolt
	{ 0x373EC933, UNKNOWNNID, }, // sceSysconBatteryGetElec
	{ 0x82861DE2, UNKNOWNNID, }, // sceSyscon_driver_82861DE2
	{ 0x876CA580, UNKNOWNNID, }, // sceSysconBatteryGetCap
	{ 0x71135D7D, UNKNOWNNID, }, // sceSyscon_driver_71135D7D
	{ 0x7CBD4522, UNKNOWNNID, }, // sceSyscon_driver_7CBD4522
	{ 0x284FE366, UNKNOWNNID, }, // sceSyscon_driver_284FE366
	{ 0x75025299, UNKNOWNNID, }, // sceSysconBatteryGetStatus
	{ 0xB5105D51, UNKNOWNNID, }, // sceSysconBatteryGetCycle
	{ 0xD5340103, UNKNOWNNID, }, // sceSysconBatteryGetSerial
	{ 0xFA4C4518, UNKNOWNNID, }, // sceSysconBatteryGetInfo
	{ 0xB71B98A8, UNKNOWNNID, }, // sceSyscon_driver_B71B98A8
	{ 0x87671B18, UNKNOWNNID, }, // sceSyscon_driver_87671B18
	{ 0x75D22BF8, UNKNOWNNID, }, // sceSyscon_driver_75D22BF8
	{ 0x4C539345, UNKNOWNNID, }, // sceSyscon_driver_4C539345
	{ 0x4C0EE2FA, UNKNOWNNID, }, // sceSyscon_driver_4C0EE2FA
	{ 0x1165C864, UNKNOWNNID, }, // sceSyscon_driver_1165C864
	{ 0x68EF0BEF, UNKNOWNNID, }, // sceSyscon_driver_68EF0BEF
	{ 0x36E28C5F, UNKNOWNNID, }, // sceSysconBatteryAuth
	{ 0x08DA3752, UNKNOWNNID, }, // sceSyscon_driver_08DA3752
	{ 0x9C4E3CA9, UNKNOWNNID, }, // sceSyscon_driver_9C4E3CA9
	{ 0x34C36FF9, UNKNOWNNID, }, // sceSyscon_driver_34C36FF9
	{ 0xB8919D79, UNKNOWNNID, }, // sceSysconMsOn
	{ 0x7BE86143, UNKNOWNNID, }, // sceSysconMsOff
	{ 0x0E8560F9, UNKNOWNNID, }, // sceSysconWlanOn
	{ 0x1B60C8AD, UNKNOWNNID, }, // sceSysconWlanOff
	{ 0xE00BFC9E, UNKNOWNNID, }, // sceSyscon_driver_E00BFC9E
	{ 0xC8D97773, UNKNOWNNID, }, // sceSyscon_driver_C8D97773
};

static nid_entry sceSysEventForKernel_nid[] = {
	{ 0xAEB300AE, UNKNOWNNID, }, // sceKernelIsRegisterSysEventHandler
	{ 0xCD9E4BB5, UNKNOWNNID, }, // sceKernelRegisterSysEventHandler
	{ 0xD7D3FDCD, UNKNOWNNID, }, // sceKernelUnregisterSysEventHandler
	{ 0x36331294, UNKNOWNNID, }, // sceKernelSysEventDispatch
	{ 0x68D55505, UNKNOWNNID, }, // sceKernelReferSysEventHandler
};

static nid_entry UtilsForKernel_nid[] = {
	{ 0x80FE032E, UNKNOWNNID, }, // sceUtilsKernelDcacheWBinvRange
	{ 0xC8186A58, UNKNOWNNID, }, // sceKernelUtilsMd5Digest
	{ 0x9E5C5086, UNKNOWNNID, }, // sceKernelUtilsMd5BlockInit
	{ 0x61E1E525, UNKNOWNNID, }, // sceKernelUtilsMd5BlockUpdate
	{ 0xB8D24E78, UNKNOWNNID, }, // sceKernelUtilsMd5BlockResult
	{ 0x840259F1, UNKNOWNNID, }, // sceKernelUtilsSha1Digest
	{ 0xF8FCD5BA, UNKNOWNNID, }, // sceKernelUtilsSha1BlockInit
	{ 0x346F6DA8, UNKNOWNNID, }, // sceKernelUtilsSha1BlockUpdate
	{ 0x585F1C09, UNKNOWNNID, }, // sceKernelUtilsSha1BlockResult
	{ 0xE860E75E, UNKNOWNNID, }, // sceKernelUtilsMt19937Init
	{ 0x06FB8A63, UNKNOWNNID, }, // sceKernelUtilsMt19937UInt
	{ 0x193D4036, UNKNOWNNID, }, // sceKernelSetGPIMask
	{ 0x95035FEF, UNKNOWNNID, }, // sceKernelSetGPOMask
	{ 0x37FB5C42, UNKNOWNNID, }, // sceKernelGetGPI
	{ 0x6AD345D7, UNKNOWNNID, }, // sceKernelSetGPO
	{ 0x7B7ED3FD, UNKNOWNNID, }, // sceKernelRegisterLibcRtcFunc
	{ 0x6151A7C3, UNKNOWNNID, }, // sceKernelReleaseLibcRtcFunc
	{ 0x91E4F6A7, UNKNOWNNID, }, // sceKernelLibcClock
	{ 0x27CC57F0, UNKNOWNNID, }, // sceKernelLibcTime
	{ 0x71EC4271, UNKNOWNNID, }, // sceKernelLibcGettimeofday
	{ 0x79D1C3FA, UNKNOWNNID, }, // sceKernelDcacheWritebackAll
	{ 0xB435DEC5, UNKNOWNNID, }, // sceKernelDcacheWritebackInvalidateAll
	{ 0x864A9D72, UNKNOWNNID, }, // sceKernelDcacheInvalidateAll
	{ 0x3EE30821, UNKNOWNNID, }, // sceKernelDcacheWritebackRange
	{ 0x34B9FA9E, UNKNOWNNID, }, // sceKernelDcacheWritebackInvalidateRange
	{ 0xBFA98062, UNKNOWNNID, }, // sceKernelDcacheInvalidateRange
	{ 0x80001C4C, UNKNOWNNID, }, // sceKernelDcacheProbe
	{ 0x16641D70, UNKNOWNNID, }, // sceKernelDcacheReadTag
	{ 0x920F104A, UNKNOWNNID, }, // sceKernelIcacheInvalidateAll
	{ 0xC2DF770E, UNKNOWNNID, }, // sceKernelIcacheInvalidateRange
	{ 0x4FD31C9D, UNKNOWNNID, }, // sceKernelIcacheProbe
	{ 0xFB05FAD0, UNKNOWNNID, }, // sceKernelIcacheReadTag
	{ 0x78934841, UNKNOWNNID, }, // sceKernelGzipDecompress
	{ 0xE0CE3E29, UNKNOWNNID, }, // sceKernelGzipIsValid
	{ 0xB0E9C31F, UNKNOWNNID, }, // sceKernelGzipGetInfo
	{ 0xE0E6BA96, UNKNOWNNID, }, // sceKernelGzipGetName
	{ 0x8C1FBE04, UNKNOWNNID, }, // sceKernelGzipGetComment
	{ 0x23FFC828, UNKNOWNNID, }, // sceKernelGzipGetCompressedData
	{ 0xE8DB3CE6, UNKNOWNNID, }, // sceKernelDeflateDecompress
};

static nid_entry sceAudioRouting_driver_nid[] = {
	{ 0x28235C56, UNKNOWNNID, }, // sceAudioRoutingGetVolumeMode
	{ 0x36FD8AA9, UNKNOWNNID, }, // sceAudioRoutingSetMode
	{ 0x39240E7D, UNKNOWNNID, }, // sceAudioRoutingGetMode
	{ 0xBB548475, UNKNOWNNID, }, // sceAudioRoutingSetVolumeMode
};

static nid_entry SysclibForKernel_nid[] = {
	{ 0x476FD94A, UNKNOWNNID, }, // strcat
	{ 0x89B79CB1, UNKNOWNNID, }, // strcspn
	{ 0xD1CD40E5, UNKNOWNNID, }, // index
	{ 0x243665ED, UNKNOWNNID, }, // rindex
	{ 0x90C5573D, UNKNOWNNID, }, // strnlen
	{ 0x0DFB7B6C, UNKNOWNNID, }, // strpbrk
	{ 0x62AE052F, UNKNOWNNID, }, // strspn
	{ 0x0D188658, UNKNOWNNID, }, // strstr
	{ 0x87F8D2DA, UNKNOWNNID, }, // strtok
	{ 0x1AB53A58, UNKNOWNNID, }, // strtok_r
	{ 0x47DD934D, UNKNOWNNID, }, // strtol
	{ 0x1D83F344, UNKNOWNNID, }, // atob
	{ 0x6A7900E1, UNKNOWNNID, }, // strtoul
	{ 0x8AF6B8F8, UNKNOWNNID, }, // SysclibForKernel_8AF6B8F8
	{ 0xDF17F4A2, UNKNOWNNID, }, // SysclibForKernel_DF17F4A2
	{ 0x7DEE14DE, UNKNOWNNID, }, // SysclibForKernel_7DEE14DE
	{ 0x5E8E5F42, UNKNOWNNID, }, // SysclibForKernel_5E8E5F42
	{ 0xC0AB8932, UNKNOWNNID, }, // strcmp
	{ 0xEC6F1CF2, UNKNOWNNID, }, // strcpy
	{ 0xB1DC2AE8, UNKNOWNNID, }, // strchr
	{ 0x4C0E0274, UNKNOWNNID, }, // strrchr
	{ 0x7AB35214, UNKNOWNNID, }, // strncmp
	{ 0xB49A7697, UNKNOWNNID, }, // strncpy
	{ 0x52DF196C, UNKNOWNNID, }, // strlen
	{ 0xD3D1A3B9, UNKNOWNNID, }, // strncat
	{ 0x68A78817, UNKNOWNNID, }, // memchr
	{ 0xAB7592FF, UNKNOWNNID, }, // memcpy
	{ 0x10F3BB61, UNKNOWNNID, }, // memset
	{ 0x81D0D1F7, UNKNOWNNID, }, // memcmp
	{ 0xA48D2592, UNKNOWNNID, }, // memmove
	{ 0x097049BD, UNKNOWNNID, }, // bcopy
	{ 0x7F8A6F23, UNKNOWNNID, }, // bcmp
	{ 0x86FEFCE9, UNKNOWNNID, }, // bzero
	{ 0xCE2F7487, UNKNOWNNID, }, // toupper
	{ 0x3EC5BBF6, UNKNOWNNID, }, // tolower
	{ 0x32C767F2, UNKNOWNNID, }, // look_ctype_table
	{ 0xD887CACD, UNKNOWNNID, }, // get_ctype_table
	{ 0x87C78FB6, UNKNOWNNID, }, // prnt
	{ 0x7661E728, UNKNOWNNID, }, // sprintf
	{ 0x909C228B, UNKNOWNNID, }, // setjmp
	{ 0x18FE80DB, UNKNOWNNID, }, // longjmp
	{ 0x1493EBD9, UNKNOWNNID, }, // wmemset
};

static nid_entry ThreadManForKernel_nid[] = {
	{ 0x0C106E53, UNKNOWNNID, }, // sceKernelRegisterThreadEventHandler
	{ 0x72F3C145, UNKNOWNNID, }, // sceKernelReleaseThreadEventHandler
	{ 0x369EEB6B, UNKNOWNNID, }, // sceKernelReferThreadEventHandlerStatus
	{ 0xE81CAF8F, UNKNOWNNID, }, // sceKernelCreateCallback
	{ 0xEDBA5844, UNKNOWNNID, }, // sceKernelDeleteCallback
	{ 0xC11BA8C4, UNKNOWNNID, }, // sceKernelNotifyCallback
	{ 0xBA4051D6, UNKNOWNNID, }, // sceKernelCancelCallback
	{ 0x2A3D44FF, UNKNOWNNID, }, // sceKernelGetCallbackCount
	{ 0x349D6D6C, UNKNOWNNID, }, // sceKernelCheckCallback
	{ 0x730ED8BC, UNKNOWNNID, }, // sceKernelReferCallbackStatus
	{ 0x9ACE131E, UNKNOWNNID, }, // sceKernelSleepThread
	{ 0x82826F70, UNKNOWNNID, }, // sceKernelSleepThreadCB
	{ 0xD59EAD2F, UNKNOWNNID, }, // sceKernelWakeupThread
	{ 0xFCCFAD26, UNKNOWNNID, }, // sceKernelCancelWakeupThread
	{ 0x9944F31F, UNKNOWNNID, }, // sceKernelSuspendThread
	{ 0x75156E8F, UNKNOWNNID, }, // sceKernelResumeThread
	{ 0x8FD9F70C, UNKNOWNNID, }, // sceKernelSuspendAllUserThreads
	{ 0x278C0DF5, UNKNOWNNID, }, // sceKernelWaitThreadEnd
	{ 0x840E8133, UNKNOWNNID, }, // sceKernelWaitThreadEndCB
	{ 0xCEADEB47, UNKNOWNNID, }, // sceKernelDelayThread
	{ 0x68DA9E36, UNKNOWNNID, }, // sceKernelDelayThreadCB
	{ 0xBD123D9E, UNKNOWNNID, }, // sceKernelDelaySysClockThread
	{ 0x1181E963, UNKNOWNNID, }, // sceKernelDelaySysClockThreadCB
	{ 0xD6DA4BA1, UNKNOWNNID, }, // sceKernelCreateSema
	{ 0x28B6489C, UNKNOWNNID, }, // sceKernelDeleteSema
	{ 0x3F53E640, UNKNOWNNID, }, // sceKernelSignalSema
	{ 0x4E3A1105, UNKNOWNNID, }, // sceKernelWaitSema
	{ 0x6D212BAC, UNKNOWNNID, }, // sceKernelWaitSemaCB
	{ 0x58B1F937, UNKNOWNNID, }, // sceKernelPollSema
	{ 0x8FFDF9A2, UNKNOWNNID, }, // sceKernelCancelSema
	{ 0xBC6FEBC5, UNKNOWNNID, }, // sceKernelReferSemaStatus
	{ 0x55C20A00, UNKNOWNNID, }, // sceKernelCreateEventFlag
	{ 0xEF9E4C70, UNKNOWNNID, }, // sceKernelDeleteEventFlag
	{ 0x1FB15A32, UNKNOWNNID, }, // sceKernelSetEventFlag
	{ 0x812346E4, UNKNOWNNID, }, // sceKernelClearEventFlag
	{ 0x402FCF22, UNKNOWNNID, }, // sceKernelWaitEventFlag
	{ 0x328C546A, UNKNOWNNID, }, // sceKernelWaitEventFlagCB
	{ 0x30FD48F0, UNKNOWNNID, }, // sceKernelPollEventFlag
	{ 0xCD203292, UNKNOWNNID, }, // sceKernelCancelEventFlag
	{ 0xA66B0120, UNKNOWNNID, }, // sceKernelReferEventFlagStatus
	{ 0x8125221D, UNKNOWNNID, }, // sceKernelCreateMbx
	{ 0x86255ADA, UNKNOWNNID, }, // sceKernelDeleteMbx
	{ 0xE9B3061E, UNKNOWNNID, }, // sceKernelSendMbx
	{ 0x18260574, UNKNOWNNID, }, // sceKernelReceiveMbx
	{ 0xF3986382, UNKNOWNNID, }, // sceKernelReceiveMbxCB
	{ 0x0D81716A, UNKNOWNNID, }, // sceKernelPollMbx
	{ 0x87D4DD36, UNKNOWNNID, }, // sceKernelCancelReceiveMbx
	{ 0xA8E8C846, UNKNOWNNID, }, // sceKernelReferMbxStatus
	{ 0x7C0DC2A0, UNKNOWNNID, }, // sceKernelCreateMsgPipe
	{ 0xF0B7DA1C, UNKNOWNNID, }, // sceKernelDeleteMsgPipe
	{ 0x876DBFAD, UNKNOWNNID, }, // sceKernelSendMsgPipe
	{ 0x7C41F2C2, UNKNOWNNID, }, // sceKernelSendMsgPipeCB
	{ 0x884C9F90, UNKNOWNNID, }, // sceKernelTrySendMsgPipe
	{ 0x74829B76, UNKNOWNNID, }, // sceKernelReceiveMsgPipe
	{ 0xFBFA697D, UNKNOWNNID, }, // sceKernelReceiveMsgPipeCB
	{ 0xDF52098F, UNKNOWNNID, }, // sceKernelTryReceiveMsgPipe
	{ 0x349B864D, UNKNOWNNID, }, // sceKernelCancelMsgPipe
	{ 0x33BE4024, UNKNOWNNID, }, // sceKernelReferMsgPipeStatus
	{ 0x56C039B5, UNKNOWNNID, }, // sceKernelCreateVpl
	{ 0x89B3D48C, UNKNOWNNID, }, // sceKernelDeleteVpl
	{ 0xBED27435, UNKNOWNNID, }, // sceKernelAllocateVpl
	{ 0xEC0A693F, UNKNOWNNID, }, // sceKernelAllocateVplCB
	{ 0xAF36D708, UNKNOWNNID, }, // sceKernelTryAllocateVpl
	{ 0xB736E9FF, UNKNOWNNID, }, // sceKernelFreeVpl
	{ 0x1D371B8A, UNKNOWNNID, }, // sceKernelCancelVpl
	{ 0x39810265, UNKNOWNNID, }, // sceKernelReferVplStatus
	{ 0xC07BB470, UNKNOWNNID, }, // sceKernelCreateFpl
	{ 0xED1410E0, UNKNOWNNID, }, // sceKernelDeleteFpl
	{ 0xD979E9BF, UNKNOWNNID, }, // sceKernelAllocateFpl
	{ 0xE7282CB6, UNKNOWNNID, }, // sceKernelAllocateFplCB
	{ 0x623AE665, UNKNOWNNID, }, // sceKernelTryAllocateFpl
	{ 0xF6414A71, UNKNOWNNID, }, // sceKernelFreeFpl
	{ 0xA8AA591F, UNKNOWNNID, }, // sceKernelCancelFpl
	{ 0xD8199E4C, UNKNOWNNID, }, // sceKernelReferFplStatus
	{ 0x110DEC9A, UNKNOWNNID, }, // sceKernelUSec2SysClock
	{ 0xC8CD158C, UNKNOWNNID, }, // sceKernelUSec2SysClockWide
	{ 0xBA6B92E2, UNKNOWNNID, }, // sceKernelSysClock2USec
	{ 0xE1619D7C, UNKNOWNNID, }, // sceKernelSysClock2USecWide
	{ 0xDB738F35, UNKNOWNNID, }, // sceKernelGetSystemTime
	{ 0x82BC5777, UNKNOWNNID, }, // sceKernelGetSystemTimeWide
	{ 0x369ED59D, UNKNOWNNID, }, // sceKernelGetSystemTimeLow
	{ 0x6652B8CA, UNKNOWNNID, }, // sceKernelSetAlarm
	{ 0xB2C25152, UNKNOWNNID, }, // sceKernelSetSysClockAlarm
	{ 0x7E65B999, UNKNOWNNID, }, // sceKernelCancelAlarm
	{ 0xDAA3F564, UNKNOWNNID, }, // sceKernelReferAlarmStatus
	{ 0x20FFF560, UNKNOWNNID, }, // sceKernelCreateVTimer
	{ 0x328F9E52, UNKNOWNNID, }, // sceKernelDeleteVTimer
	{ 0xB3A59970, UNKNOWNNID, }, // sceKernelGetVTimerBase
	{ 0xB7C18B77, UNKNOWNNID, }, // sceKernelGetVTimerBaseWide
	{ 0x034A921F, UNKNOWNNID, }, // sceKernelGetVTimerTime
	{ 0xC0B3FFD2, UNKNOWNNID, }, // sceKernelGetVTimerTimeWide
	{ 0x542AD630, UNKNOWNNID, }, // sceKernelSetVTimerTime
	{ 0xFB6425C3, UNKNOWNNID, }, // sceKernelSetVTimerTimeWide
	{ 0xC68D9437, UNKNOWNNID, }, // sceKernelStartVTimer
	{ 0xD0AEEE87, UNKNOWNNID, }, // sceKernelStopVTimer
	{ 0xD8B299AE, UNKNOWNNID, }, // sceKernelSetVTimerHandler
	{ 0x53B00E9A, UNKNOWNNID, }, // sceKernelSetVTimerHandlerWide
	{ 0xD2D615EF, UNKNOWNNID, }, // sceKernelCancelVTimerHandler
	{ 0x5F32BEAA, UNKNOWNNID, }, // sceKernelReferVTimerStatus
	{ 0x04E72261, UNKNOWNNID, }, // sceKernelAllocateKTLS
	{ 0xD198B811, UNKNOWNNID, }, // sceKernelFreeKTLS
	{ 0x3AD875C3, UNKNOWNNID, }, // sceKernelGetThreadKTLS
	{ 0xA249EAAE, UNKNOWNNID, }, // sceKernelGetKTLS
	{ 0xB50F4E46, UNKNOWNNID, }, // ThreadManForKernel_B50F4E46
	{ 0x446D8DE6, UNKNOWNNID, }, // sceKernelCreateThread
	{ 0x9FA03CD3, UNKNOWNNID, }, // sceKernelDeleteThread
	{ 0xF475845D, UNKNOWNNID, }, // sceKernelStartThread
	{ 0xAA73C935, UNKNOWNNID, }, // sceKernelExitThread
	{ 0x809CE29B, UNKNOWNNID, }, // sceKernelExitDeleteThread
	{ 0x616403BA, UNKNOWNNID, }, // sceKernelTerminateThread
	{ 0x383F7BCC, UNKNOWNNID, }, // sceKernelTerminateDeleteThread
	{ 0x3AD58B8C, UNKNOWNNID, }, // sceKernelSuspendDispatchThread
	{ 0x27E22EC2, UNKNOWNNID, }, // sceKernelResumeDispatchThread
	{ 0xEA748E31, UNKNOWNNID, }, // sceKernelChangeCurrentThreadAttr
	{ 0x71BC9871, UNKNOWNNID, }, // sceKernelChangeThreadPriority
	{ 0x912354A7, UNKNOWNNID, }, // sceKernelRotateThreadReadyQueue
	{ 0x2C34E053, UNKNOWNNID, }, // sceKernelReleaseWaitThread
	{ 0x293B45B8, UNKNOWNNID, }, // sceKernelGetThreadId
	{ 0x94AA61EE, UNKNOWNNID, }, // sceKernelGetThreadCurrentPriority
	{ 0x3B183E26, UNKNOWNNID, }, // sceKernelGetThreadExitStatus
	{ 0xF6427665, UNKNOWNNID, }, // sceKernelGetUserLevel
	{ 0x85A2A5BF, UNKNOWNNID, }, // sceKernelIsUserModeThread
	{ 0xDD55A192, UNKNOWNNID, }, // sceKernelGetSyscallRA
	{ 0xBC31C1B9, UNKNOWNNID, }, // sceKernelExtendKernelStack
	{ 0x4FE44D5E, UNKNOWNNID, }, // sceKernelCheckThreadKernelStack
	{ 0xD13BDE95, UNKNOWNNID, }, // sceKernelCheckThreadStack
	{ 0x52089CA1, UNKNOWNNID, }, // sceKernelGetThreadStackFreeSize
	{ 0xD890B370, UNKNOWNNID, }, // sceKernelGetThreadKernelStackFreeSize
	{ 0x17C1684E, UNKNOWNNID, }, // sceKernelReferThreadStatus
	{ 0xFFC36A14, UNKNOWNNID, }, // sceKernelReferThreadRunStatus
	{ 0x2D69D086, UNKNOWNNID, }, // ThreadManForKernel_2D69D086
	{ 0xFCB5EB49, UNKNOWNNID, }, // sceKernelGetSystemStatusFlag
	{ 0x627E6F3A, UNKNOWNNID, }, // sceKernelReferSystemStatus
	{ 0x94416130, UNKNOWNNID, }, // sceKernelGetThreadmanIdList
	{ 0x57CF62DD, UNKNOWNNID, }, // sceKernelGetThreadmanIdType
};

static nid_entry InitForKernel_nid[] = {
	{ 0x1D3256BA, UNKNOWNNID, }, // sceKernelRegisterChunk
	{ 0x27932388, UNKNOWNNID, }, // sceKernelBootFrom
	{ 0x2C6E9FE9, UNKNOWNNID, }, // sceKernelGetChunk
	{ 0x33D30F49, UNKNOWNNID, }, // InitForKernel_33D30F49
	{ 0x7233B5BC, UNKNOWNNID, }, // InitForKernel_7233B5BC
	{ 0x7A2333AD, UNKNOWNNID, }, // sceKernelInitApitype
	{ 0x9F9AE99C, UNKNOWNNID, }, // InitForKernel_9F9AE99C
	{ 0xA6E71B93, UNKNOWNNID, }, // sceKernelInitFileName
	{ 0xC4F1BA33, UNKNOWNNID, }, // InitForKernel_C4F1BA33
	{ 0xCE88E870, UNKNOWNNID, }, // sceKernelReleaseChunk
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

static nid_entry sceIdStorage_driver_nid[] = {
	{ 0xAB129D20, UNKNOWNNID, }, // sceIdStorageInit
	{ 0x2CE0BE69, UNKNOWNNID, }, // sceIdStorageEnd
	{ 0xF77565B6, UNKNOWNNID, }, // sceIdStorageSuspend
	{ 0xFE51173D, UNKNOWNNID, }, // sceIdStorageResume
	{ 0xEB830733, UNKNOWNNID, }, // sceIdStorageGetLeafSize
	{ 0xFEFA40C2, UNKNOWNNID, }, // sceIdStorageIsFormatted
	{ 0x2D633688, UNKNOWNNID, }, // sceIdStorageIsReadOnly
	{ 0xB9069BAD, UNKNOWNNID, }, // sceIdStorageIsDirty
	{ 0x958089DB, UNKNOWNNID, }, // sceIdStorageFormat
	{ 0xF4BCB3EE, UNKNOWNNID, }, // sceIdStorageUnformat
	{ 0xEB00C509, UNKNOWNNID, }, // sceIdStorageReadLeaf
	{ 0x1FA4D135, UNKNOWNNID, }, // sceIdStorageWriteLeaf
	{ 0x08A471A6, UNKNOWNNID, }, // sceIdStorageCreateLeaf
	{ 0x2C97AB36, UNKNOWNNID, }, // sceIdStorageDeleteLeaf
	{ 0x99ACCB71, UNKNOWNNID, }, // sceIdStorage_driver_99ACCB71
	{ 0x37833CB8, UNKNOWNNID, }, // sceIdStorage_driver_37833CB8
	{ 0x31E08AFB, UNKNOWNNID, }, // sceIdStorageEnumId
	{ 0x6FE062D1, UNKNOWNNID, }, // sceIdStorageLookup
	{ 0x683AAC10, UNKNOWNNID, }, // sceIdStorageUpdate
	{ 0x3AD32523, UNKNOWNNID, }, // sceIdStorageFlush
};

static nid_entry sceSysreg_driver_nid[] = {
	{ 0x9C863542, UNKNOWNNID, }, // sceSysregInit
	{ 0xF2DEAA14, UNKNOWNNID, }, // sceSysregEnd
	{ 0xE88B77ED, UNKNOWNNID, }, // sceSysreg_driver_E88B77ED
	{ 0xCD0F6715, UNKNOWNNID, }, // sceSysreg_driver_CD0F6715
	{ 0x844AF6BD, UNKNOWNNID, }, // sceSysreg_driver_844AF6BD
	{ 0xE2A5D1EE, UNKNOWNNID, }, // sceSysreg_driver_E2A5D1EE
	{ 0x4F46EEDE, UNKNOWNNID, }, // sceSysreg_driver_4F46EEDE
	{ 0x8F4F4E96, UNKNOWNNID, }, // sceSysreg_driver_8F4F4E96
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
	{ 0xB21B6CBF, UNKNOWNNID, }, // sceSysreg_driver_B21B6CBF
	{ 0xBB3623DF, UNKNOWNNID, }, // sceSysreg_driver_BB3623DF
	{ 0x53A6838B, UNKNOWNNID, }, // sceSysreg_driver_53A6838B
	{ 0xB4560C45, UNKNOWNNID, }, // sceSysreg_driver_B4560C45
	{ 0xDCA57573, UNKNOWNNID, }, // sceSysreg_driver_DCA57573
	{ 0x44F6CDA7, UNKNOWNNID, }, // sceSysregMeBusClockEnable
	{ 0x158AD4FC, UNKNOWNNID, }, // sceSysregMeBusClockDisable
	{ 0x4D4CE2B8, UNKNOWNNID, }, // sceSysreg_driver_4D4CE2B8
	{ 0x789BD609, UNKNOWNNID, }, // sceSysreg_driver_789BD609
	{ 0x391CE1C0, UNKNOWNNID, }, // sceSysreg_driver_391CE1C0
	{ 0x82D35024, UNKNOWNNID, }, // sceSysreg_driver_82D35024
	{ 0xAF904657, UNKNOWNNID, }, // sceSysreg_driver_AF904657
	{ 0x438AECE9, UNKNOWNNID, }, // sceSysreg_driver_438AECE9
	{ 0x678AD3ED, UNKNOWNNID, }, // sceSysregDmacplusBusClockEnable
	{ 0x168C09B8, UNKNOWNNID, }, // sceSysregDmacplusBusClockDisable
	{ 0x7E7EBC20, UNKNOWNNID, }, // sceSysregDmacBusClockEnable
	{ 0xA265C719, UNKNOWNNID, }, // sceSysregDmacBusClockDisable
	{ 0x4F5AFBBE, UNKNOWNNID, }, // sceSysregKirkBusClockEnable
	{ 0x845DD1A6, UNKNOWNNID, }, // sceSysregKirkBusClockDisable
	{ 0x16909002, UNKNOWNNID, }, // sceSysregAtaBusClockEnable
	{ 0xB6C10DF0, UNKNOWNNID, }, // sceSysregAtaBusClockDisable
	{ 0x3E216017, UNKNOWNNID, }, // sceSysregUsbBusClockEnable
	{ 0xBFBABB63, UNKNOWNNID, }, // sceSysregUsbBusClockDisable
	{ 0x4716E71E, UNKNOWNNID, }, // sceSysregMsifBusClockEnable
	{ 0x826430C0, UNKNOWNNID, }, // sceSysregMsifBusClockDisable
	{ 0x7CC6CBFD, UNKNOWNNID, }, // sceSysreg_driver_7CC6CBFD
	{ 0xEE6B9411, UNKNOWNNID, }, // sceSysreg_driver_EE6B9411
	{ 0xF97D9D73, UNKNOWNNID, }, // sceSysregEmcsmBusClockEnable
	{ 0x2D0F7755, UNKNOWNNID, }, // sceSysregEmcsmBusClockDisable
	{ 0x63B9EB65, UNKNOWNNID, }, // sceSysreg_driver_63B9EB65
	{ 0xE1AA9788, UNKNOWNNID, }, // sceSysreg_driver_E1AA9788
	{ 0xAA63C8BD, UNKNOWNNID, }, // sceSysregAudioBusClockEnable
	{ 0x054AC8C6, UNKNOWNNID, }, // sceSysregAudioBusClockDisable
	{ 0x6B01D71B, UNKNOWNNID, }, // sceSysreg_driver_6B01D71B
	{ 0xFC186A83, UNKNOWNNID, }, // sceSysreg_driver_FC186A83
	{ 0x7234EA80, UNKNOWNNID, }, // sceSysreg_driver_7234EA80
	{ 0x38EC3281, UNKNOWNNID, }, // sceSysreg_driver_38EC3281
	{ 0x31154490, UNKNOWNNID, }, // sceSysreg_driver_31154490
	{ 0x8E2D835D, UNKNOWNNID, }, // sceSysreg_driver_8E2D835D
	{ 0x8835D1E1, UNKNOWNNID, }, // sceSysreg_driver_8835D1E1
	{ 0x8B90B8B5, UNKNOWNNID, }, // sceSysreg_driver_8B90B8B5
	{ 0x7725CA08, UNKNOWNNID, }, // sceSysreg_driver_7725CA08
	{ 0xA3E4154C, UNKNOWNNID, }, // sceSysreg_driver_A3E4154C
	{ 0xE8533DCA, UNKNOWNNID, }, // sceSysreg_driver_E8533DCA
	{ 0xF6D83AD0, UNKNOWNNID, }, // sceSysreg_driver_F6D83AD0
	{ 0xA9CD1C1F, UNKNOWNNID, }, // sceSysreg_driver_A9CD1C1F
	{ 0x2F216F38, UNKNOWNNID, }, // sceSysreg_driver_2F216F38
	{ 0xA24C242A, UNKNOWNNID, }, // sceSysreg_driver_A24C242A
	{ 0xE89243BE, UNKNOWNNID, }, // sceSysreg_driver_E89243BE
	{ 0x7DCA8302, UNKNOWNNID, }, // sceSysreg_driver_7DCA8302
	{ 0x45225F8F, UNKNOWNNID, }, // sceSysreg_driver_45225F8F
	{ 0xD74D3AB6, UNKNOWNNID, }, // sceSysreg_driver_D74D3AB6
	{ 0xAD7C4ACB, UNKNOWNNID, }, // sceSysreg_driver_AD7C4ACB
	{ 0xDC68A93F, UNKNOWNNID, }, // sceSysreg_driver_DC68A93F
	{ 0x94B0323C, UNKNOWNNID, }, // sceSysreg_driver_94B0323C
	{ 0x6417CDD6, UNKNOWNNID, }, // sceSysreg_driver_6417CDD6
	{ 0x20388C9E, UNKNOWNNID, }, // sceSysreg_driver_20388C9E
	{ 0xE3AECFFA, UNKNOWNNID, }, // sceSysreg_driver_E3AECFFA
	{ 0x3BBD0C0C, UNKNOWNNID, }, // sceSysreg_driver_3BBD0C0C
	{ 0xC1DA05D2, UNKNOWNNID, }, // sceSysreg_driver_C1DA05D2
	{ 0xDE170397, UNKNOWNNID, }, // sceSysreg_driver_DE170397
	{ 0x1969E840, UNKNOWNNID, }, // sceSysreg_driver_1969E840
	{ 0x1D382514, UNKNOWNNID, }, // sceSysreg_driver_1D382514
	{ 0x833E6FB1, UNKNOWNNID, }, // sceSysreg_driver_833E6FB1
	{ 0x03340297, UNKNOWNNID, }, // sceSysreg_driver_03340297
	{ 0x9100B4E5, UNKNOWNNID, }, // sceSysreg_driver_9100B4E5
	{ 0x0A83FC7B, UNKNOWNNID, }, // sceSysreg_driver_0A83FC7B
	{ 0xD6628A48, UNKNOWNNID, }, // sceSysreg_driver_D6628A48
	{ 0x1E18EA43, UNKNOWNNID, }, // sceSysreg_driver_1E18EA43
	{ 0x9DD1F821, UNKNOWNNID, }, // sceSysregEmcsmIoEnable
	{ 0x1C4C4C7A, UNKNOWNNID, }, // sceSysregEmcsmIoDisable
	{ 0xBC68D9B6, UNKNOWNNID, }, // sceSysregUsbIoEnable
	{ 0xA3C8E075, UNKNOWNNID, }, // sceSysregUsbIoDisable
	{ 0x79338EA3, UNKNOWNNID, }, // sceSysregAtaIoEnable
	{ 0xCADB92AA, UNKNOWNNID, }, // sceSysregAtaIoDisable
	{ 0xD74F1D48, UNKNOWNNID, }, // sceSysregMsifIoEnable
	{ 0x18172C6A, UNKNOWNNID, }, // sceSysregMsifIoDisable
	{ 0x63B1AADF, UNKNOWNNID, }, // sceSysregLcdcIoEnable
	{ 0xF74F14E9, UNKNOWNNID, }, // sceSysregLcdcIoDisable
	{ 0xBB26CF1F, UNKNOWNNID, }, // sceSysregAudioIoEnable
	{ 0x8E2FB536, UNKNOWNNID, }, // sceSysregAudioIoDisable
	{ 0x0436B60F, UNKNOWNNID, }, // sceSysregIicIoEnable
	{ 0x58F47EFD, UNKNOWNNID, }, // sceSysregIicIoDisable
	{ 0x4C49A8BC, UNKNOWNNID, }, // sceSysregSircsIoEnable
	{ 0x26FA0928, UNKNOWNNID, }, // sceSysregSircsIoDisable
	{ 0xF844DDF3, UNKNOWNNID, }, // sceSysreg_driver_F844DDF3
	{ 0x29A119A1, UNKNOWNNID, }, // sceSysreg_driver_29A119A1
	{ 0x77DED992, UNKNOWNNID, }, // sceSysregKeyIoEnable
	{ 0x6879790B, UNKNOWNNID, }, // sceSysregKeyIoDisable
	{ 0x7A5D2D15, UNKNOWNNID, }, // sceSysregPwmIoEnable
	{ 0x25B0AC52, UNKNOWNNID, }, // sceSysregPwmIoDisable
	{ 0x7FD7A631, UNKNOWNNID, }, // sceSysregUartIoEnable
	{ 0xBB823481, UNKNOWNNID, }, // sceSysregUartIoDisable
	{ 0x8C5C53DE, UNKNOWNNID, }, // sceSysregSpiIoEnable
	{ 0xA16E55F4, UNKNOWNNID, }, // sceSysregSpiIoDisable
	{ 0xB627582E, UNKNOWNNID, }, // sceSysregGpioIoEnable
	{ 0x1E9C3607, UNKNOWNNID, }, // sceSysregGpioIoDisable
	{ 0x55B18B84, UNKNOWNNID, }, // sceSysreg_driver_55B18B84
	{ 0x2112E686, UNKNOWNNID, }, // sceSysreg_driver_2112E686
	{ 0x7B9E9A53, UNKNOWNNID, }, // sceSysreg_driver_7B9E9A53
	{ 0x7BDF0556, UNKNOWNNID, }, // sceSysreg_driver_7BDF0556
	{ 0x8D0FED1E, UNKNOWNNID, }, // sceSysreg_driver_8D0FED1E
	{ 0xA46E9CA8, UNKNOWNNID, }, // sceSysreg_driver_A46E9CA8
	{ 0x633595F2, UNKNOWNNID, }, // sceSysreg_driver_633595F2
	{ 0x32471457, UNKNOWNNID, }, // sceSysregUsbQueryIntr
	{ 0x692F31FF, UNKNOWNNID, }, // sceSysregUsbAcquireIntr
	{ 0xD43E98F6, UNKNOWNNID, }, // sceSysreg_driver_D43E98F6
	{ 0xBF91FBDA, UNKNOWNNID, }, // sceSysreg_driver_BF91FBDA
	{ 0x36A75390, UNKNOWNNID, }, // sceSysreg_driver_36A75390
	{ 0x25673620, UNKNOWNNID, }, // sceSysregIntrInit
	{ 0x4EE8E2C8, UNKNOWNNID, }, // sceSysregIntrEnd
	{ 0x61BFF85F, UNKNOWNNID, }, // sceSysregInterruptToOther
	{ 0x9FC87ED4, UNKNOWNNID, }, // sceSysregSemaTryLock
	{ 0x8BE2D520, UNKNOWNNID, }, // sceSysregSemaUnlock
	{ 0x083F56E2, UNKNOWNNID, }, // sceSysregEnableIntr
	{ 0x7C5B543C, UNKNOWNNID, }, // sceSysregDisableIntr
	{ 0x3EA188AD, UNKNOWNNID, }, // sceSysregRequestIntr
	{ 0x5664F8B5, UNKNOWNNID, }, // sceSysreg_driver_5664F8B5
	{ 0x44704E1D, UNKNOWNNID, }, // sceSysreg_driver_44704E1D
	{ 0x584AD989, UNKNOWNNID, }, // sceSysreg_driver_584AD989
	{ 0x377F035F, UNKNOWNNID, }, // sceSysreg_driver_377F035F
	{ 0xAB3185FD, UNKNOWNNID, }, // sceSysreg_driver_AB3185FD
	{ 0x0EA487FA, UNKNOWNNID, }, // sceSysreg_driver_0EA487FA
	{ 0x136E8F5A, UNKNOWNNID, }, // sceSysreg_driver_136E8F5A
	{ 0xF4811E00, UNKNOWNNID, }, // sceSysreg_driver_F4811E00
};

static nid_entry sceImpose_driver_nid[] = {
	{ 0x0F341BE4, UNKNOWNNID, }, // sceImposeGetHomePopup
	{ 0x116CFF64, UNKNOWNNID, }, // sceImposeCheckVideoOut
	{ 0x116DDED6, UNKNOWNNID, }, // sceImposeSetVideoOutMode
	{ 0x1AEED8FE, UNKNOWNNID, }, // sceImposeSuspend
	{ 0x1B6E3400, UNKNOWNNID, }, // sceImposeGetStatus
	{ 0x24FD7BCF, UNKNOWNNID, }, // sceImposeGetLanguageMode
	{ 0x36AA6E91, UNKNOWNNID, }, // sceImposeSetLanguageMode
	{ 0x381BD9E7, UNKNOWNNID, }, // sceImposeHomeButton
	{ 0x531C9778, UNKNOWNNID, }, // sceImposeGetParam
	{ 0x5595A71A, UNKNOWNNID, }, // sceImposeSetHomePopup
	{ 0x7084E72C, UNKNOWNNID, }, // sceImpose_driver_7084E72C
	{ 0x72189C48, UNKNOWNNID, }, // sceImposeSetUMDPopup
	{ 0x810FB7FB, UNKNOWNNID, }, // sceImposeSetParam
	{ 0x86924032, UNKNOWNNID, }, // sceImposeResume
	{ 0x8C943191, UNKNOWNNID, }, // sceImposeGetBatteryIconStatus
	{ 0x8F6E3518, UNKNOWNNID, }, // sceImposeGetBacklightOffTime
	{ 0x967F6D4A, UNKNOWNNID, }, // sceImposeSetBacklightOffTime
	{ 0x9C8C6C81, UNKNOWNNID, }, // sceImposeSetStatus
	{ 0x9DBCE0C4, UNKNOWNNID, }, // sceImpose_driver_9DBCE0C4
	{ 0xB415FC59, UNKNOWNNID, }, // sceImposeChanges
	{ 0xBDBC42A6, UNKNOWNNID, }, // sceImposeInit
	{ 0xC7E36CC7, UNKNOWNNID, }, // sceImposeEnd
	{ 0xE0887BC8, UNKNOWNNID, }, // sceImposeGetUMDPopup
};

static nid_entry sceUsb_driver_nid[] = {
	{ 0xAE5DE6AF, UNKNOWNNID, }, // sceUsbStart
	{ 0xC2464FA0, UNKNOWNNID, }, // sceUsbStop
	{ 0xC21645A4, UNKNOWNNID, }, // sceUsbGetState
	{ 0x4E537366, UNKNOWNNID, }, // sceUsbGetDrvList
	{ 0x112CC951, UNKNOWNNID, }, // sceUsbGetDrvState
	{ 0x586DB82C, UNKNOWNNID, }, // sceUsbActivate
	{ 0xC572A9C8, UNKNOWNNID, }, // sceUsbDeactivate
	{ 0x5BE0E002, UNKNOWNNID, }, // sceUsbWaitState
	{ 0x1C360735, UNKNOWNNID, }, // sceUsbWaitCancel
	{ 0xE20B23A6, UNKNOWNNID, }, // sceUsb_E20B23A6
};

static nid_entry sceUsbBus_driver_nid[] = {
	{ 0xC21645A4, UNKNOWNNID, }, // sceUsbGetState
	{ 0xB1644BE7, UNKNOWNNID, }, // sceUsbbdRegister
	{ 0xC1E2A540, UNKNOWNNID, }, // sceUsbbdUnregister
	{ 0x951A24CC, UNKNOWNNID, }, // sceUsbbdClearFIFO
	{ 0xE65441C1, UNKNOWNNID, }, // sceUsbbdStall
	{ 0x23E51D8F, UNKNOWNNID, }, // sceUsbbdReqSend
	{ 0x913EC15D, UNKNOWNNID, }, // sceUsbbdReqRecv
	{ 0xCC57EC9D, UNKNOWNNID, }, // sceUsbbdReqCancel
	{ 0xC5E53685, UNKNOWNNID, }, // sceUsbbdReqCancelAll
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
};

static nid_entry sceRtc_driver_nid[] = {
	{ 0xC41C2853, UNKNOWNNID, }, // sceRtcGetTickResolution
	{ 0x3F7AD767, UNKNOWNNID, }, // sceRtcGetCurrentTick
	{ 0x029CA3B3, UNKNOWNNID, }, // sceRtc_029CA3B3
	{ 0x4CFA57B0, UNKNOWNNID, }, // sceRtcGetCurrentClock
	{ 0xE7C27D1B, UNKNOWNNID, }, // sceRtcGetCurrentClockLocalTime
	{ 0x34885E0D, UNKNOWNNID, }, // sceRtcConvertUtcToLocalTime
	{ 0x779242A2, UNKNOWNNID, }, // sceRtcConvertLocalTimeToUTC
	{ 0x42307A17, UNKNOWNNID, }, // sceRtcIsLeapYear
	{ 0x05EF322C, UNKNOWNNID, }, // sceRtcGetDaysInMonth
	{ 0x57726BC1, UNKNOWNNID, }, // sceRtcGetDayOfWeek
	{ 0x4B1B5E82, UNKNOWNNID, }, // sceRtcCheckValid
	{ 0x3A807CC8, UNKNOWNNID, }, // sceRtcSetTime_t
	{ 0x27C4594C, UNKNOWNNID, }, // sceRtcGetTime_t
	{ 0xF006F264, UNKNOWNNID, }, // sceRtcSetDosTime
	{ 0x36075567, UNKNOWNNID, }, // sceRtcGetDosTime
	{ 0x7ACE4C04, UNKNOWNNID, }, // sceRtcSetWin32FileTime
	{ 0xCF561893, UNKNOWNNID, }, // sceRtcGetWin32FileTime
	{ 0x7ED29E40, UNKNOWNNID, }, // sceRtcSetTick
	{ 0x6FF40ACC, UNKNOWNNID, }, // sceRtcGetTick
	{ 0x9ED0AE87, UNKNOWNNID, }, // sceRtcCompareTick
	{ 0x44F45E05, UNKNOWNNID, }, // sceRtcTickAddTicks
	{ 0x26D25A5D, UNKNOWNNID, }, // sceRtcTickAddMicroseconds
	{ 0xF2A4AFE5, UNKNOWNNID, }, // sceRtcTickAddSeconds
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
	{ 0x48D07D70, UNKNOWNNID, }, // sceRtcResume
	{ 0x62685E98, UNKNOWNNID, }, // sceRtcGetLastAdjustedTime
	{ 0x6A676D2D, UNKNOWNNID, }, // sceRtc_driver_6A676D2D
	{ 0x759937C5, UNKNOWNNID, }, // sceRtcSetConf
	{ 0x7D1FBED3, UNKNOWNNID, }, // sceRtcSetAlarmTick
	{ 0x81FCDA34, UNKNOWNNID, }, // sceRtc_driver_81FCDA34
	{ 0x912BEE56, UNKNOWNNID, }, // sceRtcInit
	{ 0x9763C138, UNKNOWNNID, }, // sceRtcSetCurrentTick
	{ 0x9CC2797E, UNKNOWNNID, }, // sceRtcSuspend
	{ 0xB44BDAED, UNKNOWNNID, }, // sceRtc_driver_B44BDAED
	{ 0xC2DDBEB5, UNKNOWNNID, }, // sceRtcGetAlarmTick
	{ 0xC499AF2F, UNKNOWNNID, }, // sceRtc_driver_C499AF2F
	{ 0xCE27DE2F, UNKNOWNNID, }, // sceRtcEnd
	{ 0xDFBC5F16, UNKNOWNNID, }, // sceRtcParseDateTime
	{ 0xE1C93E47, UNKNOWNNID, }, // sceRtcGetTime64_t
	{ 0xF0B5571C, UNKNOWNNID, }, // sceRtcSynchronize
	{ 0xFB3B18CD, UNKNOWNNID, }, // sceRtc_driver_FB3B18CD
};

static nid_entry sceMpegbase_driver_nid[] = {
	{ 0xBE45C284, UNKNOWNNID, }, // sceMpegBaseYCrCbCopyVme
	{ 0x492B5E4B, UNKNOWNNID, }, // sceMpegBaseCscInit
	{ 0xCE8EB837, UNKNOWNNID, }, // sceMpegBaseCscVme
	{ 0x0530BE4E, UNKNOWNNID, }, // sceMpegbase_driver_0530BE4E
	{ 0x304882E1, UNKNOWNNID, }, // sceMpegbase_driver_304882E1
	{ 0x7AC0321A, UNKNOWNNID, }, // sceMpegBaseYCrCbCopy
	{ 0x91929A21, UNKNOWNNID, }, // sceMpegBaseCscAvc
	{ 0xAC9E717E, UNKNOWNNID, }, // sceMpegbase_driver_AC9E717E
	{ 0xBEA18F91, UNKNOWNNID, }, // sceMpegbase_BEA18F91
	{ 0x27A2982F, UNKNOWNNID, }, // sceMpegBaseInit
};

resolver_config nid_fix[] = {
#define NID_ENTRY(libname) \
	{ #libname, NELEMS(libname##_nid), libname##_nid, }
	NID_ENTRY(sceNand_driver),
	NID_ENTRY(SysMemForKernel),
	NID_ENTRY(LoadCoreForKernel),
	NID_ENTRY(LoadExecForKernel),
	NID_ENTRY(ModuleMgrForKernel),
	NID_ENTRY(ExceptionManagerForKernel),
	NID_ENTRY(IoFileMgrForKernel),
	NID_ENTRY(KDebugForKernel),
	NID_ENTRY(scePower_driver),
	NID_ENTRY(InterruptManagerForKernel),
	NID_ENTRY(sceHprm_driver),
	NID_ENTRY(sceReg_driver),
	NID_ENTRY(sceCtrl_driver),
	NID_ENTRY(sceDisplay_driver),
	NID_ENTRY(sceAudio_driver),
	NID_ENTRY(sceSyscon_driver),
	NID_ENTRY(StdioForKernel),
	NID_ENTRY(sceSysEventForKernel),
	NID_ENTRY(UtilsForKernel),
	NID_ENTRY(sceAudioRouting_driver),
	NID_ENTRY(SysclibForKernel),
	NID_ENTRY(ThreadManForKernel),
	NID_ENTRY(InitForKernel),
	NID_ENTRY(SysTimerForKernel),
	NID_ENTRY(sceIdStorage_driver),
	NID_ENTRY(sceSysreg_driver),
	NID_ENTRY(sceImpose_driver),
	NID_ENTRY(sceUsb_driver),
	NID_ENTRY(sceUsbBus_driver),
	NID_ENTRY(sceGe_driver),
	NID_ENTRY(sceRtc_driver),
	NID_ENTRY(sceMpegbase_driver),
#undef NID_ENTRY
};

u32 nid_fix_size = NELEMS(nid_fix);
