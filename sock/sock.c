/*
* file sock.c 
* Gestione Comunicazione via Socket
* 
* Progetto Easy Picking 3.0
*
* Autore : Roberto Mantovani
*
* Copyright CNI srl 2000
*/
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

#include <pmx_msq.h>
#include <msq_lib.h>
#ifdef TRACE
	#include <trace.h>
#endif

#include <ca_file.h>
#include <proc_list.h>

#include "sockets.h"
#include "sock.h"

int nAllMsg;
int nCarIn;
MSGHDRINT InMsgStruct;
char szInMsg[INFO_LEN];
char szText[INFO_LEN];
char szBufTrace[80];
char *pszNomeModulo;
char szRcvBuffer[512];

char szTimeBuffer[80];
char szDateBuffer[80];


CFGSTRUCT	Cfg;                /* struttura per la configurazione */
PROCESSO ProcList[NUM_PROC];	/* processi componenti il progetto */
PTSCKTSRVR ptScktSrvr=(PTSCKTSRVR)NULL;   /* socket di ricezione */
int nMainProcess=-1;

/*
* comandi ricevibili dal socket server 
*/
char *pszSockCmds[]={
	"DISPLAY_MSG",
	"DISPLAY_MSGBOX",
	"UPDATE_ORDINE",
	"PRINT_PDF",
	"PRINT_RAC_COLLO",
	NULL,
};

/*
* ReadConfiguration()
* lettura della configurazione dal file cni.cfg
*/
void ReadConfiguration(BOOL bReadProcInfo)
{
	char szParagraph[128];
	char szCurrentDirectory[128];

	/*
	* leggo il file di configurazione dalla directory corrente
	*/
	getcwd(szCurrentDirectory,sizeof(szCurrentDirectory));
	
	strcpy(Cfg.szCniCfg,szCurrentDirectory);
	strcat(Cfg.szCniCfg,"/cni.cfg");

	if(bReadProcInfo){
		ReadProcInfo(Cfg.szCniCfg);
	}
	/*
	* lettura del file di configurazione
	*/
	strcpy(szParagraph,"General Settings");

	/*
	* parametri specifici dell'applicazione
	*/
	Cfg.nDelay=xncGetFileInt(ProcList[PROC_SOCK].szProcName,"Delay",100,Cfg.szCniCfg,NULL);
}

/*
* ReadProcInfo()
* Lettura dei parametri relativi ai processi 
*/
void ReadProcInfo(char *szFileConfig)
{
	char szProcIndex[128];
	char szProcBuffer[128];
	int nIndex;

	for (nIndex=0; nIndex<NUM_PROC; nIndex++) {
		sprintf(szProcIndex,"Proc_%02d",nIndex);
		xncGetFileString("procinfo",szProcIndex, "", szProcBuffer, 80,szFileConfig,NULL);
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
}


/* 
* main() 
*/
void main(int argc, char **argv)
{
	pszNomeModulo = argv[0];

	/*
	* Lettura della configurazione corrente
	*/
	ReadConfiguration(TRUE);

#ifdef TRACE
	/* Apro il file trace */
	sprintf(szBufTrace, "%s%s", pszNomeModulo, TRACE_FILE_SUFFIX);
	open_trace(pszNomeModulo, szBufTrace, TRACE_FILE_MAXSIZE);

	trace_out_vstr_date(1,"Started");
	trace_out_vstr(1,"Module Name     : %s",pszNomeModulo);
#endif

	/* Apro la coda messaggi principale */
	if((ProcList[PROC_MAIN].nQNumber = OpenMsgQ(ProcList[PROC_MAIN].nQKey))<0){
#ifdef TRACE
		trace_out_vstr_date(1, "Apertura coda messaggi [%d] fallita", ProcList[PROC_MAIN].szProcName,ProcList[PROC_MAIN].nQKey);
#endif
	}
	/* Apro la coda messaggi di printlabel */
	if((ProcList[PROC_PRINTLABEL].nQNumber = OpenMsgQ(ProcList[PROC_PRINTLABEL].nQKey))<0){
#ifdef TRACE
		trace_out_vstr_date(1, "Apertura coda messaggi [%d] fallita", ProcList[PROC_PRINTLABEL].szProcName,ProcList[PROC_MAIN].nQKey);
#endif
	}
	/* Apro la coda messaggi locale */
	if((ProcList[PROC_SOCK].nQNumber = OpenMsgQ(ProcList[PROC_SOCK].nQKey))<0){
#ifdef TRACE
		trace_out_vstr(1, "Apertura coda messaggi locale fallita");
#endif
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
	SendMessage(PROC_MAIN, PROC_SOCK,  PROGRAM_STARTED, NULL);

	/*
	* loop di ricezione messaggi
	*/
	for(;;){
		nAllMsg = 0;
		if((nCarIn = ReceiveMsgNoWait(ProcList[PROC_SOCK].nQNumber, &nAllMsg, szInMsg))>=0){
			UnpackStructMsg(szInMsg, &InMsgStruct, szText, nCarIn);

			switch(InMsgStruct.code){
				case SOCK_OPEN_SERVICE:
					nMainProcess=InMsgStruct.srce-1;
					if(ptScktSrvr==(PTSCKTSRVR)NULL){
						if((ptScktSrvr = xncScktSrvrOpen("", "tcp", atoi(szText)))==(PTSCKTSRVR)NULL){
#ifdef TRACE
							trace_out_vstr_date(1,"Error in open service : [%s]",szText);
#endif
						}
					} else {
#ifdef TRACE
						trace_out_vstr_date(1,"cannot open service : already open: [%s]",szText);
#endif
					}
				break;
				case SOCK_CLOSE_SERVICE:
					nMainProcess=-1;
					if(ptScktSrvr!=(PTSCKTSRVR)NULL){
						if((xncScktClose(&(ptScktSrvr->nSocket)))==FALSE){
#ifdef TRACE
							trace_out_vstr_date(1,"Error in close service : [%s]",szText);
#endif
						}
						free((void *) ptScktSrvr);
					}
				break;
			}
		}
		/*
		* Loop di ricezione messaggi da socket
		*/
		if(ptScktSrvr && ptScktSrvr->nSocket!=ID_SOCK_NO_FD){
			xncScktSrvrGest(ptScktSrvr, GestSocket, (void *)0, 0);
		}
		usleep(Cfg.nDelay*1000);
	}	 /* fine loop */
}

/*
* GestSocket()
* gestione dei messaggi che arrivano sul socket
*/
BOOL GestSocket(int *pnFd, void *ClientData, void *CallData)
{
	char szMessage[512];
	char szCommand[256];
	char szParams[512];
	char *szSeparator=",\r\n";
	char *pPtr;
	BOOL bRC;
	int nCmdIndex;


	if( *pnFd==ID_SOCK_NO_FD )
		return(False);

	strcpy(szMessage, "");
	bRC=xncScktRecv(pnFd, szMessage, sizeof(szMessage)-1, 0);


	if( bRC ){
		/* Comando */
		if((pPtr=strtok(szMessage,szSeparator))!=NULL){
			strcpy(szCommand,pPtr);
		}
		if(pPtr==NULL){
			return FALSE;
		}
		strcpy(szParams,pPtr+strlen(pPtr)+1);

		nCmdIndex=GetCmdIndex(szCommand);

#ifdef TRACE
				trace_out_vstr_date(1,"Arrivato [%s] [%s]",szCommand, szParams);
#endif

		switch(nCmdIndex){
			case CMD_DISPLAY_MSG:
			case CMD_DISPLAY_MSGBOX:
			case CMD_UPDATE_ORDINE:
				/*
				* spedisco messaggio al main process
				*/
				if(nMainProcess!=-1){
					SendMessage(nMainProcess, PROC_SOCK,  PROC_SOCK*20+nCmdIndex, szParams);
				} else {
					SendMessage(PROC_MAIN, PROC_SOCK,  PROC_SOCK*20+nCmdIndex, szParams);
				}
			break;
			case CMD_PRINT_PDF:
#ifdef TRACE
				trace_out_vstr_date(1,"Arrivato CMD_PRINT_PDF [%s]",szParams);
#endif
				/*
				* spedisco messaggio al main process
				*/
				SendMessage(PROC_PRINTLABEL,PROC_SOCK,PRINT_PDF,szParams);
			break;
			case CMD_PRINT_RAC_COLLO:
#ifdef TRACE
				trace_out_vstr_date(1,"Arrivato CMD_PRINT_RAC_COLLO [%s]",szParams);
#endif
				/*
				* spedisco messaggio al printlabel
				*/
				SendMessage(PROC_PRINTLABEL,PROC_SOCK,PRINT_RAC_COLLO,szParams);
			break;
			default:
#ifdef TRACE
				trace_out_vstr_date(1,"Error in socket cmd management [%s]",szMessage);
#endif
			break;
		}
	}
		
	return(TRUE);
}	/*	Fine GestSocket */

/*
* GetCmdIndex()
* ritorna il codice del comando passato
* in ingresso sotto forma di stringa
*/
int GetCmdIndex(char *szCmd)
{
	int nIndex=0;

	while(pszSockCmds[nIndex]){
		if(!strcmp(szCmd,pszSockCmds[nIndex])){
			break;
		}
		nIndex++;
	}
	return nIndex;
}

/*
* Funzione SafeIntFunc:
* uscita di sicurezza
*/
void SafeIntFunc()
{
#ifdef TRACE
	trace_out_vstr_date(1,"Stopped");
#endif
	if(ptScktSrvr!=(PTSCKTSRVR)NULL && ptScktSrvr->nSocket!=ID_SOCK_NO_FD){
		if((xncScktClose(&(ptScktSrvr->nSocket)))==FALSE){
#ifdef TRACE
			trace_out_vstr_date(1,"Error in close service : [%s]",szText);
#endif
		}
		free((void *) ptScktSrvr);
	}

	/*
	* avverto MAIN dello stop
	*/
	SendMessage(PROC_MAIN, PROC_SOCK,  PROGRAM_STOPPED, NULL);

	exit(1);
} /* Fine SafeIntFunc */

/*
* SendMessage()
* Spedizione messaggio con timeout e tentativi (NON BLOCCANTE)
*/
BOOL SendMessage(int nDest,int nSrce,int nMsgCode,char *szText)
{
	int nRC;

	if((nRC=SendSpecificMsgNoWait(ProcList[nDest].nQNumber, ProcList[nDest].nGId, ProcList[nSrce].nGId, nMsgCode, szText,200,5))!=0){
		trace_out_vstr(1,"Errore [%d] in spedizione Messaggio a [%s]", nRC,ProcList[nDest].szProcName);
		return FALSE;
	}
	return TRUE;
}

