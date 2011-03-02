#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pspsdk.h>
#include <pspthreadman_kernel.h>
#include "systemctrl.h"
#include "systemctrl_se.h"
#include "systemctrl_private.h"
#include "printk.h"
#include "utils.h"
#include "strsafe.h"
#include "dirent_track.h"
#include "main.h"

static struct IoDirentEntry g_iodirent[MAX_DIRENT_NUMBER];

struct IoDirentEntry *dirent_get_unused(void)
{
	int i;

	for(i=0; i<MAX_DIRENT_NUMBER; ++i) {
		if(!g_iodirent[i].used) {
			return &g_iodirent[i];
		}
	}

	printk("%s: iodirent full\n", __func__);

	return NULL;
}

void dirent_add(struct IoDirentEntry *p, SceUID dfd, SceUID iso_dfd, const char *path)
{
	if(p < g_iodirent || p >=  g_iodirent + MAX_DIRENT_NUMBER) {
		return;
	}

	p->used = 1;
	p->dfd = dfd;
	p->iso_dfd = iso_dfd;
	STRCPY_S(p->path, path);
//	printk("%s: 0x%08X 0x%08X %s\n", __func__, dfd, iso_dfd, path);
}

void dirent_remove(struct IoDirentEntry *p)
{
	if(p < g_iodirent || p >=  g_iodirent + MAX_DIRENT_NUMBER) {
		return;
	}

	memset(p, 0, sizeof(*p));
}

struct IoDirentEntry *dirent_search(SceUID orig)
{
	int i;
	struct IoDirentEntry *p;

	if(orig < 0) {
		p = NULL;
		goto exit;
	}

	for(i=0; i<MAX_DIRENT_NUMBER; ++i) {
		if(g_iodirent[i].used) {
			if(orig == g_iodirent[i].dfd) {
				p = &g_iodirent[i];
				goto exit;
			}
		}
	}

	p = NULL;

exit:
//	printk("%s: 0x%08X -> 0x%08X\n", __func__, orig, (u32)p);

	return p;
}
