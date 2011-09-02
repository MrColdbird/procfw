	.set noreorder

#include "pspstub.s"

STUB_START "scePower", 0x40010000, 0x00040005
STUB_FUNC 0x2B7C7CF4, scePowerRequestStandby
STUB_FUNC 0x0442D852, scePowerRequestColdReset
STUB_FUNC 0xAC32C9CC, scePowerRequestSuspend
STUB_FUNC 0xEFD3C963, scePowerTick
STUB_END

STUB_START "VshCtrlLib",0x40090000,0x00020005
STUB_FUNC  0xC1BE3884,vctrlVSHUpdateConfig
STUB_FUNC  0x36197FB3,vshCtrlDeleteHibernation
STUB_END
