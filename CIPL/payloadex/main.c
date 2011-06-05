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
 * 
 * 6.3X payloadex.bin
 *
 */

#include <pspsdk.h>
#include "main.h"

#include "rebootex_conf.h"
#include "utils.h"
#include "config.h"

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

int AddPRX(char * buffer, char * insertbefore, char * prxname, u32 flags);
int AddPRXNoCopyName(char * buffer, char * insertbefore, int prxname_offset, u32 flags);


int Main(void *, void *, void *, void *, void *, void *, void *);

int Reboot_Entry(void *a0, void *a1, void *a2, void *a3, void *t0, void *t1, void *t2) __attribute__ ((section (".text.start")));
int Reboot_Entry(void *a0, void *a1, void *a2, void *a3, void *t0, void *t1, void *t2)
{
	return Main(a0, a1, a2, a3, t0, t1, t2);
}

int elf_load_flag;
int btcnf_load_flag;
int recovery_flag;
int psp_model;
u32 hold_key;

int (* Real_Reboot)(void *a0, void *a1, void *a2, void *a3, void *t0, void *t1, void *t2) = (void *)0x88600000;

int (* sceKernelCheckExecFile)(void *buf, int *check) =NULL;
int (* memlmd_E42AFE2E)(void *buf ,void *check , void *s) = NULL;
int (* memlmd_3F2AC9C6)(void *a0,void *a1) = NULL;

#if PSP_MODEL == 0
//fat
#include "../btcnf/recovery_btcnf_01g.h"
//#define BTCNF_PATH "pspbtcnf.bin"
int (* DcacheClear)(void) = (void *)0x88601510;//6.38
int (* IcacheClear)(void) = (void *)0x88600DBC;//6.38
int (* sceBootLfatOpen)(const char *path) = (void *)0x88604A20;			//6.38
int (* sceBootLfatRead)(void *buff , int max_size) = (void *)0x88604B94;//6.38
int (* sceBootLfatClose)(void) = (void *)0x88604B38;					//6.38
int (* sceKernelCheckPspConfig)(void *a0 , int size , int flag) = (void *)0x8860A890;//6.38
#elif PSP_MODEL == 1
//slim
#include "../btcnf/recovery_btcnf_02g.h"
//#define BTCNF_PATH "pspbtcnf_02g.bin"
int (* DcacheClear)(void) = (void *)0x886015E0;//6.37
int (* IcacheClear)(void) = (void *)0x88600E8C;//6.37
int (* sceBootLfatOpen)(const char *path) = (void *)0x88604AF0;			//6.37
int (* sceBootLfatRead)(void *buff , int max_size) = (void *)0x88604C64;//6.37
int (* sceBootLfatClose)(void) = (void *)0x88604C08;					//6.37
int (* sceKernelCheckPspConfig)(void *a0 , int size , int flag) = (void *)0x8860A960;
#else
#error PSP_MODEL is not defined
#endif


void ClearCaches()
{
	DcacheClear();
	IcacheClear();
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

/*
int sceBootLfatOpenPatched(char *path)//open
{
	if( memcmp( path + 4 , BTCNF_PATH , sizeof( BTCNF_PATH )) == 0)
	{
		if(recovery_flag)
		{
			btcnf_load_flag = 1;
			return 0;
		}
		
		path[9] = 'j';
	}

	return sceBootLfatOpen( path );
}

int sceBootLfatReadPatched(void *buff , int max_size )//
{
	if( btcnf_load_flag )
	{
		memcpy( buff , recovery_btcnf , size_recovery_btcnf );
		return size_recovery_btcnf;
	}

	return sceBootLfatRead( buff , max_size );
}

int sceBootLfatClosePatched(void)//close
{
	if(btcnf_load_flag)
	{
		btcnf_load_flag = 0;
		return 0;
	}

	return sceBootLfatClose();
}
*/

int memlmd_E42AFE2E_patched(PSP_Header *buf, int *check,int *s)//decrypt patch
{
	if(buf->oe_tag == 0xC01DB15D )//0x55668D96
	{
		_memcpy(buf,&(buf->main_data), buf->comp_size);
		s[0] = buf->comp_size;
		return 0;
	}
	
	return memlmd_E42AFE2E(buf , check , s);	
}


int memlmd_3F2AC9C6_patched(void *a0,void *a1)//sig check
{
	PSP_Header *head=(PSP_Header *)a0;
	int i;

	for(i=0;i<0x38;i++)
	{
		if(head->scheck[i] != 0)
			return memlmd_3F2AC9C6(a0,a1);
	}

	return 0;
}

int PatchLoadCore(void *a0, void *a1, void *a2, int (* module_start)(void *, void *, void *))
{
	u32 text_addr = ((u32)module_start);

	_MAKE_CALL(text_addr + 0x00005CC8 - 0x00000BBC , memlmd_3F2AC9C6_patched );
	_MAKE_CALL(text_addr + 0x00005CF8 - 0x00000BBC , memlmd_3F2AC9C6_patched );
	_MAKE_CALL(text_addr + 0x00005D90 - 0x00000BBC , memlmd_3F2AC9C6_patched );

	_MAKE_CALL(text_addr + 0x000041A4 - 0x00000BBC , memlmd_E42AFE2E_patched );
	_MAKE_CALL(text_addr + 0x00005CA4 - 0x00000BBC , memlmd_E42AFE2E_patched );

	memlmd_3F2AC9C6 = (void *)(text_addr + 0x00007AE8 - 0x00000BBC);
	memlmd_E42AFE2E = (void *)(text_addr + 0x00007B08 - 0x00000BBC);

	ClearCaches();//

	return module_start(a0, a1, a2);
}

int sceKernelCheckPspConfigPatched(u8 *buffer , int size , int flag)
{
	int result = sceKernelCheckPspConfig( buffer , size , flag);
//	result = btcnf_edit((BtcnfHeader *)buffer , result , flag);
//	_btcnf_header *header =( _btcnf_header *)buffer;

	if( recovery_flag ){
		_memcpy( buffer , recovery_btcnf, size_recovery_btcnf);
		result = size_recovery_btcnf;
	}else{
		result = AddPRX((char *)buffer, "/kd/init.prx", PATH_SYSTEMCTRL+sizeof(PATH_FLASH0)-2, 0x000000EF);
		result = AddPRX((char *)buffer, "/kd/vshbridge.prx", PATH_VSHCTRL+sizeof(PATH_FLASH0)-2, VSH_RUNLEVEL );
	}

	ClearCaches();
	return result;
}

#define SYSCON_CTRL_LTRG      0x00000200
#define SYSCON_CTRL_RTRG      0x00000400
#define SYSCON_CTRL_HOME      0x00001000

int Main(void *a0, void *a1, void *a2, void *a3, void *t0, void *t1, void *t2)
{	
	hold_key = *(u32 *)0x88FB0000;

	_memset(0x88FB0000 ,0,0x200);
	
	rebootex_config *conf = (rebootex_config *)(REBOOTEX_CONFIG);
	conf->magic = REBOOTEX_CONFIG_MAGIC;
	conf->psp_model = PSP_MODEL;
	conf->rebootex_size = 0;
	conf->psp_fw_version = 0x06030910;

	elf_load_flag = 0;
	btcnf_load_flag = 0;
	recovery_flag = 0;

	if ( hold_key & SYSCON_CTRL_HOME)
	{
		hold_key = ~hold_key;

		if( SYSCON_CTRL_RTRG & hold_key)
			recovery_flag = 1;

#if PSP_MODEL == 0
//fat
//		_MAKE_CALL( 0x88603EB8 , sceBootLfatOpenPatched);//6.38
//		_MAKE_CALL( 0x88603F20 , sceBootLfatReadPatched);//6.38
//		_MAKE_CALL( 0x88603F40  , sceBootLfatClosePatched);//6.38
		_MAKE_CALL( 0x88603594 , sceKernelCheckPspConfigPatched);//6.38

		_sw( 0x03E00008 , 0x8860C80C);// -6.38
		_sw( 0x24020001 , 0x8860C810);//addiu  $v0, $zr, 1

		// Patch ~PSP header check // 6.38
		_sw(0xafa50000, 0x8860A8C4);//sw $a0, 0($sp) -> sw $a1, 0($sp)
		_sw(0x20a30000, 0x8860A8C8);//addiu v1, zr,-1 -> addi	$v1, $a1, 0x0000

		///patch sceBootLfatfsMount -6.38
		_sw(0, 0x88603EB0 );

		//patch sceBootLfatSeek size -6.38
		_sw(0, 0x88603F00);

		//patch buffer size check -6.38
		_sw(0, 0x88603F10);

		_sw( _MIPS_ADDU( 7 , 15 , 0 ) , 0x8860339C );////addu $a3,$t7,$zr MIPS_ADDU( 7 ,15 , 0 ) = 0x01E03821
		_MAKE_JUMP( 0x886033A0 , PatchLoadCore );//-6.38
		_sw( 0x0280E821 , 0x886033A4);//addu  $sp, $s4, $zr

		//patch hash error -6.38
		_sw(0, 0x88603954);

#elif PSP_MODEL == 1
//slim
//		_MAKE_CALL( 0x88603F88 , sceBootLfatOpenPatched);//6.37
//		_MAKE_CALL( 0x88603FF0 , sceBootLfatReadPatched);//6.37
//		_MAKE_CALL( 0x88604010 , sceBootLfatClosePatched);//6.37
		_MAKE_CALL( 0x88603680 , sceKernelCheckPspConfigPatched);//6.37

		_sw( 0x03E00008 , 0x8860C8DC);// -6.37
		_sw( 0x24020001 , 0x8860C8E0);//addiu  $v0, $zr, 1

		// Patch ~PSP header check // 6.37
		_sw(0xafa50000, 0x8860A994);//sw $a0, 0($sp) -> sw $a1, 0($sp)
		_sw(0x20a30000, 0x8860A998);//addiu v1, zr,-1 -> addi	$v1, $a1, 0x0000

		///patch sceBootLfatfsMount -6.37
		_sw(0, 0x88603F80 );

		//patch sceBootLfatSeek size -6.37
		_sw(0, 0x88603FD0);

		//patch buffer size check -6.37
		_sw(0, 0x88603FE0);

		_sw( _MIPS_ADDU( 7 , 15 , 0 ) , 0x8860346C);////addu $a3,$t7,$zr MIPS_ADDU( 7 ,15 , 0 ) = 0x01E03821
		_MAKE_JUMP( 0x88603470 , PatchLoadCore );//-6.37
		_sw( 0x0280E821 , 0x88603474);//addu  $sp, $s4, $zr

		//patch hash error -6.37
		_sw(0, 0x88603A24);
#endif

	}

	ClearCaches();

	return Real_Reboot(a0, a1, a2, a3, t0, t1, t2);	
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