	.set noreorder

#include "pspimport.s"

	IMPORT_START	"InitForKernel",0x00090000
	IMPORT_FUNC	"InitForKernel",0x27932388,sceKernelBootFrom
	IMPORT_FUNC	"InitForKernel",0x9D33A110,sceKernelBootFromGo_660
	IMPORT_FUNC	"InitForKernel",0xEE67E450,sceKernelBootFromGo_635
	IMPORT_FUNC	"InitForKernel",0x977A0F90,sceKernelBootFromGo_620
	IMPORT_FUNC	"InitForKernel",0x7233B5BC,sceKernelApplicationType
	IMPORT_FUNC	"InitForKernel",0x7A2333AD,sceKernelInitApitype
	IMPORT_FUNC	"InitForKernel",0xA6E71B93,sceKernelInitFileName
	IMPORT_FUNC	"InitForKernel",0x040C934B,InitForKernel_040C934B
