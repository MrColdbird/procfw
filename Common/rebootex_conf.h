#ifndef PROBOOTCONF_H
#define PROBOOTCONF_H

#define REBOOTEX_CONFIG 0x88FB0000
#define REBOOTEX_CONFIG_MAGIC 0xC01DB15D
#define REBOOTEX_CONFIG_ISO_PATH (REBOOTEX_CONFIG + 0x100)

#define REBOOT_START 0x88600000
#define REBOOTEX_START 0x88FC0000
#define BTCNF_MAGIC 0x0F803001
#define BOOTCONFIG_TEMP_BUFFER 0x88FB0200

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

typedef struct _rebootex_config {
	u32 magic;
	u32 psp_model;
	u32 rebootex_size;
	u32 p2_size;
	u32 p9_size;
	char *insert_module_before;
	void *insert_module_binary;
	u32 insert_module_size;
	u32 insert_module_flags;
	u32 psp_fw_version;
	int iso_mode;
} rebootex_config;

extern rebootex_config rebootex_conf;

#endif
