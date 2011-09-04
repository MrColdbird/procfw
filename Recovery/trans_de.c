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

const char * g_messages_de[] = {
	"PRO Recovery Men",
	"Hauptmen",
	"Entering",
	"Exiting",
	"Zurck",
	"Standard",
	"Aktiviert",
	"Deaktiviert",
	"Toggle USB",
	"USB Aktiviert",
	"USB Deaktiviert",
	"Einstellungen",
	"Region wechseln",
	"Recovery Schriftart",
	"ISO Modus",
	"Normal",
	"M33 Treiber",
	"Sony NP9660",
	"Inferno",
	"XMB USB Ger„t",
	"Flash 0",
	"Flash 1",
	"Flash 2",
	"Flash 3",
	"UMD Disc",
	"Lade Battery wenn USB Kabel eingesteckt ist",
	"Aktiviere Slimline Farben fr 1g",
	"Aktiviere alternative Webbrowser Speicherorte",
	"Verstecke MAC Adresse",
	"šberspringe Sony Bootlogo",
	"šberspringe Gameboot Video",
	"Verstecke PIC0.PNG und PIC1.PNG im Spiele Men",
	"Schtze Flash-Speicher im USB Modus",
	"Verwende version.txt in /seplugins",
	"Verwende usbversion.txt in /seplugins",
	"Verwende PRO Update Server",
	"Verhindere Spielpausendaten-L”schung",
	"Erweitert",
	"XMB Plugin",
	"Game Plugin",
	"Pops Plugin",
	"NoDRM Engine",
	"Verstecke CFW Dateien vor Spielmodulen (PSP Go)",
	"Deaktiviere Analog-Stick in Spielen",
	"ef0 zu ms0 Umleitung fr Plugins (PSP Go)",
	"Inferno & NP9660 Verwende ISO Cache",
	"Inferno & NP9660 Cache Gr”áe (in MB)",
	"Inferno & NP9660 Cache Nummer",
	"Inferno & NP9660 Cache Regelsatz",
	"Erlaube Non-latin1 ISO Dateinamen",
	"Memory Stick Beschleunigung",
	"Keine",
	"Pop",
	"Game",
	"Vsh",
	"Pop & Game",
	"Game & Vsh",
	"Vsh & Pop",
	"Immer",
	"CPU Geschwindigkeit",
	"XMB CPU/BUS",
	"Game CPU/BUS",
	"Plugins",
	"Systemspeicher",
	"Memory stick",
	"Plugins am Systemspeicher",
	"Plugins am Memory Stick",
	"Registry hacks",
	"WMA aktiviert",
	"Flash aktiviert",
	"Buttonbelegung vertauscht",
	"Best„tigungs-Button: X",
	"Best„tigungs-Button: O",
	"Aktiviere WMA",
	"Aktiviere Flash",
	"Vertausche O/X buttons",
	"Vertausche O/X buttons (needs Reset VSH to take effect)",
	"L”sche Pausierungsdaten",
	"Pausierungsdaten gel”scht",
	"Starte /PSP/GAME/RECOVERY/EBOOT.PBP",
	"Ger„t abschalten",
	"Ger„t pausieren",
	"Ger„t neu starten",
	"VSH neu starten",
	"Seite",
	"Japan",
	"Amerika",
	"Europa",
	"Korea",
	"England",
	"Mexiko",
	"Australien",
	"Hongkong",
	"Taiwan",
	"Russland",
	"China",
	"Debug Typ I",
	"Debug Typ II",
};

u8 message_test_de[NELEMS(g_messages_de) == MSG_END ? 0 : -1];
