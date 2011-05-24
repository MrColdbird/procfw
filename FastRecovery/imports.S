	.set noreorder

#include "pspstub.s"

	STUB_START "SysMemUserForUser",0x40000011,0x00010005
	STUB_FUNC  0xD8DE5C1E,SysMemUserForUser_D8DE5C1E
	STUB_END

	STUB_START "sceHttpStorage",0x40090011,0x00010005
	STUB_FUNC  0x700AAD44,sceHttpStorageOpen
	STUB_END
