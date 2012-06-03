/*
 * 
 * 6.3X main_bin ex.bin
 *
 */

#include <pspsdk.h>
#include "main.h"
#include "patch_addr.h"

#if PSP_MODEL == 0
//fat
#include "../payloadex/nand_payloadex_01g.h"

#elif PSP_MODEL == 1
//slim
#include "../payloadex/nand_payloadex_02g.h"

#else
#error PSP_MODEL is not defined
#endif

void patch_main_bin(void);
void sub_040004DC_patch();
void seed_patch();

void ClearCache() __attribute__((noinline));
void ClearCache()
{
	Dcache();
	Icache();
}

void Patch_Entry(void *a0, void *a1, void *a2, void *a3, void *t0, void *t1, void *t2) __attribute__ ((section (".text.startup")));
void Patch_Entry(void *a0, void *a1, void *a2, void *a3, void *t0, void *t1, void *t2)
{
	MAKE_JUMP( 0x040EC2C8 , patch_main_bin);
	_sw( 0 , 0x040EC0D8 );

	ClearCache();

	void (* back)() = 0x040EC000;

	return back(a0, a1, a2, a3, t0, t1, t2);
}

void patch_main_bin(void)
{
	u32 *code = (u32 *)0x04000000;

	MAKE_CALL( 0x04000364 , sub_040004DC_patch );

	code[REBOOTEX_ADDR_PATCH/4] = 0x3C1988FC;//jump 0x88fc

#if PSP_MODEL == 1
	code[(SEEDKEY_PATCH_ADDR	)/4] = 0x08000000 | (((u32)seed_patch & 0x0ffffffc)>>2);
	code[(SEEDKEY_PATCH_ADDR + 4)/4] = 0;
#endif

	ClearCache();

	asm("lui		$25, 0x0400");
	asm("lui		$sp, 0x040F");
	asm("jr		$25");
	asm("ori		$sp,$sp, 0xFF00");
}

#if PSP_MODEL == 1
#include "seedkey.h"
void seed_patch()
{
	void (* back)() = (void *)0x04000000 + SEEDKEY_RETURN_ADDR;
	memcpy( 0xBFC00200 , seed_key ,sizeof(seed_key));
	return back();
}
#endif

#define SYSCON_CTRL_HOME      0x00001000

void sub_040004DC_patch()
{
	void (*sub_040004DC)() = 0x040004DC;
	u32 *key_buff = (u32 *)0x88FB0000;

	void (*pspSyscon_rx_dword)(u32 *param,u8 cmd) = (void *)SYSCON_DWORD_ADDR;

	key_buff[0] = -1;
	pspSyscon_rx_dword( key_buff , 7 );

	if( key_buff[0] & SYSCON_CTRL_HOME )
	{
		memcpy( 0x88FC0000 , nand_payloadex , size_nand_payloadex);
	}
	else
	{
		REDIRECT_FUNCTION( 0x88FC0000 , 0x88600000 );
	}

	return sub_040004DC();
}

void Icache(void)
{
	__asm__ __volatile__ ("\
	.word 0x40088000;\
	.word 0x24091000;\
	.word 0x7D081240;\
	.word 0x01094804;\
	.word 0x4080E000;\
	.word 0x4080E800;\
	.word 0x00004021;\
	.word 0xBD010000;\
	.word 0xBD030000;\
	.word 0x25080040;\
	.word 0x1509FFFC;\
	.word 0x00000000;\
	"::);

	return;
}

void Dcache(void)
{
	__asm__ __volatile__ ("\
	.word 0x40088000;\
	.word 0x24090800;\
	.word 0x7D081180;\
	.word 0x01094804;\
	.word 0x00004021;\
	.word 0xBD100000;\
	.word 0x400AE000;\
	.word 0x400BE800;\
	.word 0x7D4C0500;\
	.word 0x11800004;\
	.word 0x7D6C0500;\
	.word 0x000A5340;\
	.word 0x01485025;\
	.word 0xBD5A0000;\
	.word 0x11800003;\
	.word 0x000B5B40;\
	.word 0x01685825;\
	.word 0xBD7A0000;\
	.word 0x25080040;\
	.word 0x1509FFF1;\
	.word 0x00000000;\
	"::);

	return;
}

int memcpy(u8 *dst,u8 *src,int size)
{
	u8 *p1 = (u8 *)dst;
	u8 *p2 = (u8 *)src;
	while(size--)
	{
		*p1++ = *p2++;
	}
	return p1;
}
