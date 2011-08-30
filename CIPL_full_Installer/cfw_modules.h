

#include "galaxy.h"
#include "inferno.h"

#if _PSP_FW_VERSION == 639
#include "march33.h"
#endif

#if _PSP_FW_VERSION == 635
#include "march33.h"
#endif

#if _PSP_FW_VERSION == 620
#include "march33_620.h"
#endif

#include "popcorn.h"
#include "satelite.h"
#include "stargate.h"
#include "systemctrl.h"
#include "usbdevice.h"
#include "vshctrl.h"
#include "recovery.h"
#include "config.h"

typedef struct Module
{
	void* buffer;
	u32* size;
	char *dst;
} Module;

#define COMMON_MODULES_COUNT 10

static Module common_modules[COMMON_MODULES_COUNT] = 
{
	{ systemctrl, &size_systemctrl, PATH_SYSTEMCTRL, },
	{ vshctrl, &size_vshctrl, PATH_VSHCTRL, },
	{ galaxy, &size_galaxy, PATH_GALAXY, },
	{ stargate, &size_stargate, PATH_STARGATE, },
	{ march33, &size_march33, PATH_MARCH33, },
	{ inferno, &size_inferno, PATH_INFERNO, },
	{ usbdevice, &size_usbdevice, PATH_USBDEVICE, },
	{ popcorn, &size_popcorn, PATH_POPCORN, },
	{ satelite, &size_satelite, PATH_SATELITE, },
	{ recovery, &size_recovery, PATH_RECOVERY, },
};
