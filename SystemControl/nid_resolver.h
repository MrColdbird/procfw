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

#ifndef NID_RESOLVER_H
#define NID_RESOLVER_H

#define NID_ENTRY(libname) \
	{ #libname, NELEMS(libname##_nid), libname##_nid, 1, }

typedef struct _nid_entry {
	u32 old;
	u32 new;
} nid_entry;

typedef struct _resolver_config {
	char *name;
	u32 nidcount;
	nid_entry *nidtable;
	u32 enabled;
} resolver_config;

extern resolver_config *nid_fix;
extern u32 nid_fix_size;
extern resolver_config nid_635_fix[];
extern u32 nid_635_fix_size;
extern resolver_config nid_620_fix[];
extern u32 nid_620_fix_size;
extern resolver_config nid_639_fix[];
extern u32 nid_639_fix_size;
extern resolver_config nid_660_fix[];
extern u32 nid_660_fix_size;

resolver_config* get_nid_resolver(const char *libname);
u32 resolve_nid(resolver_config *resolver, u32 nid);
void setup_nid_resolver(void);
void resolve_syscon_driver(SceModule *mod);

#define UNKNOWNNID 0xDEADBEEF

#endif
