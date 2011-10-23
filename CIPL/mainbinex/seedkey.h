
#if _PSP_FW_VERSION == 639	

//6.37 - 6.39
const unsigned int seed_key[] = {
	0x84E3C0F5,
	0xBC861893,
	0xEEB6470D,
	0x84AAB42C,

	0x4D6DBE3C,
	0x967C51C4,
	0xE50772C2,
	0x5374B9F0,

	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,

	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000
};

#elif _PSP_FW_VERSION == 660	
const unsigned int seed_key[] = {
	0x830D9389,
	0xF2027F71,
	0x2D060DC5,
	0xBA78A905,

	0x26C9824D,
	0x3335A7A5,
	0xD5CD6368,
	0xD136A11D,

	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,

	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000
};
#else
#error seedkey.h
#endif
