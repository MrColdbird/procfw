#include <pspsdk.h>
#include <pspkernel.h>
#include <systemctrl.h>
#include <string.h>
#include <psprtc.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "main.h"

int dopen_subuid = -1;
int dopen_uid = -1;
char opened_dirname[256];
int uncategory_flag = 0;
int enable_uncategory = 0;


SceUID 	sceIoDopenPatched (const char *dirname)
{
	SceUID ret = sceIoDopen(dirname);

	SceIoStat stat;

	if( scePaf_strcmp( dirname + 4 ,"/PSP/GAME") == 0)
	{
		memset( &stat , 0 , sizeof(SceIoStat) );

		if ( (sceIoGetstat( "ef0:/seplugins/hide_uncategorized.txt" , &stat ) >= 0 )
			|| (sceIoGetstat( "ms0:/seplugins/hide_uncategorized.txt" , &stat ) >= 0 ) )
		{
			 uncategory_flag = 1;
		}
		else
		{
			uncategory_flag = 0;
		}

		ClearCategories();

		scePaf_strcpy( opened_dirname, dirname );

		enable_uncategory = 0;
		dopen_uid = ret;

	}

	return ret;
}

int sceIoDclosePatched(SceUID fd)
{
	if( fd == dopen_uid )
	{
		if(enable_uncategory)
			AddCategory("Uncategorized" , 1 );

		dopen_uid = -1;
	}

	return sceIoDclose( fd );
}

static void build_dname(char *d_name, const char *category, const char *orig_dname)
{
	char name[256];

	scePaf_strcpy(name, orig_dname);
	scePaf_snprintf(d_name, 256, "%s/%s", category, name);
}

int sceIoDreadPatched (SceUID fd, SceIoDirent *dir)
{
	int return_vaue;

	if( fd == dopen_uid )
	{

CONTINUE_LABEL:

		if( dopen_subuid >= 0)
		{
			int ret = sceIoDread( dopen_subuid , dir );
			if( ret > 0) /* more directory */ {
				if( dir->d_name[0] == '.')
					goto CONTINUE_LABEL;

				build_dname(dir->d_name, opened_dirname+14, dir->d_name);

				if(dir->d_private) {
					scePaf_strcpy( dir->d_private + 13 , dir->d_name );
				}
			} else {
				sceIoDclose( dopen_subuid );
				dopen_subuid = -1;
				goto CONTINUE_LABEL;		
			}		
			return_vaue = ret;
		} else {
			int ret = sceIoDread( fd , dir );		
			return_vaue = ret;
			if( ret > 0) /* more directory */ {
				if( dir->d_name[0] != '.' 
					&& FIO_S_ISDIR( dir->d_stat.st_mode )) {				
					SceIoStat stat;

					memset( &stat , 0 , sizeof(SceIoStat) );

					scePaf_snprintf( opened_dirname + 13 , 128 ,"/%s/EBOOT.PBP", dir->d_name );

					if( sceIoGetstat( opened_dirname ,  &stat ) < 0 ) {
						scePaf_snprintf(opened_dirname + 13 , 128 ,"/%s/PARAM.PBP", dir->d_name);

						if(sceIoGetstat( opened_dirname ,  &stat ) < 0) {
							u64 tick;

							sceRtcGetTick( (pspTime*)&(dir->d_stat.st_mtime) , &tick );
							AddCategory( dir->d_name , tick );

							scePaf_snprintf( opened_dirname + 13 , 128 ,"/%s", dir->d_name );

							dopen_subuid = sceIoDopen( opened_dirname );						
							goto CONTINUE_LABEL;
						}
					}
							
					if(uncategory_flag ) {
						goto CONTINUE_LABEL;										
					}
				}
			} else if(uncategory_flag == 0) {
				enable_uncategory = 1;											
			}
		}
	} else {
		return_vaue = sceIoDread( fd , dir );
	}

	return return_vaue;
}

int scePaf_5E7610DF_Patched(char *a0 , char* a1 , char* a2 , char* a3 , char* t0)
{
	scePaf_strcpy( a1 , t0 );

	return scePaf_snprintf( a0 , 291 , a2 , a3 , t0 );
}

void ExportPatch(u32 text_addr)
{
	//sceIoDopenPatched
	REDIRECT_FUNCTION((u32)sceIoDopenPatched, text_addr + 0x0002A5F0);

	//sceIoDreadPatched
	REDIRECT_FUNCTION((u32)sceIoDreadPatched, text_addr + 0x0002A600);

	//sceIoDclosePatched
	REDIRECT_FUNCTION((u32)sceIoDclosePatched, text_addr + 0x0002A608);

	//scePaf_5E7610DF_patched
	_sw(MAKE_CALL(scePaf_5E7610DF_Patched), text_addr + 0x0001E42C);
	_sw(MAKE_CALL(scePaf_5E7610DF_Patched), text_addr + 0x0002109C);

	//addu       $a1, $s2, $zr 
	_sw(0x02402821 , text_addr + 0x0001E420);

	//addu       $a1, $s0, $zr
	_sw(0x02002821 , text_addr + 0x00021060);
}
