/*
* ios-plc.c : gestione luci
* 
* Progetto Easy Picking 3.0 : IMS-VIDEO
*
* Autore : Roberto Mantovani
*
* Copyright A&L srl 2006
*
*/
#include <glib.h>

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/mman.h>

#ifdef HAS_IOS_CNI
	#include <hwcni.h>
#else
#warning "---------------------------------------"
#warning "ATTENZIONE : VERSIONE SENZA IOS A BORDO"
#warning "---------------------------------------"
#endif

#include <pmx_msq.h>
#include <msq_lib.h>
#include <shared.h>
#ifdef TRACE
	#include <trace.h>
#endif

#include <ep-common.h>
#include <picking.h>
#include <proc_list.h>

#include "ios.h"

/* #define SARTI */

int nAllMsg;
int nCarIn;
MSGHDRINT InMsgStruct;
char szInMsg[INFO_LEN];
char szText[INFO_LEN];
char szBufTrace[80];

char szTimeBuffer[80];
char szDateBuffer[80];


int nPID=0;                   /* Process ID */
CFGSTRUCT	Cfg;                /* struttura della configurazione */
PROCESSO ProcList[NUM_PROC];	/* processi componenti il progetto */

extern char rcsid[];
extern char __version__[];
extern char __module_name__[];
extern char __module_version__[];
extern char __customer__[];
extern char __authors__[];
extern char __copyright__[];
extern char __configuration_file__[];

int nAMATRIX ;
int nARIGA   ;
int nACOLON  ;
int nACCEN   ;
int nRACCEN  ;

int nNMATRIX ;
int nNRIGA   ;
int nNCOLON  ;
int nANNUL   ;
int nRANNUL  ;

int nSMATRIX ;
int nSRIGA   ;
int nSCOLON  ;
int nSPEGNI  ;
int nRSPEGNI ;
int nRESETL  ;
int nRRESETL ;
int nCONGELA ;


/*
* ReadConfiguration()
* lettura della configurazione dal file cni.cfg
*/
void ReadConfiguration(char *szCfgFileName)
{
	char szParagraph[128];
	char szCurrentDirectory[128];
	char szCfg[128];
	char szProcIndex[128];
	char szProcBuffer[128];
	int nIndex;

	/*
	* leggo il file di configurazione dalla directory corrente
	*/
	getcwd(szCurrentDirectory,sizeof(szCurrentDirectory));
	
	strcpy(szCfg,szCurrentDirectory);
	strcat(szCfg,"/");
	strcat(szCfg,szCfgFileName);
	/*
	* lettura del file di configurazione
	*/
	strcpy(szParagraph,"General Settings");
	Cfg.nDebugVersion=GetFileInt(szParagraph,"DebugVersion",0,szCfg,NULL);

	GetFileString(szParagraph,"PathTrace",       "../trace",    Cfg.szPathTrace,       80, szCfg, NULL);

	/*
	* Estraggo i parametri relativi ai processi 
	*/
	for (nIndex=0; nIndex<NUM_PROC; nIndex++) {
		sprintf(szProcIndex,"Proc_%02d",nIndex);
		GetFileString("procinfo",szProcIndex, "", szProcBuffer, 80,szCfg,NULL);
		sscanf(szProcBuffer, "%[^,],%d,%d,%d",
				ProcList[nIndex].szProcName,
				&ProcList[nIndex].nQKey,
				&ProcList[nIndex].nPriority,
				&ProcList[nIndex].bExecute);

		ProcList[nIndex].nGId=nIndex+1;

		/* 
		* rm 16-01-2002 : Elimino gli spazi in testa e in coda per 
		*                 problemi con la RunSimpleProcess()  e la lettura della condifgurazione
		*/
		StrTrimAll(ProcList[nIndex].szProcName);

		/*
		* resetto il numero coda messaggi del processo
		*/
		ProcList[nIndex].nQNumber=0;
	}
	/*
	* parametri specifici dell'applicazione
	*/
	/*
	* PLC Presente
	*/
	//Cfg.nPLC=GetFileInt(ProcList[PROC_IOS].szProcName,"PLC",1,szCfg,NULL);
	Cfg.nPLC=GetFileInt("PLC","PLC",1,szCfg,NULL);
	//GetFileString(ProcList[PROC_IOS].szProcName,"PLCHost","archimede",Cfg.szPLCHost,80,szCfg,NULL);
	GetFileString("PLC","PLCHost","archimede",Cfg.szPLCHost,80,szCfg,NULL);
	/*
	* Delay in millisecondi 
	*/
	Cfg.nDelay=GetFileInt(ProcList[PROC_IOS].szProcName,"Delay",100,szCfg,NULL);
	/*
	* Delay in millisecondi per la simulazione pressione tasto
	*/
	Cfg.nDelaySimulazione=GetFileInt(ProcList[PROC_IOS].szProcName,"DelaySimulazione",100,szCfg,NULL);

	/*
	* Simulazione IOS
	*/
	Cfg.nSimulazione=GetFileInt(ProcList[PROC_IOS].szProcName,"Simulazione",0,szCfg,NULL);
	/*
	* Numero delle schede IOS connesse (da aprire)
	*/
	Cfg.nIOS=GetFileInt(ProcList[PROC_IOS].szProcName,"IOSNumber",1,szCfg,NULL);


	/*
	* Lettura Lista variabili PLC
	*/
	ReadVarList(szCfg);

}

/*
* WaitIOS()
* attendo che il codice comando in TX del modulo 
* e della IOS passate in ingresso sia libero (0)
* rm 06-09-2001 : aggiunta gestione timeout
*/
void WaitIOS(int nIOS,int nCPU,int nModulo)
{
#ifdef HAS_IOS_CNI
	struct timeval tvActualTime;
	struct timeval tvStartTime;
	int nTimePassed=0;    /* msecs */

	gettimeofday(&tvStartTime,NULL);

	while(nTimePassed < IOS_TIMEOUT && IOS_TX_BYTE(nIOS,nCPU,nModulo,0)){
		gettimeofday(&tvActualTime,NULL);

		nTimePassed=(tvActualTime.tv_sec-tvStartTime.tv_sec)*1000;     /* secondi */
		nTimePassed+=(tvActualTime.tv_usec-tvStartTime.tv_usec)/1000;   /* milli secondi */
#ifdef TRACE_TIME
		trace_out_vstr(1, "TimePassed = %6d",nTimePassed);
#endif
		usleep(1000*Cfg.nDelay);
	}
	if(nTimePassed > IOS_TIMEOUT){
		char szMsg[128];

#ifdef TRACE
		trace_out_vstr_date(1, "Errore di TIMEOUT su IOS [%d.%d.%d]",nIOS,nCPU,nModulo);
#endif
		sprintf(szMsg, "Errore di TIMEOUT su IOS [%d.%d.%d]\n",nIOS,nCPU,nModulo);
		SendMessage(ProcList,PROC_MAIN, PROC_IOS, DISPLAY_MSG, szMsg);
	}
	/*
	* rm 27-02-2001 : inserito ritardo per mancata accensione luce
	*                 rif. (Sarti)
	*/
#ifdef SARTI
	usleep(Cfg.nDelay);
#endif

#endif
}

void WaitPLCVar(int nPLCVar,int nValue)
{
	struct timeval tvActualTime;
	struct timeval tvStartTime;
	int nTimePassed=0;    /* msecs */

	gettimeofday(&tvStartTime,NULL);

	/* attualizzo le var plc */
	if(Cfg.nPLC){
		if(SendCmd(CMD_MEMCOPY,(char *)NULL,TRUE)){
			GetMemCopyInfo(szRcvBuffer);
		}
	}

	/* attendo la risposta al comando plcvar */
	while(nTimePassed < PLC_TIMEOUT && VarValue(nPLCVar)!=nValue){
		gettimeofday(&tvActualTime,NULL);

		nTimePassed+=(tvActualTime.tv_sec-tvStartTime.tv_sec)*1000;     /* secondi */
		nTimePassed+=(tvActualTime.tv_usec-tvStartTime.tv_usec)/1000;   /* milli secondi */
#ifdef TRACE_TIME
		trace_out_vstr(1, "TimePassed = %6d",nTimePassed);
#endif
		usleep(1000*Cfg.nDelay);
		/* attualizzo le var plc */
		if(Cfg.nPLC){
			if(SendCmd(CMD_MEMCOPY,(char *)NULL,TRUE)){
				GetMemCopyInfo(szRcvBuffer);
			}
		}
	}
	if(nTimePassed > PLC_TIMEOUT){
		char szMsg[128];

#ifdef TRACE
		trace_out_vstr_date(1, "Errore di TIMEOUT su fronte salita PLC [%s]",PLCVars[nPLCVar].szName);
#endif
		sprintf(szMsg, "Errore di TIMEOUT su fronte salita PLC [%s]",PLCVars[nPLCVar].szName);
		SendMessage(ProcList,PROC_MAIN, PROC_IOS, DISPLAY_MSG, szMsg);
	}
}


void GetVarsIndex(void)
{
	char szBuffer[128];

	sprintf(szBuffer,"AMATRIX"); if((nAMATRIX  = GetVarIndex(szBuffer))==-1){ trace_out_vstr_date(1, "Variabile %s assente !\n",szBuffer); }
	sprintf(szBuffer,"ARIGA");   if((nARIGA    = GetVarIndex(szBuffer))==-1){ trace_out_vstr_date(1, "Variabile %s assente !\n",szBuffer); }
	sprintf(szBuffer,"ACOLON");  if((nACOLON   = GetVarIndex(szBuffer))==-1){ trace_out_vstr_date(1, "Variabile %s assente !\n",szBuffer); }
	sprintf(szBuffer,"NMATRIX"); if((nNMATRIX  = GetVarIndex(szBuffer))==-1){ trace_out_vstr_date(1, "Variabile %s assente !\n",szBuffer); }
	sprintf(szBuffer,"NRIGA");   if((nNRIGA    = GetVarIndex(szBuffer))==-1){ trace_out_vstr_date(1, "Variabile %s assente !\n",szBuffer); }
	sprintf(szBuffer,"NCOLON");  if((nNCOLON   = GetVarIndex(szBuffer))==-1){ trace_out_vstr_date(1, "Variabile %s assente !\n",szBuffer); }
	sprintf(szBuffer,"ACCEN");   if((nACCEN    = GetVarIndex(szBuffer))==-1){ trace_out_vstr_date(1, "Variabile %s assente !\n",szBuffer); }
	sprintf(szBuffer,"RACCEN");  if((nRACCEN   = GetVarIndex(szBuffer))==-1){ trace_out_vstr_date(1, "Variabile %s assente !\n",szBuffer); }
	sprintf(szBuffer,"ANNUL");   if((nANNUL    = GetVarIndex(szBuffer))==-1){ trace_out_vstr_date(1, "Variabile %s assente !\n",szBuffer); }
	sprintf(szBuffer,"RANNUL");  if((nRANNUL   = GetVarIndex(szBuffer))==-1){ trace_out_vstr_date(1, "Variabile %s assente !\n",szBuffer); }
	sprintf(szBuffer,"SMATRIX"); if((nSMATRIX  = GetVarIndex(szBuffer))==-1){ trace_out_vstr_date(1, "Variabile %s assente !\n",szBuffer); }
	sprintf(szBuffer,"SRIGA");   if((nSRIGA    = GetVarIndex(szBuffer))==-1){ trace_out_vstr_date(1, "Variabile %s assente !\n",szBuffer); }
	sprintf(szBuffer,"SCOLON");  if((nSCOLON   = GetVarIndex(szBuffer))==-1){ trace_out_vstr_date(1, "Variabile %s assente !\n",szBuffer); }
	sprintf(szBuffer,"SPEGNI");  if((nSPEGNI   = GetVarIndex(szBuffer))==-1){ trace_out_vstr_date(1, "Variabile %s assente !\n",szBuffer); }
	sprintf(szBuffer,"RSPEGNI"); if((nRSPEGNI  = GetVarIndex(szBuffer))==-1){ trace_out_vstr_date(1, "Variabile %s assente !\n",szBuffer); }
	sprintf(szBuffer,"RESETL");  if((nRESETL   = GetVarIndex(szBuffer))==-1){ trace_out_vstr_date(1, "Variabile %s assente !\n",szBuffer); }
	sprintf(szBuffer,"RRESETL"); if((nRRESETL  = GetVarIndex(szBuffer))==-1){ trace_out_vstr_date(1, "Variabile %s assente !\n",szBuffer); }
	sprintf(szBuffer,"CONGELA"); if((nCONGELA  = GetVarIndex(szBuffer))==-1){ trace_out_vstr_date(1, "Variabile %s assente !\n",szBuffer); }
}

/* 
* main() 
*/
int main(int argc, char **argv)
{
	int nRC;
	int nIndex;
	int nCPUIndex;
	int nIOS,nCPU,nModule,nRowIndex,nColIndex;
	int nBlinkState;
	char szSendBuffer[128];
	char szMsg[128];
	int nOpt;
	int nPLCError;

	while ((nOpt = getopt(argc, argv, "p:")) != -1) {
		switch(nOpt){
			case 'p':	/* Process ID */
				nPID=atoi(optarg);
			break;
		}
	}
	if(nPID==0){
		nPID=PROC_IOS;
	}

	/*
	* Lettura della configurazione corrente
	*/
	ReadConfiguration(__configuration_file__);

#ifdef TRACE
	/* Apro il file trace */
	sprintf(szBufTrace,"%s/%s%s",Cfg.szPathTrace,__module_name__,TRACE_FILE_SUFFIX);
	open_trace(__module_name__, szBufTrace, PICKING_TRACE_FILE_MAXSIZE);

	trace_out_vstr(1,"Module Name     : %s",__module_name__);
	trace_out_vstr(1,"Version         : %s",__version__);
	trace_out_vstr(1,"Customer Name   : %s",__customer__);
	trace_out_vstr(1,"RCSID String    : %s",rcsid);
	trace_out_vstr(1,"Simulazione     : %d",Cfg.nSimulazione);

	trace_out_vstr_date(1,"Started");
#endif

	/* Apro la coda messaggi principale */
	if((ProcList[PROC_MAIN].nQNumber = OpenMsgQ(ProcList[PROC_MAIN].nQKey))<0){
#ifdef TRACE
		trace_out_vstr(1, "%s : Apertura coda messaggi principale fallita", __module_name__);
#endif
	}
	/* Apro la coda messaggi processo Settori */
	if((ProcList[PROC_SETTORI].nQNumber = OpenMsgQ(ProcList[PROC_SETTORI].nQKey))<0){
#ifdef TRACE
		trace_out_vstr(1, "%s : Apertura coda messaggi settori fallita", __module_name__);
#endif
	}
	/* Apro la coda messaggi locale */
	if((ProcList[PROC_IOS].nQNumber = OpenMsgQ(ProcList[PROC_IOS].nQKey))<0){
#ifdef TRACE
		trace_out_vstr(1, "%s : Apertura coda messaggi locale fallita", __module_name__);
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
			sprintf(szMsg,"Errore in apertura Scheda IOS Numero [%2d]\n",nIndex);
			SendMessage(ProcList,PROC_MAIN, PROC_IOS, DISPLAY_MSG, szMsg);
			exit(1);
		} else {
#ifdef TRACE
			trace_out_vstr(1, "Scheda IOS Numero %2d - Versione : %c%c%c%c", 
				nIndex,
				Cfg.IOSInfo[nIndex].base[0x225],
				Cfg.IOSInfo[nIndex].base[0x226],
				Cfg.IOSInfo[nIndex].base[0x227],
				Cfg.IOSInfo[nIndex].base[0x228]);
#endif
			for (nCPUIndex = 0; nCPUIndex < MAX_IOS_CPU; nCPUIndex++) {
				if (Cfg.IOSInfo[nIndex].cpu[nCPUIndex]) {
					/* abilita il riaggancio */
					Cfg.IOSInfo[nIndex].cpu[nCPUIndex][IOS_HANDSHAKE]=(BYTE)0;
					sleep(1);
					/* disabilita il riaggancio */
					Cfg.IOSInfo[nIndex].cpu[nCPUIndex][IOS_HANDSHAKE]=(BYTE)0x55;
				}
			}
		}
	}
#endif



	/*
	* Apertura del canale di comunicazione con il PLC server (QPLC)
	*/
	if(Cfg.nPLC){
		if((nPLCError=PLCConnect(Cfg.szPLCHost))==FALSE){
			Cfg.nPLC=0;
#ifdef TRACE
			trace_out_vstr(1, "%s : Connessione al PLC Fallita : errore [%d]", __module_name__,nPLCError);
#endif
			sprintf(szMsg, "Connessione al PLC Fallita : errore [%d]", nPLCError);
			SendMessage(ProcList,PROC_MAIN, PROC_IOS, DISPLAY_MSG, szMsg);
		}
	}

	/*
	* Memorizzo gli indici nel vettore interno
	*/

	if(Cfg.nPLC){
		GetVarsIndex();

		SetVarValue(nAMATRIX,FALSE);
		SetVarValue(nARIGA,FALSE);
		SetVarValue(nACOLON,FALSE);
		SetVarValue(nACCEN,FALSE);
		SetVarValue(nANNUL,FALSE);
		SetVarValue(nRSPEGNI,FALSE);
		SetVarValue(nRESETL,FALSE);
	}


	/*
	* avverto MAIN dello start
	*/
	SendMessage(ProcList,PROC_MAIN, PROC_IOS, PROGRAM_STARTED, NULL);


	/*
	* gestione program loop
	*/
	for(;;){
		/*
		* Richiesta informazioni sulle variabili registrate per il DEBUG
		*/
		if(Cfg.nPLC){
			if(SendCmd(CMD_MEMCOPY,(char *)NULL,TRUE)){
				GetMemCopyInfo(szRcvBuffer);
			}
		}

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

#ifdef TRACE
							trace_out_vstr_date(1,"IOS_PKL_SET_RC [IOS:%d,CPU:%d,M:%2d,R:%2d,C:%2d]", nIOS,nCPU,nModule,nRowIndex,nColIndex);
#endif
							if(Cfg.nPLC){
								SetVarValue(nAMATRIX,nModule);
								SetVarValue(nARIGA,nRowIndex);
								SetVarValue(nACOLON,nColIndex);

								SetVarValue(nACCEN,TRUE);
								WaitPLCVar(nRACCEN,TRUE);
								SetVarValue(nACCEN,FALSE);
							}
							/*
							* se sono in simulazione restituisco la pressione tasto
							*/
							if(Cfg.nSimulazione){
								usleep(1000*Cfg.nDelaySimulazione);

#ifdef TRACE
								trace_out_vstr_date(1,"SIM: PKL_BUTTON_PRESSED [IOS:%d,CPU:%d,M:%2d,R:%2d,C:%2d]", nIOS,nCPU,nModule,nRowIndex,nColIndex);
#endif
								sprintf(szSendBuffer,"%d,%d,%d,%d,%d",nIOS,nCPU,nModule,nRowIndex,nColIndex);

								SendSpecificMsg(ProcList[PROC_SETTORI].nQNumber, 
									ProcList[PROC_SETTORI].nGId, 
									ProcList[PROC_IOS].nGId, 
									IOS_PKL_BUTTON_PRESSED, szSendBuffer);

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
							if(Cfg.nPLC){
								SetVarValue(nNMATRIX,nModule);
								SetVarValue(nNRIGA,nRowIndex);
								SetVarValue(nNCOLON,nColIndex);

								SetVarValue(nANNUL,TRUE);
								WaitPLCVar(nRANNUL,TRUE);
								SetVarValue(nANNUL,FALSE);
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
							Cfg.nCongela=TRUE;
							if(Cfg.nPLC){
								SetVarValue(nCONGELA,TRUE);
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
							Cfg.nCongela=FALSE;
							if(Cfg.nPLC){
								SetVarValue(nCONGELA,FALSE);
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
							if(Cfg.nPLC){
								SetVarValue(nRESETL,TRUE);
								WaitPLCVar(nRRESETL,TRUE);
								SetVarValue(nRESETL,FALSE);
							}
						break;
						case IOS_PKL_TEST:
							/* 
							* Parametri : Scheda IOS,CPU,Modulo
							*/
							sscanf(szText,"%d,%d,%d",&nIOS,&nCPU,&nModule);
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

#ifdef HAS_IOS_CNI
							if(nIOS<Cfg.nIOS){
								IOS_TX_BYTE(nIOS,nCPU,nModule,1)=(BYTE)nDigit;           /* digit */
								IOS_TX_BYTE(nIOS,nCPU,nModule,2)=(BYTE)nDPYByte[0];      /* ascii */
								IOS_TX_BYTE(nIOS,nCPU,nModule,0)=(BYTE)nComando;         /* comando */
							}
#endif
							WaitIOS(nIOS,nCPU,nModule);
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

#ifdef HAS_IOS_CNI
								if(nIOS<Cfg.nIOS){
									IOS_TX_BYTE(nIOS,nCPU,nModule,1)=(BYTE)((nRowIndex*4)+nIndex*2); /* digit */
									IOS_TX_BYTE(nIOS,nCPU,nModule,2)=(BYTE)nDPYByte[nIndex*2];       /* ascii */
									IOS_TX_BYTE(nIOS,nCPU,nModule,3)=(BYTE)nDPYByte[nIndex*2+1];     /* ascii */
									IOS_TX_BYTE(nIOS,nCPU,nModule,0)=(BYTE)nComando;                 /* comando */
								}
#endif
								WaitIOS(nIOS,nCPU,nModule);

							}
						}
						break;

						case IOS_DPY_BCD:
						{
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

#ifdef HAS_IOS_CNI
							if(nIOS<Cfg.nIOS){
								IOS_TX_BYTE(nIOS,nCPU,nModule,1)=(BYTE)nDPYByte[0];	/* Digit 0-1 o 4-5 */
								IOS_TX_BYTE(nIOS,nCPU,nModule,2)=(BYTE)nDPYByte[1];	/* Digit 2-3 o 6-7 */
								IOS_TX_BYTE(nIOS,nCPU,nModule,3)=(BYTE)nDPYByte[2];	/* Flags per blink e '.' */
								IOS_TX_BYTE(nIOS,nCPU,nModule,0)=(BYTE)nComando;                 /* comando */
							}
#endif
							
							WaitIOS(nIOS,nCPU,nModule);

						}
						break;

					}
				break;
			}
		}

		/* controllo tasti premuti */
		if(VarValue(nSPEGNI)){
			nIOS=0;
			nCPU=0;
			nModule=VarValue(nSMATRIX);
			nRowIndex=VarValue(nSRIGA);
			nColIndex=VarValue(nSCOLON);

			sprintf(szSendBuffer,"%d,%d,%d,%d,%d",nIOS,nCPU,nModule,nRowIndex,nColIndex);

			SendSpecificMsg(ProcList[PROC_SETTORI].nQNumber, 
				ProcList[PROC_SETTORI].nGId, 
				ProcList[PROC_IOS].nGId, 
				IOS_PKL_BUTTON_PRESSED, szSendBuffer);

			/* rispondo a PLC  e attendo */
			SetVarValue(nRSPEGNI,TRUE);
			WaitPLCVar(nSPEGNI,FALSE);
			SetVarValue(nRSPEGNI,FALSE);
		}



		usleep(1000*Cfg.nDelay);
	}	 /* fine ciclo */
	SafeIntFunc();
	return (0);
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
	SendMessage(ProcList,PROC_MAIN, PROC_IOS, PROGRAM_STOPPED, NULL);

	exit(1);
} /* Fine SafeIntFunc */
