	.set noreorder

#include "pspimport.s"

// Build files
// SystemCtrlForKernel_0000.o SystemCtrlForKernel_0001.o SystemCtrlForKernel_0002.o SystemCtrlForKernel_0003.o SystemCtrlForKernel_0004.o SystemCtrlForKernel_0005.o SystemCtrlForKernel_0006.o SystemCtrlForKernel_0007.o SystemCtrlForKernel_0008.o SystemCtrlForKernel_0009.o SystemCtrlForKernel_0010.o SystemCtrlForKernel_0011.o SystemCtrlForKernel_0012.o SystemCtrlForKernel_0013.o SystemCtrlForKernel_0014.o SystemCtrlForKernel_0015.o SystemCtrlForKernel_0016.o SystemCtrlForKernel_0017.o SystemCtrlForKernel_0018.o SystemCtrlForKernel_0019.o SystemCtrlForKernel_0020.o SystemCtrlForKernel_0021.o SystemCtrlForKernel_0022.o SystemCtrlForKernel_0023.o SystemCtrlForKernel_0024.o SystemCtrlForKernel_0025.o SystemCtrlForKernel_0026.o SystemCtrlForKernel_0027.o SystemCtrlForKernel_0028.o SystemCtrlForKernel_0029.o SystemCtrlForKernel_0030.o SystemCtrlForKernel_0031.o SystemCtrlForKernel_0032.o SystemCtrlForKernel_0033.o SystemCtrlForKernel_0034.o SystemCtrlForKernel_0035.o SystemCtrlForKernel_0036.o SystemCtrlForKernel_0037.o SystemCtrlForKernel_0038.o SystemCtrlForKernel_0039.o SystemCtrlForKernel_0040.o SystemCtrlForKernel_0041.o SystemCtrlForKernel_0042.o SystemCtrlForKernel_0043.o SystemCtrlForKernel_0044.o SystemCtrlForKernel_0045.o SystemCtrlForKernel_0046.o SystemCtrlForKernel_0047.o SystemCtrlForKernel_0048.o SystemCtrlForKernel_0049.o SystemCtrlForKernel_0050.o SystemCtrlForKernel_0051.o SystemCtrlForKernel_0052.o SystemCtrlForKernel_0053.o SystemCtrlForKernel_0054.o SystemCtrlForKernel_0055.o SystemCtrlForKernel_0056.o 

#ifdef F_SystemCtrlForKernel_0000
	IMPORT_START "SystemCtrlForKernel",0x00090000
#endif
#ifdef F_SystemCtrlForKernel_0001
	IMPORT_FUNC  "SystemCtrlForKernel",0x2794CCF4,sctrlKernelExitVSH
#endif
#ifdef F_SystemCtrlForKernel_0002
	IMPORT_FUNC  "SystemCtrlForKernel",0x577AF198,sctrlKernelLoadExecVSHDisc
#endif
#ifdef F_SystemCtrlForKernel_0003
	IMPORT_FUNC  "SystemCtrlForKernel",0x94FE5E4B,sctrlKernelLoadExecVSHDiscUpdater
#endif
#ifdef F_SystemCtrlForKernel_0004
	IMPORT_FUNC  "SystemCtrlForKernel",0x75643FCA,sctrlKernelLoadExecVSHMs1
#endif
#ifdef F_SystemCtrlForKernel_0005
	IMPORT_FUNC  "SystemCtrlForKernel",0xABA7F1B0,sctrlKernelLoadExecVSHMs2
#endif
#ifdef F_SystemCtrlForKernel_0006
	IMPORT_FUNC  "SystemCtrlForKernel",0xAF22D576,sctrlKernelLoadExecVSHEf2
#endif
#ifdef F_SystemCtrlForKernel_0007
	IMPORT_FUNC  "SystemCtrlForKernel",0x7B369596,sctrlKernelLoadExecVSHMs3
#endif
#ifdef F_SystemCtrlForKernel_0008
	IMPORT_FUNC  "SystemCtrlForKernel",0xD690750F,sctrlKernelLoadExecVSHMs4
#endif
#ifdef F_SystemCtrlForKernel_0009
	IMPORT_FUNC  "SystemCtrlForKernel",0x2D10FB28,sctrlKernelLoadExecVSHWithApitype
#endif
#ifdef F_SystemCtrlForKernel_0010
	IMPORT_FUNC  "SystemCtrlForKernel",0xEB74FE45,sctrlKernelSetUserLevel
#endif
#ifdef F_SystemCtrlForKernel_0011
	IMPORT_FUNC  "SystemCtrlForKernel",0xD8FF9B99,sctrlKernelSetDevkitVersion
#endif
#ifdef F_SystemCtrlForKernel_0012
	IMPORT_FUNC  "SystemCtrlForKernel",0xD339E2E9,sctrlHENIsSE
#endif
#ifdef F_SystemCtrlForKernel_0013
	IMPORT_FUNC  "SystemCtrlForKernel",0x2E2935EF,sctrlHENIsDevhook
#endif
#ifdef F_SystemCtrlForKernel_0014
	IMPORT_FUNC  "SystemCtrlForKernel",0x1090A2E1,sctrlHENGetVersion
#endif
#ifdef F_SystemCtrlForKernel_0015
	IMPORT_FUNC  "SystemCtrlForKernel",0x5328B431,sctrlHENGetMinorVersion
#endif
#ifdef F_SystemCtrlForKernel_0016
	IMPORT_FUNC  "SystemCtrlForKernel",0x78E46415,sctrlHENFindDriver
#endif
#ifdef F_SystemCtrlForKernel_0017
	IMPORT_FUNC  "SystemCtrlForKernel",0x159AF5CC,sctrlHENFindFunction
#endif
#ifdef F_SystemCtrlForKernel_0018
	IMPORT_FUNC  "SystemCtrlForKernel",0xCE0A654E,sctrlHENLoadModuleOnReboot
#endif
#ifdef F_SystemCtrlForKernel_0019
	IMPORT_FUNC  "SystemCtrlForKernel",0xF988C1DC,sctrlHENPatchSyscall
#endif
#ifdef F_SystemCtrlForKernel_0020
	IMPORT_FUNC  "SystemCtrlForKernel",0x826668E9,sctrlHENPatchSyscall
#endif
#ifdef F_SystemCtrlForKernel_0021
	IMPORT_FUNC  "SystemCtrlForKernel",0x02BFCB5F,sctrlHENPatchSyscall
#endif
#ifdef F_SystemCtrlForKernel_0022
	IMPORT_FUNC  "SystemCtrlForKernel",0x1C90BECB,sctrlHENSetStartModuleHandler
#endif
#ifdef F_SystemCtrlForKernel_0023
	IMPORT_FUNC  "SystemCtrlForKernel",0x159AF5CC,sctrlHENFindFunction
#endif
#ifdef F_SystemCtrlForKernel_0024
	IMPORT_FUNC  "SystemCtrlForKernel",0x745286D1,sctrlHENSetMemory
#endif
#ifdef F_SystemCtrlForKernel_0025
	IMPORT_FUNC  "SystemCtrlForKernel",0x1E5436EE,oe_mallocinit
#endif
#ifdef F_SystemCtrlForKernel_0026
	IMPORT_FUNC  "SystemCtrlForKernel",0xF9584CAD,oe_malloc
#endif
#ifdef F_SystemCtrlForKernel_0027
	IMPORT_FUNC  "SystemCtrlForKernel",0xA65E8BC4,oe_free
#endif
#ifdef F_SystemCtrlForKernel_0028
	IMPORT_FUNC  "SystemCtrlForKernel",0xE34A0D97,oe_mallocterminate
#endif
#ifdef F_SystemCtrlForKernel_0029
	IMPORT_FUNC  "SystemCtrlForKernel",0xBA21998E,sctrlSEGetUmdFile
#endif
#ifdef F_SystemCtrlForKernel_0030
	IMPORT_FUNC  "SystemCtrlForKernel",0x5A35C948,sctrlSESetUmdFile
#endif
#ifdef F_SystemCtrlForKernel_0031
	IMPORT_FUNC  "SystemCtrlForKernel",0xB64186D0,SetUmdFile
#endif
#ifdef F_SystemCtrlForKernel_0032
	IMPORT_FUNC  "SystemCtrlForKernel",0xAC56B90B,GetUmdFile
#endif
#ifdef F_SystemCtrlForKernel_0033
	IMPORT_FUNC  "SystemCtrlForKernel",0xBC939DC1,sctrlSESetBootConfFileIndex
#endif
#ifdef F_SystemCtrlForKernel_0034
	IMPORT_FUNC  "SystemCtrlForKernel",0x70B92B45,sctrlSEGetBootConfFileIndex
#endif
#ifdef F_SystemCtrlForKernel_0035
	IMPORT_FUNC  "SystemCtrlForKernel",0x5CB025F0,sctrlSESetBootConfFileIndex
#endif
#ifdef F_SystemCtrlForKernel_0036
	IMPORT_FUNC  "SystemCtrlForKernel",0xAD4D5EA5,sctrlSESetConfigEx
#endif
#ifdef F_SystemCtrlForKernel_0037
	IMPORT_FUNC  "SystemCtrlForKernel",0x8E426F09,sctrlSEGetConfigEx
#endif
#ifdef F_SystemCtrlForKernel_0038
	IMPORT_FUNC  "SystemCtrlForKernel",0x1DDDAD0C,sctrlSESetConfig
#endif
#ifdef F_SystemCtrlForKernel_0039
	IMPORT_FUNC  "SystemCtrlForKernel",0x16C3B7EE,sctrlSEGetConfig
#endif
#ifdef F_SystemCtrlForKernel_0040
	IMPORT_FUNC  "SystemCtrlForKernel",0x8D5BE1F0,sctrlKernelSetInitApitype
#endif
#ifdef F_SystemCtrlForKernel_0041
	IMPORT_FUNC  "SystemCtrlForKernel",0x2BA770CD,sctrlKernelSetUMDEmuFile
#endif
#ifdef F_SystemCtrlForKernel_0042
	IMPORT_FUNC  "SystemCtrlForKernel",0x128112C3,sctrlKernelSetInitFileName
#endif
#ifdef F_SystemCtrlForKernel_0043
	IMPORT_FUNC  "SystemCtrlForKernel",0x9947E189,FindProc
#endif
#ifdef F_SystemCtrlForKernel_0044
	IMPORT_FUNC  "SystemCtrlForKernel",0x98012538,SetSpeed
#endif
#ifdef F_SystemCtrlForKernel_0045
	IMPORT_FUNC  "SystemCtrlForKernel",0xCC9ADCF8,sctrlHENSetSpeed
#endif
#ifdef F_SystemCtrlForKernel_0046
	IMPORT_FUNC  "SystemCtrlForKernel",0x56CEAF00,sctrlKernelQuerySystemCall
#endif
#ifdef F_SystemCtrlForKernel_0047
	IMPORT_FUNC  "SystemCtrlForKernel",0x053FAC1D,sctrlKernelBootFrom
#endif
#ifdef F_SystemCtrlForKernel_0048
	IMPORT_FUNC  "SystemCtrlForKernel",0x62CAC4CF,sctrlPatchModule
#endif
#ifdef F_SystemCtrlForKernel_0049
	IMPORT_FUNC  "SystemCtrlForKernel",0x2A868045,sctrlModuleTextAddr
#endif
#ifdef F_SystemCtrlForKernel_0050
	IMPORT_FUNC  "SystemCtrlForKernel",0x31C6160D,sctrlSESetDiscType
#endif
#ifdef F_SystemCtrlForKernel_0051
	IMPORT_FUNC  "SystemCtrlForKernel",0xABEF849B,sctrlSEGetDiscType
#endif
#ifdef F_SystemCtrlForKernel_0052
	IMPORT_FUNC  "SystemCtrlForKernel",0x557F0B8C,sctrlGetInitTextAddr
#endif
#ifdef F_SystemCtrlForKernel_0053
	IMPORT_FUNC  "SystemCtrlForKernel",0x72F29A6E,sctrlGetInitTextAddr
#endif
#ifdef F_SystemCtrlForKernel_0054
	IMPORT_FUNC  "SystemCtrlForKernel",0x259B51CE,sctrlSetCustomStartModule
#endif
#ifdef F_SystemCtrlForKernel_0055
	IMPORT_FUNC  "SystemCtrlForKernel",0x603EE1D0,sctrlKernelSetNidResolver
#endif
#ifdef F_SystemCtrlForKernel_0056
	IMPORT_FUNC  "SystemCtrlForKernel",0xB364FBB4,sctrlKernelRand
#endif
#ifdef F_SystemCtrlForKernel_0057
	IMPORT_FUNC  "SystemCtrlForKernel",0x16100529,LZ4_decompress_fast
#endif
