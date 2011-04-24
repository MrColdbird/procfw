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

static inline void lock() {}
static inline void unlock() {}

static struct IoDirentEntry *g_head = NULL, *g_tail = NULL;

static struct IoDirentEntry *dirent_get_unused(void)
{
	struct IoDirentEntry *entry;

	entry = oe_malloc(sizeof(*entry));

	if(entry == NULL) {
		return entry;
	}

	memset(entry->path, 0, sizeof(entry->path));
	entry->dfd = entry->iso_dfd = -1;
	entry->next = NULL;

	return entry;
}

static int add_magic_dfd(struct IoDirentEntry *slot)
{
	lock();

	if(g_head == NULL) {
		g_head = g_tail = slot;
	} else {
		g_tail->next = slot;
		g_tail = slot;
	}

	unlock();

	return 0;
}

static int remove_magic_dfd(struct IoDirentEntry *slot)
{
	int ret;
	struct IoDirentEntry *fds, *prev;

	lock();

	for(prev = NULL, fds = g_head; fds != NULL; prev = fds, fds = fds->next) {
		if(slot == fds) {
			break;
		}
	}

	if(fds != NULL) {
		if(prev == NULL) {
			g_head = fds->next;

			if(g_head == NULL) {
				g_tail = NULL;
			}
		} else {
			prev->next = fds->next;

			if(g_tail == fds) {
				g_tail = prev;
			}
		}

		oe_free(fds);
		ret = 0;
	} else {
		ret = -1;
	}

	unlock();

	return ret;
}

int dirent_add(SceUID dfd, SceUID iso_dfd, const char *path)
{
	struct IoDirentEntry *p;
   
	p = dirent_get_unused();

	if(p == NULL) {
		return -1;
	}

	p->dfd = dfd;
	p->iso_dfd = iso_dfd;
	STRCPY_S(p->path, path);
	add_magic_dfd(p);

	return 0;
}

void dirent_remove(struct IoDirentEntry *p)
{
	remove_magic_dfd(p);
}

struct IoDirentEntry *dirent_search(SceUID magic)
{
	struct IoDirentEntry *fds;

	if (magic < 0)
		return NULL;

	for(fds = g_head; fds != NULL; fds = fds->next) {
		if(fds->dfd == magic)
			break;
	}

	return fds;
}
