
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspsdk.h>
#include <psphttp.h>
#include <stdlib.h>
#include <string.h>

#include <psprtc.h>
#include <psppower.h>

#include <pspnet.h>
#include <pspnet_inet.h>
#include <pspnet_apctl.h>
#include <pspnet_resolver.h>

#include <psputility_netmodules.h>
#include <psputility_sysparam.h>

#define printf pspDebugScreenPrintf

u32 cached_size = 0;
u32 malloc_size = 0;
SceUID fd ;
char stock[0x19000];
char *space = NULL;

//sub_00000540
int SaveCache()
{
	if (sceIoWrite( fd ,space , cached_size) != cached_size) {
		return -1;
	}

	cached_size = 0;
	return 0;
}

//sub_00000688
int StoreCache(void *buff , int size)
{
	if( malloc_size <= (size + cached_size)){
		if( SaveCache() < 0)
			return -1;
	}

	memcpy( space + cached_size , buff , size);
	cached_size += size;
	return size;
}

//sub_0000071C
int start_download(char *url , char *path , char* flag)//download
{
	int tid;
	int cid;
	int rid;
	int statcode;
	SceULong64 length;

	SceSize memsize = sceKernelMaxFreeMemSize();

	if(memsize >= 0x01E00000)
	{
		//sctrlHENSetMemory( 24 , 28);
		malloc_size = 0x01C00000;
	}
	else
	{
		malloc_size = 0x00800000;
	}

	SceUID  dlmem = sceKernelAllocPartitionMemory(2 , "" , 0 , malloc_size + 256 , NULL);

	if(dlmem < 0){
		printf("Error 0x%08X allocating memory.\n", dlmem);
		return -1;
	}

	//data121440
	space = (char *)sceKernelGetBlockHeadAddr(dlmem);


	if(sceHttpInit(20000) <0)
		return 0x12340000;

	tid = sceHttpCreateTemplate( flag ? flag : "M33Update-agent/0.0.1 libhttp/1.0.0" , 1, 1 );

	if(tid <0)
		return 0x12340001;

	if(sceHttpSetResolveTimeOut( tid , 3*1000*1000) < 0)
		return 0x12340002;

	if(sceHttpSetRecvTimeOut( tid , 0x3938700 ) < 0)
		return 0x12340003;

	if(sceHttpSetSendTimeOut( tid , 0x3938700) < 0)
		return 0x12340004;

	cid = sceHttpCreateConnectionWithURL( tid ,url , 0);

	if(cid < 0)
		return 0x12340005;

	rid = sceHttpCreateRequestWithURL( cid , PSP_HTTP_METHOD_GET , url , 0);

	if(rid < 0)
		return 0x12340006;

	if(sceHttpSendRequest( rid , NULL , 0) < 0)
		return 0x12340007;

	if(sceHttpGetStatusCode( rid , &statcode) < 0)
		return 0x12340008;

	if(statcode != 200)
		return 0x12340009;

	if(sceHttpGetContentLength( rid , &length ) < 0)
		return 0x1234000A;

	printf("File size: %d bytes (%d MB)\n", (u32)length , (u32)(length >> 20));

	int s7 = pspDebugScreenGetX();
	int s6 = pspDebugScreenGetY();

	fd =sceIoOpen( path , PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC , 511);
	if(fd < 0 ){
		printf("Error creating file.\n");
		return fd;
	}

	sceIoLseek( fd , length , PSP_SEEK_SET );
	sceIoLseek( fd , 0 , PSP_SEEK_SET );

	u64 c_tick;
	u64 f_tick;
	sceRtcGetCurrentTick( &c_tick );

	f_tick = c_tick;

	u32 save_size=0;
	int ret;
	u32 percent = 0;
	u32 speed=0;

	while((ret = sceHttpReadData( rid , stock , 0x19000)) != 0)
	{
		if(ret < 0)
		{
			sceIoWrite( fd , path , 4);
			sceIoClose( fd );

			printf("Error 0x%08X in sceHttpReadData.\n" , ret);
			return 0x1234000B;
		}

		//download
		save_size += ret;

		if( StoreCache( stock , ret) != ret) {
			printf("Memory stick write error.\n");
		}

		u32 current_percent = 0;
		if( save_size != 0 ) {
			current_percent = (save_size * 100)/ length;
		}
	
		if( percent != current_percent )	
		{	
			sceRtcGetCurrentTick(&c_tick);
			pspDebugScreenSetXY(s7,s6);

			percent = current_percent;//(save_size * 100)/ length;

			if( (c_tick - f_tick) > 0 )
			{
				if( ((c_tick - f_tick)/ (1000*1000)) && (save_size >> 10) )
					speed = (save_size >> 10)/ ((c_tick - f_tick)/ (1000*1000));
			}

			printf("Downloading... %3u%% (%d KB/s)\n", percent , speed);
		}

		scePowerTick(0);
		//return loop
	}


	
	pspDebugScreenSetXY(s7 , s6);
	printf("Downloading... 100%%\n");

	if ( SaveCache() < 0) {
		printf("Memory Stick write error.\n");
		return -1;
	}

	sceIoClose(fd);
	printf("File saved (size = %d bytes).\n" , save_size);
	return 0;
}