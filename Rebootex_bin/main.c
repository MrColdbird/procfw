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
#include "rebootex_conf.h"
#include "utils.h"
#include "config.h"
#include "rebootex_bin_patch_offset.h"
#include "../Permanent/ppatch_config.h"

typedef struct _btcnf_header
{
	unsigned int signature; // 0
	unsigned int devkit; // 4
	unsigned int unknown[2]; // 8
	unsigned int modestart; // 0x10
	int nmodes; // 0x14
	unsigned int unknown2[2]; // 0x18
	unsigned int modulestart; // 0x20
	int nmodules; // 0x24
	unsigned int unknown3[2]; // 0x28
	unsigned int modnamestart; // 0x30
	unsigned int modnameend; // 0x34
	unsigned int unknown4[2]; // 0x38
} _btcnf_header __attribute((packed));

typedef struct _btcnf_module
{
	unsigned int module_path; // 0x00
	unsigned int unk_04; //0x04
	unsigned int flags; //0x08
	unsigned int unk_C; //0x0C
	unsigned char hash[0x10]; //0x10
} _btcnf_module __attribute((packed));

rebootex_config *conf;

//io flags
int rebootmodule_open = 0;

//rtm data
char * loadrebootmodulebefore = NULL;
char * rebootmodule = NULL;
int size_rebootmodule = 0;
int rebootmoduleflags = 0;
u32 psp_fw_version = 0;
u8 psp_model = 0;
u8 recovery_mode = 0;
u8 ofw_mode = 0;

PspBootConfMode iso_mode = 0;

//io functions
int (* sceBootLfatOpen)(char * filename) = NULL;
int (* sceBootLfatRead)(char * buffer, int length) = NULL;
int (* sceBootLfatClose)(void) = NULL;
int (* UnpackBootConfig)(char * buffer, int length) = NULL;

//loadcore functions
int (* DecryptPSP)(char * prx, unsigned int size, unsigned int * newsize) = 0;
int (* sceKernelCheckExecFile)(unsigned char * addr, void * arg2) = NULL;

//cache sync
static inline int iCacheFlushAll(void);
static inline int dCacheFlushAll(void);

//helper functions
int _strlen(char * string);
int _strcmp(char * str1, char * str2);
int _strcpy(char * to, char * from);
int _memcmp(char * buf1, char * buf2, int length);
int _memcpy(char * to, char * from, unsigned int length);
int _memset(unsigned char * buffer, unsigned char value, unsigned int length);

//io replacements
int _sceBootLfatOpen(char * filename);
int _sceBootLfatRead(char * buffer, int length);
int _sceBootLfatClose (void);
int _UnpackBootConfig(char ** buffer, int length);

//loadcore replacements
int _DecryptPSP(char * prx, unsigned int size, unsigned int * newsize);
int _sceKernelCheckExecFile(unsigned char * addr, void * arg2);

//patch functions
int PatchLoadCore(void * arg1, void * arg2, void * arg3, int (* module_bootstart)(void *, void *, void *));

//screen debugger
void freezeme(unsigned int color);

//reboot function
void (* reboot)(int arg1, int arg2, int arg3, int arg4) = (void *)REBOOT_START;

void load_configure(void);

int RenameModule(void *buffer, char *mod_name, char *new_mod_name);
int AddPRX(char * buffer, char * insertbefore, char * prxname, u32 flags);
int AddPRXNoCopyName(char * buffer, char * insertbefore, int prxname_offset, u32 flags);
void RemovePrx(char *buffer, const char *prxname, u32 flags);
void ModifyPrxFlag(char *buffer, const char* modname, u32 flags);
int MovePrx(char * buffer, char * insertbefore, const char * prxname, u32 flags);
int GetPrxFlag(char *buffer, const char* modname, u32 *flag);

// NOTICE: main must be the FIRST soubroutine of Rebootex
void main(int arg1, int arg2, int arg3, int arg4)
{
	struct RebootexPatch *patch;

	load_configure();
	setup_patch_offset_table(psp_fw_version);

	if(psp_model == PSP_1000) {
		patch = &g_offs->rebootex_patch_01g;
	} else {
		patch = &g_offs->rebootex_patch_other;
	}

	sceBootLfatOpen = (void *)REBOOT_START + patch->sceBootLfatOpen;
	sceBootLfatRead = (void *)REBOOT_START + patch->sceBootLfatRead;
	sceBootLfatClose = (void *)REBOOT_START + patch->sceBootLfatClose;
	UnpackBootConfig = (void *)REBOOT_START + patch->UnpackBootConfig;

	if(ofw_mode) {
		_sw(MAKE_CALL(_UnpackBootConfig), REBOOT_START + patch->UnpackBootConfigCall);
		_sw(0x27A40004, REBOOT_START + patch->UnpackBootConfigBufferAddress); // addiu $a0, $sp, 4
		
		goto exit;
	}

	_sw(MAKE_CALL(_sceBootLfatOpen), REBOOT_START + patch->sceBootLfatOpenCall);
	_sw(MAKE_CALL(_sceBootLfatRead), REBOOT_START + patch->sceBootLfatReadCall);
	_sw(MAKE_CALL(_sceBootLfatClose), REBOOT_START + patch->sceBootLfatCloseCall);
	_sw(MAKE_CALL(_UnpackBootConfig), REBOOT_START + patch->UnpackBootConfigCall);

	//Killing Function
	MAKE_DUMMY_FUNCTION_RETURN_1(REBOOT_START + patch->RebootexCheck1);
	//Killing Branch Check bltz ...
	_sw(NOP, REBOOT_START + patch->RebootexCheck2);
	//Killing Branch Check bltz ...
	_sw(NOP, REBOOT_START + patch->RebootexCheck3);
	//Killing Branch Check beqz ...
	_sw(NOP, REBOOT_START + patch->RebootexCheck4);
	//Killing Branch Check bltz ...
	_sw(NOP, REBOOT_START + patch->RebootexCheck5);
	//Prepare LoadCore Patch Part #1 - addu $a3, $zr, $s1 - Stores module_start ($s1) as fourth argument.
	_sw(0x00113821, REBOOT_START + patch->LoadCoreModuleStartCall - 4);
	//Prepare LoadCore Patch Part #2 - jal PatchLoadCore
	_sw(MAKE_JUMP(PatchLoadCore), REBOOT_START + patch->LoadCoreModuleStartCall);
	//Prepare LoadCore Patch Part #3 - move $sp, $s5 - Backed up instruction.
	_sw(0x02A0E821, REBOOT_START + patch->LoadCoreModuleStartCall + 4);
	//UnpackBootConfig buffer address
	_sw(0x27A40004, REBOOT_START + patch->UnpackBootConfigBufferAddress); // addiu $a0, $sp, 4

	//initializing global variables
	rebootmodule_open = 0;

exit:
	//reboot psp
	//flush instruction cache
	dCacheFlushAll();
	iCacheFlushAll();

	reboot(arg1, arg2, arg3, arg4);
}

//cache sync
static inline int iCacheFlushAll(void)
{
	int (*_iCacheFlushAll)(void) = (void *)(REBOOT_START + g_offs->iCacheFlushAll);
	
	return (*_iCacheFlushAll)();
}

static inline int dCacheFlushAll(void)
{
	int (*_dCacheFlushAll)(void) = (void *)(REBOOT_START + g_offs->dCacheFlushAll);
	
	return (*_dCacheFlushAll)();
}

void load_configure(void)
{
	rebootex_config *conf = (rebootex_config *)(REBOOTEX_CONFIG);
	
	if(conf->magic == REBOOTEX_CONFIG_MAGIC) {
		iso_mode = conf->iso_mode;
		loadrebootmodulebefore = conf->insert_module_before;
		rebootmodule = conf->insert_module_binary;
		size_rebootmodule = conf->insert_module_size;
		rebootmoduleflags = conf->insert_module_flags;
		psp_fw_version = conf->psp_fw_version;
		iso_mode = conf->iso_mode;
		psp_model = conf->psp_model;
		recovery_mode = conf->recovery_mode;
		ofw_mode = conf->ofw_mode;
	}
}

int _strlen(char * string)
{
	//length
	int length = 0;

	//count bytes
	for(; string[length]; length++);

	return length;
}

int _memcmp(char * buf1, char * buf2, int length)
{
	//result
	int result = -1;

	//compare bytes
	int pos = 0; for(; pos < length; pos++)
	{
		//store comparison
		result = buf1[pos] - buf2[pos];

		//stop comparison
		if(result) break;
	}

	return result;
}

int _strcmp(char * str1, char * str2)
{
	//result
	int result = -1;

	//valid length
	if(_strlen(str1) == _strlen(str2))
	{
		//compare bytes
		result = _memcmp(str1, str2, _strlen(str1));
	}

	return result;
}

void _memmove(char * to, char * from, unsigned int length)
{
	//result
	int result = 0;

	if (to > from) {
		//back buffer
		char * tob = to + length;
		char * fromb = from + length;

		//loop copy
		unsigned int pos = 0; for(; pos < length; pos++)
		{
			//copy byte
			*--tob = *--fromb;

			//increment result
			result++;
		}

		return result;
	}

	return _memcpy(to, from, length);
}

int _memcpy(char * to, char * from, unsigned int length)
{
	//result
	int result = 0;

	//loop copy
	unsigned int pos = 0; for(; pos < length; pos++)
	{
		//copy byte
		to[pos] = from[pos];

		//increment result
		result++;
	}

	return result;
}

int _strcpy(char * to, char * from)
{
	//result
	int result = 0;

	//copy bytes
	result = _memcpy(to, from, _strlen(from) + 1);

	return result;
}

int _memset(unsigned char * buffer, unsigned char value, unsigned int length)
{
	//result
	int result = 0;

	//loop set
	unsigned int pos = 0; for(; pos < length; pos++)
	{
		//set byte
		buffer[pos] = value;

		//increment result
		result++;
	}

	return result;
}

static char *p_rmod;
static int size_rmod;

int _sceBootLfatRead(char * buffer, int length)
{
	//load on reboot module
	if(rebootmodule_open)
	{
		int min;

		//copy load on reboot module
		min = size_rmod < length ? size_rmod : length;
		_memcpy(buffer, p_rmod, min);
		p_rmod += min;
		size_rmod -= min;

		//set filesize
		return min;
	}

	//forward to original function
	return sceBootLfatRead(buffer, length);
}

int _sceBootLfatOpen(char * filename)
{
	//load on reboot module open
	if(_strcmp(filename, "/rtm.prx") == 0)
	{
		//mark for read
		rebootmodule_open = 1;
		p_rmod = rebootmodule;
		size_rmod = size_rebootmodule;

		//return success
		return 0;
	}

	//forward to original function
	return sceBootLfatOpen(filename);
}

int _sceBootLfatClose(void)
{
	//reboot module close
	if(rebootmodule_open)
	{
		//mark as closed
		rebootmodule_open = 0;

		//return success
		return 0;
	}

	//forward to original function
	return sceBootLfatClose();
}

int _DecryptPSP(char * prx, unsigned int size, unsigned int * newsize)
{
	//check for gzip packed prx
	if(*(unsigned int *)(prx + 0x130) == 0xC01DB15D)
	{
		//read compressed size
		unsigned int compsize = *(unsigned int *)(prx + 0xB0);

		//set newsize
		*newsize = compsize;

		//strip ~PSP header
		_memcpy(prx, prx + 0x150, compsize);

		//return success
		return 0;
	}

	//forward to original function
	return DecryptPSP(prx, size, newsize);
}

int _sceKernelCheckExecFile(unsigned char * addr, void * arg2)
{
	//scan structure
	//6.31 kernel modules use type 3 PRX... 0xd4~0x10C is zero padded
	int pos = 0; for(; pos < 0x38; pos++)
	{
		//nonzero byte encountered
		if(addr[pos + 212])
		{
			//forward to unsign function?
			return sceKernelCheckExecFile(addr, arg2);
		}
	}

	//return success
	return 0;
}

int PatchLoadCore(void * arg1, void * arg2, void * arg3, int (* module_bootstart)(void *, void *, void *))
{
	_sw((((int)(_DecryptPSP) >> 2) & 0x03FFFFFF) | 0x0C000000, (unsigned int)(module_bootstart) + g_offs->loadcore_patch.DecryptPSPCall1);
	_sw((((int)(_DecryptPSP) >> 2) & 0x03FFFFFF) | 0x0C000000, (unsigned int)(module_bootstart) + g_offs->loadcore_patch.DecryptPSPCall2);
	_sw((((int)(_sceKernelCheckExecFile) >> 2) & 0x03FFFFFF) | 0x0C000000, (unsigned int)(module_bootstart) + g_offs->loadcore_patch.sceKernelCheckExecFileCall1);
	_sw((((int)(_sceKernelCheckExecFile) >> 2) & 0x03FFFFFF) | 0x0C000000, (unsigned int)(module_bootstart) + g_offs->loadcore_patch.sceKernelCheckExecFileCall2);
	_sw((((int)(_sceKernelCheckExecFile) >> 2) & 0x03FFFFFF) | 0x0C000000, (unsigned int)(module_bootstart) + g_offs->loadcore_patch.sceKernelCheckExecFileCall3);

	DecryptPSP = (void *)((unsigned int)(module_bootstart) + g_offs->loadcore_patch.DecryptPSP);
	sceKernelCheckExecFile = (void *)((unsigned int)(module_bootstart) + g_offs->loadcore_patch.sceKernelCheckExecFile);

	//flush instruction cache
	iCacheFlushAll();

	//call module_start
	return module_bootstart(arg1, arg2, arg3);
}

int patch_bootconf_vsh(char *buffer, int length)
{
	int newsize, result;

	result = length;
	newsize = AddPRX(buffer, "/kd/vshbridge.prx", PATH_VSHCTRL+sizeof(PATH_FLASH0)-2, VSH_RUNLEVEL );

	if (newsize > 0) result = newsize;

	return result;
}

int patch_bootconf_pops(char *buffer, int length)
{
	int newsize, result;

	result = length;
	newsize = AddPRX(buffer, "/kd/usersystemlib.prx", PATH_POPCORN+sizeof(PATH_FLASH0)-2, POPS_RUNLEVEL);

	if (newsize > 0) result = newsize;

	return result;
}

struct add_module {
	char *prxname;
	char *insertbefore;
	u32 flags;
};

struct del_module {
	char *prxname;
	u32 flags;
};

static struct add_module np9660_add_mods[] = {
	{ "/kd/mgr.prx", "/kd/amctrl.prx", GAME_RUNLEVEL },
	{ "/kd/npdrm.prx", "/kd/iofilemgr_dnas.prx", GAME_RUNLEVEL },
	{ PATH_GALAXY+sizeof(PATH_FLASH0)-2, "/kd/np9660.prx", UMDEMU_RUNLEVEL },
	{ PATH_GALAXY+sizeof(PATH_FLASH0)-2, "/kd/utility.prx", GAME_RUNLEVEL },
	{ "/kd/np9660.prx", "/kd/utility.prx", GAME_RUNLEVEL },
	{ "/kd/isofs.prx", "/kd/utility.prx", GAME_RUNLEVEL },
};

static struct del_module np9660_del_mods[] = {
	{ "/kd/mediaman.prx", GAME_RUNLEVEL },
	{ "/kd/ata.prx", GAME_RUNLEVEL },
	{ "/kd/umdman.prx", GAME_RUNLEVEL },
	{ "/kd/umdcache.prx", GAME_RUNLEVEL },
	{ "/kd/umd9660.prx", GAME_RUNLEVEL },
};

int patch_bootconf_np9660(char *buffer, int length)
{
	int newsize, result, ret;

	result = length;

	int i; for(i=0; i<NELEMS(np9660_del_mods); ++i) {
		RemovePrx(buffer, np9660_del_mods[i].prxname, np9660_del_mods[i].flags);
	}

	for(i=0; i<NELEMS(np9660_add_mods); ++i) {
		newsize = MovePrx(buffer, np9660_add_mods[i].insertbefore, np9660_add_mods[i].prxname, np9660_add_mods[i].flags);

		if (newsize > 0) result = newsize;
	}

	return result;
}

static struct add_module march33_add_mods[] = {
	{ "/kd/mgr.prx", "/kd/amctrl.prx", GAME_RUNLEVEL },
	{ PATH_MARCH33+sizeof(PATH_FLASH0)-2, "/kd/utility.prx", GAME_RUNLEVEL },
	{ PATH_MARCH33+sizeof(PATH_FLASH0)-2, "/kd/isofs.prx", UMDEMU_RUNLEVEL },
	{ "/kd/isofs.prx", "/kd/utility.prx", GAME_RUNLEVEL },
};

static struct del_module march33_del_mods[] = {
	{ "/kd/mediaman.prx", GAME_RUNLEVEL },
	{ "/kd/ata.prx", GAME_RUNLEVEL },
	{ "/kd/umdman.prx", GAME_RUNLEVEL },
	{ "/kd/umdcache.prx", GAME_RUNLEVEL },
	{ "/kd/umd9660.prx", GAME_RUNLEVEL },
	{ "/kd/np9660.prx", UMDEMU_RUNLEVEL },
};

int patch_bootconf_march33(char *buffer, int length)
{
	int newsize, result, ret;

	result = length;

	int i; for(i=0; i<NELEMS(march33_del_mods); ++i) {
		RemovePrx(buffer, march33_del_mods[i].prxname, march33_del_mods[i].flags);
	}

	for(i=0; i<NELEMS(march33_add_mods); ++i) {
		newsize = MovePrx(buffer, march33_add_mods[i].insertbefore, march33_add_mods[i].prxname, march33_add_mods[i].flags);

		if (newsize > 0) result = newsize;
	}

	return result;
}

static struct add_module inferno_add_mods[] = {
	{ "/kd/mgr.prx", "/kd/amctrl.prx", GAME_RUNLEVEL },
	{ PATH_INFERNO+sizeof(PATH_FLASH0)-2, "/kd/utility.prx", GAME_RUNLEVEL },
	{ PATH_INFERNO+sizeof(PATH_FLASH0)-2, "/kd/isofs.prx", UMDEMU_RUNLEVEL },
	{ "/kd/isofs.prx", "/kd/utility.prx", GAME_RUNLEVEL },
};

static struct del_module inferno_del_mods[] = {
	{ "/kd/mediaman.prx", GAME_RUNLEVEL },
	{ "/kd/ata.prx", GAME_RUNLEVEL },
	{ "/kd/umdman.prx", GAME_RUNLEVEL },
	{ "/kd/umdcache.prx", GAME_RUNLEVEL },
	{ "/kd/umd9660.prx", GAME_RUNLEVEL },
	{ "/kd/np9660.prx", UMDEMU_RUNLEVEL },
};

int patch_bootconf_inferno(char *buffer, int length)
{
	int newsize, result, ret;

	result = length;

	int i; for(i=0; i<NELEMS(inferno_del_mods); ++i) {
		RemovePrx(buffer, inferno_del_mods[i].prxname, inferno_del_mods[i].flags);
	}

	for(i=0; i<NELEMS(inferno_add_mods); ++i) {
		newsize = MovePrx(buffer, inferno_add_mods[i].insertbefore, inferno_add_mods[i].prxname, inferno_add_mods[i].flags);

		if (newsize > 0) result = newsize;
	}

	return result;
}

static struct add_module vshumd_add_mods[] = {
	{ "/kd/isofs.prx", "/kd/utility.prx", VSH_RUNLEVEL },
	{ PATH_INFERNO+sizeof(PATH_FLASH0)-2, "/kd/chnnlsv.prx", VSH_RUNLEVEL },
};

static struct del_module vshumd_del_mods[] = {
	{ "/kd/mediaman.prx", VSH_RUNLEVEL },
	{ "/kd/ata.prx", VSH_RUNLEVEL },
	{ "/kd/umdman.prx", VSH_RUNLEVEL },
	{ "/kd/umd9660.prx", VSH_RUNLEVEL },
};

int patch_bootconf_vshumd(char *buffer, int length)
{
	int newsize, result, ret;

	result = length;

	int i; for(i=0; i<NELEMS(vshumd_del_mods); ++i) {
		RemovePrx(buffer, vshumd_del_mods[i].prxname, vshumd_del_mods[i].flags);
	}
	
	for(i=0; i<NELEMS(vshumd_add_mods); ++i) {
		newsize = MovePrx(buffer, vshumd_add_mods[i].insertbefore, vshumd_add_mods[i].prxname, vshumd_add_mods[i].flags);

		if (newsize > 0) result = newsize;
	}

	return result;
}

static struct add_module updaterumd_add_mods[] = {
	{ "/kd/isofs.prx", "/kd/utility.prx", UPDATER_RUNLEVEL },
	{ PATH_INFERNO+sizeof(PATH_FLASH0)-2, "/kd/chnnlsv.prx", UPDATER_RUNLEVEL },
};

static struct del_module updaterumd_del_mods[] = {
	{ "/kd/mediaman.prx", UPDATER_RUNLEVEL },
	{ "/kd/ata.prx", UPDATER_RUNLEVEL },
	{ "/kd/umdman.prx", UPDATER_RUNLEVEL },
	{ "/kd/umd9660.prx", UPDATER_RUNLEVEL },
};

int patch_bootconf_updaterumd(char *buffer, int length)
{
	int newsize, result, ret;

	result = length;

	int i; for(i=0; i<NELEMS(updaterumd_del_mods); ++i) {
		RemovePrx(buffer, updaterumd_del_mods[i].prxname, updaterumd_del_mods[i].flags);
	}
	
	for(i=0; i<NELEMS(updaterumd_add_mods); ++i) {
		newsize = MovePrx(buffer, updaterumd_add_mods[i].insertbefore, updaterumd_add_mods[i].prxname, updaterumd_add_mods[i].flags);

		if (newsize > 0) result = newsize;
	}

	return result;
}

int patch_bootconf_fatms371(char *buffer, int length)
{
	int newsize;

	newsize = AddPRX(buffer, "/kd/fatms.prx", PATH_FATMS_HELPER+sizeof(PATH_FLASH0)-2, 0xEF & ~VSH_RUNLEVEL);
	RemovePrx(buffer, "/kd/fatms.prx", 0xEF & ~VSH_RUNLEVEL);
	newsize = AddPRX(buffer, "/kd/wlan.prx", PATH_FATMS_371+sizeof(PATH_FLASH0)-2, 0xEF & ~VSH_RUNLEVEL);

	return newsize;
}

int is_file_existed(const char *path)
{
	int ret;

	ret = (*sceBootLfatOpen)(path);

	if(ret >= 0) {
		(*sceBootLfatClose)();
		return 1;
	}

	return 0;
}

int is_permanent_mode(void)
{
	return is_file_existed(VSHORIG + sizeof("flash0:") - 1);
}

int is_fatms371(void)
{
	return is_file_existed(PATH_FATMS_HELPER + sizeof("flash0:") - 1) && is_file_existed(PATH_FATMS_371 + sizeof("flash0:") - 1);
}

int _UnpackBootConfig(char **p_buffer, int length)
{
	int result;
	int newsize;
	char *buffer;

	result = (*UnpackBootConfig)(*p_buffer, length);
	buffer = (void*)BOOTCONFIG_TEMP_BUFFER;
	_memcpy(buffer, *p_buffer, length);
	*p_buffer = buffer;

	if(ofw_mode) {
		goto exit;
	}

	newsize = AddPRX(buffer, "/kd/init.prx", PATH_SYSTEMCTRL+sizeof(PATH_FLASH0)-2, 0x000000EF);

	if (newsize > 0) result = newsize;

	if (SearchPrx(buffer, "/vsh/module/vshmain.prx") >= 0) {
		newsize = patch_bootconf_vsh(buffer, length);

		if (newsize > 0) result = newsize;
	}

	newsize = patch_bootconf_pops(buffer, length);

	if (newsize > 0) result = newsize;

	newsize = AddPRX(buffer, "/kd/me_wrapper.prx", PATH_STARGATE+sizeof(PATH_FLASH0)-2, GAME_RUNLEVEL | UMDEMU_RUNLEVEL);

	if (newsize > 0) result = newsize;
	
	switch(iso_mode) {
		case NP9660_MODE:
			newsize = patch_bootconf_np9660(buffer, length);

			if (newsize > 0) result = newsize;
			break;
		case MARCH33_MODE:
			newsize = patch_bootconf_march33(buffer, length);

			if (newsize > 0) result = newsize;
			break;
		case INFERNO_MODE:
			newsize = patch_bootconf_inferno(buffer, length);

			if (newsize > 0) result = newsize;
			break;
		case VSHUMD_MODE:
			newsize = patch_bootconf_vshumd(buffer, length);

			if (newsize > 0) result = newsize;
			break;
		case UPDATERUMD_MODE:
			newsize = patch_bootconf_updaterumd(buffer, length);

			if (newsize > 0) result = newsize;
			break;
		case NORMAL_MODE:
		default:
			break;
	}

	//reboot variable set
	if(loadrebootmodulebefore)
	{
		//add reboot prx entry
		newsize = AddPRX(buffer, loadrebootmodulebefore, "/rtm.prx", rebootmoduleflags);

		if(newsize > 0) result = newsize;
	}

	if(!recovery_mode && is_fatms371())
	{
		newsize = patch_bootconf_fatms371(buffer, length);

		if (newsize > 0) result = newsize;
	}

exit:
	if((!recovery_mode || ofw_mode) && is_permanent_mode()) {
		RenameModule(buffer, VSHMAIN + sizeof(PATH_FLASH0) - 2, VSHORIG + sizeof(PATH_FLASH0) - 2);
	}

	return result;
}

int SearchPrx(char *buffer, const char *modname)
{
	//cast header
	_btcnf_header * header = (_btcnf_header *)buffer;

	if(header->signature != BTCNF_MAGIC) {
		return -1;
	}

	if(header->nmodules <= 0) {
		return -2;
	}

	if(header->nmodes <= 0) {
		return -3;
	}

	//cast module list
	_btcnf_module * module = (_btcnf_module *)(buffer + header->modulestart);

	//iterate modules
	int modnum = 0; for(; modnum < header->nmodules; modnum++)
	{
		//found module name
		if(_strcmp(buffer + header->modnamestart + module[modnum].module_path, modname) == 0)
		{
			//stop search
			break;
		}
	}

	//found module
	if(modnum >= header->nmodules) {
		return -4;
	}

	return modnum;
}

int AddPRXNoCopyName(char * buffer, char * insertbefore, int prxname_offset, u32 flags)
{
	int modnum;

	modnum = SearchPrx(buffer, insertbefore);

	if (modnum < 0) {
		return modnum;
	}

	_btcnf_header * header = (_btcnf_header *)buffer;

	//cast module list
	_btcnf_module * module = (_btcnf_module *)(buffer + header->modulestart);

	//add custom module
	_btcnf_module newmod; _memset(&newmod, 0, sizeof(newmod));

	newmod.module_path = prxname_offset - header->modnamestart;

	if(flags >= 0x0000FFFF) {
		newmod.flags = flags;
	} else {
		newmod.flags = 0x80010000 | (flags & 0x0000FFFF);
	}

	_memmove(&module[modnum + 1], &module[modnum + 0], buffer + header->modnameend - (unsigned int)&module[modnum + 0]);
	_memcpy(&module[modnum + 0], &newmod, sizeof(newmod));
	header->nmodules++;
	header->modnamestart += sizeof(newmod);
	header->modnameend += sizeof(newmod);

	//make mode include our module
	int modenum = 0; for(; modenum < header->nmodes; modenum++)
	{
		//increase module range
		*(unsigned short *)(buffer + header->modestart + modenum * 32) += 1;
	}

	return header->modnameend;
}

int AddPRX(char * buffer, char * insertbefore, char * prxname, u32 flags)
{
	int modnum;

	modnum = SearchPrx(buffer, prxname);

	if (modnum >= 0) {
		_btcnf_header * header = (_btcnf_header *)buffer;
		_btcnf_module * module = (_btcnf_module *)(buffer + header->modulestart);

		return AddPRXNoCopyName(buffer, insertbefore, header->modnamestart + module[modnum].module_path, flags);
	}

	modnum = SearchPrx(buffer, insertbefore);

	if (modnum < 0) {
		return modnum;
	}

	_btcnf_header * header = (_btcnf_header *)buffer;

	header->modnameend += _strcpy(buffer + header->modnameend, prxname);

	return AddPRXNoCopyName(buffer, insertbefore, header->modnameend - _strlen(prxname) - 1, flags);
}

void RemovePrx(char *buffer, const char *prxname, u32 flags)
{
	u32 old_flags;
	int ret;

	ret = GetPrxFlag(buffer, prxname, &old_flags);

	if (ret < 0)
		return ret;

	old_flags &= 0x0000FFFF;
	flags &= 0x0000FFFF;

	if (old_flags & flags) {
		// rewrite the flags to remove the modules from runlevels indicated by flags
		old_flags = old_flags & (~flags);
	}

	ModifyPrxFlag(buffer, prxname, 0x80010000 | (old_flags & 0x0000FFFF));
}

int MovePrx(char * buffer, char * insertbefore, const char * prxname, u32 flags)
{
	RemovePrx(buffer, prxname, flags);

	return AddPRX(buffer, insertbefore, prxname, flags);
}

// Note flags is 32-bits!
void ModifyPrxFlag(char *buffer, const char* modname, u32 flags)
{
	int modnum;

	modnum = SearchPrx(buffer, modname);

	if (modnum < 0) {
		return modnum;
	}

	_btcnf_header * header = (_btcnf_header *)buffer;
	
	//cast module list
	_btcnf_module * module = (_btcnf_module *)(buffer + header->modulestart);

	module[modnum].flags = flags;
}

// Note flags is 32-bits!
int GetPrxFlag(char *buffer, const char* modname, u32 *flags)
{
	int modnum;

	modnum = SearchPrx(buffer, modname);

	if (modnum < 0) {
		return modnum;
	}

	_btcnf_header * header = (_btcnf_header *)buffer;
	
	//cast module list
	_btcnf_module * module = (_btcnf_module *)(buffer + header->modulestart);

	*flags = module[modnum].flags;

	return 0;
}

// Note: new_mod_name cannot have longer filename than mod_name 
int RenameModule(void *buffer, char *mod_name, char *new_mod_name)
{
	int modnum;

	modnum = SearchPrx(buffer, mod_name);

	if (modnum < 0) {
		return modnum;
	}

	_btcnf_header * header = (_btcnf_header *)buffer;
	
	//cast module list
	_btcnf_module * module = (_btcnf_module *)(buffer + header->modulestart);

	_strcpy((char*)(buffer + header->modnamestart + module[modnum].module_path), new_mod_name);

	return 0;
}

#if 0
void freezeme(unsigned int color)
{
	while(1)
	{
		unsigned int *p = (unsigned int*) 0x04000000;
		while (p < (unsigned int*) 0x04400000) *p++ = color;
	}
}
#endif
