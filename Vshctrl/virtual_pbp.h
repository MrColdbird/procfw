#ifndef VIRTUAL_PBP
#define VIRTUAL_PBP

typedef struct _PBPSection {
	u32 lba;
	u32 size;
} PBPSection;

typedef struct _VirtualPBP {
	u32 opened;
	u32 enabled;
	u32 total_size;
	char name[128];
	u32 header[10]; // PBP header
	PBPSection sects[8];
	u32 file_pointer;
} VirtualPBP;

#define MAGIC_VPBP_FD 0x8000
#define MAX_VPBP 128
#define PTR_ALIGN_64(p) ((void*)((((u32)p)+64-1)&(~(64-1))))
#define ISOEBOOT(file) (strlen(file) == 39 && strncmp(file + 14, "ISOGAME", 7) == 0 && strcmp(file + strlen(file) - 9, "EBOOT.PBP") == 0)

int vpbp_init(void);
SceUID vpbp_open(const char * file, int flags, SceMode mode);
SceOff vpbp_lseek(SceUID fd, SceOff offset, int whence);
int vpbp_read(SceUID fd, void * data, SceSize size);
int vpbp_close(SceUID fd);
int vpbp_disable_all_caches(void);
int vpbp_remove(const char * file);
int vpbp_getstat(const char * file, SceIoStat * stat);
int vpbp_is_fd(SceUID fd);
int vpbp_loadexec(char * file, struct SceKernelLoadExecVSHParam * param);

#endif
