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

static int get_pscode_from_region(int region)
{
	switch(region) {
		case FAKE_REGION_JAPAN:
			return 3;
		case FAKE_REGION_AMERICA:
			return 4;
		case FAKE_REGION_EUROPE:
			return 5;
		case FAKE_REGION_KOREA:
			return 6;
		case FAKE_REGION_AUSTRALIA:
			return 9;
		case FAKE_REGION_HONGKONG:
			return 0xA;
		case FAKE_REGION_TAIWAN:
			return 0xB;
		case FAKE_REGION_RUSSIA:
			return 0xC;
		case FAKE_REGION_CHINA:
			return 0xD;
	}

	return 0xB;
}

static int _sceChkregGetPsCode(char *pscode)
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
