	.set noreorder

#include "pspimport.s"

	IMPORT_START	"sceCtrl_driver",0x00090000
	IMPORT_FUNC	"sceCtrl_driver",0x9F3038AC,sceCtrlReadBufferPositive_639
	IMPORT_FUNC	"sceCtrl_driver",0xD073ECA4,sceCtrlReadBufferPositive_620
	IMPORT_FUNC	"sceCtrl_driver",0xBE30CED0,sceCtrlReadBufferPositive_660
