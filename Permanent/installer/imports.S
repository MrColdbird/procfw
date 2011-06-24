	.set noreorder

#include "pspstub.s"

	STUB_START "SystemCtrlForUser",0x40090000,0x00020005
	STUB_FUNC  0x1090A2E1,sctrlHENGetVersion
	STUB_FUNC  0x5328B431,sctrlHENGetMinorVersion
	STUB_END

	STUB_START "KUBridge",0x40090000,0x00020005
	STUB_FUNC  0x4C25EA72,kuKernelLoadModule
	STUB_FUNC  0x24331850,kuKernelGetModel
	STUB_END

	STUB_START "sceUtility_private",0x40090000,0x00020005
	STUB_FUNC  0x2DC8380C,_scePowerUnregisterCallback
	STUB_FUNC  0x764F5A3C,_scePowerRegisterCallback
	STUB_END
