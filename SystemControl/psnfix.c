#include <pspsdk.h>
#include "systemctrl_patch_offset.h"

//fix playstation network account registration
void patch_npsignup(u32 text_addr)
{
	//ImageVersion = 0x10000000
	_sw(0x3C041000, text_addr + g_offs->psnfix_patch.NPSignupImageVersion);
}

//fix playstation network login
void patch_npsignin(u32 text_addr)
{
	//kill connection error
	_sw(0x10000008, text_addr + g_offs->psnfix_patch.NPSigninCheck);

	//ImageVersion = 0x10000000
	_sw(0x3C041000, text_addr + g_offs->psnfix_patch.NPSigninImageVersion);
}

//fake hardcoded np version for npmatching library (psp2, fat princess, etc.)
void patch_np(u32 text_addr, u8 mayor, u8 minor)
{
	//np firmware version spoof
	_sb(mayor, text_addr + g_offs->psnfix_patch.NPMatchVersionMajor);
	_sb(minor, text_addr + g_offs->psnfix_patch.NPMatchVersionMinor);
}
