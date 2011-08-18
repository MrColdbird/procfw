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

#include <pspkernel.h>
#include "main.h"
#include "utils.h"
#include "libs.h"
#include "printk.h"
#include "systemctrl.h"
#include "systemctrl_patch_offset.h"

extern int _sceKernelStartModule(SceUID modid, SceSize argsize, void *argp, int *status, SceKernelSMOption *option);

static int (*start_module)(u32 unk0, SceModule2 *mod, u32 unk2, u32 unk3, u32 unk4) = NULL;

void validate_stub(SceModule *pMod1)
{
	SceModule2 *pMod = (SceModule2*)pMod1;
	u32 k1;
	u32 i, j;
	u32 *cur_nid, *cur_call;
	u32 library_flag;
	u32 is_weak;
	PspModuleImport *pImp;

	k1 = pspSdkSetK1(0);
	
	if (pMod != NULL) {
		for(i=0; i<pMod->stub_size; i+=(pImp->entLen * 4)) {
			pImp = (PspModuleImport*)(pMod->stub_top+i);

			cur_nid = (u32*)pImp->fnids;
			cur_call = (u32*)pImp->funcs;
			library_flag = pImp->attribute << 16 | pImp->version;

			for(j=0; j<pImp->funcCount; j++) {
				is_weak = ((pImp->attribute & 0x0009) == 0x0009) ? 1 : 0;

				if (!is_weak && *cur_call == 0x0000054C) {
					// syscall 0x15
					printk("WARNING: %s[0x%08X] %s_%08X at 0x%08X unresolved\n",
							pMod->modname, (uint)library_flag, pImp->name, (uint)*cur_nid, (uint)cur_call);
				}

				cur_nid ++;
				cur_call += 2;
			}
		}
	}

	pspSdkSetK1(k1);
}

void validate_stub_by_uid(int modid)
{
	u32 k1;
	SceModule *pMod;

	k1 = pspSdkSetK1(0);
	pMod = sctrlKernelFindModuleByUID(modid);
	
	if (pMod != NULL) {
		validate_stub(pMod);
	}

	pspSdkSetK1(k1);
}

static int _start_module(u32 unk0, SceModule2 *mod, u32 unk2, u32 unk3, u32 unk4)
{
	int ret;

	ret = (*start_module)(unk0, mod, unk2, unk3, unk4);
	
	if(mod != NULL) {
		validate_stub_by_uid(mod->modid);
	}

	return ret;
}

void setup_validate_stub(SceModule *mod)
{
	SceModule2 *modulemgr = (SceModule2*)mod;

	start_module = (void*)(modulemgr->text_addr + g_offs->modulemgr_patch.StartModule);
	_sw(MAKE_CALL(_start_module), modulemgr->text_addr+g_offs->modulemgr_patch.StartModuleCall);
}
