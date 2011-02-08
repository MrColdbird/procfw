	.set noreorder

#include "pspstub.s"

	STUB_START "SysMemUserForUser",0x40000011,0x00010005
	STUB_FUNC  0xD8DE5C1E,SysMemUserForUser_D8DE5C1E
	STUB_END
