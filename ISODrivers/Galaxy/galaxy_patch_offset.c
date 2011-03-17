#include <pspsdk.h>
#include "galaxy_patch_offset.h"

PatchOffset g_635_offsets = {
	.fw_version = 0x06030510,
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

PatchOffset *g_offs = NULL;

void setup_patch_offset_table(u32 fw_version)
{
	if(fw_version == g_635_offsets.fw_version) {
		g_offs = &g_635_offsets;
	}
}
