	.set noreorder

#include "pspimport.s"

// Build List

	IMPORT_START	"SysMemForKernel",0x00010011
	IMPORT_FUNC	"SysMemForKernel",0x458A70B5,sceKernelGetModel
	IMPORT_FUNC	"SysMemForKernel",0x864EBFD7,sceKernelGetModel_620
	IMPORT_FUNC	"SysMemForKernel",0x5E8DCA05,sceKernelDevkitVersion
	IMPORT_FUNC	"SysMemForKernel",0xFE9BC18B,sceKernelDevkitVersion_620
	IMPORT_FUNC	"SysMemForKernel",0x00E9A04A,SysMemForKernel_00E9A04A
	IMPORT_FUNC	"SysMemForKernel",0xAF85EB1B,sceKernelCreateHeap
	IMPORT_FUNC	"SysMemForKernel",0xF9475C1A,sceKernelDeleteHeap
	IMPORT_FUNC	"SysMemForKernel",0x6D161EE2,sceKernelAllocHeapMemory
	IMPORT_FUNC	"SysMemForKernel",0xDB836ADB,sceKernelFreeHeapMemory
	IMPORT_FUNC	"SysMemForKernel",0x9F154FA1,SysMemForKernel_9F154FA1
	IMPORT_FUNC	"SysMemForKernel",0xD9A4D69E,sceKernelGetSystemStatus
	IMPORT_FUNC	"SysMemForKernel",0xF5E82409,sceKernelQueryMemoryPartitionInfo
	IMPORT_FUNC	"SysMemForKernel",0xE10F21CF,sceKernelPartitionMaxFreeMemSize
	IMPORT_FUNC	"SysMemForKernel",0x7BE9653E,sceKernelPartitionTotalFreeMemSize
