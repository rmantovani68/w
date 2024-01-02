/*
* settvar.c : gestione settori di magazzino - Dichiarazione variabili globali
* 
* Progetto Easy Picking 2.0 : Europharco
*
* Autore : Roberto Mantovani
*
* Copyright CNI srl 1995-99
*/
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termio.h>
#include <sys/stat.h>

#include <libpq-fe.h>
#include <pmx_msq.h>
#include <msq_lib.h>
#include <shared.h>
#ifdef TRACE
	#include <trace.h>
#endif
#include <ca_file.h>
#include <dbfun.h>
#include <proc_list.h>

#include	"linklist.h"
#include	"settori.h"
#include	"settstruct.h"
#include	"settext.h"
#include	"settfun.h"

CFGSTRUCT	Cfg;

int nAllMsg;
int nCarIn;
MSGHDRINT InMsgStruct;
char szInMsg[INFO_LEN];
char szText[INFO_LEN];

char szBufTrace[80];
char *pszNomeModulo;
char szTimeBuffer[80];
char szDateBuffer[80];

PLINEA_STRUCT pDatiLinea;     /* Dati di gestione sistema */
LINKED_LIST ListaOrdini[MAX_SETTORI];    /* Lista FIFO Ordini */
PSETTORE pSettori;            /* shared memory pointer */
UBICAZIONI Ubicazioni;        /* Ubicazioni */
EVENTO Evento;                /* Evento */

/*
* Connessione al DataBase PostgreSQL
*/
PGconn *DBConn;

int nPrimoSettoreAbilitato;

PROCESSO ProcList[NUM_PROC];	/* processi componenti il progetto */

CEDOLASTRUCT Cedole[MAX_CEDOLE];

DBSTRUCT tOpe;         /* Operatori */
DBSTRUCT tOpeSet;      /* Operatori - Settori */
DBSTRUCT tRicOrd;	     /* Ordini in Ricezione */
DBSTRUCT tOrdProd;	   /* Ordini in Produzione */
DBSTRUCT tColProd;	   /* Colli in Produzione */
DBSTRUCT tRigProd;	   /* Righe in Produzione */
DBSTRUCT tUbi;	     /* Ubicazioni */

/*
* vettore degli archivi utilizzati
*/
DB_ARRAY_STRUCT	UsedDB[]= {
	{&tUbi,    "ubicazioni"}, 
	{&tRicOrd, "ric_ord"},  
	{&tOrdProd,"ord_prod"},
	{&tColProd,"col_prod"},
	{&tRigProd,"rig_prod"},
	{&tOpe,    "operatori"},
	{&tOpeSet, "opeset"},
	{NULL,NULL}
};
