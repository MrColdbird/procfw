
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspsdk.h>
#include <stdlib.h>
#include <string.h>
#include <psploadexec_kernel.h>
#include <psputility.h>
#include <pspgu.h>
#include <psprtc.h>
#include <pspdisplay.h>

#include <pspnet.h>
#include <pspnet_inet.h>
#include <pspnet_apctl.h>
#include <pspnet_resolver.h>
#include <psputility_netmodules.h>
#include <psputility_netconf.h>
#include <psputility_sysparam.h>

#include <systemctrl.h>
#include <systemctrl_se.h>

#define printf pspDebugScreenPrintf

PSP_MODULE_INFO("Download", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

#if _PSP_FW_VERSION == 639
#define PBP_URL "/2011_0524_ccce1a0f3ba08e22c26ec5bc047a0063"
#define SAVE_PATH "ms0:/PSP/GAME/UPDATE/639.PBP"
#endif


int start_download(char *url , char *path , char* flag);
int sceDisplaySetHoldMode(int a0);

static unsigned int __attribute__((aligned(16))) gu_list[262144];

//sub_00000280
static int InitNetModules()
{
	int res;
	if((res = sceNetInit( 0x20000 , 42 , 0, 42 , 0)) >=0) {
		if((res = sceNetInetInit()) >= 0) {
			if((res = sceNetResolverInit()) >= 0) {
				res = sceNetApctlInit( 0x5400 , 0x30 );
			}
		}
	}

	return res;
}

//sub_000002E4
static int LoadNetModules()
{
	int res;
	if( (res =sceUtilityLoadNetModule( PSP_NET_MODULE_COMMON)) < 0) {
		printf("uLoadNetModules: Error 0x%08X loading pspnet.prx.\n" , res);
		return res;
	}

	if( (res =sceUtilityLoadNetModule( PSP_NET_MODULE_INET )) < 0) {
		printf("uLoadNetModules: Error 0x%08X loading pspnet_inet.prx.\n" , res);
		return res;
	}

	if( (res =sceUtilityLoadNetModule( PSP_NET_MODULE_PARSEURI )) < 0) {
		printf("uLoadNetModules: Error 0x%08X loading parseuri.\n" , res);
		return res;
	}

	if( (res =sceUtilityLoadNetModule( PSP_NET_MODULE_PARSEHTTP )) < 0) {
		printf("uLoadNetModules: Error 0x%08X loading parsehttp.\n" , res);
		return res;
	}

	if( (res =sceUtilityLoadNetModule( PSP_NET_MODULE_HTTP )) < 0) {
		printf("uLoadNetModules: Error 0x%08X loading libhttp.prx.\n" , res);
		return res;
	}

	return 0;
}

static void drawStuff(u32 color)
{
	sceGuStart(0 , &gu_list);
	sceGuClearColor( color);
	sceGuClearDepth(0);
	sceGuClear(5);
	sceGuFinish();
	sceGuSync(0 , 0);
}

//sub_00000400
int ConnectInternet()
{
	int lang , button;
	pspUtilityNetconfData NetData;

	sceUtilityGetSystemParamInt( PSP_SYSTEMPARAM_ID_INT_LANGUAGE , &lang);
	sceUtilityGetSystemParamInt( 9 , &button);

	memset( &NetData, 0 ,sizeof(pspUtilityNetconfData));
	NetData.base.size			= sizeof(pspUtilityNetconfData);
	NetData.base.language		= lang;
	NetData.base.buttonSwap		= button;
	NetData.base.graphicsThread	= 17;
	NetData.base.accessThread	= 19;
	NetData.base.fontThread		= 18;
	NetData.base.soundThread	= 16;
	NetData.action= PSP_NETCONF_ACTION_CONNECTAP;

	sceUtilityNetconfInitStart(&NetData);
	int done=0;

	while(1) {
		drawStuff( 0xFF000000 );

		switch(sceUtilityNetconfGetStatus()) {
			//case PSP_UTILITY_DIALOG_NONE:
			//	break;

			case PSP_UTILITY_DIALOG_VISIBLE://2
				sceUtilityNetconfUpdate(1);
				break;

			case PSP_UTILITY_DIALOG_QUIT://3
				sceUtilityNetconfShutdownStart();
				done = 1;
				break;
				
			//case PSP_UTILITY_DIALOG_FINISHED:
			//	break;

			default:
				break;
		}

		sceDisplayWaitVblankStart();
		sceGuSwapBuffers();
		
		if(done)
			break;
	}

	return 1;
}

#define BUF_WIDTH (512)
#define SCR_WIDTH (480)
#define SCR_HEIGHT (272)

static void setupGu()
{
	sceGuInit();
   	sceGuStart(GU_DIRECT, gu_list );
   	sceGuDrawBuffer(GU_PSM_8888,(void*)0, BUF_WIDTH );
   	sceGuDispBuffer(SCR_WIDTH,SCR_HEIGHT,(void*)0x88000,BUF_WIDTH);
   	sceGuDepthBuffer((void*)0x110000,BUF_WIDTH);
   	sceGuOffset(2048 - (SCR_WIDTH/2),2048 - (SCR_HEIGHT/2));
   	sceGuViewport(2048,2048,SCR_WIDTH,SCR_HEIGHT);
   	sceGuDepthRange(0xc350,0x2710);
   	sceGuScissor(0,0,SCR_WIDTH,SCR_HEIGHT);
   	sceGuEnable(GU_SCISSOR_TEST);
   	sceGuDepthFunc(GU_GEQUAL);
   	sceGuEnable(GU_DEPTH_TEST);
   	sceGuFrontFace(GU_CW);
   	sceGuShadeModel(GU_SMOOTH);
  	sceGuEnable(GU_CULL_FACE);
   	sceGuEnable(GU_CLIP_PLANES);
   	sceGuFinish();
   	sceGuSync(0,0);

   	sceDisplayWaitVblankStart();
   	sceGuDisplay(GU_TRUE);
}

char *region_list[11] = {
	"jp",
	"us",
	"eu",
	"oa",
	"ob",
	"oc",
	"od",
	"oe",
	"of",
	"og",
	"oh"
};

static void create_url( char *buff , int cnt )
{
	char region[8] = "d 01";

	region[1] = region_list[cnt][0];
	if( region[1] == 'o' )
	{
		region[4] = region_list[cnt][1];
		region[5] = 0;
	}

	strcpy( buff , "http://");
	strcat( buff , region );
	strcat( buff , ".psp.update.playstation.org/update/psp/image/");
	strcat( buff , region_list[cnt] );
	strcat( buff , PBP_URL );
	strcat( buff , "/EBOOT.PBP");

}

int main(int argc, char *argv[])
{
	u64 tick;
	SceKernelUtilsMt19937Context ctx;

	char download_url[256];

	sceRtcGetCurrentTick(&tick);
	sceKernelUtilsMt19937Init(&ctx, tick);
	u32 rand_val = sceKernelUtilsMt19937UInt(&ctx);

	create_url( download_url , rand_val % 11);
	pspDebugScreenSetTextColor( (0x00F0F0F0 & rand_val) ^ (0x000F0F0F & rand_val) << 4 );

	LoadNetModules();
	InitNetModules();

	setupGu();
	drawStuff( 0xFF0000CC );
	ConnectInternet();

	sceKernelDelayThread(400*1000);

	pspDebugScreenInit();
	pspDebugScreenClear();

//	int r = sub_0000071C( download_url , "ms0:/PSP/GAME/UPDATE/500.PBP" , NULL );
	int r = start_download( download_url , SAVE_PATH , "PSPUpdate-agent/1.0.0 libhttp/1.0.0" );
	
	if(r != 0){
		printf("uFetchFile: Error 0x%08X\n",r);

		sceKernelDelayThread( 0x4C4B40 );
		sceKernelExitGame();
		return sceKernelSleepThread();
	}


	struct SceKernelLoadExecVSHParam param;

	printf("\nReturning to updater...\n");
	sceDisplaySetHoldMode(1);

	memset( &param , 0 , sizeof(param) );
	param.size= sizeof(param);
	param.args= sizeof("ms0:/PSP/GAME/UPDATE/PUPD.PBP");
	param.argp="ms0:/PSP/GAME/UPDATE/PUPD.PBP";
	param.key="updater";

	sctrlKernelLoadExecVSHMs1( "ms0:/PSP/GAME/UPDATE/PUPD.PBP" ,&param);
	sceKernelExitGame();
	return 0;
}
