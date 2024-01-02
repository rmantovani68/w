/*
* settfun.c : gestione settori di magazzino : funzioni di gestione
* 
* Progetto Easy Picking 3.0 : wella
*
* Autore : Roberto Mantovani
*
* Copyright CNI srl 1995-99
*/
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termio.h>
#include <sys/stat.h>
#include <stdarg.h>

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


unsigned int nGetOrderTimePassed[MAX_ISOLE];
struct timeval tvOldTime[MAX_ISOLE];

/*
* void ReadConfiguration(void)
* lettura della configurazione dal file cni.cfg
*/
void ReadConfiguration(BOOL bReadProcInfo)
{
	char szParagraph[256];
	char szCurrentDirectory[256];
	char szCniCfg[256];
	char szProcIndex[256];
	char szProcBuffer[256];
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

	/* SAP */
	Cfg.nSAP = xncGetFileInt(szParagraph, "SAP", 0, szCniCfg, NULL);

	xncGetFileString(szParagraph,"Language",   "ita",         Cfg.szLanguage,     80, szCniCfg, NULL);
	xncGetFileString(szParagraph,"PathData",   "../data",     Cfg.szPathData,     80, szCniCfg, NULL);
	xncGetFileString(szParagraph,"PathExport", "../export",   Cfg.szPathExport,   80, szCniCfg, NULL);
	xncGetFileString(szParagraph,"PathExe",     "./",         Cfg.szPathExe,      80, szCniCfg, NULL);
	xncGetFileString(szParagraph,"PathExe",     "./",         Cfg.szPathExe,      80, szCniCfg, NULL);
	xncGetFileString(szParagraph,"PdfPrinterIsola1",     "lp",Cfg.szPdfPrinterIsola1,   80,szCniCfg,NULL);
	xncGetFileString(szParagraph,"PdfPrinterIsola2",     "lp",Cfg.szPdfPrinterIsola2,   80,szCniCfg,NULL);

	Cfg.nShmKey=xncGetFileInt(szParagraph,"ShmKey",1000,szCniCfg,NULL);
	Cfg.nSettoriShmKey=xncGetFileInt(szParagraph,"SettoriShmKey",1001,szCniCfg,NULL);
	Cfg.nImballiUbicati=xncGetFileInt(szParagraph,"ImballiUbicati",0,szCniCfg,NULL);
	Cfg.nChiusuraCollo=xncGetFileInt(szParagraph,"ChiusuraCollo",1,szCniCfg,NULL);
	Cfg.nFineOrdine=xncGetFileInt(szParagraph,"FineOrdine",0,szCniCfg,NULL);
	Cfg.nDebugVersion=xncGetFileInt(szParagraph,"DebugVersion",0,szCniCfg,NULL);

	if(bReadProcInfo){
		ReadProcInfo(szCniCfg);
	}

	/*
	* parametri specifici dell'applicazione
	*/
	/*
	* Delay in millisecondi
	*/
	Cfg.nDelay=xncGetFileInt(ProcList[PROC_SETTORI].szProcName,"Delay",100,szCniCfg,NULL);
	Cfg.nGetOrderDelay=xncGetFileInt(ProcList[PROC_SETTORI].szProcName,"GetOrderDelay",1000,szCniCfg,NULL);
	Cfg.nMaxDamages=xncGetFileInt(ProcList[PROC_IOS].szProcName,"MaxDamages",5,szCniCfg,NULL);

	/*
	* Numero settori componenti il magazzino
	*/
	Cfg.nNumeroSettori=xncGetFileInt(ProcList[PROC_SETTORI].szProcName,"NumeroSettori",15,szCniCfg,NULL);
	Cfg.nNumeroIsole=xncGetFileInt(ProcList[PROC_SETTORI].szProcName,"NumeroIsole",2,szCniCfg,NULL);
	/*
	* Numero displays componenti l'impianto
	*/
	Cfg.nNumeroDisplays=xncGetFileInt(ProcList[PROC_SETTORI].szProcName,"NumeroDisplays",19,szCniCfg,NULL);
	/*
	* dati relativi ai display
	*/
	for(nIndex=0;nIndex<Cfg.nNumeroDisplays;nIndex++){
		sprintf(szProcIndex,"Display_%02d",nIndex);
		xncGetFileString(ProcList[PROC_SETTORI].szProcName,szProcIndex, "", szProcBuffer, 80,szCniCfg,NULL);

		sscanf(szProcBuffer, "%d,%d,%d,%d,%d",
			&(Cfg.Displays[nIndex].nIOS),
			&(Cfg.Displays[nIndex].nCPU),
			&(Cfg.Displays[nIndex].nModulo),
			&(Cfg.Displays[nIndex].nIsola),
			&(Cfg.Displays[nIndex].nSettore));
	}
	/*
	* Numero dei moduli PKL 
	*/
	Cfg.nPKLNumber=xncGetFileInt(ProcList[PROC_IOS].szProcName,"PKLNumber",1,szCniCfg,NULL);

	/*
	* dati relativi ai moduli PKL
	*/
	for(nIndex=0; nIndex < Cfg.nPKLNumber; nIndex++){
		sprintf(szProcIndex,"PKL_%02d",nIndex);
		xncGetFileString(ProcList[PROC_IOS].szProcName,szProcIndex, "", szProcBuffer, 80,szCniCfg,NULL);

		sscanf(szProcBuffer, "%d,%d,%d",
			&(Cfg.PKL[nIndex].nIOS),
			&(Cfg.PKL[nIndex].nCPU),
			&(Cfg.PKL[nIndex].nModulo));
	}
	/*
	* lettura del file di configurazione del DataBase (PostgreSQL)
	*/
	strcpy(szParagraph,"DataBase Settings");

	xncGetFileString(szParagraph,"PGHost",    "localhost",Cfg.szPGHost,     80,szCniCfg,NULL); 
	xncGetFileString(szParagraph,"PGPort",    "5432",     Cfg.szPGPort,     80,szCniCfg,NULL); 
	xncGetFileString(szParagraph,"PGDataBase","picking",  Cfg.szPGDataBase, 80,szCniCfg,NULL); 

}

/*
* ReadStatoSettori()
* lettura della configurazione dal file cni.cfg
*/
void ReadStatoSettori(void)
{
	char szCurrentDirectory[256];
	char szCniCfg[256];
	int nIndex;
	char szFieldName[256];
	char szBuffer[256];


	/*
	* leggo il file di configurazione dalla directory corrente
	*/
	getcwd(szCurrentDirectory,sizeof(szCurrentDirectory));
	
	strcpy(szCniCfg,szCurrentDirectory);
	strcat(szCniCfg,"/cni.cfg");

	/*
	* Numero settori componenti il magazzino
	*/
	Cfg.nNumeroSettori=xncGetFileInt(ProcList[PROC_SETTORI].szProcName,"NumeroSettori",15,szCniCfg,NULL);

	/*
	* dati relativi ai settori (disabilitazione)
	*/
	for(nIndex=0;nIndex<Cfg.nNumeroSettori;nIndex++){
		sprintf(szFieldName,"Settore_%02d",nIndex);

		xncGetFileString(ProcList[PROC_SETTORI].szProcName,szFieldName, "", szBuffer, 80,szCniCfg,NULL);

		sscanf(szBuffer, "%d,%d,%d,%d",
			&(pSettori[nIndex].nIsola),
			&(pSettori[nIndex].nSettore),
			&(pSettori[nIndex].nStatoSettore),
			&(pSettori[nIndex].nTipoSettore));

		switch(pSettori[nIndex].nStatoSettore){
			case 0:
				pSettori[nIndex].nStatoSettore=DISABILITATO;
			break;
			case 1:
			default:
				pSettori[nIndex].nStatoSettore=ATTESA;
			break;
		}
	}
}

/*
* ReadProcInfo()
* Lettura dei parametri relativi ai processi 
*/
void ReadProcInfo(char *szFileConfig)
{
	char szProcIndex[256];
	char szProcBuffer[256];
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
* cerca in archivio operativo ordini il primo ordine da prelevare (in stato 'R' : Stampata RAC)
*/
BOOL GetOrdine(PORDINE pOrdine,int nIsola)
{
	PGresult *PGRes = NULL;
	int nTuples;


	//PGRes=PGExecSQL(TRUE,"select opstato,opcdisl,ordkey,opnelab,opnmcll,opnmrgh,opnmcpe,opflpdf,opflftz from ord_prod where (opstato='%c' or (opstato='%c' AND opflpdf in ('%c','%c'))) AND opcdisl=%d AND opnmcla>0  order by opprgln,ordkey;",ORDINE_ELABORATO,ORDINE_INCOMPLETO_TERMINATO_SPEDITO,ORDINE_CON_FATTURA,ORDINE_CON_DDT,nIsola);
	/*
	 * TODO
	 * Richiesta Fabio: prima finire di fare entrare gli ordini in A poi quelli in L
	 */
	PGRes=PGExecSQL(Cfg.nDebugVersion>2,"select opstato,opcdisl,ordkey,opnelab,opnmcll,opnmrgh,opnmcpe,opflpdf,opflftz from ord_prod where (opstato='%c' or (opstato='%c' AND opflpdf!='%c')) AND opcdisl=%d AND opnmcla>0  order by opprgpr,opprgln,ordkey;",ORDINE_ELABORATO,ORDINE_INCOMPLETO_TERMINATO_SPEDITO,ORDINE_IN_ATTESA_PDF,nIsola);

	nTuples=PQntuples(PGRes);
#ifdef TRACE_ELIMINATO
	trace_out_vstr_date(1,"GetOrdine(%d) : Trovati %d ordini",nIsola,nTuples);
#endif

	/*
	* A questo punto ho ottenuto 0 o piu' ordini di produzione in stato 'A' : Analizzati, o 'K' 
	* relativi all'isola di magazzino in oggetto e con almeno un collo di produzione
	*/
	if(nTuples>=1){
		strcpy(pOrdine->szSTATO,PQgetvalue(PGRes,0,0));
		strcpy(pOrdine->szORDKEY,PQgetvalue(PGRes,0,2));
		strcpy(pOrdine->szNUMELAB,PQgetvalue(PGRes,0,3));
		pOrdine->nIsola=atoi(PQgetvalue(PGRes,0,1));
		pOrdine->nColli=atoi(PQgetvalue(PGRes,0,4));
		pOrdine->nRighe=atoi(PQgetvalue(PGRes,0,5));
		pOrdine->nCopie=atoi(PQgetvalue(PGRes,0,6));
		strcpy(pOrdine->szFLPDF,PQgetvalue(PGRes,0,7));
		strcpy(pOrdine->szFLFTZ,PQgetvalue(PGRes,0,8));
	}

	PQclear(PGRes);

	return(nTuples>=1?TRUE:FALSE);
}


/*
* Operazioni di inizio prelievo nel settore
* ricerca di un ordine in attesa di prelievo e sua immissione in linea
*/
BOOL InizioPrelievoSettore(PORDINE pOrdine,PSETTORE pSettore)
{
	PGresult *PGRes;
	PDISPLAYSTRUCT pDisplay;
	int nTuples;
	int nIndex;
	int nRigaIndex;
	char cStato;

	/*
	* comincio ad elaborare l'ordine :
	* esistono prodotti da prelevare in questo settore ?
	* se si carico tutte le righe dell'ordine relative al settore in esame 
	*/

	/*
	* Query originale precedente alle modifiche pdf
	*/
	//PGRes=PGExecSQL(Cfg.nDebugVersion,"select r.ordkey,r.rpcdpro,r.rpcdubi,r.rpqtspe,r.rpprrig,r.rpnmcol,u.ios,u.cpu,u.modulo,u.riga,u.colonna,u.display,u.settore from rig_prod as r, ubicazioni as u where ordkey='%s' and u.ubicazione=r.rpcdubi and u.isola=%d order by u.priorita,r.rpnmcol,r.rpprrig;",pOrdine->szORDKEY,pSettore->nIsola);

	/*******************************************************
	*	VERSIONE CON GESTIONE PDF
	********************************************************/
	/*
	* Ora devo differenziare la select a seconda dello stato in cui si trova
	* l'ordine (pOrdine->szStato) entrato nel settore di prelievo.
	* Cio e' possibile grazie al lavoro svolto in volum per settare il flag CPFLPDF (non confondere con OPFLPDF)
	* che mi discrimina il collo a cui associare il pdf.
	* Il resto della gestione dovrebbe rimanere invariato in quanto si tratta di pure struct
	* di c che trasportano l'informazione ricavata dalla query.
	*/
	/*
	* LG: Nota che la query e' stata notevolmente appesantita. Ho aggiunto la join con
	*     la tabella col_prod. E' un problema???
	*     Inoltre vanno verificate bene!!!!
	*/
	
	cStato=pOrdine->szSTATO[0];

	switch(cStato){
		case ORDINE_IN_PRELIEVO:
		{
			/*
			* Query originale
			*/
			PGRes=PGExecSQL(Cfg.nDebugVersion,"select r.ordkey,r.rpcdpro,r.rpcdubi,r.rpqtspe,r.rpprrig,r.rpnmcol,u.ios,u.cpu,u.modulo,u.riga,u.colonna,u.display,u.settore from rig_prod as r, ubicazioni as u where ordkey='%s' and u.ubicazione=r.rpcdubi and u.isola=%d order by u.priorita,r.rpnmcol,r.rpprrig;",pOrdine->szORDKEY,pSettore->nIsola);
		}
		break;
		case ORDINE_INCOMPLETO_IN_PRELIEVO:
		{
			/*
			* Produrre tutti i colli tranne quello col PDF
			*/
			PGRes=PGExecSQL(Cfg.nDebugVersion,"select r.ordkey,r.rpcdpro,r.rpcdubi,r.rpqtspe,r.rpprrig,r.rpnmcol,u.ios,u.cpu,u.modulo,u.riga,u.colonna,u.display,u.settore from rig_prod as r,col_prod as c,ubicazioni as u where c.ordkey='%s' and c.ordkey=r.ordkey and c.cpnmcol=r.rpnmcol and cpflpdf!='%c' and u.ubicazione=r.rpcdubi and u.isola=%d order by u.priorita,r.rpnmcol,r.rpprrig;",pOrdine->szORDKEY,COLLO_CON_PDF,pSettore->nIsola);
		}
		break;
		case ORDINE_INCOMPLETO_TERMINATO_IN_PRELIEVO:
		{
			/*
			* Produrre il collo col PDF
			*/
			PGRes=PGExecSQL(Cfg.nDebugVersion,"select r.ordkey,r.rpcdpro,r.rpcdubi,r.rpqtspe,r.rpprrig,r.rpnmcol,u.ios,u.cpu,u.modulo,u.riga,u.colonna,u.display,u.settore from rig_prod as r,col_prod as c,ubicazioni as u where c.ordkey='%s' and c.ordkey=r.ordkey and c.cpnmcol=r.rpnmcol and cpflpdf='%c' and u.ubicazione=r.rpcdubi and u.isola=%d order by u.priorita,r.rpnmcol,r.rpprrig;",pOrdine->szORDKEY,COLLO_CON_PDF,pSettore->nIsola);
		}
		break;
	}
				

/**************************************************************************************************
* FINE GESTIONE PDF
***************************************************************************************************/
	

	nTuples=PQntuples(PGRes);

	/*
	* Resetto il numero copie
	*/
	nIndex=0;
	while(nIndex<nTuples){
		pSettore->RigaOrdine[nIndex].nNumCopie = 0;
		nIndex++;
	}
	/*
	* A questo punto ho ottenuto 0 o piu' righe di produzione 
	* relativi all'isola di magazzino ed al settore in oggetto 
	*/
	nIndex=0;
	nRigaIndex=0;
	while(nIndex<nTuples){
		/*
		* Se si tratta di una riga relativa al settore in esame ...
		*/
		if(pSettore->nSettore==atoi(PQgetvalue(PGRes,nIndex,12))){
			strcpy(pSettore->RigaOrdine[nRigaIndex].szCodSped, PQgetvalue(PGRes,nIndex,0));
			strcpy(pSettore->RigaOrdine[nRigaIndex].szCodProd, PQgetvalue(PGRes,nIndex,1));
			strcpy(pSettore->RigaOrdine[nRigaIndex].szCodUbi,  PQgetvalue(PGRes,nIndex,2));
			if(Cfg.nChiusuraCollo){
				pSettore->RigaOrdine[nRigaIndex].nNumCopie         = atoi(PQgetvalue(PGRes,nIndex,3));
				pSettore->RigaOrdine[nRigaIndex].nProgressivo      = atoi(PQgetvalue(PGRes,nIndex,4));
				/*
				* prelievi differenziati per collo
				*/
				pSettore->RigaOrdine[nRigaIndex].nNumCollo         = atoi(PQgetvalue(PGRes,nIndex,5));
			} else {
				pSettore->RigaOrdine[nRigaIndex].nNumCopie        += atoi(PQgetvalue(PGRes,nIndex,3));
				pSettore->RigaOrdine[nRigaIndex].nProgressivo      = atoi(PQgetvalue(PGRes,nIndex,4));
				/*
				* prelievi non differenziati per collo
				*/
				pSettore->RigaOrdine[nRigaIndex].nNumCollo         = 1;
			}
			pSettore->RigaOrdine[nRigaIndex].nIOS              = atoi(PQgetvalue(PGRes,nIndex,6));
			pSettore->RigaOrdine[nRigaIndex].nCPU              = atoi(PQgetvalue(PGRes,nIndex,7));
			pSettore->RigaOrdine[nRigaIndex].nModulo           = atoi(PQgetvalue(PGRes,nIndex,8));
			pSettore->RigaOrdine[nRigaIndex].nRiga             = atoi(PQgetvalue(PGRes,nIndex,9));
			pSettore->RigaOrdine[nRigaIndex].nColonna          = atoi(PQgetvalue(PGRes,nIndex,10));
			pSettore->RigaOrdine[nRigaIndex].nDisplay          = atoi(PQgetvalue(PGRes,nIndex,11));
			pSettore->RigaOrdine[nRigaIndex].nStato=0;

			/*
			* Assegnazione valore prossimo collo a scopo chiusura collo prima del passaggio a 
			* settore successivo
			*/
			if(Cfg.nChiusuraCollo){
				/*
				* Gestione Collo
				*/
				if(nIndex+1<nTuples){
					pSettore->RigaOrdine[nRigaIndex].nNextCollo = atoi(PQgetvalue(PGRes,nIndex+1,5));
				} else {
					pSettore->RigaOrdine[nRigaIndex].nNextCollo = -1;
				}
			} else {
				/*
				* Gestione Ordine
				*/
				if(nIndex+1<nTuples){
					pSettore->RigaOrdine[nRigaIndex].nNextCollo = 1;
				} else {
					pSettore->RigaOrdine[nRigaIndex].nNextCollo = -1;
				}
			}

			if(Cfg.nChiusuraCollo){
				nRigaIndex++;
			} else {
				if(nIndex+1<nTuples){
					if(strcmp(pSettore->RigaOrdine[nRigaIndex].szCodProd, PQgetvalue(PGRes,nIndex+1,1))){
						nRigaIndex++;
					}
				} else {
					nRigaIndex++;
				}
			}
		}
		nIndex++;
	}

	PQclear(PGRes);

	pSettore->nIndiceRigaOrdine=0;         /* puntatore alla riga d'ordine da prelevare */
	pSettore->nNumeroRigheOrdine=nRigaIndex;   /* numero delle righe d'ordine da prelevare */
	/*
	* se ci sono righe d'ordine da prelevare in questo settore
	* comincio dalla prima
	* altrimenti passo al prossimo settore
	*/
	if(nRigaIndex){
		PRIGA_PRELIEVO pRigaOrdine=&(pSettore->RigaOrdine[pSettore->nIndiceRigaOrdine]);

		/* 
		* setto lo stato della riga (ATTESA_PRELIEVO)
		*/
		pRigaOrdine->nStato=ATTESA_PRELIEVO;

		/*
		* setto i dati relativi alla riga d'ordine corrente del settore
		*/
		strcpy(pSettore->szCodSped,pRigaOrdine->szCodSped);
		pSettore->cStatoSped=cStato;
		strcpy(pSettore->szCodProd,pRigaOrdine->szCodProd);
		strcpy(pSettore->szCodUbi,pRigaOrdine->szCodUbi);
		pSettore->nNumCollo=pRigaOrdine->nNumCollo;
		pSettore->nNumCopie=pRigaOrdine->nNumCopie;
		pSettore->nIOS=pRigaOrdine->nIOS;
		pSettore->nCPU=pRigaOrdine->nCPU;
		pSettore->nModulo=pRigaOrdine->nModulo;
		pSettore->nRiga=pRigaOrdine->nRiga;
		pSettore->nColonna=pRigaOrdine->nColonna;
		pSettore->nDisplay=pRigaOrdine->nDisplay;

		pDisplay=&(Cfg.Displays[pSettore->nDisplay]);

		/*
		* scrivo i dati per il prelievo sul display
		*/
		/*
		* OrdKey : OOOOOO.I.SS
		* O : Ordine
		* I : Isola
		* S : Sottordine
		* si visualizza : OOOO.S
		*/
		strcpy(pDisplay->szRiga_1_Display,GetDisplayData(pSettore->szCodSped));
		sprintf(pDisplay->szRiga_2_Display,"P%3d",pSettore->nNumCopie);
		pDisplay->nStatoRiga1=NORMAL;
		pDisplay->nStatoRiga2=NORMAL;

		UpdateDisplay(pDisplay,TUTTO);

		/*
		* acccendo la luce di prelievo prodotto
		*/
		pSettore->nStatoLampada=1;
		SetLampada(pSettore->nIOS,pSettore->nCPU,pSettore->nModulo,pSettore->nRiga,pSettore->nColonna);
		/*
		* setto lo stato settore in attesa di conferma copie prelevate
		*/
		pSettore->nStatoSettore=PRELIEVO_COPIE;
	} else {
		if (pOrdine->szFLFTZ[0]==ORDINE_FITTIZIO){
			/*
			* non ci sono righe per questo settore essendo l'ordine fittizio
			* devo chiudere il collo e l'ordine
			* scrivo i dati per il fine ordine sul display
			*/
			pDisplay=&(Cfg.Displays[pSettore->nDisplay]);
			strcpy(pDisplay->szRiga_1_Display,GetDisplayData(pSettore->szCodSped));
			strcpy(pDisplay->szRiga_2_Display,"FFFF");
			pDisplay->nStatoRiga1=NORMAL;
			pDisplay->nStatoRiga2=BLINK;
			UpdateDisplay(pDisplay,TUTTO);

			/*
			* faccio confermare l'ultima lampada premuta 
			*/
			pSettore->nStatoLampada=1;

			SetLampada(pSettore->nIOS,pSettore->nCPU,pSettore->nModulo,pSettore->nRiga,pSettore->nColonna);

			pSettore->nStatoSettore=FINE_ORDINE;
			
		} else {
			/*
			* non ci sono righe per questo settore
			* scrivo i dati per il passaggio sul display
			*/
			pDisplay=&(Cfg.Displays[pSettore->nDisplay]);

			strcpy(pSettore->szCodSped,pOrdine->szORDKEY);
			strcpy(pDisplay->szRiga_1_Display,GetDisplayData(pSettore->szCodSped));
			strcpy(pDisplay->szRiga_2_Display,"PPPP");
			pDisplay->nStatoRiga1=NORMAL;
			pDisplay->nStatoRiga2=BLINK;
			UpdateDisplay(pDisplay,TUTTO);

			/*
			* faccio confermare l'ultima lampada premuta 
			*/
			pSettore->nStatoLampada=1;

			SetLampada(pSettore->nIOS,pSettore->nCPU,pSettore->nModulo,pSettore->nRiga,pSettore->nColonna);

			pSettore->nStatoSettore=PASSA_SETT_SUCC;
		}
	}
	return TRUE;
}

/*
* GestioneSettore()
* Gestione settore di magazzino
* logica di funzionamento del sequenziatore 
* si tratta di un automa a stati finiti
*/
BOOL GestioneSettore(EVENTO *pEvento)
{
	ORDINE Ordine;
	PDISPLAYSTRUCT pDisplay;
	PORDINE pOrdine;
	PGresult *PGRes = NULL;
	int nTuples;
	char szMsgTxt[256];
	//char szMsg[256];
	char cStato;
	char cOPFLFTZ;
	int nIndex;
	int nSettoreIndex;
	PSETTORE pSettore;
	struct timeval tvActualTime;


	if((nSettoreIndex=GetSettore(pEvento->nIsola,pEvento->nSettore))==-1){
#ifdef TRACE
		trace_out_vstr_date(1,"GestioneSettore(I:%d,S:%d) : settore non presente !",pEvento->nIsola,pEvento->nSettore);
#endif
		return FALSE;
	}
	pSettore=&pSettori[nSettoreIndex];

#ifdef TRACE_ELIMINATO
	trace_out_vstr_date(1,"GestioneSettore(I:%d,S:%d) : settore %d",pEvento->nIsola,pEvento->nSettore,nSettoreIndex);
#endif
	/*
	* caso di settore in stato di attesa 
	* e nessun evento verificatosi
	*/
	if(pSettore->nStatoSettore==ATTESA && pEvento->nEvento==-1){
		/*
		* - se si tratta del primo settore, 
		* - se la lista ordini e' vuota,
		* - se l'archivio rig_prod contiene prelievi di righe sul primo settore :
		* si mettono queste righe d'ordine in lista  
		*/
		if(pSettore->nTipoSettore==SETTORE_START){
			if((pOrdine=link_first(&(ListaOrdini[nSettoreIndex])))==NULL){
				/*
				* Gestione Timeout per get ordine (evita di ricercare un ordine sul settore di start con 
				* 'troppa' insistenza).
				*/
				gettimeofday(&tvActualTime,NULL);
				nGetOrderTimePassed[pSettore->nIsola]=(((tvActualTime.tv_sec-tvOldTime[pSettore->nIsola].tv_sec)*1000) + ((tvActualTime.tv_usec-tvOldTime[pSettore->nIsola].tv_usec)/1000));
				/*
				* Verifico se e' passato il timeout per la ricerca dell'ordine da mandare sul settore di start
				*/
				if(nGetOrderTimePassed[pSettore->nIsola] > Cfg.nGetOrderDelay){
#ifdef TRACE_ELIMINATO
					trace_out_vstr_date(1,"Settore : %d Isola : %d : Azzero nGetOrderTimePassed (%d)",pSettore->nSettore,pSettore->nIsola,nGetOrderTimePassed[pSettore->nIsola]);
#endif
					nGetOrderTimePassed[pSettore->nIsola]=0;
					gettimeofday(&(tvOldTime[pSettore->nIsola]),NULL);
				}
				if(nGetOrderTimePassed[pSettore->nIsola]==0){
					if(GetOrdine(&Ordine,pSettore->nIsola)){
					/******************************************************************************
					* INIZIO NUOVA GESTIONE PDF
					*******************************************************************************/
						/*
						* Gestione Ordine Vuoto
						* Adesso va diviso dal caso di fittizio che per sua natura è vuoto
						*/
						//if(Ordine.nColli==0 || Ordine.nRighe==0 || Ordine.nCopie==0){
						cOPFLFTZ=Ordine.szFLFTZ[0];
						if(cOPFLFTZ!=ORDINE_FITTIZIO && (Ordine.nColli==0 || Ordine.nRighe==0 || Ordine.nCopie==0)){
#ifdef TRACE
							trace_out_vstr_date(1,"Ordine [%s] VUOTO", Ordine.szORDKEY);
#endif
							sprintf(szMsgTxt,"ATTENZIONE : Isola:%d : Settore:%d : Ordine [%s][%s] VUOTO\n",
								pSettore->nIsola, pSettore->nSettore,Ordine.szNUMELAB,Ordine.szORDKEY);
							SendMessage(PROC_MAIN, PROC_SETTORI, DISPLAY_MSG, szMsgTxt);

							CambiaStatoOrdine(Ordine.szORDKEY, ORDINE_PRELEVATO);
							CambiaStatoColliOrdine(Ordine.szORDKEY,COLLO_CHIUSO);
						} else {

							/*
							* alloco un nuovo ordine da prelevare
							*/
							pOrdine=(PORDINE)malloc(sizeof(ORDINE));
							memset(pOrdine,0,sizeof(ORDINE));

							strcpy(pOrdine->szORDKEY,Ordine.szORDKEY);   /* Chiave Spedizione */
							strcpy(pOrdine->szNUMELAB,Ordine.szNUMELAB); /* Numero Elaborazione */
							strcpy(pOrdine->szFLPDF,Ordine.szFLPDF); /* Flag PDF */
							strcpy(pOrdine->szFLFTZ,Ordine.szFLFTZ); /* Flag Fittizio */
							pOrdine->nIsola=Ordine.nIsola;
							pOrdine->nColli=Ordine.nColli;
							pOrdine->nRighe=Ordine.nRighe;
							pOrdine->nCopie=Ordine.nCopie;
							strcpy(pOrdine->szSTATO,Ordine.szSTATO); /* Flag Stato */

							cStato=Ordine.szSTATO[0];

							switch(cStato){
								case ORDINE_ELABORATO:
								{
								/*
								* Ordine elaborato pronto per essere lanciato in linea
								*/
									//if((Ordine.szFLPDF[0]==ORDINE_CON_FATTURA) || (Ordine.szFLPDF[0]==ORDINE_CON_DDT) || (Ordine.szFLPDF[0]==ORDINE_SENZA_PDF)){
									if((Ordine.szFLPDF[0]==ORDINE_SENZA_PDF)){
									/*
									* L'ordine e' elaborato correttamente e ho il pdf associato.
									* 1) Faccio entrare in linea tutto l'ordine
									* 2) Stampo tutte le etichette
									* 3) Stampo anche il pdf
									*
									* Va ancora valutato il caso di ordine vuoto
									*/
#ifdef TRACE
										trace_out_vstr_date(1,"Inizio Prelievo Ordine Completo [%s][%s] su Isola %d. Documento PDF Tipo [%s] presente", Ordine.szNUMELAB, Ordine.szORDKEY,pSettore->nIsola,Ordine.szFLPDF);
#endif
										/*
										* Caso di ordine "pieno" con pdf associato o non previsto.
										* In pratica e' la condizione standard per fare le cose come prima delle modifiche
										* con in piu' la stampa del pdf
										*/
										// Cambio lo stato dell'ordine nel db e nella struttura dati
										CambiaStatoOrdine(pOrdine->szORDKEY, ORDINE_IN_PRELIEVO);
										sprintf(pOrdine->szSTATO,"%c",ORDINE_IN_PRELIEVO); 

										/*
										* Messaggio di Ordine in prelievo a MAIN
										*/
										sprintf(szMsgTxt,"%d,%d,%s,%s",pSettore->nIsola,pSettore->nSettore,pOrdine->szNUMELAB,pOrdine->szORDKEY);
										SendMessage(PROC_MAIN, PROC_SETTORI,  SETTORI_ORDINE_IN_PRELIEVO, szMsgTxt);

										/*
										* aggiungo l'ordine alla coda di questo settore
										*/
										// LG: dovro' gestire successivamente i colli da far entrare
										//     qui linko tutto l'ordine
										link_add(&(ListaOrdini[nSettoreIndex]),pOrdine);
									} else {
									/*
									* L'ordine e' elaborato correttamente e non e' senza il pdf associato.
									* 1) Faccio entrare in linea tutto l'ordine tranne l'ultimo collo
									* 2) Stampo tutte le etichette tranne l'ultima
									* 3) Non stampo il pdf
									*
									* Va ancora valutato il caso di ordine vuoto
									*/
	#ifdef TRACE
										trace_out_vstr_date(1,"Inizio Prelievo Ordine Incompleto [%s][%s] su Isola %d. Documento PDF assente", Ordine.szNUMELAB, Ordine.szORDKEY,pSettore->nIsola);
	#endif
										/*
										* Caso di ordine "pieno" senza pdf associato.
										*/
										// Cambio lo stato dell'ordine nel db e nella struttura dati
										CambiaStatoOrdine(pOrdine->szORDKEY, ORDINE_INCOMPLETO_IN_PRELIEVO);
										sprintf(pOrdine->szSTATO,"%c",ORDINE_INCOMPLETO_IN_PRELIEVO); 

										/*
										* Messaggio di Ordine Incompleto in prelievo a MAIN
										*/
										sprintf(szMsgTxt,"%d,%d,%s,%s",pSettore->nIsola,pSettore->nSettore,pOrdine->szNUMELAB,pOrdine->szORDKEY);
										SendMessage(PROC_MAIN, PROC_SETTORI,  SETTORI_ORDINE_INCOMPLETO_IN_PRELIEVO, szMsgTxt);

										/*
										* aggiungo l'ordine alla coda di questo settore
										*/
										// LG: dovro' gestire successivamente i colli da far entrare
										//     qui linko tutto l'ordine
										link_add(&(ListaOrdini[nSettoreIndex]),pOrdine);
									
									}
								/*
								* Fine lancio ordine normalmente elaborato
								*/
								}
								break;
								case ORDINE_INCOMPLETO_TERMINATO_SPEDITO:
								{
								/*
								* Ordine terminato incompleto pronto per essere lanciato in linea
								*/
									if((Ordine.szFLPDF[0]==ORDINE_CON_FATTURA) || (Ordine.szFLPDF[0]==ORDINE_CON_DDT) || (Ordine.szFLPDF[0]==ORDINE_SENZA_PDF)){
									/*
									* L'ordine e' terminato incompleto correttamente e ho il pdf associato, oppure non previsto.
									* 1) Faccio entrare in linea l'ultimo collo dell'ordine
									* 2) Stampo l'ultima etichetta piu' quella di "contiene documenti"
									* 3) Stampo anche il pdf, se previsto
									*
									* Non dovrebbe esserci il caso di ordine vuoto
									*/
#ifdef TRACE
										trace_out_vstr_date(1,"Inizio Prelievo Ordine Terminato Incompleto [%s][%s] su Isola %d. Documento PDF Tipo [%s] presente", Ordine.szNUMELAB, Ordine.szORDKEY,pSettore->nIsola,Ordine.szFLPDF);
#endif
										/*
										* Caso di ordine "pieno" terminato incompleto con pdf associato.
										*/
										// Cambio lo stato dell'ordine nel db e nella struttura dati
										CambiaStatoOrdine(pOrdine->szORDKEY, ORDINE_INCOMPLETO_TERMINATO_IN_PRELIEVO);
										sprintf(pOrdine->szSTATO,"%c",ORDINE_INCOMPLETO_TERMINATO_IN_PRELIEVO); 

										/*
										* Messaggio di Ordine in prelievo a MAIN
										*/
										sprintf(szMsgTxt,"%d,%d,%s,%s",pSettore->nIsola,pSettore->nSettore,pOrdine->szNUMELAB,pOrdine->szORDKEY);
										SendMessage(PROC_MAIN, PROC_SETTORI,  SETTORI_ORDINE_INCOMPLETO_TERMINATO_IN_PRELIEVO, szMsgTxt);

										/*
										* aggiungo l'ordine alla coda di questo settore
										*/
										// LG: dovro' gestire successivamente i colli da far entrare
										//     qui linko tutto l'ordine
										link_add(&(ListaOrdini[nSettoreIndex]),pOrdine);
									} else {
									/*
									* L'ordine e' Terminato Incompleto correttamente e NON ho il pdf associato.
									* 1) Non faccio entrare in linea l'ultimo collo
									* 2) dunque non stampo l'ultima
									* 3) e non stampo il pdf
									*
									* Va ancora valutato il caso di ordine vuoto
									*/
									}
								/*
								* Fine lancio ordine terminato incompleto
								*/
								}
								break;
							}
						
						}
						
					/******************************************************************************
					* FINE NUOVA GESTIONE PDF
					*******************************************************************************/
					
					}
				}
			}
		}

		/*
		* caso valido per tutti i settori, compreso il primo:
		* se la coda ordini e' vuota non faccio niente
		* altrimenti passo alla elaborazione del primo ordine in coda
		*/
		if((pOrdine=link_first(&(ListaOrdini[nSettoreIndex])))!=NULL){

		
			if(pSettore->nTipoSettore==SETTORE_START && Cfg.nImballiUbicati==1){
				PIMBALLO_PRELIEVO pImballo;
				/*
				* Se si tratta di un settore di start e gestisco
				* gli imballi a ubicazione
				* passo al prelievo imballi e stampa etichetta per i colli AUTOMATICI
				* altrimenti passo alla gestione righe di prelievo
				*/
				/*
				PGRes=PGExecSQL(FALSE,"select c.ordprog,c.ordkey,c.cpnmcol,c.cptpfor,u.ios,u.cpu,u.modulo,u.riga,u.colonna,u.display,u.settore from col_prod as c, ubicazioni as u where ordkey='%s' and c.cpswlin!='1' and u.codprod=c.cptpfor and u.isola=%d order by c.cpnmcol;",pOrdine->szORDKEY,pSettore->nIsola);
				*/

				/*
				* Vecchia query usata prima della gestione pdf
				*/
				//PGRes=PGExecSQL(Cfg.nDebugVersion,"select c.ordprog,c.ordkey,c.cptpfor,count(c.cptpfor),u.ios,u.cpu,u.modulo,u.riga,u.colonna,u.display,u.settore from col_prod as c, ubicazioni as u where ordkey='%s' and c.cpswlin!='1' and u.codprod=c.cptpfor and u.isola=%d group by c.cptpfor,c.ordprog,c.ordkey,u.ios,u.cpu,u.modulo,u.riga,u.colonna,u.display,u.settore,u.priorita order by u.priorita;",pOrdine->szORDKEY,pSettore->nIsola);
				
/**************************************************************************************************
* GESTIONE PDF
***************************************************************************************************/
				/*
				* Ora devo differenziare la select a seconda dello stato in cui si trova
				* l'ordine (pOrdine->szStato) entrato nel settore imballi.
				* Cio e' possibile grazie al lavoro svolto in volum per settare il flag CPFLPDF (non confondere con OPFLPDF)
				* che mi discrimina il collo a cui associare il pdf.
				* Il resto della gestione dovrebbe rimanere invariato in quanto si tratta di pure struct
				* di c che trasportano l'informazione ricavata dalla query.
				* Chi e' che stampa???
				* Se una delle query restituisce zero tuple?
				* Ma soprattutto, il pdf e' associato a ordprog o ordkey? All'ordine o al sott'ordine???
				*/
				cStato=pOrdine->szSTATO[0];

				switch(cStato){
					case ORDINE_IN_PRELIEVO:
					{
						/*
						* Query originale
						*/
						PGRes=PGExecSQL(Cfg.nDebugVersion,"select c.ordprog,c.ordkey,c.cptpfor,count(c.cptpfor),u.ios,u.cpu,u.modulo,u.riga,u.colonna,u.display,u.settore from col_prod as c, ubicazioni as u where ordkey='%s' and c.cpswlin!='1' and u.codprod=c.cptpfor and u.isola=%d group by c.cptpfor,c.ordprog,c.ordkey,u.ios,u.cpu,u.modulo,u.riga,u.colonna,u.display,u.settore,u.priorita order by u.priorita;",pOrdine->szORDKEY,pSettore->nIsola);
					}
					break;
					case ORDINE_INCOMPLETO_IN_PRELIEVO:
					{
						/*
						* Produrre tutti gli imballi tranne quello col PDF
						*/
						PGRes=PGExecSQL(Cfg.nDebugVersion,"select c.ordprog,c.ordkey,c.cptpfor,count(c.cptpfor),u.ios,u.cpu,u.modulo,u.riga,u.colonna,u.display,u.settore from col_prod as c, ubicazioni as u where ordkey='%s' and c.cpswlin!='1' and cpflpdf!='%c' and u.codprod=c.cptpfor and u.isola=%d group by c.cptpfor,c.ordprog,c.ordkey,u.ios,u.cpu,u.modulo,u.riga,u.colonna,u.display,u.settore,u.priorita order by u.priorita;",pOrdine->szORDKEY,COLLO_CON_PDF,pSettore->nIsola);
					}
					break;
					case ORDINE_INCOMPLETO_TERMINATO_IN_PRELIEVO:
					{
						/*
						* Produrre l'imballo col PDF
						*/
						PGRes=PGExecSQL(Cfg.nDebugVersion,"select c.ordprog,c.ordkey,c.cptpfor,count(c.cptpfor),u.ios,u.cpu,u.modulo,u.riga,u.colonna,u.display,u.settore from col_prod as c, ubicazioni as u where ordkey='%s' and c.cpswlin!='1' and cpflpdf='%c' and u.codprod=c.cptpfor and u.isola=%d group by c.cptpfor,c.ordprog,c.ordkey,u.ios,u.cpu,u.modulo,u.riga,u.colonna,u.display,u.settore,u.priorita order by u.priorita;",pOrdine->szORDKEY,COLLO_CON_PDF,pSettore->nIsola);
					}
					break;
				}
				

/**************************************************************************************************
* FINE GESTIONE PDF
***************************************************************************************************/

				nTuples=PQntuples(PGRes);
				for(nIndex=0;nIndex<nTuples;nIndex++){
					pImballo=&(pSettore->Imballo[nIndex]);
					strcpy(pImballo->szOrdProg,   PQgetvalue(PGRes,nIndex,0));
					strcpy(pImballo->szOrdKey,    PQgetvalue(PGRes,nIndex,1));
					strcpy(pImballo->szFormato,   PQgetvalue(PGRes,nIndex,2));
					pImballo->nNumCollo    = atoi(PQgetvalue(PGRes,nIndex,3));
					pImballo->nIOS         = atoi(PQgetvalue(PGRes,nIndex,4));
					pImballo->nCPU         = atoi(PQgetvalue(PGRes,nIndex,5));
					pImballo->nModulo      = atoi(PQgetvalue(PGRes,nIndex,6));
					pImballo->nRiga        = atoi(PQgetvalue(PGRes,nIndex,7));
					pImballo->nColonna     = atoi(PQgetvalue(PGRes,nIndex,8));
					pImballo->nDisplay     = atoi(PQgetvalue(PGRes,nIndex,9));
				}
				PQclear(PGRes);

				pSettore->nIndiceImballo=0;
				pSettore->nNumeroImballi=nIndex;
				pImballo=&(pSettore->Imballo[pSettore->nIndiceImballo]);

				strcpy(pSettore->szCodSped,pImballo->szOrdKey);
				pSettore->nNumCollo = pImballo->nNumCollo;
				pSettore->nIOS      = pImballo->nIOS;
				pSettore->nCPU      = pImballo->nCPU;
				pSettore->nModulo   = pImballo->nModulo;
				pSettore->nRiga     = pImballo->nRiga;
				pSettore->nColonna  = pImballo->nColonna;
				pSettore->nDisplay  = pImballo->nDisplay;

				/*
				* scrivo i dati per il prelievo imballi sul display
				*/
				pDisplay=&(Cfg.Displays[pSettore->nDisplay]);
				strcpy(pDisplay->szRiga_1_Display,GetDisplayData(pSettore->szCodSped));
				sprintf(pDisplay->szRiga_2_Display,"I%3d",pSettore->nNumCollo);
				pDisplay->nStatoRiga1=NORMAL;
				pDisplay->nStatoRiga2=NORMAL;

				UpdateDisplay(pDisplay,TUTTO);

				/*
				* acccendo la luce di prelievo prodotto
				*/
				pSettore->nStatoLampada=1;
				SetLampada(pSettore->nIOS,pSettore->nCPU,pSettore->nModulo,pSettore->nRiga,pSettore->nColonna);
				/*
				* setto lo stato settore in attesa di conferma copie prelevate
				*/
				pSettore->nStatoSettore=PRELIEVO_IMBALLI;
			} else {
				InizioPrelievoSettore(pOrdine,pSettore);
			}
		}
	}

	switch(pEvento->nEvento){
		case IOS_PKL_BUTTON_PRESSED:
			switch(pSettore->nStatoSettore){
				case PRELIEVO_IMBALLI:
				{
					PIMBALLO_PRELIEVO pImballo;
					/*
					* sono in attesa prelievo imballo
					* e' stato premuto il tasto di prelievo effettuato (riga/colonna giuste)
					* - se si tratta dell'ultimo imballo passo al prelievo righe
					*/
					/*
					* controllo che la riga/colonna siano quelle giuste
					*/
					switch(pEvento->nEvento){
						case IOS_PKL_BUTTON_PRESSED:
						{
							if(pEvento->nModulo==pSettore->nModulo && pEvento->nRiga==pSettore->nRiga && pEvento->nColonna==pSettore->nColonna){

								/* reset lampada */
								pSettore->nStatoLampada=0;

								if(pSettore->nIndiceImballo+1<pSettore->nNumeroImballi){
									/*
									* STAMPA ETICHETTA IMBALLO ATTUALE e 
									* ----------------------------------
									* Prelievo imballo successivo
									*/
									/*
									* LG: non e' qui che viene fatta la stampa etichetta, ma viene spedido un
									* messaggio a main piu' avanti. A sua volta main spedira a printlabel.
									*/
									pSettore->nIndiceImballo++;
									pImballo=&(pSettore->Imballo[pSettore->nIndiceImballo]);

									strcpy(pSettore->szCodSped,pImballo->szOrdKey);
									pSettore->nNumCollo=pImballo->nNumCollo;
									pSettore->nIOS=pImballo->nIOS;
									pSettore->nCPU=pImballo->nCPU;
									pSettore->nModulo=pImballo->nModulo;
									pSettore->nRiga=pImballo->nRiga;
									pSettore->nColonna=pImballo->nColonna;
									pSettore->nDisplay=pImballo->nDisplay;

									/*
									* scrivo i dati per il prelievo imballo sul display
									*/
									pDisplay=&(Cfg.Displays[pSettore->nDisplay]);
									strcpy(pDisplay->szRiga_1_Display,GetDisplayData(pSettore->szCodSped));
									sprintf(pDisplay->szRiga_2_Display,"I%3d",pSettore->nNumCollo);
									pDisplay->nStatoRiga1=NORMAL;
									pDisplay->nStatoRiga2=NORMAL;

									UpdateDisplay(pDisplay,TUTTO);

									/*
									* acccendo la luce di prelievo prodotto
									*/
									pSettore->nStatoLampada=1;
									SetLampada(pSettore->nIOS,pSettore->nCPU,pSettore->nModulo,pSettore->nRiga,pSettore->nColonna);
									/*
									* setto lo stato settore in attesa di conferma copie prelevate
									*/
									pSettore->nStatoSettore=PRELIEVO_IMBALLI;
								} else {
									/*
									* scrivo i dati per il prelievo imballo sul display
									*/
									pDisplay=&(Cfg.Displays[pSettore->nDisplay]);
									strcpy(pDisplay->szRiga_1_Display,"    ");
									strcpy(pDisplay->szRiga_2_Display,"    ");
									pDisplay->nStatoRiga1=NORMAL;
									pDisplay->nStatoRiga2=NORMAL;

									UpdateDisplay(pDisplay,TUTTO);
									/*
									* Fine Prelievo imballi
									*/
									if((pOrdine=link_first(&(ListaOrdini[nSettoreIndex])))!=NULL){
										InizioPrelievoSettore(pOrdine,pSettore);
									}
								}
							}
						}
						break;
					}
				}
				break;
				case PRELIEVO_COPIE:
				{
					/*
					* sono in attesa prelievo
					* e' stato premuto il tasto di prelievo effettuato (riga/colonna giuste)
					* - se si tratta dell'ultima riga del collo chiudo il collo
					* - se ci sono ancora righe in questo settore vado avanti nel prelievo
					* - se ci sono altre righe nei settori successivi passo il collo al settore successivo 
					*/
					/*
					* controllo che la riga/colonna siano quelle giuste
					*/
					switch(pEvento->nEvento){
						case IOS_PKL_BUTTON_PRESSED:
						{
							PRIGA_PRELIEVO pOldRigaOrdine=&(pSettore->RigaOrdine[pSettore->nIndiceRigaOrdine]);
							PRIGA_PRELIEVO pRigaOrdine=pOldRigaOrdine;

							if(pEvento->nModulo==pSettore->nModulo && pEvento->nRiga==pSettore->nRiga && pEvento->nColonna==pSettore->nColonna){

								/* reset lampada */
								pSettore->nStatoLampada=0;
								/*
								* prelievo confermato
								* setto lo stato della riga (PRELEVATA)
								* passo alla prossima riga da prelevare (se c'e')
								* se il collo e' diverso allora chiudo il collo
								*/
								pRigaOrdine->nStato=PRELEVATA;

								/*
								* Aggiorna i dati della riga ordine in oggetto con il codice operatore, data e ora prelievo e stato
								*/
								UpdateRigaOrdine(pRigaOrdine,pSettore);

								if(pSettore->nIndiceRigaOrdine==pSettore->nNumeroRigheOrdine-1){
									if(!Cfg.nChiusuraCollo && pRigaOrdine->nNextCollo==-1){
										/* Fine Ordine */
										/*
										* scrivo i dati per il fine ordine sul display
										*/
										pDisplay=&(Cfg.Displays[pSettore->nDisplay]);
										strcpy(pDisplay->szRiga_1_Display,GetDisplayData(pSettore->szCodSped));
										strcpy(pDisplay->szRiga_2_Display,"FFFF");
										pDisplay->nStatoRiga1=NORMAL;
										pDisplay->nStatoRiga2=BLINK;
										UpdateDisplay(pDisplay,TUTTO);

										/*
										* faccio confermare l'ultima lampada premuta 
										*/
										pSettore->nStatoLampada=1;

										SetLampada(pSettore->nIOS,pSettore->nCPU,pSettore->nModulo,pSettore->nRiga,pSettore->nColonna);

										pSettore->nStatoSettore=FINE_ORDINE;
									} else if(!Cfg.nChiusuraCollo || pOldRigaOrdine->nNumCollo==pOldRigaOrdine->nNextCollo){
										/*
										* finite le righe d'ordine
										* se il prossimo collo e' uguale a quello attuale
										* allora faccio il passaggio a settore successivo
										* altrimenti faccio la chiusura collo
										*/

										/*
										* scrivo i dati per il passaggio sul display
										*/
										pDisplay=&(Cfg.Displays[pSettore->nDisplay]);

										strcpy(pDisplay->szRiga_1_Display,GetDisplayData(pSettore->szCodSped));
										strcpy(pDisplay->szRiga_2_Display,"PPPP");
										pDisplay->nStatoRiga1=NORMAL;
										pDisplay->nStatoRiga2=BLINK;
										UpdateDisplay(pDisplay,TUTTO);

										/*
										* faccio confermare l'ultima lampada premuta 
										*/
										pSettore->nStatoLampada=1;

										SetLampada(pSettore->nIOS,pSettore->nCPU,pSettore->nModulo,pSettore->nRiga,pSettore->nColonna);

										pSettore->nStatoSettore=PASSA_SETT_SUCC;
									} else {
										/*
										* chiudo il collo se in gestione collo e non ordine (rif.Wella)
										*/
										/*
										* scrivo i dati per il passaggio sul display
										*/
										pDisplay=&(Cfg.Displays[pSettore->nDisplay]);

										strcpy(pDisplay->szRiga_1_Display,GetDisplayData(pSettore->szCodSped));
										pDisplay->nStatoRiga1=NORMAL;
										pDisplay->nStatoRiga2=BLINK;

										/*
										* faccio confermare l'ultima lampada premuta 
										*/
										pSettore->nStatoLampada=1;

										SetLampada(pSettore->nIOS,pSettore->nCPU,pSettore->nModulo,pSettore->nRiga,pSettore->nColonna);


										if(pRigaOrdine->nNextCollo==-1){
											/*
											* LG: non si puo' piu' chiudere l'ordine con certezza, in verita'
											* dipende dallo stato.
											*/
											// Vecchio codice
											/* ultimo collo - fine ordine */
											//strcpy(pDisplay->szRiga_2_Display,"FFFF");
											//pSettore->nStatoSettore=FINE_ORDINE;
/**************************************************************************************************
* GESTIONE PDF
***************************************************************************************************/
											/*
											* Ora devo differenziare la visualizzazione sul display
											* del settore a seconda dello stato in cui si trova
											* l'ordine entrato nel settore.
											*/
											cStato=pSettore->cStatoSped;

											switch(cStato){
												case ORDINE_IN_PRELIEVO:
												{
													/*
													* Codice originale
													*/
													strcpy(pDisplay->szRiga_2_Display,"FFFF");
												}
												break;
												case ORDINE_INCOMPLETO_IN_PRELIEVO:
												{
													/*
													* 
													*/
													strcpy(pDisplay->szRiga_2_Display,"FFFF");
													//strcpy(pDisplay->szRiga_2_Display,"FFUU");
												}
												break;
												case ORDINE_INCOMPLETO_TERMINATO_IN_PRELIEVO:
												{
													/*
													* Produrre l'imballo col PDF
													*/
													strcpy(pDisplay->szRiga_2_Display,"FFFF");
												}
												break;
											}
				

/**************************************************************************************************
* FINE GESTIONE PDF
***************************************************************************************************/
											/*
											* LG: dovro' distinguere piu' avanti che tipo di ordine finire!!!
											*/
											pSettore->nStatoSettore=FINE_ORDINE;
										} else {
											/* collo normale - chiusura collo */
											sprintf(pDisplay->szRiga_2_Display,"C%3d", pOldRigaOrdine->nNumCollo);
											pSettore->nStatoSettore=CHIUSURA_COLLO;
										}
										UpdateDisplay(pDisplay,TUTTO);
									}
								} else {
									/*
									* ci sono ancora righe d'ordine da fare
									* controllo prima che non ci sia da chiudere il collo
									*/
									if(!Cfg.nChiusuraCollo || pOldRigaOrdine->nNumCollo==pOldRigaOrdine->nNextCollo){
										pSettore->nIndiceRigaOrdine++;
										pRigaOrdine=&(pSettore->RigaOrdine[pSettore->nIndiceRigaOrdine]);
										/*
										* il collo e' lo stesso : faccio il prelievo
										*/
										/* 
										* setto lo stato della riga (ATTESA_PRELIEVO)
										*/
										pRigaOrdine->nStato=ATTESA_PRELIEVO;

										/*
										* setto i dati della riga corrente del settore
										*/
										/*
										* LG: anche se non copio CodSped e' lo stesso?
										*     Non dovrebbe essere cambiato l'ordine... e' importante per
										*     lo stato dell'ordine pSettore->cStatoSped, che non dovrebbe essere
										*     cambiato
										*/
										strcpy(pSettore->szCodSped,pRigaOrdine->szCodSped);
										strcpy(pSettore->szCodProd,pRigaOrdine->szCodProd);
										strcpy(pSettore->szCodUbi,pRigaOrdine->szCodUbi);
										pSettore->nNumCollo=pRigaOrdine->nNumCollo;
										pSettore->nNumCopie=pRigaOrdine->nNumCopie;
										pSettore->nModulo=pRigaOrdine->nModulo;
										pSettore->nRiga=pRigaOrdine->nRiga;
										pSettore->nColonna=pRigaOrdine->nColonna;
										if(pSettore->nDisplay!=pRigaOrdine->nDisplay){
											/*
											* Pulisco il display
											*/
											pDisplay=&(Cfg.Displays[pSettore->nDisplay]);
											strcpy(pDisplay->szRiga_1_Display,"    ");
											strcpy(pDisplay->szRiga_2_Display,"    ");
											pDisplay->nStatoRiga1=NORMAL;
											pDisplay->nStatoRiga2=NORMAL;
											UpdateDisplay(pDisplay,TUTTO);
										}
										pSettore->nDisplay=pRigaOrdine->nDisplay;
										pDisplay=&(Cfg.Displays[pSettore->nDisplay]);
										/*
										* scrivo i dati per il prelievo sul display
										*/
										strcpy(pDisplay->szRiga_1_Display,GetDisplayData(pSettore->szCodSped));
										sprintf(pDisplay->szRiga_2_Display,"P%3d",pSettore->nNumCopie);
										pDisplay->nStatoRiga1=NORMAL;
										pDisplay->nStatoRiga2=NORMAL;
										UpdateDisplay(pDisplay,TUTTO);
										/*
										* acccendo la luce di prelievo prodotto
										*/
										pSettore->nStatoLampada=1;
										SetLampada(pSettore->nIOS,pSettore->nCPU,pSettore->nModulo,pSettore->nRiga,pSettore->nColonna);
										/*
										* setto lo stato settore in attesa di conferma copie prelevate
										*/
										pSettore->nStatoSettore=PRELIEVO_COPIE;
									} else {
										/*
										* il collo e' diverso : faccio la chiusura collo
										*/
										/*
										* scrivo i dati per la chiusura sul display
										*/
										pDisplay=&(Cfg.Displays[pSettore->nDisplay]);
										strcpy(pDisplay->szRiga_1_Display,GetDisplayData(pSettore->szCodSped));
										sprintf(pDisplay->szRiga_2_Display,"C%3d", pOldRigaOrdine->nNumCollo);
										pDisplay->nStatoRiga1=NORMAL;
										pDisplay->nStatoRiga2=BLINK;

										/*
										* faccio confermare l'ultima lampada premuta 
										*/
										pSettore->nStatoLampada=1;

										SetLampada(pSettore->nIOS,pSettore->nCPU,pSettore->nModulo,pSettore->nRiga,pSettore->nColonna);

										pSettore->nStatoSettore=CHIUSURA_COLLO;
										
										UpdateDisplay(pDisplay,TUTTO);
									}
								}
							}
						}
						break;
					}
				}
				break;
				case PASSA_SETT_SUCC:
				{
					/*
					* dopo conferma operatore
					* infilo l'ordine in coda al primo settore abilitato successivo al presente
					* e mi rimetto in attesa prossimo ordine in coda
					*/
					/*
					* sono in attesa di conferma passaggio a settore successivo
					* e' stato premuto il tasto di conferma passaggio (riga/colonna giuste)
					* metto l'ordine in coda al settore successivo e
					* resetto il display e passo allo stato di attesa
					*/
					/*
					* controllo che la riga/colonna siano quelle giuste
					*/
					switch(pEvento->nEvento){
						case IOS_PKL_BUTTON_PRESSED:
							if(pEvento->nModulo==pSettore->nModulo && pEvento->nRiga==pSettore->nRiga && pEvento->nColonna==pSettore->nColonna){

								/* reset lampada */
								pSettore->nStatoLampada=0;

								pDisplay=&(Cfg.Displays[pSettore->nDisplay]);

								strcpy(pDisplay->szRiga_1_Display,"    ");
								strcpy(pDisplay->szRiga_2_Display,"    ");
								pDisplay->nStatoRiga1=NORMAL;
								pDisplay->nStatoRiga2=NORMAL;
								UpdateDisplay(pDisplay,TUTTO);

								/*
								* passaggio ordine confermato
								*/
								pSettore->szCodSped[0]='\0';
								/*
								* LG: aggiunto il reset dello stato dell'ordine
								*/
								pSettore->cStatoSped='\0';
								pSettore->szCodProd[0]='\0';
								pSettore->szCodUbi[0]='\0';
								pSettore->nNumCollo=0;
								pSettore->nNumCopie=0;
								pSettore->nIOS=pSettore->nFirstIOS;
								pSettore->nCPU=pSettore->nFirstCPU;
								pSettore->nModulo=pSettore->nFirstMod;
								pSettore->nRiga=pSettore->nFirstRow;
								pSettore->nColonna=pSettore->nFirstColumn;
								pSettore->nDisplay=pSettore->nFirstDisplay;
								pSettore->nIndiceRigaOrdine=0;
								pSettore->nNumeroRigheOrdine=0;

								if(pSettore->nTipoSettore==SETTORE_END){
									/*
									* rimuovo il primo ordine dalla coda
									*/
									if((pOrdine=link_first(&(ListaOrdini[nSettoreIndex])))!=NULL){

										link_remove(&(ListaOrdini[nSettoreIndex]),pOrdine);
										free(pOrdine);
									}
								} else {
									/*
									* metto il primo ordine in coda al settore successivo
									* e lo rimuovo dalla coda del settore attuale
									*/
									if((pOrdine=link_first(&(ListaOrdini[nSettoreIndex])))!=NULL){
										sprintf(szMsgTxt,"%d,%d,%s,%s",pSettore->nIsola,pSettore->nSettore,pOrdine->szNUMELAB,pOrdine->szORDKEY);

										/*
										* Messaggio di Ordine Settore Successivo a MAIN
										*/
										SendMessage(PROC_MAIN, PROC_SETTORI,  SETTORI_ORDINE_SETT_SUCC, szMsgTxt);
										/*
										* rimuovo l'ordine dal settore attuale
										*/
										link_remove(&(ListaOrdini[nSettoreIndex]),pOrdine);
										/*
										* aggiungo l'ordine al primo settore abilitato successivo al presente
										*/
										/*
										* DA SISTEMARE ... GESTIRE I SETTORI PER ISOLA !!!!
										*/
										while(pSettori[nSettoreIndex].nIsola==pSettore->nIsola && pSettori[nSettoreIndex].nStatoSettore==DISABILITATO){
											nSettoreIndex++;
										}
#ifdef TRACE
										trace_out_vstr_date(1,"Passo Ordine [%s] a Settore [%2d]", pOrdine->szORDKEY,nSettoreIndex);
#endif

										link_add(&(ListaOrdini[nSettoreIndex+1]),pOrdine);
									}
								}

								pSettore->nStatoSettore=ATTESA;
							}
						break;
					}
				}
				break;
				case CHIUSURA_COLLO:
				{
					/*
					* sono in attesa di chiusura collo
					* e' stato premuto il tasto di chiusura effettuata (riga/colonna giuste)
					* - se ci sono altre righe in questo settore: 
					*   - prelievo copie
					* - se si tratta dell'ultima riga del collo:
					*   - se si tratta dell'ultimo collo faccio fine ordine
					*   - se c'e' un altro collo passo al settore successivo
					*/
					/*
					* controllo che la riga/colonna siano quelle giuste
					*/
					switch(pEvento->nEvento){
						case IOS_PKL_BUTTON_PRESSED:
						{
							PRIGA_PRELIEVO pOldRigaOrdine=&(pSettore->RigaOrdine[pSettore->nIndiceRigaOrdine]);
							PRIGA_PRELIEVO pRigaOrdine;

							if(pEvento->nIOS==pSettore->nIOS && 
								pEvento->nCPU==pSettore->nCPU && 
								pEvento->nModulo==pSettore->nModulo && 
								pEvento->nRiga==pSettore->nRiga && 
								pEvento->nColonna==pSettore->nColonna){

								/* reset lampada */
								pSettore->nStatoLampada=0;
								/*
								* chiusura collo confermata
								*/
								if((pOrdine=link_first(&(ListaOrdini[nSettoreIndex])))!=NULL){
									sprintf(szMsgTxt,"%d,%d,%s,%s,%d", pSettore->nIsola, pSettore->nSettore, pOrdine->szNUMELAB,pOrdine->szORDKEY,pSettore->nNumCollo);
									/*
									* Messaggio di Collo Chiuso a MAIN
									*/
									SendMessage(PROC_MAIN, PROC_SETTORI,  SETTORI_ORDINE_COLLO_PRELEVATO, szMsgTxt);

									CambiaStatoCollo(pOrdine->szORDKEY,pSettore->nNumCollo,COLLO_CHIUSO);
								}
								/*
								* passo alla prossima riga da prelevare (se c'e')
								*/
								if(pSettore->nIndiceRigaOrdine==pSettore->nNumeroRigheOrdine-1){
									/*
									* non ci sono piu' righe da prelevare in questo settore;
									* visto che sono in conferma chiusura collo questo significa che :
									*  - se il prossimo collo ESISTE verra' iniziato da uno dei prossimi settori
									*    e quindi faccio il passaggio a settore successivo
									*  - se il prossimo collo NON ESISTE passo a Fine Ordine
									*
									*/
									if(pOldRigaOrdine->nNextCollo==-1){
										/* Fine Ordine */
										/*
										* scrivo i dati per il fine ordine sul display
										*/
										/*
										* Vecchio codice
										*/
//										pDisplay=&(Cfg.Displays[pSettore->nDisplay]);
//										strcpy(pDisplay->szRiga_1_Display,GetDisplayData(pSettore->szCodSped));
//										strcpy(pDisplay->szRiga_2_Display,"FFFF");
//										pDisplay->nStatoRiga1=NORMAL;
//										pDisplay->nStatoRiga2=BLINK;
//										UpdateDisplay(pDisplay,TUTTO);
//
//										/*
//										* faccio confermare l'ultima lampada premuta 
//										*/
//										pSettore->nStatoLampada=1;
//
//										SetLampada(pSettore->nIOS,pSettore->nCPU,pSettore->nModulo,pSettore->nRiga,pSettore->nColonna);
//
//										pSettore->nStatoSettore=FINE_ORDINE;
										
/**************************************************************************************************
* GESTIONE PDF
***************************************************************************************************/
										/*
										* Ora devo differenziare la visualizzazione sul display 
										* del settore a seconda dello stato in cui si trova
										* l'ordine entrato nel settore.
										*/
										cStato=pSettore->cStatoSped;

										switch(cStato){
											case ORDINE_IN_PRELIEVO:
											{
												/*
												* Codice originale
												*/
												strcpy(pDisplay->szRiga_2_Display,"FFFF");
											}
											break;
											case ORDINE_INCOMPLETO_IN_PRELIEVO:
											{
												/*
												* 
												*/
												strcpy(pDisplay->szRiga_2_Display,"FFFF");
											}
											break;
											case ORDINE_INCOMPLETO_TERMINATO_IN_PRELIEVO:
											{
												/*
												* 
												*/
												strcpy(pDisplay->szRiga_2_Display,"FFFF");
											}
											break;
										}
				

/**************************************************************************************************
* FINE GESTIONE PDF
***************************************************************************************************/

										pDisplay=&(Cfg.Displays[pSettore->nDisplay]);
										strcpy(pDisplay->szRiga_1_Display,GetDisplayData(pSettore->szCodSped));
										pDisplay->nStatoRiga1=NORMAL;
										pDisplay->nStatoRiga2=BLINK;
										UpdateDisplay(pDisplay,TUTTO);
										/*
										* faccio confermare l'ultima lampada premuta 
										*/
										pSettore->nStatoLampada=1;
										SetLampada(pSettore->nIOS,pSettore->nCPU,pSettore->nModulo,pSettore->nRiga,pSettore->nColonna);
										/*
										* LG: dovro' distinguere piu' avanti che tipo di ordine finire!!!
										*/
										pSettore->nStatoSettore=FINE_ORDINE;

									} else {
										/* Passa a Settore Successivo */
										/*
										* scrivo i dati per il passaggio sul display
										*/
										pDisplay=&(Cfg.Displays[pSettore->nDisplay]);
										strcpy(pDisplay->szRiga_1_Display,GetDisplayData(pSettore->szCodSped));
										strcpy(pDisplay->szRiga_2_Display,"PPPP");
										pDisplay->nStatoRiga1=NORMAL;
										pDisplay->nStatoRiga2=BLINK;
										UpdateDisplay(pDisplay,TUTTO);


										/*
										* faccio confermare l'ultima lampada premuta 
										*/
										pSettore->nStatoLampada=1;

										SetLampada(pSettore->nIOS,pSettore->nCPU,pSettore->nModulo,pSettore->nRiga,pSettore->nColonna);

										pSettore->nStatoSettore=PASSA_SETT_SUCC;
									}
								} else {
									/*
									* ci sono altre righe da prelevare in questo settore
									*/
									pSettore->nIndiceRigaOrdine++;
									pRigaOrdine=&(pSettore->RigaOrdine[pSettore->nIndiceRigaOrdine]);
									/*
									* il collo e' lo stesso : faccio il prelievo
									*/
									/* 
									* setto lo stato della riga (ATTESA_PRELIEVO)
									*/
									pRigaOrdine->nStato=ATTESA_PRELIEVO;

									/*
									* setto i dati di lavoro del settore
									*/
									strcpy(pSettore->szCodSped,pRigaOrdine->szCodSped);
									strcpy(pSettore->szCodProd,pRigaOrdine->szCodProd);
									strcpy(pSettore->szCodUbi,pRigaOrdine->szCodUbi);
									pSettore->nNumCollo=pRigaOrdine->nNumCollo;
									pSettore->nNumCopie=pRigaOrdine->nNumCopie;
									pSettore->nIOS=pRigaOrdine->nIOS;
									pSettore->nCPU=pRigaOrdine->nCPU;
									pSettore->nModulo=pRigaOrdine->nModulo;
									pSettore->nRiga=pRigaOrdine->nRiga;
									pSettore->nColonna=pRigaOrdine->nColonna;
									if(pSettore->nDisplay!=pRigaOrdine->nDisplay){
										/*
										* Pulisco il display
										*/
										pDisplay=&(Cfg.Displays[pSettore->nDisplay]);
										strcpy(pDisplay->szRiga_1_Display,"    ");
										strcpy(pDisplay->szRiga_2_Display,"    ");
										pDisplay->nStatoRiga1=NORMAL;
										pDisplay->nStatoRiga2=NORMAL;
										UpdateDisplay(pDisplay,TUTTO);
									}
									pSettore->nDisplay=pRigaOrdine->nDisplay;
									/*
									* scrivo i dati per il prelievo sul display
									*/
									pDisplay=&(Cfg.Displays[pSettore->nDisplay]);
									strcpy(pDisplay->szRiga_1_Display,GetDisplayData(pSettore->szCodSped));
									sprintf(pDisplay->szRiga_2_Display,"P%3d",pSettore->nNumCopie);
									pDisplay->nStatoRiga1=NORMAL;
									pDisplay->nStatoRiga2=NORMAL;
									UpdateDisplay(pDisplay,TUTTO);
									/*
									* acccendo la luce di prelievo prodotto
									*/
									pSettore->nStatoLampada=1;
									SetLampada(pSettore->nIOS,pSettore->nCPU,pSettore->nModulo,pSettore->nRiga,pSettore->nColonna);
									/*
									* setto lo stato settore in attesa di conferma copie prelevate
									*/
									pSettore->nStatoSettore=PRELIEVO_COPIE;
								}
							}
						}
						break;
					}
				}
				break;
				case FINE_ORDINE:
				{
					/*
					* sono in attesa di conferma fine ordine
					* e' stato premuto il tasto di conferma fine ordine (riga/colonna giuste)
					* resetto il display e passo allo stato di attesa
					*/
					/*
					* controllo che la riga/colonna siano quelle giuste
					*/
					switch(pEvento->nEvento){
						case IOS_PKL_BUTTON_PRESSED:
							if(pEvento->nIOS==pSettore->nIOS && 
								pEvento->nCPU==pSettore->nCPU && 
								pEvento->nModulo==pSettore->nModulo && 
								pEvento->nRiga==pSettore->nRiga && 
								pEvento->nColonna==pSettore->nColonna){

								if((pOrdine=link_first(&(ListaOrdini[nSettoreIndex])))!=NULL){
									/*
									* Messaggio di Ordine Terminato a MAIN
									*/
									/*
									* LG: codice originale
									*/
									//sprintf(szMsgTxt,"%d,%d,%s,%s",pSettore->nIsola,pSettore->nSettore,pOrdine->szNUMELAB,pOrdine->szORDKEY);
									//SendMessage(PROC_MAIN, PROC_SETTORI,  SETTORI_ORDINE_PRELEVATO, szMsgTxt);
									//CambiaStatoOrdine(pOrdine->szORDKEY, ORDINE_PRELEVATO);

									
									sprintf(szMsgTxt,"%d,%d,%s,%s",pSettore->nIsola,pSettore->nSettore,pOrdine->szNUMELAB,pOrdine->szORDKEY);
/**************************************************************************************************
* GESTIONE PDF
***************************************************************************************************/
									/*
									* Ora devo differenziare il cambio di stato dell'ordine  
									* a  seconda dello stato in cui si trova
									* l'ordine entrato nel settore.
									*/
									cStato=pOrdine->szSTATO[0];

									switch(cStato){
										case ORDINE_IN_PRELIEVO:
										{
											/*
											* Codice originale
											*/
											CambiaStatoOrdine(pOrdine->szORDKEY, ORDINE_PRELEVATO);
											SendMessage(PROC_MAIN, PROC_SETTORI,  SETTORI_ORDINE_PRELEVATO, szMsgTxt);
										}
										break;
										case ORDINE_INCOMPLETO_IN_PRELIEVO:
										{
											/*
											* 
											*/
											CambiaStatoOrdine(pOrdine->szORDKEY, ORDINE_INCOMPLETO_TERMINATO);
											SendMessage(PROC_MAIN, PROC_SETTORI,  SETTORI_ORDINE_INCOMPLETO_PRELEVATO, szMsgTxt);
										}
										break;
										case ORDINE_INCOMPLETO_TERMINATO_IN_PRELIEVO:
										{
											/*
											* 
											*/
											CambiaStatoOrdine(pOrdine->szORDKEY, ORDINE_PRELEVATO);
											SendMessage(PROC_MAIN, PROC_SETTORI,  SETTORI_ORDINE_PRELEVATO, szMsgTxt);
										}
										break;
									}
				

/**************************************************************************************************
* FINE GESTIONE PDF
***************************************************************************************************/
								}

								pDisplay=&(Cfg.Displays[pSettore->nDisplay]);
								strcpy(pDisplay->szRiga_1_Display,"    ");
								strcpy(pDisplay->szRiga_2_Display,"    ");
								pDisplay->nStatoRiga1=NORMAL;
								pDisplay->nStatoRiga2=NORMAL;
								UpdateDisplay(pDisplay,TUTTO);



								/* reset lampada */
								pSettore->nStatoLampada=0;

								/* metto il settore in attesa di ordini da gestire */
								pSettore->nStatoSettore=ATTESA;

								/*
								* fine ordine confermata
								*/
								pSettore->szCodSped[0]='\0';
								/*
								* LG: aggiunto il reset dello stato dell'ordine
								*/
								pSettore->cStatoSped='\0';
								pSettore->szCodProd[0]='\0';
								pSettore->szCodUbi[0]='\0';
								pSettore->nNumCollo=0;
								pSettore->nNumCopie=0;
								pSettore->nIOS=pSettore->nFirstIOS;
								pSettore->nCPU=pSettore->nFirstCPU;
								pSettore->nModulo=pSettore->nFirstMod;
								pSettore->nRiga=pSettore->nFirstRow;
								pSettore->nColonna=pSettore->nFirstColumn;
								pSettore->nDisplay=pSettore->nFirstDisplay;
								pSettore->nIndiceRigaOrdine=0;
								pSettore->nNumeroRigheOrdine=0;

								/*
								* Gestione di fine settore particolare per Wella (NO RULLIERA DI SCARICO)
								*/
								if(Cfg.nFineOrdine || pSettore->nTipoSettore==SETTORE_END){

									/*
									* rimuovo il primo ordine dalla coda
									*/
									if((pOrdine=link_first(&(ListaOrdini[nSettoreIndex])))!=NULL){

										link_remove(&(ListaOrdini[nSettoreIndex]),pOrdine);
										free(pOrdine);
									}
								} else {
									/*
									* Faccio proseguire l'ordine fino all'ultimo settore dell'isola
									* (SETTORE DI END)
									*/
									/*
									* metto il primo ordine in coda al settore successivo
									* e lo rimuovo dalla coda del settore attuale
									*/
									if((pOrdine=link_first(&(ListaOrdini[nSettoreIndex])))!=NULL){
										sprintf(szMsgTxt,"%d,%d,%s,%s",pSettore->nIsola,pSettore->nSettore,pOrdine->szNUMELAB,pOrdine->szORDKEY);

										/*
										* Messaggio di Ordine Settore Successivo a MAIN
										*/
										SendMessage(PROC_MAIN, PROC_SETTORI,  SETTORI_ORDINE_SETT_SUCC, szMsgTxt);
										/*
										* rimuovo l'ordine dal settore attuale
										*/
										link_remove(&(ListaOrdini[nSettoreIndex]),pOrdine);
										/*
										* aggiungo l'ordine al primo settore abilitato successivo al presente
										*/
										/*
										* DA SISTEMARE ... GESTIRE I SETTORI PER ISOLA !!!!
										*/
										while(pSettori[nSettoreIndex].nIsola==pSettore->nIsola && pSettori[nSettoreIndex].nStatoSettore==DISABILITATO){
											nSettoreIndex++;
										}
#ifdef TRACE
										trace_out_vstr_date(1,"Passo Ordine [%s] a Settore [%2d]", pOrdine->szORDKEY,nSettoreIndex);
#endif

										link_add(&(ListaOrdini[nSettoreIndex+1]),pOrdine);
									}
								}

							}
						break;
					}
				}
				break;
			}
		break;
	}

	return TRUE;
}

/*
* lettura dati Operatori/Settori
*/
void ReadOperatori(void)
{
	PGresult *PGRes = NULL;
	int nTuples;
	int nIndex;
	int nSettoreIndex;
	int nIsola;
	int nSettore;

	/*
	* reset settori
	*/
	for(nIndex=0;nIndex<Cfg.nNumeroSettori;nIndex++){
		pSettori[nIndex].szCodOperatore[0]='\0';
	}

	PGRes=PGExecSQL(FALSE,"select os.opcdope,os.opisola,os.opset,op.opnmope,op.opcgope from opeset as os,operatori as op where os.opcdope=op.opcdope;");

	nTuples=PQntuples(PGRes);

	for(nIndex=0;nIndex<nTuples;nIndex++){

		nIsola = atoi(PQgetvalue(PGRes,nIndex,1));
		nSettore = atoi(PQgetvalue(PGRes,nIndex,2));

		for(nSettoreIndex=0;nSettoreIndex<Cfg.nNumeroSettori;nSettoreIndex++){
			if(pSettori[nSettoreIndex].nIsola==nIsola && pSettori[nSettoreIndex].nSettore==nSettore){
				strcpy(pSettori[nSettoreIndex].szCodOperatore,PQgetvalue(PGRes,nIndex,0));
				strcpy(pSettori[nSettoreIndex].szNomeOperatore,PQgetvalue(PGRes,nIndex,3));
				strcpy(pSettori[nSettoreIndex].szCognomeOperatore,PQgetvalue(PGRes,nIndex,4));
			}
		}
	}

	PQclear(PGRes);
}

/*
* lettura delle ubicazioni in archivio
*/
int ReadUbicazioni(PUBICAZIONI pUbi)
{
	int nIndex;
	PGresult *PGRes = NULL;
	int nTuples;

	PGRes=PGExecSQL(FALSE,"select ubicazione, codprod, locazione, isola, settore, display, ios, cpu, modulo, riga, colonna, priorita, cnistato from ubicazioni where cancella=' ';");

	nTuples=PQntuples(PGRes);

	for(nIndex=0;nIndex<nTuples;nIndex++){

		strcpy(pUbi->Ubicazione[nIndex].szCodUbi,         PQgetvalue(PGRes,nIndex,0));
		strcpy(pUbi->Ubicazione[nIndex].szCodProd,        PQgetvalue(PGRes,nIndex,1));
		strcpy(pUbi->Ubicazione[nIndex].szLocazione,      PQgetvalue(PGRes,nIndex,2));
		pUbi->Ubicazione[nIndex].nIsola            = atoi(PQgetvalue(PGRes,nIndex,3));
		pUbi->Ubicazione[nIndex].nSettore          = atoi(PQgetvalue(PGRes,nIndex,4));
		pUbi->Ubicazione[nIndex].nDisplay          = atoi(PQgetvalue(PGRes,nIndex,5));
		pUbi->Ubicazione[nIndex].nIOS              = atoi(PQgetvalue(PGRes,nIndex,6));
		pUbi->Ubicazione[nIndex].nCPU              = atoi(PQgetvalue(PGRes,nIndex,7));
		pUbi->Ubicazione[nIndex].nModulo           = atoi(PQgetvalue(PGRes,nIndex,8));
		pUbi->Ubicazione[nIndex].nRowIndex         = atoi(PQgetvalue(PGRes,nIndex,9));
		pUbi->Ubicazione[nIndex].nColIndex         = atoi(PQgetvalue(PGRes,nIndex,10));
		pUbi->Ubicazione[nIndex].nPriorita         = atoi(PQgetvalue(PGRes,nIndex,11));

		pUbi->Ubicazione[nIndex].nIndex=nIndex;
		
	}
	PQclear(PGRes);
	pUbi->nUbicazioni=nIndex;

	return nIndex;
}

void SortUbicazioni(PUBICAZIONI pUbi)
{
	qsort(pUbi->Ubicazione,pUbi->nUbicazioni,sizeof(UBICAZIONE),(int(*)())CmpUbicazione);
}

int CmpUbicazione(PUBICAZIONE pUbi1,PUBICAZIONE pUbi2)
{
	if(pUbi1->nIOS < pUbi2->nIOS){
		return -1;
	}
	if(pUbi1->nIOS > pUbi2->nIOS){
		return 1;
	}
	if(pUbi1->nCPU < pUbi2->nCPU){
		return -1;
	}
	if(pUbi1->nCPU > pUbi2->nCPU){
		return 1;
	}
	if(pUbi1->nModulo < pUbi2->nModulo){
		return -1;
	}
	if(pUbi1->nModulo > pUbi2->nModulo){
		return 1;
	}
	if(pUbi1->nRowIndex < pUbi2->nRowIndex){
		return -1;
	}
	if(pUbi1->nRowIndex > pUbi2->nRowIndex){
		return 1;
	}
	if(pUbi1->nColIndex < pUbi2->nColIndex){
		return -1;
	}
	if(pUbi1->nColIndex > pUbi2->nColIndex){
		return 1;
	}
	return 0;
}


PUBICAZIONE SearchUbicazione(PUBICAZIONI pUbi,int nIOS,int nCPU,int nModulo,int nRowIndex,int nColIndex)
{
	UBICAZIONE Ubi;

	Ubi.nIOS=nIOS;
	Ubi.nCPU=nCPU;
	Ubi.nModulo=nModulo;
	Ubi.nRowIndex=nRowIndex;
	Ubi.nColIndex=nColIndex;

	return bsearch(&Ubi,pUbi->Ubicazione,pUbi->nUbicazioni,sizeof(UBICAZIONE),(int(*)())CmpUbicazione);
}

/*
* Visualizza dati su display CNI PKDIS
* rm 30-11-2000 : invertite le righe per CALDARELLA (WELLA)
*/
void UpdateDisplay(PDISPLAYSTRUCT pDisplay,int nRiga)
{
	int nIOS;
	int nCPU;
	int nModulo; 
	unsigned char nDot; 

	if(pDisplay==(PDISPLAYSTRUCT)NULL){
		return;
	}

	nIOS=pDisplay->nIOS;
	nCPU=pDisplay->nCPU;
	nModulo=pDisplay->nModulo; 

	/*
	* Correggo al stampa della riga ordine causa puntino 
	*/
	nDot=0;
	if(isdigit(pDisplay->szRiga_1_Display[3])){
		nDot=4;
	}

	switch(nRiga){
		case TUTTO:
			/*
			* sono costretto a resettare la riga non BCD causa buco della ios PKDIS comandi 18H e 19H (Manfredi)
			*/
			DisplayWriteStringBCD( nIOS, nCPU, nModulo, 0, pDisplay->nStatoRiga2, "    ",0,0);
			/* prima la riga 2 */
			DisplayWriteString( nIOS, nCPU, nModulo, 0, pDisplay->nStatoRiga2, pDisplay->szRiga_2_Display);
			DisplayWriteStringBCD( nIOS, nCPU, nModulo, 1, pDisplay->nStatoRiga1, pDisplay->szRiga_1_Display,nDot,0);
		break;

		case RIGA_1:
			DisplayWriteStringBCD( nIOS, nCPU, nModulo, 1, pDisplay->nStatoRiga1, pDisplay->szRiga_1_Display,nDot,0);
		break;

		case RIGA_2:
			/*
			* sono costretto a resettare la riga non BCD causa buco della ios PKDIS comandi 18H e 19H (Manfredi)
			*/
			DisplayWriteStringBCD( nIOS, nCPU, nModulo, 0, pDisplay->nStatoRiga2, "    ",0,0);
			DisplayWriteString( nIOS, nCPU, nModulo, 0, pDisplay->nStatoRiga2, pDisplay->szRiga_2_Display);
		break;
	}
}

void DisplayWriteString(int nIOS, int nCPU, int nModulo, int nRowIndex, int nMode, char *szString)
{
	char szBuffer[256];

	sprintf(szBuffer,"%d,%d,%d,%d,%d,%x,%x,%x,%x", nIOS, nCPU, nModulo, nRowIndex, nMode, szString[0], szString[1], szString[2], szString[3]);

	SendMessage(PROC_IOS, PROC_SETTORI,  IOS_DPY_STRING, szBuffer);
}

int GetBCDNibble(unsigned char cChar)
{
	switch(cChar){
		case '0': case '1': case '2': case '3': case '4': 
		case '5': case '6': case '7': case '8': case '9':
			return cChar-'0';
		break;
		case ' ': return 10; break;
		case '-': return 11; break;
		case '.': return 12; break;
		case 'X': return 13; break;	/* non gestito */
		case 'Y': return 14; break;	/* non gestito */
		case 'Z': return 15; break;	/* non gestito */
	}
	return 15;
}

/*
* DisplayWriteStringBCD(nIOS, nCPU, nModulo, nRowIndex, nMode, szString,nDot,nBlink)
* Comando di settaggio display PKDIS usando comandi BCD (11,12,13,14-18,19)
*/
void DisplayWriteStringBCD(int nIOS, int nCPU, int nModulo, int nRowIndex, int nMode, char *szString,unsigned char nDot,unsigned char nBlink)
{
	char szBuffer[256];
	unsigned char DPYByte[3];

	DPYByte[0]  = GetBCDNibble(szString[0])<<4;
	DPYByte[0] |= GetBCDNibble(szString[1]);
	DPYByte[1]  = GetBCDNibble(szString[2])<<4;
	DPYByte[1] |= GetBCDNibble(szString[3]);

	/* flags */
	/* 4 bit bassi : puntino */
	/* 4 bit alti  : lampeggio */
	DPYByte[2]  = nDot | (nBlink<<4);

	sprintf(szBuffer,"%d,%d,%d,%d,%x,%x,%x", nIOS, nCPU, nModulo, nRowIndex, DPYByte[0],DPYByte[1],DPYByte[2]);

	SendMessage(PROC_IOS, PROC_SETTORI,  IOS_DPY_BCD, szBuffer);
}

void ScongelaPKL(int nIsola)
{
	int nIndex;
	char szBuffer[256];

	for(nIndex=0;nIndex<Cfg.nPKLNumber;nIndex++){
		/* offset isola in PKL=0 */
		if(nIsola==0 || nIndex==nIsola-1){
			sprintf(szBuffer,"%d,%d,%d", Cfg.PKL[nIndex].nIOS, Cfg.PKL[nIndex].nCPU, Cfg.PKL[nIndex].nModulo);
			SendMessage(PROC_IOS, PROC_SETTORI,  IOS_PKL_SCONGELA, szBuffer);
		}
	}
}

void CongelaPKL(int nIsola)
{
	int nIndex;
	char szBuffer[256];

	for(nIndex=0;nIndex<Cfg.nPKLNumber;nIndex++){
		/* offset isola in PKL=0 */
		if(nIsola==0 || nIndex==nIsola-1){
			sprintf(szBuffer,"%d,%d,%d", Cfg.PKL[nIndex].nIOS, Cfg.PKL[nIndex].nCPU, Cfg.PKL[nIndex].nModulo);
			SendMessage(PROC_IOS, PROC_SETTORI,  IOS_PKL_CONGELA, szBuffer);
		}
	}
}

void SetLampada(int nIOS,int nCPU,int nModule,int nRowIndex, int nColumn)
{
	char szBuffer[256];

	/*
	* rm 19-11-2002 : errore di indirizzamento ?
	*/
	if(nModule==0 && nRowIndex==0 && nColumn==0){
#ifdef TRACE
		trace_out_vstr_date(1,"SetLampada(%d,%d,%d,%d,%d)",nIOS,nCPU,nModule,nRowIndex,nColumn);
#endif
	} else {
		sprintf(szBuffer,"%d,%d,%d,%d,%d", nIOS, nCPU, nModule, nRowIndex, nColumn);

		SendMessage(PROC_IOS, PROC_SETTORI,  IOS_PKL_SET_RC, szBuffer);
	}

}

void ResetLampada(int nIOS,int nCPU,int nModule,int nRowIndex, int nColumn)
{
	char szBuffer[256];

	sprintf(szBuffer,"%d,%d,%d,%d,%d", nIOS, nCPU, nModule, nRowIndex, nColumn); 

	SendMessage(PROC_IOS, PROC_SETTORI,  IOS_PKL_RESET_RC, szBuffer);
}

void ResetLuci(void)
{
	int nIndex;
	char szBuffer[256];

	for(nIndex=0;nIndex<Cfg.nPKLNumber;nIndex++){
		sprintf(szBuffer,"%d,%d,%d", Cfg.PKL[nIndex].nIOS, Cfg.PKL[nIndex].nCPU, Cfg.PKL[nIndex].nModulo);
		SendMessage(PROC_IOS, PROC_SETTORI,  IOS_PKL_RESET_ALL, szBuffer);
	}
}

void ResetSettore(int nIsola,int nSettore)
{
	PORDINE pOrdine;
	int nFirstDisplay;
	int nFirstIOS;
	int nFirstCPU;
	int nFirstMod;
	int nFirstCol;
	int nFirstRow;
	PGresult *PGRes = NULL;
	int nTuples;
	int nIndex;

#ifdef TRACE
	trace_out_vstr_date(1,"ResetSettore(Isola:%d,Settore:%d)",nIsola,nSettore);
#endif

	/*
	* Cerco l'indice del settore relativo ai parametri isola e settore passati alla funzione
	*/
	for(nIndex=0;nIndex<Cfg.nNumeroSettori;nIndex++){
		if(pSettori[nIndex].nIsola==nIsola && pSettori[nIndex].nSettore==nSettore){
			pSettori[nIndex].nIndiceRigaOrdine=0;        /* puntatore alla riga d'ordine da prelevare */
			pSettori[nIndex].nNumeroRigheOrdine=0;       /* numero delle righe d'ordine da prelevare */
			/* 
			* Lista FIFO Ordini 
			*/
			while((pOrdine=link_pop(&(ListaOrdini[nIndex])))){
				free(pOrdine);
			}
			if(pSettori[nIndex].nStatoSettore!=DISABILITATO){
				pSettori[nIndex].nStatoSettore=ATTESA;       /* Stato attuale del settore */
			}
			/*
			* Determino i dati della prima ubicazione del settore in oggetto
			*/
			PGRes=PGExecSQL(FALSE,"select display, ios, cpu, modulo, riga, colonna from ubicazioni where isola='%d' AND settore='%4d' order by priorita;",nIsola,nSettore);

			nTuples=PQntuples(PGRes);

			if(nTuples>=1){
				nFirstDisplay=atoi(PQgetvalue(PGRes,0,0));
				nFirstIOS=atoi(PQgetvalue(PGRes,0,1));
				nFirstCPU=atoi(PQgetvalue(PGRes,0,2));
				nFirstMod=atoi(PQgetvalue(PGRes,0,3));
				nFirstRow=atoi(PQgetvalue(PGRes,0,4));
				nFirstCol=atoi(PQgetvalue(PGRes,0,5));
			} else {
#ifdef TRACE
				trace_out_vstr_date(1,"ResetSettore(Isola:%d,Settore:%d) : Non esistono ubicazioni per il settore in oggetto",nIsola,nSettore);
#endif
			}
			pSettori[nIndex].nFirstIOS=nFirstIOS;
			pSettori[nIndex].nFirstCPU=nFirstCPU;
			pSettori[nIndex].nFirstMod=nFirstMod;
			pSettori[nIndex].nFirstRow=nFirstRow;
			pSettori[nIndex].nFirstColumn=nFirstCol;
			pSettori[nIndex].nFirstDisplay=nFirstDisplay;

			pSettori[nIndex].nIOS=nFirstIOS;             /* Modulo Tasto Premuto / da premere */
			pSettori[nIndex].nCPU=nFirstCPU;             /* Modulo Tasto Premuto / da premere */
			pSettori[nIndex].nModulo=nFirstMod;          /* Modulo Tasto Premuto / da premere */
			pSettori[nIndex].nRiga=nFirstRow;            /* Riga Tasto Premuto / da premere */
			pSettori[nIndex].nColonna=nFirstCol;         /* Colonna Tasto Premuto / da premere */
			pSettori[nIndex].nDisplay=nFirstDisplay;     /* Display */
			pSettori[nIndex].nNumCopie=0;                /* Numero Copie */
			pSettori[nIndex].nNumCollo=0;                /* Numero Collo */
			pSettori[nIndex].nStatoLampada=0;            /* Stato Lampada */
			pSettori[nIndex].szCodSped[0]='\0';          /* Codice Ordine Attivo */
			/*
			* LG: aggiunto il reset dello stato dell'ordine
			*/
			pSettori[nIndex].cStatoSped='\0';
			pSettori[nIndex].szCodProd[0]='\0';          /* Codice Prodotto da prelevare/prelevato*/
			pSettori[nIndex].szCodUbi[0]='\0';           /* Codice Ubicazione del prodotto da prelevare/prelevato*/

			/*
			* st + 26-10-2000 resetto le luci del settore
			*/
			/*
			for (nUbicazione=0; nUbicazione<nTuples; nUbicazione++){
				ResetLampada(atoi(PQgetvalue(PGRes,nUbicazione,1)),
				             atoi(PQgetvalue(PGRes,nUbicazione,2)),
										 atoi(PQgetvalue(PGRes,nUbicazione,3)),
										 atoi(PQgetvalue(PGRes,nUbicazione,4)),
										 atoi(PQgetvalue(PGRes,nUbicazione,5)));
			}
			*/
			
			PQclear(PGRes);
		}

	}

	/*
	* Cerco l'indice del o dei displays relativo/i ai parametri isola e settore passati alla funzione
	*/
	for(nIndex=0;nIndex<Cfg.nNumeroDisplays;nIndex++){
		if(Cfg.Displays[nIndex].nIsola==nIsola && Cfg.Displays[nIndex].nSettore==nSettore){
			strcpy(Cfg.Displays[nIndex].szRiga_1_Display,"----");     /* Riga 1 del display */
			strcpy(Cfg.Displays[nIndex].szRiga_2_Display,"----");     /* Riga 2 del display */
			Cfg.Displays[nIndex].nStatoRiga1=NORMAL;
			Cfg.Displays[nIndex].nStatoRiga2=NORMAL;

			UpdateDisplay(&(Cfg.Displays[nIndex]),TUTTO);
		}
	}
}

/*
* cambia lo stato dell'ordine in PRODUZIONE e RICEZIONE
*/
BOOL CambiaStatoOrdine(char *szOrdKey, char cStato)
{
	BOOL bRetVal=TRUE;
	PGresult *PGRes = NULL;
	char szMinOrdKey[256];

#ifdef TRACE
	trace_out_vstr_date(1,"CambiaStatoOrdine [%s][%c]",szOrdKey,cStato);
#endif

	PGRes=PGExecSQL(FALSE,"update ord_prod set opstato='%c' where ordkey='%s';",cStato,szOrdKey);
	if (!PGRes || PQresultStatus(PGRes) != PGRES_COMMAND_OK ) {
		bRetVal=FALSE;
#ifdef TRACE
		trace_out_vstr_date(1,"Fallito update su ord_prod [%s]",szOrdKey);
#endif
	}
	PQclear(PGRes);
	if(bRetVal){
		/*
		* Aggiorno ric_ord solo con lo stato del sottordine piu' piccolo
		* che e' quello deputato a contenere il pdf
		* Non faccio il controllo sul flag pdf perche' potrebbe essere forzato
		* e' quindi non cambierei mai stato a ric_ord!
		* Quindi cerco il sottoordine piu' piccolo e lo confronto con quello passato
		*/

		PGRes=PGExecSQL(FALSE,"select min(ordkey) from ord_prod where ordprog in (select ordprog from ord_prod where ordkey='%s');",szOrdKey);
		strcpy(szMinOrdKey,PQgetvalue(PGRes,0,0));
		PQclear(PGRes);

		if(!strcmp(szMinOrdKey,szOrdKey)){
			if(Cfg.nSAP){
				PGRes=PGExecSQL(Cfg.nDebugVersion>1,"update ric_ord set rostato='%c' where ordprog='%s';",cStato,LeftStr(szOrdKey,10));
			} else {
				PGRes=PGExecSQL(Cfg.nDebugVersion>1,"update ric_ord set rostato='%c' where ordprog='%s';",cStato,LeftStr(szOrdKey,6));
			}
			if (!PGRes || PQresultStatus(PGRes) != PGRES_COMMAND_OK ) {
				bRetVal=FALSE;
#ifdef TRACE
				trace_out_vstr_date(1,"Fallito update su ric_ord [%s]",szOrdKey);
#endif
			}
			PQclear(PGRes);
		}else{
			/*
			* i sottoordini non sono uguali allora non aggiorno
			*/
		}

	}

	return(bRetVal);
}

BOOL CambiaStatoColliOrdine(char *szOrdKey, char cStato)
{
	int nColli=0;
	BOOL bRetVal=TRUE;
	PGresult *PGRes = NULL;

#ifdef TRACE
	trace_out_vstr_date(1,"CambiaStatoColliOrdine [%s][%c] : ",szOrdKey,cStato);
#endif
	PGRes=PGExecSQL(FALSE,"update col_prod set cpstato='%c' where ordkey='%s';",cStato,szOrdKey);
	if (!PGRes || PQresultStatus(PGRes) != PGRES_COMMAND_OK ) {
		bRetVal=FALSE;
#ifdef TRACE
		trace_out_vstr_date(1,"Fallito update su col_prod [%s]",szOrdKey);
#endif
	}
	nColli=atoi(PQcmdTuples(PGRes));

	PQclear(PGRes);

#ifdef TRACE
	trace_out_vstr(1,"Cambiati %d colli",nColli);
#endif

	return(nColli!=0);
}

/*
* rm 09-10-2000
* -------------
* CambiaStatoRighe Ordine NON viene utilizzata...
* serve una funzione di cambio stato RIGA ordine
* per la gestione statistiche di prelievo operatore
*/
BOOL CambiaStatoRigheOrdine(char *szOrdKey,int nCollo,char cStato)
{
	int nRighe=0;
	BOOL bRetVal=TRUE;
	PGresult *PGRes = NULL;

#ifdef TRACE
	trace_out_vstr_date(1,"CambiaStatoRigheOrdine [%s][%c] : ",szOrdKey,cStato);
#endif
	PGRes=PGExecSQL(FALSE,"update rig_prod set rpstato='%c' where ordkey='%s';",cStato,szOrdKey);
	if (!PGRes || PQresultStatus(PGRes) != PGRES_COMMAND_OK ) {
		bRetVal=FALSE;
#ifdef TRACE
		trace_out_vstr_date(1,"Fallito update su rig_prod [%s]",szOrdKey);
#endif
	}
	nRighe=atoi(PQcmdTuples(PGRes));

	PQclear(PGRes);

#ifdef TRACE
	trace_out_vstr(1,"Cambiate %d righe",nRighe);
#endif

	return(nRighe!=0);
}


/*
* UpdateRigaOrdine()
* ATTENZIONE : ragiona per codice ordine e codice ubicazione
* NON UTILIZZARE IL NUMERO COLLO A CAUSA DELLA GESTIONE PRELIEVO PER ORDINE : VEDI InizioPrelievoSettore()
* 03-03-2008 RM : gestione per collo dal 2006 !!
*/
BOOL UpdateRigaOrdine(PRIGA_PRELIEVO pRigaOrdine,PSETTORE pSettore)
{
	BOOL bRetVal=TRUE;
	PGresult *PGRes = NULL;
	char cStato;
	char szSQLCmd[512];

	switch(pRigaOrdine->nStato){
		default:
		case ATTESA_PRELIEVO:
			cStato=RIGA_ELABORATA;
			/*
			* modifico solo lo stato della riga
			* 03-03-2008 RM : gestione per collo dal 2006 !!
			*/
			sprintf(szSQLCmd,"update rig_prod set rpstato='%c' where ordkey='%s' AND rpnmcol=%d AND rpcdubi='%s';",
				cStato,
				pRigaOrdine->szCodSped,
				pRigaOrdine->nNumCollo,
				pRigaOrdine->szCodUbi);
		break;
		case PRELEVATA:
			cStato=RIGA_PRELEVATA;
			/*
			* In caso di prelievo setto codice operatore e data/ora prelievo
			*/
			sprintf(szSQLCmd,"update rig_prod set rpstato='%c',rpcdopr='%s',rpdtpre='%s',rptmpre='%s' where ordkey='%s' AND rpnmcol=%d AND rpcdubi='%s';",
				cStato,
				pSettore->szCodOperatore,
				GetDateYYYYMMDD(time((long *)0),szDateBuffer),
				GetTimeHHMMSS(time((long *)0),szTimeBuffer),
				pRigaOrdine->szCodSped,
				pRigaOrdine->nNumCollo,
				pRigaOrdine->szCodUbi);

		break;
	}
	PGRes=PGExecSQL(TRUE,szSQLCmd);

	if (!PGRes || PQresultStatus(PGRes) != PGRES_COMMAND_OK ) {
		bRetVal=FALSE;
#ifdef TRACE
		trace_out_vstr_date(1,"Fallito update su rig_prod [%s,%d]",pRigaOrdine->szCodSped,pRigaOrdine->nProgressivo);
#endif
	}
	PQclear(PGRes);


	return(bRetVal);
}

BOOL CambiaStatoCollo(char *szOrdKey,int nCollo,char cStato)
{
	BOOL bRetVal=TRUE;
	PGresult *PGRes = NULL;

#ifdef TRACE
	trace_out_vstr_date(1,"CambiaStatoCollo [%s][%d][%c] : ",szOrdKey,nCollo,cStato);
#endif
	PGRes=PGExecSQL(FALSE,"update col_prod set cpstato='%c' where ordkey='%s' AND cpnmcol = %d;",cStato,szOrdKey,nCollo);
	if (!PGRes || PQresultStatus(PGRes) != PGRES_COMMAND_OK ) {
		bRetVal=FALSE;
#ifdef TRACE
		trace_out_vstr_date(1,"Fallito update su col_prod [%s]",szOrdKey);
#endif
	}
	PQclear(PGRes);

	return(bRetVal);
}

int GetTipoOrdine(char *szOrdKey)
{
	int nIndex;
	int nTuples;
	PGresult *PGRes = NULL;

	PGRes=PGExecSQL(FALSE,"select cpswlin from col_prod where ordkey='%s';",szOrdKey);

	nIndex=0;
	while(nIndex<nTuples){
		/*
		* controllo se esiste almeno un collo automatico 
		*/
		if(atoi(PQgetvalue(PGRes,nIndex,0))==0){
			PQclear(PGRes);
			return (ORDINE_AUTOMATICO);
		}
	}
	PQclear(PGRes);
	return (ORDINE_MANUALE);
}

int GetSettore(int nIsola,int nSettore)
{
	int nIndex;

	for(nIndex=0;nIndex<Cfg.nNumeroSettori;nIndex++){
		if(pSettori[nIndex].nIsola==nIsola && pSettori[nIndex].nSettore==nSettore){
			return nIndex;
		}
	}
	return -1;
}

/*
* SaveState(char *pszFileName)
* Salvataggio stato settori
*/
BOOL SaveState(char *pszFileName)
{
	FILE *fp;
	int nIndex;
	PORDINE pOrdine;
	BOOL bRetValue=TRUE;

#ifdef TRACE
	trace_out_vstr_date(1, "Salvataggio stato settori : [%s]", pszFileName);
#endif

	if((fp=fopen(pszFileName,"w"))!=(FILE *)NULL){
		/*
		* salvo la strutture dei settori
		*/
		for(nIndex=0;nIndex<Cfg.nNumeroSettori;nIndex++){
			/* 
			* struttura di definizione del settore 
			*/
			if(!fwrite((char *)&(pSettori[nIndex]),sizeof(SETTORE),1,fp)){
#ifdef TRACE
				trace_out_vstr_date(1, "Errore in salvataggio stato settori : fwrite()" );
#endif
				bRetValue=FALSE;
			}
			/* 
			* numero ordini relativi al settore 
			*/
			if(!fwrite((char *)&(ListaOrdini[nIndex].n_link),sizeof(int),1,fp)){
#ifdef TRACE
				trace_out_vstr_date(1, "Errore in salvataggio stato settori : fwrite()");
#endif
				bRetValue=FALSE;
			}
			for(pOrdine=NULL; (pOrdine=link_next(&(ListaOrdini[nIndex]),pOrdine));){
				if(!fwrite((char *)pOrdine,sizeof(ORDINE),1,fp)){
#ifdef TRACE
					trace_out_vstr_date(1, "Errore in salvataggio stato settori : fwrite()");
#endif
					bRetValue=FALSE;
				}
			}
		}
		/*
		* salvo la strutture dei displays
		*/
		for(nIndex=0;nIndex<Cfg.nNumeroDisplays;nIndex++){
			/* 
			* struttura di definizione del display 
			*/
			if(!fwrite((char *)&(Cfg.Displays[nIndex]),sizeof(DISPLAYSTRUCT),1,fp)){
#ifdef TRACE
				trace_out_vstr_date(1, "Errore in salvataggio stato displays : fwrite()" );
#endif
				bRetValue=FALSE;
			}
		}
		/*
		* Chiusura del file
		*/
		fclose(fp);
	} else {
#ifdef TRACE
		trace_out_vstr_date(1, "Errore in salvataggio stato settori : fopen(%s)", pszFileName);
#endif
		bRetValue=FALSE;
	}
	return bRetValue;
}


/*
* RestoreState(char *pszFileName)
* Ripristino stato settori
*/
BOOL RestoreState(char *pszFileName)
{
	BOOL bRetValue=TRUE;
	FILE *fp;
	int nIndex;
	int nOrdine;
	int nItems;
	PORDINE pOrdine;


#ifdef TRACE
		trace_out_vstr_date(1, "Ripristino stato settori : [%s]", pszFileName);
#endif

	if((fp=fopen(pszFileName,"r"))!=(FILE *)NULL){
		/*
		* leggo le strutture dei settori
		*/
		for(nIndex=0;nIndex<Cfg.nNumeroSettori;nIndex++){
			/* 
			* struttura di definizione del settore 
			*/
			if(!fread((char *)&pSettori[nIndex],sizeof(SETTORE),1,fp)){
#ifdef TRACE
				trace_out_vstr_date(1, "Errore in caricamento stato settori : fread()");
#endif
				bRetValue=FALSE;
			} else {
				/* 
				* Svuoto la lista FIFO Ordini attuale
				*/
				while((pOrdine=link_pop(&(ListaOrdini[nIndex])))){
					free(pOrdine);
				}
				/* 
				* numero ordini collegati al settore 
				*/
				if(!fread((char *)&nItems,sizeof(int),1,fp)){
#ifdef TRACE
					trace_out_vstr_date(1, "Errore in caricamento numero ordini : fread()");
#endif
					bRetValue=FALSE;
				} else {
					for(nOrdine=0;nOrdine<nItems;nOrdine++){
						pOrdine=(PORDINE)malloc(sizeof(ORDINE));
						memset(pOrdine,0,sizeof(ORDINE));
						if(!fread((char *)pOrdine,sizeof(ORDINE),1,fp)){
#ifdef TRACE
							trace_out_vstr_date(1, "Errore in caricamento stato ordini : fread()");
#endif
							bRetValue=FALSE;
						} else {
							link_add(&(ListaOrdini[nIndex]),pOrdine);
						}
					}
				}
			}
		}

		/*
		* leggo le strutture dei settori
		*/
		for(nIndex=0;nIndex<Cfg.nNumeroDisplays;nIndex++){
			/* 
			* struttura di definizione del display 
			*/
			if(!fread((char *)&Cfg.Displays[nIndex],sizeof(DISPLAYSTRUCT),1,fp)){
#ifdef TRACE
				trace_out_vstr_date(1, "Errore in caricamento stato display : fread()");
#endif
				bRetValue=FALSE;
			}
		}
		if(bRetValue){
			/*
			* ripristino stato displays e luci
			*/
			for(nIndex=0;nIndex<Cfg.nNumeroSettori;nIndex++){
				if(pSettori[nIndex].nStatoLampada==1){
					SetLampada( pSettori[nIndex].nIOS,pSettori[nIndex].nCPU, pSettori[nIndex].nModulo,pSettori[nIndex].nRiga,pSettori[nIndex].nColonna);
				}
			}
			for(nIndex=0;nIndex<Cfg.nNumeroDisplays;nIndex++){
				UpdateDisplay(&(Cfg.Displays[nIndex]),TUTTO);
			}
		}
		/*
		* Chiusura file
		*/
		fclose(fp);
	} else {
#ifdef TRACE
		trace_out_vstr_date(1, "Errore in rispristino stato settori : fopen(%s)", pszFileName);
#endif
		bRetValue=FALSE;
	}
	return bRetValue;
}

/*
* OrdKey : OOOOOO.I.SS
* O : Ordine
* I : Isola
* S : Sottordine
* si visualizza : OOOO.S
*/
char *GetDisplayData(char *pszOrdKey)
{
	static char szData[256];
	char szOrdine[256];

	if(Cfg.nSAP){
		strcpy(szOrdine,RightStr(pszOrdKey,8));
		strcpy(szData,LeftStr(szOrdine,3));
		strcat(szData,SubStr(szOrdine,14,1));
	} else {
		strcpy(szData,SubStr(pszOrdKey,3,3));
		strcat(szData,SubStr(pszOrdKey,10,1));
	}

	return szData;
}
