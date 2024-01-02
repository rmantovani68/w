/*
* barcode.h : gestione Barcode Reader
* 
* Progetto Mondadori Picking
*
* Autore : Roberto Mantovani
*
* Copyright CNI srl 1995-98
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

/* Inizio dichiarazione delle costanti */

#define TIMEOUT							4
#define TIMEOUT_WITHIN			4
#define TIMEOUT_BEFORE			4
#define FETCH_DATA_OFFSET		4

#define CAN_CHAR            '\x18'

#ifdef DOCUMENTAZIONE
struct msg_spec {
	char *msg_start;      /* message start string */
	int start_len;        /* start length to be checked exactly */
	int do_start_check;   /* check only for start char (but < start_len) */
	int start_char;       /* >0: start char to be checked for */
	int end_len;          /* >0: end length to be checked exactly */
	int do_end_check;     /* >0: check only for end char (but < end_len) */
	int end_char;         /* end char to be checked for (if any) */
	int do_checksum;      /* >0: add/read checksum after "end_char" */
	int do_checklen;      /* add/read check length just before "end_char" */
};
typedef struct msg_spec MSG_SPEC;
#endif

/* Messaggi ricevibili dal lettore */
MSG_SPEC msgRcv[] = {
	{"\x02", 1, 0, 0, 100, 1, 0x0d0a, 0, 0},      /* STX dati CRLF */
	{"\x02\x18\x0d\x0a", 4, 0, 0, 0, 0, 0, 0, 0}, /* CAN	*/
};
enum MsgRcv {
	BCRD_RCV_BARCODE,
	BCRD_RCV_NOREAD,

	NUM_MSG_RCV,
};

typedef struct tagCfgStruct{
	char szCommDevice[80];
	int nBaudrate;
	int nParity;
	int nDataBits;
	int nStopBits;
	int nPortFD;
} CFGSTRUCT, *PCFGSTRUCT;

/*
* Inizio dichiarazione dei prototipi
*/
void ReadConfiguration(void);
void SafeIntFunc(void);
void RxLoop(void);
BOOL SendMessage(int nDest,int nSrce,int nMsgCode,char *szText);
