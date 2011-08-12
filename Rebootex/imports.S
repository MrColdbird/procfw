	.set noreorder

#include "pspimport.s"


	IMPORT_START	"SysMemForKernel",0x00010011
	IMPORT_FUNC	"SysMemForKernel",0x458A70B5,sceKernelGetModel
	IMPORT_FUNC	"SysMemForKernel",0x864EBFD7,sceKernelGetModel_620
	IMPORT_FUNC	"SysMemForKernel",0x5E8DCA05,sceKernelDevkitVersion
	IMPORT_FUNC	"SysMemForKernel",0xFE9BC18B,sceKernelDevkitVersion_620

	IMPORT_START	"LoadCoreForKernel",0x00010000
	IMPORT_FUNC	"LoadCoreForKernel",0xEF8A0BEA,sceKernelFindModuleByName
	IMPORT_FUNC	"LoadCoreForKernel",0xBEF0A05E,sceKernelFindModuleByName_620
