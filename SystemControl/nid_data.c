#include <pspsdk.h>
#include "nid_resolver.h"
#include "utils.h"

static nid_entry SysMemForKernel_nid[] = {
	{ 0xC7E57B9C, 0x9F154FA1, },
	{ 0xCF4DE78C, 0x208F4820, },
	{ 0xFC114573, 0xF0E0AB7A, },
	{ 0x3FC9AE6A, 0x5E8DCA05, },
	{ 0x536AD5E1, 0xF3C729A6, },
	{ 0x636C953B, 0x6D161EE2, }, // sceKernelAllocHeapMemory
	{ 0xC9805775, 0xF9475C1A, }, // sceKernelDeleteHeap 
	{ 0x7B749390, 0xDB836ADB, }, // sceKernelFreeHeapMemory
	{ 0x1C1FBFE7, 0xAF85EB1B, }, // sceKernelCreateHeap
	{ 0x55A40B2C, 0xF5E82409, },
	{ 0xE6581468, 0xE10F21CF, },
	{ 0x9697CD32, 0x7BE9653E, },
	{ 0x237DBD4F, 0x4621A9CC, },
	{ 0xB6D61D02, 0x8FDAFC4C, },
	{ 0x9D9A5BA1, 0xFA5025B5, },
	{ 0xB2C7AA36, 0x00E9A04A, }, // sceKernelSetDdrMemoryProtection
};

static nid_entry LoadCoreForKernel_nid[] = {
	{ 0xCF8A41B1, 0xEF8A0BEA, }, // sceKernelFindModuleByName
	{ 0xCCE4A157, 0xED53894F, }, // sceKernelFindModuleByUID
	{ 0xDD303D79, 0x312CA47E, }, // LoadCoreForKernel_DD303D79 ( TODO: actually in 5.XX)
	{ 0xFB8AE27D, 0x312CA47E, }, // sceKernelFindModuleByAddress (3.xx NID)
	{ 0xCCE4A157, 0xED53894F, },
};

static nid_entry LoadExecForKernel_nid[] = {
	{ 0x6D302D3D, 0xFCD765C9, },
	{ 0x28D0D249, 0x3D805DE6, },
	{ 0xA3D5E142, 0x5AA1A6D2, },
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
};

static nid_entry ExceptionManagerForKernel_nid[] = {
	{ 0x565C0B0E, 0x46EA27C3 },
};

static nid_entry IoFileMgrForKernel_nid[] = {
	{ 0x3C54E908, 0x421B8EB4, }, 
	{ 0x411106BA, 0x74482CE3, }, 
};

static nid_entry KDebugForKernel_nid[] = {
	{ 0xE146606D, 0xAAC71263, }, 
};

static nid_entry scePower_driver_nid[] = {
	{ 0xEFD3C963, 0x6C4F9920, }, 
	{ 0x87440F5E, 0xF8C9FAF5, }, 
	{ 0x0AFD0D8B, 0x81DCA5A5, }, 
	{ 0x1E490401, 0xAB842949, }, 
	{ 0xD3075926, 0xDE18E7C0, }, 
	{ 0x2085D15D, 0x2CBFA597, }, 
	{ 0x8EFB3FA2, 0xC8749D1A, }, 
	{ 0x28E12023, 0xC8324682, }, 
	{ 0x483CE86B, 0x4A1118E1, }, 
	{ 0x2B7C7CF4, 0x334539BA, }, 
	{ 0xFEE03A2F, 0xFDB5BFE9, }, 
	{ 0x478FE6F5, 0x9F53A71F, }, 
	{ 0x737486F2, 0xEBD177D6, }, 
	{ 0xD6E50D7B, 0x8BA12BAF, }, //scePowerRegisterCallback
	{ 0x1688935C, 0x9F53A71F, }, //scePowerGetBusClockFrequencyInt
};

static nid_entry InterruptManagerForKernel_nid[] = {
	{ 0x8A389411, 0x359F6F5C, }, 
	{ 0xD2E8363F, 0xB1F5E99B, }, 
	{ 0x8B61808B, 0x399FF74C, },  //sceKernelQuerySystemCall
};

static nid_entry sceHprm_driver_nid[] = {
	{ 0x103DF40C, 0x0682085A, }, //sceHprmIsRemoteExist
};

static nid_entry sceCtrl_driver_nid[] = {
	{ 0x454455AC, 0x9F3038AC, },  //sceCtrlReadBufferPositive
	{ 0xC4AAD55F, 0x18654FC0, },  //sceCtrlPeekBufferPositive
	{ 0x28E71A16, 0x6CB49301, },  //sceCtrlSetSamplingMode
};

static nid_entry sceDisplay_driver_nid[] = {
	{ 0x4AB7497F, 0x37533141, },  //sceDisplaySetFrameBuf
	{ 0xB685BA36, 0xC30D327D, },  //sceDisplayWaitVblankStart
	{ 0xE56B11BA, 0x08A10838, },  //sceDisplayGetFrameBuf
	{ 0x776ADFDB, 0x89FD2128, },  //sceDisplaySetBacklightInternal
	{ 0x31C4BAA8, 0x3A5621E0, },  //sceDisplayGetBrightness
	{ 0x5B5AEFAD, 0xCCEEA6BB, },  //from 1.50
};

static nid_entry sceAudio_driver_nid[] = {
	{ 0xB282F4B2, 0xA388ABDB, },  //sceAudioGetChannelRestLength
	{ 0x669D93E4, 0xE9BCD519, },  //sceAudioSRCChReserve
	{ 0x138A70F1, 0x0399579B, },  //sceAudioSRCChRelease
	{ 0x43645E69, 0x475F41FE, },  //sceAudioSRCOutputBlocking
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
	NID_ENTRY(KDebugForKernel),
	NID_ENTRY(scePower_driver),
	NID_ENTRY(InterruptManagerForKernel),
	NID_ENTRY(sceHprm_driver),
	NID_ENTRY(sceCtrl_driver),
	NID_ENTRY(sceDisplay_driver),
	NID_ENTRY(sceAudio_driver),
#undef NID_ENTRY
};

u32 nid_fix_size = NELEMS(nid_fix);
