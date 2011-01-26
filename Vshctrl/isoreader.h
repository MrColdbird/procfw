#ifndef _ISOREADER_H_
#define _ISOREADER_H_

#include <pspsdk.h>

#define SECTOR_SIZE 0x800

#if 0
//directory structure
typedef struct {
	u8 len_dr;
	u8 len_ex;
	u32 evpos_le;
	u32 evpos_be;
	u32 datalen_le;
	u32 datalen_be;
	u8 recording[6];
	u8 fileflags;
	u8 fileunitsize;
	u8 interleavegap;
	u32 volseqnum;
	u8 fileidentlen;
	//fileidentifier
};
#endif

int isoOpen(const char *path);

void isoClose(void);

int isoGetSize(void);

//get file information
int isoGetFileInfo(char * str, u32 * filesize, u32 *lba);

int isoReadSectors(u32 sector, void *buf, int count);

//read raw data from iso
int isoRead(void *buffer, u32 lba, int offset, u32 size);

#endif
