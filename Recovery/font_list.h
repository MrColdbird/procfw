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

#ifndef FONTLIST_H
#define FONTLIST_H

typedef struct _FontListEntry {
	char *path;
	struct _FontListEntry *next;
} FontListEntry;

typedef struct _FontList {
	FontListEntry head, *tail;
	size_t count;
} FontList;

int fontlist_add(FontList *list, const char *path);
char *fontlist_get(FontList *list, size_t n);
size_t fontlist_count(FontList *list);
void fontlist_clear(FontList *list);
int fontlist_find(FontList *list, const char *search);
void fontlist_init(FontList *list);

extern char g_cur_font_select[256];
int load_recovery_font_select(void);
int save_recovery_font_select(void);

#endif
