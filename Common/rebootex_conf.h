#ifndef PROBOOTCONF_H
#define PROBOOTCONF_H

enum {
	VSH_RUNLEVEL     =     0x01,
	GAME_RUNLEVEL    =     0x02,
	UPDATER_RUNLEVEL =     0x04,
	POPS_RUNLEVEL    =     0x08,
	APP_RUNLEVEL     =     0x20,
	UMDEMU_RUNLEVEL  =     0x40,
	MLNAPP_RUNLEVEL  =     0x80,
};

typedef enum {
	NORMAL_MODE = 0,
	MARCH33_MODE = 1,
	NP9660_MODE = 2,
} PspBootConfMode;

#define REBOOTEX_CONFIG_MAGIC 0xC01DB15D

typedef struct _rebootex_config {
	u32 magic;
	int iso_mode;
	int bootdevice;
} rebootex_config;

extern rebootex_config rebootex_conf;

#endif
