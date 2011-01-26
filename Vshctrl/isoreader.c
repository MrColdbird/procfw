#include "isoreader.h"
#include <string.h>
#include <pspiofilemgr.h>
#include <psputilsforkernel.h>
#include "printk.h"
#include "utils.h"
#include "systemctrl_private.h"

#define MAX_RETRIES 8
#define CISO_IDX_BUFFER_SIZE 0x200
#define CISO_DEC_BUFFER_SIZE 0x2000

typedef struct _CISOHeader {
	u8 magic[4];			/* +00 : 'C','I','S','O'                           */
	u32 header_size;
	u64 total_bytes;	/* +08 : number of original data size              */
	u32 block_size;		/* +10 : number of compressed block size           */
	u8 ver;				/* +14 : version 01                                */
	u8 align;			/* +15 : align of index (offset = index[n]<<align) */
	u8 rsv_06[2];		/* +16 : reserved                                  */
} __attribute__ ((packed)) CISOHeader;

static void *g_ciso_dec_buf = NULL;
static u32 g_CISO_idx_cache[CISO_IDX_BUFFER_SIZE/4];
static int g_ciso_dec_buf_offset = -1;
static CISOHeader g_ciso_h;
int g_CISO_cur_idx = 0;

static const char * g_filename = NULL;
static char g_sector_buffer[SECTOR_SIZE] __attribute__((aligned(64)));;
static SceUID g_isofd = -1;
static u32 g_total_sectors = 0;
static u32 g_is_compressed = 0;

static inline u32 isoPos2LBA(u32 pos)
{
	return pos / SECTOR_SIZE;
}

static inline u32 isoLBA2Pos(u32 lba, int offset)
{
	return lba * SECTOR_SIZE + offset;
}

static inline u32 isoPos2OffsetInSector(u32 pos)
{
	return pos & (SECTOR_SIZE - 1);
}

static inline u32 isoPos2RestSize(u32 pos)
{
	return SECTOR_SIZE - isoPos2OffsetInSector(pos);
}

static int reOpen(void)
{
	int retries = MAX_RETRIES, fd = -1;

	sceIoClose(g_isofd);

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

static int ReadRawData(void* addr, u32 size, int offset)
{
	int ret, i;

	for(i=0; i<MAX_RETRIES; ++i) {
		ret = sceIoLseek32(g_isofd, offset, PSP_SEEK_SET);

		if (ret >= 0) {
			break;
		} else {
			printk("%s: got error 0x%08X, reOpening ISO: %s\n", __func__, ret, g_filename);
		}

		sceKernelDelayThread(100000);
	}

	for(i=0; i<MAX_RETRIES; ++i) {
		ret = sceIoRead(g_isofd, addr, size);

		if(ret >= 0) {
			break;
		} else {
			printk("%s: got error 0x%08X, reOpening ISO: %s\n", __func__, ret, g_filename);
		}

		sceKernelDelayThread(100000);
	}

	return ret;
}

static int CSOReadSector(int sector, void *addr)
{
	int ret;
	int n_sector;
	int offset, next_offset;
	int size;

	n_sector = sector - g_CISO_cur_idx;

	// not within sector idx cache?
	if (g_CISO_cur_idx == -1 || n_sector < 0 || n_sector >= NELEMS(g_CISO_idx_cache)) {
		ret = ReadRawData(g_CISO_idx_cache, sizeof(g_CISO_idx_cache), (sector << 2) + sizeof(CISOHeader));

		if (ret < 0) {
			return ret;
		}

		g_CISO_cur_idx = sector;
		n_sector = 0;
	}

	offset = (g_CISO_idx_cache[n_sector] & 0x7FFFFFFF) << g_ciso_h.align;

	// is uncompressed data?
	if (g_CISO_idx_cache[n_sector] & 0x80000000) {
		return ReadRawData(addr, SECTOR_SIZE, offset);
	}

	sector++;
	n_sector = sector - g_CISO_cur_idx;

	if (g_CISO_cur_idx == -1 || n_sector < 0 || n_sector >= NELEMS(g_CISO_idx_cache)) {
		ret = ReadRawData(g_CISO_idx_cache, sizeof(g_CISO_idx_cache), (sector << 2) + sizeof(CISOHeader));

		if (ret < 0) {
			return ret;
		}

		g_CISO_cur_idx = sector;
		n_sector = 0;
	}

	next_offset = (g_CISO_idx_cache[n_sector] & 0x7FFFFFFF) << g_ciso_h.align;
	size = next_offset - offset;
	
	if (size <= SECTOR_SIZE)
		size = SECTOR_SIZE;

	if (offset < g_ciso_dec_buf_offset || size + offset >= g_ciso_dec_buf_offset + CISO_DEC_BUFFER_SIZE) {
		ret = ReadRawData(PTR_ALIGN_64(g_ciso_dec_buf), CISO_DEC_BUFFER_SIZE, offset);

		if (ret < 0) {
			g_ciso_dec_buf_offset = 0xFFF00000;

			return ret;
		}

		g_ciso_dec_buf_offset = offset;
	}

	ret = sceKernelDeflateDecompress(addr, SECTOR_SIZE, PTR_ALIGN_64(g_ciso_dec_buf) + offset - g_ciso_dec_buf_offset, 0);

	if (ret >= 0) {
		return SECTOR_SIZE;
	}

	return ret;
}

static int ReadSector(u32 sector, void *buf)
{
	int ret;
	u32 pos;

	if (g_is_compressed) {
		ret = CSOReadSector(sector, buf);
	} else {
		pos = isoLBA2Pos(sector, 0);
		ret = ReadRawData(buf, SECTOR_SIZE, pos);
	}

	return ret;
}

static int findName(char * str, SceUID iso, u32 *namepos)
{
	u32 read = 0;
	u32 lba_cur = 0x16;
	int ret;

	//keep scanning
	while (read < 1000) {
		ret = ReadSector(lba_cur, g_sector_buffer);

		if (ret < 0) {
			break;
		}

		//iterate buffer
		int i = 0; for(; i < sizeof(g_sector_buffer); i++) {
			//matching string found
			if (i + strlen(str) <= sizeof(g_sector_buffer) && strncmp(g_sector_buffer + i, str, strlen(str)) == 0) break;
		}

		//match found
		if (i < sizeof(g_sector_buffer)) {
			*namepos = read * sizeof(g_sector_buffer) + i;

			return 0;
		}

		//increase read cycle
		read++;
		lba_cur++;
	}

	return -8;
}

int isoOpen(const char *path)
{
	int ret;

	if (g_isofd >= 0) {
		isoClose();
	}

	g_filename = path;

	if (reOpen() < 0) {
		printk("%s: open failed %s -> 0x%08X\n", __func__, g_filename, g_isofd);
		return -2;
	}

	sceIoLseek32(g_isofd, 0, PSP_SEEK_SET);
	memset(&g_ciso_h, 0, sizeof(g_ciso_h));
	ret = sceIoRead(g_isofd, &g_ciso_h, sizeof(g_ciso_h));

	if (ret != sizeof(g_ciso_h)) {
		return -9;
	}

	if (*(u32*)g_ciso_h.magic == 0x4F534943 && g_ciso_h.block_size == SECTOR_SIZE) {
		g_is_compressed = 1;
	} else {
		g_is_compressed = 0;
	}

	if (g_is_compressed) {
		g_total_sectors = g_ciso_h.total_bytes / g_ciso_h.block_size;
		g_CISO_cur_idx = -1;

		if (g_ciso_dec_buf == NULL) {
			g_ciso_dec_buf = oe_malloc(CISO_DEC_BUFFER_SIZE + 64);

			if (g_ciso_dec_buf == NULL) {
				printk("oe_malloc -> 0x%08x\n", (u32)g_ciso_dec_buf);

				return -6;
			}
		}
	} else {
		g_total_sectors = isoGetSize();
	}

	return 0;
}

int isoGetSize(void)
{
	int ret, size;

	ret = sceIoLseek32(g_isofd, 0, PSP_SEEK_CUR);
	size = sceIoLseek32(g_isofd, 0, PSP_SEEK_END);

	sceIoLseek(g_isofd, ret, PSP_SEEK_SET);

	return isoPos2LBA(size);
}

void isoClose(void)
{
	sceIoClose(g_isofd);
	g_isofd = -1;
	g_filename = NULL;

	if (g_ciso_dec_buf != NULL) {
		oe_free(g_ciso_dec_buf);
		g_ciso_dec_buf = NULL;
	}
}

//get file information
int isoGetFileInfo(char * path, u32 *filesize, u32 *lba)
{
	u32 filenamepos = 0;
	u8 tempdata[12];
	int ret;

	ret = findName(path, g_isofd, &filenamepos);

	if (ret < 0 || filenamepos == 0) {
		printk("%s: cannot find %s\n", __func__, path);
		return -5;
	}

	ret = isoRead(tempdata, 0x16, filenamepos-31, sizeof(tempdata));

	if (ret != sizeof(tempdata)) {
		return -6;
	}

	memcpy(lba, &tempdata[0], sizeof(*lba));

	//filesize requested
	if (filesize) {
		memcpy(filesize, &tempdata[8], sizeof(*filesize));
	}

	return 0;
}

int isoReadSectors(u32 sector, void *buf, int count)
{
	int ret, i; for(i=0; i<count; ++i) {
		ret = ReadSector(sector+i, buf + i * SECTOR_SIZE);

		if (ret < 0) {
			return i;
		}
	}

	return count;
}

int isoRead(void *buffer, u32 lba, int offset, u32 size)
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
			break;
		}

		re = MIN(isoPos2RestSize(pos), remaining);
		memcpy(buffer+copied, g_sector_buffer+isoPos2OffsetInSector(pos), re);
		remaining -= re;
		pos += re;
		copied += re;
	}

	return copied;
}
