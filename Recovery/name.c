#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <pspsdk.h>
#include <pspdebug.h>
#include <pspkernel.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <psputility.h>

#include "systemctrl.h"
#include "systemctrl_se.h"
#include "vshctrl.h"
#include "utils.h"
#include "vpl.h"
#include "main.h"

const char *get_bool_name(int boolean)
{
	if(boolean) {
		return "ON";
	}

	return "OFF";
}

const char* get_fake_region_name(int fakeregion)
{
	switch(fakeregion) {
		case FAKE_REGION_DISABLED:
			return "Disabled";
		case FAKE_REGION_JAPAN:
			return "Japan";
		case FAKE_REGION_AMERICA:
			return "America";
		case FAKE_REGION_EUROPE:
			return "Europe";
		case FAKE_REGION_KOREA:
			return "Korea";
		case FAKE_REGION_AUSTRALIA:
			return "Australia";
		case FAKE_REGION_HONGKONG:
			return "Hongkong";
		case FAKE_REGION_TAIWAN:
			return "Taiwan";
		case FAKE_REGION_RUSSIA:
			return "Taiwan";
		case FAKE_REGION_CHINA:
			return "China";
	}

	return "FIXME";
}


