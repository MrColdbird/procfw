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

const char * g_messages_fr[] = { /* Translated by xFede and Jiiwah */
        "Menu Recovery PRO",
        "Menu Principal",
        "Saisie",
        "Quitte",
        "Retour",
        "Défaut",
        "Activé",
        "Déactivé",
        "Connexion USB",
        "Connexion USB Activé",
        "Connexion USB Déactivé",
        "Configuration",
        "Changer de région",
        "Police du Menu Recovery",
        "Mode ISO",
        "Normal",
        "M33 driver",
        "Sony NP9660",
        "Inferno",
        "Connexion à un PC via un câble USB",
        "Flash 0",
        "Flash 1",
        "Flash 2",
        "Flash 3",
        "Disque UMD",
        "Charger la batterie pendant que le câble USB est inséré",
        "Utilisation de la couleur d'une Slim sur une PSP-1000",
        "Utiliser une destination personnalisée pour htmlviewer",
        "Cacher l'adresse MAC",
        "Passer le logo Sony au démarrage",
        "Passer le GameBoot",
        "Cacher PIC0.PNG et PIC1.PNG dans le menu Jeux",
        "Protéger le Flash lors d'une connexion USB",
        "Utiliser version.txt dans / seplugins",
        "Utiliser usbversion.txt dans / seplugins",
        "Mise à jour via le serveur PRO",
        "Empêcher la suppression de l'hibernation (PSP-Go)",
        "Options avancées",
        "Plugins pour le XMB",
        "Plugins pour les Jeux",
        "Plugins pour the Pops",
        "Moteur NodRm",
        "Cacher les fichiers CFW du menu jeux",
        "Bloquer le stick analogique dans les jeux",
        "Support des vieux plugins (PSP-Go)",
        "Inferno & NP9660 Utiliser ISO Cache",
        "Inferno & NP9660 Taille du cache (en Mb)",
        "Inferno & NP9660 Nombre du cache",
        "Inferno & NP9660 Politique du cache",
        "Permettre les noms de fichiers ISO non-latin1",
        "Accélérer la Memory Stick",
        "Aucun",
        "Pop",
        "Jeux",
        "VSH",
        "Pop & Jeux",
        "Jeux & VSH",
        "VSH & Pop",
        "Toujours",
        "CPU Vitesse",
        "CPU / BUS au XMB",
        "CPU / BUS dans les Jeux",
        "Plugins",
        "Mémoire interne (PSP-Go)",
        "Memory Stick",
        "Plugins dans la mémoire interne (PSP-Go)",
        "Plugins dans la memory stick",
        "Registre Hacks",
        "WMA Activé",
        "Flash Activé",
        "Touches inversées",
        "Confirmez la touche: X",
        "Confirmez la touche: O",
        "Activer la lecture du WMA",
        "Activer le lecteur Flash",
        "Inverser les touches O / X",
        "Inverser les touches O / X (vous devez redémarrer le VSH)",
        "Supprimer Hibernation (PSP-Go)",
        "Hibernation éliminée",
        "Démarrer / PSP / GAME / RECOVERY / EBOOT.PBP",
        "Eteindre la console",
        "Mettre en veille la Console",
        "Redémarrer la console",
        "Redémarrer VSH",
        "Page",
        "Japon",
        "Amérique",
        "Europe",
        "La Corée",
        "Royaume-Uni",
        "Mexique",
        "Australie",
        "Hong Kong",
        "Taïwan",
        "La Russie",
        "La Chine",
        "Le mode Debug I",
        "Le mode Debug II",
};

u8 message_test_fr[NELEMS(g_messages_fr) == MSG_END ? 0 : -1];
