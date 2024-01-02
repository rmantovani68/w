
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the MAGNETICENCODEDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// MAGNETICENCODEDLL_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef MAGNETICENCODEDLL_EXPORTS
#define MAGNETICENCODEDLL_API __declspec(dllexport)
#else
#define MAGNETICENCODEDLL_API __declspec(dllimport)
#endif

#include "YD627ECODE.h"
// This class is exported from the Magnetic EncodeDll.dll
//class MAGNETICENCODEDLL_API CMagneticEncodeDll {
//public:
//	CMagneticEncodeDll(void);
	// TODO: add your methods here.
//};

//extern MAGNETICENCODEDLL_API int nMagneticEncodeDll;

//MAGNETICENCODEDLL_API int fnMagneticEncodeDll(void);

MAGNETICENCODEDLL_API short  WINAPI gOpenSerialPort(short ID, DWORD dwBaudRate);
MAGNETICENCODEDLL_API short  WINAPI gCommunicationWithDevice(void);///
MAGNETICENCODEDLL_API short  WINAPI gCloseSerialCom(void);
MAGNETICENCODEDLL_API short  WINAPI gReset(void);
MAGNETICENCODEDLL_API short  WINAPI gSetHLCo(int Mode);
MAGNETICENCODEDLL_API short  WINAPI gSetTrack2Density(int Mode);
MAGNETICENCODEDLL_API short  WINAPI gMSRReadCard(int type, char *tr1, char *tr2, char *tr3);
MAGNETICENCODEDLL_API short  WINAPI gMSRWriteCard(int Type, char *Tk1, char *Tk2, char *Tk3);
MAGNETICENCODEDLL_API short  WINAPI gEraseInformationofCard(int tk);
MAGNETICENCODEDLL_API short  WINAPI gMSRWrtOrReadStatus(void);