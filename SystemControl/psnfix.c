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
