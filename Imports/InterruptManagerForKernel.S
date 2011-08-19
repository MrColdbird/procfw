  .set noreorder

#include "pspimport.s"

  IMPORT_START  "InterruptManagerForKernel",0x00010000
  IMPORT_FUNC "InterruptManagerForKernel",0x399FF74C,sceKernelQuerySystemCall
  IMPORT_FUNC "InterruptManagerForKernel",0xAC9306F0,sceKernelQuerySystemCall_620
  IMPORT_FUNC "InterruptManagerForKernel",0xF153B371,sceKernelQuerySystemCall_660
