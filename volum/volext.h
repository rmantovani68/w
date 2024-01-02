/*
* volext.h
* 
* Progetto Easy Picking 2.0 : Europharco
*
* Autore : Roberto Mantovani
*
* Copyright CNI srl 1995-98
*/
extern CFGSTRUCT	Cfg;

/*
* Connessione al DataBase PostgreSQL
*/
extern PGconn *DBConn;

extern int nAllMsg;
extern int nCarIn;
extern MSGHDRINT InMsgStruct;
extern char szInMsg[INFO_LEN];
extern char szText[INFO_LEN];

extern char szBufTrace[80];
extern char *pszNomeModulo;
extern char szTimeBuffer[80];
extern char szDateBuffer[80];

extern PLINEA_STRUCT pDatiLinea;     /* Dati di gestione linea */
extern UBICAZIONI Ubicazioni;        /* Ubicazioni */
extern IMBALLI Imballi;              /* Imballi */


extern PROCESSO ProcList[NUM_PROC];	/* processi componenti il progetto */

extern DBSTRUCT tOrdProd;     /* Ordini in Produzione */
extern DBSTRUCT tColProd;     /* Colli in Produzione */
extern DBSTRUCT tRigProd;     /* Righe in Produzione */
extern DBSTRUCT tRigTmp;	    /* Temporaneo Righe di Produzione */
extern DBSTRUCT tUbicazioni;  /* Ubicazioni */
extern DBSTRUCT tImballi;     /* Imballi */
extern DBSTRUCT tArticoli;    /* Prodotti */
/*
* vettore degli archivi utilizzati
*/
extern DB_ARRAY_STRUCT	UsedDB[];
