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
#include "main.h"
#include "utils.h"
#include "systemctrl.h"
#include "printk.h"
#include "elf.h"
#include "nid_resolver.h"
#include "libs.h"
#include "systemctrl_patch_offset.h"

//original functions
int (* ProbeExec1)(unsigned char * buffer, int * check) = NULL;
int (* ProbeExec2)(unsigned char * buffer, int * check) = NULL;
int (* ProbeExec3)(unsigned char * buffer, int * check) = NULL; //GO ONLY
int (* PartitionCheck)(unsigned int * st0, unsigned int * check) = NULL;

int PatchExec1(unsigned char * buffer, int * check)
{
	//grab magic
	unsigned int magic = *(unsigned int *)(buffer);

	//invalid magic
	if(magic != 0x464C457F) return -1;

	//possibly invalid apitype
	if(check[2] < 0x120) {
		//custom apitype
		if(check[2] < 0x52) {
			if(check[17] != 0) {
				check[18] = 1;
				return 0;
			}

			else return -2;
		}

		//invalid apitype
		else return -1;
	}
	//predefined apitype
	else if(check[2] == 0x120 || (check[2] >= 0x140 && check[2] <= 0x143)) {
		if(check[4] != 0) {
			check[17] = 1;
			check[18] = 1;

			//PatchExec2 (sub_00094)
			PatchExec2(buffer, check);

			return 0;
		} else if(check[17] != 0) {
			check[18] = 1;
			return 0;
		} else {
		   	return -1;
		}
	} 
	//invalid apitype
	else {
		return -1;
	}
}

int PatchExec2(unsigned char * buffer, int * check)
{
	int result = 0;
	int index = (check[19] < 0) ? (check[19] + 3) : (check[19]);

	//exclude volatile memory range from patching
	unsigned int address = (unsigned int)(buffer + index);
	if(!(address >= 0x88400000 && address <= 0x88800000)) {
		check[22] = *(unsigned short *)(buffer + index);
		result = *(int *)(buffer + index);
	}

	return result;
}

int PatchExec3(unsigned char * buffer, int * check, int isplain, int checkresult)
{
	//elf executable
	if(isplain) {
		//custom apitype
		if(check[2] < 0x52) {
			//PatchExec2 (sub_00094)
			if(PatchExec2(buffer, check) & 0xFF00) {
				check[17] = 1;
				checkresult = 0;
			}
		}
		//predefined apitype
		else {
			//static elf
			if(IsStaticElf(buffer)) {
				check[8] = 3;
			}
		}
	}

	//return result
	return checkresult;
}

int _sceKernelCheckExecFile(unsigned char * buffer, int * check)
{
	//PatchExec1 (sub_000EC)
	int result = PatchExec1(buffer, check);

	//PatchExec1 isn't enough... :(
	if(result != 0) {
		int checkresult = sctrlKernelCheckExecFile(buffer, check);

		//grab executable magic
		unsigned int magic = *(unsigned int *)(buffer);

		//PatchExec3 (sub_003C0)
		result = PatchExec3(buffer, check, magic == 0x464C457F, checkresult);
	}

	//return result
	return result;
}

int _ProbeExec1(unsigned char * buffer, int * check)
{
	//check executable (we have shifted attributes to not get detected here!)
	int result = ProbeExec1(buffer, check);

	//grab executable magic
	unsigned int magic = *(unsigned int *)(buffer);

	//plain elf executable
	if(magic == 0x464C457F) {
		//recover real attributes
		unsigned short realattr = *(unsigned short *)(buffer + check[19]);

		unsigned short attr = realattr & 0x1E00;

		if(attr != 0) {
			unsigned short attr2 = *(u16*)((void*)(check)+0x58);

			if((attr2 & 0x1E00) != attr)
				*(u16*)((void*)(check)+0x58) = realattr;
		}

		if(check[18] == 0) {
			check[18] = 1;
		}
	}

	//return result
	return result;
}

int _ProbeExec2(unsigned char * buffer, int * check)
{
	//check executable
	int result = ProbeExec2(buffer, check);

	//grab executable magic
	unsigned int magic = *(unsigned int *)(buffer);

	//plain static elf executable
	if(magic == 0x464C457F && IsStaticElf(buffer)) {
		//fake umd apitype to avoid static elf trouble
		check[2] = 0x120;

		//invalid string section offset
		if(check[19] == 0) {
			//get string section table
			char * strtab = GetStrTab(buffer);

			//found it
			if(strtab) {
				Elf32_Ehdr * header = (Elf32_Ehdr *)buffer;

				unsigned char * pData = buffer + header->e_shoff;

				int i = 0; for (; i < header->e_shnum; i++) {
					Elf32_Shdr * section = (Elf32_Shdr *)pData;

					if(strcmp(strtab + section->sh_name, ".rodata.sceModuleInfo") == 0) {
						//store valid string section information
						check[19] = section->sh_offset;
						check[22] = 0;
						break;
					}

					pData += header->e_shentsize;
				}
			}
		}
	}

	//return result
	return result;
}

int _ProbeExec3(unsigned char * buffer, int * check)
{
	//check executable
	int result = ProbeExec3(buffer, check);

	//grab executable magic
	unsigned int magic = *(unsigned int *)(buffer);

	//patch necessary
	if(check[2] >= 0x52 && magic == 0x464C457F && IsStaticElf(buffer)) {
		//patch check
		check[8] = 3;

		//fake result
		result = 0;
	}

	//return result
	return result;
}

int _PartitionCheck(unsigned int * st0, unsigned int * check)
{
	//get file descriptor
	SceUID fd = st0[6];
	unsigned int p[64 + 64 / sizeof(unsigned int)], *checkBuf;

	//module attributes
	unsigned short attributes = 0;

	checkBuf = (unsigned int*)((((u32)p) & ~(64-1)) + 64);

	//invalid file descriptor
	if(fd < 0) return PartitionCheck(st0, check);

	//get file position
	SceOff pos = sceIoLseek(fd, 0, PSP_SEEK_CUR);

	//invalid file position
	if(pos < 0) return PartitionCheck(st0, check);

	//rewind to beginning
	sceIoLseek(fd, 0, PSP_SEEK_SET);

	//read file header
	if(sceIoRead(fd, checkBuf, 256) < 256) {
		//restore position
		sceIoLseek(fd, pos, PSP_SEEK_SET);

		//fallback check
		return PartitionCheck(st0, check);
	}

	//PBP file
	if(checkBuf[0] == 0x50425000) {
		//move to executable
		sceIoLseek(fd, checkBuf[8], PSP_SEEK_SET);

		//read elf header
		sceIoRead(fd, checkBuf, 20);

		//encrypted module
		if(checkBuf[0] != 0x464C457F) {
			//restore position
			sceIoLseek(fd, pos, PSP_SEEK_SET);

			//original check
			return PartitionCheck(st0, check);
		}

		//move to module information
		sceIoLseek(fd, checkBuf[8] + check[19], PSP_SEEK_SET);

		//valid prx file
		if(!IsStaticElf(checkBuf)) {
			//allow psar file
			check[4] = checkBuf[9] - checkBuf[8];
		}
	}
	//ELF file
	else if(checkBuf[0] == 0x464C457F) {
		//move to module information
		sceIoLseek(fd, check[19], PSP_SEEK_SET);
	}
	//encrypted file
	else {
		//restore position
		sceIoLseek(fd, pos, PSP_SEEK_SET);

		//original check
		return PartitionCheck(st0, check);
	}

	//read module attributes
	sceIoRead(fd, &attributes, 2);

	//static elf file
	if(IsStaticElf(checkBuf)) {
		check[17] = 0;
	}
	//prx files
	else {
		//kernel prx
		if(attributes & 0x1000) check[0x44/4] = 1;

		//user prx
		else check[0x44/4] = 0;
	}

	//restore position
	sceIoLseek(fd, pos, PSP_SEEK_SET);

	//check executable
	return PartitionCheck(st0, check);
}

void patch_sceLoaderCore(void)
{
	//find module
	SceModule2 * loadcore = (SceModule2 *)sctrlKernelFindModuleByName("sceLoaderCore");

	//patch sceKernelCheckExecFile (sub_0C10)
	_sw((unsigned int)_sceKernelCheckExecFile, loadcore->text_addr + g_offs->loadercore_patch.sceKernelCheckExecFilePtr);
	_sw(MAKE_CALL(_sceKernelCheckExecFile), loadcore->text_addr + g_offs->loadercore_patch.sceKernelCheckExecFileCall1);
	_sw(MAKE_CALL(_sceKernelCheckExecFile), loadcore->text_addr + g_offs->loadercore_patch.sceKernelCheckExecFileCall2);
	_sw(MAKE_CALL(_sceKernelCheckExecFile), loadcore->text_addr + g_offs->loadercore_patch.sceKernelCheckExecFileCall3);

	//6.35 relocation fix for rt7
	//fake relocation type 7 to be treated like 0
	//patches handler table so jr $t5 returns properly on type 7 ;)
	u32 faketype = 0;
	u32 origtype = 7;
	_sw(*(u32 *)(loadcore->text_addr + g_offs->loadercore_patch.ReloactionTable + faketype * 4), loadcore->text_addr + g_offs->loadercore_patch.ReloactionTable + origtype * 4);

	//patch ProbeExec1 (sub_001AC)
	ProbeExec1 = (void*)loadcore->text_addr + g_offs->loadercore_patch.ProbeExec1; //dword_6248
	_sw(MAKE_CALL(_ProbeExec1), loadcore->text_addr + g_offs->loadercore_patch.ProbeExec1Call);

	//patch ProbeExec2 (sub_004E8)
	ProbeExec2 = (void*)loadcore->text_addr + g_offs->loadercore_patch.ProbeExec2; //dword_6364
	_sw(MAKE_CALL(_ProbeExec2), loadcore->text_addr + g_offs->loadercore_patch.ProbeExec2Call1);
	_sw(MAKE_CALL(_ProbeExec2), loadcore->text_addr + g_offs->loadercore_patch.ProbeExec2Call2);

	//enable syscall exports (?)
	_sw(0x3C090000, loadcore->text_addr + g_offs->loadercore_patch.EnableSyscallExport);
	
	//undo check #1
	_sw(0, loadcore->text_addr + g_offs->loadercore_patch.LoaderCoreCheck1); //bnez

	//undo check #2
	_sw(0, loadcore->text_addr + g_offs->loadercore_patch.LoaderCoreCheck2); //beqzl
	_sw(0, loadcore->text_addr + g_offs->loadercore_patch.LoaderCoreCheck2 + 4); //lui (likely branch instruction)

	//undo check #3
	_sw(0, loadcore->text_addr + g_offs->loadercore_patch.LoaderCoreCheck3); //beqzl
	_sw(0, loadcore->text_addr + g_offs->loadercore_patch.LoaderCoreCheck3 + 4); //lui (likely branch instruction)

	// pops version check
	_sw(0x1000FFCB, loadcore->text_addr + g_offs->loadercore_patch.pops_version_check); // b loc_000075B4

	//undo rebootex patches
	void * memlmd_323366CA = (void*)sctrlHENFindFunction("sceMemlmd", "memlmd", g_offs->loadercore_patch.memlmd_323366CA_NID);
	_sw(MAKE_CALL(memlmd_323366CA), loadcore->text_addr + g_offs->loadercore_patch.LoaderCoreUndo1Call1);
	_sw(MAKE_CALL(memlmd_323366CA), loadcore->text_addr + g_offs->loadercore_patch.LoaderCoreUndo1Call2);
	_sw(MAKE_CALL(memlmd_323366CA), loadcore->text_addr + g_offs->loadercore_patch.LoaderCoreUndo1Call3);
	void * memlmd_7CF1CD3E = (void*)sctrlHENFindFunction("sceMemlmd", "memlmd", g_offs->loadercore_patch.memlmd_7CF1CD3E_NID);
	_sw(MAKE_CALL(memlmd_7CF1CD3E), loadcore->text_addr + g_offs->loadercore_patch.LoaderCoreUndo2Call1);
	_sw(MAKE_CALL(memlmd_7CF1CD3E), loadcore->text_addr + g_offs->loadercore_patch.LoaderCoreUndo2Call2);

	setup_nid_resolver();

#ifdef DEBUG
	hook_import_bynid((SceModule*)loadcore, "KDebugForKernel", 0x84F370BC, printk, 0);
#endif

	patch_sceKernelStartModule(loadcore->text_addr);
}
