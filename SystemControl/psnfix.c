#include <pspsdk.h>

//fix playstation network account registration
void patch_npsignup(u32 text_addr)
{
	//ImageVersion = 0x10000000
	_sw(0x3C041000, text_addr + 0x370D0);
}

//fix playstation network login
void patch_npsignin(u32 text_addr)
{
	//kill connection error
	_sw(0x10000008, text_addr + 0x00006C94);

	//ImageVersion = 0x10000000
	_sw(0x3C041000, text_addr + 0x00009664);
}

//fake hardcoded np version for npmatching library (psp2, fat princess, etc.)
void patch_np(u32 text_addr, u8 mayor, u8 minor)
{
	//np firmware version spoof
	_sb(mayor, text_addr + 0x00004604);
	_sb(minor, text_addr + 0x0000460C);
}
