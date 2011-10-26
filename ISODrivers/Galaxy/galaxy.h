#ifndef _GALAXY_H
#define _GALAXY_H

struct IoReadArg {
	u32 offset; // 0
	u8 *address; // 4
	u32 size; // 8
};

extern int g_total_blocks;

#define ISO_SECTOR_SIZE 0x800

int iso_read(struct IoReadArg *args);
int iso_cache_read(struct IoReadArg *args);

extern u32 psp_fw_version;
extern u32 psp_model;

int infernoCacheInit(int cache_size, int cache_num);
void infernoCacheSetPolicy(int policy);

#endif
