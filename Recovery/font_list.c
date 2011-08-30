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

#include <pspsdk.h>
#include <pspkernel.h>
#include <stdio.h>
#include <string.h>
#include "vpl.h"
#include "font_list.h"

int fontlist_add(FontList *list, const char *path)
{
	char *newpath;
	FontListEntry *p;

	if(path == NULL)
		return -1;

	newpath = vpl_strdup(path);

	if(newpath == NULL) {
		return -2;
	}

	p = vpl_alloc(sizeof(*p));

	if(p == NULL) {
		vpl_free(newpath);
		return -3;
	}

	list->tail->next = p;
	list->tail = p;
	list->tail->path = newpath;
	list->tail->next = NULL;
	list->count++;

	return 0;
}

static int fontlist_remove(FontList *list, FontListEntry *pdel)
{
	FontListEntry *p, *prev;

	if(list->count == 0) {
		return -1;
	}

	for(prev = &list->head, p = list->head.next; p != NULL; prev = p, p = p->next) {
		if(p == pdel) {
			break;
		}
	}

	if(p == NULL) {
		return -1;
	}

	if(list->tail == pdel) {
		list->tail = prev;
	}
	
	prev->next = NULL;
	vpl_free(pdel->path);
	vpl_free(pdel);
	list->count--;

	return 0;
}

char *fontlist_get(FontList *list, size_t n)
{
	FontListEntry *p;

	for(p=list->head.next; p != NULL && n != 0; p=p->next, n--) {
	}

	if(p == NULL) {
		return NULL;
	}

	return p->path;
}

size_t fontlist_count(FontList *list)
{
	return list->count;
}

void fontlist_clear(FontList *list)
{
	while(list->tail != &list->head) {
		fontlist_remove(list, list->tail);
	}
}

int fontlist_find(FontList *list, const char *search)
{
	FontListEntry *p;
	int i;

	for(i=0, p=list->head.next; p != NULL; p=p->next, ++i) {
		if(0 == strcasecmp(p->path, search)) {
			break;
		}
	}

	if(p == NULL) {
		return -1;
	}

	return i;
}

void fontlist_init(FontList *list)
{
	list->head.path = NULL;
	list->head.next = NULL;
	list->tail = &list->head;
	list->count = 0;
}

char g_cur_font_select[256];

int load_recovery_font_select(void)
{
	SceUID fd;

	g_cur_font_select[0] = '\0';
	fd = sceIoOpen("ef0:/seplugins/font_recovery.txt", PSP_O_RDONLY, 0777);

	if(fd < 0) {
		fd = sceIoOpen("ms0:/seplugins/font_recovery.txt", PSP_O_RDONLY, 0777);

		if(fd < 0) {
			return fd;
		}
	}

	sceIoRead(fd, g_cur_font_select, sizeof(g_cur_font_select));
	sceIoClose(fd);

	return 0;
}

int save_recovery_font_select(void)
{
	SceUID fd;

	fd = sceIoOpen("ef0:/seplugins/font_recovery.txt", PSP_O_WRONLY | PSP_O_TRUNC | PSP_O_CREAT, 0777);

	if(fd < 0) {
		fd = sceIoOpen("ms0:/seplugins/font_recovery.txt",  PSP_O_WRONLY | PSP_O_TRUNC | PSP_O_CREAT, 0777);

		if(fd < 0) {
			return fd;
		}
	}

	sceIoWrite(fd, g_cur_font_select, strlen(g_cur_font_select));
	sceIoClose(fd);

	return 0;
}
