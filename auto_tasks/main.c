/*
* printlabel.c
* ------------
* Stampa Etichette
* 
* Progetto Easy Picking 3.0
* Data creazione 02/02/1999
*
* Autore : Roberto Mantovani
*
* Storia delle modifiche
* S. DATA         DESCRIZIONE
* -- ----------   ----------------------------------------
* rm 07-02-1999 : Inizio versione Europharco
* rm 10-02-1999 : Corretti alcuni errori di ricerca archivi ...
* rm 11-02-1999 : Aggiunte alcune gestioni (corriere, flag ritiro presso deposito ...)
* rm 23-02-1999 : Stampa Etichette Depositi
* rm 06-04-1999 : Apertura / Chiusura archivi ad ogni operazione
* rm 24-01-2000 : Inizio versione S.L.I.
* rm 09-11-2000 : Inizio versione WELLA - POSTGRESQL 
* -- ----------   ----------------------------------------
*
* Copyright CNI srl 1999-2000
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
#include <pmx_msq.h>		/* funzioni sulle message queue */
#include <msq_lib.h>		/* funzioni sulle message queue */
#include <shared.h>			/* funzioni sulle shared memory */
#ifdef TRACE
	#include <trace.h>
#endif
#include <ca_file.h>		/* libreria di funzioni per la lettura dei dati di configurazione */
#include <dbfun.h>			/* libreria di gestione database */
#include <proc_list.h>	/* strutture per la gestione dei processi costanti di identificazione dei messaggi */

#include	"main.h"
#include	"mainstruct.h"
#include	"mainext.h"
#include	"mainfun.h"

extern char *optarg;
extern int optind;

int GetNewTasksFromDB(PATASK pTask)
{
	int nId;
	int nStampe;
	char szParametri[256];
	PGresult *PGRes;
	PGresult *PGResUpdate;
	int nCod = -1;

	PGRes=PGExecSQL(Cfg.nDebugVersion>5,"select id_task,codice_tipo_task,stringa_parametri from tasks where stato=' ' order by id_task limit 1;");
	nStampe=0;
	if (PQresultStatus(PGRes) != PGRES_TUPLES_OK || PQntuples(PGRes)!=1){
	} else {
		nId=atoi(PQgetvalue(PGRes,0,0));
		nCod=atoi(PQgetvalue(PGRes,0,1));
		strcpy(szParametri,PQgetvalue(PGRes,0,2));
		pTask->nId=nId;
		pTask->nCodice=nCod;
		strcpy(pTask->szParametri,szParametri);

		PGResUpdate=PGExecSQL(Cfg.nDebugVersion>1,"update tasks set stato='S' where id_task=%d;",nId);
		PQclear(PGResUpdate);
	}
	PQclear(PGRes);
	return nCod;
}

/*
* main()
*/
int main(int argc,char** argv)
{
	int nErr=0;
	int nOpt;
	int nAnno;
	PGresult *PGRes;
	char szDateBuffer[128];
	char szOrdProg[80];
	char szLabelPrinterName[80];
	BOOL bAlone=FALSE;
	int nCodTask;
	ATASK Task;


	pszNomeModulo = argv[0];

	/*
	* Lettura della configurazione corrente
	*/
	ReadConfiguration(TRUE);

	/*
	* Connessione a DB Postgres
	*/
	ConnectDataBase(&DBConn);

#ifdef TRACE
	/* Apro il file trace */
	sprintf(szBufTrace, "%s%s", pszNomeModulo, TRACE_FILE_SUFFIX);

	open_trace(pszNomeModulo, szBufTrace, TRACE_FILE_MAXSIZE);

	trace_out_vstr(1,"Module Name     : %s",argv[0]);
	trace_out_vstr_date(1,"Started");
	trace_out_vstr(1,"Stand Alone Mode: %s",bAlone?"ON":"OFF");
#endif
	
	/* Predispongo l'utilizzo dell'uscita di sicurezza */
	signal(SIGTERM, SafeIntFunc);
	signal(SIGINT,SafeIntFunc);
	signal(SIGQUIT,SafeIntFunc);
	signal(SIGILL,SafeIntFunc);
	signal(SIGKILL,SafeIntFunc);

	if((ProcList[PROC_PRINTLABEL].nQNumber = OpenMsgQ(ProcList[PROC_PRINTLABEL].nQKey))<0){
#ifdef TRACE
		trace_out_vstr(1,"Apertura coda msg %s (%d) fallita",
			ProcList[PROC_PRINTLABEL].szProcName,
			ProcList[PROC_PRINTLABEL].nQKey);
	}
#endif
#ifdef TRACE
	trace_out_vstr(1,"coda msg %s (%d) -> %d",
		ProcList[PROC_PRINTLABEL].szProcName,
		ProcList[PROC_PRINTLABEL].nQKey,
		ProcList[PROC_PRINTLABEL].nQNumber);
#endif


	/*
	* Ciclo infinito di controllo e ricezione dati
	*/
	for(;;){

		// Ricavo l'innesco di tasks dal db
		nCodTask=GetNewTasksFromDB(&Task);

		/********************************************************************************
		* Task di invio file dettaglio rese lavorate per cliente
		*********************************************************************************/
		switch(nCodTask){
			case 1:
			{
#ifdef TRACE
				trace_out_vstr(1, "codice : %d - parametri : [%s]", nCodTask, Task.szParametri);
#endif
				SendMessage(PROC_PRINTLABEL,PROC_MAIN,PRINT_PACKING_LIST_REPORT,Task.szParametri);
			}
			break;
		}

		sleep(1);
	}

	SafeIntFunc();
}	/* End of main() */


