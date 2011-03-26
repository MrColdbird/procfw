#include <pspkernel.h>
#include <pspreg.h>
#include <stdio.h>
#include <string.h>
#include <systemctrl.h>
#include <systemctrl_se.h>
#include <pspsysmem_kernel.h>
#include <psprtc.h>
#include <psputilsforkernel.h>
#include <pspthreadman_kernel.h>
#include "utils.h"
#include "printk.h"
#include "libs.h"
#include "utils.h"
#include "systemctrl.h"
#include "systemctrl_se.h"
#include "systemctrl_private.h"
#include "inferno.h"

struct IsoOpenSlot {
	int enabled;
	u32 offset;
};

struct IoIoctlSeekCmd {
	u64 offset;
	u32 unk;
	u32 whence;
};

#define SAFE_FREE(p) \
do { \
	if(p != NULL) { \
		oe_free(p); \
		p = NULL; \
	} \
} while ( 0 )

// 0x00002484
void *g_sector_buf = NULL;

// 0x00002740
SceUID g_umd9660_sema_id = -1;

// 0x0000248C
int g_iso_opened = 0;

// 0x000023D0
SceUID g_iso_fd = -1;

// 0x00002488
int g_is_ciso = 0;

// 0x000023D4
int g_total_sectors = -1;

// 0x000024C0
static void *g_ciso_block_buf = NULL;

// 0x000024C4, size CISO_DEC_BUFFER_SIZE, align 64
static void *g_ciso_dec_buf = NULL;

// 0x00002704
static int g_CISO_cur_idx = 0;

// 0x00002700
static int g_ciso_dec_buf_offset = -1;

// 0x00002720
static u32 g_ciso_total_block = 0;

struct CISO_header {
	u8 magic[4];  // 0
	u32 header_size;  // 4
	u64 total_bytes; // 8
	u32 block_size; // 16
	u8 ver; // 20
	u8 align;  // 21
	u8 rsv_06[2];  // 22
};

// 0x00002708
static struct CISO_header g_CISO_hdr;

// 0x00002500
static u32 g_CISO_idx_cache[CISO_IDX_BUFFER_SIZE/4];

// 0x00000368
static void wait_until_ms0_ready(void)
{
	int ret, status = 0;
	const char *drvname;

	drvname = "mscmhc0:";

	if(psp_model == PSP_GO) {
		drvname = "mscmhcemu0:";
	}

	while ( 1 ) {
		ret = sceIoDevctl(drvname, 0x02025801, 0, 0, &status, sizeof(status));

		if(ret < 0) {
			sceKernelDelayThread(20000);
			continue;
		}

		if(status == 4) {
			break;
		}

		sceKernelDelayThread(20000);
	}
}

// 0x00000EE4
static int ciso_get_nsector(SceUID fd)
{
//	return g_CISO_hdr.total_bytes / g_CISO_hdr.block_size;;
	return g_ciso_total_block;
}

// 0x00000E58
static int iso_get_nsector(SceUID fd)
{
	SceOff off, total;

	off = sceIoLseek(fd, 0, PSP_SEEK_CUR);
	total = sceIoLseek(fd, 0, PSP_SEEK_END);
	sceIoLseek(fd, off, PSP_SEEK_SET);

	return total / ISO_SECTOR_SIZE;
}

// 0x00000E58
static int get_nsector(void)
{
	if(g_is_ciso) {
		return ciso_get_nsector(g_iso_fd);
	}

	return iso_get_nsector(g_iso_fd);
}

// 0x00000F00
static int is_ciso(SceUID fd)
{
	int ret;

	g_CISO_hdr.magic[0] = '\0';
	g_ciso_dec_buf_offset = 0x7FFFFFFF;

	sceIoLseek(fd, 0, PSP_SEEK_SET);
	ret = sceIoRead(fd, &g_CISO_hdr, sizeof(g_CISO_hdr));

	if (ret != sizeof(g_CISO_hdr)) {
		ret = -1;
		printk("%s: -> %d\n", __func__, ret);
		goto exit;
	}

	if (*(u32*)g_CISO_hdr.magic == 0x4F534943) { // CISO
		g_CISO_cur_idx = -1;
		g_ciso_total_block = g_CISO_hdr.total_bytes / g_CISO_hdr.block_size;
		printk("%s: total block %d\n", __func__, g_ciso_total_block);

		if (g_ciso_dec_buf == NULL) {
			g_ciso_dec_buf = oe_malloc(CISO_DEC_BUFFER_SIZE + 64);

			if (g_ciso_dec_buf == NULL) {
				ret = -2;
				printk("%s: -> %d\n", __func__, ret);
				goto exit;
			}

			if ((u32)g_ciso_dec_buf & 63)
				g_ciso_dec_buf = (void*)(((u32)g_ciso_dec_buf & 0xFFFFFFC0) + 64);
		}

		if (g_ciso_block_buf == NULL) {
			g_ciso_block_buf = oe_malloc(ISO_SECTOR_SIZE);

			if (g_ciso_block_buf == NULL) {
				ret = -3;
				printk("%s: -> %d\n", __func__, ret);
				goto exit;
			}
		}

		ret = 0;
	} else {
		ret = 0x8002012F;
	}

exit:
	return ret;
}

// 0x000009D4
static int iso_open(void)
{
	int ret;

	wait_until_ms0_ready();
	sceIoClose(g_iso_fd);
	g_iso_opened = 0;

	g_iso_fd = sceIoOpen(g_iso_fn, 0x000F0001, 0777);

	if(g_iso_fd < 0) {
		return -1;
	}

	g_is_ciso = 0;
	ret = is_ciso(g_iso_fd);

	if(ret >= 0) {
		g_is_ciso = 1;
	}

	g_iso_opened = 1;
	g_total_sectors = get_nsector();

	return 0;
}

// 0x00002744
static struct IsoOpenSlot g_open_slot[MAX_FILES_NR];

struct IoReadArg {
	u32 offset; // 0
	u8 *address; // 4
	u32 size; // 8
};

// 0x00002784
static struct IoReadArg g_read_arg;

// 0x00000BB4
static int read_raw_data(u8* addr, u32 size, int offset)
{
	int ret;
	int i;

	i = 0;

	do {
		i++;
		ret = sceIoLseek32(g_iso_fd, offset, PSP_SEEK_SET);

		if (ret >= 0) {
			i = 0;
			break;
		} else {
			iso_open();
		}
	} while(i < 16);

	if (i == 16) {
		ret = 0x80010013;
		goto exit;
	}

	for(i=0; i<16; ++i) {
		ret = sceIoRead(g_iso_fd, addr, size);

		if (ret >= 0) {
			i = 0;
			break;
		} else {
			iso_open();
		}
	}

	if (i == 16) {
		ret = 0x80010013;
		goto exit;
	}

exit:
	return ret;
}

// 0x00001018
static int read_cso_sector(u8 *addr, int sector)
{
	int ret;
	int n_sector;
	int offset, next_offset;
	int size;

	n_sector = sector - g_CISO_cur_idx;

	// not within sector idx cache?
	if (g_CISO_cur_idx == -1 || n_sector < 0 || n_sector >= NELEMS(g_CISO_idx_cache)) {
		ret = read_raw_data((u8*)g_CISO_idx_cache, sizeof(g_CISO_idx_cache), (sector << 2) + sizeof(struct CISO_header));

		if (ret != sizeof(g_CISO_idx_cache)) {
			ret = -4;
			printk("%s: -> %d\n", __func__, ret);

			return ret;
		}

		g_CISO_cur_idx = sector;
		n_sector = 0;
	}

	offset = (g_CISO_idx_cache[n_sector] & 0x7FFFFFFF) << g_CISO_hdr.align;

	// is plain?
	if (g_CISO_idx_cache[n_sector] & 0x80000000) {
		return read_raw_data(addr, ISO_SECTOR_SIZE, offset);
	}

	sector++;
	n_sector = sector - g_CISO_cur_idx;

	if (g_CISO_cur_idx == -1 || n_sector < 0 || n_sector >= NELEMS(g_CISO_idx_cache)) {
		ret = read_raw_data((u8*)g_CISO_idx_cache, sizeof(g_CISO_idx_cache), (sector << 2) + sizeof(struct CISO_header));

		if (ret != sizeof(g_CISO_idx_cache)) {
			ret = -5;
			printk("%s: -> %d\n", __func__, ret);

			return ret;
		}

		g_CISO_cur_idx = sector;
		n_sector = 0;
	}

	next_offset = (g_CISO_idx_cache[n_sector] & 0x7FFFFFFF) << g_CISO_hdr.align;
	size = next_offset - offset;
	
	if (size <= ISO_SECTOR_SIZE)
		size = ISO_SECTOR_SIZE;

	if (offset < g_ciso_dec_buf_offset || size + offset >= g_ciso_dec_buf_offset + CISO_DEC_BUFFER_SIZE) {
		ret = read_raw_data(g_ciso_dec_buf, CISO_DEC_BUFFER_SIZE, offset);

		/* May not reach CISO_DEC_BUFFER_SIZE */	
		if (ret < 0) {
			g_ciso_dec_buf_offset = 0xFFF00000;
			ret = -6;
			printk("%s: -> %d\n", __func__, ret);

			return ret;
		}

		g_ciso_dec_buf_offset = offset;
	}

	ret = sceKernelDeflateDecompress(addr, ISO_SECTOR_SIZE, g_ciso_dec_buf + offset - g_ciso_dec_buf_offset, 0);

	return ret < 0 ? ret : ISO_SECTOR_SIZE;
}

// 0x00001220
static int read_cso_data(u8* addr, u32 size, int offset)
{
	u32 cur_block = offset / ISO_SECTOR_SIZE;
	int ret;
	int read_bytes;
	int pos = offset & 0x7FF;

	if (pos) {
		ret = read_cso_sector(g_ciso_block_buf, cur_block);

		if (ret != ISO_SECTOR_SIZE) {
			ret = -7;
			printk("%s: -> %d\n", __func__, ret);

			return ret;
		}

		read_bytes = MIN(size, (ISO_SECTOR_SIZE - pos));
		memcpy(addr, g_ciso_block_buf + pos, read_bytes);
		size -= read_bytes;
		addr += read_bytes;
		cur_block++;
	} else {
		read_bytes = 0;
	}

	// more than 1 block left
	if (size / ISO_SECTOR_SIZE > 0) {
		int i;
		int block_cnt = size / ISO_SECTOR_SIZE;

		for(i=0; i<block_cnt; ++i) {
			ret = read_cso_sector(addr, cur_block);

			if (ret != ISO_SECTOR_SIZE) {
				ret = -8;
				printk("%s: -> %d\n", __func__, ret);

				return ret;
			}

			cur_block++;
			addr += ISO_SECTOR_SIZE;
			read_bytes += ISO_SECTOR_SIZE;
			size -= ISO_SECTOR_SIZE;
		}
	}

	if (size != 0) {
		ret = read_cso_sector(g_ciso_block_buf, cur_block);

		if (ret != ISO_SECTOR_SIZE) {
			ret = -9;
			printk("%s: -> %d\n", __func__, ret);

			return ret;
		}

		memcpy(addr, g_ciso_block_buf, size);
		read_bytes += size;
	}

	return read_bytes;
}

// 0x00000C7C
static int iso_read(struct IoReadArg *args)
{
	if (g_is_ciso != 0) {
		return read_cso_data(args->address, args->size, args->offset);
	}

	return read_raw_data(args->address, args->size, args->offset);
}

// 0x000023D8
static const char *g_umd_ids[] = {
	"ULES-00124",
	"ULUS-10019",
	"ULJM-05024",
	"ULAS-42009",
};

int g_00002480 = 0;

// 0x00000CB0
static int IoInit(PspIoDrvArg* arg)
{
	void *p;
	int i;

	p = oe_malloc(ISO_SECTOR_SIZE);

	if(p == NULL) {
		return -1;
	}

	g_sector_buf = p;

	g_umd9660_sema_id = sceKernelCreateSema("EcsUmd9660DeviceFile", 0, 1, 1, 0);

	if(g_umd9660_sema_id < 0) {
		return g_umd9660_sema_id;
	}

	while(0 == g_iso_opened) {
		iso_open();
		sceKernelDelayThread(20000);
	}

	memset(g_open_slot, 0, sizeof(g_open_slot));

	g_read_arg.offset = 0x8000;
	g_read_arg.address = g_sector_buf;
	g_read_arg.size = ISO_SECTOR_SIZE;
	iso_read(&g_read_arg);

	for(i=0; i<NELEMS(g_umd_ids); ++i) {
		if(0 == memcmp(g_read_arg.address + 0x00000373, g_umd_ids[i], 10)) {
			g_00002480 = 1;

			return 0;
		}
	}

	if(g_00002480) {
		return 0;
	}

	if(0 == memcmp(g_read_arg.address + 0x00000373, "NPUG-80086", 10)) {
		g_00002480 = 2;
	}

	return 0;
}

// 0x000002E8
static int IoExit(PspIoDrvArg* arg)
{
	u32 timeout = 500000;

	sceKernelWaitSema(g_umd9660_sema_id, 1, &timeout);
	SAFE_FREE(g_sector_buf);
	sceKernelDeleteSema(g_umd9660_sema_id);
	g_umd9660_sema_id = -1;

	return 0;
}

// 0x00000A78
static int IoOpen(PspIoDrvFileArg *arg, char *file, int flags, SceMode mode)
{
	int i, ret;

	i = 0;

	do {
		i++;
		ret = sceIoLseek32(g_iso_fd, 0, PSP_SEEK_SET);

		if (ret >= 0) {
			i = 0;
			break;
		} else {
			iso_open();
		}
	} while(i < 16);

	if (i == 16) {
		ret = 0x80010013;
		goto exit;
	}

	ret = sceKernelWaitSema(g_umd9660_sema_id, 1, NULL);

	if(ret < 0) {
		return -1;
	}

	for(i=0; i<NELEMS(g_open_slot); ++i) {
		if(!g_open_slot[i].enabled) {
			break;
		}
	}

	if(i == NELEMS(g_open_slot)) {
		ret = sceKernelSignalSema(g_umd9660_sema_id, 1);

		if(ret < 0) {
			return -1;
		}

		return 0x80010018;
	}

	arg->arg = (void*)i;
	g_open_slot[i].enabled = 1;
	g_open_slot[i].offset = 0;

	ret = sceKernelSignalSema(g_umd9660_sema_id, 1);

	if(ret < 0) {
		return -1;
	}

	ret = 0;

exit:
	return ret;
}

// 0x00000250
static int IoClose(PspIoDrvFileArg *arg)
{
	int ret, retv;
	int offset;

	ret = sceKernelWaitSema(g_umd9660_sema_id, 1, 0);

	if(ret < 0) {
		return -1;
	}

	offset = (int)arg->arg;

	if(!g_open_slot[offset].enabled) {
		retv = 0x80010016;
	} else {
		g_open_slot[offset].enabled = 0;
		retv = 0;
	}

	ret = sceKernelSignalSema(g_umd9660_sema_id, 1);

	if(ret < 0) {
		return -1;
	}

	return retv;
}

// 0x000003E0
static int iso_read_with_stack(u32 offset, void *ptr, u32 data_len)
{
	int ret, retv;

	ret = sceKernelWaitSema(g_umd9660_sema_id, 1, 0);

	if(ret < 0) {
		return -1;
	}

	g_read_arg.offset = offset;
	g_read_arg.address = ptr;
	g_read_arg.size = data_len;
	retv = sceKernelExtendKernelStack(0x2000, (void*)&iso_read, &g_read_arg);

	ret = sceKernelSignalSema(g_umd9660_sema_id, 1);

	if(ret < 0) {
		return -1;
	}

	return retv;
}

// 0x00000740
static int IoRead(PspIoDrvFileArg *arg, char *data, int len)
{
	int ret, retv, idx;
	u32 offset, read_len;

	ret = sceKernelWaitSema(g_umd9660_sema_id, 1, 0);

	if(ret < 0) {
		return -1;
	}

	idx = (int)arg->arg;
	offset = g_open_slot[idx].offset;
	ret = sceKernelSignalSema(g_umd9660_sema_id, 1);

	if(ret < 0) {
		return -1;
	}

	read_len = len;

	if(g_total_sectors < offset + len) {
		read_len = g_total_sectors - offset;
	}

	retv = iso_read_with_stack(offset * ISO_SECTOR_SIZE, data, read_len * ISO_SECTOR_SIZE);

	if(retv < 0) {
		return retv;
	}

	ret = sceKernelWaitSema(g_umd9660_sema_id, 1, 0);

	if(ret < 0) {
		return -1;
	}

	g_open_slot[idx].offset += retv * ISO_SECTOR_SIZE;
	ret = sceKernelSignalSema(g_umd9660_sema_id, 1);

	if(ret < 0) {
		return -1;
	}

	return retv * ISO_SECTOR_SIZE;
}

// 0x000000D8
static SceOff IoLseek(PspIoDrvFileArg *arg, SceOff ofs, int whence)
{
	int ret, idx;

	ret = sceKernelWaitSema(g_umd9660_sema_id, 1, NULL);

	if(ret < 0) {
		return -1;
	}

	idx = (int)arg->arg;
	
	if(whence == PSP_SEEK_SET) {
		g_open_slot[idx].offset = ofs;
	} else if (whence == PSP_SEEK_CUR) {
		g_open_slot[idx].offset += ofs;
	} else if (whence == PSP_SEEK_END) {
		/*
		 * Original march33 code, is it buggy?
		 * g_open_slot[idx].offset = g_total_sectors - (u32)ofs;
		 */
		g_open_slot[idx].offset = g_total_sectors + ofs;
	} else {
		ret = sceKernelSignalSema(g_umd9660_sema_id, 1);

		if(ret < 0) {
			return -1;
		}

		return 0x80010016;
	}

	if (g_total_sectors < g_open_slot[idx].offset) {
		g_open_slot[idx].offset = g_total_sectors;
	}

	ret = sceKernelSignalSema(g_umd9660_sema_id, 1);

	if(ret < 0) {
		return -1;
	}

	return g_open_slot[idx].offset;
}

// 0x0000083C
static int IoIoctl(PspIoDrvFileArg *arg, unsigned int cmd, void *indata, int inlen, void *outdata, int outlen)
{
	int ret, idx;

	idx = (int)arg->arg;

	if(cmd == 0x01F010DB) {
		return 0;
	}

	/* Read fd current offset */
	if(cmd == 0x01D20001) {
		ret = sceKernelWaitSema(g_umd9660_sema_id, 1, NULL);

		if(ret < 0) {
			return -1;
		}

		/* added more data len checks */
		if(outdata == NULL || outlen < 4) {
			return 0x80010016;
		}

		*(u32*)outdata = g_open_slot[idx].offset;
		ret = sceKernelSignalSema(g_umd9660_sema_id, 1);

		if(ret < 0) {
			return -1;
		}

		return 0;
	}

	/* UMD file seek whence */
	if(cmd == 0x01F100A6) {
		struct IoIoctlSeekCmd *seek_cmd;

		if (indata == NULL || inlen < sizeof(struct IoIoctlSeekCmd)) {
			return 0x80010016;
		}

		seek_cmd = (struct IoIoctlSeekCmd *)indata;

		return IoLseek(arg, seek_cmd->offset, seek_cmd->whence);
	}

	if(cmd == 0x01F30003) {
		u32 len;

		if(indata == NULL || inlen < 4) {
			return 0x80010016;
		}

		len = *(u32*)indata;

		if(outdata == NULL || outlen < len) {
			return 0x80010016;
		}

		return IoRead(arg, outdata, len);
	}

	printk("%s: Unknown ioctl 0x%08X\n", __func__, cmd);

	return 0x80010086;
}

// 0x00000488
static int sub_00000488(void *outdata, int outlen, void *indata)
{
	u32 indata_8, indata_16, indata_24;
	u32 offset;

	indata_16 = *(u32*)(indata+16);

	if(outlen < indata_16) {
		return 0x80010069;
	}

	indata_8 = *(u32*)(indata+8);
	indata_24 = *(u32*)(indata+24);

	if(!indata_24) {
		offset = indata_8 * ISO_SECTOR_SIZE;
		goto exit;
	}

	if(*(u32*)(indata+20)) {
		offset = indata_8 * ISO_SECTOR_SIZE - indata_24 + ISO_SECTOR_SIZE;
		goto exit;
	}

	if(!*(u32*)(indata+28)) {
		offset = indata_8 * ISO_SECTOR_SIZE + indata_24;
		goto exit;
	}

	offset = indata_8 * ISO_SECTOR_SIZE - indata_24 + ISO_SECTOR_SIZE;

exit:
	return iso_read_with_stack(offset, outdata, indata_16);
}

// 0x000004F4
static int IoDevctl(PspIoDrvFileArg *arg, const char *devname, unsigned int cmd, void *indata, int inlen, void *outdata, int outlen)
{
	if(cmd == 0x01F00003) {
		return 0;
	}

	if(cmd == 0x01F010DB) {
		return 0;
	}

	/* get UMD disc type*/
	if(cmd == 0x01F20001) {
		// 0 = No disc.
		// 0x10 = Game disc.
		// 0x20 = Video disc.
		// 0x40 = Audio disc.
		// 0x80 = Cleaning disc.
		_sw(-1, (u32)(outdata));
		_sw(0x10, (u32)(outdata+4));

		return 0;
	}

	/* missing 0x01F100A4, seek UMD disc (raw). */
	if(cmd == 0x01F100A4) {
		if(indata == NULL || inlen < 4) {
			return 0x80010016;
		}

		return 0;
	}

	/* missing 0x01F300A5, prepare UMD data into cache */
	if(cmd == 0x01F300A5) {
		if(indata == NULL || inlen < 4) {
			return 0x80010016;
		}

		if(outdata == NULL || outlen < 4) {
			return 0x80010016;
		}

		_sw(1, (u32)outdata);

		return 0;
	}

	if(cmd == 0x01F20002 || cmd == 0x01F20003) {
		_sw(g_total_sectors, (u32)(outdata));

		return 0;
	}

	if(cmd == 0x01E18030) {
		return 1;
	}

	if(cmd == 0x01E180D3) {
		return 0x80010086;
	}

	if(cmd == 0x01E080A8) {
		return 0x80010086;
	}

	if(cmd == 0x01E28035) {
		/* Added check for outdata */
		if(outdata == NULL || outlen < 4) {
			return 0x80010016;
		}

		_sw((u32)g_sector_buf, (u32)(outdata));

		return 0;
	}

	if(cmd == 0x01E280A9) {
		/* Added check for outdata */
		if(outdata == NULL || outlen < 4) {
			return 0x80010016;
		}

		_sw(ISO_SECTOR_SIZE, (u32)(outdata));

		return 0;
	}

	if(cmd == 0x01E38034) {
		if(indata == NULL || outdata == NULL) {
			return 0x80010016;
		}

		_sw(0, (u32)(outdata));

		return 0;
	}

	/**
	 * 0x01E380C0: read sectors general
	 * 0x01F200A1: read sectors
	 * 0x01F200A2: read sectors dircache
	 */
	if(cmd == 0x01E380C0 || cmd == 0X01F200A1 || cmd == 0x01F200A2) {
		if(indata == NULL || outdata == NULL) {
			return 0x80010016;
		}

		return sub_00000488(outdata, outlen, indata);
	}

	if(cmd == 0x01E38012) {
		int outlen2 = outlen;

		// loc_6E0
		if(outlen < 0) {
			outlen2 = outlen + 3;
		}

		memset(outdata, 0, outlen2);
		_sw(0xE0000800, (u32)outdata);
		_sw(0, (u32)(outdata + 8));
		_sw(g_total_sectors, (u32)(outdata + 0x1C));
		_sw(g_total_sectors, (u32)(outdata + 0x24));

		return 0;
	}

	printk("%s: Unknown cmd 0x%08X\n", __func__, cmd);

	return 0x80010086;
}

// 0x000023EC
static PspIoDrvFuncs g_drv_funcs = {
	.IoInit    = &IoInit,
	.IoExit    = &IoExit,
	.IoOpen    = &IoOpen,
	.IoClose   = &IoClose,
	.IoRead    = &IoRead,
	.IoWrite   = NULL,
	.IoLseek   = &IoLseek,
	.IoIoctl   = &IoIoctl,
	.IoRemove  = NULL,
	.IoMkdir   = NULL,
	.IoRmdir   = NULL,
	.IoDopen   = NULL,
	.IoDclose  = NULL,
	.IoDread   = NULL,
	.IoGetstat = NULL,
	.IoChstat  = NULL,
	.IoRename  = NULL,
	.IoChdir   = NULL,
	.IoMount   = NULL,
	.IoUmount  = NULL,
	.IoDevctl  = &IoDevctl,
	.IoUnk21   = NULL,
};

// 0x00002444
PspIoDrv g_iodrv = {
	.name = "umd",
	.dev_type = 4, // block device
	.unk2 = 0x800,
	.name2 = "UMD9660",
	.funcs = &g_drv_funcs,
};
