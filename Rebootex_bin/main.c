#include <pspsdk.h>
#include "rebootex_conf.h"
#include "utils.h"

#define REBOOT_START 0x88600000
#define REBOOTEX_CONFIG_START 0x88FB0000
#define REBOOTEX_START 0x88FC0000
#define BTCNF_MAGIC 0x0F803001

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
int (* iCacheFlushAll)(void) = (void *)REBOOT_START + 0x0938;

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
int _UnpackBootConfig(char * buffer, int length);

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
	unsigned int patches[17];

	load_configure();

	//32mb psp
	if(version == 0)
	{
		patches[0] = 0x8624; //0x82AC in 6.20 - sceBootLfatOpen
		patches[1] = 0x8798; //0x8420 in 6.20 - sceBootLfatRead
		patches[2] = 0x873C; //0x83C4 in 6.20 - sceBootLfatClose
		patches[3] = 0x588C; //0x565C in 6.20 - UnpackBootConfig
		patches[4] = 0x2764; //0x26DC in 6.20 - Call to sceBootLfatOpen
		patches[5] = 0x27D4; //0x274C in 6.20 - Call to sceBootLfatRead
		patches[6] = 0x2800; //0x2778 in 6.20 - Call to sceBootLfatClose
		patches[7] = 0x7348; //0x70F0 in 6.20 - Call to UnpackBootConfig
		patches[8] = 0x389C; //0x3798 in 6.20 - Killing Function Part #1 - jr $ra
		patches[9] = 0x38A0; //0x379C in 6.20 - Killing Function Part #2 - li $v0, 1
		patches[10] = 0x275C; //0x26D4 in 6.20 - Killing Branch Check bltz ...
		patches[11] = 0x27B0; //0x2728 in 6.20 - Killing Branch Check bltz ...
		patches[12] = 0x27C8; //0x2740 in 6.20 - Killing Branch Check beqz ...
		patches[13] = 0x5760; //0x5550 in 6.20 - Prepare LoadCore Patch Part #1 - addu $a3, $zr, $s1 - Stores module_start ($s1) as fourth argument.
		patches[14] = 0x5764; //0x5554 in 6.20 - Prepare LoadCore Patch Part #2 - jal PatchLoadCore
		patches[15] = 0x5768; //0x5558 in 6.20 - Prepare LoadCore Patch Part #3 - move $sp, $s5 - Backed up instruction.
		patches[16] = 0x7648; //0x7388 in 6.20 - Killing Branch Check bltz ...
	}

	//64mb psps
	else
	{
		patches[0] = 0x86F0; //0x8374 in 6.20 - sceBootLfatOpen
		patches[1] = 0x8864; //0x84E8 in 6.20 - sceBootLfatRead
		patches[2] = 0x8808; //0x848C in 6.20 - sceBootLfatClose
		patches[3] = 0x595C; //0x5724 in 6.20 - UnpackBootConfig
		patches[4] = 0x2834; //0x27A4 in 6.20 - Call to sceBootLfatOpen
		patches[5] = 0x28A4; //0x2814 in 6.20 - Call to sceBootLfatRead
		patches[6] = 0x28D0; //0x2840 in 6.20 - Call to sceBootLfatClose
		patches[7] = 0x7438; //0x71B8 in 6.20 - Call to UnpackBootConfig
		patches[8] = 0x396C; //0x3860 in 6.20 - Killing Function Part #1 - jr $ra
		patches[9] = 0x3970; //0x3864 in 6.20 - Killing Function Part #2 - li $v0, 1
		patches[10] = 0x282C; //0x279C in 6.20 - Killing Branch Check bltz ...
		patches[11] = 0x2880; //0x27F0 in 6.20 - Killing Branch Check bltz ...
		patches[12] = 0x2898; //0x2808 in 6.20 - Killing Branch Check beqz ...
		patches[13] = 0x5830; //0x5618 in 6.20 - Prepare LoadCore Patch Part #1 - addu $a3, $zr, $s1 - Stores module_start ($s1) as fourth argument.
		patches[14] = 0x5834; //0x561C in 6.20 - Prepare LoadCore Patch Part #2 - jal PatchLoadCore
		patches[15] = 0x5838; //0x5620 in 6.20 - Prepare LoadCore Patch Part #3 - move $sp, $s5 - Backed up instruction.
		patches[16] = 0x7714; //0x7450 in 6.20 - Killing Branch Check bltz ...
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
	_sw((((int)(_sceKernelCheckExecFile) >> 2) & 0x03FFFFFF) | 0x0C000000, (unsigned int)(module_bootstart) + 0x35E8);
	_sw((((int)(_sceKernelCheckExecFile) >> 2) & 0x03FFFFFF) | 0x0C000000, (unsigned int)(module_bootstart) + 0x50E8);
	_sw((((int)(_memlmd_3F2AC9C6) >> 2) & 0x03FFFFFF) | 0x0C000000, (unsigned int)(module_bootstart) + 0x510C);
	_sw((((int)(_memlmd_3F2AC9C6) >> 2) & 0x03FFFFFF) | 0x0C000000, (unsigned int)(module_bootstart) + 0x513C);
	_sw((((int)(_memlmd_3F2AC9C6) >> 2) & 0x03FFFFFF) | 0x0C000000, (unsigned int)(module_bootstart) + 0x51D4);

	//save loadcore function pointer (module_bootstart = 0xBBC)
	sceKernelCheckExecFile = (void *)((unsigned int)(module_bootstart) + 0x6F4C);
	memlmd_3F2AC9C6 = (void *)((unsigned int)(module_bootstart) + 0x6F2C);

	//flush instruction cache
	iCacheFlushAll();

	//call module_start
	return module_bootstart(arg1, arg2, arg3);
}

int patch_bootconf_vsh(char *buffer, int length)
{
	int newsize, result;

	result = length;
	newsize = AddPRX(buffer, "/kd/usersystemlib.prx", "/kd/vshctrl.prx", VSH_RUNLEVEL );

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
	{ "/kd/galaxy.prx", "/kd/np9660.prx", UMDEMU_RUNLEVEL },
	{ "/kd/galaxy.prx", "/kd/utility.prx", GAME_RUNLEVEL },
	{ "/kd/np9660.prx", "/kd/utility.prx", GAME_RUNLEVEL },
	{ "/kd/isofs.prx", "/kd/utility.prx", GAME_RUNLEVEL },
	{ "/kd/stargate.prx", "/kd/me_wrapper.prx", GAME_RUNLEVEL | UMDEMU_RUNLEVEL },
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
	{ "/kd/march33.prx", "/kd/utility.prx", GAME_RUNLEVEL },
	{ "/kd/march33.prx", "/kd/isofs.prx", UMDEMU_RUNLEVEL },
	{ "/kd/isofs.prx", "/kd/utility.prx", GAME_RUNLEVEL },
	{ "/kd/stargate.prx", "/kd/me_wrapper.prx", GAME_RUNLEVEL | UMDEMU_RUNLEVEL },
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

int _UnpackBootConfig(char * buffer, int length)
{
	int result;
	int newsize;

	result = (*UnpackBootConfig)(buffer, length);

	newsize = AddPRX(buffer, "/kd/init.prx", "/kd/systemctrl.prx", 0x00EF);

	if (newsize > 0) result = newsize;

	if (SearchPrx(buffer, "/vsh/module/vshmain.prx") >= 0) {
		newsize = patch_bootconf_vsh(buffer, length);

		if (newsize > 0) result = newsize;
	}
	
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
	newmod.flags = 0x80010000 | (flags & 0xFFFF);
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

	old_flags &= 0xFFFF;
	flags &= 0xFFFF;

	if (old_flags & flags) {
		// rewrite the flags to remove the modules from runlevels indicated by flags
		old_flags = old_flags & (~flags);
	}

	ModifyPrxFlag(buffer, prxname, 0x80010000 | (old_flags & 0xFFFF));
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
