#ifndef _ISOREADER_H_
#define _ISOREADER_H_

#include <pspsdk.h>

#define SECTOR_SIZE 0x800

#define ISO9660_FILEFLAGS_FILE	1
#define ISO9660_FILEFLAGS_DIR	2

typedef struct __attribute__((packed))
{
	/* Directory record length. */
	u8	len_dr;
	/* Extended attribute record length. */
	u8	XARlength;
	/* First logical block where file starts. */
	u32	lsbStart;
	u32	msbStart;
	/* Number of bytes in file. */
	u32	lsbDataLength;
	u32	msbDataLength;
	/* Since 1900. */
	u8	year;
	u8	month;
	u8	day;
	u8	hour;
	u8	minute;
	u8	second;
	/* 15-minute offset from Universal Time. */
	u8	gmtOffse;
	/* Attributes of a file or directory. */
	u8	fileFlags;
	/* Used for interleaved files. */
	u8	interleaveSize;
	/* Used for interleaved files. */
	u8	interleaveSkip;
	/* Which volume in volume set contains this file. */
	u16	lsbVolSetSeqNum;
	u16	msbVolSetSeqNum;
	/* Length of file identifier that follows. */
	u8	len_fi;
	/* File identifier: actual is len_fi. */
	/* Contains extra blank byte if len_fi odd. */
	char    fi;
} Iso9660DirectoryRecord;

int isoOpen(const char *path);

void isoClose(void);

int isoGetTotalSectorSize(void);

//get file information
int isoGetFileInfo(char * str, u32 * filesize, u32 *lba);

//read raw data from iso
int isoRead(void *buffer, u32 lba, int offset, u32 size);

#endif
