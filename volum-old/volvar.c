/*
* volvar.c : Analisi Volumetrica - Dichiarazione variabili globali
* 
* Progetto Easy Picking 3.0 : Wella
*
* Autore : Roberto Mantovani
*
* Copyright CNI srl 1995-2000
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

#include	"volum.h"
#include	"volstruct.h"
#include	"volext.h"
#include	"volfun.h"

CFGSTRUCT	Cfg;

/*
* Connessione al DataBase PostgreSQL
*/
PGconn *DBConn;

int nAllMsg;
int nCarIn;
MSGHDRINT InMsgStruct;
char szInMsg[INFO_LEN];
char szText[INFO_LEN];

char szBufTrace[80];
char *pszNomeModulo;
char szTimeBuffer[80];
char szDateBuffer[80];

PLINEA_STRUCT pDatiLinea;     /* Dati di gestione linea */
UBICAZIONI Ubicazioni;        /* Ubicazioni */
IMBALLI Imballi;              /* Imballi */


PROCESSO ProcList[NUM_PROC];	/* processi componenti il progetto */

DBSTRUCT tOrdProd;	  /* Ordini in Produzione */
DBSTRUCT tColProd;	  /* Colli in Produzione */
DBSTRUCT tRigProd;	  /* Righe in Produzione */
DBSTRUCT tRigTmp;	    /* Temporaneo Righe di Produzione */
DBSTRUCT tUbicazioni; /* Ubicazioni */
DBSTRUCT tImballi;    /* Imballi */
DBSTRUCT tArticoli;	  /* Prodotti */
/*
* vettore degli archivi utilizzati
*/
DB_ARRAY_STRUCT	UsedDB[]= {
	{&tImballi,    "imballi"},  
	{&tArticoli,   "articoli"}, 
	{&tOrdProd,    "ord_prod"},
	{&tColProd,    "col_prod"},
	{&tRigProd,    "rig_prod"},
	{&tRigTmp,     "rig_prod"},
	{&tUbicazioni, "ubicazioni"}, 
	{NULL,NULL}
};
