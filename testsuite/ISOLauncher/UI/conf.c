#include <pspkernel.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "strsafe.h"
#include "loader_conf.h"

static int write_u32(int fd, int value)
{
	int ret;

	ret = sceIoWrite(fd, &value, sizeof(value));

	return ret == sizeof(value) ? 0 : -1;
}

static int write_string(int fd, char *buf, u32 size)
{
	int ret;

	ret = sceIoWrite(fd, buf, size);

	return ret == size ? 0 : -1;
}

static int read_u32(int fd, int *value)
{
	int ret;

	ret = sceIoRead(fd, value, sizeof(*value));

	return ret == sizeof(*value) ? 0 : -1;
}

static int read_string(int fd, char *buf, u32 size)
{
	int ret;

	ret = sceIoRead(fd, buf, size);

	return ret == size ? 0 : -1;
}

int save_conf(void)
{
	char path[128];
	int fd;
	
	STRCPY_S(path, g_conf.prx_path);
	STRCAT_S(path, "ploader.conf");

	fd = sceIoOpen(path, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);

	if (fd < 0) {
		return fd;
	}

	write_u32(fd, g_conf.iso_mode);
	write_string(fd, g_conf.iso_path, sizeof(g_conf.iso_path));
	sceIoClose(fd);
	
	return 0;
}

int load_conf(void)
{
	char path[128];
	int fd;
	
	STRCPY_S(path, g_conf.prx_path);
	STRCAT_S(path, "ploader.conf");

	fd = sceIoOpen(path, PSP_O_RDONLY, 0777);

	if (fd < 0) {
		return fd;
	}

	read_u32(fd, (int*)&g_conf.iso_mode);
	read_string(fd, g_conf.iso_path, sizeof(g_conf.iso_path));
	sceIoClose(fd);
	
	return 0;
}
