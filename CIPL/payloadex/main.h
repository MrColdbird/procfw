

#define _JAL_OPCODE	0x0C000000
#define _J_OPCODE	0x08000000
#define _MIPS_JAL(IMM) (0xC000000 + (u32)(IMM)&0x3FFFFFF)
#define _MIPS_ADDI(RT,RS,IMM)    (0x24000000|(RS<<21)|(RT<<16)|((u32)(IMM)&0xffff))
#define _MAKE_CALL(a, f) _sw(_JAL_OPCODE | (((u32)(f) >> 2)  & 0x03ffffff), a); 
#define _MAKE_JUMP(a, f) _sw(_J_OPCODE | (((u32)(f) & 0x0ffffffc) >> 2), a); 

#define _MIPS_ADDU(RD,RS,RT) (0x00000021|(RD<<11)|(RT<<16)|(RS<<21))

//#define CHANGE_FUNC(a, f) _sw(J_OPCODE | (((u32)(f) & 0x3FFFFFFF) >> 2), a); _sw(0, a+4);


typedef struct
{
	u32		signature;  // 0
	u16		attribute; // 4  modinfo
	u16		comp_attribute; // 6
	u8		module_ver_lo;	// 8
	u8		module_ver_hi;	// 9
	char	modname[28]; // 0A
	u8		version; // 26
	u8		nsegments; // 27
	int		elf_size; // 28
	int		psp_size; // 2C
	u32		entry;	// 30
	u32		modinfo_offset; // 34
	int		bss_size; // 38
	u16		seg_align[4]; // 3C
	u32		seg_address[4]; // 44
	int		seg_size[4]; // 54
	u32		reserved[5]; // 64
	u32		devkitversion; // 78
	u32		decrypt_mode; // 7C 
	u8		key_data0[0x30]; // 80
	int		comp_size; // B0
	int		_80;	// B4
	int		reserved2[2];	// B8
	u8		key_data1[0x10]; // C0
	u32		tag; // D0
	u8		scheck[0x58]; // D4
	u32		key_data2; // 12C
	u32		oe_tag; // 130
	u8		key_data3[0x1C]; // 134
	u8		main_data;//150
}PSP_Header;

/*
typedef struct BtcnfHeader
{
	int signature; // 0
	int devkit; // 4
	int unknown[2]; // 8
	int modestart; // 0x10
	int nmodes; // 0x14
	int unknown2[2]; // 0x18
	int modulestart; // 0x20
	int nmodules; // 0x24
	int unknown3[2]; // 0x28
	int modnamestart; // 0x30
	int modnameend; // 0x34
	int unknown4[2]; // 0x38
} __attribute__((packed)) BtcnfHeader;


typedef struct ModuleEntry
{
	u32 stroffset;//0
	int reserved;//4
	u16 flags;//8
	u8 loadmode;//0x0a
	u8 loadmode2;//0x0B
	int reserved2;//0x0C
	u8 hash[0x10];//0x10
} __attribute__((packed)) ModuleEntry;

typedef struct ModeEntry
{
	u16 maxsearch;
	u16 searchstart; //
	int modeflag;
	int mode2;
	int reserved[5];
} __attribute__((packed)) ModeEntry;

*/
/*
//size 0xC0
typedef struct _SceLoadCoreExecFileInfo
{
  int unk_0;
  int unk_4; //attr? 0x1 = , 0x2 =
  int unk_8; //API
  int unk_C;
  int unk_10; //offset of start of file (after ~SCE header if it exists)
  int unk_14;
  int unk_18;
  int unk_1C;
  int elf_type; //20 - elf type - 1,2,3 valid
  int topaddr; //24 - address of gzip buffer
  int (*bootstart)(SceSize, void *); //28
  int unk_2C;
  int unk_30; //30 - size of PRX?
  int unk_34; //
  int unk_38;
  int unk_3C;
  int unk_40; //partition id
  int unk_44;
  int unk_48;
  int unk_4C;
  SceModuleInfo *module_info; //50 - pointer to module info i.e. PSP_MODULE_INFO(...)
  int unk_54;
  short unk_58; //attr as in PSP_MODULE_INFO - 0x1000 = kernel
  short unk_5A; //attr? 0x1 = use gzip
  int unk_5C; //size of gzip buffer to allocate
  int unk_60;
  int unk_64;
  int unk_68;
  int unk_6C;
  reglibin *export_libs; //70
  int num_export_libs; //74
  int unk_78;
  int unk_7C;
  int unk_80;
  unsigned char unk_84[4];
  unsigned int segmentaddr[4]; //88
  unsigned int segmentsize[4]; //98
  unsigned int unk_A8;
  unsigned int unk_AC;
  unsigned int unk_B0;
  unsigned int unk_B4;
  unsigned int unk_B8;
  unsigned int unk_BC;
} SceLoadCoreExecFileInfo;
*/