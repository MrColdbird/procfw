	.set noreorder

#include "pspstub.s"

	STUB_START "SysMemUserForUser",0x40000011,0x00010005
	STUB_FUNC  0xD8DE5C1E,SysMemUserForUser_D8DE5C1E
	STUB_END
	
	STUB_START "UtilsForUser",0x40000011,0x00020005
	STUB_FUNC  0x920F104A,sceKernelIcacheInvalidateAll
	STUB_FUNC  0xB435DEC5,sceKernelDcacheWritebackInvalidateAll
	STUB_END

	STUB_START "sceHttpStorage",0x40090011,0x00010005
	STUB_FUNC  0x700AAD44,sceHttpStorageOpen
	STUB_END

	STUB_START "sceUtility_private",0x40000011,0x00040005
	STUB_FUNC  0xE65F37C8, scePowerRegisterCallbackPrivate_635
	STUB_FUNC  0x9ABAFCC5, scePowerUnregisterCallbackPrivate_635
	STUB_FUNC  0x764F5A3C, scePowerRegisterCallbackPrivate_620
	STUB_FUNC  0x2DC8380C, scePowerUnregisterCallbackPrivate_620
	STUB_END

	STUB_START "sceNetIfhandle_lib",0x40090011,0x00010005
	STUB_FUNC  0xE80F00A4, sceNetMPulldown
	STUB_END
