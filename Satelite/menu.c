/*
	PSP VSH MENU controll
	based Booster's vshex
*/

#include "common.h"

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

//
void change_clock(int dir , int a);
//


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//extern char path_buf[256];
extern int pwidth;
extern char umd_path[72];
extern SEConfig cnf;

/////////////////////////////////////////////////////////////////////////////
// draw strings
/////////////////////////////////////////////////////////////////////////////
char freq_buf[3+3+2] = "";
char freq2_buf[3+3+2] = "";

char device_buf[13] = "";


const char str_default[] = "Default";

const char * enable_disable[] ={
	"Enable",
	"Disable"
};
/*
const char *iso[]={
	"Normal",
	"OE isofs",
	"M33 driver",
	"Sony NP9660"
};
*/
#define TMENU_MAX 9

enum{
	TMENU_XMB_CLOCK,
	TMENU_GAME_CLOCK,
	TMENU_USB_DEVICE,
//	TMENU_UMD_MODE,
//	TMENU_UMD_VIDEO,
	TMENU_XMB_PLUGINS,
	TMENU_GAME_PLUGINS,
	TMENU_POPS_PLUGINS,
//	TMENU_PLUGINS_MGR,
	TMENU_SHUTDOWN_DEVICE,
	TMENU_RESET_DEVICE,
	TMENU_EXIT
};

const char *top_menu_list[TMENU_MAX] ={
	"CPU CLOCK XMB  ",
	"CPU CLOCK GAME ",
	"USB DEVICE     ",
//	"UMD ISO MODE   ",
//	"ISO VIDEO MOUNT",
	"XMB  PLUGINS   ",
	"GAME PLUGINS   ",
	"POPS PLUGINS   ",
//	"PLUGINS MANAGER  ->",
	"SHUTDOWN DEVICE",
	"RESET DEVICE",
	"EXIT"
};


int item_fcolor[TMENU_MAX];
char *item_str[TMENU_MAX];

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
static int menu_sel = TMENU_XMB_CLOCK;

const int xyPoint[] ={0x98, 0x30, 0xC0, 0xA0, 0x70, 0x08, 0x0E};//data243C=
const int xyPoint2[] ={0xB0, 0x30, 0xD8, 0xB8, 0x88, 0x08, 0x11};//data2458=


/////////////////////////////////////////////////////////////////////////////
// draw menu 
/////////////////////////////////////////////////////////////////////////////
int menu_draw(void)
{
	u32 fc,bc;
	const char *msg;
//	int menu_len;
	int max_menu;
	int *pointer;
	int xPointer;
	
	// check & setup video mode
	if( blit_setup() < 0) return -1;

	if(pwidth==720)
	{
		pointer = xyPoint;
	}
	else
	{
		pointer = xyPoint2;
	}

	// show menu list
	blit_set_color(0xffffff,0x8000ff00);

//	blit_string_ctr(4*8,"vshex DEVHOOK MENU"); // noise on right side
	blit_string( pointer[0], pointer[1] ,"6.35 VSH MENU");

	
	// menu title & list
	//menu_len  = 12; //get_max_len(top_menu_list,TMENU_MAX);

	for(max_menu=0;max_menu<TMENU_MAX;max_menu++)
	{
		fc = 0xffffff;
		bc = (max_menu==menu_sel) ? 0xff8080 : 0xc00000ff;
		blit_set_color(fc,bc);

		msg = top_menu_list[max_menu];
		if(msg)
		{
			switch(max_menu)
			{
			case TMENU_EXIT:
				xPointer = pointer[2];
				break;
			case TMENU_RESET_DEVICE:
				xPointer = pointer[3];
				break;
			case TMENU_SHUTDOWN_DEVICE:
				xPointer = 176;
				break;
			default:
				xPointer=pointer[4];
				break;
			}

			blit_string( xPointer ,(pointer[5] + max_menu)*8,msg);

			
			msg = item_str[max_menu];
			if(msg)
			{
				blit_set_color(item_fcolor[max_menu],bc);
				blit_string( (pointer[6] * 8) + 128 , (pointer[5] + max_menu)*8 ,msg);
			}
			
		}
	}


	blit_set_color(0x00ffffff,0x00000000);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// setup to menu draw
/////////////////////////////////////////////////////////////////////////////

int menu_setup(void)
{

	int i;
	char *bridge;


	//scePaf_6439FDBC_memset(item_str , 0 , sizeof(item_str));

	// preset
	for(i=0;i<TMENU_MAX;i++)
	{
		item_str[i] = NULL;
		item_fcolor[i] = RGB(255,255,255);
	}

	//xmb clock
	if( cpu2no(cnf.vshcpuspeed) && ( bus2no(cnf.vshbusspeed)))	
	{		
		scePaf_11EFC5FD_sprintf(freq_buf,"%d/%d",cnf.vshcpuspeed,cnf.vshbusspeed);
		bridge = freq_buf;
	}
	else
	{
		//scePaf_98DE3BA6_strcpy(freq_buf, str_default );
		bridge = str_default;
	}
	item_str[TMENU_XMB_CLOCK] = bridge;

	//game clock
	if( cpu2no(cnf.umdisocpuspeed) && ( bus2no(cnf.umdisobusspeed)))	
	{		
		scePaf_11EFC5FD_sprintf(freq2_buf,"%d/%d",cnf.umdisocpuspeed,cnf.umdisobusspeed);
		bridge = freq2_buf;
	}
	else
	{
		//scePaf_98DE3BA6_strcpy(freq2_buf, str_default );
		bridge = str_default;
	}
	item_str[TMENU_GAME_CLOCK] = bridge;

	//usb device
	if((cnf.usbdevice>0) && (cnf.usbdevice<5))
	{
		scePaf_11EFC5FD_sprintf(device_buf,"Flash %d",cnf.usbdevice-1);	
		bridge = device_buf;

	}else
	{
		char *device;

		if(cnf.usbdevice==5)
			device="UMD Disc";
		else
			device="Memory Stick";

		bridge = device;
		//scePaf_98DE3BA6_strcpy(device_buf,device);	
	}	
	item_str[TMENU_USB_DEVICE] = bridge;

	//plugins
	item_str[TMENU_XMB_PLUGINS] =enable_disable[cnf.plugvsh];
	item_str[TMENU_GAME_PLUGINS]=enable_disable[cnf.pluggame];
	item_str[TMENU_POPS_PLUGINS]=enable_disable[cnf.plugpop];


	return 0;
}
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
int menu_ctrl(u32 buttons,u32 button_on)
{
	int direction;

	if( (button_on&PSP_CTRL_SELECT) ||
		(button_on&PSP_CTRL_HOME))
	{
		menu_sel = TMENU_EXIT;
		return 1;
	}

	// change menu select
	direction = 0;
	if(button_on&PSP_CTRL_DOWN) direction++;
	if(button_on&PSP_CTRL_UP) direction--;

	menu_sel = limit(menu_sel+direction,0,TMENU_MAX-1);


	// LEFT & RIGHT
	direction = -2;
	if(button_on&PSP_CTRL_LEFT)   direction = -1;
	if(button_on&PSP_CTRL_CROSS) direction = 0;
	if(button_on&PSP_CTRL_RIGHT)  direction = 1;
	if(direction>-2)
	{
		switch(menu_sel)
		{
				
		case TMENU_XMB_CLOCK:
			if(direction) change_clock( direction , 0 );
			break;

		case TMENU_GAME_CLOCK:
			if(direction) change_clock( direction , 1);
			break;

		case TMENU_USB_DEVICE:
			if(direction) change_usb( direction );
			break;
/*		case TMENU_UMD_MODE:
			if(direction) change_umd_mode( direction );
			break;
		case TMENU_UMD_VIDEO:
			change_umd_video( direction );
			break;
*/
		case TMENU_XMB_PLUGINS:
			if(direction) change_plugins(direction , 0 );
			break;
		case TMENU_GAME_PLUGINS:
			if(direction) change_plugins(direction , 1 );
			break;
		case TMENU_POPS_PLUGINS:
			if(direction) change_plugins(direction , 2 );
			break;
/*
		case TMENU_PLUGINS_MGR:
			if(direction == 0) return 4;
			break;
*/
		case TMENU_SHUTDOWN_DEVICE:			
			if(direction==0)
			{
				return 3; // finish
			}
			break;

		case TMENU_RESET_DEVICE:	
			if(direction==0)
			{
				return 2; // finish
			}
			break;

		case TMENU_EXIT:
			if(direction==0) return 1; // finish
			break;
		}
	}

	return 0; // continue
}

