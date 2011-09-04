	.set noreorder

#include "pspstub.s"

	STUB_START "SysMemForKernel",0x40000011,0x00030005
	STUB_FUNC  0xC7E57B9C,sceKernelSetQTGP3
	STUB_FUNC  0x3FC9AE6A,sceKernelDevkitVersion
	STUB_FUNC  0x6373995D,sceKernelGetModel
	STUB_END

	STUB_START "InitForKernel",0x00090011,0x00010005
	STUB_FUNC  0x7233B5BC,sceKernelApplicationType
	STUB_END
