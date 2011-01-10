#include <pspkernel.h>
#include "utils.h"
#include "libs.h"
#include "printk.h"
#include "systemctrl.h"

void validate_stub(SceModule *pMod1)
{
#ifdef DEBUG
	SceModule2 *pMod = (SceModule2*)pMod1;
	u32 k1 = pspSdkGetK1();
	u32 i, j;
	u32 *cur_nid, *cur_call;
	u32 library_flag;
	u32 is_weak;
	PspModuleImport *pImp;

	pspSdkSetK1(0);
	
	if (pMod == NULL) {
		pspSdkSetK1(k1);
		return;
	}

	for(i=0; i<pMod->stub_size; i+=(pImp->entLen * 4)) {
		pImp = (PspModuleImport*)(pMod->stub_top+i);
		
		cur_nid = (u32*)pImp->fnids;
		cur_call = (u32*)pImp->funcs;
		library_flag = pImp->attribute << 16 | pImp->version;

		for(j=0; j<pImp->funcCount; j++) {
			is_weak = ((pImp->attribute & 0x0009) == 0x0009) ? 1 : 0;
			
			if (!is_weak && *cur_call == 0x0000054C) {
				// syscall 0x15
				printk("WARNING: %s[0x%08X] %s_%08X at 0x%08X unresolved\r\n",
						pMod->modname, library_flag, pImp->name, *cur_nid, (u32)cur_call);
			}
			
			cur_nid ++;
			cur_call += 2;
		}
	}

	pspSdkSetK1(k1);
#endif
}

void validate_stub_by_uid(int modid)
{
#ifdef DEBUG
	u32 k1 = pspSdkGetK1();
	SceModule2 *pMod;

	pspSdkSetK1(0);
	pMod = (SceModule2*) sceKernelFindModuleByUID(modid);
	
	if (pMod == NULL) {
		pspSdkSetK1(k1);
		return;
	}

	validate_stub((SceModule*)pMod);
	pspSdkSetK1(k1);
#endif
}

