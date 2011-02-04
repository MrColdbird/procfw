#include <pspsdk.h>
#include <pspsysmem_kernel.h>
#include <pspkernel.h>
#include <psputilsforkernel.h>
#include <psppower.h>
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "utils.h"
#include "systemctrl.h"
#include "printk.h"
#include "libs.h"

static inline void nullify_function(u32 addr)
{
	if (addr == 0) {
		printk("Warning: nullify_function got a NULL function\n");
		return;
	}

	_sw(0x03E00008, addr);
	_sw(0x00001021, addr + 4);
}

static void patch_vlf_functions(u32 nid)
{
	u32 fp;

	fp = sctrlHENFindFunction("VLF_Module", "VlfGui", nid);
	nullify_function(fp);
}

void patch_VLF_Module(void)
{
	patch_vlf_functions(0x2A245FE6);
	patch_vlf_functions(0x7B08EAAB);
	patch_vlf_functions(0x22050FC0);
	patch_vlf_functions(0x158EE61A);
	patch_vlf_functions(0xD495179F);
}
