#include <pspsdk.h>
#include <pspkernel.h>
#include <psputility_sysparam.h>
#include <systemctrl.h>
#include <string.h>
#include <psprtc.h>
#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "utils.h"

extern u32 game_plugin_text_addr;
extern char str_buff[];

wchar_t category_name_jp[] = L"\u30AB\u30C6\u30B4\u30EA\u5225";
wchar_t category_name_cn[] = L"\u6309\u7c7b\u522b";
wchar_t category_name_ru[] = L"\u041f\u043e \u043a\u0430\u0442\u0435\u0433\u043e\u0440\u0438\u0438";
wchar_t category_name_fr[] = L"Par cat\u00e9gorie";
wchar_t category_name_ge[] = L"Nach Kategorie";
wchar_t category_name_en[] = L"By Category";

wchar_t *scePaf_70082F6F(int *a0 , char *a1);
int scePaf_4E96DECC(char *a0, int a1, int a2);

wchar_t *GetCategoryMenuName(int *a0 , char *a1)
{
	if( a1)
	{
		if( scePaf_strcmp( a1 ,"msg_by_expiration") == 0)
		{
			int system_language;

			sceUtilityGetSystemParamInt( PSP_SYSTEMPARAM_ID_INT_LANGUAGE , &system_language );

			wchar_t *bridge;

			switch( system_language )
			{
			case PSP_SYSTEMPARAM_LANGUAGE_JAPANESE:
				bridge = category_name_jp;
				break;
			case PSP_SYSTEMPARAM_LANGUAGE_CHINESE_SIMPLIFIED:
				bridge = category_name_cn;
				break;
			case PSP_SYSTEMPARAM_LANGUAGE_RUSSIAN:
				bridge = category_name_ru;
				break;
			case PSP_SYSTEMPARAM_LANGUAGE_GERMAN:
				bridge = category_name_ge;
				break;
			case PSP_SYSTEMPARAM_LANGUAGE_FRENCH:
				bridge = category_name_fr;
				break;
			default:
				bridge = category_name_en;
				break;
			}

			return bridge;

		}
	}

	return scePaf_70082F6F( a0 , a1);
}


//index = 3 PSP
// 5 = PS
// 6 = PCƒGƒ“ƒWƒ“
// 7 = neogeo?

int GetCategoryType(int index)
{
	u32 text_addr = game_plugin_text_addr;

	u32 value = *(u32 *)(text_addr + 0x0002EF84 );

	u32 *value2 = (u32 *)( ((u32 *)value)[ index ]);
	char *folder_name = (char *)(value2[68/4]);

	int ret_cnt = 0;
	Category *p = NULL;

	while(1)
	{
		p = GetNextCategory(p);

		if(!p)
		{
			ret_cnt --;
			break;
		}

		int len = scePaf_strlen(&p->name);

		if( scePaf_memcmp( &p->name , folder_name , len ) == 0)//memcmp?
		{
			if( folder_name[len] == '/')
			{
				break;
			}
		}

		ret_cnt ++;
	}

	return ret_cnt;
}

char *GetCategorySubinfo( char *a0 , char *a1)
{
	char sp[8];
	char str_buffer[128];

	scePaf_strcpy( sp , a1 + 0x154 );

	char *game_type = NULL;
	char *version_str = NULL;

	if (scePaf_strcmp( a1 + 0x15C , "EG") == 0)
	{
		game_type = "PSN Game";
	
		if( a1[0x154] == 0)
		{
			version_str = "5.00";
		}
	}
	else if(scePaf_strcmp( a1 + 0x15C , "ME") == 0)
	{	
		game_type = "PS1 Game";
	
		if( a1[0x154] == 0)
		{
			version_str = "3.03";
		}
	}
	else
	{
	
		if( scePaf_strcmp( a1 + 0xEC , "UCJS10041") != 0)
		{
			game_type = "Game";

			if( a1[0x154] == 0)
				version_str = "1.00";
		}
		else
		{	
			game_type = "Homebrew Game";

			if( a1[0x154] == 0)
				version_str = "2.71";
		}
	}

	if(version_str)
		scePaf_strcpy(sp , version_str);

	char *firm_str;

	if( (sp[0] < '6')
		|| ( sp[2] < '3'))
	{
		firm_str = "%s (for %s - 6.35)";
	}
	else
	{
		firm_str = "%s (requires %s)";
	}

	scePaf_snprintf( str_buffer , 128 , firm_str , game_type , sp );

	char *from = str_buffer;
	char *to = str_buff;

	while( from[0] )
	{
		to[0] = from[0];
		from ++;
		to += 2;
	}

	to[0] = 0;
	to[1] = 0;

	return str_buff;
}


int GetCategoryCnt(char *a0 ,int a1 , int a2)
{

	int ret = a1;

	if(a1 == 3)
		ret = CountCategories();

	return scePaf_4E96DECC( a0 , ret ,a2 );
}


wchar_t folder_name[] = L"unknown";
wchar_t *GetCategoryFolderName(int index)//sub_00000AC8
{
	Category *ret = (Category *)GetNextCategory( NULL );
	int cnt = 0;

	while( ret )
	{
		if( index == cnt)
		{
			char *from = &ret->name;
			char *to = str_buff;

			while( from[0] )
			{
				to[0] = from[0];
				to += 2;
				from ++;
			}

			to[0] = 0;
			to[1] = 0;

			return (wchar_t *)str_buff;//data1C68
		}

		cnt ++;
		ret = GetNextCategory( ret );
	}

	return folder_name;
}

void SystemPatch(u32 text_addr )
{
	//category cnt patch
	_sw(MAKE_CALL(GetCategoryCnt), text_addr + 0x0000E98C);

	//category patch
	_sw(MAKE_CALL(GetCategoryType), text_addr + 0x00001600);
	_sw( 0x0040A021 , text_addr + 0x00001610 );//addu $s4, $v0, $zr 
	_sw( 0 , text_addr + 0x00001620 );
	_sw( 0x1000FFCD , text_addr + 0x00001628 );

	//change info string
	_sw(MAKE_CALL(GetCategorySubinfo), text_addr + 0x0000A420);
	_sw( 0x100000D8 , text_addr + 0x0000A0AC );
	_sw( 0x02602821 , text_addr + 0x0000A424 );//addu $a1, $s3, $zr

	//patch folder name
	_sw(MAKE_CALL(GetCategoryFolderName), text_addr + 0x0000FE54);
	_sw(MAKE_CALL(GetCategoryFolderName), text_addr + 0x00012908);
	_sw( 0x10000019 , text_addr + 0x0000FDE4 );
	_sw( 0x1000001B , text_addr + 0x0001288C );
	_sw( 0x00809821 , text_addr + 0x00012900 );//addu $s3, $a0, $zr 
	_sw( 0x02602021 , text_addr + 0x0000FE58 );//addu $a0, $s3, $zr
	_sw( 0x02602021 , text_addr + 0x0001290C );//addu $a0, $s3, $zr

	_sw( 0x02602821 , text_addr + 0x0000FE6C );//addu $a1, $s3, $zr
	_sw( 0x02602821 , text_addr + 0x00012920 );//addu $a1, $s3, $zr

	//patch menu string
	_sw(MAKE_CALL(GetCategoryMenuName), text_addr + 0x0001176C);
	_sw(MAKE_CALL(GetCategoryMenuName), text_addr + 0x000123A4);
}
