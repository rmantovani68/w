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

#include	"printlabel.h"
#include	"printstruct.h"
#include	"printext.h"
#include	"printfun.h"

extern char *optarg;
extern int optind;

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


	szOrdProg[0]='\0';
	szLabelPrinterName[0]='\0';
	while ((nOpt = getopt(argc, argv, "ao:p:h")) != -1) {
		switch(nOpt){
			case 'a':	/* alone */
				bAlone=TRUE;
			break;
			case 'o':	/* ordine */
				strcpy(szOrdProg,optarg);
			break;
			case 'p':	/* printer */
				strcpy(szLabelPrinterName,optarg);
			break;
			case 'h':   /* help */
				printf("%s : copyright 1999-2000 CNI srl\n",argv[0]);
				printf("Uso: %s [-a -o <Progressivo ordine> -p <printer name>] \n",argv[0]);
				printf("\t-h: help\n");
				exit(-1);
			break;
			default:
				nErr++;
			break;
		}
	}
 

	if(nErr){
		printf("%s : copyright 1999-2000 CNI srl\n",argv[0]);
		printf("Uso: %s [-a -o <Progressivo ordine> -p <printer name>] \n",argv[0]);
		printf("\t-h: help\n");
		exit(0);
	}

	pszNomeModulo = argv[0];

	/*
	* Lettura della configurazione corrente
	*/
	ReadConfiguration(TRUE);

	/*
	* Connessione a DB Postgres
	*/
	ConnectDataBase(&DBConn);

	/*
	* Lettura configurazione tabelle utilizzate
	*/
	ReadTablesInfo();


#ifdef TRACE
	/* Apro il file trace */
	sprintf(szBufTrace, "%s%s", pszNomeModulo, TRACE_FILE_SUFFIX);

	open_trace(pszNomeModulo, szBufTrace, TRACE_FILE_MAXSIZE);

	trace_out_vstr(1,"Module Name     : %s",argv[0]);
	trace_out_vstr_date(1,"Started");
	trace_out_vstr(1,"Stand Alone Mode: %s",bAlone?"ON":"OFF");
#endif
	
	if(!bAlone){
		/* Apro la coda messaggi locale */
		if((ProcList[PROC_PRINTLABEL].nQNumber = OpenMsgQ(ProcList[PROC_PRINTLABEL].nQKey))<0){
#ifdef TRACE
			trace_out_vstr(1, "Apertura coda messaggi locale fallita [%s]",ProcList[PROC_PRINTLABEL].szProcName);
#endif
		}
	} else {
		if(szOrdProg[0] && szLabelPrinterName[0]){
			char szSequence[128];

			strcpy(szSequence,"progcollo");
			StampaOrdine(szOrdProg,szLabelPrinterName,FALSE,0,szSequence);
			SafeIntFunc();
		}
	}

	/* Predispongo l'utilizzo dell'uscita di sicurezza */
	signal(SIGTERM, SafeIntFunc);
	signal(SIGINT,SafeIntFunc);
	signal(SIGQUIT,SafeIntFunc);
	signal(SIGILL,SafeIntFunc);
	signal(SIGKILL,SafeIntFunc);


	/*
	* avverto MAIN dello start
	*/
	SendBroadcastMessage(PROC_PRINTLABEL, PROGRAM_STARTED, NULL);

	/*
	* gestione program loop
	*/
	for(;;){
		/*
		* Gestione Messaggi Ricevuti
		*/
		nAllMsg = 0;
		if((nCarIn = ReceiveMsg(ProcList[PROC_PRINTLABEL].nQNumber, &nAllMsg, szInMsg))>=0){
			UnpackStructMsg(szInMsg, &InMsgStruct, szText, nCarIn);

			switch(InMsgStruct.code){
				case REFRESH_CONFIGURATION:
					/*
					* Lettura della configurazione corrente
					*/
					ReadConfiguration(FALSE);
				break;
				/*
				* Messaggi di stampa
				*/
				case PRINT_PACKING_LIST_COLLO:
				{
					char  *pPtr;
					char  *szSeparator=",\t\n";
					char szOrdKey[128];
					char szPrinter[128];
					int nCollo;

					/* OrdKey  */ if((pPtr=strtok(szText,szSeparator))!=NULL){ strcpy(szOrdKey,pPtr); }
					/* Collo   */ if((pPtr=strtok(NULL  ,szSeparator))!=NULL){ nCollo=atoi(pPtr); }
					/* Printer */ if((pPtr=strtok(NULL  ,szSeparator))!=NULL){ strcpy(szPrinter,pPtr); }

#ifdef TRACE
					trace_out_vstr_date(1, "PRINT_PACKING_LIST_COLLO [%s] [%d] on [%s]", szOrdKey,nCollo,szPrinter);
#endif
					/*
					* Stampa packing list di un collo all'interno di un ordine
					*/
					StampaPackingListCollo(szOrdKey,nCollo,szPrinter);
				}
				break;
				case PRINT_PACKING_LIST_REPORT:
				{
					char  *pPtr;
					char  *szSeparator=",\t\n";
					char szOrdKey[128];
					char szPrinter[128];
					int tipo;

					/* OrdKey  */ if((pPtr=strtok(szText,szSeparator))!=NULL){ strcpy(szOrdKey,pPtr); }
					/* Printer */ if((pPtr=strtok(NULL  ,szSeparator))!=NULL){ strcpy(szPrinter,pPtr); }
					/* tipo */    if((pPtr=strtok(NULL  ,szSeparator))!=NULL){ tipo=atoi(pPtr); }

#ifdef TRACE
					trace_out_vstr_date(1, "PRINT_PACKING_LIST_REPORT [%s:%d] on [%s]", szOrdKey, tipo, szPrinter);
#endif
					/*
					* Stampa packing list di un ordine
					*/
					StampaPackingListReport(szOrdKey,szPrinter,tipo);
				}
				break;

				case PRINT_PACKING_LIST:
				{
					char  *pPtr;
					char  *szSeparator=",\t\n";
					char szOrdKey[128];
					char szPrinter[128];

					/* OrdKey  */ if((pPtr=strtok(szText,szSeparator))!=NULL){ strcpy(szOrdKey,pPtr); }
					/* Printer */ if((pPtr=strtok(NULL  ,szSeparator))!=NULL){ strcpy(szPrinter,pPtr); }

#ifdef TRACE
					trace_out_vstr_date(1, "PRINT_PACKING_LIST [%s] on [%s]", szOrdKey,szPrinter);
#endif
					/*
					* Stampa packing list di un ordine
					*/
					StampaPackingList(szOrdKey,szPrinter);
				}
				break;
				case PRINT_PACKING_LIST_NO_PDF:
				{
					char  *pPtr;
					char  *szSeparator=",\t\n";
					char szOrdKey[128];
					char szPrinter[128];

					/* OrdKey  */ if((pPtr=strtok(szText,szSeparator))!=NULL){ strcpy(szOrdKey,pPtr); }
					/* Printer */ if((pPtr=strtok(NULL  ,szSeparator))!=NULL){ strcpy(szPrinter,pPtr); }

#ifdef TRACE
					trace_out_vstr_date(1, "PRINT_PACKING_LIST_NO_PDF [%s] on [%s]", szOrdKey,szPrinter);
#endif
					/*
					* Stampa packing list di un ordine
					*/
					StampaPackingListSenzaColliPdf(szOrdKey,szPrinter);
				}
				break;

				case PRINT_PACKING_LIST_PDF:
				{
					char  *pPtr;
					char  *szSeparator=",\t\n";
					char szOrdKey[128];
					char szPrinter[128];

					/* OrdKey  */ if((pPtr=strtok(szText,szSeparator))!=NULL){ strcpy(szOrdKey,pPtr); }
					/* Printer */ if((pPtr=strtok(NULL  ,szSeparator))!=NULL){ strcpy(szPrinter,pPtr); }

#ifdef TRACE
					trace_out_vstr_date(1, "PRINT_PACKING_LIST_PDF [%s] on [%s]", szOrdKey,szPrinter);
#endif
					/*
					* Stampa packing list di un ordine
					*/
					StampaPackingListSoloColliPdf(szOrdKey,szPrinter);
				}
				break;
				case PRINT_INIZIO_ELABORAZIONE:
				{
					char  *pPtr;
					char  *szSeparator=",\t\n";
					char szOrdKey[128];
					char szPrinter[128];

					/* OrdKey       */ if((pPtr=strtok(szText,szSeparator))!=NULL){ strcpy(szOrdKey,pPtr); }
					/* Printer      */ if((pPtr=strtok(NULL  ,szSeparator))!=NULL){ strcpy(szPrinter,pPtr); }

#ifdef TRACE
					trace_out_vstr_date(1, "PRINT_INIZIO_ELABORAZIONE Ordine [%s] on Printer [%s]", szOrdKey,szPrinter);
#endif
					StampaInizioElaborazione(szOrdKey,szPrinter);
				}
				break;
				case PRINT_FINE_ELABORAZIONE:
				{
					char  *pPtr;
					char  *szSeparator=",\t\n";
					char szOrdKey[128];
					char szPrinter[128];

					/* OrdKey       */ if((pPtr=strtok(szText,szSeparator))!=NULL){ strcpy(szOrdKey,pPtr); }
					/* Printer      */ if((pPtr=strtok(NULL  ,szSeparator))!=NULL){ strcpy(szPrinter,pPtr); }

#ifdef TRACE
					trace_out_vstr_date(1, "PRINT_FINE_ELABORAZIONE Ordine [%s] on Printer [%s]", szOrdKey,szPrinter);
#endif
					StampaFineElaborazione(szOrdKey,szPrinter);
				}
				break;
				case PRINT_RAC:
				{
					char  *pPtr;
					char  *szSeparator=",\t\n";
					char szOrdKey[128];
					char szPrinter[128];
					char szSequence[128];

					/* OrdKey       */ if((pPtr=strtok(szText,szSeparator))!=NULL){ strcpy(szOrdKey,pPtr); }
					/* Printer      */ if((pPtr=strtok(NULL  ,szSeparator))!=NULL){ strcpy(szPrinter,pPtr); }

#ifdef TRACE
					trace_out_vstr_date(1, "PRINT_RAC Ordine [%s] on Printer [%s]", szOrdKey,szPrinter);
#endif
					/* seleziono la sequenza di progressivo collo da incrementare */
					strcpy(szSequence,"progcollo");
					StampaOrdine(szOrdKey,szPrinter,TRUE,InMsgStruct.srce-1,szSequence);
				}
				break;
				case PRINT_RAC_AMAZON_BERTELLO:
				{
					char  *pPtr;
					char  *szSeparator=",\t\n";
					char szOrdProg[128];
					char szPrinter[128];
					char szSequence[128];

					/* OrdProg      */ if((pPtr=strtok(szText,szSeparator))!=NULL){ strcpy(szOrdProg,pPtr); }
					/* Printer      */ if((pPtr=strtok(NULL  ,szSeparator))!=NULL){ strcpy(szPrinter,pPtr); }

#ifdef TRACE
					trace_out_vstr_date(1, "PRINT_RAC Amazon Bertello Ordine [%s] on Printer [%s]", szOrdProg,szPrinter);
#endif
					StampaOrdineBertello(szOrdProg,szPrinter,TRUE,InMsgStruct.srce-1);
				}
				break;
				case PRINT_RAC_NO_PDF:
				{
					char  *pPtr;
					char  *szSeparator=",\t\n";
					char szOrdKey[128];
					char szPrinter[128];
					char szSequence[128];

					/* OrdKey       */ if((pPtr=strtok(szText,szSeparator))!=NULL){ strcpy(szOrdKey,pPtr); }
					/* Printer      */ if((pPtr=strtok(NULL  ,szSeparator))!=NULL){ strcpy(szPrinter,pPtr); }

#ifdef TRACE
					trace_out_vstr_date(1, "PRINT_RAC_NO_PDF Ordine [%s] on Printer [%s]", szOrdKey,szPrinter);
#endif
					/* seleziono la sequenza di progressivo collo da incrementare */
					strcpy(szSequence,"progcollo");
					StampaOrdineSenzaColliPdf(szOrdKey,szPrinter,TRUE,InMsgStruct.srce-1,szSequence);
				}
				break;
				case PRINT_RAC_PDF:
				{
					char  *pPtr;
					char  *szSeparator=",\t\n";
					char szOrdKey[128];
					char szPrinter[128];
					char szSequence[128];

					/* OrdKey       */ if((pPtr=strtok(szText,szSeparator))!=NULL){ strcpy(szOrdKey,pPtr); }
					/* Printer      */ if((pPtr=strtok(NULL  ,szSeparator))!=NULL){ strcpy(szPrinter,pPtr); }

#ifdef TRACE
					trace_out_vstr_date(1, "PRINT_RAC_PDF Ordine [%s] on Printer [%s]", szOrdKey,szPrinter);
#endif
					/* seleziono la sequenza di progressivo collo da incrementare */
					strcpy(szSequence,"progcollo");
					StampaOrdineSoloColliPdf(szOrdKey,szPrinter,TRUE,InMsgStruct.srce-1,szSequence);
				}
				break;
				case PRINT_ALL_PACKING_LIST:
				{
					/*
					* Stampa le packing list degli ordini elaborati
					*/
#ifdef TRACE
					trace_out_vstr_date(1, "PRINT_ALL_PACKING_LIST");
#endif
				}
				break;
				case PRINT_PACKING_LIST_STORICO:
				{
					char szOrdProg[128];
					char *szSeparator=",\t\n";
					char szPrinterName[128];
					char *pPtr;
					/*
					* stampa la packing-list dell'ordine storico selezionato
					*/
					/* OrdProg      */ if((pPtr=strtok(szText,szSeparator))!=NULL){ strcpy(szOrdProg,pPtr); }
					/* Printer      */ if((pPtr=strtok(NULL  ,szSeparator))!=NULL){ strcpy(szPrinterName,pPtr); }
#ifdef TRACE
					trace_out_vstr_date(1, "PRINT_PACKING_LIST_STORICO [ordine %s] [stampante%s]",szOrdProg,szPrinterName);
#endif
					StampaOrdineStorico(szOrdProg,szPrinterName);
				}
				break;
				case PRINT_ALL_RAC:
				{
					char szSequence[128];
					/*
					* Stampa etichette degli ordini elaborati
					*/
#ifdef TRACE
					trace_out_vstr_date(1, "PRINT_ALL_RAC on [%s]",szText);
#endif
					/* seleziono la sequenza di progressivo collo da incrementare */
					strcpy(szSequence,"progcollo");
					StampaOrdini(szText,szSequence);
					SendMessage(InMsgStruct.srce-1, PROC_PRINTLABEL, PRINTLABEL_ALL_RAC_STAMPATO, NULL);
				}
				break;
				case PRINT_RAC_COLLO:
				{
					char  *pPtr;
					char  *szSeparator=",\t\n";
					char szOrdKey[128];
					char szPrinter[128];
					char szSequence[128];
					int nCollo;

					/* OrdKey  */  if((pPtr=strtok(szText,szSeparator))!=NULL){ strcpy(szOrdKey,pPtr); }
					/* Collo   */  if((pPtr=strtok(NULL  ,szSeparator))!=NULL){ nCollo=atoi(pPtr); }
					/* Printer */  if((pPtr=strtok(NULL  ,szSeparator))!=NULL){ strcpy(szPrinter,pPtr); }

#ifdef TRACE
					trace_out_vstr_date(1, "PRINT_RAC_COLLO [%s,%d] on [%s]", szOrdKey,nCollo,szPrinter);
#endif
					/*
					* Stampa etichette di un collo all'interno di un ordine
					* gli passo il nome della sequenza da incrementare (progcollo)
					*/
					strcpy(szSequence,"progcollo");
					StampaCollo(szOrdKey,nCollo,szPrinter,szSequence);
				}
				break;
				case PRINT_RAC_COLLO_ADD:
				{
					char  *pPtr;
					char  *szSeparator=",\t\n";
					char szOrdKey[128];
					char szPrinter[128];
					char szSequence[128];
					int nCollo;

					/* OrdKey  */ if((pPtr=strtok(szText,szSeparator))!=NULL){ strcpy(szOrdKey,pPtr); }
					/* Collo   */ if((pPtr=strtok(NULL  ,szSeparator))!=NULL){ nCollo=atoi(pPtr); }
					/* Printer */ if((pPtr=strtok(NULL  ,szSeparator))!=NULL){ strcpy(szPrinter,pPtr); }

#ifdef TRACE
					trace_out_vstr_date(1, "PRINT_RAC_COLLO_ADD [%s,%d] on [%s]", szOrdKey,nCollo,szPrinter);
#endif
					/*
					* Stampa etichette di un collo all'interno di un ordine
					* gli passo il nome della sequenza da incrementare (progcollo_add)
					*/
					strcpy(szSequence,"progcollo_add");
					StampaCollo(szOrdKey,nCollo,szPrinter,szSequence);
				}
				break;
				case PRINT_PDF:
				{
					char  *pPtr;
					char  *szSeparator=",\t\n";
					char szOrdine[128];
					char szPrinter[128];

					/* Ordine       */ if((pPtr=strtok(szText,szSeparator))!=NULL){ strcpy(szOrdine,pPtr); }
					/* Printer */      if((pPtr=strtok(NULL  ,szSeparator))!=NULL){ strcpy(szPrinter,pPtr); }

					if(Cfg.nAdvancedInvoicing){
#ifdef TRACE
						trace_out_vstr_date(1, "PRINT_PDF Ordine [%s] ADVANCED INVOICING on Printer [%s]", szOrdine,szPrinter);
#endif
						StampaPdfOrdineAdvanced(szOrdine,szPrinter);
					} else {
#ifdef TRACE
						trace_out_vstr_date(1, "PRINT_PDF Ordine [%s] on Printer [%s]", szOrdine,szPrinter);
#endif
						StampaPdfOrdine(szOrdine,szPrinter);
					}
				}
				break;
				case PRINT_RAC_CONTIENE_DOC:
				{
					char  *pPtr;
					char  *szSeparator=",\t\n";
					char szPrinter[128];
					char szOrdine[128];

					/* Ordine       */ if((pPtr=strtok(szText,szSeparator))!=NULL){ strcpy(szOrdine,pPtr); }
					/* Printer */      if((pPtr=strtok(NULL  ,szSeparator))!=NULL){ strcpy(szPrinter,pPtr); }
					///* Printer */ if((pPtr=strtok(szText  ,szSeparator))!=NULL){ strcpy(szPrinter,pPtr); }

#ifdef TRACE
					trace_out_vstr_date(1, "PRINT_RAC_CONTIENE_DOC on printer [%s]", szPrinter);
#endif
					StampaContieneDocumento(szOrdine,szPrinter);
				}
				break;
			}
		}
	}

	SafeIntFunc();
}	/* End of main() */

