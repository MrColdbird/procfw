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

const char * g_messages_it[] = { /* Translated by xFede and sinistro */
	"Recovery Menu PRO",
	"Menu Principale",
	"Entrando",
	"Uscendo",
	"Indietro",
	"Default",
	"Attivato",
	"Disattivato",
	"Connessione USB",
	"Connessione USB Abilitata",
	"Connessione USB Disabilitata",
	"Configurazione",
	"Cambia Regione",
	"Font del Menu Recovery",
	"Modalità ISO",
	"Normale",
	"M33 driver",
	"Sony NP9660",
	"Inferno",
	"Connetti al PC tramite cavo USB",
	"Flash 0",
	"Flash 1",
	"Flash 2",
	"Flash 3",
	"Disco UMD",
	"Carica la batteria quando il cavo USB è inserito",
	"Usa i colori della Slim su una PSP-1000",
	"Usa una destinazione custom per l'htmlviewer",
	"Nascondi l'indirizzo MAC",
	"Salta il logo della Sony all'accensione",
	"Salta il GameBoot",
	"Nascondi PIC0.PNG e PIC1.PNG nel menu Giochi",
	"Proteggi la Flash durante la connessione USB",
	"Usa version.txt in /seplugins",
	"Usa usbversion.txt in /seplugins",
	"Aggiorna tramite il Server PRO",
	"Previeni l'Eliminazione dell'Ibernazione (PSP-Go)",
	"Opzioni Avanzate",
	"Plugins per la XMB",
	"Plugins per i Giochi",
	"Plugins per i Pops",
	"Motore NoDRM",
	"Nascondi i file legati al PRO nel menu Gioco",
	"Blocca lo Stick Analogico nei Giochi",
	"Supporto per i vecchi plugins (PSP-Go)",
	"Inferno & NP9660 Usa ISO Cache",
	"Inferno & NP9660 Dimensione Cache (in MB)",
	"Inferno & NP9660 Numero Cache",
	"Inferno & NP9660 Politica Cache",
	"Permetti Nomi Non-latin1 per le ISO",
	"Velocizza la Memoria Stick",
	"Nessuno",
	"Pop",
	"Giochi",
	"Vsh",
	"Pop & Giochi",
	"Giochi & Vsh",
	"Vsh & Pop",
	"Sempre",
	"Velocità della CPU",
	"CPU/BUS nella XMB",
	"CPU/BUS nei Giochi",
	"Plugins",
	"Memoria Interna (PSP-Go)",
	"Memoria stick",
	"Plugins nella memoria interna (PSP-Go)",
	"Plugins nella memoria stick",
	"Registro degli Hack",
	"Riproduzione WMA Abilitata",
	"Flash Player Abilitato",
	"Tasti Invertiti",
	"Conferma il Tasto: X",
	"Conferma il Tasto: O",
	"Abilita la Riproduzione WMA",
	"Abilita il Flash Player",
	"Inverti i tasti O/X",
	"Inverti i tasti O/X (e necessario Riavviare la VSH)",
	"Elimina ibernazione (PSP-Go)",
	"Ibernazione eliminata",
	"Avvia /PSP/GAME/RECOVERY/EBOOT.PBP",
	"Spegni la Console",
	"Sospendi la Console",
	"Riavvia la Console",
	"Riavvia la VSH",
	"Pagina",
	"Giappone",
	"America",
	"Europa",
	"Corea",
	"Regno Unito",
	"Messico",
	"Australia",
	"Hong Kong",
	"Taiwan",
	"Russia",
	"Cina",
	"Modalità di Debug I",
	"Modalità di Debug II",
};

u8 message_test_it[NELEMS(g_messages_it) == MSG_END ? 0 : -1];
