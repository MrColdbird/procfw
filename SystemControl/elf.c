#include <pspsdk.h>
#include <pspsysmem_kernel.h>
#include <pspkernel.h>
#include <psputilsforkernel.h>
#include <pspsysevent.h>
#include <pspiofilemgr.h>
#include <stdio.h>
#include <string.h>
#include <psploadexec_kernel.h>
#include <string.h>
#include "systemctrl.h"
#include "utils.h"
#include "elf.h"

int IsStaticElf(void * buf)
{
	Elf32_Ehdr * header = (Elf32_Ehdr *)buf;

	if (header->e_magic == 0x464C457F && header->e_type == 2) {
		return 1;
	}

	return 0;
}

char * GetStrTab(unsigned char * buf)
{
	Elf32_Ehdr * header = (Elf32_Ehdr *)buf;

	if (header->e_magic != 0x464C457F)
		return NULL;

	unsigned char *pData = buf+header->e_shoff;

	int i = 0; for(; i < header->e_shnum; i++) {
		if (header->e_shstrndx == i) {
			Elf32_Shdr * section = (Elf32_Shdr *)pData;

			if (section->sh_type == 3)
				return (char *)buf+section->sh_offset;
		}

		pData += header->e_shentsize;

	}

	return NULL;
}
