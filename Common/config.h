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

#ifndef CONFIG_H
#define CONFIG_H

/* PRO Configuration */

/**
 * Please add a '_' before the filename
 * and don't overwrite any OFW files or you will get a brick
 */

#define PATH_FLASH0 "flash0:/"
#define PATH_USBDEVICE PATH_FLASH0 "kd/_usbdevice.prx"
#define PATH_SYSTEMCTRL PATH_FLASH0 "kd/_systemctrl.prx"
#define PATH_VSHCTRL PATH_FLASH0 "kd/_vshctrl.prx"
#define PATH_GALAXY PATH_FLASH0 "kd/_galaxy.prx"
#define PATH_STARGATE PATH_FLASH0 "kd/_stargate.prx"
#define PATH_MARCH33 PATH_FLASH0 "kd/_march33.prx"
#define PATH_INFERNO PATH_FLASH0 "kd/_inferno.prx"
#define PATH_POPCORN PATH_FLASH0 "kd/_popcorn.prx"
#define PATH_SATELITE PATH_FLASH0 "vsh/module/_satelite.prx"
#define PATH_RECOVERY PATH_FLASH0 "vsh/module/_recovery.prx"
#define PATH_FATMS_HELPER PATH_FLASH0 "kd/_fatmshlp.prx"
#define PATH_FATMS_371 PATH_FLASH0 "kd/_fatms371.prx"

#endif
