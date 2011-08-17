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
#include <pspkernel.h>
#include <pspsysmem_kernel.h>
#include <pspthreadman_kernel.h>
#include <pspdebug.h>
#include <pspinit.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "pspcrypt.h"
#include "psputilsforkernel.h"
#include "systemctrl.h"
#include "printk.h"
#include "psid_check.h"
#include "utils.h"

#define KIRK_PRNG_CMD 0xE

static u8 g_trust_users_hash[][20] = {
};

static u32 (*_sceOpenPSIDGetOpenPSID)(u8 psid[0x10]);

static int is_trusted_user(u8 *shabuf)
{
	int i;

	for(i=0; i<NELEMS(g_trust_users_hash); ++i) {
		if (memcmp(shabuf, g_trust_users_hash[i], sizeof(g_trust_users_hash[i])) == 0)
			return 1;
	}

	return 0;
}

// trust - 0
// crash - 1
int confirm_usage_right(void)
{
	u8 shabuf[20];
	u8 psid[16];
	int i;

	memset(psid, 0, sizeof(psid));
	_sceOpenPSIDGetOpenPSID = (void*)sctrlHENFindFunction("sceOpenPSID_Service", "sceOpenPSID_driver", 0xC69BEBCE);
	(*_sceOpenPSIDGetOpenPSID)(psid);

	printk("Dumping PSID:\n");
	printk( "%02X%02X%02X%02X"
			"%02X%02X%02X%02X"
			"%02X%02X%02X%02X"
			"%02X%02X%02X%02X\n", psid[0], psid[1], psid[2], psid[3], psid[4], psid[5], psid[6], psid[7], psid[8], psid[9], psid[10], psid[11], psid[12], psid[13], psid[14], psid[15]);

	memset(shabuf, 0, sizeof(shabuf));
	sceKernelUtilsSha1Digest(psid, sizeof(psid), shabuf);

	for(i=0; i<512-1; ++i) {
		sceKernelUtilsSha1Digest(shabuf, sizeof(shabuf), shabuf);
	}

	return !is_trusted_user(shabuf);
}

static int crash_thread(SceSize args, void *argp)
{
	while (1) {
		u32 ret;
		int i;

		ret = (0x88000000 + (sctrlKernelRand() % 0x400000)) & 0xFFFFFFF0;
		printk("%s: 0x%08X\n", __func__, ret);

		for(i=0; i<16; ++i) {
			*((u8*)(ret+i)) = '\0';
		}

		sceKernelDelayThread(1 * 1000000L);
	}

	return 0;
}

int crash_me(void)
{
	SceUID thid;

	thid = sceKernelCreateThread("", &crash_thread, 0x10, 0x1000, 0, 0);

	if (thid >= 0) {
		sceKernelStartThread(thid, 0, NULL);
	} else {
		memset((void*)0x88000000, 0, 0x20000);
	}

	return 0;
}
