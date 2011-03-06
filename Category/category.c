/*
	Game Categories v 12.0
	Copyright (C) 2009, Bubbletune

	category.c: Maintains linked list with all categories
	
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <pspsdk.h>
#include <pspkernel.h>
#include <systemctrl.h>
#include <string.h>
#include <psprtc.h>
#include <stdio.h>
#include <stdlib.h>

#include "main.h"

int uncategorized;
Category *first_category = NULL;


Category *GetNextCategory(Category *prev)
{
	u64 time = 0, last;
	Category *newest = NULL;

	if (prev)
	{
		last = prev->mtime;
	}
	else
	{
		last = (u64)-1;
	}

	Category *p = (Category *)first_category;

	while (p)
	{
		if (p->mtime < last)
		{
			if (p->mtime > time)
			{
				time = p->mtime;
				newest = p;
			}
		}

		p = p->next;
	}

	return newest;
}

void ClearCategories()
{
	Category *next;
	Category *p = (void *)first_category;

	while (p)
	{
		next = p->next;
		scePaf_free(p);		
		p = next;
	}

	first_category = NULL;
}

int CountCategories()
{
	int i = 0;
	Category *p = (void *)first_category;

	while (p)
	{	
		i++;
		p = p->next;
	}

	return i;
}

void AddCategory(char *category, u64 mtime )
{
	Category *p, *category_entry;

	p = first_category;

	while (p)
	{
		if (strcmp(category, &p->name) == 0)
		{
			return;		
		}
	
		if (p->mtime == mtime)
		{
			mtime++;
			break;
		}

		p = p ->next;
	}

	category_entry = (Category *)scePaf_malloc(sizeof(Category)+scePaf_strlen(category)+1);

	if (category_entry)
	{
		category_entry->next = NULL;
//		category_entry->folder = folder;
		category_entry->mtime = mtime;
		scePaf_strcpy(&category_entry->name, category);

		if (!first_category)
		{
			first_category = category_entry;
		}
		else
		{
			p = (Category *)first_category;
			while (p->next) { p = p->next; }
			p->next = category_entry;
		}
	}

}

void DelCategory(char *category)
{
	Category *prev = NULL;
	Category *p = (Category *)first_category;

	while (p)
	{
		if (strcmp(&p->name, category) == 0)
		{
			if (prev)
			{
				prev->next = p->next;
			}
			else
			{
				first_category = p->next;
			}
			
			break;
		}

		prev = p;
		p = p->next;
	}
}
