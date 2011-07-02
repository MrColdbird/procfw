	.set noreorder

#include "pspimport.s"

	IMPORT_START	"LoadExecForKernel",0x00090000
	IMPORT_FUNC	"LoadExecForKernel",0x5AA1A6D2,sceKernelExitVSH
	IMPORT_FUNC	"LoadExecForKernel",0xAEFEF0F4,sceKernelExitVSH_620
	IMPORT_FUNC	"LoadExecForKernel",0x45C6125B,sceKernelLoadExecVSHDisc
	IMPORT_FUNC	"LoadExecForKernel",0x73BAD4F8,sceKernelLoadExecVSHDisc_620
	IMPORT_FUNC	"LoadExecForKernel",0x179D905A,sceKernelLoadExecVSHDiscUpdater
	IMPORT_FUNC	"LoadExecForKernel",0xD364D324,sceKernelLoadExecVSHDiscUpdater_620
	IMPORT_FUNC	"LoadExecForKernel",0x7286CF0B,sceKernelLoadExecVSHMs1
	IMPORT_FUNC	"LoadExecForKernel",0x783EA19F,sceKernelLoadExecVSHMs1_620
	IMPORT_FUNC	"LoadExecForKernel",0x3D805DE6,sceKernelLoadExecVSHMs2
	IMPORT_FUNC	"LoadExecForKernel",0x254F01CB,sceKernelLoadExecVSHMs2_620
	IMPORT_FUNC	"LoadExecForKernel",0xBAEB4B89,sceKernelLoadExecVSHMs3
	IMPORT_FUNC	"LoadExecForKernel",0xE09DAFDC,sceKernelLoadExecVSHMs3_620
	IMPORT_FUNC	"LoadExecForKernel",0x8EF38192,sceKernelLoadExecVSHMs4
	IMPORT_FUNC	"LoadExecForKernel",0xEFA048F1,sceKernelLoadExecVSHMs4_620
	IMPORT_FUNC	"LoadExecForKernel",0xD35D6403,sceKernelLoadExecVSHEf2
	IMPORT_FUNC	"LoadExecForKernel",0x88F03BAF,sceKernelLoadExecVSHEf2_620
