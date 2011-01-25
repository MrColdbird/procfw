#include "isoreader.h"
#include <string.h>
#include <pspiofilemgr.h>
#include "printk.h"
#include "utils.h"

#define MAX_RETRIES 8

static const char * g_filename = NULL;
static char g_buffer[SECTOR_SIZE] __attribute__((aligned(64)));;
static SceUID g_isofd = -1;
static u32 g_total_sector = 0;
static u32 g_is_cso = 0;

static inline u32 isoPos2LBA(u32 pos)
{
	return pos / SECTOR_SIZE;
}

static inline u32 isoLBA2Pos(u32 lba, u32 offset)
{
	return lba * SECTOR_SIZE + offset;
}

static inline u32 isoPos2RestSize(u32 pos)
{
	return (isoPos2LBA(pos) + 1) * SECTOR_SIZE - pos;
}

static int reOpen(void)
{
	int retries = MAX_RETRIES, fd = -1;

	while(retries -- > 0) {
		fd = sceIoOpen(g_filename, PSP_O_RDONLY, 0777);

		if (fd >= 0) {
			break;
		}

		sceKernelDelayThread(100000);
	}

	if (fd >= 0) {
		g_isofd = fd;
	}

	return fd;
}

int isoOpen(const char *path)
{
	if (g_isofd >= 0) {
		isoClose();
	}

	g_filename = path;

	if (reOpen() < 0) {
		printk("%s: open failed %s -> 0x%08X\n", __func__, g_filename, g_isofd);
		return -2;
	}

	g_total_sector = isoGetSize();
	g_is_cso = 0;

	return 0;
}

int isoGetSize(void)
{
	int ret, size;

	ret = sceIoLseek32(g_isofd, 0, PSP_SEEK_CUR);
	size = sceIoLseek32(g_isofd, 0, PSP_SEEK_END);

	sceIoLseek32(g_isofd, ret, PSP_SEEK_SET);

	return isoPos2LBA(size);
}

void isoClose(void)
{
	sceIoClose(g_isofd);
	g_isofd = -1;
	g_filename = NULL;
}

int isoLseek(u32 lba, u32 offset)
{
	SceOff off;
	u32 pos;

	pos = isoLBA2Pos(lba, offset);
	off = sceIoLseek32(g_isofd, pos, PSP_SEEK_SET);

	if (off < 0) {
		printk("%s: 0x%08X %s -> 0x%08X\n", __func__, g_isofd, g_filename, (u32)off);

		if (reOpen() >= 0) {
			off = sceIoLseek32(g_isofd, pos, PSP_SEEK_SET);
		}

		return -3;
	}

	return 0;
}

u32 isoFindName(char * str, SceUID iso)
{
	//result
	u32 result = 0;

	//read data counter
	u32 read = 0;

	//keep scanning
	while (!result && read < 1000) {
		sceIoRead(iso, g_buffer, sizeof(g_buffer));

		//iterate buffer
		int i = 0; for(; i < sizeof(g_buffer); i++) {
			//matching string found
			if (i + strlen(str) <= sizeof(g_buffer) && strncmp(g_buffer + i, str, strlen(str)) == 0) break;
		}

		//match found
		if (i < sizeof(g_buffer)) {
			result = read * sizeof(g_buffer) + i;
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
	u32 result = 0;

	//skip uninteresting data
	isoLseek(0x16, 0);

	u32 filenamepos = isoFindName(str, g_isofd);

	if (filenamepos) {
		//seek backwards
		isoLseek(0x16, filenamepos-31);

		//read lba
		sceIoRead(g_isofd, &result, 4);

		//filesize requested
		if (filesize) {
			//skip big endian lba
			sceIoLseek32(g_isofd, 4, PSP_SEEK_CUR);

			//read filesize
			sceIoRead(g_isofd, filesize, 4);
		}
	}

	//return result
	return result;
}

static int ReadSector(u32 sector, u8 *buf)
{
	int ret;

	isoLseek(sector, 0);
	ret = sceIoRead(g_isofd, buf, SECTOR_SIZE);

	if (ret != SECTOR_SIZE) {
		return -4;
	}

	return 0;
}

int isoReadSectors(u32 sector, u8 *buf, int count)
{
	int ret, i; for(i=0; i<count; ++i) {
		ret = ReadSector(sector+i, buf + i * SECTOR_SIZE);

		if (ret < 0) {
			return ret;
		}
	}

	return count;
}

int isoRead(void *buf, int size)
{
	int ret;

	ret = sceIoRead(g_isofd, buf, size);

	if (ret < 0) {
		printk("%s: 0x%08X %s -> 0x%08X\n", __func__, g_isofd, g_filename, ret);
		
		if (reOpen() >= 0) {
			ret = sceIoRead(g_isofd, buf, size);
		}
	}

	return ret;
}

#if 0
static u8 g_sector_buffer[SECTOR_SIZE] __attribute__((aligned(64)));

//read raw data from iso
int isoReadRawData(u8 *buffer, u32 lba, u32 offset, u32 size)
{
	u32 remaining;
	u32 pos, copied;
	u32 re;
	int ret;

	remaining = size;
	pos = isoLBA2Pos(lba, offset);
	copied = 0;

	while(remaining > 0) {
		ret = ReadSector(isoPos2LBA(pos), g_sector_buffer);

		if (ret < 0) {
			return ret;
		}

		re = MIN(isoPos2RestSize(pos), remaining);
		memcpy(buffer+copied, g_sector_buffer+offset, re);
		remaining -= re;
		pos += re;
		copied += re;
	}

	//return result
	return copied;
}
#else

//read raw data from iso
int isoReadRawData(u8 *buffer, u32 lba, u32 offset, u32 size)
{
	int ret;

	isoLseek(lba, offset);
	ret = isoRead(buffer, size);

	return ret;
}
#endif
