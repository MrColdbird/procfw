	.set noreorder

#include "pspstub.s"

	STUB_START "SystemCtrlForKernel",0x00090000,0x00010005
	STUB_FUNC  0x159AF5CC,sctrlHENFindFunction
	STUB_END
