	.set noreorder

#include "pspstub.s"

	STUB_START "KModule",0x40090000,0x00010005
	STUB_FUNC  0xEDE4C954,get_kirk_header
	STUB_END
