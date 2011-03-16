#include <pspsdk.h>
#include "utils.h"
#include "systemctrl.h"

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
int (* iCacheFlushAll)(void) = (void *)REBOOT_START + 0x000000938;

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

//reboot function
void (* reboot)(int arg1, int arg2, int arg3, int arg4) = (void *)REBOOT_START;

//reboot replacement
void main(int arg1, int arg2, int arg3, int arg4)
{
	//grab psp version
	int version = *(int *)REBOOTEX_CONFIG_START;

	//patch offsets
	unsigned int patches[17];

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

