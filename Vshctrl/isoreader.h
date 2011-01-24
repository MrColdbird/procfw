#ifndef _ISOREADER_H_
#define _ISOREADER_H_

#include <pspsdk.h>

#define SECTOR_SIZE 0x800

//set filename
void isoSetFile(char * str);

//get file information
u32 isoGetFileInfo(char * str, u32 * filesize);

//read raw data from iso
u32 isoReadRawData(u8 * buffer, u32 lba, u32 offset, u32 size);

#endif
