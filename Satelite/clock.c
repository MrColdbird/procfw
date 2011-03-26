/*
	PSP VSH
*/
#include "common.h"
#include "utils.h"

extern SEConfig cnf;

const int cpu_list[]={0, 20, 75, 100, 133, 222, 266, 300, 333};
const int bus_list[]={0, 10, 37, 50, 66, 111, 133, 150, 166};

int cpu2no(int cpu)
{
	int i;

	for(i=0; i<NELEMS(cpu_list); i++) {
		if(cpu==cpu_list[i])
			return i;
	}

	return 0;
}

int bus2no(int cpu)
{
	int i;

	for(i=0; i<NELEMS(bus_list); i++) {
		if(cpu==bus_list[i])
			return i;
	}

	return 0;
}

void change_clock(int dir, int flag)
{
	int sel;
	int *cpu[2];

	if(flag) {
		cpu[0]=&(cnf.umdisocpuspeed);
		cpu[1]=&(cnf.umdisobusspeed);
	} else {
		cpu[0]=&(cnf.vshcpuspeed);
		cpu[1]=&(cnf.vshbusspeed);
	}

	sel = cpu2no(*cpu[0]);

	// select new
	sel = limit(sel+dir, 0, NELEMS(cpu_list)-1);

	*cpu[0] = cpu_list[sel];
	*cpu[1] = bus_list[sel];
}

void change_usb(int dir)
{
	int sel = cnf.usbdevice;

	// select new
	sel = limit(sel+dir, 0, 5);
	
	cnf.usbdevice=sel;
}

void change_umd_mode(int dir)
{
	int sel = cnf.umdmode;

	// select new
	sel = limit(sel+dir, 0, 3);
	cnf.umdmode=sel;
}

void change_region(int dir, int max)
{
	int sel = cnf.fakeregion;

	// select new
	sel = limit(sel+dir, 0, max);
	cnf.fakeregion=sel;
}

void change_plugins(int dir, int flag)
{
	int sel;
	int *plugins;

	if(flag == 0) {
		plugins=&(cnf.plugvsh);
	} else if(flag == 1) {
		plugins=&(cnf.pluggame);
	} else {
		plugins=&(cnf.plugpop);
	}

	sel = *plugins;
	sel = !sel;
	*plugins = sel;
}

void change_bool_option(int *p, int direction)
{
	int sel = *p;

	sel = limit(sel+direction, 0, 1);
	*p=sel;
}
