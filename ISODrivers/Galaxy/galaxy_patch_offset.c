#include <pspsdk.h>
#include "galaxy_patch_offset.h"

#if !defined(CONFIG_635) && !defined(CONFIG_620) && !defined(CONFIG_639)
#error You have to define CONFIG_620 or CONFIG_635 or CONFIG_639
#endif

#ifdef CONFIG_639
PatchOffset g_639_offsets = {
	.fw_version = FW_639,
	.StoreFd = 0x00008A04,
	.StoreFd2 = 0x00005BA4,
	.Data1 = 0x00005BB4 - 0x00005BA4 + 0x00008A04,
	.Data2 = 0x00005BBC - 0x00005BA4 + 0x00008A04,
	.Data3 = 0x00005BD0 - 0x00005BA4 + 0x00008A04,
	.Data4 = 0x00005BD8 - 0x00005BA4 + 0x00008A04,
	.Data5 = 0x00008994,
	.InitForKernelCall = 0x00003C34,
	.Func1 = 0x00003C4C,
	.Func2 = 0x000043B4,
	.Func3 = 0x0000590C,
	.Func4 = 0x00003680,
	.Func5 = 0x00004F8C,
	.Func6 = 0x00004FFC,
	.sceIoClose = 0x00007D08,
	.sceKernelCreateThread = 0x000191B4,
	.sceKernelStartThread = 0x00019358,
};
#endif

#ifdef CONFIG_635
PatchOffset g_635_offsets = {
	.fw_version = FW_635,
	.StoreFd = 0x00008A04,
	.StoreFd2 = 0x00005BA4,
	.Data1 = 0x00005BB4 - 0x00005BA4 + 0x00008A04,
	.Data2 = 0x00005BBC - 0x00005BA4 + 0x00008A04,
	.Data3 = 0x00005BD0 - 0x00005BA4 + 0x00008A04,
	.Data4 = 0x00005BD8 - 0x00005BA4 + 0x00008A04,
	.Data5 = 0x00008994,
	.InitForKernelCall = 0x00003C34,
	.Func1 = 0x00003C4C,
	.Func2 = 0x000043B4,
	.Func3 = 0x0000590C,
	.Func4 = 0x00003680,
	.Func5 = 0x00004F8C,
	.Func6 = 0x00004FFC,
	.sceIoClose = 0x00007D08,
	.sceKernelCreateThread = 0x000191B4,
	.sceKernelStartThread = 0x00019358,
};
#endif

#ifdef CONFIG_620
PatchOffset g_620_offsets = {
	.fw_version = FW_620,
	.StoreFd = 0x00008944,
	.StoreFd2 = 0x00005BA4,
	.Data1 = 0x00005BB4 - 0x00005BA4 + 0x00008944,
	.Data2 = 0x00005BBC - 0x00005BA4 + 0x00008944,
	.Data3 = 0x00005BD0 - 0x00005BA4 + 0x00008944,
	.Data4 = 0x00005BD8 - 0x00005BA4 + 0x00008944,
	.Data5 = 0x000088D4,
	.InitForKernelCall = 0x00003BD8,
	.Func1 = 0x00003BF0,
	.Func2 = 0x00004358,
	.Func3 = 0x0000582C,
	.Func4 = 0x00003624,
	.Func5 = 0x00004EAC,
	.Func6 = 0x00004F1C,
	.sceIoClose = 0x00007C28,
	.sceKernelCreateThread = 0x000191B4,
	.sceKernelStartThread = 0x00019358,
};
#endif

PatchOffset *g_offs = NULL;

void setup_patch_offset_table(u32 fw_version)
{
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
