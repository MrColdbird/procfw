	.set noreorder

#include "pspstub.s"

	STUB_START "SysMemUserForUser",0x40000011,0x00010005
	STUB_FUNC  0xD8DE5C1E,SysMemUserForUser_D8DE5C1E
	STUB_END

	STUB_START "sceUtility_private",0x40000011,0x00020005
	STUB_FUNC  0xE65F37C8, scePowerRegisterCallbackPrivate
	STUB_FUNC  0x9ABAFCC5, scePowerUnregisterCallbackPrivate
	STUB_END
