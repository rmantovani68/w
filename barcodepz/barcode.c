/*
* barcode.c : gestione Barcode Rader
* 
* Progetto Mondadori Picking
*
* Autore : Roberto Mantovani
*
* Copyright CNI srl 1995-98
*/
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>

#include <pmx_msq.h>
#include <msq_lib.h>
#ifdef TRACE
	#include <trace.h>
#endif

#include <ca_file.h>
#include <proc_list.h>

#include "barcode.h"

CFGSTRUCT	Cfg;                /* struttura della configurazione */
PROCESSO ProcList[NUM_PROC];	/* processi componenti il progetto */

char szBufTrace[80];
char szBarcode[80];
char szRcvBuffer[256];
char *pszNomeModulo;
char szTimeBuffer[80];
char szDateBuffer[80];

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
	char szBuffer[128];
	char  *pPtr;
	char  *szSeparator=",\t\n";
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

	/*
	* Estraggo i parametri relativi ai processi 
	*/
	for (nIndex=0; nIndex<NUM_PROC; nIndex++) {
		sprintf(szProcIndex,"Proc_%02d",nIndex);
		xncGetFileString("procinfo",szProcIndex, "", szProcBuffer, 80,szCniCfg,NULL);
		sscanf(szProcBuffer, "%[^,],%d,%d",
				ProcList[nIndex].szProcName,
				&ProcList[nIndex].nQKey,
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
	strcpy(szParagraph,ProcList[PROC_BARCODE].szProcName);
	/*
	* nome della porta seriale utilizzata per il colloquio con lo scanner
	*/
	xncGetFileString(szParagraph,"CommDevice","/dev/com2", Cfg.szCommDevice, 80,szCniCfg,NULL);

	/*
	* Parametri di comunicazione utilizzati per il colloquio con lo scanner
	*/
	xncGetFileString(szParagraph,"CommParams","9600,n,8,1", szBuffer, 80,szCniCfg,NULL);

	/* Baudrate */
	if((pPtr=strtok(szBuffer,szSeparator))!=NULL){
		Cfg.nBaudrate=atoi(pPtr);
	}
	/* Parity */
	if((pPtr=strtok(NULL,szSeparator))!=NULL){
		switch(*pPtr){
			default:
			case 'N': 
			case 'n':
				Cfg.nParity=0; /* NONE */
			break;
			case 'O': 
			case 'o':
				Cfg.nParity=1; /* ODD */
			break;
			case 'E': 
			case 'e':
				Cfg.nParity=2; /* EVEN */
			break;
		}
	};
	/* Data Bits */
	if((pPtr=strtok(NULL,szSeparator))!=NULL){
		Cfg.nDataBits=atoi(pPtr);
	};
	/* Stop Bits */
	if((pPtr=strtok(NULL,szSeparator))!=NULL){
		Cfg.nStopBits=atoi(pPtr);
	};
}
/* main() */
void main(int argc, char **argv)
{
	pszNomeModulo = argv[0];

#ifdef TRACE
	/* Apro il file trace */
	sprintf(szBufTrace, "%s%s", pszNomeModulo, TRACE_FILE_SUFFIX);

	open_trace(pszNomeModulo, szBufTrace, TRACE_FILE_MAXSIZE);

	trace_out_vstr_date(1,"Started");
#endif


	ReadConfiguration();

	/* Apro la coda messaggi principale */
	if((ProcList[PROC_PESO].nQNumber = OpenMsgQ(ProcList[PROC_PESO].nQKey))<0){
#ifdef TRACE
		trace_out_vstr(1, "%s : Apertura coda messaggi principale fallita", pszNomeModulo);
#endif
	}

	/* Predispongo l'utilizzo dell'uscita di sicurezza */
	signal(SIGTERM, SafeIntFunc);
	signal(SIGINT,SafeIntFunc);
	signal(SIGQUIT,SafeIntFunc);
	signal(SIGILL,SafeIntFunc);
	signal(SIGKILL,SafeIntFunc);

#ifdef TRACE
		trace_out_vstr(1, "%s : Parametri di comunicazione : %s : %d,%d,%d,%d", pszNomeModulo,
			Cfg.szCommDevice,Cfg.nBaudrate,Cfg.nParity,Cfg.nDataBits,Cfg.nStopBits);
#endif
	/* Apertura porta seriale */
	if (InitSer(Cfg.szCommDevice,&Cfg.nPortFD,Cfg.nBaudrate,Cfg.nDataBits,Cfg.nParity,Cfg.nStopBits,O_RDWR)) {
#ifdef TRACE
		trace_out_vstr(1, "%s : Apertura porta '%s' fallita", pszNomeModulo,Cfg.szCommDevice);
#endif
	}

	RxLoop();
}



/*
* funzione di ricezione
* del barcode e spedizione messaggio di barcode ricevuto al main process
*/
void RxLoop(void)
{
	int	nMsgLung;
	int	nRitorno;
	int	nRC;
	int	nMsgList[MAX_NUM_MESSAGES];
	int nNumMsg;

	for (;;) {
		nMsgList[0] = BCRD_RCV_BARCODE;
		nMsgList[1] = BCRD_RCV_NOREAD;
		/*
		* rm 24-09-1998 : eliminato il no read (lo gestisco nella lettura normale)
		nNumMsg = 2;
		*/
		nNumMsg = 1;
		nRitorno = RecSerMsg(Cfg.nPortFD, szRcvBuffer, msgRcv, nMsgList, &nNumMsg, &nMsgLung, 0, TIMEOUT_WITHIN);
	
		/* Controllo il messaggio ricevuto */
		switch(nRitorno){
			case BCRD_RCV_BARCODE:
				/*
				* barcode : <STX> + dato + <CR> + <LF>
				* dato : barcode letto o <CAN>
				*/
				strncpy(szBarcode,szRcvBuffer+1,nMsgLung-2);
				szBarcode[nMsgLung-3]='\0';
				if(szBarcode[0]==CAN_CHAR){
#ifdef TRACE
					trace_out_vstr_date(1,"BCRD: Ricevuto NOREAD");
#endif
					SendMessage(PROC_PESO, PROC_BARCODE,  BCRD_RX_NOREAD, NULL);
				} else {
#ifdef TRACE
					trace_out_vstr_date(1,"BCRD: Ricevuto Barcode :[%s]",szBarcode);
#endif
					SendMessage(PROC_PESO, PROC_BARCODE,  BCRD_RX_BARCODE, szBarcode);
				}
			break;

			case BCRD_RCV_NOREAD:
#ifdef TRACE
				trace_out_vstr_date(1,"BCRD: Ricevuto NOREAD");
#endif
				SendMessage(PROC_PESO, PROC_BARCODE,  BCRD_RX_NOREAD, NULL);
			break;

			default:
#ifdef TRACE
				trace_out_vstr(1,"BCRD: Errore numero %d", nRitorno);
#endif
				/*
				* SendMessage(PROC_PESO, PROC_BARCODE,  BCRD_RX_ERROR, NULL);
				*/
			break;

		}
	}
} /* Fine RxLoop*/



/*
* Funzione SafeIntFunc:
* uscita di sicurezza
*/
void SafeIntFunc()
{
	CloseSer();
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
