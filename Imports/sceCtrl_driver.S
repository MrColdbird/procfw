	.set noreorder

#include "pspimport.s"

// Build List

	IMPORT_START	"sceCtrl_driver",0x00090011
	IMPORT_FUNC	"sceCtrl_driver",0x458A70B5,sceCtrlRegisterButtonCallback
	IMPORT_FUNC	"sceCtrl_driver",0x864EBFD7,sceCtrlRegisterButtonCallback_620
