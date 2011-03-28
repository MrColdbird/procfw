#ifndef INFERNO_H
#define INFERNO_H

#include "inferno_patch_offset.h"

#define ISO_SECTOR_SIZE 2048
#define CISO_IDX_BUFFER_SIZE 0x200
#define CISO_DEC_BUFFER_SIZE 0x2000

#define MAX_FILES_NR 8

#define SAFE_FREE(p) \
do { \
	if(p != NULL) { \
		oe_free(p); \
		p = NULL; \
	} \
} while ( 0 )

struct IoReadArg {
	u32 offset; // 0
	u8 *address; // 4
	u32 size; // 8
};

extern u32 psp_model;
extern u32 psp_fw_version;
extern PspIoDrv g_iodrv;

extern SceUID g_umd_cbid;
extern int g_umd_error_status;
extern int g_drive_status;

extern const char *g_iso_fn;
extern int g_game_fix_type;
extern SceUID g_drive_status_evf;
extern void *g_sector_buf;
extern SceUID g_umd9660_sema_id;
extern int g_iso_opened;
extern SceUID g_iso_fd;
extern int g_total_sectors;
extern struct IoReadArg g_read_arg;

extern int iso_open(void);
extern int iso_read(struct IoReadArg *args);
extern int iso_read_with_stack(u32 offset, void *ptr, u32 data_len);

#endif
