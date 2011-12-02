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
 * 6.XX payloadex.bin
 *
 */

#include <pspsdk.h>
#include "main.h"
#include "payloadex_patch_addr.h"

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

int recovery_flag;
u32 hold_key;

int (* Real_Reboot)(void *a0, void *a1, void *a2, void *a3, void *t0, void *t1, void *t2) = (void *)0x88600000;
int (* memlmd_E42AFE2E)(void *buf ,void *check , void *s) = NULL;
int (* memlmd_3F2AC9C6)(void *a0,void *a1) = NULL;


#if PSP_MODEL == 0//fat
#include "../btcnf/recovery_btcnf_01g.h"
#elif PSP_MODEL == 1//slim
#include "../btcnf/recovery_btcnf_02g.h"
#else
#error PSP_MODEL is not defined
#endif

#if _PSP_FW_VERSION == 639
#define DEVKIT_VER	0x06030910
#elif _PSP_FW_VERSION == 660
#define DEVKIT_VER	0x06060010
#endif

void ClearCaches()
{
	int (* DcacheClear)(void) = (void *)payloadex_patch_list.function_list.DcacheClearAddr;
	int (* IcacheClear)(void) = (void *)payloadex_patch_list.function_list.IcacheClearAddr;

	DcacheClear();
	IcacheClear();
}

static int sceKernelCheckPspConfig(void *buffer , int size , int flag)
{
	int (* sceKernelCheckPspConfig_k)(void *, int, int ) = (void *)payloadex_patch_list.function_list.CheckPspConfig;
	return sceKernelCheckPspConfig_k( buffer, size, flag );
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

//memlmd_E42AFE2E_patched
int memlmd_Decrypt_patched(PSP_Header *buf, int *check,int *s)
{
	if(buf->oe_tag == 0xC01DB15D )//0x55668D96
	{
		_memcpy(buf,&(buf->main_data), buf->comp_size);
		*s = buf->comp_size;
		return 0;
	}
	
	return memlmd_E42AFE2E(buf , check , s);	
}

//memlmd_3F2AC9C6_patched
int memlmd_Sigcheck_patched(void *a0,void *a1)
{
	PSP_Header *head=(PSP_Header *)a0;
	int i;

#if _PSP_FW_VERSION == 639
	for(i=0;i<0x38;i++)
#else
	for(i=0;i<0x30;i++)
#endif
	{
		if(head->scheck[i] != 0)
			return memlmd_3F2AC9C6(a0,a1);
	}

	return 0;
}

int PatchLoadCore(void *a0, void *a1, void *a2, int (* module_start)(void *, void *, void *))
{
	u32 text_addr = ((u32)module_start) - payloadex_patch_list.memlmd_list.ModuleOffsetAddr;

	_MAKE_CALL(text_addr + payloadex_patch_list.memlmd_list.SigcheckPatchAddr , memlmd_Sigcheck_patched );
	_MAKE_CALL(text_addr + payloadex_patch_list.memlmd_list.DecryptPatchAddr , memlmd_Decrypt_patched );

	memlmd_3F2AC9C6 = (void *)(text_addr + payloadex_patch_list.memlmd_list.SigcheckFuncAddr );
	memlmd_E42AFE2E = (void *)(text_addr + payloadex_patch_list.memlmd_list.DecryptFuncAddr );

	ClearCaches();
	return module_start(a0, a1, a2);
}

int sceKernelCheckPspConfigPatched(u8 *buffer , int size , int flag)
{
	int result = sceKernelCheckPspConfig( buffer , size , flag);

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
	conf->psp_fw_version = DEVKIT_VER;

	recovery_flag = 0;

	if ( hold_key & SYSCON_CTRL_HOME)
	{
		hold_key = ~hold_key;

		if( SYSCON_CTRL_RTRG & hold_key)
			recovery_flag = 1;

		_MAKE_CALL( payloadex_patch_list.patch_list.CheckPspConfigPatch	, sceKernelCheckPspConfigPatched);

		_sw( 0x03E00008 , payloadex_patch_list.patch_list.KdebugPatchAddr );
		_sw( 0x24020001 , payloadex_patch_list.patch_list.KdebugPatchAddr + 4);//addiu  $v0, $zr, 1

		// Patch ~PSP header check
		_sw(0xafa50000, payloadex_patch_list.patch_list.BtHeaderPatchAddr );//sw $a0, 0($sp) -> sw $a1, 0($sp)
		_sw(0x20a30000, payloadex_patch_list.patch_list.BtHeaderPatchAddr + 4 );//addiu v1, zr,-1 -> addi	$v1, $a1, 0x0000

		///patch sceBootLfatfsMount
		_sw(0, payloadex_patch_list.patch_list.LfatMountPatchAddr );

		//patch sceBootLfatSeek size
		_sw(0, payloadex_patch_list.patch_list.LfatSeekPatchAddr1 );
		_sw(0, payloadex_patch_list.patch_list.LfatSeekPatchAddr2 );

		//MIPS_ADDU( 7 , 15 , 0 )
		_sw( 0x01E03821 ,	payloadex_patch_list.patch_list.LoadCorePatchAddr );//addu $a3,$t7,$zr
		_MAKE_JUMP(			payloadex_patch_list.patch_list.LoadCorePatchAddr + 4 , PatchLoadCore );
		_sw( 0x0280E821 ,	payloadex_patch_list.patch_list.LoadCorePatchAddr + 8 );//addu  $sp, $s4, $zr

		//patch hash error
		_sw(0, payloadex_patch_list.patch_list.HashCheckPatchAddr );
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