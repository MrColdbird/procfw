#include <pspsdk.h>
#include "rebootex_conf.h"
#include "utils.h"
#include "config.h"

#define REBOOT_START 0x88600000
#define REBOOTEX_CONFIG_START 0x88FB0000
#define REBOOTEX_START 0x88FC0000
#define BTCNF_MAGIC 0x0F803001
#define BOOTCONFIG_TEMP_BUFFER 0x88FB0200

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

//io flags
int rebootmodule_open = 0;

//rtm data
char * loadrebootmodulebefore = NULL;
char * rebootmodule = NULL;
int size_rebootmodule = 0;
int rebootmoduleflags = 0;

PspBootConfMode iso_mode = 0;

//io functions
int (* sceBootLfatOpen)(char * filename) = NULL;
int (* sceBootLfatRead)(char * buffer, int length) = NULL;
int (* sceBootLfatClose)(void) = NULL;
int (* UnpackBootConfig)(char * buffer, int length) = NULL;

//loadcore functions
int (* sceKernelCheckExecFile)(char * prx, unsigned int size, unsigned int * newsize) = 0;
int (* memlmd_3F2AC9C6)(unsigned char * addr, void * arg2) = NULL;

//cache sync
int (* iCacheFlushAll)(void) = (void *)REBOOT_START + 0x00000938;

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
int _sceKernelCheckExecFile(char * prx, unsigned int size, unsigned int * newsize);
int _memlmd_3F2AC9C6(unsigned char * addr, void * arg2);

//patch functions
int PatchLoadCore(void * arg1, void * arg2, void * arg3, int (* module_bootstart)(void *, void *, void *));

//screen debugger
//void freezeme(unsigned int color);

//reboot function
void (* reboot)(int arg1, int arg2, int arg3, int arg4) = (void *)REBOOT_START;

void load_configure(void);

int AddPRX(char * buffer, char * insertbefore, char * prxname, u32 flags);
int AddPRXNoCopyName(char * buffer, char * insertbefore, int prxname_offset, u32 flags);
void RemovePrx(char *buffer, const char *prxname, u32 flags);
void ModifyPrxFlag(char *buffer, const char* modname, u32 flags);
int MovePrx(char * buffer, char * insertbefore, const char * prxname, u32 flags);
int GetPrxFlag(char *buffer, const char* modname, u32 *flag);

//reboot replacement
void main(int arg1, int arg2, int arg3, int arg4)
{
	//grab psp version
	int version = *(int *)REBOOTEX_CONFIG_START;

	//patch offsets
	unsigned int patches[18];

	load_configure();

	//32mb psp
	if(version == 0)
	{
		patches[0] = 0x00008624; //0x000082AC in 6.20 - sceBootLfatOpen
		patches[1] = 0x00008798; //0x00008420 in 6.20 - sceBootLfatRead
		patches[2] = 0x0000873C; //0x000083C4 in 6.20 - sceBootLfatClose
		patches[3] = 0x0000588C; //0x0000565C in 6.20 - UnpackBootConfig
		patches[4] = 0x00002764; //0x000026DC in 6.20 - Call to sceBootLfatOpen
		patches[5] = 0x000027D4; //0x0000274C in 6.20 - Call to sceBootLfatRead
		patches[6] = 0x00002800; //0x00002778 in 6.20 - Call to sceBootLfatClose
		patches[7] = 0x00007348; //0x000070F0 in 6.20 - Call to UnpackBootConfig
		patches[8] = 0x0000389C; //0x00003798 in 6.20 - Killing Function Part #1 - jr $ra
		patches[9] = 0x000038A0; //0x0000379C in 6.20 - Killing Function Part #2 - li $v0, 1
		patches[10] = 0x0000275C; //0x000026D4 in 6.20 - Killing Branch Check bltz ...
		patches[11] = 0x000027B0; //0x00002728 in 6.20 - Killing Branch Check bltz ...
		patches[12] = 0x000027C8; //0x00002740 in 6.20 - Killing Branch Check beqz ...
		patches[13] = 0x00005760; //0x00005550 in 6.20 - Prepare LoadCore Patch Part #1 - addu $a3, $zr, $s1 - Stores module_start ($s1) as fourth argument.
		patches[14] = 0x00005764; //0x00005554 in 6.20 - Prepare LoadCore Patch Part #2 - jal PatchLoadCore
		patches[15] = 0x00005768; //0x00005558 in 6.20 - Prepare LoadCore Patch Part #3 - move $sp, $s5 - Backed up instruction.
		patches[16] = 0x00007648; //0x00007388 in 6.20 - Killing Branch Check bltz ...
		patches[17] = 0x00007308; //UnpackBootConfig buffer address
	}

	//64mb psps
	else
	{
		patches[0] = 0x000086F0; //0x00008374 in 6.20 - sceBootLfatOpen
		patches[1] = 0x00008864; //0x000084E8 in 6.20 - sceBootLfatRead
		patches[2] = 0x00008808; //0x0000848C in 6.20 - sceBootLfatClose
		patches[3] = 0x0000595C; //0x00005724 in 6.20 - UnpackBootConfig
		patches[4] = 0x00002834; //0x000027A4 in 6.20 - Call to sceBootLfatOpen
		patches[5] = 0x000028A4; //0x00002814 in 6.20 - Call to sceBootLfatRead
		patches[6] = 0x000028D0; //0x00002840 in 6.20 - Call to sceBootLfatClose
		patches[7] = 0x00007438; //0x000071B8 in 6.20 - Call to UnpackBootConfig
		patches[8] = 0x0000396C; //0x00003860 in 6.20 - Killing Function Part #1 - jr $ra
		patches[9] = 0x00003970; //0x00003864 in 6.20 - Killing Function Part #2 - li $v0, 1
		patches[10] = 0x0000282C; //0x0000279C in 6.20 - Killing Branch Check bltz ...
		patches[11] = 0x00002880; //0x000027F0 in 6.20 - Killing Branch Check bltz ...
		patches[12] = 0x00002898; //0x00002808 in 6.20 - Killing Branch Check beqz ...
		patches[13] = 0x00005830; //0x00005618 in 6.20 - Prepare LoadCore Patch Part #1 - addu $a3, $zr, $s1 - Stores module_start ($s1) as fourth argument.
		patches[14] = 0x00005834; //0x0000561C in 6.20 - Prepare LoadCore Patch Part #2 - jal PatchLoadCore
		patches[15] = 0x00005838; //0x00005620 in 6.20 - Prepare LoadCore Patch Part #3 - move $sp, $s5 - Backed up instruction.
		patches[16] = 0x00007714; //0x00007450 in 6.20 - Killing Branch Check bltz ...
		patches[17] = 0x000073F8; //UnpackBootConfig buffer address
	}

	//global offsets
	sceBootLfatOpen = (void *)REBOOT_START + patches[0];
	sceBootLfatRead = (void *)REBOOT_START + patches[1];
	sceBootLfatClose = (void *)REBOOT_START + patches[2];
	UnpackBootConfig = (void *)REBOOT_START + patches[3];

	//reboot patches
	_sw(MAKE_CALL(_sceBootLfatOpen), REBOOT_START + patches[4]);
	_sw(MAKE_CALL(_sceBootLfatRead), REBOOT_START + patches[5]);
	_sw(MAKE_CALL(_sceBootLfatClose), REBOOT_START + patches[6]);
	_sw(MAKE_CALL(_UnpackBootConfig), REBOOT_START + patches[7]);
	_sw(0x03E00008, REBOOT_START + patches[8]); // jr $ra
	_sw(0x24020001, REBOOT_START + patches[9]); // li $v0, 1
	_sw(0, REBOOT_START + patches[10]);
	_sw(0, REBOOT_START + patches[11]);
	_sw(0, REBOOT_START + patches[12]);
	_sw(0x00113821, REBOOT_START + patches[13]); // move $a3, $s1
	_sw(MAKE_JUMP(PatchLoadCore), REBOOT_START + patches[14]);
	_sw(0x02A0E821, REBOOT_START + patches[15]); // move $sp, $s5
	_sw(0, REBOOT_START + patches[16]);
	_sw(0x27A40004, REBOOT_START + patches[17]); // addiu $a0, $sp, 4

	load_configure();

	//initializing global variables
	rebootmodule_open = 0;

	//flush instruction cache
	iCacheFlushAll();

	//reboot psp
	reboot(arg1, arg2, arg3, arg4);
}

void load_configure(void)
{
	loadrebootmodulebefore = *(int *)(REBOOTEX_CONFIG_START + 0x10);
	rebootmodule = (char *)(*(int *)(REBOOTEX_CONFIG_START + 0x14));
	size_rebootmodule = *(int *)(REBOOTEX_CONFIG_START + 0x18);
	rebootmoduleflags = *(int *)(REBOOTEX_CONFIG_START + 0x1C);

	rebootex_config *conf = (rebootex_config *)(REBOOTEX_CONFIG_START + 0x20);
	
	if(conf->magic == REBOOTEX_CONFIG_MAGIC) {
		iso_mode = conf->iso_mode;
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

// memlmd_E42AFE2E
int _sceKernelCheckExecFile(char * prx, unsigned int size, unsigned int * newsize)
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
	return sceKernelCheckExecFile(prx, size, newsize);
}

int _memlmd_3F2AC9C6(unsigned char * addr, void * arg2)
{
	//scan structure
	//6.31 kernel modules use type 3 PRX... 0xd4~0x10C is zero padded
	int pos = 0; for(; pos < 0x38; pos++)
	{
		//nonzero byte encountered
		if(addr[pos + 212])
		{
			//forward to unsign function?
			return memlmd_3F2AC9C6(addr, arg2);
		}
	}

	//return success
	return 0;
}

int PatchLoadCore(void * arg1, void * arg2, void * arg3, int (* module_bootstart)(void *, void *, void *))
{
	//replace all occurrences
	_sw((((int)(_sceKernelCheckExecFile) >> 2) & 0x03FFFFFF) | 0x0C000000, (unsigned int)(module_bootstart) + 0x000035E8);
	_sw((((int)(_sceKernelCheckExecFile) >> 2) & 0x03FFFFFF) | 0x0C000000, (unsigned int)(module_bootstart) + 0x000050E8);
	_sw((((int)(_memlmd_3F2AC9C6) >> 2) & 0x03FFFFFF) | 0x0C000000, (unsigned int)(module_bootstart) + 0x0000510C);
	_sw((((int)(_memlmd_3F2AC9C6) >> 2) & 0x03FFFFFF) | 0x0C000000, (unsigned int)(module_bootstart) + 0x0000513C);
	_sw((((int)(_memlmd_3F2AC9C6) >> 2) & 0x03FFFFFF) | 0x0C000000, (unsigned int)(module_bootstart) + 0x000051D4);

	//save loadcore function pointer (module_bootstart = 0xBBC)
	sceKernelCheckExecFile = (void *)((unsigned int)(module_bootstart) + 0x00006F4C);
	memlmd_3F2AC9C6 = (void *)((unsigned int)(module_bootstart) + 0x00006F2C);

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

int _UnpackBootConfig(char **p_buffer, int length)
{
	int result;
	int newsize;
	char *buffer;

	result = (*UnpackBootConfig)(*p_buffer, length);
	buffer = (void*)BOOTCONFIG_TEMP_BUFFER;
	_memcpy(buffer, *p_buffer, length);
	*p_buffer = buffer;
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
