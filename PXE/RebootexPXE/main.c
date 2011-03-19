#include <pspsdk.h>
#include "utils.h"
#include "systemctrl.h"
#include "../../Rebootex_bin/rebootex_bin_patch_offset.h"

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
int systemcontrol_open = 0;

//io functions
int (* sceBootLfatOpen)(char * filename) = NULL;
int (* sceBootLfatRead)(char * buffer, int length) = NULL;
int (* sceBootLfatClose)(void) = NULL;
int (* UnpackBootConfig)(char * buffer, int length) = NULL;

//loadcore functions
int (* sceKernelCheckExecFile)(char * prx, unsigned int size, unsigned int * newsize) = 0;
int (* memlmd_3F2AC9C6)(unsigned char * addr, void * arg2) = NULL;

//cache sync
static inline int iCacheFlushAll(void)
{
	int (*_iCacheFlushAll)(void) = (void *)(REBOOT_START + g_offs->iCacheFlushAll);
	
	return (*_iCacheFlushAll)();
}

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
int AddPRX(char * buffer, char * insertafter, char * prxname, u32 flags);
void RenameModule(void *buffer, char *mod_name, char *new_mod_name);

//reboot function
void (* reboot)(int arg1, int arg2, int arg3, int arg4) = (void *)REBOOT_START;

//reboot replacement
void main(int arg1, int arg2, int arg3, int arg4)
{
	//grab psp version
	int version = *(int *)REBOOTEX_CONFIG_START;
	struct RebootexPatch *patch;

	setup_patch_offset_table(*(int*)(REBOOTEX_CONFIG_START+0x00000008));

	//32mb psp
	if(version == 0)
	{
		patch = &g_offs->rebootex_patch_01g;
	}

	//64mb psps
	else
	{
		patch = &g_offs->rebootex_patch_other;
	}

	//global offsets
	sceBootLfatOpen = (void *)REBOOT_START + patch->sceBootLfatOpen;
	sceBootLfatRead = (void *)REBOOT_START + patch->sceBootLfatRead;
	sceBootLfatClose = (void *)REBOOT_START + patch->sceBootLfatClose;
	UnpackBootConfig = (void *)REBOOT_START + patch->UnpackBootConfig;

	//0x000026DC in 6.20 - Call to sceBootLfatOpen
	_sw(MAKE_CALL(_sceBootLfatOpen), REBOOT_START + patch->sceBootLfatOpenCall);
	//0x0000274C in 6.20 - Call to sceBootLfatRead
	_sw(MAKE_CALL(_sceBootLfatRead), REBOOT_START + patch->sceBootLfatReadCall);
	//0x00002778 in 6.20 - Call to sceBootLfatClose
	_sw(MAKE_CALL(_sceBootLfatClose), REBOOT_START + patch->sceBootLfatCloseCall);
	//0x000070F0 in 6.20 - Call to UnpackBootConfig
	_sw(MAKE_CALL(_UnpackBootConfig), REBOOT_START + patch->UnpackBootConfigCall);
	//0x00003798 in 6.20 - Killing Function Part #1 - jr $ra
	_sw(0x03E00008, REBOOT_START + patch->RebootexCheck1);
	//0x0000379C in 6.20 - Killing Function Part #2 - li $v0, 1
	_sw(0x24020001, REBOOT_START + patch->RebootexCheck1 + 4);
	//0x000026D4 in 6.20 - Killing Branch Check bltz ...
	_sw(NOP, REBOOT_START + patch->RebootexCheck2);
	//0x00002728 in 6.20 - Killing Branch Check bltz ...
	_sw(NOP, REBOOT_START + patch->RebootexCheck3);
	//0x00002740 in 6.20 - Killing Branch Check beqz ...
	_sw(NOP, REBOOT_START + patch->RebootexCheck4);
	//0x00007388 in 6.20 - Killing Branch Check bltz ...
	_sw(NOP, REBOOT_START + patch->RebootexCheck5);
	//0x00005550 in 6.20 - Prepare LoadCore Patch Part #1 - addu $a3, $zr, $s1 - Stores module_start ($s1) as fourth argument.
	_sw(0x00113821, REBOOT_START + patch->LoadCoreModuleStartCall - 4);
	//0x00005554 in 6.20 - Prepare LoadCore Patch Part #2 - jal PatchLoadCore
	_sw(MAKE_JUMP(PatchLoadCore), REBOOT_START + patch->LoadCoreModuleStartCall);
	//0x00005558 in 6.20 - Prepare LoadCore Patch Part #3 - move $sp, $s5 - Backed up instruction.
	_sw(0x02A0E821, REBOOT_START + patch->LoadCoreModuleStartCall + 4);
	
	//initializing global variables
	systemcontrol_open = 0;

	//flush instruction cache
	iCacheFlushAll();

	//reboot psp
	reboot(arg1, arg2, arg3, arg4);
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

static char *p_sctrl;
static int size_sctrl;

int _sceBootLfatRead(char * buffer, int length)
{
	//systemcontrol load
	if(systemcontrol_open)
	{
		int min;

		//copy systemcontrol
		min = size_sctrl < length ? size_sctrl : length;
		_memcpy(buffer, p_sctrl, min);
		p_sctrl += min;
		size_sctrl -= min;

		//set filesize
		return min;
	}

	//forward to original function
	return sceBootLfatRead(buffer, length);
}

int _sceBootLfatOpen(char * filename)
{
	//systemcontrol open
	if(_strcmp(filename, "/hen.prx") == 0) {
		//mark for read
		systemcontrol_open = 1;
		p_sctrl = systemcontrol;
		size_sctrl = size_systemcontrol;

		//return success
		return 0;
	}

	//forward to original function
	return sceBootLfatOpen(filename);
}

int _sceBootLfatClose(void)
{
	//systemcontrol close
	if(systemcontrol_open)
	{
		//mark as closed
		systemcontrol_open = 0;

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
	_sw((((int)(_sceKernelCheckExecFile) >> 2) & 0x03FFFFFF) | 0x0C000000, (unsigned int)(module_bootstart) + g_offs->loadcore_patch.sceKernelCheckExecFileCall1);
	_sw((((int)(_sceKernelCheckExecFile) >> 2) & 0x03FFFFFF) | 0x0C000000, (unsigned int)(module_bootstart) + g_offs->loadcore_patch.sceKernelCheckExecFileCall2);
	_sw((((int)(_memlmd_3F2AC9C6) >> 2) & 0x03FFFFFF) | 0x0C000000, (unsigned int)(module_bootstart) + g_offs->loadcore_patch.memlmd_3F2AC9C6_call1);
	_sw((((int)(_memlmd_3F2AC9C6) >> 2) & 0x03FFFFFF) | 0x0C000000, (unsigned int)(module_bootstart) + g_offs->loadcore_patch.memlmd_3F2AC9C6_call2);
	_sw((((int)(_memlmd_3F2AC9C6) >> 2) & 0x03FFFFFF) | 0x0C000000, (unsigned int)(module_bootstart) + g_offs->loadcore_patch.memlmd_3F2AC9C6_call3);

	sceKernelCheckExecFile = (void *)((unsigned int)(module_bootstart) + g_offs->loadcore_patch.sceKernelCheckExecFile);
	memlmd_3F2AC9C6 = (void *)((unsigned int)(module_bootstart) + g_offs->loadcore_patch.memlmd_3F2AC9C6);

	//flush instruction cache
	iCacheFlushAll();

	//call module_start
	return module_bootstart(arg1, arg2, arg3);
}

int _UnpackBootConfig(char * buffer, int length)
{
	//unpack boot config from flash0
	int result = UnpackBootConfig(buffer, length);

	//add our own prx entry
	int addsize = AddPRX(buffer, "/kd/init.prx", "/hen.prx", 0xEF);

	//add size modification
	if(addsize > 0) result += addsize;

	return result;
}

int AddPRX(char * buffer, char * insertafter, char * prxname, u32 flags)
{
	//cast header
	_btcnf_header * header = (_btcnf_header *)buffer;

	//valid boot config
	if(header->signature == BTCNF_MAGIC)
	{
		//valid number of modules
		if(header->nmodules > 0)
		{
			//valid number of modes
			if(header->nmodes > 0)
			{
				//add module name
				header->modnameend += _strcpy(buffer + header->modnameend, prxname);

				//cast module list
				_btcnf_module * module = (_btcnf_module *)(buffer + header->modulestart);

				//iterate modules
				int modnum = 0; for(; modnum < header->nmodules; modnum++)
				{
					//found module name
					if(_strcmp(buffer + header->modnamestart + module[modnum].module_path, insertafter) == 0)
					{
						//stop search
						break;
					}
				}

				//found module
				if(modnum < header->nmodules)
				{
					//add custom module
					_btcnf_module newmod; _memset(&newmod, 0, sizeof(newmod));
					newmod.module_path = header->modnameend - _strlen(prxname) - 1 - header->modnamestart;
					newmod.flags = 0x80010000 | (flags & 0xFFFF);
					_memmove(&module[modnum + 2], &module[modnum + 1], buffer + header->modnameend - (unsigned int)&module[modnum + 1]);
					_memcpy(&module[modnum + 1], &newmod, sizeof(newmod));
					header->nmodules++;
					header->modnamestart += sizeof(newmod);
					header->modnameend += sizeof(newmod);

					//make mode include our module
					int modenum = 0; for(; modenum < header->nmodes; modenum++)
					{
						//increase module range
						*(unsigned short *)(buffer + header->modestart + modenum * 32) += 1;
					}

					//return size modificator
					return _strlen(prxname) + 1 + sizeof(_btcnf_module);
				}

				//module not found
				return -4;
			}

			//invalid number of modes
			return -3;
		}

		//invalid number of modules
		return -2;
	}

	//invalid magic value
	return -1;
}

// Note: new_mod_name cannot have longer filename than mod_name 
void RenameModule(void *buffer, char *mod_name, char *new_mod_name)
{
	_btcnf_module *pmod;
	int i;
	char *modules_start, *names_start;
	_btcnf_header *hdr = buffer;

	modules_start = (char *) (buffer + hdr->modulestart);
	names_start = (char *) (buffer + hdr->modnamestart);
	pmod = (_btcnf_header *) modules_start;

	if (hdr->nmodules < 0)
		return;

	for (i = 0; i < hdr->nmodules; i++) {
		if (0 == _strcmp(names_start + pmod->module_path, mod_name))
			break;
		pmod++;
	}

	if (i == hdr->nmodules)
		return;

	_strcpy(names_start + pmod->module_path, new_mod_name);
}
