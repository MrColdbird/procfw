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
#include <pspsysmem_kernel.h>
#include <pspkernel.h>
#include <psputilsforkernel.h>
#include <pspsysevent.h>
#include <pspiofilemgr.h>
#include <stdio.h>
#include <string.h>
#include "printk.h"
#include "utils.h"
#include "strsafe.h"
#include "rebootex_conf.h"
#include "main.h"
#include "systemctrl_patch_offset.h"

typedef struct __attribute__((packed))
{
	u32 signature;
	u32 version;
	u32 fields_table_offs;
	u32 values_table_offs;
	int nitems;
} SFOHeader;

typedef struct __attribute__((packed))
{
	u16 field_offs;
	u8  unk;
	u8  type; // 0x2 -> string, 0x4 -> number
	u32 unk2;
	u32 unk3;
	u16 val_offs;
	u16 unk4;
} SFODir;

static int check_large_mem(const char *sfo)
{
	SFOHeader *header = (SFOHeader *)sfo;
	SFODir *entries = (SFODir *)(sfo+0x14);
	int i;

	if(header->signature != 0x46535000) {
		return 0;
	}

	for (i=0; i<header->nitems; i++) {
		if (0 == strcmp(sfo+header->fields_table_offs+entries[i].field_offs, "MEMSIZE")) {
			u32 value;

			if(entries[i].type != 4) {
				return 0;
			}

			value = *(u32*)(sfo+header->values_table_offs+entries[i].val_offs);

			if(value == 1) {
				return 1;
			}

			return 0;
		}
	}

	return  0;
}

int (*sceSystemFileGetIndex)(u8 *sfo, u32 unk1, u32 unk2) = NULL;

int _sceSystemFileGetIndex(u8 *sfo, u32 unk1, u32 unk2)
{
	int ret;

	ret = check_large_mem((const char*)sfo);

	if(ret == 1) {
		printk("%s: found MEMSIZE=1 in this homebrew\n", __func__);

		if(psp_model != PSP_1000) {
			patch_partitions();
			sync_cache();
		}
	}

	ret = (*sceSystemFileGetIndex)(sfo, unk1, unk2);

	return ret;
}

static inline int is_pbp(const char *init_file)
{
	const char *p;

	if(init_file == NULL)
		return 0;

	p = strrchr(init_file, '.');

	if(p == NULL)
		return 0;

	if(0 != stricmp(p, ".PBP"))
		return 0;

	return 1;
}

void patch_sceMediaSync(u32 scemediasync_text_addr)
{
	const char* init_file;

	sceSystemFileGetIndex = (void*)(scemediasync_text_addr + g_offs->mediasync_patch.sceSystemFileGetIndex);

	if (rebootex_conf.iso_mode != NORMAL_MODE) {
		// patch MsCheckMedia
		// MsCheckMedia: mediasync used it to check EBOOT.PBP
		// Let it return 1 always
		MAKE_DUMMY_FUNCTION_RETURN_1(scemediasync_text_addr + g_offs->mediasync_patch.MsCheckMediaCheck);

		// patch DiscCheckMedia
		_sw(0x1000001D, scemediasync_text_addr + g_offs->mediasync_patch.DiscCheckMediaCheck1);
		_sw(0x1000001D, scemediasync_text_addr + g_offs->mediasync_patch.DiscCheckMediaCheck2);
	}

	// patch MsSystemFile
	_sw(0x1000FFDB, scemediasync_text_addr + g_offs->mediasync_patch.MsSystemFileCheck);

	// Patch check on homebrews without DISC_ID
	_sw(NOP, scemediasync_text_addr + g_offs->mediasync_patch.DiscIDCheck1);
	_sw(NOP, scemediasync_text_addr + g_offs->mediasync_patch.DiscIDCheck2);

	if(g_p2_size != 24 || g_p9_size != 24) {
		printk("%s: p2/p9 %d/%d\n", __func__, (int)g_p2_size, (int)g_p9_size);

		if(psp_model != PSP_1000) {
			patch_partitions();
			sync_cache();
		}
	} else {
		init_file = sceKernelInitFileName();

		if(is_pbp(init_file)) {
			_sw(MAKE_CALL(_sceSystemFileGetIndex), scemediasync_text_addr + g_offs->mediasync_patch.sceSystemFileGetIndexCall);
		}
	}
}
