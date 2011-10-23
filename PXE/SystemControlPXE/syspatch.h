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

#pragma once

extern u8 *rebootex;
extern u32 rebootex_size;

extern char *g_insert_module_after;
extern void *g_insert_module_binary;
extern int g_insert_module_size;
extern int g_insert_module_flags;

int on_module_start(SceModule2 *mod);
void patch_sceLoadCore(void);
void patch_sceLoadExec(void);
int load_rebootex_from_ms0(void);
void patch_sceInterruptManager(void);
void patch_partitions(void);
