#include <pspkernel.h>
#include "systemctrl.h"
#include "utils.h"
#include "jumper.h"
#include "printk.h"

//current entry count
u32 jumper_cur_count = 0;

//jumper table buffer
static u32 jumper_table[17 * JUMPER_MAX_COUNT];

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

  //log trace
  printk("TRACE: %s_%08X to %s_%08X ret = %08X\n", caller_name, ra - caller_textaddr - 8, callee_name, target - callee_textaddr, result);
}

//single trace install
int installJALTrace(u32 address)
{
  //result
  int result = 0;

  //valid address and slot available
  if(address && jumper_cur_count < JUMPER_MAX_COUNT)
  {
    //get instruction
    u32 inst = *(u32 *)(address);

    //jal instruction
    if((inst & 0xFC000000) == 0x0C000000)
    {
      //get target address
      u32 target = GET_CALL_ADDR(inst);

      //add kernel scope specifier
      if(address & 0x80000000) target |= 0x80000000;

      //get jumper entry
      u32 * jumper = &jumper_table[jumper_cur_count * 17];

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

      //call log function
      jumper[9] = MAKE_CALL(dumpJAL); //jal dumpJAL

      //prepare log argument #3 : function result (delay slot)
      jumper[10] = 0x00403021; //move $a2, $v0

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
      jumper_cur_count++;
    }
  }

  //invalid address or no slot available
  else result = -1;

  //return result
  return result;
}

//module trace install
void installModuleJALTrace(SceModule2 * module)
{
  //valid argument
  if(module)
  {
    //iterate instructions
    u32 pos = 0; for(; pos < module->text_size; pos += 4)
    {
      //check instruction and install trace
      if(installJALTrace(module->text_addr + pos) != 0) break;
    }
  }
}

//clear tracing table
void clearTraceTable(void)
{
  //restore instructions
  u32 i = 0; for(; i < jumper_cur_count; i++)
  {
    //get table entry
    u32 * jumper = &jumper_table[i * 17];

    //restore instruction
    _sw(jumper[2], jumper[16]);
  }

  //reset counter
  jumper_cur_count = 0;
}
