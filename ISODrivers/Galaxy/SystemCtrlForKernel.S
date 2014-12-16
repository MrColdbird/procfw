	.set noreorder

#include "pspimport.s"

	IMPORT_START "SystemCtrlForKernel",0x00090000
	IMPORT_FUNC  "SystemCtrlForKernel",0x1E5436EE,oe_mallocinit
	IMPORT_FUNC  "SystemCtrlForKernel",0xF9584CAD,oe_malloc
	IMPORT_FUNC  "SystemCtrlForKernel",0xA65E8BC4,oe_free
	IMPORT_FUNC  "SystemCtrlForKernel",0xE34A0D97,oe_mallocterminate
	IMPORT_FUNC  "SystemCtrlForKernel",0xBA21998E,sctrlSEGetUmdFile
	IMPORT_FUNC  "SystemCtrlForKernel",0x16100529,LZ4_decompress_fast
