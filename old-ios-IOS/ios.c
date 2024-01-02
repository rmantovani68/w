/*
* ios.c : gestione IOS
* 
* Progetto Easy Picking 3.0 : Wella
*
* Autore : Roberto Mantovani
*
* Copyright CNI srl 1995-2000
*
* Modifiche per impanto Wella:
* - Gestione di n schede IOS per testiere su piu' isole
* - Gestione di n displays per singolo settore
*/
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>

#ifdef HAS_IOS_CNI
	#include <hwcni.h>
#endif

#include <pmx_msq.h>
#include <msq_lib.h>
#include <shared.h>
#ifdef TRACE
	#include <trace.h>
#endif

#include <ca_file.h>
#include <proc_list.h>

#include "ios.h"

#define SARTI

int nAllMsg;
int nCarIn;
MSGHDRINT InMsgStruct;
char szInMsg[INFO_LEN];
char szText[INFO_LEN];
char szBufTrace[80];
char *pszNomeModulo;

char szTimeBuffer[80];
char szDateBuffer[80];


CFGSTRUCT	Cfg;                /* struttura della configurazione */
PROCESSO ProcList[NUM_PROC];	/* processi componenti il progetto */

/*
* ReadConfiguration()
* lettura della configurazione dal file cni.cfg
*/
void ReadConfiguration(void)
{
	char szParagraph[128];
	char szCurrentDirectory[128];
	char szCniCfg[128];
	char szProcIndex[128];
	char szProcBuffer[128];
	int nIndex;

	/*
	* leggo il file di configurazione dalla directory corrente
	*/
	getcwd(szCurrentDirectory,sizeof(szCurrentDirectory));
	
	strcpy(szCniCfg,szCurrentDirectory);
	strcat(szCniCfg,"/cni.cfg");
	/*
	* lettura del file di configurazione
	*/
	strcpy(szParagraph,"General Settings");
	Cfg.nDebugVersion=xncGetFileInt(szParagraph,"DebugVersion",0,szCniCfg,NULL);

	/*
	* Estraggo i parametri relativi ai processi 
	*/
	for (nIndex=0; nIndex<NUM_PROC; nIndex++) {
		sprintf(szProcIndex,"Proc_%02d",nIndex);
		xncGetFileString("procinfo",szProcIndex, "", szProcBuffer, 80,szCniCfg,NULL);
		sscanf(szProcBuffer, "%[^,],%d,%d,%d",
				ProcList[nIndex].szProcName,
				&ProcList[nIndex].nQKey,
				&ProcList[nIndex].nPriority,
				&ProcList[nIndex].bExecute);

		ProcList[nIndex].nGId=nIndex+1;

		/*
		* resetto il numero coda messaggi del processo
		*/
		ProcList[nIndex].nQNumber=0;
	}
	/*
	* parametri specifici dell'applicazione
	*/
	/*
	* Delay in millisecondi 
	*/
	Cfg.nDelay=xncGetFileInt(ProcList[PROC_IOS].szProcName,"Delay",100,szCniCfg,NULL);
	/*
	* Delay in millisecondi per la simulazione pressione tasto
	*/
	Cfg.nDelaySimulazione=xncGetFileInt(ProcList[PROC_IOS].szProcName,"DelaySimulazione",100,szCniCfg,NULL);
	/*
	* Numero delle schede IOS connesse (da aprire)
	*/
	Cfg.nIOS=xncGetFileInt(ProcList[PROC_IOS].szProcName,"IOSNumber",1,szCniCfg,NULL);
	/*
	* Numero dei moduli PKL 
	*/
	Cfg.nPKLNumber=xncGetFileInt(ProcList[PROC_IOS].szProcName,"PKLNumber",1,szCniCfg,NULL);

	/*
	* dati relativi ai moduli PKL
	*/
	for(nIndex=0;nIndex<Cfg.nPKLNumber;nIndex++){
		sprintf(szProcIndex,"PKL_%02d",nIndex);
		xncGetFileString(ProcList[PROC_IOS].szProcName,szProcIndex, "", szProcBuffer, 80,szCniCfg,NULL);

		sscanf(szProcBuffer, "%d,%d,%d",
			&(Cfg.PKL[nIndex].nIOS),
			&(Cfg.PKL[nIndex].nCPU),
			&(Cfg.PKL[nIndex].nModulo));
	}

	/*
	* Simulazione IOS
	*/
	Cfg.nSimulazione=xncGetFileInt(ProcList[PROC_IOS].szProcName,"Simulazione",0,szCniCfg,NULL);

}

/*
* WaitIOS()
* attendo che il codice comando in TX del modulo 
* e della IOS passate in ingresso sia libero (0)
* rm 06-09-2001 : aggiunta gestione timeout
*/
void WaitIOS(int nIOS,int nCPU,int nModulo)
{
	struct timeval tvActualTime;
	struct timeval tvStartTime;
	int nTimePassed=0;    /* msecs */

	gettimeofday(&tvStartTime,NULL);

#ifdef HAS_IOS_CNI
	if(nIOS<Cfg.nIOS){
		while(nTimePassed < IOS_TIMEOUT && IOS_TX_BYTE(nIOS,nCPU,nModulo,0)){

			gettimeofday(&tvActualTime,NULL);

			nTimePassed=(tvActualTime.tv_sec-tvStartTime.tv_sec)*1000;     /* secondi */
			nTimePassed+=(tvActualTime.tv_usec-tvStartTime.tv_usec)/1000;   /* milli secondi */
#ifdef TRACE
			trace_out_vstr(1, "TimePassed = %6d",nTimePassed);
#endif

			usleep(1000*Cfg.nDelay);
		}
		if(nTimePassed > IOS_TIMEOUT){
			char szMsg[128];

#ifdef TRACE
			trace_out_vstr(1, "Errore di TIMEOUT su IOS [%d.%d.%d]",nIOS,nCPU,nModulo);
#endif
			sprintf(szMsg, "Errore di TIMEOUT su IOS [%d.%d.%d]\n",nIOS,nCPU,nModulo);
			SendMessage(PROC_MAIN, PROC_IOS, DISPLAY_MSG, szMsg);
		}
	}
	/*
	* rm 27-02-2001 : inserito ritardo per mancata accensione luce
	*                 rif. (Sarti)
	*/
#ifdef SARTI
	usleep(20000);
#endif
#endif
}

/* 
* main() 
*/
void main(int argc, char **argv)
{
	int nRC;
	int nIndex;
	int nIOS,nCPU,nModule,nRowIndex,nColIndex;
	int nBlinkState;
	char szSendBuffer[128];

	pszNomeModulo = argv[0];

	/*
	* Lettura della configurazione corrente
	*/
	ReadConfiguration();

#ifdef TRACE
	/* Apro il file trace */
	sprintf(szBufTrace, "%s%s", pszNomeModulo, TRACE_FILE_SUFFIX);
	open_trace(pszNomeModulo, szBufTrace, TRACE_FILE_MAXSIZE);

	trace_out_vstr(1,"Module Name : %s",pszNomeModulo);
	trace_out_vstr(1,"Simulazione : %d",Cfg.nSimulazione);
	trace_out_vstr_date(1,"Started");
#endif

	/* Apro la coda messaggi principale */
	if((ProcList[PROC_MAIN].nQNumber = OpenMsgQ(ProcList[PROC_MAIN].nQKey))<0){
#ifdef TRACE
		trace_out_vstr(1, "%s : Apertura coda messaggi principale fallita", pszNomeModulo);
#endif
	}
	/* Apro la coda messaggi processo Settori */
	if((ProcList[PROC_SETTORI].nQNumber = OpenMsgQ(ProcList[PROC_SETTORI].nQKey))<0){
#ifdef TRACE
		trace_out_vstr(1, "%s : Apertura coda messaggi settori fallita", pszNomeModulo);
#endif
	}
	/* Apro la coda messaggi locale */
	if((ProcList[PROC_IOS].nQNumber = OpenMsgQ(ProcList[PROC_IOS].nQKey))<0){
#ifdef TRACE
		trace_out_vstr(1, "%s : Apertura coda messaggi locale fallita", pszNomeModulo);
#endif
	}


	/* Predispongo l'utilizzo dell'uscita di sicurezza */
	signal(SIGTERM, SafeIntFunc);

#ifdef HAS_IOS_CNI
	for(nIndex=0;nIndex<Cfg.nIOS;nIndex++){
		Cfg.nFdIOS[nIndex]=0xff;	/* NON NULL VALUE */
		nRC=iosOpenGeneral(nIndex,&(Cfg.IOSInfo[nIndex]),&Cfg.nFdIOS[nIndex]);
		/*
		* gestire il valore di ritorno ....
		*/
		if(nRC==-1){
#ifdef TRACE
			trace_out_vstr(1, "Errore in apertura Scheda IOS Numero [%2d]",nIndex);
#endif
		} else {
#ifdef TRACE
			trace_out_vstr(1, "Scheda IOS Numero %2d - Versione : %c%c%c%c", 
				nIndex,
				Cfg.IOSInfo[nIndex].base[0x225],
				Cfg.IOSInfo[nIndex].base[0x226],
				Cfg.IOSInfo[nIndex].base[0x227],
				Cfg.IOSInfo[nIndex].base[0x228]);
#endif
			/* abilita il riaggancio */
			Cfg.IOSInfo[nIndex].base[0x24d]=(BYTE)0;
			sleep(1);
			/* disabilita il riaggancio */
			Cfg.IOSInfo[nIndex].base[0x24d]=(BYTE)0x55;
		}
	}
#endif

	/*
	* avverto MAIN dello start
	*/
	SendMessage(PROC_MAIN, PROC_IOS, PROGRAM_STARTED, NULL);


	/*
	* gestione program loop
	*/
	for(;;){

		/*
		* Gestione Messaggi Ricevuti
		*/
		nAllMsg = 0;
		if((nCarIn = ReceiveMsgNoWait(ProcList[PROC_IOS].nQNumber, &nAllMsg, szInMsg))>=0){
			UnpackStructMsg(szInMsg, &InMsgStruct, szText, nCarIn);

			switch (InMsgStruct.srce-1) {
				/* 
				* gestisco i messaggi provenienti da MAIN e SETTORI 
				*/
				case PROC_SETTORI:
				case PROC_MAIN:

					switch(InMsgStruct.code){
						case IOS_PKL_SET_RC:
							/* 
							* Parametri : Scheda IOS,CPU,Modulo,Riga,Colonna
							*/
							if(szText==NULL){
								break;
							}
							sscanf(szText,"%d,%d,%d,%d,%d",&nIOS,&nCPU,&nModule,&nRowIndex,&nColIndex);

							WaitIOS(nIOS,nCPU,nModule);

#ifdef TRACE
							trace_out_vstr_date(1,"IOS_PKL_SET_RC [IOS:%d,CPU:%d,M:%2d,R:%2d,C:%2d]", nIOS,nCPU,nModule,nRowIndex,nColIndex);
#endif
							if(nIOS<Cfg.nIOS){
								IOS_TX_BYTE(nIOS,nCPU,nModule,1)=(BYTE)nColIndex;      /* riga */
								IOS_TX_BYTE(nIOS,nCPU,nModule,2)=(BYTE)nRowIndex;      /* colonna */
								IOS_TX_BYTE(nIOS,nCPU,nModule,0)=(BYTE)PKL_LIGHT_ON;   /* comando */
							}
							/*
							* se sono in simulazione restituisco la pressione tasto
							*/
							if(Cfg.nSimulazione){
								usleep(1000*Cfg.nDelaySimulazione);

								sprintf(szSendBuffer,"%d,%d,%d,%d,%d",nIOS,nCPU,nModule,nRowIndex,nColIndex);
#ifdef TRACE
								trace_out_vstr_date(1,"PKL_BUTTON_PRESSED [IOS:%d,CPU:%d,M:%2d,R:%2d,C:%2d]", nIOS,nCPU,nModule,nRowIndex,nColIndex);
#endif

								SendSpecificMsg(ProcList[PROC_SETTORI].nQNumber, 
									ProcList[PROC_SETTORI].nGId, 
									ProcList[PROC_IOS].nGId, 
									IOS_PKL_BUTTON_PRESSED, szSendBuffer);

								if(nIOS<Cfg.nIOS){
									IOS_TX_BYTE(nIOS,nCPU,nModule,1)=(BYTE)nColIndex;      /* riga */
									IOS_TX_BYTE(nIOS,nCPU,nModule,2)=(BYTE)nRowIndex;      /* colonna */
									IOS_TX_BYTE(nIOS,nCPU,nModule,0)=(BYTE)PKL_LIGHT_OFF;  /* comando */
								}
							}
						break;
						case IOS_PKL_RESET_RC:
							/* 
							* Parametri : Scheda IOS,CPU,Modulo,Riga,Colonna
							*/
							if(szText==NULL){
								break;
							}
							sscanf(szText,"%d,%d,%d,%d,%d",&nIOS,&nCPU,&nModule,&nRowIndex,&nColIndex);
#ifdef TRACE
							trace_out_vstr_date(1,"IOS_PKL_RESET_RC [IOS:%d,CPU:%d,M:%2d R:%2d C:%2d]", nIOS, nCPU, nModule,nRowIndex,nColIndex);
#endif
							WaitIOS(nIOS,nCPU,nModule);

							if(nIOS<Cfg.nIOS){
								IOS_TX_BYTE(nIOS,nCPU,nModule,1)=(BYTE)nColIndex;      /* riga */
								IOS_TX_BYTE(nIOS,nCPU,nModule,2)=(BYTE)nRowIndex;      /* colonna */
								IOS_TX_BYTE(nIOS,nCPU,nModule,0)=(BYTE)PKL_LIGHT_OFF;  /* comando */
							}
						break;
						case IOS_PKL_CONGELA:
							/* 
							* Parametri : Scheda IOS,CPU,Modulo
							*/
							sscanf(szText,"%d,%d,%d",&nIOS,&nCPU,&nModule);
#ifdef TRACE
							trace_out_vstr_date(1,"IOS_PKL_CONGELA [IOS:%d,CPU:%d,M:%2d]", nIOS, nCPU, nModule);
#endif
							WaitIOS(nIOS,nCPU,nModule);

							if(nIOS<Cfg.nIOS){
								IOS_TX_BYTE(nIOS,nCPU,nModule,1)=(BYTE)1;              /* azione */
								IOS_TX_BYTE(nIOS,nCPU,nModule,0)=(BYTE)PKL_FREEZE;     /* comando */
							}
						break;
						case IOS_PKL_SCONGELA:
							/* 
							* Parametri : Scheda IOS,CPU,Modulo
							*/
							sscanf(szText,"%d,%d,%d",&nIOS,&nCPU,&nModule);
#ifdef TRACE
							trace_out_vstr_date(1,"IOS_PKL_SCONGELA [IOS:%d,CPU:%d,M:%2d]", nIOS, nCPU, nModule);
#endif
							WaitIOS(nIOS,nCPU,nModule);

							if(nIOS<Cfg.nIOS){
								IOS_TX_BYTE(nIOS,nCPU,nModule,1)=(BYTE)0;              /* azione */
								IOS_TX_BYTE(nIOS,nCPU,nModule,0)=(BYTE)PKL_FREEZE;     /* comando */
							}
						break;
						case IOS_PKL_RESET_ALL:
							/* 
							* Parametri : Scheda IOS,CPU,Modulo
							*/
							sscanf(szText,"%d,%d,%d",&nIOS,&nCPU,&nModule);
#ifdef TRACE
							trace_out_vstr_date(1,"IOS_PKL_RESET_ALL [IOS:%d,CPU:%d,M:%2d]", nIOS, nCPU, nModule);
#endif
							WaitIOS(nIOS,nCPU,nModule);

							if(nIOS<Cfg.nIOS){
								IOS_TX_BYTE(nIOS,nCPU,nModule,0)=(BYTE)PKL_RESET;      /* comando */
							}
						break;
						case IOS_PKL_TEST:
							/* 
							* Parametri : Scheda IOS,CPU,Modulo
							*/
							sscanf(szText,"%d,%d,%d",&nIOS,&nCPU,&nModule);
							WaitIOS(nIOS,nCPU,nModule);

							if(nIOS<Cfg.nIOS){
								IOS_TX_BYTE(nIOS,nCPU,nModule,0)=(BYTE)PKL_TEST;       /* comando */
							}
						break;

						case IOS_DPY_CHAR:
						{
							int nDigit;
							int nComando;
							int nDPYByte[4];

							/* ricavo i dati da visualizzare */
							sscanf(szText,"%d,%d,%d,%d,%d,%c",
								&nIOS,
								&nCPU,
								&nModule,
								&nDigit,
								&nBlinkState,
								(char *)&(nDPYByte[0]));


							switch(nBlinkState){
								case 0: default: nComando = DPY_ASCII_DGT; break;
								case 1: nComando = DPY_ASCII_DGT_BLINK; break;
							}

							WaitIOS(nIOS,nCPU,nModule);
#ifdef TRACE
							trace_out_vstr_date(1,"IOS_DPY_CHAR [IOS:%d,CPU:%d,M:%2d D%d:%c]", nIOS, nCPU, nModule,nDigit, nDPYByte[0]);
#endif

							if(nIOS<Cfg.nIOS){
								IOS_TX_BYTE(nIOS,nCPU,nModule,1)=(BYTE)nDigit;           /* digit */
								IOS_TX_BYTE(nIOS,nCPU,nModule,2)=(BYTE)nDPYByte[0];      /* ascii */
								IOS_TX_BYTE(nIOS,nCPU,nModule,0)=(BYTE)nComando;         /* comando */
							}
						}
						break;

						case IOS_DPY_STRING:
						{
							int nIndex;
							int nComando;
							int nDPYByte[4];

							/* ricavo i dati da visualizzare */
							sscanf(szText,"%d,%d,%d,%d,%d,%x,%x,%x,%x",
								&nIOS,
								&nCPU,
								&nModule,
								&nRowIndex,
								&nBlinkState,
								&nDPYByte[0],
								&nDPYByte[1],
								&nDPYByte[2],
								&nDPYByte[3]);

							switch(nBlinkState){
								case 0: default: nComando = DPY_ASCII_DGT; break;
								case 1: nComando = DPY_ASCII_DGT_BLINK; break;
							}
#ifdef TRACE
							trace_out_vstr_date(1,"IOS_DPY_STRING [IOS:%d,CPU:%d,M:%2d R%d:%c%c%c%c]", nIOS, nCPU, nModule, nRowIndex, nDPYByte[0], nDPYByte[1], nDPYByte[2], nDPYByte[3]);
#endif

							for(nIndex=0;nIndex<2;nIndex++){
								
								WaitIOS(nIOS,nCPU,nModule);

								if(nIOS<Cfg.nIOS){
									IOS_TX_BYTE(nIOS,nCPU,nModule,1)=(BYTE)((nRowIndex*4)+nIndex*2); /* digit */
									IOS_TX_BYTE(nIOS,nCPU,nModule,2)=(BYTE)nDPYByte[nIndex*2];       /* ascii */
									IOS_TX_BYTE(nIOS,nCPU,nModule,3)=(BYTE)nDPYByte[nIndex*2+1];     /* ascii */
									IOS_TX_BYTE(nIOS,nCPU,nModule,0)=(BYTE)nComando;                 /* comando */
								}

							}
						}
						break;

						case IOS_DPY_BCD:
						{
							int nIndex;
							int nComando;
							int nDPYByte[4];

							/* ricavo i dati da visualizzare */
							sscanf(szText,"%d,%d,%d,%d,%x,%x,%x",
								&nIOS,
								&nCPU,
								&nModule,
								&nRowIndex,
								&nDPYByte[0],
								&nDPYByte[1],
								&nDPYByte[2]);

							switch(nRowIndex){
								case 0: nComando=DPY_BCD_03_FLAG; break;
								case 1: nComando=DPY_BCD_47_FLAG; break;
							}
#ifdef TRACE
							trace_out_vstr_date(1,"IOS_DPY_BCD (%x) [IOS:%d,CPU:%d,M:%2d R%d:%x:%x:%x]", nComando, nIOS, nCPU, nModule, nRowIndex, nDPYByte[0], nDPYByte[1], nDPYByte[2]);
#endif
							WaitIOS(nIOS,nCPU,nModule);

							if(nIOS<Cfg.nIOS){
								IOS_TX_BYTE(nIOS,nCPU,nModule,1)=(BYTE)nDPYByte[0];	/* Digit 0-1 o 4-5 */
								IOS_TX_BYTE(nIOS,nCPU,nModule,2)=(BYTE)nDPYByte[1];	/* Digit 2-3 o 6-7 */
								IOS_TX_BYTE(nIOS,nCPU,nModule,3)=(BYTE)nDPYByte[2];	/* Flags per blink e '.' */
								IOS_TX_BYTE(nIOS,nCPU,nModule,0)=(BYTE)nComando;                 /* comando */
							}

						}
						break;
						case IOS_TEST_CMD:
						{
							int nIndex;
							int nComando;
							int nByte[4];
							int nNumBytes;

							/* ricavo i dati da spedire */
							sscanf(szText,"%d,%d,%d,%x,%x,%x,%x,%x,%x",
								&nIOS,
								&nCPU,
								&nModule,
								&nComando,
								&nNumBytes,
								&nByte[0],
								&nByte[1],
								&nByte[2],
								&nByte[3]);

#ifdef TRACE
							trace_out_vstr_date(1,"IOS_TEST_CMD (%x) [IOS:%d,CPU:%d,M:%2d %x:%x:%x:%x]", nComando, nIOS, nCPU, nModule, nByte[0], nByte[1], nByte[2], nByte[3]);
#endif
							WaitIOS(nIOS,nCPU,nModule);

							if(nIOS<Cfg.nIOS){
								for(nIndex=0;nIndex<nNumBytes;nIndex++){
									IOS_TX_BYTE(nIOS,nCPU,nModule,nIndex+1)=(BYTE)nByte[nIndex];
								}
								IOS_TX_BYTE(nIOS,nCPU,nModule,0)=(BYTE)nComando;                 /* comando */
							}

						}
						break;
					}
				break;
			}
		}

		/*
		* controllo gli eventi della IOS (tastiera o errore su lampada)
		* Byte 0 in ricezione
		*/
		for(nIndex=0;nIndex<Cfg.nPKLNumber;nIndex++){
			nIOS=Cfg.PKL[nIndex].nIOS;
			nCPU=Cfg.PKL[nIndex].nCPU;
			nModule=Cfg.PKL[nIndex].nModulo;

			switch(IOS_RX_BYTE(nIOS,nCPU,nModule,0)){
				case PKL_BUTTON_PRESSED:
					nColIndex=IOS_RX_BYTE(nIOS,nCPU,nModule,1);
					nRowIndex=IOS_RX_BYTE(nIOS,nCPU,nModule,2);

#ifdef TRACE
					trace_out_vstr_date(1,"PKL_BUTTON_PRESSED [IOS:%d,CPU:%d,M:%2d,R:%2d,C:%2d]", nIOS,nCPU,nModule,nRowIndex,nColIndex);
#endif
					sprintf(szSendBuffer,"%d,%d,%d,%d,%d",nIOS,nCPU,nModule,nRowIndex,nColIndex);

					SendSpecificMsg(ProcList[PROC_SETTORI].nQNumber, 
						ProcList[PROC_SETTORI].nGId, 
						ProcList[PROC_IOS].nGId, 
						IOS_PKL_BUTTON_PRESSED, szSendBuffer);

					/* reset comando */
					IOS_RX_BYTE(nIOS,nCPU,nModule,0)=0x0;
					

					/*
					* attendo che il codice comando in TX sia libero (0)
					*/
					WaitIOS(nIOS,nCPU,nModule);

					/*
					* Attesa forzata per evitare la perdita di comandi da parte della IOS
					*/
#ifdef SARTI
					usleep(20000);
#endif

					/* 
					* replica del comando in TX 
					*/
					IOS_TX_BYTE(nIOS,nCPU,nModule,0)=PKL_BUTTON_PRESSED;	
					
				break;

				case PKL_LIGHT_DAMAGED:
					nColIndex=IOS_RX_BYTE(nIOS,nCPU,nModule,1);
					nRowIndex=IOS_RX_BYTE(nIOS,nCPU,nModule,2);


#ifdef TRACE
					trace_out_vstr_date(1,"PKL_LIGHT_DAMAGED [IOS:%d,CPU:%d,M:%2d,R:%2d,C:%2d]", nIOS,nCPU,nModule,nRowIndex,nColIndex);
#endif
					sprintf(szSendBuffer,"%d,%d,%d,%d,%d",nIOS,nCPU,nModule,nRowIndex,nColIndex);

					SendSpecificMsg(ProcList[PROC_SETTORI].nQNumber, 
						ProcList[PROC_SETTORI].nGId, 
						ProcList[PROC_IOS].nGId, 
						IOS_PKL_LIGHT_DAMAGED, szSendBuffer);

					/* reset comando */
					IOS_RX_BYTE(nIOS,nCPU,nModule,0)=0x0;

					/*
					* attendo che il codice comando in TX sia libero (0)
					*/
					WaitIOS(nIOS,nCPU,nModule);

					/*
					* Attesa forzata per evitare la perdita di comandi da parte della IOS
					*/
#ifdef SARTI
					usleep(20000);
#endif
					/* 
					* replica del comando in TX 
					*/
					IOS_TX_BYTE(nIOS,nCPU,nModule,0)=PKL_LIGHT_DAMAGED;	

				break;
				case PKL_NULL_CMD:
					/*
					* non fare niente ...
					*/
				break;
				default:
				{
					BYTE bComando;
					
					/* comando non gestito */
					bComando=IOS_RX_BYTE(nIOS,nCPU,nModule,0);
#ifdef TRACE
					trace_out_vstr_date(1,"Comando [%d] [%d,%d,%d,%2d,%2d]", bComando, nIOS,nCPU,nModule,nRowIndex,nColIndex);
#endif
					/* reset comando */
					IOS_RX_BYTE(nIOS,nCPU,nModule,0)=0x0;

					/*
					* attendo che il codice comando in TX sia libero (0)
					*/
					WaitIOS(nIOS,nCPU,nModule);

					/*
					* Attesa forzata per evitare la perdita di comandi da parte della IOS
					*/
#ifdef SARTI
					usleep(20000);
#endif
					/* 
					* replica del comando in TX 
					*/
					IOS_TX_BYTE(nIOS,nCPU,nModule,0)=bComando;	
				}
				break;
			}
		}
		usleep(1000*Cfg.nDelay);
	}	 /* fine ciclo */
	SafeIntFunc();
}




/*
* Funzione SafeIntFunc:
* uscita di sicurezza
*/
void SafeIntFunc()
{
	int nIndex;

#ifdef TRACE
	trace_out_vstr_date(1,"Stopped");
#endif
#ifdef HAS_IOS_CNI
	/*
	* chiusura IOS
	*/
	for(nIndex=0;nIndex<Cfg.nIOS;nIndex++){
		close(Cfg.nFdIOS[nIndex]);
	}
#endif

	/*
	* avverto MAIN dello stop
	*/
	SendMessage(PROC_MAIN, PROC_IOS, PROGRAM_STOPPED, NULL);

	exit(1);
} /* Fine SafeIntFunc */

/*
* GetTime()
* get time of day as a string
*/
char *GetTime(long time,char *szTimeString)
{
	struct tm *ltime;
	long curr_time;

	curr_time = time;
	ltime = localtime(&curr_time);
	sprintf(szTimeString, "%.2d:%.2d:%.2d", ltime->tm_hour, ltime->tm_min, ltime->tm_sec);

	return szTimeString;
}

/*
* GetDate()
* get date a string
*/
char *GetDate(long time,char *szDateString)
{
	struct tm *ltime;
	long curr_time;

	curr_time = time;
	ltime = localtime(&curr_time);
	sprintf(szDateString, "%.2d-%.2d-%.2d", ltime->tm_mday, ltime->tm_mon+1, ltime->tm_year%100);

	return szDateString;
}

/*
* SendMessage()
* Spedizione messaggio con timeout e tentativi (NON BLOCCANTE)
*/
BOOL SendMessage(int nDest,int nSrce,int nMsgCode,char *szText)
{
	int nRC;

	if((nRC=SendSpecificMsg(ProcList[nDest].nQNumber, ProcList[nDest].nGId, ProcList[nSrce].nGId, nMsgCode, szText))!=0){
		trace_out_vstr_date(1,"Errore [%d] in spedizione Messaggio a [%s]", nRC,ProcList[nDest].szProcName);
		return FALSE;
	}

	return TRUE;
}
