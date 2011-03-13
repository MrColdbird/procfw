	.set noreorder

#include "pspstub.s"


STUB_START "SystemCtrlForUser",0x40090000,0x00020005 
STUB_FUNC 0x1DDDAD0C, sctrlSESetConfig
STUB_FUNC 0x16C3B7EE, sctrlSEGetConfig
STUB_END 

STUB_START "scePower", 0x40010000, 0x00030005
STUB_FUNC 0x2B7C7CF4, scePowerRequestStandby
STUB_FUNC 0x0442D852, scePowerRequestColdReset
STUB_FUNC 0xAC32C9CC, scePowerRequestSuspend
STUB_END

STUB_START "VshCtrlLib",0x40090000,0x00020005 
STUB_FUNC 0xFD26DA72, vctrlVSHRegisterVshMenu
STUB_FUNC 0xCD6B3913, vctrlVSHExitVSHMenu
STUB_END 

STUB_START "scePaf",0x00010011,0x00070005 
STUB_FUNC 0x49A72E5D, scePaf_strlen
STUB_FUNC 0x43B6A88A, scePaf_memset
STUB_FUNC 0x861C4627, scePaf_memcmp
STUB_FUNC 0x4C386F3C, scePaf_sprintf
STUB_FUNC 0x726776D7, scePaf_snprintf
STUB_FUNC 0xE281261E, scePaf_memcpy
STUB_FUNC 0x8F95CC01, scePaf_strcpy
STUB_END 
