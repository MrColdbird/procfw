	.set noreorder

#include "pspimport.s"

	IMPORT_START "psplink",0x40090000
	IMPORT_FUNC  "psplink",0x670C6041,psplinkPresent
