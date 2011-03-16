#include <pspsdk.h>
#include <pspsysmem_kernel.h>
#include <pspkernel.h>
#include <psputilsforkernel.h>
#include <pspsysevent.h>
#include <pspiofilemgr.h>
#include <stdio.h>
#include <string.h>
#include "printk.h"
#include "utils.h"
#include "systemctrl_patch_offset.h"

void patch_pspMarch33_Driver(u32 march33_text_addr)
{
	_sw(NOP, march33_text_addr+g_offs->march33_patch.MsMediaInsertedCheck);
}
