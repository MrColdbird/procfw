#include <pspsdk.h>
#include <pspiofilemgr.h>
#include <psploadcore.h>
#include <stdio.h>
#include <string.h>
#include "pspcipher.h"
#include "printk.h"
#include "libs.h"
#include "stargate.h"

static SceUID g_sema = -1;

typedef struct {
	u32 tag;
	u8 *key;
	u8 *xor;
	u32 code;
	u32 type;
} Cipher;

static u8 key_2fd312f0[16] = {
	0xC5, 0xFB, 0x69, 0x03, 0x20, 0x7A, 0xCF, 0xBA, 0x2C, 0x90, 0xF8, 0xB8, 0x4D, 0xD2, 0xF1, 0xDE
};

static u8 key_2fd30bf0[16] = {
	0xD8, 0x58, 0x79, 0xF9, 0xA4, 0x22, 0xAF, 0x86, 0x90, 0xAC, 0xDA, 0x45, 0xCE, 0x60, 0x40, 0x3F
};

static u8 key_2fd311f0[16] = {
	0x3A, 0x6B, 0x48, 0x96, 0x86, 0xA5, 0xC8, 0x80, 0x69, 0x6C, 0xE6, 0x4B, 0xF6, 0x04, 0x17, 0x44
};

static u8 key_2fd313f0[16] = {
	0xB0, 0x24, 0xC8, 0x16, 0x43, 0xE8, 0xF0, 0x1C, 0x8C, 0x30, 0x67, 0x73, 0x3E, 0x96, 0x35, 0xEF
};

static u8 xor_key[16] = {
	0xA9, 0x1E, 0xDD, 0x7B, 0x09, 0xBB, 0x22, 0xB5, 0x9D, 0xA3, 0x30, 0x69, 0x13, 0x6E, 0x0E, 0xD8
};

static Cipher g_cipher[] = {
	{ 0x2fd30bf0, key_2fd30bf0, xor_key, 0x47, 5},
	{ 0x2fd311f0, key_2fd311f0, xor_key, 0x47, 5},
	{ 0x2fd312f0, key_2fd312f0, xor_key, 0x47, 5},
	{ 0x2fd313f0, key_2fd313f0, xor_key, 0x47, 5},
};

static Cipher *GetCipherByTag(u32 tag)
{
	int i;

	for(i=0; i<sizeof(g_cipher) / sizeof(g_cipher[0]); ++i) {
		if (g_cipher[i].tag == tag)
			return &g_cipher[i];
	}

	return NULL;
}

int myPauth_98B83B5D(u8 *p, u32 size, u32 *newsize, u8 *xor_key)
{
	user_decryptor db;
	u32 tag = 0;
	int ret;
	Cipher *cipher = NULL;

	u32 k1 = pspSdkGetK1();
	pspSdkSetK1(0);

	tag = *((u32*)(p+0xd0));
	cipher = GetCipherByTag(tag);

	if (cipher == NULL) {
		printk("%s: unknown key tag 0x%08x\n", __func__, tag);
		pspSdkSetK1(k1);
		return -1;
	}

	db.tag = &tag;
	db.prx = p;
	db.size  = size;
	db.newsize = newsize;
	db.use_polling = 0;
	db.blacklist = NULL;
	db.blacklistsize = 0;
	db.code = cipher->code;
	db.type = cipher->type;
	db.xor_key2 = xor_key;

	db.key = cipher->key;
	db.xor_key1 = cipher->xor;

	ret = sceKernelWaitSema(g_sema, 1, 0);

	if (ret < 0)
		return ret;

	ret = _uprx_decrypt(&db);
	sceKernelSignalSema(g_sema, 1);

	printk("%s: tag 0x%08x returns 0x%08x\n", __func__, tag, ret);
	pspSdkSetK1(k1);

	return ret;
}

int myPauth_init(void)
{
#ifdef DEBUG
	g_sema = sceKernelCreateSema("pauth", 0, 1, 1, 0);
#else
	g_sema = sceKernelCreateSema("", 0, 1, 1, 0);
#endif

	return g_sema >= 0 ? 0 : -1;
}
