#ifndef __payloadex_patch_list__
#define __payloadex_patch_list__


struct FunctionList{
	u32 BootLfatOpen;
	u32 BootLfatRead;
	u32 BootLfatClose;
	u32 CheckPspConfig;
	u32 DcacheClearAddr;
	u32 IcacheClearAddr;
};

struct PatchAddrList{
	u32 BootLfatOpenPatch;
	u32 BootLfatReadPatch;
	u32 BootLfatClosePatch;
	u32 CheckPspConfigPatch;
	u32 KdebugPatchAddr;
	u32 BtHeaderPatchAddr;
	u32 LfatMountPatchAddr;
	u32 LfatSeekPatchAddr1;
	u32 LfatSeekPatchAddr2;
	u32 LoadCorePatchAddr;
	u32 HashCheckPatchAddr;
};

struct MmlmdPatchList {
	u32 ModuleOffsetAddr;
	u32 SigcheckPatchAddr;
	u32 SigcheckFuncAddr;
	u32 DecryptPatchAddr;
	u32 DecryptFuncAddr;
};

struct AddrList{
	struct FunctionList function_list;
	struct PatchAddrList patch_list;
	struct MmlmdPatchList memlmd_list;
};

#if _PSP_FW_VERSION == 639

static const struct AddrList payloadex_patch_list = {
#if PSP_MODEL == 0
	.function_list = {
		.BootLfatOpen	= 0x88604A20,
		.BootLfatRead	= 0x88604B94,
		.BootLfatClose	= 0x88604B38,
		.CheckPspConfig	= 0x8860A890,
		.DcacheClearAddr= 0x88601510,
		.IcacheClearAddr= 0x88600DBC,
	},
	.patch_list = {
		.BootLfatOpenPatch	= 0x88603EB8,
		.BootLfatReadPatch	= 0x88603F20,
		.BootLfatClosePatch	= 0x88603F40,
		.CheckPspConfigPatch= 0x88603594,
		.KdebugPatchAddr	= 0x8860C80C,
		.BtHeaderPatchAddr	= 0x8860A8C4,
		.LfatMountPatchAddr	= 0x88603EB0,
		.LfatSeekPatchAddr1	= 0x88603F00,
		.LfatSeekPatchAddr2	= 0x88603F10,
		.LoadCorePatchAddr	= 0x8860339C,
		.HashCheckPatchAddr	= 0x88603954,
	},
	
#elif (PSP_MODEL == 1)
	.function_list = {
		.BootLfatOpen	= 0x88604AF0,
		.BootLfatRead	= 0x88604C64,
		.BootLfatClose	= 0x88604C08,
		.CheckPspConfig	= 0x8860A960,
		.DcacheClearAddr= 0x886015E0,
		.IcacheClearAddr= 0x88600E8C,
	},
	.patch_list = {
		.BootLfatOpenPatch	= 0x88603F88,
		.BootLfatReadPatch	= 0x88603FF0,
		.BootLfatClosePatch	= 0x88604010,
		.CheckPspConfigPatch= 0x88603680,
		.KdebugPatchAddr	= 0x8860C8DC,
		.BtHeaderPatchAddr	= 0x8860A994,
		.LfatMountPatchAddr	= 0x88603F80,
		.LfatSeekPatchAddr1	= 0x88603FD0,
		.LfatSeekPatchAddr2	= 0x88603FE0,
		.LoadCorePatchAddr	= 0x8860346C,
		.HashCheckPatchAddr	= 0x88603A24,
	},
#endif
	
	.memlmd_list = {
		.ModuleOffsetAddr	= 0x00000BBC,
		.SigcheckPatchAddr	= 0x00005CC8,
		.SigcheckFuncAddr	= 0x00007AE8,//memlmd_3F2AC9C6
		.DecryptPatchAddr	= 0x00005CA4,
		.DecryptFuncAddr	= 0x00007B08,//memlmd_E42AFE2E
	},
};

#elif _PSP_FW_VERSION == 660
static const struct AddrList payloadex_patch_list = {
#if PSP_MODEL == 0
	.function_list = {
		.BootLfatOpen	= 0x88604B38,
		.BootLfatRead	= 0x88604CAC,
		.BootLfatClose	= 0x88604C50,
		.CheckPspConfig	= 0x8860A9C0,
		.DcacheClearAddr= 0x88601614,
		.IcacheClearAddr= 0x88600EC0,
	},
	.patch_list = {
		.BootLfatOpenPatch	= 0x88603FD0,
		.BootLfatReadPatch	= 0x88604038,
		.BootLfatClosePatch	= 0x88604058,
		.CheckPspConfigPatch= 0x886036AC,
		.KdebugPatchAddr	= 0x8860C8EC,
		.BtHeaderPatchAddr	= 0x8860A9F4,
		.LfatMountPatchAddr	= 0x88603FC8,
		.LfatSeekPatchAddr1	= 0x88604018,
		.LfatSeekPatchAddr2	= 0x88604028,
		.LoadCorePatchAddr	= 0x886034B4,
		.HashCheckPatchAddr	= 0x88603A6C,
	},

#elif (PSP_MODEL == 1)
	.function_list = {
		.BootLfatOpen	= 0x88604C00,
		.BootLfatRead	= 0x88604D74,
		.BootLfatClose	= 0x88604D18,
		.CheckPspConfig	= 0x8860AA80,
		.DcacheClearAddr= 0x886016DC,
		.IcacheClearAddr= 0x88600F88,
	},
	.patch_list = {
		.BootLfatOpenPatch	= 0x88604098,
		.BootLfatReadPatch	= 0x88604100,
		.BootLfatClosePatch	= 0x88604120,
		.CheckPspConfigPatch= 0x88603790,
		.KdebugPatchAddr	= 0x8860C9AC,
		.BtHeaderPatchAddr	= 0x8860AAB4,
		.LfatMountPatchAddr	= 0x88604090,
		.LfatSeekPatchAddr1	= 0x886040E0,
		.LfatSeekPatchAddr2	= 0x886040F0,
		.LoadCorePatchAddr	= 0x8860357C,
		.HashCheckPatchAddr	= 0x88603B34,
	},
#endif
	.memlmd_list = {
		.ModuleOffsetAddr	= 0x00000AF8,
		.SigcheckPatchAddr	= 0x00005994,
		.SigcheckFuncAddr	= 0x00007824,//memlmd_6192F715
		.DecryptPatchAddr	= 0x00005970,
		.DecryptFuncAddr	= 0x0000783C,//memlmd_EF73E85B
	},
	
};


#else
#error Target PatchList is not found !
#endif

#endif