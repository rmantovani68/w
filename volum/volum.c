/*
* volum.c : Analisi Volumetrica
* 
* Progetto Easy Picking 3.0 : Wella Rebuid
*
* Autore : Roberto Mantovani - Stefano Tarroni
*
* Copyright CNI srl 1995-99
*
* 10-10-2000 rm+st : Inizio rebuild Wella 
*
* Sostituzione delle librerie CB con PostgreSQL
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

extern char *optarg;
extern int optind;

/*
* main()
*/
int main(int argc,char** argv)
{
	int nErr=0;
	int nOpt;


	Cfg.bAlone=FALSE;

	while ((nOpt = getopt(argc, argv, "a")) != -1) {
		switch(nOpt){
			case 'a':	/* alone */
				Cfg.bAlone=TRUE;
			break;
			default:
				nErr++;
			break;
		}
	}
 

	if(nErr){
		printf("%s : copyright 1995-2000 CNI srl\n",argv[0]);
		printf("Uso : %s [-a]\n",argv[0]);
		printf("\t-a    : Run Alone\n");

		exit(0);
	}


	pszNomeModulo = argv[0];

	/*
	* Lettura della configurazione corrente
	*/
	ReadConfiguration(TRUE);

	/*
	* Lettura configurazione tabelle utilizzate
	*/
	ReadTablesInfo();

	/*
	* Connetto a DB Postgres
	*/
	ConnectDataBase(&DBConn);


#ifdef TRACE
	/* Apro il file trace */
	sprintf(szBufTrace, "%s%s", pszNomeModulo, TRACE_FILE_SUFFIX);

	open_trace(pszNomeModulo, szBufTrace, TRACE_FILE_MAXSIZE);

	trace_out_vstr(1,"Module Name     : %s",argv[0]);
	trace_out_vstr(1,"Started         : %s %s",
			GetDate(time((long *)0), szDateBuffer),
			GetTime(time((long *)0), szTimeBuffer));
	trace_out_vstr(1,"Stand Alone Mode: %s",Cfg.bAlone?"ON":"OFF");
#endif
	

	/*
	* Apro la coda messaggi principale 
	*/
	if((ProcList[PROC_MAIN].nQNumber = OpenMsgQ(ProcList[PROC_MAIN].nQKey))<0){
#ifdef TRACE
		trace_out_vstr(1, "%s : Apertura coda messaggi principale fallita", pszNomeModulo);
#endif
	}
	if(Cfg.bAlone){
		/* Creo la coda messaggi locale */
		if((ProcList[PROC_VOLUM].nQNumber = CreateMsgQ(ProcList[PROC_VOLUM].nQKey,1))<0){
#ifdef TRACE
			trace_out_vstr(1, "%s : Creazione coda messaggi locale fallita", pszNomeModulo);
#endif
		}
	} else {
		/* Apro la coda messaggi locale */
		if((ProcList[PROC_VOLUM].nQNumber = OpenMsgQ(ProcList[PROC_VOLUM].nQKey))<0){
	#ifdef TRACE
			trace_out_vstr(1, "%s : Apertura coda messaggi locale fallita", pszNomeModulo);
	#endif
		}
	}

	/*
	* Leggo la shared memory per la gestione della linea
	*/
	if((pDatiLinea=(PLINEA_STRUCT)GetShm(Cfg.nShmKey,sizeof(LINEA_STRUCT)))==NULL){
#ifdef TRACE
		trace_out_vstr(1,"%s : Cannot Get Shared Memory [%d] !\n",
			pszNomeModulo, Cfg.nShmKey);
#endif
	}

	/* Predispongo l'utilizzo dell'uscita di sicurezza */
	signal(SIGTERM, SafeIntFunc);
	signal(SIGINT,SafeIntFunc);
	signal(SIGQUIT,SafeIntFunc);
	signal(SIGILL,SafeIntFunc);
	signal(SIGKILL,SafeIntFunc);

	/* 
	* modifico il livello di protezione di lettura / scrittura dei files
	*/
	umask(0);


	ReadImballi(&Imballi);
	SortImballi(&Imballi);

	/*
	* avverto MAIN dello start
	*/
	SendMessage(PROC_MAIN, PROC_VOLUM, PROGRAM_STARTED, NULL);


	
	if(Cfg.nNoUnpack==0){
#ifdef TRACE
	trace_out_vstr_date(1,"GESTIONE PRELIEVO RIGHE A PACCHETTO");
#endif
	}

	if(Cfg.bAlone){
		AnalisiVolumetrica();
	}

	/*
	* gestione program loop
	*/
	for(;;){
		/*
		* Gestione Messaggi Ricevuti
		*/
		nAllMsg = 0;
		if((nCarIn = ReceiveMsg(ProcList[PROC_VOLUM].nQNumber, &nAllMsg, szInMsg))>=0){
			UnpackStructMsg(szInMsg, &InMsgStruct, szText, nCarIn);

			switch (InMsgStruct.srce-1){
				case PROC_MAIN:
					switch(InMsgStruct.code){
						case REFRESH_DATA:

							ReadImballi(&Imballi);
							SortImballi(&Imballi);

						break;
						case START_ANALISI:
						{
							BOOL bRC;

							ReadImballi(&Imballi);
							SortImballi(&Imballi);

							SendMessage(PROC_MAIN, PROC_VOLUM, VOLUM_START_ANALISI, NULL);
							pDatiLinea->bAnalisi=TRUE;
							bRC=AnalisiVolumetrica();
							pDatiLinea->bAnalisi=FALSE;
							if(bRC){
								SendMessage(PROC_MAIN, PROC_VOLUM, VOLUM_END_ANALISI, NULL);
								/*
								* st 20-08-01 aggiorno le relazioni di produzione causa rallentamenti di DB
								* UpdateTables();
								*/
							}
						}
						break;
						case START_ANALISI_TEST:
						{
							BOOL bRC;

							ReadImballi(&Imballi);
							SortImballi(&Imballi);

							SendMessage(PROC_MAIN, PROC_VOLUM, VOLUM_START_ANALISI_TEST, NULL);
							pDatiLinea->bAnalisi=TRUE;
							bRC=AnalisiVolumetrica();
							pDatiLinea->bAnalisi=FALSE;
							if(bRC){
								SendMessage(PROC_MAIN, PROC_VOLUM, VOLUM_END_ANALISI_TEST, NULL);
							}
						}
						break;
					}
				break;
			}
		}
	}

	SafeIntFunc();
}	/* End of main() */

