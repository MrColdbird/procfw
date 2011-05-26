	.set noreorder

#include "pspstub.s"

	STUB_START "SystemCtrlForUser",0x40090000,0x00010005
	STUB_FUNC  0x1090A2E1,sctrlHENGetVersion
	STUB_END

	STUB_START "KUBridge",0x40090000,0x00010005
	STUB_FUNC  0x4C25EA72,kuKernelLoadModule
	STUB_END

	STUB_START "scePower",0x40090000,0x00010005
	STUB_FUNC  0x0442D852,scePowerRequestColdReset
	STUB_END
	
	STUB_START "sceUtility_private",0x40090000,0x00020005
	STUB_FUNC  0x2DC8380C,scePowerUnregisterCallbackPrivate
	STUB_FUNC  0x764F5A3C,scePowerRegisterCallbackPrivate
	STUB_END

	STUB_START "UtilsForUser",0x40000011,0x00020005
	STUB_FUNC  0x920F104A,sceKernelIcacheInvalidateAll
	STUB_FUNC  0xB435DEC5,sceKernelDcacheWritebackInvalidateAll
	STUB_END
