/*
* bilancia.h :  include per gestione bilancia
* 
* Progetto Easy Picking 3.0 - Wella
*
* Autore : Roberto Mantovani - Stefano Tarroni
*
* Copyright CNI srl 1995-2000
*/

#ifndef BOOL_TYPE_DEFINED
typedef unsigned int BOOL;
#define FALSE	0
#define TRUE 	1
#define BOOL_TYPE_DEFINED
#endif

#ifndef MIN_MAX_DEFINED
#define max(a,b)	((a)>=(b)?(a):(b))
#define min(a,b)	((a)<=(b)?(a):(b))
#endif

#ifndef BYTE_TYPE_DEFINED
typedef unsigned char BYTE;
#define BYTE_TYPE_DEFINED
#endif

typedef struct tagCfgStruct{
	char szCniCfg[128];
	int nDelay;
} CFGSTRUCT, *PCFGSTRUCT;

/*
* Inizio dichiarazione dei prototipi
*/
void SafeIntFunc();
void ReadConfiguration(BOOL bReadprocInfo);
void ReadProcInfo(char *szFileConfig);
BOOL SendMessage(int nDest,int nSrce,int nMsgCode,char *szText);
BOOL GestSocket(int *pnFd, void *ClientData, void *CallData);
int GetCmdIndex(char *szCmd);
