	.set noreorder

#include "pspstub.s"

	STUB_START "pspDecrypt",0x40090000,0x00010005
	STUB_FUNC  0xB081FD37,pspDecryptPRX
	STUB_END
