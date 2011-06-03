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
#include "main.h"
#include "systemctrl_patch_offset.h"

static int (*msstor_read)(PspIoDrvFileArg *arg, char *data, int len) = NULL;
static int (*msstor_write)(PspIoDrvFileArg *arg, const char *data, int len) = NULL;
static SceOff (*msstor_lseek)(PspIoDrvFileArg *arg, SceOff ofs, int whence) = NULL;

static char *msstor_cache_read_buf = NULL;
static SceOff msstor_cache_read_pos = -1;
static int msstor_cache_read_bufsize = 0;

static u32 read_call = 0;
static u32 read_hit = 0;
static u32 read_missed = 0;
static u32 read_uncacheable = 0;

static int msstor_cache_read(PspIoDrvFileArg *arg, char *data, int len)
{
	int ret;
	SceOff pos;
	int read_len;

	pos = (*msstor_lseek)(arg, 0, PSP_SEEK_CUR);

	// cache hit?
	if(msstor_cache_read_pos != -1 && pos >= msstor_cache_read_pos && pos + len <= msstor_cache_read_pos + msstor_cache_read_bufsize) {
		read_len = MIN(len, msstor_cache_read_bufsize - (pos - msstor_cache_read_pos));
		memcpy(data, msstor_cache_read_buf + pos - msstor_cache_read_pos, read_len);
		ret = read_len;
		(*msstor_lseek)(arg, pos + ret, PSP_SEEK_SET);
		read_hit++;
	} else if(len <= msstor_cache_read_bufsize) {
		ret = (*msstor_read)(arg, msstor_cache_read_buf, msstor_cache_read_bufsize);

		if(ret >= 0) {
			read_len = MIN(len, ret);
			memcpy(data, msstor_cache_read_buf, read_len);
			ret = read_len;
			msstor_cache_read_pos = pos;
			(*msstor_lseek)(arg, pos + ret, PSP_SEEK_SET);
		} else {
			printk("%s: read -> 0x%08X\n", __func__, ret);
			msstor_cache_read_pos = -1; // disable cache
		}

		read_missed++;
	} else {
		ret = (*msstor_read)(arg, data, len);
//		printk("%s: read len %d too large\n", __func__, len);
		read_uncacheable++;
	}

	read_call++;

	return ret;
}

static int msstor_cache_write(PspIoDrvFileArg *arg, const char *data, int len)
{
	int ret;
	SceOff pos;

	pos = (*msstor_lseek)(arg, 0, PSP_SEEK_CUR);

	if(pos >= msstor_cache_read_pos && pos < msstor_cache_read_pos + msstor_cache_read_bufsize) {
		msstor_cache_read_pos = -1; // disable cache
	} else if(pos + len >= msstor_cache_read_pos && pos + len <= msstor_cache_read_pos + msstor_cache_read_bufsize) {
		msstor_cache_read_pos = -1; // disable cache
	}

	ret = (*msstor_write)(arg, data, len);

	return ret;
}

int msstor_init(int bufnum)
{
	PspIoDrvFuncs *funcs;
	PspIoDrv *pdrv;
	SceUID memid;
	
	if(bufnum < 6) {
		return -1;
	}

	memid = sctrlKernelAllocPartitionMemory(9, "MsStorCache", PSP_SMEM_High, bufnum * 0x200 + 64, NULL);

	if(memid < 0) {
		printk("%s: sctrlKernelAllocPartitionMemory -> 0x%08X\n", __func__, memid);
		printk("%s: retry with p2\n", __func__);
		memid = sctrlKernelAllocPartitionMemory(2, "MsStorCache", PSP_SMEM_High, bufnum * 0x200, NULL);

		if(memid < 0) {
			printk("%s: sctrlKernelAllocPartitionMemory #2 -> 0x%08X\n", __func__, memid);
			return -2;
		}
	}

	msstor_cache_read_buf = sctrlKernelGetBlockHeadAddr(memid);

	if(msstor_cache_read_buf == NULL) {
		return -3;
	}

	msstor_cache_read_buf = (void*)(((u32)msstor_cache_read_buf) & ~(64-1));
	msstor_cache_read_bufsize = bufnum * 0x200;
	memset(msstor_cache_read_buf, 0, msstor_cache_read_bufsize);
	
	if(psp_model == PSP_GO && sctrlKernelBootFrom() == 0x50) {
		pdrv = sctrlHENFindDriver("eflash0a0f1p");
	} else {
		pdrv = sctrlHENFindDriver("msstor0p");
	}

	if(pdrv == NULL) {
		return -4;
	}

	funcs = pdrv->funcs;
	msstor_read = funcs->IoRead;
	msstor_write = funcs->IoWrite;
	msstor_lseek = funcs->IoLseek;
	funcs->IoRead = msstor_cache_read;
	funcs->IoWrite = msstor_cache_write;

	return 0;
}

// call @SystemControl:SystemCtrlPrivate,0xD3014719@
void msstor_stat(void)
{
	char buf[256];

	if(read_call != 0) {
		sprintf(buf, "%s: bufsize: %d\n", __func__, msstor_cache_read_bufsize / 1024);
		sceIoWrite(1, buf, strlen(buf));
		sprintf(buf, "hit percent: %02d%%/%02d%%/%02d%%, [%d/%d/%d/%d]\n", 100 * read_hit / read_call, 100 * read_missed / read_call, 100 * read_uncacheable / read_call, read_hit, read_missed, read_uncacheable, read_call);
		sceIoWrite(1, buf, strlen(buf));
	} else {
		sprintf(buf, "no msstor cache call yet\n");
		sceIoWrite(1, buf, strlen(buf));
	}
}
