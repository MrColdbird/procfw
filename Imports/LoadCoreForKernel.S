	.set noreorder

#include "pspimport.s"

	IMPORT_START	"LoadCoreForKernel",0x00010000
	IMPORT_FUNC	"LoadCoreForKernel",0xEF8A0BEA,sceKernelFindModuleByName
	IMPORT_FUNC	"LoadCoreForKernel",0xBEF0A05E,sceKernelFindModuleByName_620
	IMPORT_FUNC	"LoadCoreForKernel",0xED53894F,sceKernelFindModuleByUID
	IMPORT_FUNC	"LoadCoreForKernel",0x312CA47E,sceKernelFindModuleByAddress
	IMPORT_FUNC	"LoadCoreForKernel",0x312CA47E,LoadCoreForKernel_312CA47E
	IMPORT_FUNC	"LoadCoreForKernel",0x01DB1EB3,LoadCoreForKernel_01DB1EB3
	IMPORT_FUNC	"LoadCoreForKernel",0x07738DC3,LoadCoreForKernel_07738DC3
	IMPORT_FUNC	"LoadCoreForKernel",0x6565B132,LoadCoreForKernel_6565B132
