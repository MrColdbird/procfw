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

const char * g_messages_es[] = { /* Translated by xFede */
	"Recovery Menú PRO",
	"Menu Principal",
	"Entrando",
	"Saliendo",
	"Atrás",
	"Default", /* TODO: Por Defecto */
	"Activado",
	"Desactivado",
	"Conexión USB",
	"Conexión USB  Habilitada",
	"Conexión USB Deshabilitada",
	"Configuración",
	"Cambiar Región",
	"Font del Recovery Menú",
	"Modalidad ISO",
	"Normal",
	"M33 driver",
	"Sony NP9660",
	"Inferno",
	"Conecta a PC mediante un cable USB",
	"Flash 0",
	"Flash 1",
	"Flash 2",
	"Flash 3",
	"Disco UMD",
	"Cargar PSP mientras el cable USB esta conectado",
	"Utiliza los colores de una Slim en una PSP-1000",
	"Utiliza una ubicación custom para el htmlviewer",
	"Ocultar la dirección MAC",
	"Obvia el logo de la Sony al encender la PSP",
	"Obvia el GameBoot",
	"Oculta PIC0.PNG y PIC1.PNG en el menú Juego",
	"Proteger la Flash durante la conexión USB",
	"Utiliza version.txt en /seplugins",
	"Utiliza usbversion.txt en /seplugins",
	"Actualiza atraves el Servidor PRO",
	"Evita la Eliminación de la Hibernación (PSP-Go)",
	"Opciones Avanzadas",
	"Plugins para la XMB",
	"Plugins para los Juegos",
	"Plugins para los Pops",
	"Motor NoDRM",
	"Oculta los file relacionados al CFW nel menú Juego",
	"Bloquea el Stick Analógico en los Juegos",
	"Soporte para los viejos plugin (PSP-Go)",
	"Inferno & NP9660 Usa ISO Cache",
	"Inferno & NP9660 Tamaño de la Cache (en MB)",
	"Inferno & NP9660 Numero de  la Cache",
	"Inferno & NP9660 Política de la Cache",
	"Permite los Nombres Non-latín1 para las ISO",
	"Acelera la Memoria Stick",
	"Ninguno",
	"Pop",
	"Juegos",
	"Vsh",
	"Pop & Juegos",
	"Juegos & Vsh",
	"Vsh & Pop",
	"Siempre",
	"Velocidad de la CPU",
	"CPU/BUS en la XMB",
	"CPU/BUS en los Juegos",
	"Plugins",
	"Memoria Interna (PSP-Go)",
	"Memoria Stick",
	"Plugins en la memoria interna (PSP-Go)",
	"Plugins en la memoria stick",
	"Registro de los Hack",
	"Reproducción WMA Habilitada",
	"Flash Player Habilitado",
	"Tastos Invertidos",
	"Confirma el Tasto: X",
	"Confirma el Tasto: O",
	"Habilita la Reproducción WMA",
	"Habilita el Flash Player",
	"Invertí los tastos O/X",
	"Invertí los tastos O/X (es necesario Reiniciar la VSH)",
	"Elimina Hibernación (PSP-Go)",
	"Hibernación eliminada",
	"Ejecuta /PSP/GAME/RECOVERY/EBOOT.PBP",
	"Apaga la Consola",
	"Suspende la Consola",
	"Reinicia la Consola",
	"Reinicia la VSH",
	"Pagina",
	"Japón",
	"América",
	"Europa",
	"Corea",
	"Reino Unido",
	"Mexico",
	"Australia",
	"Hong Kong",
	"Taiwan",
	"Rusia",
	"China",
	"Modalidad de Debug I",
	"Modalidad de Debug II",
};

u8 message_test_es[NELEMS(g_messages_es) == MSG_END ? 0 : -1];
