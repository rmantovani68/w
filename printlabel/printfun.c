/*
* printfun.c : Stampa Etichette : funzioni di utilita'
* 
* Progetto Easy Picking 2.0 
*
* Autore : Roberto Mantovani
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
#include <dirent.h>

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

#include	<picking.h>

#include	"printlabel.h"
#include	"printstruct.h"
#include	"printext.h"
#include	"printfun.h"
#include	"linklist.h"



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
	/*
	* lettura del file di configurazione
	*/
	strcpy(szParagraph,"General Settings");

	/* SAP */
	Cfg.nSAP = xncGetFileInt(szParagraph, "SAP", 0, Cfg.szCniCfg, NULL);

	/* Advanced Invoicing */
	Cfg.nAdvancedInvoicing = xncGetFileInt(szParagraph, "AdvancedInvoicing", 0, Cfg.szCniCfg, NULL);

	Cfg.nDebugVersion=xncGetFileInt(szParagraph,"DebugVersion",0,Cfg.szCniCfg,NULL);
	xncGetFileString(szParagraph,"Language",    "ita",        Cfg.szLanguage,     80,Cfg.szCniCfg,NULL);
	xncGetFileString(szParagraph,"PathData",    "../data",    Cfg.szPathData,     80,Cfg.szCniCfg,NULL);
	xncGetFileString(szParagraph,"PathExport",  "../export",  Cfg.szPathExport,   80,Cfg.szCniCfg,NULL);
	xncGetFileString(szParagraph,"PathExe",     "./",         Cfg.szPathExe,      80,Cfg.szCniCfg,NULL);
	xncGetFileString(szParagraph,"PathStampe",  "../stampe",  Cfg.szPathStampe,   80,Cfg.szCniCfg,NULL);

	/*
	* lettura del file di configurazione del DataBase (PostgreSQL)
	*/
	strcpy(szParagraph,"DataBase Settings");

	xncGetFileString(szParagraph,"PGHost",    "localhost",Cfg.szPGHost,     80,Cfg.szCniCfg,NULL); 
	xncGetFileString(szParagraph,"PGPort",    "5432",     Cfg.szPGPort,     80,Cfg.szCniCfg,NULL); 
	xncGetFileString(szParagraph,"PGDataBase","picking",  Cfg.szPGDataBase, 80,Cfg.szCniCfg,NULL); 

	if(bReadProcInfo){
		ReadProcInfo(Cfg.szCniCfg);
	}

	ReadPrintFieldsInfo(Cfg.szCniCfg);

	/*
	* parametri specifici dell'applicazione
	*/
	strcpy(szParagraph,ProcList[PROC_PRINTLABEL].szProcName);

	xncGetFileString(szParagraph,"LabelBaseName", "labelbase.txt", Cfg.szLabelBaseName, 80,Cfg.szCniCfg,NULL);
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
* ReadPrintFieldsInfo()
*
* Printer Fields structure inizialization
*/
int ReadPrintFieldsInfo(char *szCfgFile)
{
	char szBuffer[256];
	char szParagraph[80];
	char szField[80];
	char *pPtr;
	char *szSeparator=" ,\t\n";
	int nIndex;

	strcpy(szParagraph,"Etichetta");
	nIndex=0;
	sprintf(szField,"Field_%02d",nIndex);
	while(xncGetFileString(szParagraph,szField,"",szBuffer,sizeof(szBuffer),szCfgFile,NULL)){
		if(strlen(szBuffer)){
			/* ID Campo */
			if((pPtr=strtok(szBuffer,szSeparator))!=NULL){
				strcpy(LabelFields[nIndex].szFieldID,pPtr);
			} else {return(-1);}
			/* Tipo Campo */
			if((pPtr=strtok(NULL,szSeparator))!=NULL){
				strcpy(LabelFields[nIndex].szFieldType,pPtr);
			} else {return(-1);}
			/* Tabella Campo */
			if((pPtr=strtok(NULL,szSeparator))!=NULL){
				strcpy(LabelFields[nIndex].szTableName,pPtr);
			} else {return(-1);}
			/* Nome Campo in Tabella */
			if((pPtr=strtok(NULL,szSeparator))!=NULL){
				strcpy(LabelFields[nIndex].szFieldName,pPtr);
			} else {return(-1);}
		}
		nIndex++;
		sprintf(szField,"Field_%02d",nIndex);
	}
	LabelFields[nIndex].szFieldName[0]='\0';

	return nIndex;
}

void ReadPrintersConfiguration(void)
{
	char szParagraph[128];

	/*
	* lettura del file di configurazione
	*/
	strcpy(szParagraph,"General Settings");

}


char *GetStatoCollo(char cStato,char *pszBuffer)
{
	switch(cStato){
		case COLLO_ELABORATO:
			strcpy(pszBuffer,"ELABORATO");
		break;
		case COLLO_PRELIEVO:
			strcpy(pszBuffer,"IN PRELIEVO");
		break;
		case COLLO_CHIUSO:
			strcpy(pszBuffer,"CHIUSO");
		break;
		case COLLO_EVASO:
			strcpy(pszBuffer,"EVASO");
		break;
		default:
			strcpy(pszBuffer,"NON DEFINITO");
		break;
	}
	return pszBuffer;
}

/*
* StampaOrdini(char *pszPrinterName, char *szSequence)
* Descrizione
*  Funzione che stampa tutte le etichette degli ordini che sono nello stato elaborato
*	Parametri di ingresso
*  -pszPrinterName: nome della stampante
*  -szSequence: sequenza del progressivo collo da incrementare
*/
BOOL StampaOrdini(char *pszPrinterName, char *szSequence)
{
	char szFileName[128];
	char szCommand[128];
	FILE *fpEtichette;
	PGresult *PGRes;
	int nOrdiniDaStampare;
	int nOrdineIndex;
	char szOrdKey[128];
	char szOrdProg[128];
	char szSQLCmd[1024];


#ifdef TRACE
	trace_out_vstr_date(1, "StampaOrdini(%s)",pszPrinterName);
#endif
	/*
	* Apertura del file per la stampa
	*/
	sprintf(szFileName,"%s/EtichetteDaStampare",Cfg.szPathData);
	if ((fpEtichette=fopen(szFileName,"w"))==(FILE *)NULL) {
		return FALSE;
	}

	PGRes=PGExecSQL(Cfg.nDebugVersion,"select ordkey,ordprog from ord_prod where opstato='%c';",ORDINE_SPEDITO);

	nOrdiniDaStampare=PQntuples(PGRes);

	for(nOrdineIndex=0; nOrdineIndex<nOrdiniDaStampare; nOrdineIndex++){
		strcpy(szOrdKey,PQgetvalue(PGRes,nOrdineIndex,0));
		strcpy(szOrdProg,PQgetvalue(PGRes,nOrdineIndex,1));
		sprintf(szSQLCmd,"select * from ric_ord,ord_prod,col_prod where ric_ord.ordprog=col_prod.ordprog and ord_prod.ordkey=col_prod.ordkey and col_prod.ordkey='%s' order by cpnmcol;",szOrdKey);
		StampaEtichette(szSQLCmd,fpEtichette,szSequence);

		/*
		sprintf(szSQLCmd,"select * from ordini_bertello o,colli_bertello c where c.ordprog=o.ordprog and o.ordprog='%s' order by cpnmcol;",szOrdProg);
		StampaEtichetteBertello(szSQLCmd,fpEtichette,szSequence);
		*/
	}
	PQclear(PGRes);

	fclose(fpEtichette);

	sprintf(szCommand,"lpr -P%s %s",pszPrinterName,szFileName);
	system(szCommand);

	return TRUE;
}

/*
* StampaOrdineBertello(char *pszOrdProg,char *pszPrinterName,BOOL bSendMsg,int nGID)
* Descrizione
*  Funzione che stampa le etichette amazon di tutti i colli dell'ordine bertello specificato
*	Parametri di ingresso
*  -pszOrdProg : progressivo ordine
*  -pszPrinterName: nome della stampante
*/
BOOL StampaOrdineBertello(char *pszOrdProg,char *pszPrinterName,BOOL bSendMsg,int nGID)
{
	char szFileName[128];
	char szCommand[128];
	FILE *fpEtichetta;
	char szSQLCmd[1024];
	BOOL bRetVal=TRUE;


#ifdef TRACE
	trace_out_vstr_date(1, "StampaOrdineBertello(%s,%s)",pszOrdProg,pszPrinterName);
#endif
	/*
	* Apertura del file per la stampa
	*/
	sprintf(szFileName,"%s/OB%s",Cfg.szPathData,pszOrdProg);
	if ((fpEtichetta=fopen(szFileName,"w"))==(FILE *)NULL) {
		return FALSE;
	}

	/* gestione amazon / bertello */
	sprintf(szSQLCmd,"select * from ric_ord r,ordini_bertello o,colli_bertello c where c.ordprog=o.ordprog and r.ordprog=o.ordprog and o.ordprog='%s' order by cpnmcol;",pszOrdProg);
	StampaEtichetteAmazon(szSQLCmd,fpEtichetta);

	fclose(fpEtichetta);
	if(Cfg.nDebugVersion<=2){
		sprintf(szCommand,"lpr -r -P%s %s",pszPrinterName,szFileName);
	} else {
		sprintf(szCommand,"lpr -P%s %s",pszPrinterName,szFileName);
	}
	system(szCommand);
	if(bRetVal){
		if(bSendMsg){
			SendMessage(nGID, PROC_PRINTLABEL, PRINTLABEL_RAC_STAMPATO, pszOrdProg);
		}
	} else {
		SendMessage(nGID, PROC_PRINTLABEL, PRINTLABEL_RAC_ERROR, pszOrdProg);
	}

	return bRetVal;
}


/*
* StampaOrdine(char *pszOrdKey,char *pszPrinterName,BOOL bSendMsg,int nGID)
* Descrizione
*  Funzione che stampa le etichette di tutti i colli dell'ordine specificato
*	Parametri di ingresso
*  -pszOrdKey : progressivo ordine
*  -pszPrinterName: nome della stampante
*  -szSequence: nome della sequenza del progressivo collo da incrementare
*/
BOOL StampaOrdine(char *pszOrdKey,char *pszPrinterName,BOOL bSendMsg,int nGID,char *szSequence)
{
	char szFileName[128];
	char szCommand[128];
	FILE *fpEtichetta;
	PGresult *PGRes;
	PGresult *PGResAmazon;
	int nOrdiniDaStampare;
	int nOrdineIndex;
	int nOrdini;
	int nIndex;
	char szOrdKey[128];
	char szOrdProg[128];
	int nIsolaOrdine;
	int nSottoOrdine;
	int nAmazon;
	int nIsola;
	char szSQLCmd[1024];
	BOOL bRetVal=TRUE;


#ifdef TRACE
	trace_out_vstr_date(1, "StampaOrdine(%s,%s)",pszOrdKey,pszPrinterName);
#endif
	/*
	* Apertura del file per la stampa
	*/
	sprintf(szFileName,"%s/O%s",Cfg.szPathData,pszOrdKey);
	if ((fpEtichetta=fopen(szFileName,"w"))==(FILE *)NULL) {
		return FALSE;
	}

	PGRes=PGExecSQL(Cfg.nDebugVersion,"select ordkey,r.ordprog,opcdisl,opcdsto,roflamz from ord_prod o,ric_ord r where o.ordkey='%s' and r.ordprog=o.ordprog;",pszOrdKey);

	nOrdiniDaStampare=PQntuples(PGRes);
	if(nOrdiniDaStampare){
		for(nOrdineIndex=0; nOrdineIndex<nOrdiniDaStampare; nOrdineIndex++){
			strcpy(szOrdKey,        PQgetvalue(PGRes,nOrdineIndex,0));
			strcpy(szOrdProg,       PQgetvalue(PGRes,nOrdineIndex,1));
			nIsolaOrdine     = atoi(PQgetvalue(PGRes,nOrdineIndex,2));
			nSottoOrdine     = atoi(PQgetvalue(PGRes,nOrdineIndex,3));
			nAmazon          = atoi(PQgetvalue(PGRes,nOrdineIndex,4));

			sprintf(szSQLCmd,"select * from ric_ord,ord_prod,col_prod where ric_ord.ordprog=col_prod.ordprog and ord_prod.ordkey=col_prod.ordkey and col_prod.ordkey='%s' order by cpnmcol;",szOrdKey);
			StampaEtichette(szSQLCmd,fpEtichetta,szSequence);

			if(nAmazon){
#ifdef TRACE
				trace_out_vstr_date(1, "Ordine %s AMAZON",pszOrdKey);
#endif
				sprintf(szSQLCmd,"select * from ric_ord,ord_prod,col_prod where ric_ord.ordprog=col_prod.ordprog and ord_prod.ordkey=col_prod.ordkey and col_prod.ordkey='%s' order by cpnmcol;",szOrdKey);
				StampaEtichetteAmazon(szSQLCmd,fpEtichetta);

				PGResAmazon=PGExecSQL(Cfg.nDebugVersion,"select min(opcdisl) from ord_prod o where ordprog='%s';",szOrdProg);
				if(PQntuples(PGRes)){
					/* 
					* se l'ordine e' il primo sottordine su isola 1 
					* oppure se e' il primo sottordine su isola 2 
					* e non ci sono sottordini su isola 1 allora stampo etichette amazon per bertello 
					*/
					nIsola=atoi(PQgetvalue(PGResAmazon,0,0));
					if(nSottoOrdine==1 && (nIsolaOrdine ==1 || nIsola==2)){
						/* gestione amazon / bertello */
						sprintf(szSQLCmd,"select * from ric_ord r,ordini_bertello o,colli_bertello c where c.ordprog=o.ordprog and r.ordprog=o.ordprog and o.ordprog='%s' order by cpnmcol;",szOrdProg);
						StampaEtichetteAmazon(szSQLCmd,fpEtichetta);
					}
				}
				PQclear(PGResAmazon);
			}


			/*
			* LG: devo stampare l'etichetta con "CONTIENE DOCUMENTI"
			* NON PIU', STAMPO DA MAIN
			*/
			//StampaEtichetta(szSQLCmd,fpEtichetta,"label_doc.txt");
		}
	} else {
		bRetVal=FALSE;
	}

	PQclear(PGRes);
	fclose(fpEtichetta);
	if(Cfg.nDebugVersion<=2){
		sprintf(szCommand,"lpr -r -P%s %s",pszPrinterName,szFileName);
	} else {
		sprintf(szCommand,"lpr -P%s %s",pszPrinterName,szFileName);
	}
	system(szCommand);
	if(bRetVal){
		if(bSendMsg){
			SendMessage(nGID, PROC_PRINTLABEL, PRINTLABEL_RAC_STAMPATO, pszOrdKey);
		}
	} else {
		SendMessage(nGID, PROC_PRINTLABEL, PRINTLABEL_RAC_ERROR, pszOrdKey);
	}

	return bRetVal;
}

/*
* StampaOrdineSenzaColliPdf(char *pszOrdKey,char *pszPrinterName,BOOL bSendMsg,int nGID)
* Descrizione
*  Funzione che stampa le etichette di tutti i colli dell'ordine specificato tranne quella col PDF
*	Parametri di ingresso
*  -pszOrdKey : progressivo ordine
*  -pszPrinterName: nome della stampante
*  -szSequence: nome della sequenza del progressivo collo da incrementare
*/
BOOL StampaOrdineSenzaColliPdf(char *pszOrdKey,char *pszPrinterName,BOOL bSendMsg,int nGID,char *szSequence)
{
	char szFileName[128];
	char szCommand[128];
	FILE *fpEtichetta;
	PGresult *PGRes;
	int nOrdiniDaStampare;
	int nOrdineIndex;
	char szOrdKey[128];
	char szOrdProg[128];
	char szSQLCmd[1024];
	BOOL bRetVal=TRUE;


#ifdef TRACE
	trace_out_vstr_date(1, "StampaOrdineIncompleto(%s,%s)",pszOrdKey,pszPrinterName);
#endif
	/*
	* Apertura del file per la stampa
	*/
	sprintf(szFileName,"%s/O%s",Cfg.szPathData,pszOrdKey);
	if ((fpEtichetta=fopen(szFileName,"w"))==(FILE *)NULL) {
		return FALSE;
	}

	PGRes=PGExecSQL(Cfg.nDebugVersion,"select ordkey,ordprog from ord_prod where ord_prod.ordkey='%s';",pszOrdKey);

	nOrdiniDaStampare=PQntuples(PGRes);
	if(nOrdiniDaStampare){
		for(nOrdineIndex=0; nOrdineIndex<nOrdiniDaStampare; nOrdineIndex++){
			strcpy(szOrdKey,PQgetvalue(PGRes,nOrdineIndex,0));
			strcpy(szOrdProg,PQgetvalue(PGRes,nOrdineIndex,1));
			sprintf(szSQLCmd,"select * from ric_ord,ord_prod,col_prod where ric_ord.ordprog=col_prod.ordprog and ord_prod.ordkey=col_prod.ordkey and col_prod.ordkey='%s' and col_prod.cpflpdf!='%c' order by cpnmcol;",szOrdKey,COLLO_CON_PDF);
			StampaEtichette(szSQLCmd,fpEtichetta,szSequence);

		}
	} else {
		bRetVal=FALSE;
	}

	PQclear(PGRes);
	fclose(fpEtichetta);
	if(Cfg.nDebugVersion<=2){
		sprintf(szCommand,"lpr -r -P%s %s",pszPrinterName,szFileName);
	} else {
		sprintf(szCommand,"lpr -P%s %s",pszPrinterName,szFileName);
	}
	system(szCommand);
	if(bRetVal){
		if(bSendMsg){
			SendMessage(nGID, PROC_PRINTLABEL, PRINTLABEL_RAC_NO_PDF_STAMPATO, pszOrdKey);
		}
	} else {
		SendMessage(nGID, PROC_PRINTLABEL, PRINTLABEL_RAC_ERROR, pszOrdKey);
	}

	return bRetVal;
}

/*
* StampaOrdineSoloColliPdf(char *pszOrdKey,char *pszPrinterName,BOOL bSendMsg,int nGID)
* Descrizione
*  Funzione che stampa le etichette di tutti i colli dell'ordine specificato che contengono PDF
*	Parametri di ingresso
*  -pszOrdKey : progressivo ordine
*  -pszPrinterName: nome della stampante
*  -szSequence: nome della sequenza del progressivo collo da incrementare
*
*  NB: Dovra' esssere stampata anche l'etichetta di "Contiene Documenti"
*/
BOOL StampaOrdineSoloColliPdf(char *pszOrdKey,char *pszPrinterName,BOOL bSendMsg,int nGID,char *szSequence)
{
	char szFileName[128];
	char szCommand[128];
	FILE *fpEtichetta;
	PGresult *PGRes;
	int nOrdiniDaStampare;
	int nOrdineIndex;
	char szOrdKey[128];
	char szOrdProg[128];
	char szSQLCmd[1024];
	BOOL bRetVal=TRUE;


#ifdef TRACE
	trace_out_vstr_date(1, "StampaOrdineIncompleto(%s,%s)",pszOrdKey,pszPrinterName);
#endif
	/*
	* Apertura del file per la stampa
	*/
	sprintf(szFileName,"%s/O%s",Cfg.szPathData,pszOrdKey);
	if ((fpEtichetta=fopen(szFileName,"w"))==(FILE *)NULL) {
		return FALSE;
	}

	PGRes=PGExecSQL(Cfg.nDebugVersion,"select ordkey,ordprog from ord_prod where ord_prod.ordkey='%s';",pszOrdKey);

	nOrdiniDaStampare=PQntuples(PGRes);
	if(nOrdiniDaStampare){
		for(nOrdineIndex=0; nOrdineIndex<nOrdiniDaStampare; nOrdineIndex++){
			strcpy(szOrdKey,PQgetvalue(PGRes,nOrdineIndex,0));
			strcpy(szOrdProg,PQgetvalue(PGRes,nOrdineIndex,1));
			sprintf(szSQLCmd,"select * from ric_ord,ord_prod,col_prod where ric_ord.ordprog=col_prod.ordprog and ord_prod.ordkey=col_prod.ordkey and col_prod.ordkey='%s' and col_prod.cpflpdf='%c' order by cpnmcol;",szOrdKey,COLLO_CON_PDF);
			StampaEtichette(szSQLCmd,fpEtichetta,szSequence);

			/*
			* LG: devo stampare l'etichetta con "CONTIENE DOCUMENTI"
			* NON PIU', STAMPO DA MAIN
			*/
			//StampaEtichetta(szSQLCmd,fpEtichetta,"label_doc.txt");
		}
	} else {
		bRetVal=FALSE;
	}

	PQclear(PGRes);
	fclose(fpEtichetta);
	if(Cfg.nDebugVersion<=2){
		sprintf(szCommand,"lpr -r -P%s %s",pszPrinterName,szFileName);
	} else {
		sprintf(szCommand,"lpr -P%s %s",pszPrinterName,szFileName);
	}
	system(szCommand);
	if(bRetVal){
		if(bSendMsg){
			SendMessage(nGID, PROC_PRINTLABEL, PRINTLABEL_RAC_PDF_STAMPATO, pszOrdKey);
		}
	} else {
		SendMessage(nGID, PROC_PRINTLABEL, PRINTLABEL_RAC_ERROR, pszOrdKey);
	}

	return bRetVal;
}



/*
* StampaCollo(char *pszOrdKey,int nCollo,char *pszPrinterName,char *szSequence)
* Descrizione
*  Funzione che stampa l'etichetta di un collo dell'ordine specificato
*	Parametri di ingresso
*  -pszOrdKey : Progressivo Ordine
*  -nCollo     : Numero Collo
*  -pszPrinterName: nome della stampante
*  -szSequence: nome della sequenza da incrementare
*/
BOOL StampaCollo(char *pszOrdKey,int nCollo,char *pszPrinterName,char *szSequence)
{
	char szFileName[128];
	char szCommand[128];
	FILE *fpEtichetta;
	PGresult *PGRes;
	int nOrdiniDaStampare;
	int nOrdineIndex;
	char szOrdKey[128];
	char szSQLCmd[1024];

#ifdef TRACE
	trace_out_vstr_date(1, "StampaCollo(%s,%d,%s)",pszOrdKey,nCollo,pszPrinterName);
#endif
	/*
	* Apertura del file per la stampa
	*/
	sprintf(szFileName,"%s/C%s%03d",Cfg.szPathData,pszOrdKey,nCollo);
	if ((fpEtichetta=fopen(szFileName,"w"))==(FILE *)NULL) {
		return FALSE;
	}
	PGRes=PGExecSQL(Cfg.nDebugVersion,"select ordkey from ord_prod where ord_prod.ordkey='%s';",pszOrdKey);

	nOrdiniDaStampare=PQntuples(PGRes);

	for(nOrdineIndex=0; nOrdineIndex<nOrdiniDaStampare; nOrdineIndex++){
		strcpy(szOrdKey,PQgetvalue(PGRes,nOrdineIndex,0));
		sprintf(szSQLCmd,"select * from ric_ord,ord_prod,col_prod where ric_ord.ordprog=col_prod.ordprog and ord_prod.ordkey=col_prod.ordkey and col_prod.ordkey='%s' and col_prod.cpnmcol=%d;",szOrdKey,nCollo);
		StampaEtichette(szSQLCmd,fpEtichetta,szSequence);

	}
	PQclear(PGRes);
	fclose(fpEtichetta);
	if(Cfg.nDebugVersion<=2){
		sprintf(szCommand,"lpr -r -P%s %s",pszPrinterName,szFileName);
	} else {
		sprintf(szCommand,"lpr -P%s %s",pszPrinterName,szFileName);
	}
	system(szCommand);


	return(TRUE);
}

/*
* StampaInizioElaborazione(char *pszOrdKey,char *pszPrinterName)
* Descrizione
*  Funzione che stampa l'etichetta di inizio elaborazione in corso
*	Parametri di ingresso
*  -pszOrdKey : Progressivo Ordine
*  -pszPrinterName: nome della stampante
*/
BOOL StampaInizioElaborazione(char *pszOrdKey,char *pszPrinterName)
{
	char szFileName[128];
	char szCommand[128];
	FILE *fpEtichetta;
	PGresult *PGRes;
	int nTuple;
	char szOrdKey[128];
	char szSQLCmd[1024];

#ifdef TRACE
	trace_out_vstr_date(1, "StampaInizioElaborazione(%s,%s)",pszOrdKey,pszPrinterName);
#endif
	/*
	* Apertura del file per la stampa
	*/
	sprintf(szFileName,"%s/IE%s",Cfg.szPathData,pszOrdKey);
	if ((fpEtichetta=fopen(szFileName,"w"))==(FILE *)NULL) {
		return FALSE;
	}
	PGRes=PGExecSQL(Cfg.nDebugVersion>1,"select ordkey from ord_prod where ord_prod.ordkey='%s';",pszOrdKey);

	nTuple=PQntuples(PGRes);

	if(nTuple){
		strcpy(szOrdKey,PQgetvalue(PGRes,0,0));
		sprintf(szSQLCmd,"select * from ric_ord,ord_prod,col_prod where ric_ord.ordprog=col_prod.ordprog and ord_prod.ordkey=col_prod.ordkey and col_prod.ordkey='%s';",szOrdKey);
		StampaEtichetta(szSQLCmd,fpEtichetta,"label_start.txt");
	}
	PQclear(PGRes);
	fclose(fpEtichetta);
	if(Cfg.nDebugVersion<=2){
		sprintf(szCommand,"lpr -r -P%s %s",pszPrinterName,szFileName);
	} else {
		sprintf(szCommand,"lpr -P%s %s",pszPrinterName,szFileName);
	}
	system(szCommand);


	return(TRUE);
}

/*
* StampaFineElaborazione(char *pszOrdKey,char *pszPrinterName)
* Descrizione
*  Funzione che stampa l'etichetta di fine elaborazione in corso
*	Parametri di ingresso
*  -pszOrdKey : Progressivo Ordine
*  -pszPrinterName: nome della stampante
*/
BOOL StampaFineElaborazione(char *pszOrdKey,char *pszPrinterName)
{
	char szFileName[128];
	char szCommand[128];
	FILE *fpEtichetta;
	PGresult *PGRes;
	int nTuple;
	char szOrdKey[128];
	char szSQLCmd[1024];

#ifdef TRACE
	trace_out_vstr_date(1, "StampaFineElaborazione(%s,%s)",pszOrdKey,pszPrinterName);
#endif
	/*
	* Apertura del file per la stampa
	*/
	sprintf(szFileName,"%s/FE%s",Cfg.szPathData,pszOrdKey);
	if ((fpEtichetta=fopen(szFileName,"w"))==(FILE *)NULL) {
		return FALSE;
	}
	PGRes=PGExecSQL(Cfg.nDebugVersion>1,"select ordkey from ord_prod where ord_prod.ordkey='%s';",pszOrdKey);

	nTuple=PQntuples(PGRes);

	if(nTuple){
		strcpy(szOrdKey,PQgetvalue(PGRes,0,0));
		sprintf(szSQLCmd,"select * from ric_ord,ord_prod,col_prod where ric_ord.ordprog=col_prod.ordprog and ord_prod.ordkey=col_prod.ordkey and col_prod.ordkey='%s';",szOrdKey);
		StampaEtichetta(szSQLCmd,fpEtichetta,"label_end.txt");
	}
	PQclear(PGRes);
	fclose(fpEtichetta);
	if(Cfg.nDebugVersion<=2){
		sprintf(szCommand,"lpr -r -P%s %s",pszPrinterName,szFileName);
	} else {
		sprintf(szCommand,"lpr -P%s %s",pszPrinterName,szFileName);
	}
	system(szCommand);

	return(TRUE);
}

/*
* StampaContieneDocumento(char *pszOrdKey,char *pszPrinterName)
* Descrizione
*  Funzione che stampa l'etichetta di contiene documento
*	Parametri di ingresso
*  -pszPrinterName: nome della stampante
*/
//BOOL StampaContieneDocumento(char *pszPrinterName)
BOOL StampaContieneDocumento(char *pszOrdKey, char *pszPrinterName)
{
	char szFileName[128];
	char szCommand[128];
	FILE *fpEtichetta;
	char szSQLCmd[1024];

#ifdef TRACE
	trace_out_vstr_date(1, "StampaContieneDocumento(%s)",pszPrinterName);
#endif
	/*
	* Apertura del file per la stampa
	*/
	//sprintf(szFileName,"%s/CD",Cfg.szPathData);
	sprintf(szFileName,"%s/CD_%s",Cfg.szPathData,pszOrdKey);
	if ((fpEtichetta=fopen(szFileName,"w"))==(FILE *)NULL) {
		return FALSE;
	}
	/*
	* Query per fare funzionare la funzione StampaEtichetta
	*/
	sprintf(szSQLCmd,"select true;");
	StampaEtichetta(szSQLCmd,fpEtichetta,"label_doc.txt");
	fclose(fpEtichetta);
	if(Cfg.nDebugVersion<=2){
		sprintf(szCommand,"lpr -r -P%s %s",pszPrinterName,szFileName);
	} else {
		sprintf(szCommand,"lpr -P%s %s",pszPrinterName,szFileName);
	}
	system(szCommand);


	return(TRUE);
}


/*
* Descrizione
* Stampa etichetta
* Parametri di ingresso
*  fp : file sul quale si produce la stampa
*  szSequence : nome della sequenza del progressivo collo da incrementare
*/
BOOL StampaEtichette(char *szSQLCmd,FILE *fp,char *szSequence)
{
	FILE *fpBase;
	char szBuffer[256];
	char szDestination[256];
	char *pPtr;
	int nIndex;
	int nColloIndex;
	int nFieldIndex;
	BOOL bFound;
	BOOL bSaltaRiga=FALSE;
	char szLabelBaseFileName[256];
	char szOrdKey[256];
	char szBarcodeCollo[256];
	int nCPPRGCL;
	int nCPNMCOL;
	int nORDKEY;
	int nCollo;
	PGresult *PGResCollo;
	PGresult *PGRes;

#ifdef TRACE
	trace_out_vstr_date(1, "StampaEtichette(%s)",szSQLCmd);
#endif

	PGRes=PGExecSQL(Cfg.nDebugVersion,szSQLCmd);
	if(PQntuples(PGRes)){

		if((nCPNMCOL=PQfnumber(PGRes,"cpnmcol"))==-1){
			PQclear(PGRes);
			return FALSE;
		}
		if((nCPPRGCL=PQfnumber(PGRes,"cpprgcl"))==-1){
			PQclear(PGRes);
			return FALSE;
		}
		if((nORDKEY=PQfnumber(PGRes,"ordkey"))==-1){
			PQclear(PGRes);
			return FALSE;
		}

		for(nColloIndex=0;nColloIndex<PQntuples(PGRes);nColloIndex++){
			nCollo=atoi(PQgetvalue(PGRes,nColloIndex,nCPNMCOL));
			strcpy(szOrdKey,PQgetvalue(PGRes,nColloIndex,nORDKEY));

			/*
			* Calcolo del progressivo collo
			*  se il contenuto numerico nell'archivio e' 0 si tratta di una stampa
			*   si incrementa il progressivo
			*   si aggiorna il numero di configurazione
			*   si assegna il progressivo nell'archivio
			*  altrimenti si tratta di una ristampa
			*   estrarre il progressivo dall'archivio
			*/
			/* 
			* calcolo il barcode vettore solo se presente il segnacollo 
			* progressivo - assegnato da analisi volumetrica 
			*/
			if (atoi(PQgetvalue(PGRes,nColloIndex,nCPPRGCL))!=0){
				int nProgCollo;

				nProgCollo=atoi(PQgetvalue(PGRes,nColloIndex,nCPPRGCL));

				sprintf(szBuffer,"%03d",atoi(PQgetvalue(PGRes,nColloIndex,PQfnumber(PGRes,"roflpar"))));strcpy(szBarcodeCollo,szBuffer);
				sprintf(szBuffer,"%03d",atoi(PQgetvalue(PGRes,nColloIndex,PQfnumber(PGRes,"roflarr"))));strcat(szBarcodeCollo,szBuffer);
				sprintf(szBuffer,"%02d",atoi(PQgetvalue(PGRes,nColloIndex,PQfnumber(PGRes,"ronrser"))));strcat(szBarcodeCollo,szBuffer);
				sprintf(szBuffer,"%07d",nProgCollo);strcat(szBarcodeCollo,szBuffer);
				sprintf(szBuffer,"%02d",atoi(PQgetvalue(PGRes,nColloIndex,PQfnumber(PGRes,"rozncon"))));strcat(szBarcodeCollo,szBuffer);
				sprintf(szBuffer,"%c",CalcCheck(szBarcodeCollo));strcat(szBarcodeCollo,szBuffer);


				PGResCollo=PGExecSQL(Cfg.nDebugVersion,"update col_prod set cpbcvet='%s' where col_prod.ordkey='%s' and cpnmcol=%d;", szBarcodeCollo,szOrdKey,nCollo);
				PQclear(PGResCollo);
			}

		}
	}
	PQclear(PGRes);

	/*
	* Rilancio la select dei colli da stampare
	*/
	PGRes=PGExecSQL(Cfg.nDebugVersion,szSQLCmd);
	if(PQntuples(PGRes)){
		for(nColloIndex=0;nColloIndex<PQntuples(PGRes);nColloIndex++){
			nCollo=atoi(PQgetvalue(PGRes,nColloIndex,nCPPRGCL));
			strcpy(szOrdKey,PQgetvalue(PGRes,nColloIndex,nORDKEY));

			sprintf(szLabelBaseFileName,"%s/%s",Cfg.szPathData,Cfg.szLabelBaseName);
			if((fpBase=fopen(szLabelBaseFileName,"r"))!=(FILE *)NULL){
				while(fgets(szBuffer,sizeof(szBuffer)-1,fpBase)){
					/*
					* prevedo piu' campi su una riga
					*/
					bFound=TRUE;
					while(bFound){
						/*
						* cerco l'ID campo all'interno della riga
						*/
						nIndex=0;
						bFound=FALSE;
						while(!bFound && strlen(LabelFields[nIndex].szFieldID)){
							if((pPtr=strstr(szBuffer,LabelFields[nIndex].szFieldID))!=NULL){
								bFound=TRUE;
								continue;
							}
							nIndex++;
						}
						strcpy(szDestination,szBuffer);
						if(bFound){
							szDestination[pPtr-szBuffer]='\0';
							if((nFieldIndex=PQfnumber(PGRes,LabelFields[nIndex].szFieldName))==-1){
								strcat(szDestination,"XXXX");
							} else {
								if(Cfg.nSAP){
									/*
									* modifica ordprog sap (da 6 a 10 cifre -> ma se ne visualizzano 7)
									*/
									/*
									 * rm 7-12-2013 : diventano 6 + 4 ... 
									 */
									if (nIndex==8){
										/* rm 10-05-2013 : da togliere per nuova gestione ordine a 10 cifre ... */
										strcat(szDestination,StrTrimAll(SubStr(PQgetvalue(PGRes,nColloIndex,nFieldIndex),4,6)));
										// strcat(szDestination,StrTrimAll(PQgetvalue(PGRes,nColloIndex,nFieldIndex)));
									} else if (nIndex==29){
										strcat(szDestination,StrTrimAll(SubStr(PQgetvalue(PGRes,nColloIndex,nFieldIndex),0,4)));
										// strcat(szDestination,StrTrimAll(PQgetvalue(PGRes,nColloIndex,nFieldIndex)));
									} else {
										strcat(szDestination,StrTrimAll(PQgetvalue(PGRes,nColloIndex,nFieldIndex)));
									}
								} else {
									strcat(szDestination,StrTrimAll(PQgetvalue(PGRes,nColloIndex,nFieldIndex)));
								}
							}

							strcat(szDestination,pPtr+strlen(LabelFields[nIndex].szFieldID));
							/*
							* riporto le modifiche 
							*/
							strcpy(szBuffer,szDestination);
						}
					}
					if(!bSaltaRiga){
						fputs(szDestination,fp);
					}
					bSaltaRiga=FALSE;
				}
				fclose(fpBase);
			}
		}
	}
	PQclear(PGRes);

	return TRUE;
}

/*
* Descrizione
* Stampa etichetta bertello
* Parametri di ingresso
*  fp : file sul quale si produce la stampa
*  szSequence : nome della sequenza del progressivo collo da incrementare
*/
BOOL StampaEtichetteAmazon(char *szSQLCmd,FILE *fp)
{
	FILE *fpBase;
	char szBuffer[256];
	char szDestination[256];
	char *pPtr;
	int nIndex;
	int nColloIndex;
	int nFieldIndex;
	BOOL bFound;
	BOOL bSaltaRiga=FALSE;
	char szLabelBaseFileName[256];
	char szOrdKey[256];
	char szBarcodeCollo[256];
	int nCPPRGCL;
	int nCPNMCOL;
	int nORDKEY;
	int nCollo;
	PGresult *PGResCollo;
	PGresult *PGRes;

#ifdef TRACE
	trace_out_vstr_date(1, "StampaEtichetteAmazon(%s)",szSQLCmd);
#endif

	/*
	* Rilancio la select dei colli da stampare
	*/
	PGRes=PGExecSQL(Cfg.nDebugVersion,szSQLCmd);
	if(PQntuples(PGRes)){
		for(nColloIndex=0;nColloIndex<PQntuples(PGRes);nColloIndex++){

			sprintf(szLabelBaseFileName,"%s/wella-amazon.txt",Cfg.szPathData);
			if((fpBase=fopen(szLabelBaseFileName,"r"))!=(FILE *)NULL){
				while(fgets(szBuffer,sizeof(szBuffer)-1,fpBase)){
					/*
					* prevedo piu' campi su una riga
					*/
					bFound=TRUE;
					while(bFound){
						/*
						* cerco l'ID campo all'interno della riga
						*/
						nIndex=0;
						bFound=FALSE;
						while(!bFound && strlen(LabelFields[nIndex].szFieldID)){
							if((pPtr=strstr(szBuffer,LabelFields[nIndex].szFieldID))!=NULL){
								bFound=TRUE;
								continue;
							}
							nIndex++;
						}
						strcpy(szDestination,szBuffer);
						if(bFound){
							szDestination[pPtr-szBuffer]='\0';
							if((nFieldIndex=PQfnumber(PGRes,LabelFields[nIndex].szFieldName))==-1){
								strcat(szDestination,"XXXX");
							} else {
								if(Cfg.nSAP){
									/*
									* modifica ordprog sap (da 6 a 10 cifre -> ma se ne visualizzano 7)
									*/
									/*
									 * rm 7-12-2013 : diventano 6 + 4 ... 
									 */
									if (nIndex==8){ 
										/* rm 10-05-2013 : da togliere per nuova gestione ordine a 10 cifre ... */
										strcat(szDestination,StrTrimAll(SubStr(PQgetvalue(PGRes,nColloIndex,nFieldIndex),4,6)));
										// strcat(szDestination,StrTrimAll(PQgetvalue(PGRes,nColloIndex,nFieldIndex)));
									} else if (nIndex==29){
										strcat(szDestination,StrTrimAll(SubStr(PQgetvalue(PGRes,nColloIndex,nFieldIndex),0,4)));
										// strcat(szDestination,StrTrimAll(PQgetvalue(PGRes,nColloIndex,nFieldIndex)));
									} else {
										strcat(szDestination,StrTrimAll(PQgetvalue(PGRes,nColloIndex,nFieldIndex)));
									}
								} else {
									strcat(szDestination,StrTrimAll(PQgetvalue(PGRes,nColloIndex,nFieldIndex)));
								}
							}

							strcat(szDestination,pPtr+strlen(LabelFields[nIndex].szFieldID));
							/*
							* riporto le modifiche 
							*/
							strcpy(szBuffer,szDestination);
						}
					}
					if(!bSaltaRiga){
						fputs(szDestination,fp);
					}
					bSaltaRiga=FALSE;
				}
				fclose(fpBase);
			}
		}
	}
	PQclear(PGRes);

	return TRUE;
}

/*
* Descrizione
* Stampa etichetta generica per Fine e Inizio Elaborazione
* Parametri di ingresso
*  fp : file sul quale si produce la stampa
*/
BOOL StampaEtichetta(char *szSQLCmd,FILE *fp,char *szLabelBaseName)
{
	FILE *fpBase;
	char szBuffer[256];
	char szDestination[256];
	char szLabelBaseFileName[256];
	char *pPtr;
	int nIndex;
	int nFieldIndex;
	BOOL bFound;
	BOOL bSaltaRiga=FALSE;
	PGresult *PGRes;

#ifdef TRACE
	trace_out_vstr_date(1, "StampaEtichetta(%s,%s)",szSQLCmd,szLabelBaseName);
#endif

	PGRes=PGExecSQL(Cfg.nDebugVersion,szSQLCmd);
	if(PQntuples(PGRes)){

		sprintf(szLabelBaseFileName,"%s/%s",Cfg.szPathData,szLabelBaseName);
		if((fpBase=fopen(szLabelBaseFileName,"r"))!=(FILE *)NULL){
			while(fgets(szBuffer,sizeof(szBuffer)-1,fpBase)){
				/*
				* prevedo piu' campi su una riga
				*/
				bFound=TRUE;
				while(bFound){
					/*
					* cerco l'ID campo all'interno della riga
					*/
					nIndex=0;
					bFound=FALSE;
					while(!bFound && strlen(LabelFields[nIndex].szFieldID)){
						if((pPtr=strstr(szBuffer,LabelFields[nIndex].szFieldID))!=NULL){
							bFound=TRUE;
							continue;
						}
						nIndex++;
					}
					strcpy(szDestination,szBuffer);
					if(bFound){
						szDestination[pPtr-szBuffer]='\0';
						if((nFieldIndex=PQfnumber(PGRes,LabelFields[nIndex].szFieldName))==-1){
							strcat(szDestination,"XXXX");
						} else {
							strcat(szDestination,StrTrimAll(PQgetvalue(PGRes,0,nFieldIndex)));
						}

						strcat(szDestination,pPtr+strlen(LabelFields[nIndex].szFieldID));
						/*
						* riporto le modifiche 
						*/
						strcpy(szBuffer,szDestination);
					}
				}
				if(!bSaltaRiga){
					fputs(szDestination,fp);
				}
				bSaltaRiga=FALSE;
			}
			fclose(fpBase);
		}
	}
	PQclear(PGRes);

	return TRUE;
}

/*
* Calcola Checksum Modulo 10 per Bartolini
*/
int	CalcDigit(char value)
{
	int	tmp=0;
	char	TmpString[3];
	int	RetVal=0;

	tmp=(value-'0')*2;
	if(tmp<10)	RetVal=tmp;
	else{
		sprintf(TmpString,"%d",tmp);
		RetVal+=TmpString[0]-'0';
		RetVal+=TmpString[1]-'0';
	}
	return(RetVal);		
}

/*
* Calcola Checksum Modulo 10 per Bartolini
*/
char	CalcCheck(char *szString)
{
	char	RetVal;
	int	i;
	int	Sum=0;
	char	EndString[5];
	int	tmp1,tmp2;

	for(i=strlen(szString)-1;i>=0;i-=2) Sum+=CalcDigit(szString[i]);
	for(i=strlen(szString)-2;i>=1;i-=2) Sum+=szString[i]-'0';
	sprintf(EndString,"%03d",Sum);
	if(EndString[2]=='0')	RetVal='0';
	else{
		tmp1=Sum/10;
		tmp2=(tmp1+1)*10;
		tmp1=tmp2-Sum;
		RetVal=tmp1+'0';
	}
	return(RetVal);	
}

BOOL StampaPackingListReport(char *szOrdKey, char *szPrinterName, int tipo)
{
	char szParagraph[128];
	char szSelectCmd[2048];
	FILE *fp;
	char szFileName[128];
	char szCommand[128];
	char szFLFTZ[128];
	char cFlagFtz;
	int nRC;
	int nSeparatorField;
	int nCodiceProdottoField;
	BOOL bRetVal=TRUE;
	PGresult *PGRes;
	char file_name[128];

	PGRes=PGExecSQL(Cfg.nDebugVersion,"select opflftz from ord_prod where ordkey='%s';",szOrdKey);
	if(PQntuples(PGRes)==1){
		strcpy(szFLFTZ,PQgetvalue(PGRes,0,0));
		cFlagFtz=szFLFTZ[0];
		switch(cFlagFtz){
			case ORDINE_FITTIZIO:
				strcpy(file_name, "packing_list_solo_documento.xml");
			break;
			default:
				switch(tipo){
					default: 
					case 0: /* normale */
						strcpy(file_name, "packing_list.xml");
					break;
					case 1: /* no pdf */
						strcpy(file_name, "packing_list_no_pdf.xml");
					break;
					case 2: /* solo pdf */
						strcpy(file_name, "packing_list_pdf.xml");
					break;
				}
			break;
		}
	} else {
		bRetVal=FALSE;
	}

	PQclear(PGRes);

	if(bRetVal){
		sprintf(szCommand,"rptrender -databaseURL=pgsql://%s/%s -username=roberto -passwd=3zin -param=ordkey=%s -close -pdf -outpdf=%s/print.pdf %s/%s	",
			Cfg.szPGHost,
			Cfg.szPGDataBase,
			szOrdKey,
			Cfg.szPathStampe,
			Cfg.szPathData,
			file_name);
	#ifdef TRACE
		trace_out_vstr_date(1,"RPTrender : %s",szCommand);
	#endif
		system(szCommand);

		sprintf(szCommand,"lpr -P%s %s/print.pdf",szPrinterName,Cfg.szPathStampe);

		system(szCommand);
	}
	
	return bRetVal;

}

BOOL StampaPackingList(char *szOrdKey, char *szPrinterName)
{
	char szParagraph[128];
	char szSelectCmd[2048];
	FILE *fp;
	char szFileName[128];
	char szCommand[128];
	char szFLFTZ[128];
	char cFlagFtz;
	int nRC;
	int nSeparatorField;
	int nCodiceProdottoField;
	BOOL bRetVal=TRUE;
	PGresult *PGRes;

	/*
	* Apertura del file per la stampa
	*/
	sprintf(szFileName,"%s/print_file",Cfg.szPathData);
	if ((fp=fopen(szFileName,"w"))==(FILE *)NULL) {
		bRetVal = FALSE;
	}
	if(bRetVal){
		PGRes=PGExecSQL(Cfg.nDebugVersion,"select ro.rodscli,ro.roincli,ro.rocpcli,ro.rolocli,ro.roprcli,op.opnmcll,op.opnmrgh,op.opnmcpe,op.oppspre,op.opflftz, to_char(CURRENT_TIMESTAMP, 'DD-MM-YYYY') from ric_ord as \"ro\", ord_prod as \"op\" where ro.ordprog=op.ordprog and op.ordkey='%s';",szOrdKey);

		if(PQntuples(PGRes)==1){
			fprintf(fp,"Cliente : %s                        %s\n",PQgetvalue(PGRes,0,0), PQgetvalue(PGRes,0,10));
			fprintf(fp,"          %s\n",PQgetvalue(PGRes,0,1));
			fprintf(fp,"          %s - %s %s\n",PQgetvalue(PGRes,0,2),PQgetvalue(PGRes,0,3),PQgetvalue(PGRes,0,4));

			if(Cfg.nSAP){
				/*
				* st 10-07-01 elimino le prime 3 cifre del codice ordkey
				*/ 
				fprintf(fp,"!3%s  Colli : !3%d  Righe : %3d  Copie : %4d\n",RightStr(szOrdKey,13),
					atoi(PQgetvalue(PGRes,0,5)), atoi(PQgetvalue(PGRes,0,6)), atoi(PQgetvalue(PGRes,0,7)));
			} else {
				fprintf(fp,"!3%s  Colli : !3%d  Righe : %3d  Copie : %4d\n",szOrdKey,
					atoi(PQgetvalue(PGRes,0,5)), atoi(PQgetvalue(PGRes,0,6)), atoi(PQgetvalue(PGRes,0,7)));
			}

/*
* LG: Ricordarsi di mettere nella query in cni.cfg la gestione della divisione
*     per zero nel calcolo del peso. La funzione postgres adatta dovrebbe essere
*     LEAST(......)
*/
			strcpy(szFLFTZ,PQgetvalue(PGRes,0,9));
			cFlagFtz=szFLFTZ[0];
			switch(cFlagFtz){
				case ORDINE_FITTIZIO:
					fprintf(fp,"!3 SOLO DOCUMENTO\n");
				break;
				default:
				break;
			}



			strcpy(szParagraph,"Stampa Packing List Ordine");
			xncGetFileString(szParagraph,"SELECT_CMD", "", szSelectCmd, sizeof(szSelectCmd),Cfg.szCniCfg,NULL); 
			nSeparatorField = xncGetFileInt(szParagraph, "SEPARATOR_FIELD", -1, Cfg.szCniCfg, NULL);
			nCodiceProdottoField = xncGetFileInt(szParagraph, "CODPROD_FIELD", -1, Cfg.szCniCfg, NULL);
			nRC=CustomPrintListFromSelect(fp,szSelectCmd, szOrdKey,0,szPrinterName,nSeparatorField,nCodiceProdottoField);
			if (nRC==-1){
				bRetVal = FALSE;
			}
		} else {
			bRetVal = FALSE;
		}
		PQclear(PGRes);
	}
	/*
	* rm 29-11-2000 : gestione taglierina stampante MT 2033-S
	* configurazione stampante:
	* AUTOQTO : Tagliare al FF
	* MODULO : RIGHE : 18 (3 pollici)
	*/
	fprintf(fp,"\n\n\n\014");
	fclose(fp);

	if(bRetVal){
		/*
		* sprintf(szCommand,"pr -f -h 'CNI - Stampa Lista Ordine %s' %s | lpr -P%s",szOrdKey,szFileName,szPrinterName);
		*/
		sprintf(szCommand,"lpr -P%s %s ",szPrinterName,szFileName);
		system(szCommand);
	}
	return bRetVal;

}

BOOL StampaPackingListSenzaColliPdf(char *szOrdKey, char *szPrinterName)
{
	char szParagraph[128];
	char szSelectCmd[2048];
	FILE *fp;
	char szFileName[128];
	char szCommand[128];
	char szFLFTZ[128];
	char cFlagFtz;
	int nRC;
	int nSeparatorField;
	int	nCodiceProdottoField;
	BOOL bRetVal=TRUE;
	PGresult *PGRes;
	PGresult *PGResRighe;
	int nColli;
	int nRighe;
	int nCopie;

	/*
	* Apertura del file per la stampa
	*/
	sprintf(szFileName,"%s/print_file",Cfg.szPathData);
	if ((fp=fopen(szFileName,"w"))==(FILE *)NULL) {
		bRetVal = FALSE;
	}
	if(bRetVal){
		PGRes=PGExecSQL(Cfg.nDebugVersion,"select ro.rodscli,ro.roincli,ro.rocpcli,ro.rolocli,ro.roprcli,op.opnmcll,op.opnmrgh,op.opnmcpe,op.oppspre,op.opflftz from ric_ord as \"ro\", ord_prod as \"op\" where ro.ordprog=op.ordprog and op.ordkey='%s';",szOrdKey);

		if(PQntuples(PGRes)==1){
			fprintf(fp,"Cliente : %s\n",PQgetvalue(PGRes,0,0));
			fprintf(fp,"          %s\n",PQgetvalue(PGRes,0,1));
			fprintf(fp,"          %s - %s %s\n",PQgetvalue(PGRes,0,2),PQgetvalue(PGRes,0,3),PQgetvalue(PGRes,0,4));
			nColli=atoi(PQgetvalue(PGRes,0,5));
			nRighe=atoi(PQgetvalue(PGRes,0,6));
			nCopie=atoi(PQgetvalue(PGRes,0,7));

			/*
			* Ricavo alcuni valori dalla tabella colli se possibile
			*/
			PGResRighe=PGExecSQL(Cfg.nDebugVersion,"select max(cpnmcol), sum(cpnmrgh), sum(cpnmcpe) from col_prod where ordkey='%s' and cpflpdf='%c';",szOrdKey,COLLO_SENZA_PDF);
			if(PQntuples(PGRes)==1){
				nColli=atoi(PQgetvalue(PGResRighe,0,0));
				nRighe=atoi(PQgetvalue(PGResRighe,0,1));
				nCopie=atoi(PQgetvalue(PGResRighe,0,2));
			}
			PQclear(PGResRighe);

			if(Cfg.nSAP){
				/*
				* st 10-07-01 elimino le prime 3 cifre del codice ordkey
				*/ 
				fprintf(fp,"!3%s  Colli : !3%d  Righe : %3d  Copie : %4d\n",RightStr(szOrdKey,13),
					nColli,nRighe,nCopie);
			} else {
				fprintf(fp,"!3%s  Colli : !3%d  Righe : %3d  Copie : %4d\n",szOrdKey,
					nColli,nRighe,nCopie);
			}

/*
* LG: Ricordarsi di mettere nella query in cni.cfg la gestione della divisione
*     per zero nel calcolo del peso. La funzione postgres adatta dovrebbe essere
*     LEAST(......)
*/
			strcpy(szFLFTZ,PQgetvalue(PGRes,0,9));
			cFlagFtz=szFLFTZ[0];
			switch(cFlagFtz){
				case ORDINE_FITTIZIO:
					fprintf(fp,"!3 SOLO DOCUMENTO\n");
				break;
				default:
				break;
			}



			strcpy(szParagraph,"Stampa Packing List Ordine");
			xncGetFileString(szParagraph,"SELECT_CMD_NO_PDF", "", szSelectCmd, sizeof(szSelectCmd),Cfg.szCniCfg,NULL); 
			nSeparatorField = xncGetFileInt(szParagraph, "SEPARATOR_FIELD", -1, Cfg.szCniCfg, NULL);
			nCodiceProdottoField = xncGetFileInt(szParagraph, "CODPROD_FIELD", -1, Cfg.szCniCfg, NULL);
			nRC=CustomPrintListFromSelect(fp,szSelectCmd, szOrdKey,0,szPrinterName,nSeparatorField,nCodiceProdottoField);
			if (nRC==-1){
				bRetVal = FALSE;
			}
		} else {
			bRetVal = FALSE;
		}
		PQclear(PGRes);
	}
	/*
	* rm 29-11-2000 : gestione tagierina stampante MT 2033-S
	* configurazione stampante:
	* AUTOQTO : Tagliare al FF
	* MODULO : RIGHE : 18 (3 pollici)
	*/
	fprintf(fp,"\n\n\n\014");
	fclose(fp);

	if(bRetVal){
		/*
		* sprintf(szCommand,"pr -f -h 'CNI - Stampa Lista Ordine %s' %s | lpr -P%s",szOrdKey,szFileName,szPrinterName);
		*/
		sprintf(szCommand,"lpr -P%s %s ",szPrinterName,szFileName);
		system(szCommand);
	}
	return bRetVal;

}

BOOL StampaPackingListSoloColliPdf(char *szOrdKey, char *szPrinterName)
{
	char szParagraph[128];
	char szSelectCmd[2048];
	FILE *fp;
	char szFileName[128];
	char szCommand[128];
	char szFLFTZ[128];
	char cFlagFtz;
	int nRC;
	int nSeparatorField;
	int	nCodiceProdottoField;
	BOOL bRetVal=TRUE;
	PGresult *PGRes;
	PGresult *PGResRighe;
	int nColli;
	int nRighe;
	int nCopie;

	/*
	* Apertura del file per la stampa
	*/
	sprintf(szFileName,"%s/print_file",Cfg.szPathData);
	if ((fp=fopen(szFileName,"w"))==(FILE *)NULL) {
		bRetVal = FALSE;
	}
	if(bRetVal){
		PGRes=PGExecSQL(Cfg.nDebugVersion,"select ro.rodscli,ro.roincli,ro.rocpcli,ro.rolocli,ro.roprcli,op.opnmcll,op.opnmrgh,op.opnmcpe,op.oppspre,op.opflftz from ric_ord as \"ro\", ord_prod as \"op\" where ro.ordprog=op.ordprog and op.ordkey='%s';",szOrdKey);

		if(PQntuples(PGRes)==1){
			fprintf(fp,"Cliente : %s\n",PQgetvalue(PGRes,0,0));
			fprintf(fp,"          %s\n",PQgetvalue(PGRes,0,1));
			fprintf(fp,"          %s - %s %s\n",PQgetvalue(PGRes,0,2),PQgetvalue(PGRes,0,3),PQgetvalue(PGRes,0,4));
			nColli=atoi(PQgetvalue(PGRes,0,5));
			nRighe=atoi(PQgetvalue(PGRes,0,6));
			nCopie=atoi(PQgetvalue(PGRes,0,7));

			/*
			* Ricavo alcuni valori dalla tabella colli se possibile
			*/
			PGResRighe=PGExecSQL(Cfg.nDebugVersion,"select max(cpnmcol), sum(cpnmrgh), sum(cpnmcpe) from col_prod where ordkey='%s' and cpflpdf='%c';",szOrdKey,COLLO_CON_PDF);
			if(PQntuples(PGRes)==1){
				/*
				 * Richiesta Rodighiero: l'ultimo collo e uno solo allora scrivo 1 al posto del max(cpnmcol)
				 */
				//nColli=atoi(PQgetvalue(PGResRighe,0,0));
				nColli=1;
				nRighe=atoi(PQgetvalue(PGResRighe,0,1));
				nCopie=atoi(PQgetvalue(PGResRighe,0,2));
			}
			PQclear(PGResRighe);

			if(Cfg.nSAP){
				/*
				* st 10-07-01 elimino le prime 3 cifre del codice ordkey
				*/ 
				fprintf(fp,"!3%s  Colli : !3%d  Righe : %3d  Copie : %4d\n",RightStr(szOrdKey,13),
					nColli,nRighe,nCopie);
			} else {
				fprintf(fp,"!3%s  Colli : !3%d  Righe : %3d  Copie : %4d\n",szOrdKey,
					nColli,nRighe,nCopie);
			}

/*
* LG: Ricordarsi di mettere nella query in cni.cfg la gestione della divisione
*     per zero nel calcolo del peso. La funzione postgres adatta dovrebbe essere
*     LEAST(......)
*/
			strcpy(szFLFTZ,PQgetvalue(PGRes,0,9));
			cFlagFtz=szFLFTZ[0];
			switch(cFlagFtz){
				case ORDINE_FITTIZIO:
					fprintf(fp,"!3 SOLO DOCUMENTO\n");
				break;
				default:
				break;
			}

			strcpy(szParagraph,"Stampa Packing List Ordine");
			xncGetFileString(szParagraph,"SELECT_CMD_PDF", "", szSelectCmd, sizeof(szSelectCmd),Cfg.szCniCfg,NULL); 
			nSeparatorField = xncGetFileInt(szParagraph, "SEPARATOR_FIELD", -1, Cfg.szCniCfg, NULL);
			nRC=PrintListFromSelect(fp,szSelectCmd, szOrdKey,0,szPrinterName,nSeparatorField);

			if (nRC==-1){
				bRetVal = FALSE;
			}

		} else {
			bRetVal = FALSE;
		}
		PQclear(PGRes);
	}
	/*
	* rm 29-11-2000 : gestione tagierina stampante MT 2033-S
	* configurazione stampante:
	* AUTOQTO : Tagliare al FF
	* MODULO : RIGHE : 18 (3 pollici)
	*/
	fprintf(fp,"\n\n\n\014");
	fclose(fp);

	if(bRetVal){
		/*
		* sprintf(szCommand,"pr -f -h 'CNI - Stampa Lista Ordine %s' %s | lpr -P%s",szOrdKey,szFileName,szPrinterName);
		*/
		sprintf(szCommand,"lpr -P%s %s ",szPrinterName,szFileName);
		system(szCommand);
	}
	return bRetVal;

}


BOOL StampaPackingListCollo(char *szOrdKey, int nCollo, char *szPrinterName)
{
	char szParagraph[128];
	char szSelectCmd[2048];
	FILE *fp;
	char szFileName[128];
	char szCommand[128];
	int nRC;
	BOOL bRetVal=TRUE;
	PGresult *PGRes;
	PGresult *PGResRighe;
	int nColli;
	int nRighe;
	int nCopie;

	/*
	* Apertura del file per la stampa
	*/
	sprintf(szFileName,"%s/print_file",Cfg.szPathData);
	if ((fp=fopen(szFileName,"w"))==(FILE *)NULL) {
		bRetVal = FALSE;
	}
	if(bRetVal){
		// fprintf(fp,"Stampa Packing List Collo : %s %d\n",szOrdKey,nCollo);

		PGRes=PGExecSQL(Cfg.nDebugVersion,"select ro.ordprog,ro.rodscli,ro.roincli,ro.rocpcli,ro.rolocli,ro.roprcli,op.opnmcll,cp.cpnmrgh,cp.cpnmcpe,cp.cppspre, to_char(CURRENT_TIMESTAMP, 'DD-MM-YYYY') from ric_ord as \"ro\", ord_prod as \"op\", col_prod as \"cp\" where op.ordprog=ro.ordprog and op.ordkey='%s' and cp.ordkey=op.ordkey and cpnmcol=%d;",szOrdKey,nCollo);

		if(PQntuples(PGRes)==1){
			fprintf(fp,"Cliente : %s                        %s\n",PQgetvalue(PGRes,0,1), PQgetvalue(PGRes,0,10));
			fprintf(fp,"          %s\n",PQgetvalue(PGRes,0,2));
			fprintf(fp,"          %s - %s %s\n\n",PQgetvalue(PGRes,0,3),PQgetvalue(PGRes,0,4),PQgetvalue(PGRes,0,5));
			nColli=atoi(PQgetvalue(PGRes,0,6));
			nRighe=atoi(PQgetvalue(PGRes,0,7));
			nCopie=atoi(PQgetvalue(PGRes,0,8));

			fprintf(fp,"Collo : %d / %d\nRighe : %3d  Copie : %4d\n\n", nCollo,nColli, nRighe,nCopie);
		}

		PQclear(PGRes);



		strcpy(szParagraph,"Stampa Packing List Collo");
		xncGetFileString(szParagraph,"SELECT_CMD", "", szSelectCmd, sizeof(szSelectCmd),Cfg.szCniCfg,NULL); 
		nRC=PrintListFromSelect(fp,szSelectCmd, szOrdKey,nCollo,szPrinterName,-1);
		if (nRC==-1){
			bRetVal = FALSE;
		}
	}
	fclose(fp);

	if(bRetVal){
		/*
		* sprintf(szCommand,"pr -f -h 'CNI - Stampa Packing List Collo' %s | lpr -P%s",szFileName,szPrinterName);
		*/
		// sprintf(szCommand,"lpr -P%s %s ",szPrinterName,szFileName);
		// system(szCommand);
		char szTitolo[128];

		sprintf(szTitolo, "CONSEGNA %s Collo %d di %d", szOrdKey, nCollo,nColli);
		// PrintFile(szFileName,szPrinterName,"A&L - Stampa Packing List Collo",PRINT_A2PS,TRUE);
		PrintFile(szFileName,szPrinterName,szTitolo,PRINT_A2PS,TRUE);
	}
	return bRetVal;

}

int GetMaxLineWidth(char *szFile)
{
	FILE *fp;
	char szBuffer[1024];
	int nMaxWidth=0;

	if ((fp=fopen(szFile,"r"))!=(FILE *)NULL) {
		while( fgets(szBuffer, sizeof(szBuffer), fp)!=NULL ){
			nMaxWidth=max(nMaxWidth,strlen(szBuffer));
		}
		fclose(fp);
	}

	return nMaxWidth;
}

/* 
* Stampa un file
* defaults: stampa ascii
*/
BOOL PrintFile(char *szFile,char *szPrinter,char *szTitle,int nPrintType,BOOL bDelete)
{
	char szCommand[2048];
	char szBuffer[128];
	BOOL bHtml2PS=FALSE;
	BOOL bA2PS=FALSE;
	BOOL bAscii=FALSE;
	int nWidth=0;
	char szHtml2PSCfgFileName[128];
	BOOL bOK=TRUE;

	switch(nPrintType){
		case PRINT_A2PS:
			bA2PS=TRUE;
		break;
		case PRINT_HTML2PS:
			bHtml2PS=TRUE;
		break;
		default:
		case PRINT_ASCII:
			bAscii=TRUE;
		break;
	}

	if(bHtml2PS){
		sprintf(szCommand,"html2ps -o %s.ps %s",szFile,szFile);
		strcat(szCommand," | lpr ");
		/* stampante specificata */
		if(szPrinter && strlen(szPrinter)){
			sprintf(szBuffer," -P%s",szPrinter);
			strcat(szCommand,szBuffer);
		}
	} else if(bA2PS){
		strcpy(szCommand,"a2ps -R -q --medium=A4  --columns=1");
		/* titolo */
		if(szTitle && strlen(szTitle)){
			sprintf(szBuffer," --center-title=\"%s\"",szTitle);
			strcat(szCommand,szBuffer);
		}
		sprintf(szBuffer," -l %d", GetMaxLineWidth(szFile));
		strcat(szCommand,szBuffer);

		strcat(szCommand," ");
		strcat(szCommand,szFile);

		/* stampante specificata */
		if(szPrinter && strlen(szPrinter)){
			sprintf(szBuffer," -P%s",szPrinter);
			strcat(szCommand,szBuffer);
		}
	} else if(bAscii) {
		sprintf(szCommand,"cat %s",szFile);
		strcat(szCommand," | lpr ");
		/* stampante specificata */
		if(szPrinter && strlen(szPrinter)){
			sprintf(szBuffer," -P%s",szPrinter);
			strcat(szCommand,szBuffer);
		}
	} else {
		return FALSE;
	}
	

#ifdef TRACE
	trace_out_vstr_date(1,"PrintFile : %s",szCommand);
#endif
	if(system(szCommand)==-1){
		bOK=FALSE;
	}
	if(bDelete){
		unlink(szFile);
	}

	return bOK;
}

BOOL StampaPdfOrdine(char *szOrdine, char *szPrinterName)
{
	char szFileName[128];
	char szCommand[128];
	PGresult *PGRes;
	int nTuple;
	char szFLPDF[128];
	char cPdfStato;
	BOOL bOK=TRUE;

#ifdef TRACE
	trace_out_vstr_date(1, "StampaPdfOrdine(%s,%s)",szOrdine,szPrinterName);
#endif
	/*
	* Verifico l'esistenza dell'ordine nel db, il tipo di doc da stampare (F,D) e la presenza
	* del file nell'apposito direttorio.
	*/
	PGRes=PGExecSQL(Cfg.nDebugVersion>1,"select ordprog,roflpdf from ric_ord where ordprog='%s';",szOrdine);

	nTuple=PQntuples(PGRes);

	if(nTuple){
		//strcpy(szOrdprog,PQgetvalue(PGRes,0,0));
		strcpy(szFLPDF,PQgetvalue(PGRes,0,1));
		cPdfStato=szFLPDF[0];
	}else{
#ifdef TRACE
		trace_out_vstr_date(1, "L'ordine [%s] non e' presente nella tabella ric_ord",szOrdine);
#endif
		bOK=FALSE;
	}
	PQclear(PGRes);

	if (!bOK) return bOK;
	
	/*
	* Apertura del file per la stampa
	*/
	/*
	 * Qui con .PDF maiuscolo
	 */
	sprintf(szFileName,"%s/pdfstr/%c%s.PDF",Cfg.szPathExport,cPdfStato,szOrdine);
	if (FileExists(szFileName)) {

#ifdef TRACE
		trace_out_vstr_date(1, "Stampa file [%s] ",szFileName);
#endif
		sprintf(szCommand,"lpr -P%s %s",szPrinterName,szFileName);
		system(szCommand);
	}else {
		/*
		 * Qui con .pdf maiuscolo
		 */
		sprintf(szFileName,"%s/pdfstr/%c%s.pdf",Cfg.szPathExport,cPdfStato,szOrdine);
		if (FileExists(szFileName)) {
#ifdef TRACE
			trace_out_vstr_date(1, "Stampa file [%s] ",szFileName);
#endif
			sprintf(szCommand,"lpr -P%s %s",szPrinterName,szFileName);
			system(szCommand);
		}else {
			/*
			 * Non trovata fattura o bolla
			 */
#ifdef TRACE
			trace_out_vstr_date(1, "File [%s] non presente",szFileName);
#endif
			bOK=FALSE;
		}
	}

	return(bOK);
}


BOOL StampaPdfOrdineAdvanced(char *szOrdine, char *szPrinterName)
{
	char szFileName[128];
	char szCommand[128];
	char szDirectoryName[128];
	PGresult *PGRes;
	int nTuple;
	char szFLPDF[128];
	char cPdfStato;
	BOOL bOK=TRUE;
	BOOL bFound;
	DIR *dir;
	struct dirent *files_in_dir;

#ifdef TRACE
	trace_out_vstr_date(1, "StampaPdfOrdineAdvanced(%s,%s)",szOrdine,szPrinterName);
#endif
	/*
	* Verifico l'esistenza dell'ordine nel db, il tipo di doc da stampare (F,D) e la presenza
	* del file nell'apposito direttorio.
	*/
	PGRes=PGExecSQL(Cfg.nDebugVersion>1,"select ordprog,roflpdf from ric_ord where ordprog='%s';",szOrdine);

	nTuple=PQntuples(PGRes);

	if(nTuple){
		//strcpy(szOrdprog,PQgetvalue(PGRes,0,0));
		strcpy(szFLPDF,PQgetvalue(PGRes,0,1));
		cPdfStato=szFLPDF[0];
	}else{
#ifdef TRACE
		trace_out_vstr_date(1, "L'ordine [%s] non e' presente nella tabella ric_ord",szOrdine);
#endif
		bOK=FALSE;
	}
	PQclear(PGRes);

	/* rm 2024-07-19 11:11:03 : se l'ordine non ha fattura non stampo niente ed esco */
	if (cPdfStato != ORDINE_CON_FATTURA){
#ifdef TRACE
		trace_out_vstr_date(1, "L'ordine [%s:%c] non ha fattura",szOrdine, cPdfStato);
#endif
		bOK=FALSE;
	}


	if (!bOK) return bOK;
	
	bFound = FALSE;

	/*
	* Apertura del file per la stampa
	*/
	sprintf(szDirectoryName,"%s/pdfstr", Cfg.szPathExport);
	dir = opendir (szDirectoryName);
	if (dir != NULL) {
		while ((files_in_dir = readdir (dir))){
			strcpy(szFileName,files_in_dir->d_name);
			/* si tratta di una stampa ordine se i primi 10 caratteri sono l'ordine e a seguire _XXXXX.pdf */
#ifdef TRACE
			trace_out_vstr_date(1, "Controllo file [%s] ",szFileName);
#endif
			if(!strncmp(szFileName, szOrdine, 10) && !strncasecmp(szFileName+(strlen(szFileName)-4), ".pdf", 4)){
#ifdef TRACE
				trace_out_vstr_date(1, "Stampa file [%s] ",szFileName);
#endif
				sprintf(szCommand,"lpr -P%s %s",szPrinterName,szFileName);
				system(szCommand);
				bFound = TRUE;
			}
		}
	}


	if (!bFound) {
#ifdef TRACE
		trace_out_vstr_date(1, "non sono presenti fatture per ordine presente",szOrdine);
#endif
		bOK=FALSE;
	}

	return(bOK);
}


/*
* CustomPrintListFromSelect()
* ritorna il numero di tuple o -1 se errore
* rm 29-10-2012 : vestione customizzata per la gestione delle righe 'doppie'
*                 che significano cambio lotto di produzione, si verifica il campo codice (0) e
*                 si mettono in grassetto le righe interessate
*/
int CustomPrintListFromSelect(FILE *fp,char *szSqlCmd, char *szOrdProg, int nNumCollo,char *szPrinterName,int nSeparatorFieldIndex,int nCodiceProdottoFieldIndex)
{
	int nTuples;
	int nFields;
	int nIndex;
	char szBuffer[128];
	char szSeparator[1024];
	char szTitles[1024];
	char szSeparatorValue[256];
	char szOldSeparatorValue[256];
	int nFieldIndex;
	char szCmd[4096];
	char *pPtr;
	PGresult *PGRes = NULL;
	int nFieldLength[256];
	char szCodiceProdotto[256];
	char szOldCodiceProdotto[256];
	BOOL bBold=FALSE;

#ifdef TRACE
	trace_out_vstr_date(1, "PrintListFromSelect(%s)",szPrinterName);
#endif

	strcpy(szCmd,szSqlCmd);
	/*
	* Sosituisco i valori parametrici
	* %ORDPROG%   : Codice Ordine
	* %NUMCOLLO%  : Numero Collo
	*/
	if((pPtr=strstr(szSqlCmd,"%ORDPROG%"))){
		*pPtr='\0';
		strcpy(szCmd,szSqlCmd);
		strcat(szCmd,szOrdProg);
		strcat(szCmd,pPtr+9);
	}
	strcpy(szSqlCmd,szCmd);
	if((pPtr=strstr(szSqlCmd,"%NUMCOLLO%"))){
		*pPtr='\0';
		strcpy(szCmd,szSqlCmd);
		sprintf(szBuffer,"%d",nNumCollo);
		strcat(szCmd,szBuffer);
		strcat(szCmd,pPtr+10);
	}
	strcpy(szSqlCmd,szCmd);
	
	/*
	* Eseguo la select specificata
	*/
	PGRes=PGExecSQL(Cfg.nDebugVersion,szSqlCmd);

	nTuples=PQntuples(PGRes);
	nFields=PQnfields(PGRes);
	szSeparator[0]='\0';

	if(nTuples){
		fprintf(fp,"+");
		strcat(szSeparator,"+");
		for(nFieldIndex=0;nFieldIndex<nFields;nFieldIndex++){
			/*
			* ricavo le lunghezze dei titoli
			*/ 
			if(PQftype(PGRes,nFieldIndex)==1042){
				/* si tratta di una stringa */
				nFieldLength[nFieldIndex]=strlen(PQfname(PGRes,nFieldIndex));
			} else if(PQftype(PGRes,nFieldIndex)==701){
				/* si tratta di un float */
				nFieldLength[nFieldIndex]=strlen(PQfname(PGRes,nFieldIndex));
			} else if(PQftype(PGRes,nFieldIndex)==23){
				/* si tratta di un int */
				nFieldLength[nFieldIndex]=strlen(PQfname(PGRes,nFieldIndex));
			} else {
				/* altro... lo tratto come una stringa */
				nFieldLength[nFieldIndex]=strlen(PQfname(PGRes,nFieldIndex));
			}
			
			memset(szBuffer,'-',nFieldLength[nFieldIndex]); szBuffer[nFieldLength[nFieldIndex]]='\0';

			fprintf(fp,szBuffer);
			fprintf(fp,"+");
			strcat(szSeparator,szBuffer);
			strcat(szSeparator,"+");
		}
		fprintf(fp,"\n");

		szTitles[0]='\0';

		fprintf(fp,"|");
		strcat(szTitles,"|");
		for(nFieldIndex=0;nFieldIndex<nFields;nFieldIndex++){
			/*
			* impostazione titolo colonna : utilizzo la .._set_column_title per avere la possibilita'
			* di ottenere la larghezza ottimale in fase di visualizzazione
			*/ 
			sprintf(szBuffer,"%-*.*s|",nFieldLength[nFieldIndex],nFieldLength[nFieldIndex],PQfname(PGRes,nFieldIndex));
			fprintf(fp,szBuffer);
			strcat(szTitles,szBuffer);
		}
		fprintf(fp,"\n");

		fprintf(fp,szSeparator);
		fprintf(fp,"\n");

		/*
		* Aggiungo le tuple alla lista (una alla volta)
		*/
		strcpy(szSeparatorValue,"");
		strcpy(szOldSeparatorValue,"");

		strcpy(szCodiceProdotto,"");
		strcpy(szOldCodiceProdotto,"");

		for(nIndex=0;nIndex<nTuples;nIndex++){


			if(nSeparatorFieldIndex!=-1) strcpy(szSeparatorValue,PQgetvalue(PGRes,nIndex,nSeparatorFieldIndex));

			/* valori diversi per il campo discriminante */
			if(nIndex && strcmp(szSeparatorValue,szOldSeparatorValue)){
				fprintf(fp,szSeparator);
				fprintf(fp,"\n");
				fprintf(fp,szTitles);
				fprintf(fp,"\n");
				fprintf(fp,szSeparator);
				fprintf(fp,"\n");
				
				strcpy(szSeparatorValue,szOldSeparatorValue);

			}

			/*
			* rm 29-10-2012 : prendo il codice prodotto - campo nCodiceProdottoFieldIndex 
			*/ 
			if(nCodiceProdottoFieldIndex!=-1) strcpy(szCodiceProdotto,PQgetvalue(PGRes,nIndex,nCodiceProdottoFieldIndex));

			/* valori uguali per il campo codice prodotto */
			if(nIndex && !strcmp(szCodiceProdotto,szOldCodiceProdotto)){
				bBold=TRUE;
			}


			fprintf(fp,"|");
			for(nFieldIndex=0;nFieldIndex<nFields;nFieldIndex++){
				//if(bBold) fprintf(fp,"");
				if(PQftype(PGRes,nFieldIndex)==1042){
					/* si tratta di una stringa */
					fprintf(fp,"%-*.*s",nFieldLength[nFieldIndex],nFieldLength[nFieldIndex],PQgetvalue(PGRes,nIndex,nFieldIndex));
				} else if(PQftype(PGRes,nFieldIndex)==701){
					/* si tratta di un float */
					fprintf(fp,"%*.*f",nFieldLength[nFieldIndex],3,atof(PQgetvalue(PGRes,nIndex,nFieldIndex)));
				} else if(PQftype(PGRes,nFieldIndex)==23){
					/* si tratta di un int */
					fprintf(fp,"%*d",nFieldLength[nFieldIndex],atoi(PQgetvalue(PGRes,nIndex,nFieldIndex)));
				} else {
					/* altro... lo tratto come una stringa */
					fprintf(fp,"%-*.*s",nFieldLength[nFieldIndex],nFieldLength[nFieldIndex],PQgetvalue(PGRes,nIndex,nFieldIndex));
				}
				//if(bBold) fprintf(fp,"");
				fprintf(fp,"|");
			}
			fprintf(fp,"\n");

			if(nSeparatorFieldIndex!=-1) strcpy(szOldSeparatorValue,PQgetvalue(PGRes,nIndex,nSeparatorFieldIndex));

			/*
			* rm 29-10-2012 : modifiche per gestione righe con codice uguale (cambio lotto)
			*/
			bBold=FALSE;
			strcpy(szOldCodiceProdotto,szCodiceProdotto);
		}

		fprintf(fp,szSeparator);
		fprintf(fp,"\n");

	}
	
	PQclear(PGRes);

	return nTuples;
}


/*
* PrintListFromSelect()
* ritorna il numero di tuple o -1 se errore
*/
int PrintListFromSelect(FILE *fp,char *szSqlCmd, char *szOrdProg, int nNumCollo,char *szPrinterName,int nSeparatorFieldIndex)
{
	int nTuples;
	int nFields;
	int nIndex;
	char szBuffer[128];
	char szSeparator[1024];
	char szTitles[1024];
	char szSeparatorValue[256];
	char szOldSeparatorValue[256];
	int nFieldIndex;
	char szCmd[4096];
	char *pPtr;
	PGresult *PGRes = NULL;
	int nFieldLength[256];

#ifdef TRACE
	trace_out_vstr_date(1, "PrintListFromSelect(%s)",szPrinterName);
#endif

	strcpy(szCmd,szSqlCmd);
	/*
	* Sosituisco i valori parametrici
	* %ORDPROG%   : Codice Ordine
	* %NUMCOLLO%  : Numero Collo
	*/
	if((pPtr=strstr(szSqlCmd,"%ORDPROG%"))){
		*pPtr='\0';
		strcpy(szCmd,szSqlCmd);
		strcat(szCmd,szOrdProg);
		strcat(szCmd,pPtr+9);
	}
	strcpy(szSqlCmd,szCmd);
	if((pPtr=strstr(szSqlCmd,"%NUMCOLLO%"))){
		*pPtr='\0';
		strcpy(szCmd,szSqlCmd);
		sprintf(szBuffer,"%d",nNumCollo);
		strcat(szCmd,szBuffer);
		strcat(szCmd,pPtr+10);
	}
	strcpy(szSqlCmd,szCmd);
	
	/*
	* Eseguo la select specificata
	*/
	PGRes=PGExecSQL(Cfg.nDebugVersion,szSqlCmd);

	nTuples=PQntuples(PGRes);
	nFields=PQnfields(PGRes);
	szSeparator[0]='\0';

	if(nTuples){
		fprintf(fp,"+");
		strcat(szSeparator,"+");
		for(nFieldIndex=0;nFieldIndex<nFields;nFieldIndex++){
			/*
			* ricavo le lunghezze dei titoli
			*/ 
			if(PQftype(PGRes,nFieldIndex)==1042){
				/* si tratta di una stringa */
				nFieldLength[nFieldIndex]=strlen(PQfname(PGRes,nFieldIndex));
			} else if(PQftype(PGRes,nFieldIndex)==701){
				/* si tratta di un float */
				nFieldLength[nFieldIndex]=strlen(PQfname(PGRes,nFieldIndex));
			} else if(PQftype(PGRes,nFieldIndex)==23){
				/* si tratta di un int */
				nFieldLength[nFieldIndex]=strlen(PQfname(PGRes,nFieldIndex));
			} else {
				/* altro... lo tratto come una stringa */
				nFieldLength[nFieldIndex]=strlen(PQfname(PGRes,nFieldIndex));
			}
			
			memset(szBuffer,'-',nFieldLength[nFieldIndex]); szBuffer[nFieldLength[nFieldIndex]]='\0';

			fprintf(fp,szBuffer);
			fprintf(fp,"+");
			strcat(szSeparator,szBuffer);
			strcat(szSeparator,"+");
		}
		fprintf(fp,"\n");

		szTitles[0]='\0';

		fprintf(fp,"|");
		strcat(szTitles,"|");
		for(nFieldIndex=0;nFieldIndex<nFields;nFieldIndex++){
			/*
			* impostazione titolo colonna : utilizzo la .._set_column_title per avere la possibilita'
			* di ottenere la larghezza ottimale in fase di visualizzazione
			*/ 
			sprintf(szBuffer,"%-*.*s|",nFieldLength[nFieldIndex],nFieldLength[nFieldIndex],PQfname(PGRes,nFieldIndex));
			fprintf(fp,szBuffer);
			strcat(szTitles,szBuffer);
		}
		fprintf(fp,"\n");

		fprintf(fp,szSeparator);
		fprintf(fp,"\n");

		/*
		* Aggiungo le tuple alla lista (una alla volta)
		*/
		strcpy(szSeparatorValue,"");
		strcpy(szOldSeparatorValue,"");

		for(nIndex=0;nIndex<nTuples;nIndex++){

			if(nSeparatorFieldIndex!=-1) strcpy(szSeparatorValue,PQgetvalue(PGRes,nIndex,nSeparatorFieldIndex));

			/* valori diversi per il campo discriminante */
			if(nIndex && strcmp(szSeparatorValue,szOldSeparatorValue)){
				fprintf(fp,szSeparator);
				fprintf(fp,"\n");
				fprintf(fp,szTitles);
				fprintf(fp,"\n");
				fprintf(fp,szSeparator);
				fprintf(fp,"\n");
				
				strcpy(szSeparatorValue,szOldSeparatorValue);

			}


			fprintf(fp,"|");
			for(nFieldIndex=0;nFieldIndex<nFields;nFieldIndex++){
				if(PQftype(PGRes,nFieldIndex)==1042){
					/* si tratta di una stringa */
					fprintf(fp,"%-*.*s|",nFieldLength[nFieldIndex],nFieldLength[nFieldIndex],PQgetvalue(PGRes,nIndex,nFieldIndex));
				} else if(PQftype(PGRes,nFieldIndex)==701){
					/* si tratta di un float */
					fprintf(fp,"%*.*f|",nFieldLength[nFieldIndex],3,atof(PQgetvalue(PGRes,nIndex,nFieldIndex)));
				} else if(PQftype(PGRes,nFieldIndex)==23){
					/* si tratta di un int */
					fprintf(fp,"%*d|",nFieldLength[nFieldIndex],atoi(PQgetvalue(PGRes,nIndex,nFieldIndex)));
				} else {
					/* altro... lo tratto come una stringa */
					fprintf(fp,"%-*.*s|",nFieldLength[nFieldIndex],nFieldLength[nFieldIndex],PQgetvalue(PGRes,nIndex,nFieldIndex));
				}
			}
			fprintf(fp,"\n");

			if(nSeparatorFieldIndex!=-1) strcpy(szOldSeparatorValue,PQgetvalue(PGRes,nIndex,nSeparatorFieldIndex));
		}

		fprintf(fp,szSeparator);
		fprintf(fp,"\n");

	}
	
	PQclear(PGRes);

	return nTuples;
}



/*
* BOOL StampaOrdineStorico(char *pszOrdProg, char *pszPrinterName)
* Stampa la packing-list dell'ordine storico szOrdProg sulla stampante szPrinterName
*/
BOOL StampaOrdineStorico(char *pszOrdProg, char *pszPrinterName)
{
	char szParagraph[128];
	char szSelectCmd[2048];
	FILE *fp;
	char szFileName[128];
	char szCommand[128];
	char szOrdKey[128];
	int nRC;
	int nIndex;
	int nOrdini;
	int nSeparatorField;
	BOOL bRetVal=TRUE;
	PGresult *PGRes;
	PGresult *PGResOrdini;

	/*
	* Apertura del file per la stampa
	*/
	sprintf(szFileName,"%s/print_file",Cfg.szPathData);
	if ((fp=fopen(szFileName,"w"))==(FILE *)NULL) {
		bRetVal = FALSE;
	}
	if(bRetVal){
		
		PGResOrdini=PGExecSQL(Cfg.nDebugVersion,"select ordprog,rodscli,roincli,rocpcli,rolocli,roprcli,ronmcll,ronmrgh,ronmcpe,ropspre::float4/(1000),ropsrea::float4/(1000),abs(ropspre-ropsrea)::float4/(1000) from ric_stor where ordprog='%s';",pszOrdProg);
		
		if(PQntuples(PGResOrdini)==1){
			fprintf(fp,"CLIENTE : %s\n",PQgetvalue(PGResOrdini,0,1));
			fprintf(fp,"          %s\n",PQgetvalue(PGResOrdini,0,2));
			fprintf(fp,"          %s - %s %s\n\n",PQgetvalue(PGResOrdini,0,3),PQgetvalue(PGResOrdini,0,4),PQgetvalue(PGResOrdini,0,5));
			fprintf(fp,"ORDINE  : %s   Colli : %4d  Righe : %4d  Pezzi : %4d\n\n",pszOrdProg,
				atoi(PQgetvalue(PGResOrdini,0,6)), atoi(PQgetvalue(PGResOrdini,0,7)), atoi(PQgetvalue(PGResOrdini,0,8)));
			fprintf(fp,"PESO REALE   : %4.3f Kg\n",atof(PQgetvalue(PGResOrdini,0,10)));
			fprintf(fp,"PESO PREDET. : %4.3f Kg\n",atof(PQgetvalue(PGResOrdini,0,9)));
			fprintf(fp,"DIFFERENZA   :  %4.3f Kg\n",atof(PQgetvalue(PGResOrdini,0,11)));
		}
		PQclear(PGResOrdini);

		PGResOrdini=PGExecSQL(Cfg.nDebugVersion,"select ordkey from ord_stor where ordprog='%s';",pszOrdProg);
		nOrdini=PQntuples(PGResOrdini);

		for (nIndex=0;nIndex<nOrdini;nIndex++){
			strcpy(szOrdKey,PQgetvalue(PGResOrdini,nIndex,0));

			PGRes=PGExecSQL(Cfg.nDebugVersion,"select opnmcll,opnmrgh,opnmcpe,oppspre::float4/(1000),oppsrea::float4/(1000),abs(oppsrea-oppspre)::float4/(1000) from ord_stor where ordkey='%s';",szOrdKey);

			if(PQntuples(PGRes)==1){
				fprintf(fp,"\n\n+------------------------------------------------------------------------------+\n");
				fprintf(fp,"|Ordine  : %-12s   Colli : %-4d  Righe : %-4d  Pezzi : %-4d             |\n",szOrdKey,
					atoi(PQgetvalue(PGRes,0,0)), atoi(PQgetvalue(PGRes,0,1)), atoi(PQgetvalue(PGRes,0,2)));
				fprintf(fp,"|Peso reale : %8.3f Peso predet. : %8.3f Differenza : %8.3f           |\n",
					atof(PQgetvalue(PGRes,0,3)),atof(PQgetvalue(PGRes,0,4)),atof(PQgetvalue(PGRes,0,5)));

				strcpy(szParagraph,"Stampa Packing List Ordine");
				xncGetFileString(szParagraph,"SELECT_CMD_STOR", "", szSelectCmd, sizeof(szSelectCmd),Cfg.szCniCfg,NULL); 
				nSeparatorField = xncGetFileInt(szParagraph, "SEPARATOR_FIELD_STOR", -1, Cfg.szCniCfg, NULL);
				nRC=PrintListFromSelect(fp,szSelectCmd, szOrdKey,0,pszPrinterName,nSeparatorField);
				if (nRC==-1){
					bRetVal = FALSE;
				}
			} else {
				bRetVal = FALSE;
			}
			PQclear(PGRes);
		}
		PQclear(PGResOrdini);
	}
	/*
	* rm 29-11-2000 : gestione tagierina stampante MT 2033-S
	* configurazione stampante:
	* AUTOQTO : Tagliare al FF
	* MODULO : RIGHE : 18 (3 pollici)
	*/
	fprintf(fp,"\n\n\n\014");
	fclose(fp);

	if(bRetVal){
		/*
		* sprintf(szCommand,"pr -f -h 'CNI - Stampa Lista Ordine %s' %s | lpr -P%s",szOrdKey,szFileName,szPrinterName);
		*/
		sprintf(szCommand,"lpr -P%s %s ",pszPrinterName,szFileName);
		system(szCommand);
#ifdef TRACE
		trace_out_vstr_date(1, "Stampato Ordine Storico[%s] su %s",pszOrdProg,pszPrinterName);
#endif
	}
	return bRetVal;
}


char GetPdfFlagFromOrdProd(char *szOrdKey)
{
	PGresult *PGRes;
	int nTuple;
	char szFLPDF[128];
	char cPdfStato;
	/*
	* Setto lo stato a un default
	*/
	cPdfStato=ORDINE_SENZA_PDF;

	/*
	* Verifico l'esistenza dell'ordine nel db, il tipo di doc da stampare (F,D) e la presenza
	* del file nell'apposito direttorio.
	*/
	PGRes=PGExecSQL(Cfg.nDebugVersion>1,"select opflpdf from ord_prod where ordkey='%s';",szOrdKey);
	nTuple=PQntuples(PGRes);
	if(nTuple){
		strcpy(szFLPDF,PQgetvalue(PGRes,0,0));
		cPdfStato=szFLPDF[0];
	}else{
		#ifdef TRACE
		trace_out_vstr_date(1, "L'ordine [%s] non e' presente nella tabella ord_prod",szOrdKey);
		#endif
	}
	PQclear(PGRes);

	return cPdfStato;
}

char GetPdfFlagFromRicOrd(char *szOrdine)
{
	PGresult *PGRes;
	int nTuple;
	char szFLPDF[128];
	char cPdfStato;
	/*
	* Setto lo stato a un default
	*/
	cPdfStato=ORDINE_SENZA_PDF;

	/*
	* Verifico l'esistenza dell'ordine nel db, il tipo di doc da stampare (F,D) e la presenza
	* del file nell'apposito direttorio.
	*/
	PGRes=PGExecSQL(Cfg.nDebugVersion>1,"select roflpdf from ric_ord where ordprog='%s';",szOrdine);
	nTuple=PQntuples(PGRes);
	if(nTuple){
		strcpy(szFLPDF,PQgetvalue(PGRes,0,0));
		cPdfStato=szFLPDF[0];
	}else{
		#ifdef TRACE
		trace_out_vstr_date(1, "L'ordine [%s] non e' presente nella tabella ric_ord",szOrdine);
		#endif
	}
	PQclear(PGRes);

	return cPdfStato;
}

char GetFtzFlagFromOrdProd(char *szOrdKey)
{
	PGresult *PGRes;
	int nTuple;
	char szFLFTZ[128];
	char cFtzStato;
	/*
	* Setto lo stato a un default
	*/
	cFtzStato=ORDINE_NON_FITTIZIO;

	/*
	* Verifico l'esistenza dell'ordine nel db, il tipo di doc da stampare (F,D) e la presenza
	* del file nell'apposito direttorio.
	*/
	PGRes=PGExecSQL(Cfg.nDebugVersion>1,"select opflftz from ord_prod where ordkey='%s';",szOrdKey);
	nTuple=PQntuples(PGRes);
	if(nTuple){
		strcpy(szFLFTZ,PQgetvalue(PGRes,0,0));
		cFtzStato=szFLFTZ[0];
	}else{
		#ifdef TRACE
		trace_out_vstr_date(1, "L'ordine [%s] non e' presente nella tabella ord_prod",szOrdKey);
		#endif
	}
	PQclear(PGRes);

	return cFtzStato;
}

char GetFtzFlagFromRicOrd(char *szOrdine)
{
	PGresult *PGRes;
	int nTuple;
	char szFLFTZ[128];
	char cFtzStato;
	/*
	* Setto lo stato a un default
	*/
	cFtzStato=ORDINE_NON_FITTIZIO;

	/*
	* Verifico l'esistenza dell'ordine nel db, il tipo di doc da stampare (F,D) e la presenza
	* del file nell'apposito direttorio.
	*/
	PGRes=PGExecSQL(Cfg.nDebugVersion>1,"select roflftz from ric_ord where ordprog='%s';",szOrdine);
	nTuple=PQntuples(PGRes);
	if(nTuple){
		strcpy(szFLFTZ,PQgetvalue(PGRes,0,0));
		cFtzStato=szFLFTZ[0];
	}else{
		#ifdef TRACE
		trace_out_vstr_date(1, "L'ordine [%s] non e' presente nella tabella ric_ord",szOrdine);
		#endif
	}
	PQclear(PGRes);

	return cFtzStato;
}

