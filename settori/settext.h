/*
* settext.h
* 
* Progetto Easy Picking 2.0 : Europharco
*
* Autore : Roberto Mantovani
*
* Copyright CNI srl 1995-99
*/
#pragma once

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

extern PLINEA_STRUCT pDatiLinea;     /* Dati di gestione sistema */
extern LINKED_LIST ListaOrdini[MAX_SETTORI];    /* Lista FIFO Ordini */
extern PSETTORE pSettori;            /* shared memory pointer */
extern UBICAZIONI Ubicazioni;        /* Ubicazioni */
extern EVENTO Evento;                /* Evento */

/*
* Connessione al DataBase PostgreSQL
*/
extern PGconn *DBConn;

extern PROCESSO ProcList[NUM_PROC];	/* processi componenti il progetto */

extern CEDOLASTRUCT Cedole[MAX_CEDOLE];

extern int nPrimoSettoreAbilitato;

extern DBSTRUCT tOpe;         /* Operatori */
extern DBSTRUCT tOpeSet;      /* Operatori - Settori */
extern DBSTRUCT tRicOrd;      /* Ordini in Ricezione */
extern DBSTRUCT tOrdProd;	    /* Ordini in Produzione */
extern DBSTRUCT tColProd;	    /* Colli in Produzione */
extern DBSTRUCT tRigProd;	    /* Righe in Produzione */
extern DBSTRUCT tUbi;	        /* Ubicazioni */
/*
* vettore degli archivi utilizzati
*/
extern DB_ARRAY_STRUCT	UsedDB[];
