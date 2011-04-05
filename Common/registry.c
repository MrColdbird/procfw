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
#include <pspreg.h>

#include "systemctrl.h"
#include "systemctrl_se.h"
#include "utils.h"
#include "main.h"

int sceRegOpenRegistry_ALL(struct RegParam *reg, int mode, REGHANDLE *h)
{
	#ifdef SYSTEMCONTROL
		#ifdef CONFIG_635
			return sceRegOpenRegistry_635(reg, mode, h);
		#endif
		#ifdef CONFIG_620
			return sceRegOpenRegistry_620(reg, mode, h);
		#endif
	#else
		return sceRegOpenRegistry(reg, mode, h);
	#endif
}

int sceRegFlushRegistry_ALL(REGHANDLE h)
{
	#ifdef SYSTEMCONTROL
		#ifdef CONFIG_635
			return sceRegFlushRegistry_635(h);
		#endif
		#ifdef CONFIG_620
			return sceRegFlushRegistry_620(h);
		#endif
	#else
		return sceRegFlushRegistry(h);
	#endif
}

int sceRegCloseRegistry_ALL(REGHANDLE h)
{
	#ifdef SYSTEMCONTROL
		#ifdef CONFIG_635
			return sceRegCloseRegistry_635(h);
		#endif
		#ifdef CONFIG_620
			return sceRegCloseRegistry_620(h);
		#endif
	#else
		return sceRegCloseRegistry(h);
	#endif
}

int sceRegOpenCategory_ALL(REGHANDLE h, const char *name, int mode, REGHANDLE *hd)
{
	#ifdef SYSTEMCONTROL
		#ifdef CONFIG_635
			return sceRegOpenCategory_635(h, name, mode, hd);
		#endif
		#ifdef CONFIG_620
			return sceRegOpenCategory_620(h, name, mode, hd);
		#endif
	#else
		return sceRegOpenCategory(h, name, mode, hd);
	#endif
}

int sceRegCloseCategory_ALL(REGHANDLE hd)
{
	#ifdef SYSTEMCONTROL
		#ifdef CONFIG_635
			return sceRegCloseCategory_635(hd);
		#endif
		#ifdef CONFIG_620
			return sceRegCloseCategory_620(hd);
		#endif
	#else
		return sceRegCloseCategory(hd);
	#endif
}

int sceRegFlushCategory_ALL(REGHANDLE hd)
{
	#ifdef SYSTEMCONTROL
		#ifdef CONFIG_635
			return sceRegFlushCategory_635(hd);
		#endif
		#ifdef CONFIG_620
			return sceRegFlushCategory_620(hd);
		#endif
	#else
		return sceRegFlushCategory(hd);
	#endif
}

int sceRegGetKeyInfo_ALL(REGHANDLE hd, const char *name, REGHANDLE *hk, unsigned int *type, SceSize *size)
{
	#ifdef SYSTEMCONTROL
		#ifdef CONFIG_635
			return sceRegGetKeyInfo_635(hd, name, hk, type, size);
		#endif
		#ifdef CONFIG_620
			return sceRegGetKeyInfo_620(hd, name, hk, type, size);
		#endif
	#else
		return sceRegGetKeyInfo(hd, name, hk, type, size);
	#endif
}

int sceRegGetKeyValue_ALL(REGHANDLE hd, REGHANDLE hk, void *buf, SceSize size)
{
	#ifdef SYSTEMCONTROL
		#ifdef CONFIG_635
			return sceRegGetKeyValue_635(hd, hk, buf, size);
		#endif
		#ifdef CONFIG_620
			return sceRegGetKeyValue_620(hd, hk, buf, size);
		#endif
	#else
		return sceRegGetKeyValue(hd, hk, buf, size);
	#endif
}

int sceRegSetKeyValue_ALL(REGHANDLE hd, const char *name, const void *buf, SceSize size)
{
	#ifdef SYSTEMCONTROL
		#ifdef CONFIG_635
			return sceRegSetKeyValue_635(hd, name, buf, size);
		#endif
		#ifdef CONFIG_620
			return sceRegSetKeyValue_620(hd, name, buf, size);
		#endif
	#else
		return sceRegSetKeyValue(hd, name, buf, size);
	#endif
}

int sceRegCreateKey_ALL(REGHANDLE hd, const char *name, int type, SceSize size)
{
	#ifdef SYSTEMCONTROL
		#ifdef CONFIG_635
			return sceRegCreateKey_635(hd, name, type, size);
		#endif
		#ifdef CONFIG_620
			return sceRegCreateKey_620(hd, name, type, size);
		#endif
	#else
		return sceRegCreateKey(hd, name, type, size);
	#endif
}

int get_registry_value(const char *dir, const char *name, unsigned int *val)
{
    int ret = 0;
    struct RegParam reg;
    REGHANDLE h;

    memset(&reg, 0, sizeof(reg));
    reg.regtype = 1;
    reg.namelen = strlen("/system");
    reg.unk2 = 1;
    reg.unk3 = 1;
    strcpy(reg.name, "/system");
    if(sceRegOpenRegistry_ALL(&reg, 2, &h) == 0)
    {
        REGHANDLE hd;
        if(!sceRegOpenCategory_ALL(h, dir, 2, &hd))
        {
            REGHANDLE hk;
            unsigned int type, size;

            if(!sceRegGetKeyInfo_ALL(hd, name, &hk, &type, &size))
            {
                if(!sceRegGetKeyValue_ALL(hd, hk, val, 4))
                {
                    ret = 1;
                    sceRegFlushCategory_ALL(hd);
                }
            }
            sceRegCloseCategory_ALL(hd);
        }
        sceRegFlushRegistry_ALL(h);
        sceRegCloseRegistry_ALL(h);
    }
    return ret;
}

int set_registry_value(const char *dir, const char *name, unsigned int val)
{
    int ret = 0;
    struct RegParam reg;
    REGHANDLE h;

    memset(&reg, 0, sizeof(reg));
    reg.regtype = 1;
    reg.namelen = strlen("/system");
    reg.unk2 = 1;
    reg.unk3 = 1;
    strcpy(reg.name, "/system");
    if(sceRegOpenRegistry_ALL(&reg, 2, &h) == 0)
    {
        REGHANDLE hd;
        if(!sceRegOpenCategory_ALL(h, dir, 2, &hd))
        {
            if(!sceRegSetKeyValue_ALL(hd, name, &val, 4))
            {
                ret = 1;
                sceRegFlushCategory_ALL(hd);
            }
			else
			{
				sceRegCreateKey_ALL(hd, name, REG_TYPE_INT, 4);
				sceRegSetKeyValue_ALL(hd, name, &val, 4);
				ret = 1;
                sceRegFlushCategory_ALL(hd);
			}
            sceRegCloseCategory_ALL(hd);
        }
        sceRegFlushRegistry_ALL(h);
        sceRegCloseRegistry_ALL(h);
    }

	return ret;
}
