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

#include	"main.h"
#include	"mainstruct.h"
#include	"mainext.h"
#include	"mainfun.h"

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
