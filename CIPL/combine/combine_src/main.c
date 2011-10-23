#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//#include <zlib.h>

#define u8	unsigned char
#define u16 unsigned short
#define u32	unsigned int

int ReadFile(char *file, void *buf, int size)
{
	FILE *f = fopen(file, "rb");
	
	if (!f)
		return -1;

	int rd = fread(buf, 1, size, f);
	fclose(f);

	return rd;
}

int WriteFile(char *file, void *buf, int size)
{
	FILE *f = fopen(file, "wb");
	
	if (!f)
		return -1;	

	int wt = fwrite(buf, 1, size, f);
	fclose(f);

	return wt;
}


void usage(char *prog)
{
	printf("Usage: %s [-l size ] basefile input output\n", prog);
}

// 0x1000
// 0x4000

int main(int argc, char *argv[])
{
	u8 *input, *output;
	u32 input_size = 0;
	FILE *f;
	int res = 0;
	char *outfile;

	if (argc < 2 || argc > 6)
	{
		usage(argv[0]);
		return -1;
	}

	if (strcmp(argv[1], "-l") == 0)
	{		
		argv++;
		argc--;

		input_size = strtol(argv[1], NULL, 16);		
		printf("Input size: 0x%08X\n", input_size );

		argv++;
		argc--;
	}

	if( input_size < 0x4000 )
		input_size = 0x4000;

	f = fopen(argv[1], "rb");
	if (!f)
	{
		printf("Cannot open %s\n", argv[1]);
		return -1;
	}

	fseek(f, 0, SEEK_SET);

	output = malloc( input_size ); 
	if (!output)
	{
		printf("Cannot allocate memory for output buffer.\n");
		return -1;
	}

	memset( output , 0 , input_size );

	fread( output , 1, 0x1000 , f);
	fclose(f);

	f = fopen(argv[2], "rb");
	if (!f)
	{
		printf("Cannot open %s\n", argv[2]);
		free(output);
		return -1;
	}
	fseek(f, 0, SEEK_SET);
	fread( output + 0x1000 , 1, 0x3000 , f);
	fclose(f);
	
	if (WriteFile( argv[3] , output, input_size ) != input_size )
	{
		printf("Error writing file %s.\n", outfile);	
		res = -1;
	}

	free(output);
	return res;
}

