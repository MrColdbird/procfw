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

#include <string.h>
#include "systemctrl.h"
#include "systemctrl_se.h"
#include "printk.h"

SEConfig conf;

#define CONFIG_MAGIC 0x47434554

static inline u32 get_conf_magic(void)
{
	u32 version;

	version = (sctrlHENGetVersion() & 0xF);
	version = (version << 16) | sctrlHENGetMinorVersion();
	version += CONFIG_MAGIC;

	return version;
}

int GetConfig(SEConfig *config)
{
	SceUID fd;
	u32 k1;
   
	k1 = pspSdkSetK1(0);
	memset(config, 0, sizeof(*config));
	fd = sceIoOpen("flash1:/config.se", PSP_O_RDONLY, 0644);

	if (fd < 0) {
		pspSdkSetK1(k1);

		return -1;
	}

	if (sceIoRead(fd, config, sizeof(*config)) != sizeof(*config)) {
		sceIoClose(fd);
		pspSdkSetK1(k1);

		return -2;
	}

	sceIoClose(fd);
	pspSdkSetK1(k1);

	return 0;
}

int SetConfig(SEConfig *config)
{
	u32 k1;
	SceUID fd;

	k1 = pspSdkSetK1(0);
	sceIoRemove("flash1:/config.se");
	fd = sceIoOpen("flash1:/config.se", PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);

	if (fd < 0) {
		pspSdkSetK1(k1);

		return -1;
	}

	config->magic = get_conf_magic();

	if (sceIoWrite(fd, config, sizeof(*config)) != sizeof(*config)) {
		sceIoClose(fd);
		pspSdkSetK1(k1);

		return -1;
	}

	sceIoClose(fd);
	pspSdkSetK1(k1);

	return 0;
}

int sctrlSESetConfigEx(SEConfig *config, int size)
{
	u32 k1;
	SceUID fd;
	int written;
   
	k1 = pspSdkSetK1(0);
	fd = sceIoOpen("flash1:/config.se", PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);

	if (fd < 0) {
		pspSdkSetK1(k1);

		return -1;
	}

	config->magic = get_conf_magic();

	written = sceIoWrite(fd, config, size);

	if (written != size) {
		sceIoClose(fd);
		pspSdkSetK1(k1);

		return -1;
	}

	sceIoClose(fd);
	pspSdkSetK1(k1);

	return 0;
}

int sctrlSEGetConfigEx(SEConfig *config, int size)
{
	int read;
	u32 k1;
	SceUID fd;

	read = -1;
	k1 = pspSdkSetK1(0);
	memset(config, 0, size);
	fd = sceIoOpen("flash1:/config.se", PSP_O_RDONLY, 0666);

	if (fd > 0) {
		read = sceIoRead(fd, config, size);
		sceIoClose(fd);
		pspSdkSetK1(k1);
		
		return (read == size) ? 0 : -2;
	}

	pspSdkSetK1(k1);

	return -1;
}

int sctrlSESetConfig(SEConfig *config)
{
	return sctrlSESetConfigEx(config, sizeof(*config));
}

int sctrlSEGetConfig(SEConfig *config)
{
	return sctrlSEGetConfigEx(config, sizeof(*config));
}

void load_default_conf(SEConfig *config)
{
	memset(config, 0, sizeof(*config));
	config->magic = get_conf_magic();
	config->umdmode = MODE_INFERNO;
	config->usbcharge = 0;
	config->machidden = 1;
	config->skipgameboot = 0;
	config->hidepic = 0;
	config->plugvsh = config->pluggame = config->plugpop = 1;
	config->usbdevice = 0;
	config->flashprot = 1;
	config->fakeregion = FAKE_REGION_DISABLED;
	config->skiplogo = 0;
	config->useversion = 0;
	config->useownupdate = 1;
	config->usenodrm = 1;
	config->hibblock = 1;
	config->noanalog = 0;
	config->oldplugin = 1;
	config->htmlviewer_custom_save_location = 1;
	config->hide_cfw_dirs = 1;
	config->chn_iso = 1;
	config->msspeed = MSSPEED_NONE;
	config->inferno_cache = 0;
	config->inferno_cache_total_size = 20;
	config->inferno_cache_num = 128;
	config->inferno_cache_policy = CACHE_POLICY_LRU;
}

void load_config(void)
{
	int ret;
	
	ret = GetConfig(&conf);

	if (ret != 0) {
		load_default_conf(&conf);
		SetConfig(&conf);
	} else {
		printk("Loading config OK\n");
	}
}
