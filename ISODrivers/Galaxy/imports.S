	.set noreorder

#include "pspstub.s"

	STUB_START "SysMemForKernel",0x40000011,0x00020005
	STUB_FUNC  0xC7E57B9C,SysMemForKernel_C7E57B9C
	STUB_FUNC  0x3FC9AE6A,sceKernelDevkitVersion
	STUB_END
