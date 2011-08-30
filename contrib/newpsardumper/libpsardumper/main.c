#include <pspsdk.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspcrypt.h>
#include <psputilsforkernel.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <libpsardumper.h>
#include <pspdecrypt.h>

/* most code here from psppet's psardumper */

PSP_MODULE_INFO("pspPSAR_Driver", 0x1006, 1, 0);
PSP_MAIN_THREAD_ATTR(0);

//#define OVERHEAD    0x150 /* size of encryption block overhead */
static int OVERHEAD;
#define SIZE_A      0x110 /* size of uncompressed file entry = 272 bytes */

int iBase, cbChunk, psarVersion;
int psarPosition;
int decrypted;

// for 1.50 and later, they mangled the plaintext parts of the header
static void Demangle(const u8* pIn, u8* pOut)
{
    u8 buffer[20+0x130];
	u8 K1[0x10] = { 0xD8, 0x69, 0xB8, 0x95, 0x33, 0x6B, 0x63, 0x34, 0x98, 0xB9, 0xFC, 0x3C, 0xB7, 0x26, 0x2B, 0xD7 };
	u8 K2[0x10] = { 0x0D, 0xA0, 0x90, 0x84, 0xAF, 0x9E, 0xB6, 0xE2, 0xD2, 0x94, 0xF2, 0xAA, 0xEF, 0x99, 0x68, 0x71 };
	int i;
	memcpy(buffer+20, pIn, 0x130);
	if (psarVersion == 5) for ( i = 0; i < 0x130; ++i ) { buffer[20+i] ^= K1[i & 0xF]; }
    u32* pl = (u32*)buffer; // first 20 bytes
    pl[0] = 5;
    pl[1] = pl[2] = 0;
    pl[3] = 0x55;
    pl[4] = 0x130;

    sceUtilsBufferCopyWithRange(buffer, 20+0x130, buffer, 20+0x130, 0x7);
	if (psarVersion == 5) for ( i = 0; i < 0x130; ++i ) { buffer[i] ^= K2[i & 0xF]; }
    memcpy(pOut, buffer, 0x130);
}

static int DecodeBlock(const u8* pIn, int cbIn, u8* pOut)
{
    // pOut also used as temporary buffer for mangled input
    // assert((((u32)pOut) & 0x3F) == 0); // must be aligned

	if (decrypted)
	{
		if (pIn != pOut)
		{
			memcpy(pOut, pIn, cbIn);
		}

		return cbIn;
	}

    memcpy(pOut, pIn, cbIn + 0x10); // copy a little more for $10 page alignment

    int ret;
    int cbOut;
    
    if (psarVersion != 1)
	{
		Demangle(pIn+0x20, pOut+0x20); // demangle the inside $130 bytes
	}

    if (*(u32 *)&pOut[0xD0] == 0x0E000000)
    {
		ret = sceMesgd_driver_102DC8AF(pOut, cbIn, &cbOut);
	}
	else if (*(u32 *)&pOut[0xD0] == 0x06000000)
	{
		ret = sceNwman_driver_9555D68D(pOut, cbIn, &cbOut);
	}
	else
	{
		cbOut = pspDecryptPRX(pOut, pOut, cbIn);
		if (cbOut < 0)
		{
			Kprintf("Unknown psar tag.\n");
			return 0xFFFFFFFC;
		}
		else
			return cbOut;
	}	
        
    if (ret != 0)
        return ret; // error

	return cbOut;
}

int pspPSARInit(u8 *dataPSAR, u8 *dataOut, u8 *dataOut2)
{
	int k1 = pspSdkSetK1(0);

	if (memcmp(dataPSAR, "PSAR", 4) != 0)
    {
        pspSdkSetK1(k1);
		return -1;
    }

	decrypted = (*(u32 *)&dataPSAR[0x20] == 0x2C333333); // 3.5X M33, and 3.60 unofficial psar's

	if (decrypted)
	{
		OVERHEAD = 0;
	}
	else
	{
		OVERHEAD = 0x150;
	}

	//oldschool = (dataPSAR[4] == 1); /* bogus update */
	psarVersion = dataPSAR[4];

    int cbOut;
	
    // at the start of the PSAR file,
    //   there are one or two special version data chunks
    // printf("Special PSAR records:\n");
    cbOut = DecodeBlock(&dataPSAR[0x10], OVERHEAD+SIZE_A, dataOut);
    if (cbOut <= 0)
    {
        pspSdkSetK1(k1);
        return cbOut;
    }

    if (cbOut != SIZE_A)
    {
        pspSdkSetK1(k1);
        return -2;
    }

    iBase = 0x10+OVERHEAD+SIZE_A; // after first entry
            // iBase points to the next block to decode (0x10 aligned)

	if (decrypted)
	{
		cbOut = DecodeBlock(&dataPSAR[0x10+OVERHEAD+SIZE_A], *(u32 *)&dataOut[0x90], dataOut2);
		if (cbOut <= 0)
		{
			pspSdkSetK1(k1);
			return -3;		
		}
		   
		iBase += OVERHEAD+cbOut; 
		pspSdkSetK1(k1);
		return 0;
	}
	
	if (psarVersion != 1)
	{
		// second block
		cbOut = DecodeBlock(&dataPSAR[0x10+OVERHEAD+SIZE_A], OVERHEAD+100, dataOut2);
		if (cbOut <= 0)
		{
			//printf("Performing V2.70 test\n"); // version 2.7 is bigger
			cbOut = DecodeBlock(&dataPSAR[0x10+OVERHEAD+SIZE_A], OVERHEAD+144, dataOut2);
			if (cbOut <= 0)
			{
				cbOut = DecodeBlock(&dataPSAR[0x10+OVERHEAD+SIZE_A], OVERHEAD+*(u16 *)&dataOut[0x90], dataOut2);
				if (cbOut <= 0)
				{
					pspSdkSetK1(k1);
					return -4;
				}				
			}
		}
	}
       
	cbChunk = (cbOut + 15) & 0xFFFFFFF0;
	iBase += OVERHEAD+cbChunk; 
	psarPosition = 0;

	pspSdkSetK1(k1);
	return 0;
}

int pspPSARGetNextFile(u8 *dataPSAR, int cbFile, u8 *dataOut, u8 *dataOut2, char *name, int *retSize, int *retPos, int *signcheck)
{
	int k1 = pspSdkSetK1(0);
	int cbOut;

	if (iBase >= (cbFile-OVERHEAD))
	{
		pspSdkSetK1(k1);
		return 0; // no more files
	}

	cbOut = DecodeBlock(&dataPSAR[iBase-psarPosition], OVERHEAD+SIZE_A, dataOut);
	if (cbOut <= 0)
	{
		pspSdkSetK1(k1);
		return -1;
	}
	if (cbOut != SIZE_A)
	{
		pspSdkSetK1(k1);
		return -1;	
	}

	strcpy(name, (const char*)&dataOut[4]);
	u32* pl = (u32*)&dataOut[0x100];
	*signcheck = (dataOut[0x10F] == 2);
        
	// pl[0] is 0
	// pl[1] is the PSAR chunk size (including OVERHEAD)
	// pl[2] is true file size (TypeA=272=SIZE_A, TypeB=size when expanded)
	// pl[3] is flags or version?
	if (pl[0] != 0)
	{
		pspSdkSetK1(k1);
		return -1;
	}

	iBase += OVERHEAD + SIZE_A;
	u32 cbDataChunk = pl[1]; // size of next data chunk (including OVERHEAD)
	u32 cbExpanded = pl[2]; // size of file when expanded

	if (cbExpanded > 0)
	{
		cbOut = DecodeBlock(&dataPSAR[iBase-psarPosition], cbDataChunk, dataOut);
		if (cbOut > 10 && dataOut[0] == 0x78 && dataOut[1] == 0x9C)
		{
			// standard Deflate header

			const u8* pbIn = &dataOut[2]; // after header
			u32 pbEnd;
			int ret = sceKernelDeflateDecompress(dataOut2, cbExpanded, pbIn, &pbEnd);
			
			if (ret == cbExpanded)
			{
                *retSize = ret;				
			}
                    
			else
			{
				pspSdkSetK1(k1);
				return -1;
			}
		}

		else
		{
			iBase -= (OVERHEAD + SIZE_A);
			pspSdkSetK1(k1);
			return -1;
		}
	}

	else if (cbExpanded == 0)
	{
        *retSize = 0; 
		// Directory	
	}
	
	else
	{
		pspSdkSetK1(k1);
		return -1;
	}
        
	iBase += cbDataChunk; 
	*retPos = iBase;
	
	pspSdkSetK1(k1);
	return 1; // morefiles
}

int pspPSARSetBufferPosition(int position)
{
	int k1 = pspSdkSetK1(0);
	psarPosition = position;
	pspSdkSetK1(k1);
	return 0;
}

int module_start(SceSize args, void *argp)
{
	return 0;
}

int module_stop(void)
{
	return 0;
}
