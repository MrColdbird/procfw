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

/*
 * This file is part of stargate.
 *
 * Copyright (C) 2008 hrimfaxi (outmatch@gmail.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <pspsdk.h>
#include <pspkernel.h>
#include <pspsysmem_kernel.h>
#include <pspthreadman_kernel.h>
#include <pspcrypt.h>
#include <pspdebug.h>
#include <pspinit.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "libs.h"
#include "psputilsforkernel.h"
#include "systemctrl.h"
#include "kubridge.h"
#include "pspcipher.h"
#include "printk.h"

static int check_blacklist(u8 *prx, u8 *blacklist, u32 blacklistsize)
{
	u32 i;

	if (blacklistsize / 16 == 0) {
		return 0;
	}

	i = 0;

	while (i < blacklistsize / 16) {
		if (!memcmp(blacklist + i * 16, (prx + 0x140), 0x10)) {
			return 1;
		}

		i++;
	}

	return 0;
}

static int kirk7(u8* prx, u32 size, u32 scramble_code, u32 use_polling)
{
	int ret;

	((u32 *) prx)[0] = 5;
	((u32 *) prx)[1] = 0;
	((u32 *) prx)[2] = 0;
	((u32 *) prx)[3] = scramble_code;
	((u32 *) prx)[4] = size;

	if (!use_polling) {
		ret = sceUtilsBufferCopyWithRange (prx, size + 20, prx, size + 20, 7);
	} else {
		ret = sceUtilsBufferCopyByPollingWithRange (prx, size + 20, prx, size + 20, 7);
	}

	return ret;
}

static void prx_xor_key_into(u8 *dstbuf, u32 size, u8 *srcbuf, u8 *xor_key)
{
	u32 i;

	i = 0;

	while (i < size) {
		dstbuf[i] = srcbuf[i] ^ xor_key[i];
		++i;
	}
}

static void prx_xor_key_large(u8 *buf, u32 size, u8 *xor_key)
{
	u32 i;

	i = 0;

	while (i < size) {
		buf[i] = buf[i] ^ xor_key[i];
		++i;
	}
}

static void prx_xor_key(u8 *buf, u32 size, u8 *xor_key1, u8 *xor_key2)
{
	u32 i;

	i =0;
	while (i < size) {
		if (xor_key2 != NULL) {
			buf[i] = buf[i] ^ xor_key2[i&0xf];
		}

		buf[i] = buf[i] ^ xor_key1[i&0xf];
		++i;
	}
}

static void prx_xor_key_single(u8 *buf, u32 size, u8 *xor_key)
{
	return prx_xor_key(buf, size, xor_key, NULL);
}

static u8 buf1[0x150];
static u8 buf2[0x150];
static u8 buf3[0x90];
static u8 buf4[0xb4];
static u8 buf5[0x20];

int _uprx_decrypt(user_decryptor *pBlock)
{
	if (pBlock == NULL)
		return -1;

	if (pBlock->prx == NULL || pBlock->newsize == NULL)
		return -2;

	if (pBlock->size < 0x160)
		return -202;

	if ((u32)pBlock->prx & 0x3f)
		return -203;

	if (((0x00220202 >> (((u32)pBlock->prx >> 27) & 0x001F)) & 0x0001) == 0x0000)
		return -204;

	u32 b_0xd4 = 0;

	memset(buf1, 0, sizeof(buf1));
	memset(buf2, 0, sizeof(buf2));
	memset(buf3, 0, sizeof(buf3));
	memset(buf4, 0, sizeof(buf4));
	memset(buf5, 0, sizeof(buf5));

	memcpy(buf1, pBlock->prx, 0x150);

	/** tag mismatched */
	if (memcmp(buf1 + 0xd0, pBlock->tag, 4))
		return -45;

	int ret = -1;

	if (pBlock->type == 3) {
		u8 *p = buf1;
		u32 cnt = 0;

		while (p[0xd4] && cnt < 0x18 ) {
			cnt++;
			p = buf1 + cnt;
		}

		if (p[0xd4] != 0)
			return -17;
	} else if (pBlock->type == 2) {
		u8 *p = buf1;
		u32 cnt = 0;

		while (p[0xd4] && cnt < 0x58 ) {
			cnt++;
			p = buf1 + cnt;
		}

		if (p[0xd4] != 0)
			return -12;
	} else if (pBlock->type == 5) {
		u8 *p = buf1 + 1;
		u32 cnt = 1;

		while (p[0xd4] && cnt < 0x58 ) {
			cnt++;
			p = buf1 + cnt;
		}

		if (p[0xd4] != 0)
			return -13;

		b_0xd4 = buf1[0xd4];
	} else if (pBlock->type == 6) {
		u8 *p = buf1;
		u32 cnt = 0;

		while (p[0xd4] && cnt < 0x38 ) {
			cnt++;
			p = buf1 + cnt;
		}

		if (p[0xd4] != 0)
			return -302;
	} else if (pBlock->type == 7) {
		u8 *p = buf1 + 1;
		u32 cnt = 1;

		while (p[0xd4] && cnt < 0x38 ) {
			cnt++;
			p = buf1 + cnt;
		}

		if (p[0xd4] != 0)
			return -302;
	}

//label38:
	if (pBlock->blacklist != NULL && pBlock->blacklistsize != 0) {
		ret = check_blacklist(buf1, pBlock->blacklist, pBlock->blacklistsize);

		if (ret == 1)
			return -305;
	}

	u32 elf_size_comp =  *(u32*)(buf1+0xb0);
//	printf("elf_size_comp: %d\n", elf_size_comp);
	*pBlock->newsize = elf_size_comp;

	if (pBlock->size - 50 < elf_size_comp)
		return -206;

	if (pBlock->type >= 2 && pBlock->type <= 7) {
		int i;

		for (i=0; i<9; i++)
		{
			memcpy(buf2 + 0x14 + (i << 4), pBlock->key, 0x10);
			buf2[0x14+ (i<<4)] = i;
		}
	} else {
		memcpy(buf2 + 14, pBlock->key, 0x90);
	}

	if ((ret = kirk7(buf2, 0x90, pBlock->code, pBlock->use_polling)) < 0) {
		return ret;
	}

	if (pBlock->type == 3 || pBlock->type == 5 || pBlock->type == 7) {
		if (pBlock->xor_key2 != NULL) {
			prx_xor_key_single(buf2, 0x90, pBlock->xor_key2);
		}
	}

	memcpy(buf3, buf2, 0x90);

	if (pBlock->type == 3) {
		memcpy(buf2, buf1 + 0xec, 0x40);
		memset(buf2 + 0x40, 0, 0x50);
		buf2[0x60] = 0x03;
		buf2[0x70] = 0x50;

		memcpy(buf2 + 0x90, buf1 + 0x80, 0x30);
		memcpy(buf2 + 0xc0, buf1 + 0xc0, 0x10);
		memcpy(buf2 + 0xd0, buf1 + 0x12c, 0x10);

		prx_xor_key(buf2+144, 0x50, pBlock->xor_key1, pBlock->xor_key2);
		ret = sceUtilsBufferCopyWithRange(buf4, 0xb4, buf2, 0X150, 3);

		if (ret != 0) {
			return -14;
		}
		
		memcpy(buf2, buf1 + 0xd0, 4);
		memset(buf2 + 4, 0, 0x58);
		memcpy(buf2 + 0x5c, buf1 + 0x140, 0x10);
		memcpy(buf2 + 0x6c, buf1 + 0x12c, 0x14);
		memcpy(buf2 + 0x6c, buf4, 0x10);
		memcpy(buf2 + 0x80, buf4, 0x30);
		memcpy(buf2 + 0xb0, buf4 + 0x30, 0x10);
		memcpy(buf2 + 0xc0, buf1 + 0xb0, 0x10);
		memcpy(buf2 + 0xd0, buf1, 0x80);
	} else if (pBlock->type == 5 || pBlock->type == 7) {
		memcpy(buf2 + 0x14, buf1 + 0x80, 0x30);
		memcpy(buf2 + 0x44, buf1 + 0xc0, 0x10);
		memcpy(buf2 + 0x54, buf1 + 0x12c, 0x10);
		prx_xor_key(buf2+20, 0x50, pBlock->xor_key1, pBlock->xor_key2);
		ret = kirk7 (buf2, 0x50, pBlock->code, pBlock->use_polling);

		if (ret != 0) {
			return -11;
		}

		memcpy(buf4, buf2, 0x50);
		memcpy(buf2, buf1 + 0xd0, 0x4);
		memset(buf2 + 4, 0, 0x58);
		memcpy(buf2 + 0x5c, buf1 + 0x140, 0x10);
		memcpy(buf2 + 0x6c, buf1 + 0x12c, 0x14);
		memcpy(buf2 + 0x6c, buf4 + 0x40, 0x10);
		memcpy(buf2 + 0x80, buf4, 0x30);
		memcpy(buf2 + 0xb0, buf4 + 0x30, 0x10);
		memcpy(buf2 + 0xc0, buf1 + 0xb0, 0x10);
		memcpy(buf2 + 0xd0, buf1, 0x80);
	} else if (pBlock->type != 2 && pBlock->type != 4 && pBlock->type != 6) {
		memcpy(buf2, buf1 + 0xd0, 0x80);
		memcpy(buf2 + 0x80, buf1 + 0x80, 0x50);
		memcpy(buf2 + 0xd0, buf1, 0x80);
	} else {
		memcpy(buf2       , buf1 +  0xd0, 0x5C);
		memcpy(buf2 + 0x5c, buf1 + 0x140, 0x10);
		memcpy(buf2 + 0x6c, buf1 + 0x12c, 0x14);
		memcpy(buf2 + 0x80, buf1 +  0x80, 0x30);
		memcpy(buf2 + 0xb0, buf1 +  0xc0, 0x10);
		memcpy(buf2 + 0xc0, buf1 +  0xb0, 0x10);
		memcpy(buf2 + 0xd0, buf1        , 0x80);
	}

//label159:
	if (pBlock->type == 1)
	{
		memcpy(buf4 + 0x14, buf2 + 0x10, 0xa0);
		ret = kirk7(buf4, 0xa0, pBlock->code, pBlock->use_polling);

		if (ret < 0) {
			return -15;
		}

		memcpy(buf2 + 0x10, buf4, 0xa0);
	} else {
		if (pBlock->type >= 2 && pBlock->type <= 7) {
			memcpy(buf4 + 0x14, buf2 + 0x5c, 0x60);
		}

		if (pBlock->type == 3 || pBlock->type == 5 || pBlock->type == 7) {
			prx_xor_key_single(buf4 + 20, 0x60, pBlock->xor_key1);
		}

		if (kirk7(buf4, 0x60, pBlock->code, pBlock->use_polling) < 0) {
			return -5;
		}

		memcpy(buf2 + 0x5c, buf4, 0x60);
	}

	if (pBlock->type < 2 || pBlock->type > 7) {
		memcpy(buf4, buf2 + 0x4, 0x14);
		*((u32*)buf2) = 0x14c;
		memcpy(buf2 + 4, buf3, 0x14);
	} else {
		memcpy(buf4, buf2 + 0x6c, 0x14);

		if (pBlock->type == 4) {
			memmove(buf2 + 0x18, buf2, 0x67);
		} else {
			memcpy(buf2+0x70, buf2+0x5C, 0x10);

			if (pBlock->type == 6 || pBlock->type == 7) {
				memcpy(buf5, buf2+60, 0x20);
				memcpy(buf2+80, buf5, 0x20);
				memset(buf2+24, 0, 0x38);
			} else {
				memset(buf2+0x18, 0, 0x58);
			}

			if ( b_0xd4 == 0x80 ) {
				buf2[0x18] = 0x80;
			}
		}

		memcpy(buf2+0x04, buf2, 0x04);
		*((u32*)buf2) = 0x014C;
		memcpy(buf2+0x08, buf3, 0x10);	
	}

	if (pBlock->use_polling == 0) {
		ret = sceUtilsBufferCopyWithRange (buf2, 0x150, buf2, 0x150, 0xB);
	} else {
		ret = sceUtilsBufferCopyByPollingWithRange (buf2, 0x150, buf2, 0x150, 0xB);
	}

	if (ret != 0) {
		return -6;
	}

	if (memcmp(buf2, buf4, 0x14))
	{
		return -8;
	}

	if (pBlock->type < 2 || pBlock->type > 7) {
		prx_xor_key_large(buf2+0x40, 0x70, buf3+0x14);

		if (kirk7(buf2 + 0x2c, 0x70, pBlock->code, pBlock->use_polling) < 0) {
			return -16;
		}

		prx_xor_key_into(pBlock->prx+64, 0x70, buf2+44, buf3+32);
		memcpy(pBlock->prx+176, buf2 + 0xb0, 0xa0);
	} else {
		prx_xor_key_large(buf2 + 128, 0x40, buf3 + 16);

		if (kirk7(buf2 + 108, 0x40, pBlock->code, pBlock->use_polling) < 0) {
			return -7;
		}

		prx_xor_key_into(pBlock->prx + 64, 0x40, buf2 + 108, buf3 + 80);

		if (pBlock->type == 6 || pBlock->type == 7) {
			memcpy(pBlock->prx+128, buf5, 0x20);
			memset(pBlock->prx+160, 0, 0x10);
			((u8*)pBlock->prx)[164] = 1;
			((u8*)pBlock->prx)[160] = 1;
		} else {
			memset(pBlock->prx+128, 0, 0x30);
			((u8*)pBlock->prx)[160] = 1;
		}

		memcpy(pBlock->prx+176, buf2+0xc0, 0x10);
		memset(pBlock->prx+192, 0, 0x10);
		memcpy(pBlock->prx+208, buf2+0xd0, 0x80);
	}

	if (b_0xd4 == 0x80) {
		if (((u8*)pBlock->prx)[1424])
			return -302;

		((u8*)pBlock->prx)[1424] |= 0x80;
	}

	// The real decryption
	if (sceUtilsBufferCopyWithRange(pBlock->prx, pBlock->size, pBlock->prx+0x40, pBlock->size-0x40, 0x1) != 0)
	{
		return -9;
	}

	if (elf_size_comp < 0x150)
	{
		// Fill with 0
		memset(pBlock->prx+elf_size_comp, 0, 0x150-elf_size_comp);		
	}

	return 0;
}

int uprx_decrypt(u32 *tag, u8 *key, u32 code, u8 *prx, u32 size, u32 *newsize, u32 use_polling, u8 *blacklist, u32 blacklistsize, u32 type, u8 *xor_key1, u8 *xor_key2)
{
	user_decryptor block;
	int ret;

	block.tag = tag;
	block.key = key;
	block.code = code;
	block.prx = prx;
	block.size = size;
	block.newsize = newsize;
	block.use_polling = use_polling;
	block.blacklist = blacklist;
	block.blacklistsize = blacklistsize;
	block.type = type;
	block.xor_key1 = xor_key1;
	block.xor_key2 = xor_key2;

	ret = _uprx_decrypt(&block);

	if (ret < 0)
		ret = -301;

	return ret;
}
