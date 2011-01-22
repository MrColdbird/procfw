#ifndef _ISOREADER_H_
#define _ISOREADER_H_

#include <pspsdk.h>

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

//set filename
void isoSetFile(char * str);

//get file information
u32 isoGetFileInfo(char * str, u32 * filesize);

//read raw data from iso
u32 isoReadRawData(u8 * buffer, u32 lba, u32 offset, u32 size);

#endif
