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

#ifndef STARGATE_H
#define STARGATE_H

extern SEConfig conf;
extern u32 psp_model;
extern u32 psp_fw_version;

int myPauth_98B83B5D(u8 *p, u32 size, u32 *newsize, u8 *xor_key);
int myPauth_init(void);

void patch_sceMesgLed(void);

int nodrm_init(void);
void patch_drm_imports(SceModule *mod);
void patch_analog_imports(SceModule *mod);
void patch_utility(SceModule *mod);
int nodrm_get_normal_functions(void);
int nodrm_get_npdrm_functions(void);
int load_module_get_function(void);
void patch_load_module(SceModule *mod);
void hide_cfw_folder(SceModule *mod);
void module_blacklist(SceModule *mod);

extern int (*mesgled_decrypt)(u32 *tag, u8 *key, u32 code, u8 *prx, u32 size, u32 *newsize, u32 use_polling, u8 *blacklist, u32 blacklistsize, u32 type, u8 *xor_key1, u8 *xor_key2);

void patch_for_old_fw(SceModule *mod);
void get_functions_for_old_fw(void);

void patch_IsoDrivers(void);

#endif
