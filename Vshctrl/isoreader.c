#include "isoreader.h"
#include <string.h>
#include <pspiofilemgr.h>

//filename
char * filename = NULL;

//set filename
void isoSetFile(char * str)
{
	filename = str;
}

/*
u32 isoFindName(char * str, SceUID iso)
{
	//result
	u32 result = 0;

	//scan buffer
	char buffer[32] __attribute__((align(64)));

	//read data counter
	u32 read = 0;

	//read directory sectors
	while (!result && read < 0x20000) {
		//read buffer
		sceIoRead(iso, buffer, strlen(str));

		//match found
		if(strncmp(buffer, str, strlen(str)) == 0) {
			//seek backwards
			result = (u32)sceIoLseek32(iso, - strlen(str), PSP_SEEK_CUR);
		}

		//no match found yet
		else {
			//increase read counter
			read++;

			//seek backwards
			sceIoLseek32(iso, 1 - strlen(str), PSP_SEEK_CUR);
		}
	}

	//log
	printk("Found Name: %s\n", str);

	//return result
	return result;
}
*/

u32 isoFindName(char * str, SceUID iso)
{
	//result
	u32 result = 0;

	//scan buffer
	static char buffer[2048];

	//read data counter
	u32 read = 0;

	//keep scanning
	while (!result && read < 1000) {
		//read buffer
		sceIoRead(iso, buffer, sizeof(buffer));

		//iterate buffer
		int i = 0; for(; i < sizeof(buffer); i++) {
			//matching string found
			if (i + strlen(str) <= sizeof(buffer) && strncmp(buffer + i, str, strlen(str)) == 0) break;
		}

		//match found
		if (i < sizeof(buffer)) {
			result = read * sizeof(buffer) + i;
		}

		//increase read cycle
		read++;
	}

	//return result
	return result;
}

//get file information
u32 isoGetFileInfo(char * str, u32 * filesize)
{
	//result
	u32 result = 0;

	//get kernel permission
	u32 k1 = pspSdkSetK1(0);

	//open iso file
	SceUID iso = sceIoOpen(filename, PSP_O_RDONLY, 0777);
	if (iso >= 0)
	{
		//skip uninteresting data
		sceIoLseek32(iso, 0xB000, PSP_SEEK_SET);

		//find filename position
		u32 filenamepos = isoFindName(str, iso);

		//filename found
		if (filenamepos) {
			//seek backwards
			sceIoLseek32(iso, 0xB000+filenamepos-31, PSP_SEEK_SET);

			//read lba
			sceIoRead(iso, &result, 4);

			//filesize requested
			if (filesize) {
				//skip big endian lba
				sceIoLseek32(iso, 4, PSP_SEEK_CUR);

				//read filesize
				sceIoRead(iso, filesize, 4);
			}
		}

		//close iso file
		sceIoClose(iso);
	}

	//restore user permission
	pspSdkSetK1(k1);

	//return result
	return result;
}

//read raw data from iso
u32 isoReadRawData(u8 * buffer, u32 lba, u32 offset, u32 size)
{
	//result
	u32 result = 0;

	//get kernel permission
	u32 k1 = pspSdkSetK1(0);

	//open iso file
	SceUID iso = sceIoOpen(filename, PSP_O_RDONLY, 0777);
	if (iso >= 0) {
		//move to sector
		sceIoLseek32(iso, 2048 * lba + offset, PSP_SEEK_SET);

		//read data
		result = sceIoRead(iso, buffer, size);

		//close iso file
		sceIoClose(iso);
	}

	//restore user permission
	pspSdkSetK1(k1);

	//return result
	return result;
}
