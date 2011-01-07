	.set noreorder

#include "pspimport.s"

// Build List

	IMPORT_START	"SysMemForKernel",0x00010011
	IMPORT_FUNC	"SysMemForKernel",0x458A70B5,sceKernelGetModel
	IMPORT_FUNC	"SysMemForKernel",0x5E8DCA05,sceKernelDevkitVersion
