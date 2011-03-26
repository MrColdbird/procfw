#ifndef INFERNO_H
#define INFERNO_H

extern u32 psp_model;
extern u32 psp_fw_version;
extern PspIoDrv g_iodrv;

extern u32 g_umd_error_status;
extern u32 g_drive_status;

extern const char *g_iso_fn;

extern int g_00002480;

extern SceUID g_mediaman_semaid;

#define ISO_SECTOR_SIZE 2048
#define CISO_IDX_BUFFER_SIZE 0x200
#define CISO_DEC_BUFFER_SIZE 0x2000

#define MAX_FILES_NR 8

#endif
