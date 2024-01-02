/*
* printvar.c : Stampa Picking List - Dichiarazione variabili globali
* 
* Progetto Mondadori Picking
*
* Autore : Daniele Ravaioli
*
* Copyright CNI srl 1998
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

#include	"printlabel.h"
#include	"printstruct.h"
#include	"printext.h"
#include	"printfun.h"

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
/*
* Connessione al DataBase PostgreSQL
*/
PGconn *DBConn;

PROCESSO ProcList[NUM_PROC];	/* processi componenti il progetto */

LABEL_FIELD_STRUCT LabelFields[256];
/*
* Definizioni per le tabelle utilizzate in IMPORT/EXPORT
*/
DBSTRUCT tRicOrd;     /* Ordini Ricevuti */
DBSTRUCT tRicArt;     /* Righe Ricevute */
DBSTRUCT tArticoli;   /* Prodotti */
DBSTRUCT tOrdProd;    /* Ordini di Produzione */
DBSTRUCT tColProd;    /* Colli di Produzione */
DBSTRUCT tRigProd;    /* Righe di Produzione */
DBSTRUCT tImballi;    /* Anagrafica Imballi */
DBSTRUCT tUbicazioni; /* Anagrafica Ubicazioni */

/*
* vettore degli archivi utilizzati
*/
DB_ARRAY_STRUCT	Tables[]= {
	{&tRicOrd,     "ric_ord"},
	{&tRicArt,     "ric_art"},
	{&tArticoli,   "articoli"},
	{&tOrdProd,    "ord_prod"},
	{&tColProd,    "col_prod"},
	{&tRigProd,    "rig_prod"},
	{&tImballi,    "imballi"},
	{&tUbicazioni, "ubicazioni"},

	{NULL,NULL}
};
