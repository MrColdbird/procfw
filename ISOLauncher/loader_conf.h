#ifndef LOADER_CONF_H
#define LOADER_CONF_H

enum IsoMode {
	USE_NP9660_MODE = 2,
	USE_M33_MODE = 1,
};

struct PLoaderConf {
	enum IsoMode iso_mode;
	char iso_path[128];
	char prx_path[128];
	u32 fw_version;
	u32 psp_model;
};

extern struct PLoaderConf g_conf;

#endif
