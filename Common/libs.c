/*
 * PSPLINK
 * -----------------------------------------------------------------------
 * Licensed under the BSD license, see LICENSE in PSPLINK root for details.
 *
 * libs.c - Module library code for psplink.
 *
 * Copyright (c) 2005 James F <tyranid@gmail.com>
 *
 * $HeadURL: svn://svn.pspdev.org/psp/trunk/psplinkusb/psplink/libs.c $
 * $Id: libs.c 2301 2007-08-26 13:48:05Z tyranid $
 */

#include <pspkernel.h>
#include <pspdebug.h>
#include <pspsysmem_kernel.h>
#include <psputilsforkernel.h>
#include <pspmoduleexport.h>
#include <psploadcore.h>
#include <pspsdk.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "libs.h"
#include "printk.h"
#include "main.h"
#include "systemctrl.h"

PspModuleImport *find_import_lib(SceModule *pMod, char *library)
{
	PspModuleImport *pImp;
	void *stubTab;
	int stubLen;
	int i = 0;

	if(pMod == NULL)
		return NULL;

	stubTab = pMod->stub_top;
	stubLen = pMod->stub_size;
	while(i<stubLen) {
		pImp = (PspModuleImport*)(stubTab+i);
		if((pImp->name) && (strcmp(pImp->name, library) == 0))
			return pImp;
		i += (pImp->entLen * 4);
	}

	return NULL;
}


unsigned int find_import_bynid(SceModule *pMod, char *library, unsigned int nid)
{
	PspModuleImport *pImp;
	int i;

	pImp = find_import_lib(pMod, library);
	if(pImp) {
		for(i=0; i<pImp->funcCount; i++) {
			if(pImp->fnids[i] == nid)
				return (unsigned int) &pImp->funcs[i*2];
		}
	}

	return 0;
}

/**
 * Remember you have to export the hooker function if using syscall hook
 */
int hook_import_bynid(SceModule *pMod, char *library, unsigned int nid, void *func, int syscall)
{
	PspModuleImport *pImp;
	void *stubTab;
	int stubLen;
	int i = 0;

	if(pMod == NULL)
		return -1;

	stubTab = pMod->stub_top;
	stubLen = pMod->stub_size;

	while(i<stubLen) {
		pImp = (PspModuleImport*)(stubTab+i);

		if((pImp->name) && (strcmp(pImp->name, library) == 0)) {
			int j;

			for(j=0; j<pImp->funcCount; j++) {
				if(pImp->fnids[j] == nid) {
					void *addr = (void*)(&pImp->funcs[j*2]);

					if(syscall) {
						u32 syscall_num;

						syscall_num = sctrlKernelQuerySystemCall(func);

						if(syscall_num == (u32)-1) {
							printk("%s: cannot find syscall in %s_%08X\n", __func__, library, nid);

							return -1;
						}

						_sw(0x03E00008, (u32)addr);
						_sw(((syscall_num<<6)|12), (u32)addr + 4);
					} else {
						_sw(MAKE_JUMP(func), (u32)addr);
						_sw(0, (u32)(addr + 4));
					}

					sceKernelDcacheWritebackInvalidateRange(addr, 8);
					sceKernelIcacheInvalidateRange(addr, 8);
				}
			}
		}

		i += (pImp->entLen * 4);
	}

	return 0;
}
