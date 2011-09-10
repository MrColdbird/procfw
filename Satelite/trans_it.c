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

#include "common.h"

const char *g_messages_it[] = { /* Translated by xFede */
        "Default",
        "Disattivato",
        "Attivato",
        "Normale",
        "M33 driver",
        "Sony NP9660",
        "Inferno",
        "CPU CLOCK XMB  ",
        "CPU CLOCK GIOCHI ",
        "PERIFERICA USB  ",
        "MODALITÀ ISO  ",
        "VIDEO .ISO",
        "RECOVERY MENU  ->",
        "SPEGNI LA CONSOLE",
        "SOSPENDI LA CONSOLE",
        "RIAVVIA LA CONSOLE",
        "RIAVVIA LA VSH",
        "ESCI",
        "PRO VSH MENU",
        "Flash",
        "Disco UMD",
        "Memoria Stick",
        "Nessuno",
};

u8 message_test[NELEMS(g_messages_it) == MSG_END ? 0 : -1];
