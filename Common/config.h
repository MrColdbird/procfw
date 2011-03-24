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
#define PATH_POPCORN PATH_FLASH0 "kd/_popcorn.prx"
#define PATH_SATELITE PATH_FLASH0 "vsh/module/_satelite.prx"
#define PATH_RECOVERY PATH_FLASH0 "vsh/module/_recovery.prx"

#endif
