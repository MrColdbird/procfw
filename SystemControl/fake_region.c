#include <pspsdk.h>
#include <pspsysmem_kernel.h>
#include <pspkernel.h>
#include <psputilsforkernel.h>
#include <pspsysevent.h>
#include <pspiofilemgr.h>
#include <stdio.h>
#include <string.h>
#include "printk.h"
#include "main.h"
#include "utils.h"

static u8 get_pscode_from_region(int region)
{
	u8 code;

	code = region;
	
	if(code < 12) {
		code += 2;
	} else {
		code -= 11;
	}

	if(code == 2) {
		code = 3;
	}

	printk("%s: region %d code %d\n", __func__, region, code);

	return code;
}

static int _sceChkregGetPsCode(u8 *pscode)
{
	pscode[0] = 1;
	pscode[1] = 0;
	pscode[2] = get_pscode_from_region(conf.fakeregion);
	pscode[3] = 0;
	pscode[4] = 1;
	pscode[5] = 0;
	pscode[6] = 1;
	pscode[7] = 0;

	return 0;
}

void patch_sceChkreg(void)
{
	u32 fp;
   
	// sceChkregGetPsCode
	fp = sctrlHENFindFunction("sceChkreg", "sceChkreg_driver", 0x59F8491D); 

	if (fp && conf.fakeregion) {
		REDIRECT_FUNCTION(_sceChkregGetPsCode, fp);
	}
}
