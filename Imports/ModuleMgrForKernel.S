	.set noreorder

#include "pspimport.s"

	IMPORT_START	"ModuleMgrForKernel",0x00010000
	IMPORT_FUNC	"ModuleMgrForKernel",0xFFB9B760,sceKernelLoadModule
	IMPORT_FUNC	"ModuleMgrForKernel",0xE6BF3960,sceKernelStartModule
	IMPORT_FUNC	"ModuleMgrForKernel",0x0D053026,sceKernelUnloadModule
	IMPORT_FUNC	"ModuleMgrForKernel",0x32292450,ModuleMgrForKernel_32292450
