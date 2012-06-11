/*
 * This file is part of PRO CFW.

 * PRO CFW is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * PRO CFW is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PRO CFW. If not, see <http://www.gnu.org/licenses/ .
 */

#include "systemctrl.h"
#include "systemctrl_se.h"
#include "utils.h"
#include "main.h"

const char * g_messages_en[] = {
	"PRO Recovery Menu",
	"Main Menu",
	"Entering",
	"Exiting",
	"Back",
	"Default",
	"Enabled",
	"Disabled",
	"Toggle USB",
	"USB Enabled",
	"USB Disabled",
	"Configuration",
	"Fake Region",
	"Recovery Font",
	"ISO Mode",
	"Normal",
	"M33 driver",
	"Sony NP9660",
	"Inferno",
	"XMB USB Device",
	"Flash 0",
	"Flash 1",
	"Flash 2",
	"Flash 3",
	"UMD Disc",
	"Charge battery when USB cable is plugged in",
	"Use Slim Color on PSP-1000",
	"Use htmlviewer custom save location",
	"Hide MAC address",
	"Skip Sony Logo at Startup",
	"Skip Game Boot Screen",
	"Hide PIC0.PNG and PIC1.PNG in game menu",
	"Protect flash in USB device mount",
	"Use version.txt in /seplugins",
	"Use usbversion.txt in /seplugins",
	"Use Custom Update Server",
	"Prevent Hibernation Deletion (PSP-Go only)",
	"Advanced",
	"XMB Plugin",
	"Game Plugin",
	"Pops Plugin",
	"NoDRM Engine",
	"Hide CFW Files from game",
	"Block Analog Input in Game",
	"Old Plugin Support (PSP-Go only)",
	"Inferno & NP9660 Use ISO Cache",
	"Inferno & NP9660 Cache Size(in MB)",
	"Inferno & NP9660 Cache Number",
	"Inferno & NP9660 Cache Policy",
	"Allow Non-latin1 ISO Filename",
	"Memory Stick Speedup",
	"None",
	"Pop",
	"Game",
	"Vsh",
	"Pop & Game",
	"Game & Vsh",
	"Vsh & Pop",
	"Always",
	"CPU Speed",
	"XMB CPU/BUS",
	"Game CPU/BUS",
	"Plugins",
	"System storage",
	"Memory stick",
	"Plugins on system storage",
	"Plugins on memory stick",
	"Registry hacks",
	"WMA activated",
	"Flash activated",
	"Buttons swapped",
	"Confirm Button: X",
	"Confirm Button: O",
	"Activate WMA",
	"Activate Flash",
	"Swap O/X buttons",
	"Swap O/X buttons (needs Reset VSH to take effect)",
	"Delete Hibernation",
	"Hibernation deleted",
	"Run /PSP/GAME/RECOVERY/EBOOT.PBP",
	"Shutdown device",
	"Suspend device",
	"Reset device",
	"Reset VSH",
	"Page",
	"Japan",
	"America",
	"Europe",
	"Korea",
	"United Kingdom",
	"Mexico",
	"Australia",
	"Hongkong",
	"Taiwan",
	"Russia",
	"China",
	"Debug Type I",
	"Debug Type II",
	"Force High Memory Layout",
	"Use mac.txt in /seplugins",
};

u8 message_test_en[NELEMS(g_messages_en) == MSG_END ? 0 : -1];
