#ifndef PROBOOTCONF_H
#define PROBOOTCONF_H

typedef enum {
	VSH_MODE = 0,
	MARCH33_MODE = 1,
	NP9660_MODE = 2,
} PspBootConfMode;

#define REBOOTEX_CONFIG_MAGIC 0xC01DB15D

typedef struct _rebootex_config {
	u32 magic;
	int iso_mode;
} rebootex_config;

extern rebootex_config rebootex_conf;

#endif
