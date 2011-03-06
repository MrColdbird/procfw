
#include <pspsdk.h>
#include <pspkernel.h>
#include <pspsysmem.h>
#include <systemctrl.h>
#include <string.h>
#include <psprtc.h>
#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "utils.h"

PSP_MODULE_INFO("category", 0x0007, 1, 0);
PSP_NO_CREATE_MAIN_THREAD();

STMOD_HANDLER previous = NULL;

u32 game_plugin_text_size = 0;
int vsh_patch_flag = 0;
u32 game_plugin_text_addr = 0;
int syspatch_flag = 0;

char str_buff[256];

extern int sceKernelGetCompiledSdkVersion(void);
extern int vsh_3E5F64EB(void *a0);

int vsh_3E5F64EB_Patched(void *a0)
{
	if( vsh_patch_flag == 0)
	{
		_sw(2, game_plugin_text_addr + 0x0002E654);

		sceKernelGetCompiledSdkVersion();
		vsh_patch_flag = 1;
	}

	return vsh_3E5F64EB(a0);
}

void *category_buffer = NULL;

void patch_category_buff(int index )
{
	u32 text_addr = game_plugin_text_addr;

	u32 start_addr = text_addr;
	u32 end_addr = text_addr + game_plugin_text_size;

	if( category_buffer )
	{
		scePaf_free( category_buffer );
	}

	u32 size = index * 12 + 816;

	u32 *buff = (u32 *)scePaf_malloc( size);

	category_buffer = buff;

	memset( buff , 0 , size );

	u16 value = (index - 1)& 0xFFFF;

#define BUFF_PATCH_ADDR 0x0002E660
	u32 patch_value = 0x24000000 | ( (text_addr + BUFF_PATCH_ADDR ) & 0xFFFF);

	_sh( value , text_addr + 0x0001A8A8 );
	_sw( (u32)buff , text_addr + BUFF_PATCH_ADDR );
	_sh( value , text_addr + 0x0001AF58 );

	while( start_addr < end_addr )
	{
		u32 lw_value = *(u32 *)start_addr;

		if( (lw_value & 0xFC00FFFF ) == patch_value )
		{
			if( lw_value & 0x03E00000)
			{
				if( lw_value & 0x001F0000)
				{
					//lw $? ?($?)
					_sw( ( lw_value & 0x03FFFFFF ) | 0x8C000000 , start_addr );
				}
			}
		}

		start_addr += 4;
	}

}

int OnModuleStart(SceModule2 *mod)
{
	u32 text_addr = mod->text_addr;
	char *modname = mod->modname;

	if( scePaf_strcmp( modname , "game_plugin_module") == 0)
	{
		
		game_plugin_text_size = mod->text_size;
		vsh_patch_flag = 0;
		syspatch_flag = 0;
		game_plugin_text_addr = text_addr;

		SystemPatch( text_addr );
		ExportPatch( text_addr );

		_sw(MAKE_CALL(vsh_3E5F64EB_Patched), text_addr+0x000012E0);

		patch_category_buff( 32 );

	}
	else if ( scePaf_strcmp( modname , "vsh_module") == 0)
	{
		//patch compiled sdk version
		_sw(MAKE_JUMP(ClearCaches), 0x88009B28);
	}

	ClearCaches();

	if (!previous)
		return 0;

	return previous(mod);
}

int module_start(SceSize args, void *argp)
{
	previous = sctrlHENSetStartModuleHandler(OnModuleStart);
	
	return 0;
}

void ClearCaches()
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
	.word 0x40088000;\
	.word 0x24090800;\
	.word 0x7D081180;\
	.word 0x01094804;\
	.word 0x00004021;\
	.word 0xBD140000;\
	.word 0xBD140000;\
	.word 0x25080040;\
	.word 0x1509FFFC;\
	.word 0x00000000;\
	"::);

	return;
}
