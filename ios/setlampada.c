/*
* setlampada.c : Gestione luci - versione PLC
* 
* Progetto Easy Picking 3.0 
*
* Autore : Roberto Mantovani
*
* Copyright A&L srl 2006-2009
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

#define SARTI

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

int nLMATRIX ;
int nLRIGA   ;
int nLCOLON  ;
int nLPROT  ;
int nRLPROT ;

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
	usleep(20000);
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
		nTimePassed+=(tvActualTime.tv_usec-tvStartTime.tv_usec)/1000;   /* micro secondi */
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
	sprintf(szBuffer,"LMATRIX"); if((nLMATRIX  = GetVarIndex(szBuffer))==-1){ trace_out_vstr_date(1, "Variabile %s assente !\n",szBuffer); }
	sprintf(szBuffer,"LRIGA");   if((nLRIGA    = GetVarIndex(szBuffer))==-1){ trace_out_vstr_date(1, "Variabile %s assente !\n",szBuffer); }
	sprintf(szBuffer,"LCOLON");  if((nLCOLON   = GetVarIndex(szBuffer))==-1){ trace_out_vstr_date(1, "Variabile %s assente !\n",szBuffer); }
	sprintf(szBuffer,"LPROT");   if((nLPROT    = GetVarIndex(szBuffer))==-1){ trace_out_vstr_date(1, "Variabile %s assente !\n",szBuffer); }
	sprintf(szBuffer,"RLPROT");  if((nRLPROT   = GetVarIndex(szBuffer))==-1){ trace_out_vstr_date(1, "Variabile %s assente !\n",szBuffer); }
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

	if (argc != 4) {
		printf("Utilizzo : %s <Modulo> <Riga> <Colonna>\n",argv[0]);
		return;
	} else {
		nModule=atoi(argv[1]);
		nRowIndex=atoi(argv[2]);
		nColIndex=atoi(argv[3]);
	}

	/*
	* Lettura della configurazione corrente
	*/
	ReadConfiguration(__configuration_file__);

#ifdef TRACE
	/* Apro il file trace */
	sprintf(szBufTrace,"%s/%s%s",Cfg.szPathTrace,"setlampada",TRACE_FILE_SUFFIX);
	open_trace("setlampada", szBufTrace, PICKING_TRACE_FILE_MAXSIZE);

	trace_out_vstr(1,"Module Name     : %s","setlampada");

	trace_out_vstr_date(1,"Started");
#endif

	/* Predispongo l'utilizzo dell'uscita di sicurezza */
	signal(SIGTERM, SafeIntFunc);
	signal(SIGINT,SafeIntFunc);
	signal(SIGQUIT,SafeIntFunc);
	signal(SIGILL,SafeIntFunc);
	signal(SIGKILL,SafeIntFunc);



	/*
	* Apertura del canale di comunicazione con il PLC server (QPLC)
	*/
	if(Cfg.nPLC){
		if((nPLCError=PLCConnect(Cfg.szPLCHost))==FALSE){
			Cfg.nPLC=0;
		}
	}

	/*
	* Memorizzo gli indici nel vettore interno
	*/

	if(Cfg.nPLC){
		GetVarsIndex();
	}

	if(Cfg.nPLC){
		SetVarValue(nAMATRIX,nModule);
		SetVarValue(nARIGA,nRowIndex);
		SetVarValue(nACOLON,nColIndex);

		SetVarValue(nACCEN,TRUE);
		WaitPLCVar(nRACCEN,TRUE);
		SetVarValue(nACCEN,FALSE);
		WaitPLCVar(nRACCEN,FALSE);
	}

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


		/* controllo tasti premuti */
		if(VarValue(nSPEGNI) && VarValue(nRSPEGNI)==0){
			nIOS=0;
			nCPU=0;
			nModule=VarValue(nSMATRIX);
			nRowIndex=VarValue(nSRIGA);
			nColIndex=VarValue(nSCOLON);

			printf("PKL_BUTTON_PRESSED [IOS:%d,CPU:%d,M:%2d,R:%2d,C:%2d]", nIOS,nCPU,nModule,nRowIndex,nColIndex);

			/* rispondo a PLC  e attendo */
			SetVarValue(nRSPEGNI,TRUE);
			WaitPLCVar(nSPEGNI,FALSE);
			SetVarValue(nRSPEGNI,FALSE);
		}

		/* controllo lampada rotta */
		if(VarValue(nLPROT) && VarValue(nRLPROT)==0){
			nIOS=0;
			nCPU=0;
			nModule=VarValue(nLMATRIX);
			nRowIndex=VarValue(nLRIGA);
			nColIndex=VarValue(nLCOLON);

			printf("PKL_LIGHT_DAMAGED [IOS:%d,CPU:%d,M:%2d,R:%2d,C:%2d]", nIOS,nCPU,nModule,nRowIndex,nColIndex);

			/* rispondo a PLC  e attendo */
			SetVarValue(nRLPROT,TRUE);
			WaitPLCVar(nLPROT,FALSE);
			SetVarValue(nRLPROT,FALSE);
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
