	.set noreorder

#include "pspimport.s"

	IMPORT_START	"LoadCoreForKernel",0x00010000
	IMPORT_FUNC	"LoadCoreForKernel",0xEF8A0BEA,sceKernelFindModuleByName
	IMPORT_FUNC	"LoadCoreForKernel",0xED53894F,sceKernelFindModuleByUID
	IMPORT_FUNC	"LoadCoreForKernel",0x312CA47E,sceKernelFindModuleByAddress
	IMPORT_FUNC	"LoadCoreForKernel",0x01DB1EB3,sceKernelCheckExecFile
	IMPORT_FUNC	"LoadCoreForKernel",0xBEF0A05E,sceKernelFindModuleByName_620
	IMPORT_FUNC	"LoadCoreForKernel",0x77E8F516,sceKernelFindModuleByUID_620
	IMPORT_FUNC	"LoadCoreForKernel",0x452C5524,sceKernelFindModuleByAddress_620
	IMPORT_FUNC	"LoadCoreForKernel",0xB0466E46,sceKernelCheckExecFile_620
	IMPORT_FUNC	"LoadCoreForKernel",0xF6B1BF0F,sceKernelFindModuleByName_660
	IMPORT_FUNC	"LoadCoreForKernel",0x40972E6E,sceKernelFindModuleByUID_660
	IMPORT_FUNC	"LoadCoreForKernel",0xBC99C625,sceKernelFindModuleByAddress_660
	IMPORT_FUNC	"LoadCoreForKernel",0xD3353EC4,sceKernelCheckExecFile_660
