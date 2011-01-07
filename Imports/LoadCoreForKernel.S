	.set noreorder

#include "pspimport.s"

	IMPORT_START	"LoadCoreForKernel",0x00010000
	IMPORT_FUNC	"LoadCoreForKernel",0xEF8A0BEA,sceKernelFindModuleByName
	IMPORT_FUNC	"LoadCoreForKernel",0xED53894F,sceKernelFindModuleByUID
	IMPORT_FUNC	"LoadCoreForKernel",0x312CA47E,LoadCoreForKernel_312CA47E
	IMPORT_FUNC	"LoadCoreForKernel",0x01DB1EB3,LoadCoreForKernel_01DB1EB3
