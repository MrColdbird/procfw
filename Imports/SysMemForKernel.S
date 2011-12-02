	.set noreorder

#include "pspimport.s"

// Build List

	IMPORT_START	"SysMemForKernel",0x00010011
	IMPORT_FUNC	"SysMemForKernel",0x458A70B5,sceKernelGetModel
	IMPORT_FUNC	"SysMemForKernel",0x5E8DCA05,sceKernelDevkitVersion
	IMPORT_FUNC	"SysMemForKernel",0x00E9A04A,sceKernelSetDdrMemoryProtection
	IMPORT_FUNC	"SysMemForKernel",0xAF85EB1B,sceKernelCreateHeap
	IMPORT_FUNC	"SysMemForKernel",0xF9475C1A,sceKernelDeleteHeap
	IMPORT_FUNC	"SysMemForKernel",0x6D161EE2,sceKernelAllocHeapMemory
	IMPORT_FUNC	"SysMemForKernel",0xDB836ADB,sceKernelFreeHeapMemory
	IMPORT_FUNC	"SysMemForKernel",0x9F154FA1,SysMemForKernel_9F154FA1
	IMPORT_FUNC	"SysMemForKernel",0xB00D1EC2,SysMemForKernel_B00D1EC2
	IMPORT_FUNC	"SysMemForKernel",0xD9A4D69E,sceKernelGetSystemStatus
	IMPORT_FUNC	"SysMemForKernel",0xF5E82409,sceKernelQueryMemoryPartitionInfo
	IMPORT_FUNC	"SysMemForKernel",0x7BE9653E,sceKernelPartitionTotalFreeMemSize
	IMPORT_FUNC	"SysMemForKernel",0x52B54B93,sceKernelGetBlockHeadAddr
	IMPORT_FUNC	"SysMemForKernel",0x4621A9CC,sceKernelAllocPartitionMemory
	IMPORT_FUNC	"SysMemForKernel",0xE10F21CF,sceKernelPartitionMaxFreeMemSize
	IMPORT_FUNC	"SysMemForKernel",0x864EBFD7,sceKernelGetModel_620
	IMPORT_FUNC	"SysMemForKernel",0xFE9BC18B,sceKernelDevkitVersion_620
	IMPORT_FUNC	"SysMemForKernel",0x31DFE03F,sceKernelSetDdrMemoryProtection_620
	IMPORT_FUNC	"SysMemForKernel",0xC6A782DA,sceKernelCreateHeap_620
	IMPORT_FUNC	"SysMemForKernel",0x9BAA857E,sceKernelDeleteHeap_620
	IMPORT_FUNC	"SysMemForKernel",0x96BFE779,sceKernelAllocHeapMemory_620
	IMPORT_FUNC	"SysMemForKernel",0xC2A8C491,sceKernelFreeHeapMemory_620
	IMPORT_FUNC	"SysMemForKernel",0x957A966D,sceKernelGetSystemStatus_620
	IMPORT_FUNC	"SysMemForKernel",0xE057A674,sceKernelQueryMemoryPartitionInfo_620
	IMPORT_FUNC	"SysMemForKernel",0xA25BF7A1,sceKernelPartitionMaxFreeMemSize_620
	IMPORT_FUNC	"SysMemForKernel",0x35588461,sceKernelPartitionTotalFreeMemSize_620
	IMPORT_FUNC	"SysMemForKernel",0xFEB5C72B,sceKernelGetBlockHeadAddr_620
	IMPORT_FUNC	"SysMemForKernel",0xE5E18A99,sceKernelAllocPartitionMemory_620
	IMPORT_FUNC	"SysMemForKernel",0x07C586A1,sceKernelGetModel_660
	IMPORT_FUNC	"SysMemForKernel",0xC886B169,sceKernelDevkitVersion_660
	IMPORT_FUNC	"SysMemForKernel",0x83B5226D,sceKernelSetDdrMemoryProtection_660
	IMPORT_FUNC	"SysMemForKernel",0x58148F07,sceKernelCreateHeap_660
	IMPORT_FUNC	"SysMemForKernel",0xDD6512D0,sceKernelDeleteHeap_660
	IMPORT_FUNC	"SysMemForKernel",0x23D81675,sceKernelAllocHeapMemory_660
	IMPORT_FUNC	"SysMemForKernel",0x87C2AB85,sceKernelFreeHeapMemory_660
	IMPORT_FUNC	"SysMemForKernel",0x36C503A9,sceKernelGetSystemStatus_660
	IMPORT_FUNC	"SysMemForKernel",0xC4EEAF20,sceKernelQueryMemoryPartitionInfo_660
	IMPORT_FUNC	"SysMemForKernel",0x13F4A0DE,sceKernelPartitionMaxFreeMemSize_660
	IMPORT_FUNC	"SysMemForKernel",0x0115B0F8,sceKernelPartitionTotalFreeMemSize_660
	IMPORT_FUNC	"SysMemForKernel",0xF12A62F7,sceKernelGetBlockHeadAddr_660
	IMPORT_FUNC	"SysMemForKernel",0x7158CE7E,sceKernelAllocPartitionMemory_660
