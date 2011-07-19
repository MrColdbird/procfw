	.set noreorder

#include "pspimport.s"

// Build files
// SystemCtrlForUser_0000.o SystemCtrlForUser_0001.o SystemCtrlForUser_0002.o SystemCtrlForUser_0003.o SystemCtrlForUser_0004.o SystemCtrlForUser_0005.o SystemCtrlForUser_0006.o SystemCtrlForUser_0007.o SystemCtrlForUser_0008.o SystemCtrlForUser_0009.o SystemCtrlForUser_0010.o SystemCtrlForUser_0011.o SystemCtrlForUser_0012.o SystemCtrlForUser_0013.o SystemCtrlForUser_0014.o SystemCtrlForUser_0015.o SystemCtrlForUser_0016.o SystemCtrlForUser_0017.o SystemCtrlForUser_0018.o SystemCtrlForUser_0019.o SystemCtrlForUser_0020.o SystemCtrlForUser_0021.o SystemCtrlForUser_0022.o SystemCtrlForUser_0023.o SystemCtrlForUser_0024.o SystemCtrlForUser_0025.o SystemCtrlForUser_0026.o SystemCtrlForUser_0027.o SystemCtrlForUser_0028.o SystemCtrlForUser_0029.o SystemCtrlForUser_0030.o SystemCtrlForUser_0031.o SystemCtrlForUser_0032.o SystemCtrlForUser_0033.o SystemCtrlForUser_0034.o SystemCtrlForUser_0035.o SystemCtrlForUser_0036.o SystemCtrlForUser_0037.o SystemCtrlForUser_0038.o SystemCtrlForUser_0039.o SystemCtrlForUser_0040.o SystemCtrlForUser_0041.o SystemCtrlForUser_0042.o SystemCtrlForUser_0043.o SystemCtrlForUser_0044.o SystemCtrlForUser_0045.o SystemCtrlForUser_0046.o 

#ifdef F_SystemCtrlForUser_0000
	IMPORT_START "SystemCtrlForUser",0x40090000
#endif
#ifdef F_SystemCtrlForUser_0001
	IMPORT_FUNC  "SystemCtrlForUser",0x2794CCF4,sctrlKernelExitVSH
#endif
#ifdef F_SystemCtrlForUser_0002
	IMPORT_FUNC  "SystemCtrlForUser",0x577AF198,sctrlKernelLoadExecVSHDisc
#endif
#ifdef F_SystemCtrlForUser_0003
	IMPORT_FUNC  "SystemCtrlForUser",0x94FE5E4B,sctrlKernelLoadExecVSHDiscUpdater
#endif
#ifdef F_SystemCtrlForUser_0004
	IMPORT_FUNC  "SystemCtrlForUser",0x75643FCA,sctrlKernelLoadExecVSHMs1
#endif
#ifdef F_SystemCtrlForUser_0005
	IMPORT_FUNC  "SystemCtrlForUser",0xABA7F1B0,sctrlKernelLoadExecVSHMs2
#endif
#ifdef F_SystemCtrlForUser_0006
	IMPORT_FUNC  "SystemCtrlForUser",0xAF22D576,sctrlKernelLoadExecVSHEf2
#endif
#ifdef F_SystemCtrlForUser_0007
	IMPORT_FUNC  "SystemCtrlForUser",0x7B369596,sctrlKernelLoadExecVSHMs3
#endif
#ifdef F_SystemCtrlForUser_0008
	IMPORT_FUNC  "SystemCtrlForUser",0xD690750F,sctrlKernelLoadExecVSHMs4
#endif
#ifdef F_SystemCtrlForUser_0009
	IMPORT_FUNC  "SystemCtrlForUser",0x2D10FB28,sctrlKernelLoadExecVSHWithApitype
#endif
#ifdef F_SystemCtrlForUser_0010
	IMPORT_FUNC  "SystemCtrlForUser",0xEB74FE45,sctrlKernelSetUserLevel
#endif
#ifdef F_SystemCtrlForUser_0011
	IMPORT_FUNC  "SystemCtrlForUser",0xD8FF9B99,sctrlKernelSetDevkitVersion
#endif
#ifdef F_SystemCtrlForUser_0012
	IMPORT_FUNC  "SystemCtrlForUser",0xD339E2E9,sctrlHENIsSE
#endif
#ifdef F_SystemCtrlForUser_0013
	IMPORT_FUNC  "SystemCtrlForUser",0x2E2935EF,sctrlHENIsDevhook
#endif
#ifdef F_SystemCtrlForUser_0014
	IMPORT_FUNC  "SystemCtrlForUser",0x1090A2E1,sctrlHENGetVersion
#endif
#ifdef F_SystemCtrlForUser_0015
	IMPORT_FUNC  "SystemCtrlForUser",0x5328B431,sctrlHENGetMinorVersion
#endif
#ifdef F_SystemCtrlForUser_0016
	IMPORT_FUNC  "SystemCtrlForUser",0x78E46415,sctrlHENFindDriver
#endif
#ifdef F_SystemCtrlForUser_0017
	IMPORT_FUNC  "SystemCtrlForUser",0x159AF5CC,sctrlHENFindFunction
#endif
#ifdef F_SystemCtrlForUser_0018
	IMPORT_FUNC  "SystemCtrlForUser",0xCE0A654E,sctrlHENLoadModuleOnReboot
#endif
#ifdef F_SystemCtrlForUser_0019
	IMPORT_FUNC  "SystemCtrlForUser",0xF988C1DC,sctrlHENPatchSyscall
#endif
#ifdef F_SystemCtrlForUser_0020
	IMPORT_FUNC  "SystemCtrlForUser",0x826668E9,sctrlHENPatchSyscall
#endif
#ifdef F_SystemCtrlForUser_0021
	IMPORT_FUNC  "SystemCtrlForUser",0x02BFCB5F,sctrlHENPatchSyscall
#endif
#ifdef F_SystemCtrlForUser_0022
	IMPORT_FUNC  "SystemCtrlForUser",0x1C90BECB,sctrlHENSetStartModuleHandler
#endif
#ifdef F_SystemCtrlForUser_0023
	IMPORT_FUNC  "SystemCtrlForUser",0x159AF5CC,sctrlHENFindFunction
#endif
#ifdef F_SystemCtrlForUser_0024
	IMPORT_FUNC  "SystemCtrlForUser",0x745286D1,sctrlHENSetMemory
#endif
#ifdef F_SystemCtrlForUser_0025
	IMPORT_FUNC  "SystemCtrlForUser",0x5A35C948,sctrlSESetUmdFile
#endif
#ifdef F_SystemCtrlForUser_0026
	IMPORT_FUNC  "SystemCtrlForUser",0xBC939DC1,sctrlSESetBootConfFileIndex
#endif
#ifdef F_SystemCtrlForUser_0027
	IMPORT_FUNC  "SystemCtrlForUser",0x70B92B45,sctrlSEGetBootConfFileIndex
#endif
#ifdef F_SystemCtrlForUser_0028
	IMPORT_FUNC  "SystemCtrlForUser",0x5CB025F0,sctrlSESetBootConfFileIndex
#endif
#ifdef F_SystemCtrlForUser_0029
	IMPORT_FUNC  "SystemCtrlForUser",0xAD4D5EA5,sctrlSESetConfigEx
#endif
#ifdef F_SystemCtrlForUser_0030
	IMPORT_FUNC  "SystemCtrlForUser",0x8E426F09,sctrlSEGetConfigEx
#endif
#ifdef F_SystemCtrlForUser_0031
	IMPORT_FUNC  "SystemCtrlForUser",0x1DDDAD0C,sctrlSESetConfig
#endif
#ifdef F_SystemCtrlForUser_0032
	IMPORT_FUNC  "SystemCtrlForUser",0x16C3B7EE,sctrlSEGetConfig
#endif
#ifdef F_SystemCtrlForUser_0033
	IMPORT_FUNC  "SystemCtrlForUser",0x8D5BE1F0,sctrlKernelSetInitApitype
#endif
#ifdef F_SystemCtrlForUser_0034
	IMPORT_FUNC  "SystemCtrlForUser",0x2BA770CD,sctrlKernelSetUMDEmuFile
#endif
#ifdef F_SystemCtrlForUser_0035
	IMPORT_FUNC  "SystemCtrlForUser",0x128112C3,sctrlKernelSetInitFileName
#endif
#ifdef F_SystemCtrlForUser_0036
	IMPORT_FUNC  "SystemCtrlForUser",0x9947E189,FindProc
#endif
#ifdef F_SystemCtrlForUser_0037
	IMPORT_FUNC  "SystemCtrlForUser",0x98012538,SetSpeed
#endif
#ifdef F_SystemCtrlForUser_0038
	IMPORT_FUNC  "SystemCtrlForUser",0xCC9ADCF8,sctrlHENSetSpeed
#endif
#ifdef F_SystemCtrlForUser_0039
	IMPORT_FUNC  "SystemCtrlForUser",0x56CEAF00,sctrlKernelQuerySystemCall
#endif
#ifdef F_SystemCtrlForUser_0040
	IMPORT_FUNC  "SystemCtrlForUser",0x053FAC1D,sctrlKernelBootFrom
#endif
#ifdef F_SystemCtrlForUser_0041
	IMPORT_FUNC  "SystemCtrlForUser",0x62CAC4CF,sctrlPatchModule
#endif
#ifdef F_SystemCtrlForUser_0042
	IMPORT_FUNC  "SystemCtrlForUser",0x2A868045,sctrlModuleTextAddr
#endif
#ifdef F_SystemCtrlForUser_0043
	IMPORT_FUNC  "SystemCtrlForUser",0x31C6160D,sctrlSESetDiscType
#endif
#ifdef F_SystemCtrlForUser_0044
	IMPORT_FUNC  "SystemCtrlForUser",0xABEF849B,sctrlSEGetDiscType
#endif
#ifdef F_SystemCtrlForUser_0045
	IMPORT_FUNC  "SystemCtrlForUser",0x603EE1D0,sctrlKernelSetNidResolver
#endif
#ifdef F_SystemCtrlForUser_0046
	IMPORT_FUNC  "SystemCtrlForUser",0xB364FBB4,sctrlKernelRand
#endif
