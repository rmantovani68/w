/*
* printext.h
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
extern LABEL_FIELD_STRUCT LabelFields[256];

/*
* Definizioni per le tabelle utilizzate in IMPORT/EXPORT
*/
extern DBSTRUCT tRicOrd;     /* Ordini Ricevuti */
extern DBSTRUCT tRicArt;     /* Righe Ricevute */
extern DBSTRUCT tArticoli;   /* Prodotti */
extern DBSTRUCT tOrdProd;    /* Ordini di Produzione */
extern DBSTRUCT tColProd;    /* Colli di Produzione */
extern DBSTRUCT tRigProd;    /* Righe di Produzione */
extern DBSTRUCT tImballi;    /* Anagrafica Imballi */
extern DBSTRUCT tUbicazioni; /* Anagrafica Ubicazioni */

/*
* vettore degli archivi utilizzati
*/
extern DB_ARRAY_STRUCT	Tables[];
