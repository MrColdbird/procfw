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

#ifndef NID_DATA_MISSING_H
#define NID_DATA_MISSING_H

extern int ownsetjmp();
extern void ownlongjmp();

MissingNIDEntry missing_SysclibForKernel_entries[] = {
	{ 0x89B79CB1, (u32)ownstrcspn,  },
	{ 0x62AE052F, (u32)ownstrspn,   },
	{ 0x87F8D2DA, (u32)ownstrtok,   },
	{ 0x1AB53A58, (u32)ownstrtok_r, },
	{ 0xD3D1A3B9, (u32)strncat,     },
	{ 0x909C228B, (u32)&ownsetjmp,  }, // setjmp
	{ 0x18FE80DB, (u32)&ownlongjmp,  }, // longjmp
};

MissingNIDResolver missing_SysclibForKernel = {
	"SysclibForKernel",
	missing_SysclibForKernel_entries,
	NELEMS(missing_SysclibForKernel_entries),
};

/////////////////////////////////////////////////////////////////////////

MissingNIDEntry missing_LoadCoreForKernel_entries[] = {
	{ 0xD8779AC6, 0, },
};

MissingNIDResolver missing_LoadCoreForKernel = {
	"LoadCoreForKernel",
	missing_LoadCoreForKernel_entries,
	NELEMS(missing_LoadCoreForKernel_entries),
};

/////////////////////////////////////////////////////////////////////////

MissingNIDEntry missing_sceSyscon_driver_entries[] = {
	{ 0xC8439C57, 0, }
};

MissingNIDResolver missing_sceSyscon_driver = {
	"sceSyscon_driver",
	missing_sceSyscon_driver_entries,
	NELEMS(missing_sceSyscon_driver_entries),
};

/////////////////////////////////////////////////////////////////////////

MissingNIDResolver *g_missing_resolver[] = {
	&missing_SysclibForKernel,
	&missing_LoadCoreForKernel,
	&missing_sceSyscon_driver,
};

MissingNIDResolver *g_missing_resolver_user[] = {
};

#endif
