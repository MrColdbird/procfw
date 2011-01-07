	.set noreorder

#include "pspimport.s"

	IMPORT_START	"IoFileMgrForKernel",0x00090000
	IMPORT_FUNC	"IoFileMgrForKernel",0x810C4BC3,sceIoClose
	IMPORT_FUNC	"IoFileMgrForKernel",0x109F50BC,sceIoOpen
	IMPORT_FUNC	"IoFileMgrForKernel",0x6A638D83,sceIoRead
	IMPORT_FUNC	"IoFileMgrForKernel",0x42EC03AC,sceIoWrite
	IMPORT_FUNC	"IoFileMgrForKernel",0x27EB27B8,sceIoLseek
