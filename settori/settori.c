/*
* settori.c : gestione settori di magazzino
* 
* Progetto Easy Picking 3.0 : Wella
*
* Autore : Roberto Mantovani
*
* Copyright A&L srl 2003-2010
*
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

extern char *optarg;
extern int optind;

typedef struct _test_ {
	BOOL bTest;
	BOOL bLoop;
	BOOL bLampada;
	int nSettore;
	int nIsola;
	int nModulo;
	int nRiga;
	int nColonna;
	char szSQLCmd[512];
} IOS_TEST;

/*
* main()
*/
int main(int argc,char** argv)
{
	int nIndex;
	int nErr=0;
	int nOpt;
	IOS_TEST Test;

	Test.bTest=FALSE;
	Test.bLoop=FALSE;
	Test.bLampada=FALSE;
	Test.nSettore=-1;
	Test.nIsola=-1;
	Test.nModulo=-1;
	Test.nRiga=-1;
	Test.nColonna=-1;

	while ((nOpt = getopt(argc, argv, "atli:s:x:q")) != -1) {
		switch(nOpt){
			case 'a':	/* alone */
				Cfg.bAlone=TRUE;
			break;
			case 'q':	/* quit */
				Cfg.bQuit=TRUE;
			break;
			case 't':	/* test */
				Test.bTest=TRUE;
			break;
			case 'l':	/* test loop */
				Test.bLoop=TRUE;
			break;
			case 'i':	/* isola */
				Test.nIsola=atoi(optarg);
			break;
			case 's':	/* settore */
				Test.nSettore=atoi(optarg);
			break;
			case 'x':	/* lampada modulo,riga,colonna*/
			{
				int nModulo,nRiga,nColonna;

				sscanf(optarg,"%d,%d,%d",&nModulo,&nRiga,&nColonna);
				Test.nModulo=nModulo;
				Test.nRiga=nRiga;
				Test.nColonna=nColonna;
				Test.bLampada=TRUE;
			}
			break;
			default:
				nErr++;
			break;
		}
	}
 
 	if(Test.bTest && Test.nIsola==-1){
		nErr++;
	}



	if(nErr){
		printf("%s : copyright 2003-2010 A&L srl\n",argv[0]);
		printf("Uso : %s [-a][-t][-l][-i isola][-s settore][-x modulo,riga,colonna]\n",argv[0]);
		printf("\t-a          : Run Alone\n");
		printf("\t-q          : Quite after test\n");
		printf("\t-t          : Run Test\n");
		printf("\t-l          : Loop Test\n");
		printf("\t-s settore  : settore\n");
		printf("\t-i isola    : isola\n");
		printf("\t-x lampada  : lampada\n");

		exit(0);
	}


	/* nome dell'eseguibile */
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
	* Connetto a DB Server	
	*/
	ConnectDataBase(&DBConn);


#ifdef TRACE
	/* Apro il file trace (trasmissione) */
	sprintf(szBufTrace, "%s%s", pszNomeModulo, TRACE_FILE_SUFFIX);

	open_trace(pszNomeModulo, szBufTrace, TRACE_FILE_MAXSIZE);

	trace_out_vstr(1,"Module Name     : %s",argv[0]);
	trace_out_vstr(1,"Stand Alone Mode: %s",Cfg.bAlone?"ON":"OFF");
	trace_out_vstr_date(1,"Started");
#endif
	
	/*
	* alloco la shared memory dei settori
	*/
	if((pSettori=(PSETTORE)SetupShm(Cfg.nSettoriShmKey,Cfg.nNumeroSettori*sizeof(SETTORE)))==NULL){
#ifdef TRACE
		trace_out_vstr_date(1,"Cannot Alloc Shared Memory [%d] (Settori)", Cfg.nSettoriShmKey);
#endif
	}
	memset(pSettori,0,Cfg.nNumeroSettori*sizeof(SETTORE));


	ReadStatoSettori();

	if(Cfg.bAlone){
		/* Creo la coda messaggi locale */
		if((ProcList[PROC_SETTORI].nQNumber = CreateMsgQ(ProcList[PROC_SETTORI].nQKey,1))<0){
#ifdef TRACE
			trace_out_vstr_date(1, "Creazione coda messaggi locale fallita");
#endif
		}
		/*
		* Leggo la shared memory per la gestione della linea
		*/
		if((pDatiLinea=(PLINEA_STRUCT)SetupShm(Cfg.nShmKey,sizeof(LINEA_STRUCT)))==NULL){
#ifdef TRACE
			trace_out_vstr_date(1,"Cannot Get Shared Memory [%d] !\n", Cfg.nShmKey);
#endif
		}
		RunSimpleProcess(PROC_IOS);       /* gestione IOS */

	} else {
		/* Apro la coda messaggi principale */
		if((ProcList[PROC_MAIN].nQNumber = OpenMsgQ(ProcList[PROC_MAIN].nQKey))<0){
#ifdef TRACE
			trace_out_vstr_date(1, "Apertura coda messaggi principale fallita");
#endif
		}
		/* Apro la coda messaggi locale */
		if((ProcList[PROC_SETTORI].nQNumber = OpenMsgQ(ProcList[PROC_SETTORI].nQKey))<0){
#ifdef TRACE
			trace_out_vstr_date(1, "Apertura coda messaggi locale fallita");
#endif
		}
		/* Apro la coda messaggi di IOS */
		if((ProcList[PROC_IOS].nQNumber = OpenMsgQ(ProcList[PROC_IOS].nQKey))<0){
#ifdef TRACE
			trace_out_vstr_date(1, "Apertura coda messaggi IOS fallita");
#endif
		}
	}

	/*
	* Leggo la shared memory per la gestione della linea
	*/
	if((pDatiLinea=(PLINEA_STRUCT)GetShm(Cfg.nShmKey,sizeof(LINEA_STRUCT)))==NULL){
#ifdef TRACE
		trace_out_vstr_date(1,"Cannot Get Shared Memory [%d] !", Cfg.nShmKey);
#endif
	}

	/* Predispongo l'utilizzo dell'uscita di sicurezza */
	signal(SIGTERM, SafeIntFunc);
	signal(SIGINT,SafeIntFunc);
	signal(SIGQUIT,SafeIntFunc);
	signal(SIGILL,SafeIntFunc);
	signal(SIGKILL,SafeIntFunc);

	signal(SIGUSR1,SaveStateIntFunc);

	/* 
	* modifico il livello di protezione di lettura / scrittura dei files
	*/
	umask(0);

	ReadUbicazioni(&Ubicazioni);
	SortUbicazioni(&Ubicazioni);

	/*
	* avverto MAIN dello start
	*/
	SendMessage(PROC_MAIN, PROC_SETTORI,  PROGRAM_STARTED, NULL);



	/*
	* Test settori
	* Vengono accese le luci in sequenza di prelievo e si attende pressione tasto da operatore
	*/
	if(Cfg.bAlone && Test.bTest){
		do {
			BOOL bTastoOK=FALSE;
			char szBuffer[256];
			PGresult *PGRes = NULL;
			int nDisplayIndex;
			int nIsola;
			int nSettore;
			int nIOS;
			int nCPU;
			int nModulo;
			int nRow;
			int nCol;
			PDISPLAYSTRUCT pDisplay;
			int nIndex;
			int nTuples;

			ResetLuci();

			ScongelaPKL();

			for(nIndex=0;nIndex<Cfg.nNumeroSettori;nIndex++){
				if(Test.nIsola!=-1 && pSettori[nIndex].nIsola==Test.nIsola){
					ResetSettore(pSettori[nIndex].nIsola,pSettori[nIndex].nSettore);
				}
				/*
				* rm 06-04-2000 : ritardo per evitare il riempimento della coda msg di LynxOS (MAX 35 !!)
				*/
				// usleep(1000*10*Cfg.nDelay);
			}
			/*
			* Selezione di tutte le ubicazioni dell'impianto 
			*/
			strcpy(Test.szSQLCmd,"select ubicazione, codprod, cancella, locazione, isola, settore, display, ios, 0, settore_can, riga_can, colonna_can, priorita, cnistato from ubicazioni where enabled = 1 ");
			if(Test.nIsola!=-1){
				sprintf(szBuffer,"and isola=%d ",Test.nIsola);
				strcat(Test.szSQLCmd,szBuffer);
			}
			if(Test.nSettore!=-1){
				sprintf(szBuffer,"and settore=%d ",Test.nSettore);
				strcat(Test.szSQLCmd,szBuffer);
			}
			if(Test.bLampada){
				sprintf(szBuffer,"and modulo_can=%d and riga_can=%d and colonna_can=%d ",Test.nModulo,Test.nRiga,Test.nColonna);
				strcat(Test.szSQLCmd,szBuffer);
			}
			strcat(Test.szSQLCmd,"order by isola,priorita;");

			PGRes=PGExecSQL(TRUE,Test.szSQLCmd);

			nTuples=PQntuples(PGRes);

			for(nIndex=0;nIndex<nTuples;nIndex++){
				nIsola=atoi(PQgetvalue(PGRes,nIndex,4));
				nSettore=atoi(PQgetvalue(PGRes,nIndex,5));
				nDisplayIndex=atoi(PQgetvalue(PGRes,nIndex,6));
				nIOS=atoi(PQgetvalue(PGRes,nIndex,7));
				nCPU=atoi(PQgetvalue(PGRes,nIndex,8));
				nModulo=atoi(PQgetvalue(PGRes,nIndex,9));
				nRow=atoi(PQgetvalue(PGRes,nIndex,10));
				nCol=atoi(PQgetvalue(PGRes,nIndex,11));
				pDisplay=&(Cfg.Displays[nDisplayIndex]);

				sprintf(pDisplay->szRiga_1_Display,"%04d",nRow);     /* Riga 1 del display */
				sprintf(pDisplay->szRiga_2_Display,"%04d",nCol);     /* Riga 2 del display */
				pDisplay->nStatoRiga1=NORMAL;
				pDisplay->nStatoRiga2=NORMAL;
				UpdateDisplay(pDisplay,TUTTO);

				sprintf(szBuffer,"%d,%d,%d,%d,%d", 0,0,nModulo,nRow,nCol);
				SendMessage(PROC_IOS, PROC_SETTORI,  IOS_PKL_SET_RC, szBuffer);

				bTastoOK=FALSE;

				while(!bTastoOK){
					nAllMsg = 0;
					if((nCarIn = ReceiveMsgNoWait(ProcList[PROC_SETTORI].nQNumber, &nAllMsg, szInMsg))>=0){
						UnpackStructMsg(szInMsg, &InMsgStruct, szText, nCarIn);

						switch (InMsgStruct.srce-1){
							case PROC_IOS:
								switch(InMsgStruct.code){
									case IOS_PKL_BUTTON_PRESSED:
									{
										PUBICAZIONE pUbicazione;
										int nMsgIOS,nMsgCPU,nMsgSettore,nMsgRowIndex,nMsgColIndex;

										sscanf(szText,"%d,%d,%d,%d,%d",&nMsgIOS,&nMsgCPU,&nMsgSettore,&nMsgRowIndex,&nMsgColIndex);

#ifdef TRACE
										trace_out_vstr_date(1, "TEST : Ricevuto IOS_PKL_BUTTON_PRESSED [IOS:%d CPU:%d s:%02d,r:%02d,c:%02d]", nMsgIOS,nMsgCPU,nMsgSettore,nMsgRowIndex,nMsgColIndex);
#endif

										if((pUbicazione=SearchUbicazione(&Ubicazioni,nMsgIOS,nMsgCPU,nMsgSettore,nMsgRowIndex,nMsgColIndex))!=NULL){
											if(nMsgSettore==nModulo && nMsgRowIndex==nRow && nMsgColIndex==nCol){
												bTastoOK=TRUE;
												/* reset lampada */
												ResetLampada(nMsgIOS,nMsgCPU,nMsgSettore,nMsgRowIndex,nMsgColIndex);
											}
										} else {
#ifdef TRACE
											trace_out_vstr_date(1, "TEST : ERRORE IN RICERCA UBICAZIONE [IOS:%d CPU:%d s:%02d,r:%02d,c:%02d]", nMsgIOS,nMsgCPU,nMsgSettore,nMsgRowIndex,nMsgColIndex);
#endif
										}
									}
									break;
									case IOS_PKL_LIGHT_DAMAGED:
									{
										PUBICAZIONE pUbicazione;
										int nMsgIOS,nMsgCPU,nMsgSettore,nMsgRowIndex,nMsgColIndex;

										sscanf(szText,"%d,%d,%d,%d,%d",&nMsgIOS,&nMsgCPU,&nMsgSettore,&nMsgRowIndex,&nMsgColIndex);

#ifdef TRACE
										trace_out_vstr_date(1, "TEST : Ricevuto IOS_PKL_LIGHT_DAMAGED [IOS:%d CPU:%d s:%02d,r:%02d,c:%02d]", nMsgIOS,nMsgCPU,nMsgSettore,nMsgRowIndex,nMsgColIndex);
#endif

										if((pUbicazione=SearchUbicazione(&Ubicazioni,nMsgIOS,nMsgCPU,nMsgSettore,nMsgRowIndex,nMsgColIndex))!=NULL){
											if(nMsgSettore==nModulo && nMsgRowIndex==nRow && nMsgColIndex==nCol){
												if(!Test.bLampada){
													bTastoOK=TRUE;
													/* reset lampada */
													ResetLampada(nMsgIOS,nMsgCPU,nMsgSettore,nMsgRowIndex,nMsgColIndex);
												}
											}
										}
									}
									break;
								}
							break;
						}
					}
					usleep(1000*Cfg.nDelay);
				}
			}
			/*
			* Fine del test
			*/
			PQclear(PGRes);
		} while (Test.bLoop);
	}



	/*
	* Lettura della lista operatori
	*/
	ReadOperatori();

	ResetLuci();
	for(nIndex=0;nIndex<Cfg.nNumeroSettori;nIndex++){

		ResetSettore(pSettori[nIndex].nIsola,pSettori[nIndex].nSettore);


		/* gestire su mancanza letura badge del giorno ... vedi registry ... */
		// pSettori[nIndex].nAttesaBadge = TRUE;	
		// pSettori[nIndex].nOpBlinking=1;
		//
		/*
		* devo decidere se il settore e' in attesa lettura badge oppure no:
		* se non ci sono letture in giornata alloa sono in attesa lettura badge altrimenti vale la situazione atuale
		*/
		/* 
		* se non ci sono letture in giornata ... metto in stato attesa badge 
		*/
		if(Cfg.nGestioneBadge){
			if(get_letture_badge(pSettori[nIndex].nIsola,pSettori[nIndex].nSettore)==0){
				pSettori[nIndex].nAttesaBadge = TRUE;	
				pSettori[nIndex].nOpBlinking=1;
			}
		}
			

		/*
		* rm 06-04-2000 : ritardo per evitare il riempimento della coda msg di LynxOS (MAX 35 !!)
		*/
		usleep(10*1000*Cfg.nDelay);


	}
	pDatiLinea->nStatoLinea_1=LINEA_IN_STOP;
	pDatiLinea->nStatoLinea_2=LINEA_IN_STOP;

	CongelaPKL();



	if(Cfg.bQuit){
		SafeIntFunc();
	}
	/*
	* gestione program loop
	*/
	for(;;){
		/* 
		* resetto l'evento
		*/
		Evento.nSettore=-1;
		Evento.nEvento=-1;

		/*
		* Gestione Messaggi Ricevuti
		*/
		nAllMsg = 0;
		if((nCarIn = ReceiveMsgNoWait(ProcList[PROC_SETTORI].nQNumber, &nAllMsg, szInMsg))>=0){
			UnpackStructMsg(szInMsg, &InMsgStruct, szText, nCarIn);

			switch (InMsgStruct.srce-1){
				case PROC_MAIN:
					switch(InMsgStruct.code){
						case REFRESH_DATA:
							ReadUbicazioni(&Ubicazioni);
							SortUbicazioni(&Ubicazioni);

							ReadOperatori();
							/*
							* 28-09-2000 : TODO : DA FINIRE
							*/

						break;
						case SIMULA_LUCE:
						{
							int nIsola;
							int nSettore;

#ifdef TRACE
							trace_out_vstr_date(1, "SIMULA LUCE [%s]",szText);
#endif
							sscanf(szText,"%d,%d",&nIsola,&nSettore);
							do_simula_luce(nIsola,nSettore);
						}
						break;
						case BARCODE_ID_PRODOTTO:
						{
							int nIsola;
							int nSettore;
							char szBarcode[128];
							int nBadge=0;

#ifdef TRACE
							/* trace_out_vstr_date(1, "BARCODE_ID_PRODOTTO [%s]",szText); */
#endif
							/*
							* rm 16-11-2019 : implementata lettura badge
							* il codice badge e' del tipo %<CODE>? ma il primo carattere (%) viene gia' tolto da display...
							*/
							sscanf(szText,"%d,%d,%s",&nIsola,&nSettore,szBarcode);

							Evento.nIsola=nIsola;
							Evento.nSettore=nSettore;
							strcpy(Evento.szBarcode,szBarcode);

							nIndex=0;
							nBadge=0;
							do {
								/* si tratta di un badge ? */
								if(szBarcode[nIndex]=='?'){
									nBadge=1;
									break;
								}
								nIndex++;
							} while(szBarcode[nIndex]!='\0');


							if(nBadge){
								Evento.nEvento=BADGE_ID;
#ifdef TRACE
								trace_out_vstr_date(1, "BADGE_ID EVENTO CodEvento: [%d] Isola: [%d] Settore: [%d] Barcode [%s]",Evento.nEvento,Evento.nIsola,Evento.nSettore,Evento.szBarcode);
#endif
							} else {
								Evento.nEvento=BARCODE_ID_PRODOTTO;
#ifdef TRACE
								trace_out_vstr_date(1, "BARCODE_ID_PRODOTTO EVENTO CodEvento: [%d] Isola: [%d] Settore: [%d] Barcode [%s]",Evento.nEvento,Evento.nIsola,Evento.nSettore,Evento.szBarcode);
#endif
							}

						}
						break;
						/*
						* gestione start/stop linea
						* rm : differenziato per isola
						*/
						case START_LINEA:
						{
							int nIndex;
							int nIsola;

							nIsola=atoi(szText);

							ReadUbicazioni(&Ubicazioni);
							SortUbicazioni(&Ubicazioni);

							ReadOperatori();

							/*
							* ripristino stato displays e luci
							*/
							for(nIndex=0;nIndex<Cfg.nNumeroDisplays;nIndex++){
								if(nIsola==0 || Cfg.Displays[nIndex].nIsola==nIsola){
									UpdateDisplay(&(Cfg.Displays[nIndex]),TUTTO);
								}
							}
							for(nIndex=0;nIndex<Cfg.nNumeroSettori;nIndex++){
								if(nIsola==0 || pSettori[nIndex].nIsola==nIsola){
									if(pSettori[nIndex].nStatoLampada==1){
										SetLampada(pSettori[nIndex].nIOS,pSettori[nIndex].nCPU,pSettori[nIndex].nModulo,pSettori[nIndex].nRiga,pSettori[nIndex].nColonna);
									}
								}
							}

							/*
							* 29-10-2012 : rilettura configurazione per parametri operativi (AttesaPDFOrdineCompleto)
							*/
							ReadConfiguration(FALSE);

							ScongelaPKL();

							switch(nIsola){
								case 0:
									pDatiLinea->nStatoLinea_1=LINEA_IN_START;
									pDatiLinea->nStatoLinea_2=LINEA_IN_START;
								break;
								case 1:
									pDatiLinea->nStatoLinea_1=LINEA_IN_START;
								break;
								case 2:
									pDatiLinea->nStatoLinea_2=LINEA_IN_START;
								break;
							}
						}
						break;
						case STOP_LINEA:
						{
							int nIsola;

							nIsola=atoi(szText);

							CongelaPKL();

							switch(nIsola){
								case 0:
									pDatiLinea->nStatoLinea_1=LINEA_IN_STOP;
									pDatiLinea->nStatoLinea_2=LINEA_IN_STOP;
								break;
								case 1:
									pDatiLinea->nStatoLinea_1=LINEA_IN_STOP;
								break;
								case 2:
									pDatiLinea->nStatoLinea_2=LINEA_IN_STOP;
								break;
							}
						}
						break;
						case RESET_SETTORE:
						{
							int nIsola,nSettore;

							sscanf(szText,"%d,%d",&nIsola,&nSettore);
							/*
							* resetto solo il settore specificato
							*/
							ResetSettore(nIsola,nSettore);
						}
						break;
						case RESET_ALL:
							/*
							* resetto tutti i settori
							*/
							ResetLuci();
							for(nIndex=0;nIndex<Cfg.nNumeroSettori;nIndex++){
								ResetSettore(pSettori[nIndex].nIsola,pSettori[nIndex].nSettore);
								/*
								* rm 06-04-2000 : ritardo per evitare il riempimento della coda msg di LynxOS (MAX 35 !!)
								*/
								usleep(1000*Cfg.nDelay);
							}
						break;
						case SAVE_STATE:
							if(!SaveState(szText)){
								SendMessage(PROC_MAIN, PROC_SETTORI,  DISPLAY_MSG, "Errore in salvataggio stato settori\n");
							}
						break;

						case RESTORE_STATE:
							ScongelaPKL();
							if(!RestoreState(szText)){
								SendMessage(PROC_MAIN, PROC_SETTORI,  DISPLAY_MSG, "Errore in caricamento stato settori\n");
							}
							/* 
							* se non ci sono letture in giornata ... metto in stato attesa badge 
							*/
							for(nIndex=0;nIndex<Cfg.nNumeroSettori;nIndex++){
								if(Cfg.nGestioneBadge){
									if(get_letture_badge(pSettori[nIndex].nIsola,pSettori[nIndex].nSettore)==0){
										pSettori[nIndex].nAttesaBadge = TRUE;	
										pSettori[nIndex].nOpBlinking=1;
									}
								} else {
									pSettori[nIndex].nAttesaBadge = FALSE;	
									pSettori[nIndex].nOpBlinking=0;
								}
							}
								
							CongelaPKL();
						break;
						case SET_BADGE:
						{
							int nIsola,nSettore;
							PSETTORE pSettore;
							int nSettoreIndex;

							sscanf(szText,"%d,%d",&nIsola,&nSettore);
#ifdef TRACE
							trace_out_vstr_date(1, "SET_BADGE : [%d] Isola: [%d] Settore: [%d]",nIsola,nSettore);
#endif
							if((nSettoreIndex=GetSettore(nIsola,nSettore))!=-1){
								if(Cfg.nGestioneBadge){
									pSettore=&pSettori[nSettoreIndex];
									pSettore->nAttesaBadge=FALSE;
									pSettore->nOpBlinking=0;
								}
							} else {
#ifdef TRACE
								trace_out_vstr_date(1, "ERROR - [%s] [%s]",nIsola,nSettore);
#endif
							}
						}
						break;
						case RESET_BADGE:
						{
							int nIsola,nSettore;
							PSETTORE pSettore;
							int nSettoreIndex;

							sscanf(szText,"%d,%d",&nIsola,&nSettore);
#ifdef TRACE
							trace_out_vstr_date(1, "RESET_BADGE : [%d] Isola: [%d] Settore: [%d]",nIsola,nSettore);
#endif
							if((nSettoreIndex=GetSettore(nIsola,nSettore))!=-1){
								if(Cfg.nGestioneBadge){
									pSettore=&pSettori[nSettoreIndex];
									pSettore->nAttesaBadge=TRUE;
									pSettore->nOpBlinking=1;
								}
							} else {
#ifdef TRACE
								trace_out_vstr_date(1, "ERROR - [%s] [%s]",nIsola,nSettore);
#endif
							}
						}
						break;
					}
				break;
				case PROC_IOS:
					switch(InMsgStruct.code){
						case IOS_PKL_BUTTON_PRESSED:
						{
							PUBICAZIONE pUbicazione;
							int nIOS,nCPU,nModulo,nSettore,nRowIndex,nColIndex;

							sscanf(szText,"%d,%d,%d,%d,%d",&nIOS,&nCPU,&nModulo,&nRowIndex,&nColIndex);

							Evento.nEvento=IOS_PKL_BUTTON_PRESSED;
							Evento.nRiga=nRowIndex;
							Evento.nColonna=nColIndex;
							Evento.nModulo=nModulo;
							Evento.nIOS=nIOS;
							Evento.nCPU=nCPU;

							/*
							* rm 04-04-2000 : Azzero il contatore guasti
							*/
							Cfg.Modulo[nModulo].DamageCounter[nRowIndex][nColIndex]=0;

							if((pUbicazione=SearchUbicazione(&Ubicazioni,nIOS,nCPU,nModulo,nRowIndex,nColIndex))!=NULL){
								Evento.pUbicazione=pUbicazione;
								Evento.nSettore=pUbicazione->nSettore;
								Evento.nIsola=pUbicazione->nIsola;
							} else {
#ifdef TRACE
								trace_out_vstr_date(1, "ERRORE IN RICERCA UBICAZIONE [IOS:%d CPU:%d s:%02d,r:%02d,c:%02d]", nIOS,nCPU,nModulo,nRowIndex,nColIndex);
#endif
							}
#ifdef TRACE
							trace_out_vstr_date(1, "Ricevuto IOS_PKL_BUTTON_PRESSED [IOS:%d,CPU:%d,M:%2d,R:%2d,C:%2d] Isola[%d]", 
								nIOS,nCPU,nModulo,nRowIndex,nColIndex,Evento.nIsola);
#endif
						}
						break;
						case IOS_PKL_LIGHT_DAMAGED:
						{
							PUBICAZIONE pUbicazione;
							PDISPLAYSTRUCT pDisplay;
							int nIOS,nCPU,nModulo,nRowIndex,nColIndex;
							char szMsg[128];
							char szBuffer[128];
							int nColonna;
							int nPiano;

							sscanf(szText,"%d,%d,%d,%d,%d",&nIOS,&nCPU,&nModulo,&nRowIndex,&nColIndex);


							if((pUbicazione=SearchUbicazione(&Ubicazioni,nIOS,nCPU,nModulo,nRowIndex,nColIndex))!=NULL){
								PSETTORE pSettore;
								int nSettoreIndex;




								if((nSettoreIndex=GetSettore(pUbicazione->nIsola,pUbicazione->nSettore))!=-1){
									pSettore=&pSettori[nSettoreIndex];
									sprintf(pSettore->szTitoloProd,"LUCE [%s]", pUbicazione->szCodUbi);
								}
#ifdef TRACE
								trace_out_vstr_date(1, "IOS_PKL_LIGHT_DAMAGED S:%d I:%d IOS:%d CPU:%d M:%d R:%2d C:%2d",
									pUbicazione->nSettore,
									pUbicazione->nIsola,
									nIOS,nCPU,nModulo,nRowIndex,nColIndex);
#endif
								/*
								* Avverto l'operatore 
								*/
								nColonna=atoi(SubStr(pUbicazione->szCodUbi,4,2));
								nPiano=atoi(SubStr(pUbicazione->szCodUbi,6,1));
								sprintf(szMsg,"ATTENZIONE ! Lampada Rotta : %s - %s [I:%d S:%d C:%d P:%d]\n",
									pUbicazione->szCodProd,
									pUbicazione->szCodUbi,
									pUbicazione->nIsola,
									pUbicazione->nSettore,
									nColonna,nPiano);

								SendMessage(PROC_MAIN, PROC_SETTORI,  DISPLAY_MSG, szMsg);

								/*
								* rm 04-04-2000 : gestione lampada rotta con contatore per
								*                 problemi IOS
								* Ricavo indice modulo PKL
								*/

								Cfg.Modulo[nModulo].DamageCounter[nRowIndex][nColIndex]++;
								/* rm 06-09-2017 - eliminato tutto il giro */
								if(0 && Cfg.Modulo[nModulo].DamageCounter[nRowIndex][nColIndex]>Cfg.nMaxDamages){

									pDisplay=&(Cfg.Displays[pUbicazione->nDisplay]);


									strcpy(szBuffer,"LUCE");
#ifdef ELIMINATO
									/*
									* Azzero puntino (buco del firmware della pkdis (Manfredi)
									*/
									DisplayWriteStringBCD( pDisplay->nIOS, pDisplay->nCPU, pDisplay->nModulo, 1, NORMAL, "    ",0,0);
									DisplayWriteString( pDisplay->nIOS, pDisplay->nCPU, pDisplay->nModulo, 0, NORMAL, szBuffer);

									/* 26-08-2005 rm : visualizzo colonna e piano */
									/* sprintf(szBuffer,"%2d%2d",nRowIndex,nColIndex); */
									nColonna=atoi(SubStr(pUbicazione->szCodUbi,4,2));
									nPiano=atoi(SubStr(pUbicazione->szCodUbi,6,1));
									sprintf(szBuffer,"%2d%2d",nColonna,nPiano);

									DisplayWriteStringBCD( pDisplay->nIOS, pDisplay->nCPU, pDisplay->nModulo, 1, NORMAL, szBuffer,2,0);
#endif

									/*
									* 19-04-2001 st: modifica disperata per problema ai cablaggi
									* visualizzo il messaggio luce per 1 sec. e successivamente bleffo
									* mandandomi un messaggio di pulsante premuto
									* usleep(1000*1000);
									* sprintf(szBuffer,"%d,%d,%d,%d,%d",nIOS,nCPU,nModule,nRowIndex,nColIndex);
									* SendMessage(PROC_SETTORI, PROC_IOS,  IOS_PKL_BUTTON_PRESSED, szBuffer);
									*/

								} else {
									/*
									* provo a riaccendere la luce dopo 2 decimi di secondo
									*/
									// rm+lg 04-08-2017 : cambio strategia di gestione lampada rotta
									// ora viene solo segnalato a video il problema.
									// perchè in conflitto con funzionamento plc/can (più luci accese su settore)
									// usleep(1000*200);
									// SetLampada(nIOS,nCPU,nModulo,nRowIndex,nColIndex);
								}
							}
						}
						break;
					}
				break;
			}
		}
		
		/* gestione badge */
		if(Evento.nSettore!=-1){
			GestioneBadge(&Evento);
		}

		/*
		* rm : gestione start/stop differenziata per isola 
		*/
		if(pDatiLinea->nStatoLinea_1==LINEA_IN_START && Evento.nSettore!=-1 && Evento.nIsola==1){
			GestioneSettore(&Evento);
		}
		if(pDatiLinea->nStatoLinea_2==LINEA_IN_START && Evento.nSettore!=-1 && Evento.nIsola==2){
			GestioneSettore(&Evento);
		}

		/*
		* gestione dei settori non interessati da eventi di tastiera
		*/
		Evento.nEvento=-1;
		for(nIndex=0;nIndex<Cfg.nNumeroSettori;nIndex++){
			if(pDatiLinea->nStatoLinea_1==LINEA_IN_START && pSettori[nIndex].nIsola==1){
				Evento.nSettore=pSettori[nIndex].nSettore;
				Evento.nIsola=pSettori[nIndex].nIsola;
				GestioneSettore(&Evento);
			}
			if(pDatiLinea->nStatoLinea_2==LINEA_IN_START && pSettori[nIndex].nIsola==2){
				Evento.nSettore=pSettori[nIndex].nSettore;
				Evento.nIsola=pSettori[nIndex].nIsola;
				GestioneSettore(&Evento);
			}
		}



		/*
		* ritardo in microsecondi 
		*/
		usleep(1000*Cfg.nDelay);
	}

	SafeIntFunc();
}	/* End of main() */

