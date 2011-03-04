#ifndef NID_DATA_MISSING_H
#define NID_DATA_MISSING_H

MissingNIDEntry missing_SysclibForKernel_entries[] = {
	{ 0x89B79CB1, (u32)ownstrcspn,  },
	{ 0x62AE052F, (u32)ownstrspn,   },
	{ 0x87F8D2DA, (u32)ownstrtok,   },
	{ 0x1AB53A58, (u32)ownstrtok_r, },
	{ 0xD3D1A3B9, (u32)strncat,     },
	{ 0x909C228B, (u32)0x88002E88,  },
	{ 0x18FE80DB, (u32)0x88002EC4,  },
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
