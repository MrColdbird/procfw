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

static const int cpu_list[]={0, 20, 75, 100, 133, 222, 266, 300, 333};
static const int bus_list[]={0, 10, 37, 50, 66, 111, 133, 150, 166};

const char *get_bool_name(int boolean)
{
	if(boolean) {
		return "Enabled";
	}

	return "Disabled";
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

const char *get_iso_name(int iso_mode)
{
	switch(iso_mode) {
		case MODE_UMD:
			return "Normal";
		case MODE_MARCH33:
			return "M33 driver";
		case MODE_NP9660:
			return "Sony NP9660";
	}

	return "FIXME";
}

int get_cpu_number(int cpu)
{
	int i;

	for(i=0; i<NELEMS(cpu_list); i++) {
		if(cpu==cpu_list[i])
			return i;
	}

	return 0;
}

int get_bus_number(int cpu)
{
	int i;

	for(i=0; i<NELEMS(bus_list); i++) {
		if(cpu==bus_list[i])
			return i;
	}

	return 0;
}

int get_cpu_freq(int number)
{
	if(number >= 0 && number < NELEMS(cpu_list)) {
		return cpu_list[number];
	}

	return 0;
}

int get_bus_freq(int number)
{
	if(number >= 0 && number < NELEMS(bus_list)) {
		return bus_list[number];
	}

	return 0;
}

const char *get_plugin_name(int type)
{
	switch(type) {
		case TYPE_VSH:
			return "VSH";
		case TYPE_GAME:
			return "GAME";
		case TYPE_POPS:
			return "POPS";
	}

	return "FIXME";
}
