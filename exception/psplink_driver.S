	.set noreorder

#include "pspimport.s"

	IMPORT_START "psplink_driver",0x00090000
	IMPORT_FUNC  "psplink_driver",0x670C6041,psplinkPresent
