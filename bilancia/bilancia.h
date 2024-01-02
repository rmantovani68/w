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

#define CH_DLE				0x10
#define CH_STX				0x02
#define CH_ETX				0x03
#define CH_ACK				0x06
#define CH_ENQ				0x05
#define CH_NAK				0x15
#define CH_RET				0x0d

/* Inizio dichiarazione delle costanti */

#define TIMEOUT							4
#define TIMEOUT_WITHIN			0
#define TIMEOUT_BEFORE			4
#define FETCH_DATA_OFFSET		4

#ifdef DOC
struct msg_spec {
	char *msg_start;		/* message start string */
	int start_len;			/* start length to be checked exactly */
	int do_start_check;		/* check only for start char (but < start_len) */
	int start_char;			/* >0: start char to be checked for */
	int end_len;			/* >0: end length to be checked exactly */
	int do_end_check;		/* >0: check only for end char (but < end_len) */
	int end_char;			/* end char to be checked for (if any) */
	int do_checksum;		/* >0: add/read checksum after "end_char" */
	int do_checklen;		/* add/read check length just before "end_char" */
};
#endif

/* 
* Messaggi ricevibili dal processo di gestione bilancia e 
* relativo simulatore 
*/
MSG_SPEC msgRcv[] = {
	{"\x02", 1, 0, 0, 0, 1, 0x03, 0, 0},
	{"\x05", 1, 0, 0, 1, 0, 0, 0, 0},
};
enum MsgRcv {
	BILANCIA_RCV_DATA,
	BILANCIA_SIM_RCV_TX_REQ,

	NUM_MSG_RCV,
};

/* 
* Messaggi trasmettibili dalla bilancia e 
* relativo simulatore 
*/
MSG_SPEC msgSnd[] = {
	{"\x05", 1, 0, 0, 1, 0, 0, 0, 0},
	{"\x02", 1, 0, 0, 0, 1, 0x03, 0, 0},
};
enum 
{
	BILANCIA_SND_REQ,	
	BILANCIA_SIM_SND_DATA,

	NUM_MSG_SND,
};

typedef struct tagCfgStruct{
	char szCommDevice[80];
	int nBaudrate;
	int nParity;
	int nDataBits;
	int nStopBits;
	int nPortFD;
	int nDelay;
} CFGSTRUCT, *PCFGSTRUCT;

/*
* Inizio dichiarazione dei prototipi
*/
void SafeIntFunc(void);
void RxLoop(void);
void ReadConfiguration(void);
BOOL SendMessage(int nDest,int nSrce,int nMsgCode,char *szText);
