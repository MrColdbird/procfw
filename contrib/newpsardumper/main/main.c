// PSAR dumper for Updater data 
// Original author: PspPet
//
// Contributions:
//
// Vampire (bugfixes)
// Nem (ipl decryption)
// Dark_AleX (2.60-2.80 decryption)
// Noobz (3.00-3.02 decryption)
// Team C+D (3.03-3.52 decryption)
// M33 Team (3.60-3.71 decryption) + recode for 2.XX+ kernels 
// bbtgp (6.00-6.20 decryption)

#include <pspsdk.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <pspsuspend.h>
#include <psppower.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <kubridge.h>

#include <libpsardumper.h>
#include <pspdecrypt.h>

PSP_MODULE_INFO("NewPsarDumper", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(0);

#define printf    pspDebugScreenPrintf

////////////////////////////////////////////////////////////////////
// big buffers for data. Some system calls require 64 byte alignment

// big enough for the full PSAR file

#define PSAR_BUFFER_SIZE	9400000

static u8 g_dataPSAR[PSAR_BUFFER_SIZE] __attribute__((aligned(64))); 

// big enough for the largest (multiple uses)
static u8 g_dataOut[3000000] __attribute__((aligned(0x40)));
   
// for deflate output
//u8 g_dataOut2[3000000] __attribute__((aligned(0x40)));
static u8 *g_dataOut2;   

void ErrorExit(int milisecs, char *fmt, ...)
{
	va_list list;
	char msg[256];	

	va_start(list, fmt);
	vsprintf(msg, fmt, list);
	va_end(list);

	printf(msg);
	
	//sceKernelDelayThread(milisecs*1000);
	
	printf("\n\nPress X to exit\n");
	
	while (1)
	{
		SceCtrlData pad;
		sceCtrlReadBufferPositive(&pad, 1);
		if (pad.Buttons & PSP_CTRL_CROSS)
			break;
		sceKernelDelayThread(10000);
	}

	
	sceKernelExitGame();
}

////////////////////////////////////////////////////////////////////
// File helpers

int ReadFile(char *file, int seek, void *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_RDONLY, 0);
	if (fd < 0)
		return fd;

	if (seek > 0)
	{
		if (sceIoLseek(fd, seek, PSP_SEEK_SET) != seek)
		{
			sceIoClose(fd);
			return -1;
		}
	}

	int read = sceIoRead(fd, buf, size);
	
	sceIoClose(fd);
	return read;
}

int WriteFile(char *file, void *buf, int size)
{
	SceUID fd = sceIoOpen(file, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
	
	if (fd < 0)
	{
		return fd;
	}

	int written = sceIoWrite(fd, buf, size);

	sceIoClose(fd);
	return written;
}

static char com_table[0x4000];
static int comtable_size;

static char _1g_table[0x4000];
static int _1gtable_size;

static char _2g_table[0x4000];
static int _2gtable_size;

static char _3g_table[0x4000];
static int _3gtable_size;

static char _4g_table[0x4000];
static int _4gtable_size;

static char _5g_table[0x4000];
static int _5gtable_size;

static char _6g_table[0x4000];
static int _6gtable_size;

static char _7g_table[0x4000];
static int _7gtable_size;

static char _8g_table[0x4000];
static int _8gtable_size;

static char _9g_table[0x4000];
static int _9gtable_size;

static char _10g_table[0x4000];
static int _10gtable_size;

static char _11g_table[0x4000];
static int _11gtable_size;

static char _12g_table[0x4000];
static int _12gtable_size;

enum
{
	MODE_ENCRYPT_SIGCHECK,
	MODE_ENCRYPT,
	MODE_DECRYPT,
};

static int FindTablePath(char *table, int table_size, char *number, char *szOut)
{
	int i, j, k;

	for (i = 0; i < table_size-5; i++)
	{
		if (strncmp(number, table+i, 5) == 0)
		{
			for (j = 0, k = 0; ; j++, k++)
			{
				if (table[i+j+6] < 0x20)
				{
					szOut[k] = 0;
					break;
				}

				if (table[i+5] == '|' && !strncmp(table+i+6, "flash", 5) &&
					j == 6)
				{
					szOut[6] = ':';
					szOut[7] = '/';
					k++;
				}
				else if (table[i+5] == '|' && !strncmp(table+i+6, "ipl", 3) &&
					j == 3)
				{
					szOut[3] = ':';
					szOut[4] = '/';
					k++;
				}
				else
				{				
					szOut[k] = table[i+j+6];
				}
			}

			return 1;
		}
	}

	return 0;
}

static int FindReboot(u8 *input, u8 *output, int size)
{
	int i;

	for (i = 0; i < (size - 0x30); i++)
	{
		if (memcmp(input+i, "~PSP", 4) == 0)
		{
			size = *(u32 *)&input[i+0x2C];

			memcpy(output, input+i, size);
			return size;
		}
	}

	return -1;
}

static void ExtractReboot(int mode, char *loadexec, char *reboot, char *rebootname)
{
	int s = ReadFile(loadexec, 0, g_dataOut, sizeof(g_dataOut));

	if (s <= 0)
		return;
	
	printf("Extracting %s... ", rebootname);

	if (mode != MODE_DECRYPT)
	{
		if (mode == MODE_ENCRYPT_SIGCHECK)
		{
			memcpy(g_dataOut2, g_dataOut, s);
			pspSignCheck(g_dataOut2);

			if (WriteFile(loadexec, g_dataOut2, s) != s)
			{
				ErrorExit(5000, "Cannot write %s.\n", loadexec);
			}
		}
			
		s = pspDecryptPRX(g_dataOut, g_dataOut2, s);
		if (s <= 0)
		{
			ErrorExit(5000, "Cannot decrypt %s.\n", loadexec);
		}

		s = pspDecompress(g_dataOut2, g_dataOut, sizeof(g_dataOut));
		if (s <= 0)
		{
			ErrorExit(5000, "Cannot decompress %s.\n", loadexec);
		}
	}

	s = FindReboot(g_dataOut, g_dataOut2, s);
	if (s <= 0)
	{
		ErrorExit(5000, "Cannot find %s inside loadexec.\n", rebootname);
	}

	s = pspDecryptPRX(g_dataOut2, g_dataOut, s);
	if (s <= 0)
	{
		ErrorExit(5000, "Cannot decrypt %s.\n", rebootname);
	}

	WriteFile(reboot, g_dataOut, s);

	s = pspDecompress(g_dataOut, g_dataOut2, sizeof(g_dataOut));
	if (s <= 0)
	{
		ErrorExit(5000, "Cannot decompress %s (0x%08X).\n", rebootname, s);
	}

	if (WriteFile(reboot, g_dataOut2, s) != s)
	{
		ErrorExit(5000, "Cannot write %s.\n", reboot);
	}

	printf("done.\n");
}

static char *GetVersion(char *buf)
{
	char *p = strrchr(buf, ',');

	if (!p)
		return NULL;

	return p+1;
}

static int is5Dnum(char *str)
{
	int len = strlen(str);

	if (len != 5)
		return 0;

	int i;

	for (i = 0; i < len; i++)
	{
		if (str[i] < '0' || str[i] > '9')
			return 0;
	}

	return 1;
}

int LoadStartModule(char *module, int partition)
{
	SceUID mod = kuKernelLoadModule(module, 0, NULL);

	if (mod < 0)
		return mod;

	return sceKernelStartModule(mod, 0, NULL, NULL, NULL);
}

int main(void)
{
    int mode=0, s, res;
	u8 pbp_header[0x28];
	SceUID fd;
	int error = 0;
	int psar_pos = 0, psar_offs;
	int table_mode;
	int psarVersion = 0;
	
	pspDebugScreenInit();

	if (sceKernelDevkitVersion() < 0x02070110)
	{
		ErrorExit(10000, "This program requires 2.71 or higher.\n",
			             "If you are in a cfw, please reexecute psardumper on the higher kernel.\n");
	}

	SceUID mod = LoadStartModule("libpsardumper.prx", PSP_MEMORY_PARTITION_KERNEL);
	if (mod < 0)
	{
		ErrorExit(5000, "Error 0x%08X loading/starting libpsardumper.prx.\n", mod);
	}

	mod = LoadStartModule("pspdecrypt.prx", PSP_MEMORY_PARTITION_KERNEL);
	if (mod < 0)
	{
		ErrorExit(5000, "Error 0x%08X loading/starting pspdecrypt.prx.\n", mod);
	}

    printf("PSAR Dumper\n");
    printf(" by PspPet\n");
   
	printf("2.80 Decryption by Dark_AleX.\n");
	printf("3.00 Decryption by Team Noobz.\n");
	printf("3.03 + 3.10 + 3.30 Decryption by Team C+D.\n");
	printf("3.60 + 3.71 decryption by M33 Team.\n");

	
	printf("Press cross to dump encrypted with sigcheck and decrypted reboot.bin.\n");
	printf("Press circle to dump encrypted without sigcheck and decrypted reboot.bin.\n");
	printf("Press square to decrypt all.\n\n");

	while (1)
	{
		SceCtrlData pad;

		sceCtrlReadBufferPositive(&pad, 1);

		if (pad.Buttons & PSP_CTRL_CROSS)
		{
			mode = MODE_ENCRYPT_SIGCHECK;
			break;
		}
		else if (pad.Buttons & PSP_CTRL_CIRCLE)
		{
			mode = MODE_ENCRYPT;
			break;
		}
		else if (pad.Buttons & PSP_CTRL_SQUARE)
		{
			mode = MODE_DECRYPT;
			break;
		}

		sceKernelDelayThread(10000);
	}

	sceKernelVolatileMemLock(0, (void *)&g_dataOut2, &s);

	printf("Loading psar...\n");

	if (ReadFile("ms0:/EBOOT.PBP", 0, pbp_header, sizeof(pbp_header)) != sizeof(pbp_header))
	{
		ErrorExit(5000, "Cannot find EBOOT.PBP at root.\n");
	}

	psar_offs = *(u32 *)&pbp_header[0x24];
	fd = sceIoOpen("ms0:/EBOOT.PBP", PSP_O_RDONLY, 0);
	
	int cbFile = sceIoLseek32(fd, 0, PSP_SEEK_END) - psar_offs;
	sceIoLseek32(fd, psar_offs, PSP_SEEK_SET);

	if (sceIoRead(fd, g_dataPSAR, PSAR_BUFFER_SIZE) <= 0)
	{
		ErrorExit(5000, "Error Reading EBOOT.PBP.\n");
	}
	
	//printf("PSAR file loaded (%d bytes)\n", cbFile);

    if (memcmp(g_dataPSAR, "PSAR", 4) != 0)
    {
        ErrorExit(5000, "Not a PSAR file\n");  		
    }
   
	psarVersion = g_dataPSAR[4];

	res = pspPSARInit(g_dataPSAR, g_dataOut, g_dataOut2);
	if (res < 0)
	{
		ErrorExit(5000, "pspPSARInit failed with error 0x%08X!.\n", res);
	}

	char *version = GetVersion((char *)g_dataOut+0x10);
	printf("Version %s.\n", version);

	if (memcmp(version, "3.8", 3) == 0 || memcmp(version, "3.9", 3) == 0)
	{
		table_mode = 1;
	}
	else if (memcmp(version, "4.", 2) == 0)
	{
		table_mode = 2;
	}
	else if (memcmp(version, "5.", 2) == 0)
	{
		table_mode = 3;
	}
    else if ((memcmp(version, "6.3", 3) == 0) && (psarVersion == 5))
	{
		table_mode = 4;
	}
    else if ((memcmp(version, "6.", 2) == 0) && (psarVersion == 5))
	{
		table_mode = 4;
	}
    else if (memcmp(version, "6.", 2) == 0)
	{
		table_mode = 4;
	}
	else
	{
		table_mode = 0;
	}

	printf("table_mode = %d\n", table_mode);

    sceIoMkdir("ms0:/F0", 0777);
	sceIoMkdir("ms0:/F0/PSARDUMPER", 0777);
	sceIoMkdir("ms0:/F0/data", 0777);
	sceIoMkdir("ms0:/F0/dic", 0777);
	sceIoMkdir("ms0:/F0/font", 0777);
	sceIoMkdir("ms0:/F0/kd", 0777);
	sceIoMkdir("ms0:/F0/vsh", 0777);
	sceIoMkdir("ms0:/F0/data/cert", 0777);
	sceIoMkdir("ms0:/F0/kd/resource", 0777);
	sceIoMkdir("ms0:/F0/vsh/etc", 0777);
	sceIoMkdir("ms0:/F0/vsh/module", 0777);
	sceIoMkdir("ms0:/F0/vsh/resource", 0777);
	sceIoMkdir("ms0:/F0/codepage", 0777);

	
	
    while (1)
	{
		char name[128];
		int cbExpanded;
		int pos;
		int signcheck;

		int res = pspPSARGetNextFile(g_dataPSAR, cbFile, g_dataOut, g_dataOut2, name, &cbExpanded, &pos, &signcheck);

		if (res < 0)
		{
			if (error)			
				ErrorExit(5000, "PSAR decode error, pos=0x%08X.\n", pos);

			int dpos = pos-psar_pos;
			psar_pos = pos;
			
			error = 1;
			memmove(g_dataPSAR, g_dataPSAR+dpos, PSAR_BUFFER_SIZE-dpos);

			if (sceIoRead(fd, g_dataPSAR+(PSAR_BUFFER_SIZE-dpos), dpos) <= 0)
			{
				ErrorExit(5000, "Error Reading EBOOT.PBP.\n");
			}

			pspPSARSetBufferPosition(psar_pos);

			continue;
		}
		else if (res == 0) /* no more files */
		{
			break;
		}

		if (is5Dnum(name))
		{
			if (   strcmp(name, "00001") != 0 && strcmp(name, "00002") != 0 && strcmp(name, "00003") != 0 && strcmp(name, "00004") != 0 && strcmp(name, "00005") != 0
                && strcmp(name, "00006") != 0 && strcmp(name, "00007") != 0 && strcmp(name, "00008") != 0 && strcmp(name, "00009") != 0 && strcmp(name, "00010") != 0 && strcmp(name, "00011") != 0 && strcmp(name, "00012") != 0)
			{
				int found = 0;
				
				if (_1gtable_size > 0)
				{
					found = FindTablePath(_1g_table, _1gtable_size, name, name);
				}

				if (!found && _2gtable_size > 0)
				{
					found = FindTablePath(_2g_table, _2gtable_size, name, name);
				}

				if (!found && _3gtable_size > 0)
				{
					found = FindTablePath(_3g_table, _3gtable_size, name, name);
				}
				
				if (!found && _4gtable_size > 0)
				{
					found = FindTablePath(_4g_table, _4gtable_size, name, name);
				}
				
				if (!found && _5gtable_size > 0)
				{
					found = FindTablePath(_5g_table, _5gtable_size, name, name);
				}

				if (!found && _6gtable_size > 0)
				{
					found = FindTablePath(_6g_table, _6gtable_size, name, name);
				}

				if (!found && _7gtable_size > 0)
				{
					found = FindTablePath(_7g_table, _7gtable_size, name, name);
				}

				if (!found && _8gtable_size > 0)
				{
					found = FindTablePath(_8g_table, _8gtable_size, name, name);
				}

				if (!found && _9gtable_size > 0)
				{
					found = FindTablePath(_9g_table, _9gtable_size, name, name);
				}
				
				if (!found && _10gtable_size > 0)
				{
					found = FindTablePath(_10g_table, _10gtable_size, name, name);
				}
				
				if (!found && _11gtable_size > 0)
				{
					found = FindTablePath(_11g_table, _11gtable_size, name, name);
				}
				if (!found && _12gtable_size > 0)
				{
					found = FindTablePath(_12g_table, _12gtable_size, name, name);
				}

				if (!found)
				{
					ErrorExit(5000, "Part 1 Error: cannot find path of %s.\n", name);
					//printf("Warning: first cannot find path of %s\n", name);
					//sceKernelDelayThread(2*1000*1000);
					error = 0;
					continue;
				}
			}
		}
		
		else if (!strncmp(name, "com:", 4) && comtable_size > 0)
		{
			if (!FindTablePath(com_table, comtable_size, name+4, name))
			{
				ErrorExit(5000, "Part 2 Error: cannot find path of %s.\n", name);
				//printf("Warning: second cannot find path of %s\n", name);
				//sceKernelDelayThread(2*1000*1000);
				error = 0;
				continue;
				//ErrorExit(5000, "Error: cannot find path of %s.\n", name);
			}
		}

		else if (!strncmp(name, "01g:", 4) && _1gtable_size > 0)
		{
			if (!FindTablePath(_1g_table, _1gtable_size, name+4, name))
			{
				ErrorExit(5000, "Error: 01g cannot find path of %s.\n", name);
			}
		}

		else if (!strncmp(name, "02g:", 4) && _2gtable_size > 0)
		{
			if (!FindTablePath(_2g_table, _2gtable_size, name+4, name))
			{
				ErrorExit(5000, "Error: 01g cannot find path of %s.\n", name);
			}
		}

        printf("'%s' ", name);

		char* szFileBase = strrchr(name, '/');
		
		if (szFileBase != NULL)
			szFileBase++;  // after slash
		else
			szFileBase = "err.err";

		if (cbExpanded > 0)
		{
			char szDataPath[128];
			
			if (!strncmp(name, "flash0:/", 8))
			{
				sprintf(szDataPath, "ms0:/F0/%s", name+8);
			}

			else if (!strncmp(name, "flash1:/", 8))
			{
				sprintf(szDataPath, "ms0:/F1/%s", name+8);
			}

			else if (!strcmp(name, "com:00000"))
			{
				comtable_size = pspDecryptTable(g_dataOut2, g_dataOut, cbExpanded, table_mode);
							
				if (comtable_size <= 0)
				{
					ErrorExit(5000, "Cannot decrypt common table.\n");
				}

				if (comtable_size > sizeof(com_table))
				{
					ErrorExit(5000, "Com table buffer too small. Recompile with bigger buffer.\n");
				}

				memcpy(com_table, g_dataOut2, comtable_size);						
				strcpy(szDataPath, "ms0:/F0/PSARDUMPER/common_files_table.bin");
			}

			else if (!strcmp(name, "01g:00000") || !strcmp(name, "00001"))
			{
				_1gtable_size = pspDecryptTable(g_dataOut2, g_dataOut, cbExpanded, table_mode);
					
				if (_1gtable_size <= 0)
				{
					
					ErrorExit(5000, "Cannot decrypt 1g table.\n");
					
				}

				if (_1gtable_size > sizeof(_1g_table))
				{
					ErrorExit(5000, "1g table buffer too small. Recompile with bigger buffer.\n");
				}

				memcpy(_1g_table, g_dataOut2, _1gtable_size);						
				strcpy(szDataPath, "ms0:/F0/PSARDUMPER/1000_files_table.bin");
			}
					
			else if (!strcmp(name, "02g:00000") || !strcmp(name, "00002"))
			{
				_2gtable_size = pspDecryptTable(g_dataOut2, g_dataOut, cbExpanded, table_mode);
							
				if (_2gtable_size <= 0)
				{
					ErrorExit(5000, "Cannot decrypt 2g table %08X.\n", _2gtable_size);
				}

				if (_2gtable_size > sizeof(_2g_table))
				{
					ErrorExit(5000, "2g table buffer too small. Recompile with bigger buffer.\n");
				}

				memcpy(_2g_table, g_dataOut2, _2gtable_size);						
				strcpy(szDataPath, "ms0:/F0/PSARDUMPER/2000_files_table.bin");
			}

			else if (!strcmp(name, "00003"))
			{
				_3gtable_size = pspDecryptTable(g_dataOut2, g_dataOut, cbExpanded, table_mode);
							
				if (_3gtable_size <= 0)
				{
					// We don't have yet the keys for table of 3000, they are only in mesg_led03g.prx
					printf("Cannot decrypt 3g table %08X.\n", _3gtable_size);
					error = 0;
					continue;
				}

				if (_3gtable_size > sizeof(_3g_table))
				{
					ErrorExit(5000, "3g table buffer too small. Recompile with bigger buffer.\n");
				}

				memcpy(_3g_table, g_dataOut2, _3gtable_size);						
				strcpy(szDataPath, "ms0:/F0/PSARDUMPER/3000_files_table.bin");
			}
			else if (!strcmp(name, "00004"))
			{
				_4gtable_size = pspDecryptTable(g_dataOut2, g_dataOut, cbExpanded, table_mode);
							
				if (_4gtable_size <= 0)
				{
					printf("Cannot decrypt 4g table %08X.\n", _4gtable_size);
					error = 0;
					continue;
				}

				if (_4gtable_size > sizeof(_4g_table))
				{
					ErrorExit(5000, "4g table buffer too small. Recompile with bigger buffer.\n");
				}

				memcpy(_4g_table, g_dataOut2, _4gtable_size);						
				strcpy(szDataPath, "ms0:/F0/PSARDUMPER/4000_files_table.bin");
			}
			else if (!strcmp(name, "00005"))
			{
				_5gtable_size = pspDecryptTable(g_dataOut2, g_dataOut, cbExpanded, table_mode);
							
				if (_5gtable_size <= 0)
				{
					printf("Cannot decrypt 5g table %08X.\n", _5gtable_size);
					error = 0;
					continue;
				}

				if (_5gtable_size > sizeof(_5g_table))
				{
					ErrorExit(5000, "5g table buffer too small. Recompile with bigger buffer.\n");
				}

				memcpy(_5g_table, g_dataOut2, _5gtable_size);						
				strcpy(szDataPath, "ms0:/F0/PSARDUMPER/5000_files_table.bin");
			}
			else if (!strcmp(name, "00006"))
			{
				_6gtable_size = pspDecryptTable(g_dataOut2, g_dataOut, cbExpanded, table_mode);
							
				if (_6gtable_size <= 0)
				{
					printf("Cannot decrypt 6g table %08X.\n", _6gtable_size);
					error = 0;
					continue;
				}

				if (_6gtable_size > sizeof(_6g_table))
				{
					ErrorExit(5000, "6g table buffer too small. Recompile with bigger buffer.\n");
				}

				memcpy(_6g_table, g_dataOut2, _6gtable_size);						
				strcpy(szDataPath, "ms0:/F0/PSARDUMPER/6000_files_table.bin");
			}
			else if (!strcmp(name, "00007"))
			{
				_7gtable_size = pspDecryptTable(g_dataOut2, g_dataOut, cbExpanded, table_mode);
							
				if (_7gtable_size <= 0)
				{
					printf("Cannot decrypt 7g table %08X.\n", _7gtable_size);
					error = 0;
					continue;
				}

				if (_7gtable_size > sizeof(_7g_table))
				{
					ErrorExit(5000, "7g table buffer too small. Recompile with bigger buffer.\n");
				}

				memcpy(_7g_table, g_dataOut2, _7gtable_size);						
				strcpy(szDataPath, "ms0:/F0/PSARDUMPER/7000_files_table.bin");
			}
			else if (!strcmp(name, "00008"))
			{
				_8gtable_size = pspDecryptTable(g_dataOut2, g_dataOut, cbExpanded, table_mode);
							
				if (_8gtable_size <= 0)
				{
					printf("Cannot decrypt 8g table %08X.\n", _8gtable_size);
					error = 0;
					continue;
				}

				if (_8gtable_size > sizeof(_8g_table))
				{
					ErrorExit(5000, "8g table buffer too small. Recompile with bigger buffer.\n");
				}

				memcpy(_8g_table, g_dataOut2, _8gtable_size);						
				strcpy(szDataPath, "ms0:/F0/PSARDUMPER/8000_files_table.bin");
			}
			else if (!strcmp(name, "00009"))
			{
				_9gtable_size = pspDecryptTable(g_dataOut2, g_dataOut, cbExpanded, table_mode);
							
				if (_9gtable_size <= 0)
				{
					printf("Cannot decrypt 9g table %08X.\n", _9gtable_size);
					error = 0;
					continue;
				}

				if (_9gtable_size > sizeof(_9g_table))
				{
					ErrorExit(5000, "9g table buffer too small. Recompile with bigger buffer.\n");
				}

				memcpy(_9g_table, g_dataOut2, _9gtable_size);						
				strcpy(szDataPath, "ms0:/F0/PSARDUMPER/9000_files_table.bin");
			}
			else if (!strcmp(name, "00010"))
			{
				_10gtable_size = pspDecryptTable(g_dataOut2, g_dataOut, cbExpanded, table_mode);
							
				if (_10gtable_size <= 0)
				{
					printf("Cannot decrypt 10g table %08X.\n", _10gtable_size);
					error = 0;
					continue;
				}

				if (_10gtable_size > sizeof(_10g_table))
				{
					ErrorExit(5000, "10g table buffer too small. Recompile with bigger buffer.\n");
				}

				memcpy(_10g_table, g_dataOut2, _10gtable_size);						
				strcpy(szDataPath, "ms0:/F0/PSARDUMPER/10000_files_table.bin");
			}
			else if (!strcmp(name, "00011"))
			{
				_11gtable_size = pspDecryptTable(g_dataOut2, g_dataOut, cbExpanded, table_mode);
							
				if (_11gtable_size <= 0)
				{
					printf("Cannot decrypt 11g table %08X.\n", _11gtable_size);
					error = 0;
					continue;
				}

				if (_11gtable_size > sizeof(_11g_table))
				{
					ErrorExit(5000, "11g table buffer too small. Recompile with bigger buffer.\n");
				}

				memcpy(_11g_table, g_dataOut2, _11gtable_size);						
				strcpy(szDataPath, "ms0:/F0/PSARDUMPER/11000_files_table.bin");
			}
			else if (!strcmp(name, "00012"))
			{
				_12gtable_size = pspDecryptTable(g_dataOut2, g_dataOut, cbExpanded, table_mode);
							
				if (_12gtable_size <= 0)
				{
					printf("Cannot decrypt 12g table %08X.\n", _12gtable_size);
					error = 0;
					continue;
				}

				if (_12gtable_size > sizeof(_12g_table))
				{
					ErrorExit(5000, "12g table buffer too small. Recompile with bigger buffer.\n");
				}

				memcpy(_12g_table, g_dataOut2, _12gtable_size);						
				strcpy(szDataPath, "ms0:/F0/PSARDUMPER/12000_files_table.bin");
			}

			else
			{
				sprintf(szDataPath, "ms0:/F0/PSARDUMPER/%s", strrchr(name, '/') + 1);
			}

			printf("expanded"); 

			if (signcheck && mode == MODE_ENCRYPT_SIGCHECK 
				&& (strcmp(name, "flash0:/kd/loadexec.prx") != 0)
				&& (strcmp(name, "flash0:/kd/loadexec_01g.prx") != 0)
				&& (strcmp(name, "flash0:/kd/loadexec_02g.prx") != 0)
				&& (strcmp(name, "flash0:/kd/loadexec_03g.prx") != 0)
				&& (strcmp(name, "flash0:/kd/loadexec_04g.prx") != 0)
				&& (strcmp(name, "flash0:/kd/loadexec_05g.prx") != 0)
				&& (strcmp(name, "flash0:/kd/loadexec_06g.prx") != 0)
				&& (strcmp(name, "flash0:/kd/loadexec_07g.prx") != 0)
				&& (strcmp(name, "flash0:/kd/loadexec_08g.prx") != 0)
				&& (strcmp(name, "flash0:/kd/loadexec_09g.prx") != 0)
				&& (strcmp(name, "flash0:/kd/loadexec_10g.prx") != 0)
				&& (strcmp(name, "flash0:/kd/loadexec_11g.prx") != 0)
				&& (strcmp(name, "flash0:/kd/loadexec_12g.prx") != 0))
			{
				pspSignCheck(g_dataOut2);
			}

			if ((mode != MODE_DECRYPT) || (memcmp(g_dataOut2, "~PSP", 4) != 0))
			{
				if (strstr(szDataPath, "ipl") && (strstr(szDataPath, "2000") || strstr(szDataPath, "02h") || strstr(szDataPath, "02g")))
				{
					// IPL Pre-decryption
					cbExpanded = pspDecryptPRX(g_dataOut2, g_dataOut, cbExpanded);

					if (cbExpanded <= 0)
					{
						printf("Warning: cannot pre-decrypt 2000 IPL.\n");
					}
					else
					{
						memcpy(g_dataOut2, g_dataOut, cbExpanded);
					}							
				}
						
				if (WriteFile(szDataPath, g_dataOut2, cbExpanded) != cbExpanded)
	            {
					ErrorExit(5000, "Cannot write %s.\n", szDataPath);
					break;
				}
	                    
				printf(",saved");
			}

			if ((memcmp(g_dataOut2, "~PSP", 4) == 0) &&
				(mode == MODE_DECRYPT))
			{
				memset(g_dataOut2+0x104,0,0x28);
				int cbDecrypted = pspDecryptPRX(g_dataOut2, g_dataOut, cbExpanded);

				// output goes back to main buffer
				// trashed 'g_dataOut2'
				if (cbDecrypted > 0)
				{
					u8* pbToSave = g_dataOut;
					int cbToSave = cbDecrypted;

					printf(",decrypted");
                            
					if ((g_dataOut[0] == 0x1F && g_dataOut[1] == 0x8B) ||
						memcmp(g_dataOut, "2RLZ", 4) == 0 || memcmp(g_dataOut, "KL4E", 4) == 0)
					{
						int cbExp = pspDecompress(g_dataOut, g_dataOut2, sizeof(g_dataOut));
						
						if (cbExp > 0)
						{
							printf(",expanded");
							pbToSave = g_dataOut2;
							cbToSave = cbExp;
						}
						else
						{
							printf("Decompress error 0x%08X\n"
								   "File will be written compressed.\n", cbExp);
						}
					}
        			
					if (WriteFile(szDataPath, pbToSave, cbToSave) != cbToSave)
					{
						ErrorExit(5000, "Error writing %s.\n", szDataPath);
					}
                    
					printf(",saved!");
				}
				else
				{
					
					printf(",not decrypted.\n");
					
				}
			}

			else if (strncmp(name, "ipl:", 4) == 0)
			{
				sprintf(szDataPath, "ms0:/F0/PSARDUMPER/part1_%s", szFileBase);

				int cb1 = pspDecryptIPL1(g_dataOut2, g_dataOut, cbExpanded);
				if (cb1 > 0 && (WriteFile(szDataPath, g_dataOut, cb1) == cb1))
				{
					int cb2 = pspLinearizeIPL2(g_dataOut, g_dataOut2, cb1);
					sprintf(szDataPath, "ms0:/F0/PSARDUMPER/part2_%s", szFileBase);
					
					WriteFile(szDataPath, g_dataOut2, cb2);
					
					int cb3 = pspDecryptIPL3(g_dataOut2, g_dataOut, cb2);
					sprintf(szDataPath, "ms0:/F0/PSARDUMPER/part3_%s", szFileBase);
					WriteFile(szDataPath, g_dataOut, cb3);
				}
			}
		}
		else if (cbExpanded == 0)
		{
			printf("empty");
		}

		printf("\n");
		error = 0;
		scePowerTick(0);	
	}

	sceIoClose(fd);

	ExtractReboot(mode, "ms0:/F0/kd/loadexec.prx", "ms0:/F0/reboot.bin", "reboot.bin");
	ExtractReboot(mode, "ms0:/F0/kd/loadexec_01g.prx", "ms0:/F0/reboot.bin", "reboot.bin");
	ExtractReboot(mode, "ms0:/F0/kd/loadexec_02g.prx", "ms0:/F0/reboot_02g.bin", "reboot_02g.bin");
	ExtractReboot(mode, "ms0:/F0/kd/loadexec_03g.prx", "ms0:/F0/reboot_03g.bin", "reboot_03g.bin");
	ExtractReboot(mode, "ms0:/F0/kd/loadexec_04g.prx", "ms0:/F0/reboot_04g.bin", "reboot_04g.bin");
	ExtractReboot(mode, "ms0:/F0/kd/loadexec_05g.prx", "ms0:/F0/reboot_05g.bin", "reboot_05g.bin");
	ExtractReboot(mode, "ms0:/F0/kd/loadexec_06g.prx", "ms0:/F0/reboot_06g.bin", "reboot_06g.bin");
	ExtractReboot(mode, "ms0:/F0/kd/loadexec_07g.prx", "ms0:/F0/reboot_07g.bin", "reboot_07g.bin");
	ExtractReboot(mode, "ms0:/F0/kd/loadexec_08g.prx", "ms0:/F0/reboot_08g.bin", "reboot_08g.bin");
	ExtractReboot(mode, "ms0:/F0/kd/loadexec_09g.prx", "ms0:/F0/reboot_09g.bin", "reboot_09g.bin");
	ExtractReboot(mode, "ms0:/F0/kd/loadexec_10g.prx", "ms0:/F0/reboot_10g.bin", "reboot_10g.bin");
	ExtractReboot(mode, "ms0:/F0/kd/loadexec_11g.prx", "ms0:/F0/reboot_11g.bin", "reboot_11g.bin");
	ExtractReboot(mode, "ms0:/F0/kd/loadexec_12g.prx", "ms0:/F0/reboot_12g.bin", "reboot_12g.bin");

    scePowerTick(0);	
	ErrorExit(10000, "Done.\n");

    return 0;
}

