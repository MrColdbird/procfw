#ifndef _XMBISO_H_
#define _XMBISO_H_

#include <pspiofilemgr.h>
#include <psploadexec_kernel.h>

//------ structures ------
typedef struct {
	u32 magic;
	u32 version;
	u32 paramsfo;
	u32 icon0png;
	u32 icon1pmf;
	u32 pic0png;
	u32 pic1png;
	u32 snd0at3;
	u32 datapsp;
	u32 datapsar;
} EBOOTHeader;

//------ stage 1 - fake directory existence ------

//open directory
SceUID gamedopen(const char * dirname);

//read directory
int gamedread(SceUID fd, SceIoDirent * dir);

//close directory
int gamedclose(SceUID fd);

//------ stage 2 - fake file existence ------

//create and open temporary eboot
SceUID opentemp(const char * file, int flags, SceMode mode);

//open file
SceUID gameopen(const char * file, int flags, SceMode mode);

//read file
int gameread(SceUID fd, void * data, SceSize size);

//close file
int gameclose(SceUID fd);

//seek file
SceOff gamelseek(SceUID fd, SceOff offset, int whence);

//get file status
int gamegetstat(const char * file, SceIoStat * stat);

//remove file
int gameremove(const char * file);

//remove folder
int gamermdir(const char * path);

//load and execute file
int gameloadexec(char * file, struct SceKernelLoadExecVSHParam * param);

//rename file
int gamerename(const char *oldname, const char *newfile);

#endif
