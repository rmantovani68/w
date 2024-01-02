/*
* settori.c : gestione settori di magazzino
* 
* Progetto Easy Picking 3.0 : Wella
*
* Autore : Roberto Mantovani
*
* Copyright CNI srl 1995-2000
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

/*
* main()
*/
int main(int argc,char** argv)
{
	int nIndex;
	int nErr=0;
	int nOpt;
	BOOL bTest=FALSE;
	BOOL bDisplay=FALSE;
	BOOL bLoop=FALSE;
	BOOL bAccensioni=FALSE;
	int nSettore=0;
	char szFila[20];

	szFila[0]='\0';

	while ((nOpt = getopt(argc, argv, "actdls:p:f:")) != -1) {
		switch(nOpt){
			case 'a':	/* alone */
				Cfg.bAlone=TRUE;
			break;
			case 't':	/* test */
				bTest=TRUE;
			break;
			case 'd':	/* test display*/
				bDisplay=TRUE;
			break;
			case 's':	/* test luci accese */
				nSettore=atoi(optarg);
			break;
			case 'f':	/* test luci accese */
				strcpy(szFila,optarg);
			break;
			case 'l':	/* test loop */
				bLoop=TRUE;
			break;
			default:
				nErr++;
			break;
		}
	}
 



	if(nErr){
		printf("%s : copyright 1995-2000 CNI srl\n",argv[0]);
		printf("Uso : %s [-d]\n",argv[0]);
		printf("\t-a    : Run Alone\n");
		printf("\t-t    : Run Test\n");
		printf("\t-s settore  : settore\n");
		printf("\t-f fila     : fila\n");

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
	if(Cfg.bAlone && bTest){
		BOOL bTastoOK=FALSE;
		char szBuffer[256];
		PGresult *PGRes = NULL;
		int nDisplayIndex;
		int nIOS;
		int nCPU;
		int nModulo;
		int nRow;
		int nCol;
		PDISPLAYSTRUCT pDisplay;
		int nIndex;
		int nTuples;

		ResetLuci();
		for(nIndex=0;nIndex<Cfg.nNumeroSettori;nIndex++){
			ResetSettore(pSettori[nIndex].nIsola,pSettori[nIndex].nSettore);
			/*
			* rm 06-04-2000 : ritardo per evitare il riempimento della coda msg di LynxOS (MAX 35 !!)
			*/
			usleep(1000*10*Cfg.nDelay);
		}
		ScongelaPKL(1);
		ScongelaPKL(2);
		/*
		* Selezione di tutte le ubicazioni dell'impianto
		*/
		// PGRes=PGExecSQL(TRUE,"select ubicazione, codprod, cancella, locazione, isola, settore, display, ios, cpu, modulo, riga, colonna, priorita, cnistato from ubicazioni where isola=1 order by isola,priorita;");
		PGRes=PGExecSQL(TRUE,"select ubicazione, codprod, cancella, locazione, isola, settore, display, ios, cpu, modulo, riga, colonna, priorita, cnistato from ubicazioni where isola=2  order by isola,priorita;");

		nTuples=PQntuples(PGRes);

		for(nIndex=0;nIndex<nTuples;nIndex++){
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

			sprintf(szBuffer,"%d,%d,%d,%d,%d", nIOS,nCPU,nModulo,nRow,nCol);
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
									int nMsgIOS,nMsgCPU,nMsgModulo,nMsgRowIndex,nMsgColIndex;

									sscanf(szText,"%d,%d,%d,%d,%d",&nMsgIOS,&nMsgCPU,&nMsgModulo,&nMsgRowIndex,&nMsgColIndex);

#ifdef TRACE
									trace_out_vstr_date(1, "TEST : Ricevuto IOS_PKL_BUTTON_PRESSED [IOS:%d CPU:%d m:%02d,r:%02d,c:%02d]", nMsgIOS,nMsgCPU,nMsgModulo,nMsgRowIndex,nMsgColIndex);
#endif

									if((pUbicazione=SearchUbicazione(&Ubicazioni,nMsgIOS,nMsgCPU,nMsgModulo,nMsgRowIndex,nMsgColIndex))!=NULL){
										if(nMsgIOS==nIOS && nMsgCPU==nCPU && nMsgModulo==nModulo && nMsgRowIndex==nRow && nMsgColIndex==nCol){
											bTastoOK=TRUE;
										}
									}
								}
								break;
								case IOS_PKL_LIGHT_DAMAGED:
								{
									PUBICAZIONE pUbicazione;
									int nMsgIOS,nMsgCPU,nMsgModulo,nMsgRowIndex,nMsgColIndex;

									sscanf(szText,"%d,%d,%d,%d,%d",&nMsgIOS,&nMsgCPU,&nMsgModulo,&nMsgRowIndex,&nMsgColIndex);

#ifdef TRACE
									trace_out_vstr_date(1, "TEST : Ricevuto IOS_PKL_LIGHT_DAMAGED [IOS:%d CPU:%d m:%02d,r:%02d,c:%02d]", nMsgIOS,nMsgCPU,nMsgModulo,nMsgRowIndex,nMsgColIndex);
#endif

									if((pUbicazione=SearchUbicazione(&Ubicazioni,nMsgIOS,nMsgCPU,nMsgModulo,nMsgRowIndex,nMsgColIndex))!=NULL){
										if(nMsgIOS==nIOS && nMsgCPU==nCPU && nMsgModulo==nModulo && nMsgRowIndex==nRow && nMsgColIndex==nCol){
											bTastoOK=TRUE;
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
	}



	/*
	* Lettura della lista operatori
	*/
	ReadOperatori();

	ResetLuci();
	for(nIndex=0;nIndex<Cfg.nNumeroSettori;nIndex++){

		ResetSettore(pSettori[nIndex].nIsola,pSettori[nIndex].nSettore);
		/*
		* rm 06-04-2000 : ritardo per evitare il riempimento della coda msg di LynxOS (MAX 35 !!)
		*/
		usleep(10*1000*Cfg.nDelay);
	}
	pDatiLinea->nStatoLinea_1=LINEA_IN_STOP;
	pDatiLinea->nStatoLinea_2=LINEA_IN_STOP;
	CongelaPKL(0);

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
							ScongelaPKL(nIsola);
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

							CongelaPKL(nIsola);
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
							if(!RestoreState(szText)){
								SendMessage(PROC_MAIN, PROC_SETTORI,  DISPLAY_MSG, "Errore in caricamento stato settori\n");
							}
						break;
					}
				break;
				case PROC_IOS:
					switch(InMsgStruct.code){
						case IOS_PKL_BUTTON_PRESSED:
						{
							PUBICAZIONE pUbicazione;
							int nIOS,nCPU,nModule,nRowIndex,nColIndex;

							sscanf(szText,"%d,%d,%d,%d,%d",&nIOS,&nCPU,&nModule,&nRowIndex,&nColIndex);

							Evento.nEvento=IOS_PKL_BUTTON_PRESSED;
							Evento.nRiga=nRowIndex;
							Evento.nColonna=nColIndex;
							Evento.nModulo=nModule;
							Evento.nIOS=nIOS;
							Evento.nCPU=nCPU;
							/*
							* rm 04-04-2000 : Azzero il contatore guasti
							*/
							nIndex=0;
							while(nIndex<Cfg.nPKLNumber){
								if(Cfg.PKL[nIndex].nIOS==nIOS && Cfg.PKL[nIndex].nCPU==nCPU && Cfg.PKL[nIndex].nModulo==nModule){
									Cfg.PKL[nIndex].DamageCounter[nRowIndex][nColIndex]=0;
									Evento.nIsola=nIndex+1;
									break;
								}
								nIndex++;
							}

							if((pUbicazione=SearchUbicazione(&Ubicazioni,nIOS,nCPU,nModule,nRowIndex,nColIndex))!=NULL){
								Evento.pUbicazione=pUbicazione;
								Evento.nSettore=pUbicazione->nSettore;
								Evento.nIsola=pUbicazione->nIsola;
							}
#ifdef TRACE
							trace_out_vstr_date(1, "Ricevuto IOS_PKL_BUTTON_PRESSED [IOS:%d,CPU:%d,M:%2d,R:%2d,C:%2d] Isola[%d]", 
								nIOS,nCPU,nModule,nRowIndex,nColIndex,Evento.nIsola);
#endif
						}
						break;
						case IOS_PKL_LIGHT_DAMAGED:
						{
							PUBICAZIONE pUbicazione;
							PDISPLAYSTRUCT pDisplay;
							int nIOS,nCPU,nModule,nRowIndex,nColIndex;
							char szMsg[128];
							char szBuffer[128];
							int nColonna;
							int nPiano;

							sscanf(szText,"%d,%d,%d,%d,%d",&nIOS,&nCPU,&nModule,&nRowIndex,&nColIndex);


							if((pUbicazione=SearchUbicazione(&Ubicazioni,nIOS,nCPU,nModule,nRowIndex,nColIndex))!=NULL){
#ifdef TRACE
								trace_out_vstr_date(1, "IOS_PKL_LIGHT_DAMAGED S:%d I:%d IOS:%d CPU:%d M:%d R:%2d C:%2d",
									pUbicazione->nSettore,
									pUbicazione->nIsola,
									nIOS,nCPU,nModule,nRowIndex,nColIndex);
#endif
								/*
								* rm 04-04-2000 : gestione lampada rotta con contatore per
								*                 problemi IOS
								* Ricavo indice modulo PKL
								*/
								nIndex=0;
								while(nIndex<Cfg.nPKLNumber){
									if(Cfg.PKL[nIndex].nIOS==nIOS && Cfg.PKL[nIndex].nCPU==nCPU && Cfg.PKL[nIndex].nModulo==nModule){
										Cfg.PKL[nIndex].DamageCounter[nRowIndex][nColIndex]++;
										if(Cfg.PKL[nIndex].DamageCounter[nRowIndex][nColIndex]>Cfg.nMaxDamages){

											pDisplay=&(Cfg.Displays[pUbicazione->nDisplay]);

											strcpy(szBuffer,"LUCE");
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

											/*
											* Avverto l'operatore 
											*/
											sprintf(szMsg,"ATTENZIONE ! Lampada Rotta : %s [I:%d S:%d C:%d P:%d]\n",
												pUbicazione->szCodProd,
												pUbicazione->nIsola,
												pUbicazione->nSettore,
												nColonna,nPiano);

											SendMessage(PROC_MAIN, PROC_SETTORI,  DISPLAY_MSG, szMsg);

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
											usleep(1000*200);
											SetLampada(nIOS,nCPU,nModule,nRowIndex,nColIndex);
										}
									}
									nIndex++;
								}
							}
						}
						break;
					}
				break;
			}
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

