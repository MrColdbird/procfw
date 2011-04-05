	.set noreorder

#include "pspimport.s"

// Build List

	IMPORT_START	"sceReg_driver",0x00090011
	IMPORT_FUNC	"sceReg_driver",0x7F17123E,sceRegOpenRegistry_635
	IMPORT_FUNC	"sceReg_driver",0x873EB4E1,sceRegOpenRegistry_620
	IMPORT_FUNC "sceReg_driver",0xBA1A6A86,sceRegOpenCategory_635
	IMPORT_FUNC "sceReg_driver",0x8A0F5133,sceRegOpenCategory_620
	IMPORT_FUNC "sceReg_driver",0xA81437D0,sceRegGetKeyInfo_635
	IMPORT_FUNC "sceReg_driver",0x400BB753,sceRegGetKeyInfo_620
	IMPORT_FUNC "sceReg_driver",0x693B0D7F,sceRegGetKeyValue_635
	IMPORT_FUNC "sceReg_driver",0x08BEB24C,sceRegGetKeyValue_620
	IMPORT_FUNC "sceReg_driver",0x55C9A620,sceRegFlushCategory_635
	IMPORT_FUNC "sceReg_driver",0x0404652C,sceRegFlushCategory_620
	IMPORT_FUNC "sceReg_driver",0xA1671F90,sceRegCloseCategory_635
	IMPORT_FUNC "sceReg_driver",0x21928F15,sceRegCloseCategory_620
	IMPORT_FUNC "sceReg_driver",0xAEF5B913,sceRegFlushRegistry_635
	IMPORT_FUNC "sceReg_driver",0x31F9DB91,sceRegFlushRegistry_620
	IMPORT_FUNC "sceReg_driver",0xA349B88E,sceRegCloseRegistry_635
	IMPORT_FUNC "sceReg_driver",0xF3300EBF,sceRegCloseRegistry_620
	IMPORT_FUNC "sceReg_driver",0x5476A667,sceRegSetKeyValue_635
	IMPORT_FUNC "sceReg_driver",0xDC7EF81C,sceRegSetKeyValue_620
	IMPORT_FUNC "sceReg_driver",0x79C81DCA,sceRegCreateKey_635
	IMPORT_FUNC "sceReg_driver",0x88F1377B,sceRegCreateKey_620
