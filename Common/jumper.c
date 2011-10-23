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

#include <pspkernel.h>
#include "systemctrl.h"
#include "utils.h"
#include "jumper.h"
#include "printk.h"

#include <pspthreadman_kernel.h>

//current entry count
u32 jumper_cur_count_kernel = 0;
u32 jumper_cur_count_user = 0;

//jumper table buffer
u32 jumper_table_kernel[17 * JUMPER_MAX_COUNT];
u32 * jumper_table_user = NULL;

//dumper syscall number
u32 syscallnum = 0;

//init user memory
int initUserTraceMemory(void)
{
	//get functions
	int (* alloc)(u32, char *, u32, u32, u32) = (void *)sctrlHENFindFunction("sceSystemMemoryManager", "SysMemUserForUser", 0x237DBD4F);
	void * (* gethead)(u32) = (void *)sctrlHENFindFunction("sceSystemMemoryManager", "SysMemUserForUser", 0x9D9A5BA1);

	//allocate memory
	int result = alloc(2, "JumperMemory", PSP_SMEM_High, 17 * JUMPER_MAX_COUNT, 0);
	if(result >= 0)
	{
		//get memory
		jumper_table_user = gethead(result);

		//get syscall number
		syscallnum = sctrlKernelQuerySystemCall(dumpJAL);

		//log syscall number
		printk("Jumper Syscall Number: %08X\n", syscallnum);
	}

	//return result
	return result;
}

//trace log function
void dumpJAL(u32 target, u32 ra, u32 result)
{
	//get caller module
	SceModule2 * caller = (SceModule2 *)sceKernelFindModuleByAddress(ra);

	//get callee module
	SceModule2 * callee = (SceModule2 *)sceKernelFindModuleByAddress(target);

	//caller data
	const char * caller_name = "UNK";
	u32 caller_textaddr = 0;
	if(caller)
	{
		caller_name = caller->modname;
		caller_textaddr = caller->text_addr;
	}

	//callee data
	const char * callee_name = "UNK";
	u32 callee_textaddr = 0;
	if(callee)
	{
		callee_name = callee->modname;
		callee_textaddr = callee->text_addr;
	}

	//textual trace
	printk("TRACE %08X: %s_%08X to %s_%08X ret = %08X\n", sceKernelGetThreadId(), caller_name, ra - caller_textaddr - 8, callee_name, target - callee_textaddr, result);
}

//for readability
#define KERNEL_OKAY(address) (address & 0x80000000 && jumper_cur_count_kernel < JUMPER_MAX_COUNT)
#define USER_OKAY(address) (address && jumper_table_user && jumper_cur_count_user < JUMPER_MAX_COUNT)

//single trace install
void installJALTrace(u32 address)
{
	//valid address and slot available
	if(KERNEL_OKAY(address) || USER_OKAY(address))
	{
		//get instruction
		u32 inst = *(u32 *)(address);

		//jal instruction
		if((inst & 0xFC000000) == 0x0C000000)
		{
			//get target address
			u32 target = GET_CALL_ADDR(inst);

			//jumper logging stub
			u32 * jumper = NULL;

			//kernel address
			if(address & 0x80000000)
			{
				//fix target address for kernel range
				target |= 0x80000000;

				//get jumper stub
				jumper = &jumper_table_kernel[jumper_cur_count_kernel * 17];
			}

			//user address
			else
			{
				//get jumper stub
				jumper = &jumper_table_user[jumper_cur_count_user * 17];
			}

			//allocate 16 bytes of memory on stack
			jumper[0] = 0x27BDFFF0; //addiu $sp, $sp, -16

			//backup return address (can't place in delay slot, would ruin register)
			jumper[1] = 0xAFBF0000; //sw $ra, 0($sp)

			//execute original function call
			jumper[2] = inst; //jal target

			//necessary nop (delay slot)
			jumper[3] = 0; //nop

			//backup function result
			jumper[4] = 0xAFA20004; //sw $v0, 4($sp)
			jumper[5] = 0xAFA30008; //sw $v1, 8($sp)

			//prepare log argument #1 : target address
			jumper[6] = 0x3C040000 | (target >> 16); //lui $a0, (target >> 16)
			jumper[7] = 0x34840000 | (target & 0xFFFF); //ori $a0, $a0, (target & 0xFFFF)

			//prepare log argument #2 : return address
			jumper[8] = 0x8FA50000; //lw $a1, 0($sp)

			//kernel module
			if(address & 0x80000000)
			{
				//call log function
				jumper[9] = MAKE_CALL(dumpJAL); //jal dumpJAL

				//prepare log argument #3 : function result (delay slot)
				jumper[10] = 0x00403021; //move $a2, $v0
			}

			//user module
			else
			{
				//prepare log argument #3 : function result
				jumper[9] = 0x00403021; //move $a2, $v0

				//call log function
				jumper[10] = (syscallnum << 6) | 12; //syscall dumpJAL
			}

			//restore return address
			jumper[11] = 0x8FBF0000; //lw $ra, 0($sp)

			//restore function result
			jumper[12] = 0x8FA20004; //lw $v0, 4($sp)
			jumper[13] = 0x8FA30008; //lw $v1, 8($sp)

			//return to caller
			jumper[14] = 0x03E00008; //jr $ra

			//free 16 bytes of memory on stack (delay slot)
			jumper[15] = 0x27BD0010; //addiu $sp, $sp, 16

			//save instruction address for restoring
			jumper[16] = address;

			//overwrite original jal
			_sw(MAKE_CALL(jumper), address);

			//increase jumper count
			if(address & 0x80000000) jumper_cur_count_kernel++;
			else jumper_cur_count_user++;

			//log installed trace
			//printk("Installed Trace: %08X\n", address);
		}
	}
}

void installMemoryJALTrace(u32 start, u32 size)
{
	//iterate instructions
	u32 pos = start; for(; pos < start + size; pos += 4)
	{
		//check instruction and install trace
		installJALTrace(pos);
	}
}

//module trace install
void installModuleJALTrace(SceModule2 * module)
{
	//valid argument
	if(module && (KERNEL_OKAY(module->text_addr) || USER_OKAY(module->text_addr)))
	{
		//iterate instructions
		u32 pos = 0; for(; pos < module->text_size; pos += 4)
		{
			//check instruction and install trace
			installJALTrace(module->text_addr + pos);
		}
	}
}

//clear tracing table
void clearTraceTable(void)
{
	//restore kernel instructions
	u32 i = 0; for(; i < jumper_cur_count_kernel; i++)
	{
		//get table entry
		u32 * jumper = &jumper_table_kernel[i * 17];

		//restore instruction
		_sw(jumper[2], jumper[16]);
	}

	//restore user instructions
	for(i = 0; i < jumper_cur_count_user; i++)
	{
		//get table entry
		u32 * jumper = &jumper_table_user[i * 17];

		//restore instruction
		_sw(jumper[2], jumper[16]);
	}

	//reset counter
	jumper_cur_count_kernel = 0;
	jumper_cur_count_user = 0;
}
