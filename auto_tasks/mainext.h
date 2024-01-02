/*
* mainext.h
* 
* Progetto Mondadori Picking
*
* Autore : Roberto Mantovani
*
* Copyright CNI srl 1998
*/

extern CFGSTRUCT	Cfg;

extern int nAllMsg;
extern int nCarIn;
extern MSGHDRINT InMsgStruct;
extern char szInMsg[INFO_LEN];
extern char szText[INFO_LEN];

extern char szBufTrace[80];
extern char *pszNomeModulo;
extern char szTimeBuffer[80];
extern char szDateBuffer[80];
/*
* Connessione al DataBase PostgreSQL
*/
extern PGconn *DBConn;

extern PROCESSO ProcList[NUM_PROC];	/* processi componenti il progetto */
