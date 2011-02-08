#include <pspkernel.h>
#include <pspsysevent.h>
#include <pspiofilemgr.h>
#include <stdio.h>
#include <string.h>

#include "systemctrl.h"
#include "libs.h"
#include "pspmodulemgr_kernel.h"
#include "printk.h"
#include "utils.h"
#include "kubridge.h"
#include "modhandler.h"
#include "funcs_630.h"
#include "libs.h"
#include "elf.h"

SceUID g_mod_start_thid = -1;
SceModule2 *g_mod_start = NULL;
int (*g_on_module_start)(SceModule2*) = NULL;

int PatchExec1(unsigned char * buffer, int * check);
int PatchExec2(unsigned char * buffer, int * check);
int PatchExec3(unsigned char * buffer, int * check, int isplain, int checkresult);
int LoadCoreForKernel_01DB1EB3(unsigned char * buffer, int * check);

//original functions
int (* ProbeExec1)(unsigned char * buffer, int * check) = NULL;
int (* ProbeExec2)(unsigned char * buffer, int * check) = NULL;
int (* ProbeExec3)(unsigned char * buffer, int * check) = NULL; //GO ONLY
int (* PartitionCheck)(unsigned int * st0, unsigned int * check);

#define J_TARGET(f) (((f & ~(0x08000000)) << 2) | 0x80000000)
#define JAL_TARGET(f) (((f & ~(0x0C000000)) << 2) | 0x80000000)

int myKernelStartThread(SceUID thid, SceSize arglen, void *argp)
{
	int ret;

	if (g_mod_start_thid != -1 && thid == g_mod_start_thid) {
//		printk("%s: g_mod_start 0x%08x g_mod_start_thid 0x%08x g_on_module_start 0x%08x\r\n", __func__, g_mod_start, g_mod_start_thid, g_on_module_start);

		if (g_on_module_start != NULL && g_mod_start != NULL) {
			(*g_on_module_start)(g_mod_start);
		}

		g_mod_start_thid = -1;
		g_mod_start = NULL;
	}

	ret = sceKernelStartThread(thid, arglen, argp);

	return ret;
}

int myKernelCreateThread(const char *name, void *entry, int pri, int stack, u32 attr, void *opt)
{
	int ret;

	ret = sceKernelCreateThread(name, entry, pri, stack, attr, opt);

	if (ret >= 0 && !strcmp(name, "SceModmgrStart")) {
//		printk("%s: g_mod_start 0x%08x g_mod_start_thid 0x%08x entry 0x%08x\r\n", __func__, g_mod_start, g_mod_start_thid, entry);
		g_mod_start = LoadCoreForKernel_312CA47E(entry);
		g_mod_start_thid = ret;
	}

	return ret;
}

int _sceKernelCheckExecFile(unsigned char * buffer, int * check);
int _ProbeExec1(unsigned char * buffer, int * check);
int _ProbeExec2(unsigned char * buffer, int * check);
int _ProbeExec3(unsigned char * buffer, int * check); //GO ONLY
int _PartitionCheck(unsigned int * st0, unsigned int * check);
int _NIDResolver(unsigned char * stubs, unsigned int stubsize, unsigned int arg3);
int _ModuleStarter(int (* ModuleStarter)(int arg1, int arg2), int arg2);
SceUID _CreateThread(const char * name, SceKernelThreadEntry entry, int initPriority, int stackSize, SceUInt attr, SceKernelThreadOptParam * option);
int _StartThread(SceUID thid, SceSize args, void * argp);

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
		//sceKernelCheckExecFile (LoadCoreForKernel_B0466E46)
		//LoadCoreForKernel_01DB1EB3 in 6.30
		int checkresult = LoadCoreForKernel_01DB1EB3(buffer, check);

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

static unsigned int checkBuf[64] __attribute__((aligned(64)));

int _PartitionCheck(unsigned int * st0, unsigned int * check)
{
	//get file descriptor
	SceUID fd = st0[6];

	//module attributes
	unsigned short attributes = 0;

	//invalid file descriptor
	if(fd < 0) return PartitionCheck(st0, check);

	//get file position
	int pos = sceIoLseek(fd, 0, PSP_SEEK_CUR);

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

int patch_sceModuleManager(void)
{
	SceModule2 *mod;
	int intc;

	mod = (SceModule2*) sceKernelFindModuleByName("sceModuleManager");

	if (mod == NULL)
		return -1;
	
	intc = pspSdkDisableInterrupts();

	//backup function pointer (dword_622C)
	ProbeExec3 = (void*)mod->text_addr + 0x8864;

	//override function (sub_0045C)
	_sw(MAKE_CALL(_ProbeExec3), mod->text_addr + 0x7C6C);

	_sw(MAKE_JUMP(_sceKernelCheckExecFile), mod->text_addr+0x87E4);

	PartitionCheck = (void*)mod->text_addr + 0x7FE0;
	_sw(MAKE_CALL(_PartitionCheck), mod->text_addr + 0x652C);
	_sw(MAKE_CALL(_PartitionCheck), mod->text_addr + 0x68A8);

	hook_import_bynid((SceModule*)mod, "ThreadManForKernel", 0x446D8DE6, &myKernelCreateThread, 0);
	hook_import_bynid((SceModule*)mod, "ThreadManForKernel", 0xF475845D, &myKernelStartThread, 0);

	//no device check patches
	_sw(0, mod->text_addr + 0x0760);
	_sw(0x24020000, mod->text_addr + 0x07C0);
	_sw(0, mod->text_addr + 0x30B0);
	_sw(0, mod->text_addr + 0x310C);
	_sw(0x10000009, mod->text_addr + 0x3138);
	_sw(0, mod->text_addr + 0x3444);
	_sw(0, mod->text_addr + 0x349C);
	_sw(0x10000010, mod->text_addr + 0x34C8);
	_sw(0, mod->text_addr + 0x4360);
	_sw(0, mod->text_addr + 0x43A8);
	_sw(0, mod->text_addr + 0x43C0);

	sync_cache();
	pspSdkEnableInterrupts(intc);

	return 0;
}

void patch_sceLoadCore(void)
{
	//find module
	SceModule2 * loadcore = (SceModule2 *)sceKernelFindModuleByName("sceLoaderCore");

	//patch sceKernelCheckExecFile (sub_00C10)
	_sw((unsigned int)_sceKernelCheckExecFile, loadcore->text_addr + 0x7DC0);
	_sw(MAKE_CALL(_sceKernelCheckExecFile), loadcore->text_addr + 0x1570);
	_sw(MAKE_CALL(_sceKernelCheckExecFile), loadcore->text_addr + 0x15C0);
	_sw(MAKE_CALL(_sceKernelCheckExecFile), loadcore->text_addr + 0x4BE8);

	//patch ProbeExec1 (sub_001AC)
	ProbeExec1 = (void*)loadcore->text_addr + 0x679C; //dword_6248
	_sw(MAKE_CALL(_ProbeExec1), loadcore->text_addr + 0x47E4);

	//patch ProbeExec2 (sub_004E8)
	ProbeExec2 = (void*)loadcore->text_addr + 0x66F4; //dword_6364
	_sw(MAKE_CALL(_ProbeExec2), loadcore->text_addr + 0x49E4);
	_sw(MAKE_CALL(_ProbeExec2), loadcore->text_addr + 0x6A14);

	//undo check #1
	_sw(0, loadcore->text_addr + 0x76E4); //bnez

	//undo check #2
	_sw(0, loadcore->text_addr + 0x5C34); //beqzl
	_sw(0, loadcore->text_addr + 0x5C38); //lui (likely branch instruction)

	//undo check #3
	_sw(0, loadcore->text_addr + 0x5D44); //beqzl
	_sw(0, loadcore->text_addr + 0x5D48); //lui (likely branch instruction)

	//undo rebootex patches
	void * memlmd_3F2AC9C6 = (void*)sctrlHENFindFunction("sceMemlmd", "memlmd", 0x3F2AC9C6);
	_sw(MAKE_CALL(memlmd_3F2AC9C6), loadcore->text_addr + 0x5CC8);
	_sw(MAKE_CALL(memlmd_3F2AC9C6), loadcore->text_addr + 0x5CF8);
	_sw(MAKE_CALL(memlmd_3F2AC9C6), loadcore->text_addr + 0x5D90);
	void * memlmd_E42AFE2E = (void*)sctrlHENFindFunction("sceMemlmd", "memlmd", 0xE42AFE2E);
	_sw(MAKE_CALL(memlmd_E42AFE2E), loadcore->text_addr + 0x41A4);
	_sw(MAKE_CALL(memlmd_E42AFE2E), loadcore->text_addr + 0x5CA4);
}
