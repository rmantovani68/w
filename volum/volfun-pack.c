/*
* volfun.c :  funzioni di utilita' di Analisi volumetrica 
* 
* Progetto Easy Picking 3.0
*
* Autore : Roberto Mantovani - Stefano Tarroni
*
* Copyright CNI srl 1995-2000
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

#include	"volum.h"
#include	"volstruct.h"
#include	"volext.h"
#include	"volfun.h"

/* 
* gestione transazioni in analisi volumetrica 
*/
// #define TRANSACTIONS_ON


/*
* lettura dell'archivio Imballli
*/
int ReadImballi(PIMBALLI pImb)
{
	int nIndex;
	PGresult *PGRes = NULL;
	int nTuples;

	/*
	* Seleziono solo gli imballi con dimensioni diverse da 0 se tipo lavorazione 0 o 2 
	* oppure anche con volume nullo se tipo lavorazione = 1 (fuori linea)
	*/
	PGRes=PGExecSQL(Cfg.nDebugVersion,"select codice, descriz, lunghezza, altezza, larghezza, tara, pesomax, volperc, tplavor from imballi where (larghezza > 0 AND altezza > 0 AND lunghezza > 0) OR tplavor = 1;");

	nTuples=PQntuples(PGRes);

	nIndex=0;
	pImb->nImballiAutomatici=0;
	pImb->nImballi=0;
	for(nIndex=0;nIndex<nTuples;nIndex++){
		strcpy(pImb->Imballo[nIndex].szCodice,PQgetvalue(PGRes,nIndex,0));
		pImb->Imballo[nIndex].nLength  = atoi(PQgetvalue(PGRes,nIndex,2));
		pImb->Imballo[nIndex].nHeight  = atoi(PQgetvalue(PGRes,nIndex,3));
		pImb->Imballo[nIndex].nWidth   = atoi(PQgetvalue(PGRes,nIndex,4));
		pImb->Imballo[nIndex].nTara    = atoi(PQgetvalue(PGRes,nIndex,5));
		pImb->Imballo[nIndex].nPesoMax = atoi(PQgetvalue(PGRes,nIndex,6));
		pImb->Imballo[nIndex].nVolPerc = atoi(PQgetvalue(PGRes,nIndex,7));
		/* tipo imballo - 0:linea automatica, 1:linea manuale 2:evasione su chiusura*/
		pImb->Imballo[nIndex].nSwLinea = atoi(PQgetvalue(PGRes,nIndex,8));

		pImb->Imballo[nIndex].nDim[0] = pImb->Imballo[nIndex].nLength;
		pImb->Imballo[nIndex].nDim[1] = pImb->Imballo[nIndex].nWidth;
		pImb->Imballo[nIndex].nDim[2] = pImb->Imballo[nIndex].nHeight;
		/*
		* ordino le dimensioni della riga
		*/
		qsort(pImb->Imballo[nIndex].nDim,3,sizeof(int),(int(*)())CmpInt);

		/*
		* calcolo volume scatola in cc (Centimetri Cubici)
		*/
		pImb->Imballo[nIndex].nVolume = (pImb->Imballo[nIndex].nLength * pImb->Imballo[nIndex].nWidth * pImb->Imballo[nIndex].nHeight)/1000;


		/*
		* calcolo volume utile scatola (percentuale volume totale)
		*/
		pImb->Imballo[nIndex].nVolumeUtile = pImb->Imballo[nIndex].nVolume * (float)((float)pImb->Imballo[nIndex].nVolPerc/(float)100);
		if( pImb->Imballo[nIndex].nSwLinea != 1){
			pImb->nImballiAutomatici++;
		}
		pImb->nImballi++;
	}
		

	return pImb->nImballi;
}


int CmpInt(int *pInt1,int *pInt2)
{
	return *pInt1 - *pInt2;
}

void SortImballi(PIMBALLI pImb)
{
	qsort(pImb->Imballo,pImb->nImballi,sizeof(IMBALLO),(int(*)())CmpImballo);
}

int CmpImballo(PIMBALLO pImb1,PIMBALLO pImb2)
{
	/* 
	* se il tipo lavorazione e' diverso
	*/
	if( pImb1->nSwLinea != pImb2->nSwLinea ){
		/*
		* Prima gli imballi con sw <> 1 e dopo quelli con sw = 1
		*/
		if(pImb2->nSwLinea == 1 || pImb1->nSwLinea == 1){
			return pImb2->nSwLinea == 1?-1:1;
		}
	}
	/* 
	* imballo 1 piu' piccolo di imballo 2 per volume utile
	*/
	if( pImb1->nVolumeUtile <= pImb2->nVolumeUtile){
		return -1;
	}
	/* 
	* imballo 1 piu' grande di imballo 2 per volume utile
	*/
	if( pImb1->nVolumeUtile > pImb2->nVolumeUtile){
		return 1;
	}

	return 0;
}

/*
* InsertRigProd(PGRes, nResIndex, nCopie, bFF, bPCF, pOrdine)
* Crea una riga nell'archivio righe di produzione con i dati contenuti in PGRes
*   - Crea la prima parte del codice ORDKEY (ORDINE e SETTORE)
*   - NON assegna la riga di produzione al collo ed al sottordine
* PARAMETRI: Ordine,Collo,Riga, Tabella da utilizzare
* Gestisce i flag di FF (Fuori Formato) e PCF (Preconf).
*/
BOOL InsertRigProd(PORDINE pOrdine,PCOLLO_ORDINE pCollo,PRIGA_ORDINE pRiga, char *szTable,char cStato)
{
	char szSQLCmd[512];
	PGresult *PGResInsert;


	sprintf(szSQLCmd,"insert into %s (ordprog, ordkey, rpnmcol, rpcdpro, rpqtspe,rpqtpes,rpprrig,rpcdubi,rpswffo,rpswpcf,rpstato) \
	values ('%s','%s',%d,'%s',%d,%d,'%d','%s',%d,%d,'%c');",
		szTable,
		pOrdine->szOrdProg,
		pOrdine->szOrdKey,
		pCollo->nCollo,
		pRiga->szCodiceProdotto,
		pRiga->nCopie, 
		pRiga->nPesoTotale, 
		pRiga->nProgRiga, 
		pRiga->szUbicazione,
		pRiga->nSwFF,
		pRiga->nSwPCF,
		cStato);

	PGResInsert=PGExecSQL(Cfg.nDebugVersion,szSQLCmd);
	PQclear(PGResInsert);

	return TRUE;
}

/*
* InsertColProd(pOrdine,pCollo)
* Crea un collo nell'archivio colli di produzione 
*/
BOOL InsertColProd(PORDINE pOrdine,PCOLLO_ORDINE pCollo,char cStato)
{
	char szSQLCmd[512];
	PGresult *PGResInsert;
	PGresult *PGResSel;
	int nPesoReale=0;


	switch(pOrdine->cStatoOrdineElaborato){
		default:
		case ORDINE_ELABORATO:
			nPesoReale=0;
			szTimeBuffer[0]='\0';
			szDateBuffer[0]='\0';
		break;
		case ORDINE_EVASO:
			nPesoReale=pOrdine->nPesoTotale;
			GetTimeHHMMSS(time((long *)0),szTimeBuffer);
			GetDateYYYYMMDD(time((long *)0),szDateBuffer);
		break;
	}

	/*
	* Ricavo il codice a barre del collo
	*/
	strcpy(pCollo->szBarcode,ColloBarcode(pOrdine,pCollo));

	/* 
	* 05-04-2011 rm-gg : Modifica per gestire il caso di piu' prodotti pericolosi con stesso codice di pericolosita'
	*                    In questo caso si stampa il codice pericolosita' e non la semplice scritta : LQ
	*/
	PGResSel=PGExecSQL(Cfg.nDebugVersion,"select distinct ppcdper from prodotti_pericolosi,rig_prod where ordkey='%s' and rpnmcol=%d and rpcdpro=ppcdpro;",pOrdine->szOrdKey,pCollo->nCollo);
	if(PQntuples(PGResSel)==1){
		strcpy(pCollo->szPericoloso,PQgetvalue(PGResSel,0,0));
	}
	PQclear(PGResSel);


	sprintf(szSQLCmd,"insert into col_prod (ordprog, ordkey, cpnelab, cptpfor, cpswlin, cpnmcol, cpnmrgh, cpnmcpe, cppspre, cpbrcde, cppsrea, cpdteva, cptmeva, cpperic, cpstato, cpflftz) values ( '%s','%s','%s','%s','%d',%d,%d,%d,%d,'%s',%d,'%s','%s','%s','%c','%c');",
		pOrdine->szOrdProg,
		pOrdine->szOrdKey,
		pOrdine->szNumElab,
		pCollo->szCodiceImballo,
		pCollo->nSwLinea,
		pCollo->nCollo, 
		pCollo->nRighe, 
		pCollo->nCopie, 
		pCollo->nPeso, 
		pCollo->szBarcode,
		nPesoReale,
		szDateBuffer,
		szTimeBuffer,
		pCollo->szPericoloso,
		cStato,
		pOrdine->cFlagFittizio);

	PGResInsert=PGExecSQL(Cfg.nDebugVersion,szSQLCmd);
	PQclear(PGResInsert);

	return TRUE;
}

/*
* UpdateOrdProd(pOrdine)
* Aggiorna i dati di un ordine nell'archivio ordini di produzione 
*/
BOOL UpdateOrdProd(PORDINE pOrdine)
{
	char szSQLCmd[512];
	PGresult *PGResInsert;

	sprintf(szSQLCmd,"update ord_prod set opnmrgh=%d, opnmprs=%d, opnmcpe=%d, opnmcla=%d, opnmcll=%d, oppspre=%d where ord_prod.ordkey='%s';",
		pOrdine->nRighe,
		pOrdine->nPrese,
		pOrdine->nCopie,
		pOrdine->nColliAutomatici,
		pOrdine->nColli,
		pOrdine->nPesoTotale,
		pOrdine->szOrdKey);

	PGResInsert=PGExecSQL(Cfg.nDebugVersion,szSQLCmd);
	PQclear(PGResInsert);

	return TRUE;
}

/*
* InsertOrdProd(pOrdine)
* Crea un ordine nell'archivio ordini di produzione 
*/
BOOL InsertOrdProd(PORDINE pOrdine,char cStato)
{
	char szSQLCmd[512];
	PGresult *PGResInsert;
	int nPesoReale=0;

	switch(pOrdine->cStatoOrdineElaborato){
		default:
		case ORDINE_ELABORATO:
		case ORDINE_INCOMPLETO_TERMINATO:
			nPesoReale=0;
			szTimeBuffer[0]='\0';
			szDateBuffer[0]='\0';
		break;
		case ORDINE_EVASO:
			nPesoReale=pOrdine->nPesoTotale;
			GetTimeHHMMSS(time((long *)0),szTimeBuffer);
			GetDateYYYYMMDD(time((long *)0),szDateBuffer);
		break;
	}

	sprintf(szSQLCmd,"insert into ord_prod (ordkey, ordprog, opnelab, opcdisl, opcdsto, opnmrgh, opnmprs, opnmcpe, opnmcla, opnmcll, oppspre, opprgln, opprgpr, oppsrea, opdteva, optmeva, opstato, opflpdf, opflftz) values ( '%s','%s','%s',%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,'%s','%s','%c','%c','%c');",
		pOrdine->szOrdKey,
		pOrdine->szOrdProg,
		pOrdine->szNumElab,
		pOrdine->nIsola,
		pOrdine->nSottoOrdine,
		pOrdine->nRighe,
		pOrdine->nPrese,
		pOrdine->nCopie,
		pOrdine->nColliAutomatici,
		pOrdine->nColli,
		pOrdine->nPesoTotale,
		pOrdine->nProgressivoLancio,
		pOrdine->nProgressivoPriorita,
		nPesoReale,
		szDateBuffer,
		szTimeBuffer,
		cStato,
		pOrdine->cFlagPdf,
		pOrdine->cFlagFittizio);

	PGResInsert=PGExecSQL(Cfg.nDebugVersion,szSQLCmd);
	PQclear(PGResInsert);

	return TRUE;
}

/*
* UpdateSpedizione(pSpedizione)
* Aggiorna una spedizione nell'archivio ordini ricevuti
*/
BOOL UpdateSpedizione(PSPEDIZIONE pSpedizione,char cStato)
{
	char szSQLCmd[512];
	PGresult *PGResUpdate;
	int nPesoReale=0;


	switch(pSpedizione->cStatoOrdineElaborato){
		default:
		case ORDINE_ELABORATO:
			nPesoReale=0;
			szTimeBuffer[0]='\0';
			szDateBuffer[0]='\0';
		break;
		case ORDINE_EVASO:
			nPesoReale=pSpedizione->nPesoTotale;
			GetTimeHHMMSS(time((long *)0),szTimeBuffer);
			GetDateYYYYMMDD(time((long *)0),szDateBuffer);
		break;
	}

	sprintf(szSQLCmd,"update ric_ord set ronmcll=%d, ronmrgh=%d, ronmcpe=%d, ropspre=%d, ropsrea=%d, rodteva='%s', rotmeva='%s', rostato='%c' where ordprog='%s';",
		pSpedizione->nColli,
		pSpedizione->nRighe,
		pSpedizione->nCopie,
		pSpedizione->nPesoTotale,
		nPesoReale,
		szDateBuffer,
		szTimeBuffer,
		cStato,
		pSpedizione->szOrdProg);

	PGResUpdate=PGExecSQL(Cfg.nDebugVersion,szSQLCmd);
	PQclear(PGResUpdate);

	return TRUE;
}

/*
* BOOL AnalisiVolumetrica()
* Analisi Volumetrica degli ordini spediti,
* produce gli archivi OrdProd,ColProd,RigProc
* rm+st 13-02-2001 : modifiche per gestione secondi invii (ordini da evadere)
*/
BOOL AnalisiVolumetrica(void)
{
	SPEDIZIONE Spedizione;    /* Dati spedizione (insieme di ordini) */
	ORDINE Ordine;            /* Dati ordine (insieme di colli per isola) */
	COLLO_ORDINE Collo;       /* Dati collo (insieme di righe) */
	RIGA_ORDINE Riga;         /* Dati di riga di prelievo */
	BOOL bPallet=FALSE;       /* Non si gestisce l'ordine a pallet */
	BOOL bFine=FALSE;
	BOOL bOK=TRUE;
	char szMsg[128];
	char szBuffer[128];
	int nRC;
	int nCollo;
	char szOrdKey[128];
	int nCopieRiga;
	int nColliPCF;            /* Colli preconfezionati */
	int nCopiePCF;            /* Copie di preconfezionato da mettere nei colli preconf.*/
	int nRestoPCF;            /* Resto di preconfezionato da mettere nei colli di linea */
	int nColliFF;             /* Colli di Fuori Formato */
	int nCopieFF;             /* Copie di Fuori Formato */
	int nRestoFF;             /* Resto di Fuori Formato da mettere nei colli di linea */
	int nTuples;
	int nOrdiniDaAnalizzare;
	int nIndex;
	int nRigaIndex;
	int nOrdineIndex;
	int nRigheOrdine;
	int nIsole;
	int nIsolaIndex;
	int nColloIndex;
	PGresult *PGResOrdini;
	PGresult *PGResRighe;
	PGresult *PGResIsole;
	PGresult *PGRes;
	PGresult *PGResCat;
	PGresult *PGResUbi;
	PGresult *PGResColli;
	PGresult *PGResCollo;
	PGresult *PGResPDF;
	int nProgressivoCollo=0;
	int nColliSpedizione=0;


	/*
	* Seleziono gli ordini 'segnati' per l'analisi volumetrica
	* ordinamento per sequenza di lancio
	*/
	PGResOrdini=PGExecSQL(Cfg.nDebugVersion,"select ordprog,ronelab,roprgln,rostato,roflpdf,roflftz,roprgpr from ric_ord where (rostato='%c' OR rostato='%c' OR rostato='%c') order by roprgln;",ORDINE_SPEDITO,ORDINE_SPEDITO_DA_EVADERE,ORDINE_SPEDITO_DA_CONSUNTIVARE);

	nOrdiniDaAnalizzare=PQntuples(PGResOrdini);

	for(nOrdineIndex=0; nOrdineIndex<nOrdiniDaAnalizzare; nOrdineIndex++){
		bOK=TRUE;
		
		/*
		* Cancello il contenuto della tabella rig_tmp usata come deposito temporaneo
		* righe di produzione
		*/
		PGResRighe=PGExecSQL(Cfg.nDebugVersion,"truncate rig_tmp;");
		PQclear(PGResRighe);

		strcpy(Spedizione.szOrdProg,PQgetvalue(PGResOrdini,nOrdineIndex,0));
		strcpy(Spedizione.szNumElab,PQgetvalue(PGResOrdini,nOrdineIndex,1));
		strcpy(szBuffer,PQgetvalue(PGResOrdini,nOrdineIndex,3));
		Spedizione.cStato=szBuffer[0];
		switch(Spedizione.cStato){
			default:
			case ORDINE_SPEDITO:
				Spedizione.cStatoOrdineElaborato=ORDINE_ELABORATO;
				Spedizione.cStatoColloElaborato=COLLO_ELABORATO;
				Spedizione.cStatoRigaElaborata=RIGA_ELABORATA;
			break;
			case ORDINE_SPEDITO_DA_EVADERE:
				Spedizione.cStatoOrdineElaborato=ORDINE_EVASO;
				Spedizione.cStatoColloElaborato=COLLO_EVASO;
				Spedizione.cStatoRigaElaborata=RIGA_EVASA;
			break;
			case ORDINE_SPEDITO_DA_CONSUNTIVARE:
				Spedizione.cStatoOrdineElaborato=ORDINE_RICEVUTO;
				Spedizione.cStatoColloElaborato=' ';
				Spedizione.cStatoRigaElaborata=' ';
			break;
		}

		Spedizione.nProgressivoLancio=atoi(PQgetvalue(PGResOrdini,nOrdineIndex,2));
		Spedizione.nProgressivoPriorita=atoi(PQgetvalue(PGResOrdini,nOrdineIndex,6));
		Spedizione.nColli=0;
		Spedizione.nRighe=0;
		Spedizione.nCopie=0;
		Spedizione.nPesoTotale=0;
		Spedizione.nSottoOrdini=0;
		strcpy(szBuffer,PQgetvalue(PGResOrdini,nOrdineIndex,4));
		Spedizione.cFlagPdf=szBuffer[0];
		strcpy(szBuffer,PQgetvalue(PGResOrdini,nOrdineIndex,5));
		Spedizione.cFlagFittizio=szBuffer[0];
		/*
		 * BRI
		 */
		Spedizione.cFlagBarcodeIdentificativo=' ';

		strcpy(Ordine.szOrdProg,PQgetvalue(PGResOrdini,nOrdineIndex,0));
		strcpy(Ordine.szNumElab,PQgetvalue(PGResOrdini,nOrdineIndex,1));
		strcpy(Ordine.szOrdKey,PQgetvalue(PGResOrdini,nOrdineIndex,0));
		Ordine.nProgressivoLancio=Spedizione.nProgressivoLancio;
		Ordine.nProgressivoPriorita=Spedizione.nProgressivoPriorita;
		Ordine.cStato=Spedizione.cStato;
		Ordine.cStatoOrdineElaborato=	Spedizione.cStatoOrdineElaborato;
		Ordine.cStatoColloElaborato =	Spedizione.cStatoColloElaborato;
		Ordine.cStatoRigaElaborata  =	Spedizione.cStatoRigaElaborata;
		// Il sottoordine viene settato senza pdf
		// Solo il primo sottoordine dovra' essere impostato con pdf Vedi Avanti
		Ordine.cFlagPdf=ORDINE_SENZA_PDF;
		// Il sottoordine viene settato col flag fittizio della spedizione
		Ordine.cFlagFittizio=Spedizione.cFlagFittizio;
#ifdef TRACE
		trace_out_vstr(1,"Analisi Volumetrica Spedizione [%s] Stato [%c]",Spedizione.szOrdProg,Spedizione.cStato);
		trace_out_vstr(1,"=================================================");
#endif

		/* cancellazione dati prima dell'analisi volumetrica */
		PGRes=PGExecSQL(Cfg.nDebugVersion,"delete from ord_prod where ordprog='%s';",Spedizione.szOrdProg);
		nTuples=PQntuples(PGRes);
		PQclear(PGRes);
		PGRes=PGExecSQL(Cfg.nDebugVersion,"delete from col_prod where ordprog='%s';",Spedizione.szOrdProg);
		nTuples=PQntuples(PGRes);
		PQclear(PGRes);
		PGRes=PGExecSQL(Cfg.nDebugVersion,"delete from rig_prod where ordprog='%s';",Spedizione.szOrdProg);
		nTuples=PQntuples(PGRes);
		PQclear(PGRes);




#ifdef TRANSACTIONS_ON
		PGRes=PGExecSQL(Cfg.nDebugVersion,"BEGIN WORK;");
		PQclear(PGRes);
#endif

		/*
		* Cancellazione eventuali dati di produzione
		*/
		PGRes=PGExecSQL(Cfg.nDebugVersion,"select ordprog from ord_prod where ordprog='%s';",Spedizione.szOrdProg);
		nTuples=PQntuples(PGRes);
		PQclear(PGRes);
		if(nTuples){
			/* 
			* 15-09-2005 rm
			* modifica per memorizzare il numero del primo segnacollo
			*/
			PGRes=PGExecSQL(Cfg.nDebugVersion,"select cpprgcl from col_prod where ordprog='%s' order by cpprgcl;",Spedizione.szOrdProg);
			if(PQntuples(PGRes)){
				nProgressivoCollo=atoi(PQgetvalue(PGRes,0,0));
				nColliSpedizione=PQntuples(PGRes);
			}
			PQclear(PGRes);
			PGRes=PGExecSQL(Cfg.nDebugVersion,"delete from ord_prod where ordprog='%s';",Spedizione.szOrdProg); PQclear(PGRes);
			PGRes=PGExecSQL(Cfg.nDebugVersion,"delete from col_prod where ordprog='%s';",Spedizione.szOrdProg); PQclear(PGRes);
			PGRes=PGExecSQL(Cfg.nDebugVersion,"delete from rig_prod where ordprog='%s';",Spedizione.szOrdProg); PQclear(PGRes);
		}

		/*
		* PRIMA SCANSIONE DELL'ANALISI VOLUMETRICA:
		*
		* inserimento in rig_prod delle righe appartenenti all'ordine in oggetto
		* se collo DI LINEA        -> per ogni riga ricevuta ne inserisco una in rig_prod
		* se collo PRECONFEZIONATO -> inserimento di (CopieRiga / NumCopiePrec) righe {+1 se c'e' del resto} 
		* se collo FUORI FORMATO   -> inserimento di (CopieRiga / nCopieFF) righe {+1 se c'e' del resto} 
		*/

		/*
		* Seleziono le righe relative all'ordine in oggetto
		*/
		PGResRighe=PGExecSQL(Cfg.nDebugVersion,"select racdpro, raqtord from ric_art where ordprog='%s';",Ordine.szOrdProg);

		nRigheOrdine=PQntuples(PGResRighe);



/**************************************************************************************************
* GESTIONE PDF
***************************************************************************************************/


		
		/*
		*	Ordine Fittizio
		* Lo capisco perche ho 0 righe ma uso Spedizione.cFlagFittizio che forse
		* e' piu' sicuro
		*/
		if (Spedizione.cFlagFittizio==ORDINE_FITTIZIO){
			
			Collo.nIndiceImballo=0;

			CreazioneCollo(&Collo,nCollo,FALSE);

			/*
			* Collo Automatico
			*/
			Collo.nSwLinea = 0;
			/* 
			* Aggiungo la tara al peso calcolato del collo 
			*/
			Collo.nPeso=Imballi.Imballo[Collo.nIndiceImballo].nTara;
			Collo.nCollo=1;


			/*
			* Imposto dati ordine
			*/
			Ordine.nColli=1;
			Ordine.nColliAutomatici=1;
			Ordine.nRighe=0;
			Ordine.nPrese=0;
			Ordine.nCopie=0;
			Ordine.nPesoTotale=0;
			Ordine.nIsola=1;
			Ordine.nSottoOrdine=1;
			Spedizione.nColli=Ordine.nColli;
			Spedizione.nRighe=Ordine.nRighe;
			Spedizione.nCopie=Ordine.nCopie;
			Spedizione.nPesoTotale=Ordine.nPesoTotale;
			//Spedizione.nSottoOrdini++;

			sprintf(Ordine.szOrdKey,"%s.%d.%02d",Ordine.szOrdProg,Ordine.nIsola,Ordine.nSottoOrdine);

#ifdef TRACE
			trace_out_vstr(1,"Analisi Volumetrica Sotto Ordine [%s]",Ordine.szOrdKey);
			trace_out_vstr(1,"Collo F.to  Peso    Volume Vol.%% Toll.%%");
			trace_out_vstr(1,"----- ----- ------- ------ ----- ------");
#endif
			/*
			* Inserimento del collo in col_prod
			*/
			InsertColProd(&Ordine,&Collo,Ordine.cStatoColloElaborato);
		
			/*
			* Inserimento sotto-ordine di produzione
			*/
			InsertOrdProd(&Ordine,Ordine.cStatoOrdineElaborato);
		} else {
		

/**************************************************************************************************
* FINE GESTIONE PDF
***************************************************************************************************/
			for(nRigaIndex=0,bFine=FALSE; nRigaIndex<nRigheOrdine && !bFine; nRigaIndex++){
				/*
				* metto i dati nella struttura riga
				*/
				strcpy(Riga.szCodiceProdotto,PQgetvalue(PGResRighe,nRigaIndex,0));
				nCopieRiga            = atoi(PQgetvalue(PGResRighe,nRigaIndex,1));
				Riga.nCopie           = atoi(PQgetvalue(PGResRighe,nRigaIndex,1));
				Riga.nCopieCaricate   = 0;
				Riga.nLength          = 0;
				Riga.nWidth           = 0;
				Riga.nHeight          = 0;
				Riga.nSwAltezza       = 0;
				Riga.nPesoCopia       = 0;
				Riga.nPesoTotale      = 0;
				Riga.nProgRiga        = 0;
				Riga.nSwFF            = 0;
				Riga.nSwPCF           = 0;
				Riga.nProdottoInPiedi = 0;
				/*
				 * BRI
				 */
				Riga.cFlagBarcodeIdentificativo=' ';

				/*
				* rm 01/11/2019 : prqtkar indica il numero copie in casi di prodotto da non spacchettare
				*/
				PGResCat=PGExecSQL(Cfg.nDebugVersion,"select prswffo, prqtpcf, prswbri,prqtkar from articoli where prcdpro='%s';",Riga.szCodiceProdotto);
				if(PQntuples(PGResCat)){
					nCopieFF      = atoi(PQgetvalue(PGResCat,0,0));
					nCopiePCF     = atoi(PQgetvalue(PGResCat,0,1));
					Riga.nPRQTKAR = atoi(PQgetvalue(PGResCat,0,3));
				} else {
					sprintf(szMsg,"ATTENZIONE : il prodotto [%s] non e' presente in catalogo\n",Riga.szCodiceProdotto);
					SendMessage(PROC_MAIN, PROC_VOLUM, DISPLAY_MSG, szMsg);
#ifdef TRACE
					trace_out_vstr(1,"Prodotto [%s] non presente in catalogo !",Riga.szCodiceProdotto); 
#endif
					bOK=FALSE;
				}
				PQclear(PGResCat);

				PGResUbi=PGExecSQL(Cfg.nDebugVersion,"select ubicazione, isola from ubicazioni where codprod='%s';",Riga.szCodiceProdotto);
				if(PQntuples(PGResUbi)){
					strcpy(Riga.szUbicazione,PQgetvalue(PGResUbi,0,0));
				} else {
					sprintf(szMsg,"ATTENZIONE : il prodotto [%s] non e' presente in ubicazioni\n",Riga.szCodiceProdotto);
					SendMessage(PROC_MAIN, PROC_VOLUM, DISPLAY_MSG, szMsg);
	#ifdef TRACE
					trace_out_vstr(1,"Prodotto [%s] non presente in ubicazioni !",Riga.szCodiceProdotto); 
	#endif
					bOK=FALSE;
				}
				PQclear(PGResUbi);

/* rm 7-12-2013 : parte non funzionante ... */
				if(0 && nCopiePCF){
					/*
					* ------------------------
					* Gestione Preconfezionato
					* ------------------------
					*/
					nColliPCF   = nCopieRiga / nCopiePCF;
					nRestoPCF   = nCopieRiga % nCopiePCF;
					Riga.nSwPCF = atoi(PQgetvalue(PGResRighe,nRigaIndex,4));
					strcpy(Riga.szFCPCF,PQgetvalue(PGResRighe,nRigaIndex,10));
					/*
					* se c'e' del resto creo una riga di produzione apposita
					*/
					Collo.nCollo=0;
					if(nRestoPCF){
						Riga.nCopie = nRestoPCF;
						if(!InsertRigProd(&Ordine,&Collo,&Riga,"rig_tmp",Ordine.cStatoRigaElaborata)){
							bFine=TRUE;
						}
					}
					/*
					* righe di produzione per il PCF
					*/
					Riga.nCopie = nCopiePCF;
					Collo.nCollo=0;
					for(nIndex=0;nIndex<nColliPCF;nIndex++){
						if(!InsertRigProd(&Ordine,&Collo,&Riga,"rig_tmp",Ordine.cStatoRigaElaborata)){
							bFine=TRUE;
						}
					}
					/* non funzionante ... */
				} else if(0 && nCopieFF){
					/*
					* ----------------------
					* Gestione Fuori Formato
					* ----------------------
					* PRSWFFO se > 0 individua un collo di fuori formato e contemporanemente 
					* fornisce il coefficente di colliFF per riga di produzione
					*/
					nColliFF=nCopieRiga/nCopieFF;
					nRestoFF=nCopieRiga%nCopieFF;
					Riga.nSwFF = atoi(PQgetvalue(PGResRighe,nRigaIndex,2));

					/*
					* se c'e' del resto creo una riga di produzione apposita
					*/
					if(nRestoFF){
						Riga.nCopie = nRestoFF;
						Collo.nCollo=0;
						if(!InsertRigProd(&Ordine,&Collo,&Riga,"rig_tmp",Ordine.cStatoRigaElaborata)){
							bFine=TRUE;
						}
					}
					/*
					* righe di produzione per colli di Fuori Formato (F)
					*/
					for(nIndex=0;nIndex<nColliFF;nIndex++){
						Riga.nCopie = nCopieFF;
						Collo.nCollo=0;
						if(!InsertRigProd(&Ordine,&Collo,&Riga,"rig_tmp",Ordine.cStatoRigaElaborata)){
							bFine=TRUE;
						}
					}
				} else if(Cfg.nNoUnpack==1 && Riga.nPRQTKAR > 1){
					/*
					* gestione non spacchettamento
					*/
					int nRigheKAR;
					int nCopieKAR;
					int nCopieResto;
					int nRigaKarIndex;

					if(Cfg.nPrelievoSingoloPacco){
						/*
						* se prelevo un singolo pacco alla volta allora creo piu' righe con copie = PRQTKAR
						*/
						nRigheKAR = Riga.nCopie / Riga.nPRQTKAR;
						nCopieKAR = Riga.nPRQTKAR;
					} else {
						/*
						* altrimenti prelevo un multiplo di copie in una sola riga (se possibile)
						*/
						nRigheKAR = Riga.nCopie < Riga.nPRQTKAR ? 0 : 1;
						nCopieKAR = Riga.nCopie - (Riga.nCopie % Riga.nPRQTKAR);
					}
					nCopieResto = Riga.nCopie % Riga.nPRQTKAR;


					/*
					* aggiungo un record all'archivio rig_tmp per ogni riga qtkar
					*/
					Collo.nCollo=0;
					for (nRigaKarIndex=0; nRigaKarIndex< nRigheKAR; nRigaKarIndex++){

						Riga.nCopie = nCopieKAR;;

						if(!InsertRigProd(&Ordine,&Collo,&Riga,"rig_tmp",Ordine.cStatoRigaElaborata)){
							bFine=TRUE;
						}
					}
					/*
					* aggiungo eventuale copie resto qtkar
					*/
					if(nCopieResto){
						Riga.nCopie = nCopieResto;

						if(!InsertRigProd(&Ordine,&Collo,&Riga,"rig_tmp",Ordine.cStatoRigaElaborata)){
							bFine=TRUE;
						}
					}

				} else {
					/*
					* ------------------------------------
					* Riga di linea automatica di prelievo
					* ------------------------------------
					* aggiungo un record all'archivio rig_tmp per ogni riga 
					*/
					Collo.nCollo=0;
					if(!InsertRigProd(&Ordine,&Collo,&Riga,"rig_tmp",Ordine.cStatoRigaElaborata)){
						bFine=TRUE;
					}
				}
			}

		}
		PQclear(PGResRighe);

		/*
		* ---------------------------------------------
		* FINE PRIMA SCANSIONE DELL'ANALISI VOLUMETRICA
		* ---------------------------------------------
		* SECONDA SCANSIONE DELL'ANALISI VOLUMETRICA
		* ---------------------------------------------
		*/
		if(bOK==FALSE){
#ifdef TRANSACTIONS_ON
			PGRes=PGExecSQL(Cfg.nDebugVersion,"ABORT WORK;");
			PQclear(PGRes);
#endif
			/*
			* Messaggio di fine ordine al MAIN
			*/
			SendMessage(PROC_MAIN, PROC_VOLUM, VOLUM_ERRORE_ANALISI_ORDINE, Spedizione.szOrdProg);
			continue;
		}

		/*
		* Creazione dei colli da archivio rig_prod
		*/

		/*
		* ricavo le isole interessate dall'ordine 
		*/
		PGResIsole=PGExecSQL(Cfg.nDebugVersion,"select distinct(u.isola) from ubicazioni as u, rig_tmp as r where r.ordprog='%s' and  u.ubicazione=r.rpcdubi;",Ordine.szOrdProg);
		nIsole=PQntuples(PGResIsole);
		for(nIsolaIndex=0;nIsolaIndex<nIsole;nIsolaIndex++){
			/*
			* reset dati ordine
			*/
			Ordine.nColli=0;
			Ordine.nColliAutomatici=0;
			Ordine.nRighe=0;
			Ordine.nPrese=0;
			Ordine.nCopie=0;
			Ordine.nPesoTotale=0;

			Collo.nCollo=0;

			Ordine.nIsola=atoi(PQgetvalue(PGResIsole,nIsolaIndex,0));
			Ordine.nSottoOrdine=1;

			/*
			* Da modificare : gestione sotto ordini con numero di colli minore o uguale a 6 
			*/
			sprintf(Ordine.szOrdKey,"%s.%d.%02d",Ordine.szOrdProg,Ordine.nIsola,Ordine.nSottoOrdine);

#ifdef TRACE
			trace_out_vstr(1,"Analisi Volumetrica Sotto Ordine [%s]",Ordine.szOrdKey);
			trace_out_vstr(1,"Collo F.to  Peso    Volume Vol.%% Toll.%%");
			trace_out_vstr(1,"----- ----- ------- ------ ----- ------");
#endif

			/*
			* Assegno il codice progressivo al primo collo da creare
			*/
			nCollo=1;

			/*
			* --------------------------------------------
			* Prima faccio la gestione del Preconfezionato
			* --------------------------------------------
			*/
			if(!bPallet){
				/*
				* Seleziono le righe relative all'ordine in oggetto
				* rm 01-11-2019 : prqtkar e'il numero di copie da non spacchettare
				*/
				PGResRighe=PGExecSQL(Cfg.nDebugVersion,"select \
					r.rpcdpro, r.rpqtspe, r.rpcdubi, r.rpswffo, r.rpswpcf,\
					a.prlungh, a.prlargh, a.praltez, a.prswalt, a.prpesgr, a.prfcpcf,a.prperic,a.prflgpp, a.prqtkar \
					from rig_tmp as r, articoli as a, ubicazioni as u \
					where r.ordprog='%s' and substr(r.rpcdubi,1,1)='%d' and a.prcdpro=r.rpcdpro and u.codprod=r.rpcdpro order by u.isola,u.priorita;",Ordine.szOrdProg,Ordine.nIsola);

				nRigheOrdine=PQntuples(PGResRighe);

				for(nRigaIndex=0;nRigaIndex<nRigheOrdine;nRigaIndex++){
					/*
					* metto i dati nella struttura riga
					*/
					strcpy(Riga.szCodiceProdotto,PQgetvalue(PGResRighe,nRigaIndex,0));
					Riga.nCopie           = atoi(PQgetvalue(PGResRighe,nRigaIndex,1));
					strcpy(Riga.szUbicazione,    PQgetvalue(PGResRighe,nRigaIndex,2));
					Riga.nSwFF            = atoi(PQgetvalue(PGResRighe,nRigaIndex,3));
					Riga.nSwPCF           = atoi(PQgetvalue(PGResRighe,nRigaIndex,4));
					Riga.nLength          = atoi(PQgetvalue(PGResRighe,nRigaIndex,5));
					Riga.nWidth           = atoi(PQgetvalue(PGResRighe,nRigaIndex,6));
					Riga.nHeight          = atoi(PQgetvalue(PGResRighe,nRigaIndex,7));
					Riga.nSwAltezza       = atoi(PQgetvalue(PGResRighe,nRigaIndex,8));
					Riga.nPesoCopia       = atoi(PQgetvalue(PGResRighe,nRigaIndex,9));
					strcpy(Riga.szFCPCF,         PQgetvalue(PGResRighe,nRigaIndex,10));
					strcpy(Riga.szPericoloso,    PQgetvalue(PGResRighe,nRigaIndex,11));
					Riga.nProdottoInPiedi = atoi(PQgetvalue(PGResRighe,nRigaIndex,12));
					Riga.nPRQTKAR         = atoi(PQgetvalue(PGResRighe,nRigaIndex,13));

					/*
					* ordino le dimensioni della riga
					*/
					Riga.nDim[0] = Riga.nLength;
					Riga.nDim[1] = Riga.nWidth;
					Riga.nDim[2] = Riga.nHeight;
					qsort(Riga.nDim,3,sizeof(int),(int(*)())CmpInt);

					/*
					* calcolo volume riga in cc (Centimetri Cubici)
					* e peso in grammi
					*/
					Riga.nVolumeCopia   = (Riga.nLength * Riga.nWidth * Riga.nHeight)/1000; 
					Riga.nVolumeTotale  = (Riga.nLength * Riga.nWidth * Riga.nHeight * Riga.nCopie)/1000; 
					Riga.nPesoTotale    = Riga.nPesoCopia * Riga.nCopie;
					Riga.nCopieCaricate = 0;

					if(Riga.nVolumeCopia==0){
						sprintf(szMsg,"ATTENZIONE : il prodotto [%s] ha volume NULLO\n",Riga.szCodiceProdotto);
						SendMessage(PROC_MAIN, PROC_VOLUM, DISPLAY_MSG, szMsg);
					}


					/*
					* Preconfezionato
					*/
					if(Riga.nSwPCF){
						CreazioneCollo(&Collo,nCollo,bPallet);

						Collo.nIndiceImballo=0;
						while(Collo.nIndiceImballo<Imballi.nImballi && !strcmp(Imballi.Imballo[Collo.nIndiceImballo].szCodice,Riga.szFCPCF)){
							Collo.nIndiceImballo++;	
						}
						strcpy(Collo.szCodiceImballo,"C");    /* tipo imballo 'C' */
						if(Collo.nIndiceImballo<Imballi.nImballi){
							/* codice imballo relativo - se presente*/
							strcat(Collo.szCodiceImballo,"-");
							strcat(Collo.szCodiceImballo,Imballi.Imballo[Collo.nIndiceImballo].szCodice);
						}

						Collo.nCopie = Riga.nCopie;
						Collo.nRighe = 1;
						/* rm 04-10-2006 */
						if(atoi(Riga.szPericoloso)){
							strcpy(Collo.szPericoloso,"LQ");
						}

						/*
						* Collo Manuale
						*/
						Collo.nSwLinea = 1;
						/* 
						* Aggiungo la tara al peso calcolato del collo 
						*/
						Collo.nPeso = Riga.nPesoTotale;
						Collo.nPeso+=Imballi.Imballo[Collo.nIndiceImballo].nTara;

						/*
						* Inserimento del collo in col_prod
						*/
						InsertColProd(&Ordine,&Collo,Ordine.cStatoColloElaborato);

						/*
						* Aggiorno i dati dell'ordine in corso di analisi
						*/
						Ordine.nColli++;
						if(Collo.nSwLinea!=1){
							Ordine.nColliAutomatici++;
						}
						Ordine.nRighe+=Collo.nRighe;
						Ordine.nCopie+=Collo.nCopie;
						Ordine.nPesoTotale+=Collo.nPeso;
#ifdef TRACE
						trace_out_vstr(1,"   %2d %-5.5s %7.2f %6d   %2d%%   %2d%%",
							Ordine.nColli-1,
							Collo.szCodiceImballo,
							(float)((float)Collo.nPeso/(float)1000),
							Collo.nVolume,
							(int)((float)(Collo.nVolume*100)/(float)Imballi.Imballo[Collo.nIndiceImballo].nVolume),
							Imballi.Imballo[Collo.nIndiceImballo].nVolPerc);
#endif

						InsertRigProd(&Ordine,&Collo,&Riga,"rig_prod",Ordine.cStatoRigaElaborata);
						/*
						* scarico le informazioni della riga prelevata
						*/
						Riga.nProgRiga++;

						/*
						* incremento il progressivo per il prossimo collo
						*/
						nCollo++;
					}
				}
				PQclear(PGResRighe);
			}

			/*
			* --------------------------------------------------------------
			* rm 05-01-1999 : Terminata la gestione del preconfezionato.
			*                 a questo punto passo alla gestione delle righe 
			*                 di produzione normali
			* --------------------------------------------------------------
			*/
			CreazioneCollo(&Collo,nCollo++,bPallet);

			/*
			* Seleziono le righe relative all'ordine in oggetto
			*/
			PGResRighe=PGExecSQL(Cfg.nDebugVersion,"select \
				r.rpcdpro, r.rpqtspe, r.rpcdubi, r.rpswffo, r.rpswpcf,\
				a.prlungh, a.prlargh, a.praltez, a.prswalt, a.prpesgr, a.prfcpcf, a.prqtprs,a.prperic,a.prflgpp, a.prqtkar \
				from rig_tmp as r, articoli as a, ubicazioni as u \
				where r.ordprog='%s' and substr(r.rpcdubi,1,1)='%d' and a.prcdpro=r.rpcdpro and u.codprod=r.rpcdpro order by u.isola,u.priorita,rpprogr;",Ordine.szOrdProg,Ordine.nIsola);

			nRigheOrdine=PQntuples(PGResRighe);

			for(nRigaIndex=0,bFine=FALSE;nRigaIndex<nRigheOrdine && !bFine;nRigaIndex++){

				/*
				* metto i dati nella struttura riga
				*/
				strcpy(Riga.szCodiceProdotto,PQgetvalue(PGResRighe,nRigaIndex,0));
				Riga.nCopie           = atoi(PQgetvalue(PGResRighe,nRigaIndex,1));
				strcpy(Riga.szUbicazione,    PQgetvalue(PGResRighe,nRigaIndex,2));
				Riga.nSwFF            = atoi(PQgetvalue(PGResRighe,nRigaIndex,3));
				Riga.nSwPCF           = atoi(PQgetvalue(PGResRighe,nRigaIndex,4));
				Riga.nLength          = atoi(PQgetvalue(PGResRighe,nRigaIndex,5));
				Riga.nWidth           = atoi(PQgetvalue(PGResRighe,nRigaIndex,6));
				Riga.nHeight          = atoi(PQgetvalue(PGResRighe,nRigaIndex,7));
				Riga.nSwAltezza       = atoi(PQgetvalue(PGResRighe,nRigaIndex,8));
				Riga.nPesoCopia       = atoi(PQgetvalue(PGResRighe,nRigaIndex,9));
				strcpy(Riga.szFCPCF,         PQgetvalue(PGResRighe,nRigaIndex,10));
				Riga.nPreseProdotto   = atoi(PQgetvalue(PGResRighe,nRigaIndex,11));
				strcpy(Riga.szPericoloso,    PQgetvalue(PGResRighe,nRigaIndex,12));
				Riga.nProdottoInPiedi = atoi(PQgetvalue(PGResRighe,nRigaIndex,13));
				Riga.nPRQTKAR         = atoi(PQgetvalue(PGResRighe,nRigaIndex,14));
				
				/* 
				* resetto prqtkar se si tratta del resto ...
				*/
				if(Riga.nPRQTKAR > 1 && Riga.nCopie < Riga.nPRQTKAR){
#ifdef TRACE
					trace_out_vstr_date(1,"Prodotto [%s] : prqtkar = %d e riga copie = %d - RESTO",Riga.szCodiceProdotto, Riga.nPRQTKAR, Riga.nCopie);
#endif
					Riga.nPRQTKAR = 0;
				}

				if(Riga.nPRQTKAR > 1 && Riga.nCopie > Riga.nPRQTKAR){
#ifdef TRACE
					trace_out_vstr_date(1,"Prodotto [%s] : prqtkar = %d e riga copie = %d",Riga.szCodiceProdotto, Riga.nPRQTKAR, Riga.nCopie);
#endif
				}

				if(Riga.nPRQTKAR > 1 && Riga.nCopie == Riga.nPRQTKAR){
#ifdef TRACE
					trace_out_vstr_date(1,"Prodotto [%s] : prqtkar = %d e riga copie = %d",Riga.szCodiceProdotto, Riga.nPRQTKAR, Riga.nCopie);
#endif
				}

				/*
				* calcolo prese
				*/
				if(Riga.nPreseProdotto){
					Ordine.nPrese+=(Riga.nCopie/Riga.nPreseProdotto)+((Riga.nCopie%Riga.nPreseProdotto)?1:0);
				}

				Riga.nDim[0] = Riga.nLength;
				Riga.nDim[1] = Riga.nWidth;
				Riga.nDim[2] = Riga.nHeight;

				if(Riga.nSwPCF){
					/*
					* se la riga e' di tipo PCF si tratta di un preconfezionato
					* e quindi e' gia' stata gestita in precedenza
					*/
					continue;
				}
				
				/*
				* ordino le dimensioni della riga
				*/
				qsort(Riga.nDim,3,sizeof(int),(int(*)())CmpInt);

				/*
				* calcolo volume riga in cc (Centimetri Cubici)
				*/
				Riga.nVolumeCopia   = (Riga.nLength * Riga.nWidth * Riga.nHeight)/1000; 
				Riga.nVolumeTotale  = (Riga.nLength * Riga.nWidth * Riga.nHeight * Riga.nCopie)/1000; 
				Riga.nPesoTotale    = Riga.nPesoCopia * Riga.nCopie;
				Riga.nCopieCaricate = 0;

				if(Riga.nVolumeCopia==0){
					sprintf(szMsg,"ATTENZIONE : il prodotto [%s] ha volume NULLO\n",Riga.szCodiceProdotto);
#ifdef TRACE
					trace_out_vstr_date(1,"Prodotto [%s] : volume NULLO",Riga.szCodiceProdotto);
#endif
					SendMessage(PROC_MAIN, PROC_VOLUM, DISPLAY_MSG, szMsg);
				}

				do {
					/*
					* Prelievo prodotti riga ed inserimento nel collo
					*/
					nRC=InsertRiga(&Riga,&Collo);

					switch(nRC){
						case VOL_PRELIEVO_OK:
							/*
							* scarico le informazioni della riga prelevata
							*/
							InsertRigProd(&Ordine,&Collo,&Riga,"rig_prod",Ordine.cStatoRigaElaborata);
							Riga.nProgRiga++;
							/* rm 04-10-2006 */
							if(atoi(Riga.szPericoloso)){
								strcpy(Collo.szPericoloso,"LQ");
							}
						break;
						case VOL_RIGA_SPEZZATA:	
						{
							int nCopieRimaste;

							nCopieRimaste = Riga.nCopie-Riga.nCopieCaricate;
							/* 
							* chiudo comunque il collo 
							* e in piu' creo una nuova riga di prelievo 
							*/
							/*
							* cambio i dati di numero copie della riga attuale
							*/
							Riga.nCopie=Riga.nCopieCaricate;
							Riga.nPesoTotale=Riga.nPesoCopia*Riga.nCopieCaricate;

							InsertRigProd(&Ordine,&Collo,&Riga,"rig_prod",Ordine.cStatoRigaElaborata);

							Riga.nProgRiga++;

							Riga.nCopie=nCopieRimaste;
							Riga.nPesoTotale=nCopieRimaste*Riga.nPesoCopia;
							Riga.nVolumeTotale=nCopieRimaste*Riga.nVolumeCopia;
							Riga.nCopieCaricate=0;

							/* rm 04-10-2006 */
							if(atoi(Riga.szPericoloso)){
								strcpy(Collo.szPericoloso,"LQ");
							}

							/*
							* passo al 'case' successivo per chiudere il collo
							*/
						}
						case VOL_CHIUDI_COLLO:
							if(Collo.nCopie==0 || Collo.nRighe==0){
#ifdef TRACE
								trace_out_vstr_date(1,"Ordine [%s] Collo [%d] VUOTO !",Ordine.szOrdKey,Collo.nCollo);
								/*
								* se il collo attuale e' vuoto
								* segnalo errore a video
								*/
								sprintf(szMsg,"ATTENZIONE : Ordine [%s] Collo [%d] VUOTO\n",Ordine.szOrdKey,Collo.nCollo);
								SendMessage(PROC_MAIN, PROC_VOLUM, DISPLAY_MSG, szMsg);
#endif
								bFine=TRUE;
							} else {

								strcpy(Collo.szCodiceImballo,Imballi.Imballo[Collo.nIndiceImballo].szCodice);
								Collo.nSwLinea=Imballi.Imballo[Collo.nIndiceImballo].nSwLinea;

								/* 
								* Aggiungo la tara al peso calcolato del collo 
								*/
								Collo.nPeso+=Imballi.Imballo[Collo.nIndiceImballo].nTara;
								InsertColProd(&Ordine,&Collo,Ordine.cStatoColloElaborato);

								/*
								* Aggiorno i dati dell'ordine in corso di analisi
								*/
								Ordine.nColli++;
								if(Collo.nSwLinea!=1){
									Ordine.nColliAutomatici++;
								}
								Ordine.nRighe+=Collo.nRighe;
								Ordine.nCopie+=Collo.nCopie;
								Ordine.nPesoTotale+=Collo.nPeso;
#ifdef TRACE
								if(Imballi.Imballo[Collo.nIndiceImballo].nVolume){
									trace_out_vstr(1,"   %2d %-5.5s %7.2f %6d   %2d%%   %2d%%",
										Ordine.nColli-1,
										Collo.szCodiceImballo,
										(float)((float)Collo.nPeso/(float)1000),
										Collo.nVolume,
										(int)((float)(Collo.nVolume*100)/(float)Imballi.Imballo[Collo.nIndiceImballo].nVolume),
										Imballi.Imballo[Collo.nIndiceImballo].nVolPerc);
								} else {
									trace_out_vstr(1,"   %2d %-5.5s %7.2f",
										Ordine.nColli-1,
										Collo.szCodiceImballo,
										(float)((float)Collo.nPeso/(float)1000));
								}
#endif


								/*
								* Se il numero colli e' superiore al numero
								* massimo di colli per sotto-ordine (impostato per WELLA a 6)
								* chiudo l'ordine e ne apro un altro
								*/
								if(Cfg.nMaxColliOrdine && Ordine.nColli>=Cfg.nMaxColliOrdine){
									/*
									* Creo il nuovo sottoordine
									*/
									InsertOrdProd(&Ordine,Ordine.cStatoOrdineElaborato);
#ifdef TRACE
									trace_out_vstr(1,"----- ----- ------- ------ ----- ------");
									trace_out_vstr(1,"Fine Analisi Sotto Ordine [%s] (MAX_COLLI)", Ordine.szOrdKey);
									trace_out_vstr(1,"Numero Colli   : %7d",Ordine.nColli);
									trace_out_vstr(1,"Numero Righe   : %7d",Ordine.nRighe);
									trace_out_vstr(1,"Numero Prese   : %7d",Ordine.nPrese);
									trace_out_vstr(1,"Numero Copie   : %7d",Ordine.nCopie);
									trace_out_vstr(1,"Peso Totale    : %7.2f",(float)((float)Ordine.nPesoTotale/(float)1000));
#endif
									/*
									* Aggiornamento Dati Spedizione
									*/
									Spedizione.nColli+=Ordine.nColli;
									Spedizione.nRighe+=Ordine.nRighe;
									Spedizione.nCopie+=Ordine.nCopie;
									Spedizione.nPesoTotale+=Ordine.nPesoTotale;
									Spedizione.nSottoOrdini++;

									/*
									* reset dati ordine
									*/
									Ordine.nColli=0;
									Ordine.nColliAutomatici=0;
									Ordine.nRighe=0;
									Ordine.nPrese=0;
									Ordine.nCopie=0;
									Ordine.nPesoTotale=0;
									/*
									* Incremento il sottoordine
									*/
									Ordine.nSottoOrdine++;

									Collo.nCollo=0;

									sprintf(Ordine.szOrdKey,"%s.%d.%02d",Ordine.szOrdProg,Ordine.nIsola,Ordine.nSottoOrdine);

#ifdef TRACE
									trace_out_vstr(1,"Analisi Volumetrica Sotto Ordine [%s]",Ordine.szOrdKey);
									trace_out_vstr(1,"Collo F.to  Peso    Volume Vol.%% Toll.%%");
									trace_out_vstr(1,"----- ----- ------- ------ ----- ------");
#endif

									/*
									* Assegno il codice progressivo al primo collo da creare
									*/
									nCollo=1;

								}

								/*
								* conteggio volume copie contenute nel collo
								*/
								CreazioneCollo(&Collo,nCollo++,bPallet);
							}
							Riga.nProgRiga=0;
						break;
						case VOL_COLLO_FF_NON_PRESENTE:
							/*
							* ERRORE IRREVERSIBILE
							* segnalare all'operatore
							*/
							/*
							* Messaggio di errore su elaborazione ordine al MAIN
							*/
							sprintf(szMsg,"Ordine [%s],Tipo Collo Fuori Formato non presente in archivio imballi",Ordine.szOrdProg);
							SendMessage(PROC_MAIN, PROC_VOLUM, VOLUM_ERRORE_ANALISI_ORDINE, szMsg);
							bFine=TRUE;
						break;
						case VOL_DIM_PROD_ERRATE:
							/*
							* ERRORE IRREVERSIBILE
							* segnalare all'operatore
							*/
							/*
							* Messaggio di errore su elaborazione ordine al MAIN
							*/
							sprintf(szMsg,"Ordine [%s],Dimensioni Prodotto [%s] errate",Ordine.szOrdProg,Riga.szCodiceProdotto);
							SendMessage(PROC_MAIN, PROC_VOLUM, VOLUM_ERRORE_ANALISI_ORDINE, szMsg);
							bFine=TRUE;
						break;
						case VOL_DIM_PROD_ERRATE_ALTEZZA:
							/*
							* ERRORE IRREVERSIBILE
							* segnalare all'operatore
							*/
							/*
							* Messaggio di errore su elaborazione ordine al MAIN
							*/
							sprintf(szMsg,"Ordine [%s],Altezza [%s] errata",Ordine.szOrdProg,Riga.szCodiceProdotto);
							SendMessage(PROC_MAIN, PROC_VOLUM, VOLUM_ERRORE_ANALISI_ORDINE, szMsg);
							bFine=TRUE;
						break;
					}
				} while (bFine==FALSE && (nRC==VOL_RIGA_SPEZZATA || nRC==VOL_CHIUDI_COLLO));
			}
			PQclear(PGResRighe);
			/*
			* chiusura ultimo collo 
			*/
			if(Collo.nCopie==0 || Collo.nRighe==0){
#ifdef TRACE
				trace_out_vstr_date(1,"Ordine [%s] Collo [%d] VUOTO !",Ordine.szOrdKey,Collo.nCollo);
				/*
				* se il collo attuale e' vuoto
				* segnalo errore a video
				*/
				sprintf(szMsg,"ATTENZIONE : Ordine [%s] Collo [%d] VUOTO\n",Ordine.szOrdKey,Collo.nCollo);
				SendMessage(PROC_MAIN, PROC_VOLUM, DISPLAY_MSG, szMsg);
#endif
			} else {

				strcpy(Collo.szCodiceImballo,Imballi.Imballo[Collo.nIndiceImballo].szCodice);
				Collo.nSwLinea=Imballi.Imballo[Collo.nIndiceImballo].nSwLinea;

				/* 
				* Aggiungo la tara al peso calcolato del collo 
				*/
				Collo.nPeso+=Imballi.Imballo[Collo.nIndiceImballo].nTara;
				InsertColProd(&Ordine,&Collo,Ordine.cStatoColloElaborato);

				/*
				* Aggiorno i dati dell'ordine in corso di analisi
				*/
				Ordine.nColli++;
				if(Collo.nSwLinea!=1){
					Ordine.nColliAutomatici++;
				}
				Ordine.nRighe+=Collo.nRighe;
				Ordine.nCopie+=Collo.nCopie;
				Ordine.nPesoTotale+=Collo.nPeso;
#ifdef TRACE
				if(Imballi.Imballo[Collo.nIndiceImballo].nVolume){
					trace_out_vstr(1,"   %2d %-5.5s %7.2f %6d   %2d%%   %2d%%",
						Ordine.nColli-1,
						Collo.szCodiceImballo,
						(float)((float)Collo.nPeso/(float)1000),
						Collo.nVolume,
						(int)((float)(Collo.nVolume*100)/(float)Imballi.Imballo[Collo.nIndiceImballo].nVolume),
						Imballi.Imballo[Collo.nIndiceImballo].nVolPerc);
				} else {
					trace_out_vstr(1,"   %2d %-5.5s %7.2f",
						Ordine.nColli-1,
						Collo.szCodiceImballo,
						(float)((float)Collo.nPeso/(float)1000));
				}
#endif
			}

			
			/*
			* Inserimento sotto-ordine di produzione
			*/
			InsertOrdProd(&Ordine,Ordine.cStatoOrdineElaborato);

#ifdef TRACE
			trace_out_vstr(1,"----- ----- ------- ------ ----- ------");
			trace_out_vstr(1,"Fine Analisi Sotto Ordine [%s]", Ordine.szOrdKey);
			trace_out_vstr(1,"Numero Colli   : %7d",Ordine.nColli);
			trace_out_vstr(1,"Numero Righe   : %7d",Ordine.nRighe);
			trace_out_vstr(1,"Numero Prese   : %7d",Ordine.nPrese);
			trace_out_vstr(1,"Numero Copie   : %7d",Ordine.nCopie);
			trace_out_vstr(1,"Peso Totale    : %7.2f",(float)((float)Ordine.nPesoTotale/(float)1000));
#endif


			Spedizione.nColli+=Ordine.nColli;
			Spedizione.nRighe+=Ordine.nRighe;
			Spedizione.nCopie+=Ordine.nCopie;
			Spedizione.nPesoTotale+=Ordine.nPesoTotale;
			Spedizione.nSottoOrdini++;
		}
		PQclear(PGResIsole);
#ifdef TRACE
		trace_out_vstr(1,"=================================================");
		trace_out_vstr(1,"Fine Analisi Spedizione [%s]", Spedizione.szOrdProg);
		trace_out_vstr(1,"Numero Ordini  : %7d",Spedizione.nSottoOrdini);
		trace_out_vstr(1,"Numero Colli   : %7d",Spedizione.nColli);
		trace_out_vstr(1,"Numero Righe   : %7d",Spedizione.nRighe);
		trace_out_vstr(1,"Numero Copie   : %7d",Spedizione.nCopie);
		trace_out_vstr(1,"Peso Totale    : %7.2f",(float)((float)Spedizione.nPesoTotale/(float)1000));
#endif


/**************************************************************************************************
* GESTIONE PDF
***************************************************************************************************/

		/*
		* Faccio un giro sui colli dell'ordine per settare l'ultimo collo utile del sotto-ordine con pdf
		* per un'eventuale stampa del pdf
		* Inoltre segno il sotto-ordine col flag pdf della spedizione. Gli altri sotto-ordini li marco
		* invece con ORDINE_SENZA_PDF
		* Collo automatico se cpswlin=0
		*/
		/*
		* Seleziono la spedizione con ordkey minore
		*/
		PGResColli=PGExecSQL(Cfg.nDebugVersion, "select min(ordkey) from col_prod where ordprog='%s' and cpswlin='0';",Spedizione.szOrdProg);
		nTuples=PQntuples(PGResColli);
		if(nTuples){
			strcpy(szOrdKey,PQgetvalue(PGResColli,0,0));
		}
		PQclear(PGResColli);
		
		PGResColli=PGExecSQL(Cfg.nDebugVersion, "select max(cpnmcol) from col_prod where ordkey='%s';",szOrdKey);

		nTuples=PQntuples(PGResColli);
		if(nTuples){
			PGresult *PGResUpdate;
			nCollo=atoi(PQgetvalue(PGResColli,0,0));
			if (nCollo){
				PGResUpdate=PGExecSQL(Cfg.nDebugVersion,"update col_prod set cpflpdf='%c' where ordkey='%s' and cpnmcol=%d;",
					COLLO_CON_PDF,szOrdKey,nCollo);
				PQclear(PGResUpdate);
#ifdef TRACE
				trace_out_vstr_date(1,"Ordine [%s]: PDF assegnato al Collo [%d]  !",szOrdKey,nCollo);
#endif
				//if (nCollo==1) 
				if (nCollo==1 && Spedizione.cFlagPdf!=ORDINE_SENZA_PDF) {
					/*
					* NB: importantissimo
					*/
					// Se il sotto-ordine ha un solo collo allora va nello stato incompleto terminato 
					// Chiaramente solo se aspetta un pdf altrimenti va nel normale stato A. Ecco perche'
					// nell'if precedente viene valutato se la spedizione e' forzata senza Pdf
					// 2012-10-29 LG: aggiunto il Cfg.nAttesaPDFOrdineCompleto alle clausole
					if (!Cfg.nAttesaPDFOrdineCompleto) {
						if(Spedizione.cStato!=ORDINE_SPEDITO_DA_CONSUNTIVARE){
							Ordine.cStatoOrdineElaborato=ORDINE_INCOMPLETO_TERMINATO;
						}
					}
				}else{
				}
				if(Spedizione.cStato!=ORDINE_SPEDITO_DA_CONSUNTIVARE){
					/*
					* ricavo il valore attuale di roflpdf e lo assegno a ord_prod
					*/
					Spedizione.cFlagPdf=ORDINE_SENZA_PDF;
					PGResPDF=PGExecSQL(Cfg.nDebugVersion,"select roflpdf from ric_ord where ordprog='%s';",Spedizione.szOrdProg);
					if(PQntuples(PGResPDF)){
						strcpy(szBuffer,PQgetvalue(PGResPDF,0,0));
						Spedizione.cFlagPdf=szBuffer[0];
					}
					PQclear(PGResPDF);

					PGResUpdate=PGExecSQL(Cfg.nDebugVersion,"update ord_prod set opstato='%c',opflpdf='%c' where ordkey='%s';",
						Ordine.cStatoOrdineElaborato,Spedizione.cFlagPdf,szOrdKey);
					PQclear(PGResUpdate);
					PGResUpdate=PGExecSQL(Cfg.nDebugVersion,"update ord_prod set opflpdf='%c' where ordprog='%s' and ordkey!='%s';",
						ORDINE_SENZA_PDF,Spedizione.szOrdProg,szOrdKey);
					PQclear(PGResUpdate);
				}
			} else {
			}
		} else {
		}
		PQclear(PGResColli);

/**************************************************************************************************
* FINE GESTIONE PDF
***************************************************************************************************/

		/*
		* Aggiorno i dati di spedizione (ordine ricevuto) 
		*/
		UpdateSpedizione(&Spedizione,Ordine.cStatoOrdineElaborato);
#ifdef TRANSACTIONS_ON
		PGRes=PGExecSQL(Cfg.nDebugVersion,"COMMIT WORK;");
		PQclear(PGRes);
#endif


		/*
		* ----------------------------------------------------------------------------------
		* rm 21-02-2004 : assegno segnacollo in questa fase 
		*                 per potere ottenere il dato in fase di consuntivazione anticipata
		* ----------------------------------------------------------------------------------
		*/
		PGResColli=PGExecSQL(Cfg.nDebugVersion, "select ordprog,ordkey,cpnmcol,cpprgcl from col_prod where ordprog='%s' order by ordprog,ordkey,cpnmcol;",Spedizione.szOrdProg);

		for(nColloIndex=0;nColloIndex<PQntuples(PGResColli);nColloIndex++){
			nCollo=atoi(PQgetvalue(PGResColli,nColloIndex,2));
			strcpy(szOrdKey,PQgetvalue(PGResColli,nColloIndex,1));

			/*
			* Calcolo del progressivo collo
			*  se il contenuto numerico nell'archivio e' 0 :
			*   si incrementa il progressivo (nextval)
			*   si assegna il progressivo nell'archivio (update)
			*/
			if (atoi(PQgetvalue(PGResColli,nColloIndex,3))==0){
				int nProgCollo;
				PGresult *PGResSegnacollo;

				/*
				* 15-09-2005 rm
				* utilizzo il valore del primo segnacollo se presente 
				*/
				if(nProgressivoCollo==0){
					/*
					* Il progressivo non e' mai stato registrato e quindi lo calcolo e lo assegno
					*/

					PGResSegnacollo=PGExecSQL(TRUE,"select nextval ('progcollo');");
					nProgCollo=atoi(PQgetvalue(PGResSegnacollo,0,0));
					PQclear(PGResSegnacollo);

					PGResCollo=PGExecSQL(Cfg.nDebugVersion,"update col_prod set cpprgcl=%d where ordkey='%s' and cpnmcol=%d;",
						nProgCollo,szOrdKey,nCollo);
					PQclear(PGResCollo);
				} else {
					/*
					* 15-09-2005 rm
					* Il progressivo e' stato memorizzato in precedenza
					*/
					PGResCollo=PGExecSQL(Cfg.nDebugVersion,"update col_prod set cpprgcl=%d where ordkey='%s' and cpnmcol=%d;",
						nProgressivoCollo,szOrdKey,nCollo);
					PQclear(PGResCollo);

					/* 
					* 15-09-2005 rm
					* incremento il progressivo collo
					* se il numero colli ricalcolati e' maggiore del numero colli precedente
					* attingo dalla sequenza progcollo_add per evitare 'sovrapposizioni'
					*/
					if(nColloIndex>=nColliSpedizione){
						PGResSegnacollo=PGExecSQL(TRUE,"select nextval ('progcollo_add');");
						nProgressivoCollo=atoi(PQgetvalue(PGResSegnacollo,0,0));
						PQclear(PGResSegnacollo);
					} else {
						nProgressivoCollo++;
					}
				}
			}

		}
		PQclear(PGResColli);

		/*
		* 2015-12-01 lg:
		* Assegnamento dei flag BRI ai colli interessati
		*/
		SetRigheOrdineConControlloBarcode(Spedizione.szOrdProg);

		/* 
		* 2015-12-18 lg: Modifica per gestire il flag di imballo speciale
		*                Se esiste almeno un prodotto, all'interno dell'intero ordine, che ha il flag di speciale (prswpsp)
		*                allora tutti i colli dell'ordine vengono flaggati con imballo speciale. 
		*/
		SetColliOrdineConImballoSpeciale(Spedizione.szOrdProg);
		
		/*
		* Messaggio di fine analisi ordine al MAIN
		*/
		SendMessage(PROC_MAIN, PROC_VOLUM, VOLUM_END_ANALISI_ORDINE, Spedizione.szOrdProg);
	}
	PQclear(PGResOrdini);

	return TRUE;
}

/*
* Funzione per l'assegnamento delle righe e dei colli interessati al
* controllo dei barcode ID
*/
BOOL SetRigheOrdineConControlloBarcode(char *szOrdProg)
{
	PGresult *PGRes;
	PGresult *PGResCollo;
	char szCDPRO[80];
	int nQTBRI=0;
	int nIndex=0;
	BOOL bOK=TRUE;
#ifdef TRACE
	trace_out_vstr_date(1,"SETTAGGIO RIGHE CON CONTROLLO BARCODE");
#endif

	/* Cerco le righe degli ordini in cui un prodotto marchiato BRI superi la soglia */
	PGRes=PGExecSQL(Cfg.nDebugVersion, "select ordprog,rpcdpro,sum(rpqtspe) from rig_prod,articoli where prcdpro=rpcdpro and ordprog='%s' and prswbri='%c' group by ordprog,rpcdpro having sum(rpqtspe)>=%d;"
		,szOrdProg,CONTROLLO_BARCODE_ON,Cfg.nSogliaControlloBarcode);
	/* Per ogni riga dell'ordine con prodotto BRI e soglia ... */
	if (PQresultStatus(PGRes) == PGRES_TUPLES_OK) {
		for(nIndex=0;nIndex<PQntuples(PGRes);nIndex++) {
			strcpy(szCDPRO,PQgetvalue(PGRes,nIndex,1));
			nQTBRI=atoi(PQgetvalue(PGRes,nIndex,2));

#ifdef TRACE
			trace_out_vstr_date(1,"Prodotto [%s] su ordine [%s] da controllare !",szCDPRO,szOrdProg);
#endif

			PGResCollo=PGExecSQL(Cfg.nDebugVersion,"update rig_prod set rpflbri='%c' where ordprog='%s' and rpcdpro='%s';"
				,CONTROLLO_BARCODE_INCOMPLETO,szOrdProg,szCDPRO);
			if (PQresultStatus(PGResCollo)!=PGRES_COMMAND_OK) {
				bOK=FALSE;
			}
			PQclear(PGResCollo);
		}
		/* Per ogni riga per la quale ho settato il flag BRI setto il flag BRI del collo, per il controllo su bilancia */
		if (PQntuples(PGRes)) {
			PGResCollo=PGExecSQL(Cfg.nDebugVersion,"update col_prod set cpflbri='%c' where ordprog='%s' and cpnmcol in (select distinct(rpnmcol) from rig_prod where ordprog='%s' and rpflbri='%c');",
				CONTROLLO_BARCODE_INCOMPLETO,szOrdProg,szOrdProg,CONTROLLO_BARCODE_INCOMPLETO);
			if (PQresultStatus(PGResCollo)!=PGRES_COMMAND_OK) {
				bOK=FALSE;
			}
			PQclear(PGResCollo);
		}
	} else {
		bOK=FALSE;
	}
	PQclear(PGRes);

	return bOK;
}

/*
* Funzione per l'assegnamento del flag ai colli degli
* ordini contenenti prodotti per imballo speciale
*/
BOOL SetColliOrdineConImballoSpeciale(char *szOrdProg)
{
	PGresult *PGRes;
	PGresult *PGResCollo;
	BOOL bOK=TRUE;
#ifdef TRACE
	trace_out_vstr_date(1,"SETTAGGIO COLLI CON IMBALLO SPECIALE");
#endif

	/* Cerco le righe degli ordini in cui vi sia almeno un prodotto marchiato speciale */
	PGRes=PGExecSQL(Cfg.nDebugVersion, "select ordprog,rpcdpro from rig_prod,articoli where prcdpro=rpcdpro and ordprog='%s' and prswpsp='%c' group by ordprog,rpcdpro ;"
		,szOrdProg,IMBALLO_SPECIALE_ON);
	/* Se esiste almeno una riga all'interno dell'ordine tutti i colli vanno marchiati speciali */
	if (PQresultStatus(PGRes) == PGRES_TUPLES_OK) {
		if (PQntuples(PGRes)) {
			PGResCollo=PGExecSQL(Cfg.nDebugVersion,"update col_prod set cpflisp='%c' where ordprog='%s';"
				,IMBALLO_SPECIALE_ON,szOrdProg);
			if (PQresultStatus(PGResCollo)!=PGRES_COMMAND_OK) {
				bOK=FALSE;
			}
			PQclear(PGResCollo);
		}
	} else {
		bOK=FALSE;
	}
	PQclear(PGRes);

	return bOK;
}

/*
* CreazioneCollo()
* apre un nuovo collo (il piu' piccolo)
* se bPallet=TRUE allora apre un collo di tipo PALLET
*/
void CreazioneCollo(PCOLLO_ORDINE pCollo,int nCollo,BOOL bPallet)
{
	pCollo->nIndiceImballo=0;

	if(bPallet){
		while(pCollo->nIndiceImballo<Imballi.nImballi && Imballi.Imballo[pCollo->nIndiceImballo].szCodice[0]!=COLLO_PALLET){
			pCollo->nIndiceImballo++;	
		}
	}
	/*
	* gestire l'errore di collo tipo PALLET non esistente
	*/
	if(pCollo->nIndiceImballo>=Imballi.nImballi){
		pCollo->nIndiceImballo=0;
	}
	strcpy(pCollo->szCodiceImballo,Imballi.Imballo[pCollo->nIndiceImballo].szCodice);

	pCollo->nCollo=nCollo;
	pCollo->nVolume=0;
	pCollo->nPeso=0;
	pCollo->nCopie=0;
	pCollo->nRighe=0;
	strcpy(pCollo->szPericoloso,"");
}

/*
* Creazione del Barcode collo
* 20 caratteri 
*/
char *ColloBarcode(PORDINE pOrdine,PCOLLO_ORDINE pCollo)
{
	static char szBuffer[80];
	char szNumCollo[80];

	sprintf(szNumCollo,"%03d",pCollo->nCollo);
	/*
	* data eliminata per WELLA
	*/
	strcpy(szBuffer,GetDateYYYYMMDD(time((long *)0), szDateBuffer)); /* Data         8 caratteri */
	strcpy(szBuffer,pOrdine->szOrdProg);                             /* Spedizione   6 caratteri */
	if(Cfg.nSAP){
		strcat(szBuffer,SubStr(pOrdine->szOrdKey,11,1));                  /* Isola        1 caratteri */
		strcat(szBuffer,SubStr(pOrdine->szOrdKey,13,2));                  /* SottoOrdine  2 caratteri */
	} else {
		strcat(szBuffer,SubStr(pOrdine->szOrdKey,7,1));                  /* Isola        1 caratteri */
		strcat(szBuffer,SubStr(pOrdine->szOrdKey,9,2));                  /* SottoOrdine  2 caratteri */
	}
	strcat(szBuffer,szNumCollo);                                     /* Numero Collo 3 caratteri */

	return (szBuffer);
}


/*
* InsertRiga 
* Cerca di inserire la riga nel collo
* tiene conto dei vincoli dimensionali e di peso .
* se il prodotto e' fuori formato puo' essere presente in un unica copia in un apposito collo Fuori Formato
*
* 16-10-1998 rm : ore 19:30 presso CLIENTE !! Controllo Peso ...
* 29-11-2000 rm : ore 20:30 presso CLIENTE !! Gestione Prese ...
* 01-11-2019 tm : gestione prqtkar - non spacchettamento 
*/
int InsertRiga(PRIGA_ORDINE pRiga,PCOLLO_ORDINE pCollo)
{
	int nRigaCopieCollo=0;
	BOOL bFine;

	if(Cfg.nNoUnpack==0 || (pRiga->nPRQTKAR>1 && pRiga->nCopie < pRiga->nPRQTKAR)){
		/* 
		* se non sono in gestione pacchetto oppure
		* se le copie riga sono minori di qtkar allora non gestisco comunque il pacco 
		*/
#ifdef TRACE
		trace_out_vstr(1,"Copie < prqtkar- metto prqtkar = 0");
#endif
		pRiga->nPRQTKAR = 0;	
	}

	if(Imballi.Imballo[pCollo->nIndiceImballo].szCodice[0]!=COLLO_PALLET && (pRiga->nSwFF==1 || pRiga->nSwFF==2)){
		/*
		* articolo fuori formato
		* creo un collo a parte di tipo 'COLLO_FUORI_FORMATO'
		*/
		if(pCollo->nCopie){
			/*
			* chiudo il collo precedente se contiene delle copie
			*/
			if(Imballi.Imballo[pCollo->nIndiceImballo].szCodice[0]!=COLLO_FUORI_FORMATO){
				char szMsg[128];

				/*
				* se il collo attuale non e' un fuori formato
				* segnalo errore a video
				*/
				sprintf(szMsg,"ATTENZIONE : Fuori Formato [%s] Ubicato Linea Automatica\n",pRiga->szCodiceProdotto);
				SendMessage(PROC_MAIN, PROC_VOLUM, DISPLAY_MSG, szMsg);
			}

			return VOL_CHIUDI_COLLO;
		} else {
			pCollo->nIndiceImballo=0;
			while(pCollo->nIndiceImballo<Imballi.nImballi && Imballi.Imballo[pCollo->nIndiceImballo].szCodice[0]!=COLLO_FUORI_FORMATO){
				pCollo->nIndiceImballo++;	
			}
			/*
			* non trovato il tipo collo Fuori Formato
			* segnalare 
			*/
			if(pCollo->nIndiceImballo == Imballi.nImballi){
				return VOL_COLLO_FF_NON_PRESENTE;
			}
			/*
			* riempi l'imballo fuori formato con le copie della riga
			* (se la riga contiene piu' di una copia occorre formare piu' di un collo)
			*/
		}
	} else {
		/*
		* se collo FF chiudi collo
		*/
		if(Imballi.Imballo[pCollo->nIndiceImballo].szCodice[0]==COLLO_FUORI_FORMATO){
			/*
			* chiudo il collo precedente se era un Fuori Formato
			*/
			return VOL_CHIUDI_COLLO;
		}
	}
	if(pCollo->nIndiceImballo>=Imballi.nImballiAutomatici){
		/*
		* collo manuale (Fuori Formato o Pallet)
		*/
		switch(Imballi.Imballo[pCollo->nIndiceImballo].szCodice[0]){
			case COLLO_PALLET:
				/*
				* provo a farci stare una copia alla volta (dimensioni singolo prodotto / peso)
				* se il collo non e' il piu' grande cambio il tipo imballo
				*/
				nRigaCopieCollo=0;
				bFine=FALSE;
				while(!bFine){
					/*
					* se ho finito le copie mi fermo (ci stanno tutte)
					*/
					if(nRigaCopieCollo==pRiga->nCopie){
						bFine=TRUE;
						continue;
					}
					/* 
					* controllo peso 
					*/
					if(pCollo->nPeso+pRiga->nPesoCopia*(nRigaCopieCollo+max(1,pRiga->nPRQTKAR)) > Imballi.Imballo[pCollo->nIndiceImballo].nPesoMax){
						/* 
						* mi fermo perche' il peso e' gia troppo alto
						*/
						bFine=TRUE;
						continue;
					}
					if( pCollo->nVolume+pRiga->nVolumeCopia*(nRigaCopieCollo+max(1,pRiga->nPRQTKAR)) > Imballi.Imballo[pCollo->nIndiceImballo].nVolumeUtile){

						bFine=TRUE;
						continue;
					}
					if(pRiga->nPRQTKAR > 1){
						/* 
						* se non posso spacchettare allora provo con tutte le copie della riga (gia' gestita in precedenza )
						*/
						nRigaCopieCollo+=pRiga->nPRQTKAR;
					} else {
						nRigaCopieCollo++;
					}
				}
				/* fine del riempimento pallet */
				if(nRigaCopieCollo==pRiga->nCopie){
					/*
					* ci sta la riga completa, carico il collo e passo alla prossima
					*/
					pRiga->nCopieCaricate = pRiga->nCopie;
					pCollo->nVolume += pRiga->nVolumeTotale;
					pCollo->nPeso   += pRiga->nPesoTotale;
					pCollo->nCopie  += pRiga->nCopie;
					pCollo->nRighe++;

#ifdef TRACE_ELIMINATO
					trace_out_vstr(1,"PP-P:[%s]: %3d copie %4dx%4dx%4d = %6d - V.C: %8d V.U: %8d",
						pRiga->szCodiceProdotto,
						pRiga->nCopieCaricate,
						pRiga->nLength,
						pRiga->nWidth,
						pRiga->nHeight,
						(pRiga->nLength* pRiga->nWidth* pRiga->nHeight * pRiga->nCopieCaricate)/1000,
						pCollo->nVolume,Imballi.Imballo[pCollo->nIndiceImballo].nVolumeUtile);
#endif
					return VOL_PRELIEVO_OK;
				} else if(nRigaCopieCollo){
					/*
					* a questo punto chiudo il collo e ne apro un altro
					*/
					pRiga->nCopieCaricate=nRigaCopieCollo;
					pCollo->nVolume += (nRigaCopieCollo * pRiga->nVolumeCopia);
					pCollo->nPeso   += nRigaCopieCollo * pRiga->nPesoCopia;
					pCollo->nCopie  += nRigaCopieCollo;
					pCollo->nRighe++;
#ifdef TRACE_ELIMINATO
				trace_out_vstr(1,"SP-P:[%s]: %3d copie %4dx%4dx%4d = %6d - V.C: %8d V.U: %8d",
					pRiga->szCodiceProdotto,
					pRiga->nCopieCaricate,
					pRiga->nLength,
					pRiga->nWidth,
					pRiga->nHeight,
					(pRiga->nLength* pRiga->nWidth* pRiga->nHeight * pRiga->nCopieCaricate)/1000,
					pCollo->nVolume,Imballi.Imballo[pCollo->nIndiceImballo].nVolumeUtile);
#endif
					return VOL_RIGA_SPEZZATA;
				} else {
					/*
					* non ci sta neanche una copia : chiudo il collo e apro il successivo
					*/
					return VOL_CHIUDI_COLLO;
				}
			break;
			case COLLO_FUORI_FORMATO:
				/*
				* carico una copia alla volta
				*/
				pRiga->nCopieCaricate=1;
				pCollo->nVolume+=pRiga->nVolumeCopia;
				pCollo->nPeso+=pRiga->nPesoCopia;
				/*
				* rm 19-10-2000
				* Attenzione : considerare il fuori formato a una o PIU' copie ...
				*/
				pCollo->nCopie+=1;
				pCollo->nRighe++;

#ifdef TRACE_ELIMINATO
				trace_out_vstr(1,"FF-P:[%s]: %3d copie %4dx%4dx%4d = %6d - V.C: %8d",
					pRiga->szCodiceProdotto,
					pRiga->nCopieCaricate,
					pRiga->nLength,
					pRiga->nWidth,
					pRiga->nHeight,
					(pRiga->nLength* pRiga->nWidth* pRiga->nHeight * pRiga->nCopieCaricate)/1000,
					pCollo->nVolume);
#endif

				if(pRiga->nCopieCaricate==pRiga->nCopie){
					return VOL_PRELIEVO_OK;
				} else {
					return VOL_RIGA_SPEZZATA;
				}
			break;
		}
	}
	/* rm 12-03-2013 : gestione altezza prodotto */
	if(pRiga->nProdottoInPiedi){
		if( Imballi.Imballo[Imballi.nImballiAutomatici-1].nHeight<pRiga->nHeight ){
			/*
			* l'articolo non ci sta IN ALTEZZA nell'imballo piu' grande.
			* Se non ci sono imballi per contenere l'articolo (troppo grande)
			* ritorno : VOL_DIM_PROD_ERRATE_ALTEZZA;
			* (errore non reversibile : blocco della analisi volumetrica)
			*/
			return VOL_DIM_PROD_ERRATE_ALTEZZA;
		}
	} else {
		/*
		* controllo dimensionale per colli da fare in automatico 
		* se non ci sta nel piu' grande torno subito errore
		*/
		if( Imballi.Imballo[Imballi.nImballiAutomatici-1].nDim[0]<pRiga->nDim[0] || 
				Imballi.Imballo[Imballi.nImballiAutomatici-1].nDim[1]<pRiga->nDim[1] || 
				Imballi.Imballo[Imballi.nImballiAutomatici-1].nDim[2]<pRiga->nDim[2]){

			/*
			* l'articolo non ci sta nell'imballo piu' grande.
			* Se non ci sono imballi per contenere l'articolo (troppo grande)
			* ritorno : VOL_DIM_PROD_ERRATE;
			* (errore non reversibile : blocco della analisi volumetrica)
			*/
			return VOL_DIM_PROD_ERRATE;
		}
	}
	/*
	* se l'articolo ci sta' : controllo il volume del collo
	*/



	/*
	* se qtkar > 1
	* provo a farci stare una copia alla volta (dimensioni singolo prodotto / peso)
	* se il collo non e' il piu' grande cambio il tipo imballo
	*/
	nRigaCopieCollo=0;
	if(pRiga->nPRQTKAR > 1 && pRiga->nCopie >=pRiga->nPRQTKAR){
		
		bFine=FALSE;
		while(!bFine && pCollo->nIndiceImballo<Imballi.nImballiAutomatici){
			/*
			* se ho finito le copie mi fermo (ci stanno tutte)
			*/
			if(nRigaCopieCollo==pRiga->nCopie){
				bFine=TRUE;
				continue;
			}
			/* 
			* controllo peso 
			*/
			if(pCollo->nPeso+pRiga->nPesoCopia*(nRigaCopieCollo+max(1,pRiga->nPRQTKAR)) > Imballi.Imballo[pCollo->nIndiceImballo].nPesoMax){
				/* 
				* mi fermo perche' il peso e' gia troppo alto
				*/
				bFine=TRUE;
				continue;
			}
			/* rm 12-03-2013 : gestione altezza prodotto */
			if(pRiga->nProdottoInPiedi){
	#ifdef TRACE
					trace_out_vstr(1,"PP-P:[%s]: %3d copie %4dx%4dx%4d = %6d - V.C: %8d V.U: %8d",
						pRiga->szCodiceProdotto,
						pRiga->nCopieCaricate,
						pRiga->nLength,
						pRiga->nWidth,
						pRiga->nHeight,
						(pRiga->nLength* pRiga->nWidth* pRiga->nHeight * pRiga->nCopieCaricate)/1000,
						pCollo->nVolume,Imballi.Imballo[pCollo->nIndiceImballo].nVolumeUtile);
	#endif
				/* 
				* controllo dimensionale (ALTEZZA) + volume
				*/
				if( Imballi.Imballo[pCollo->nIndiceImballo].nHeight<pRiga->nHeight ||
						pCollo->nVolume+pRiga->nVolumeCopia*(nRigaCopieCollo+max(1,pRiga->nPRQTKAR)) > Imballi.Imballo[pCollo->nIndiceImballo].nVolumeUtile){
	#ifdef TRACE
					trace_out_vstr(0,"... NON CI STA");
	#endif

					if(pCollo->nIndiceImballo+1==Imballi.nImballiAutomatici){
						bFine=TRUE;
						continue;
					} else {
						/*
						* provo con l'imballo piu' grande
						*/
						pCollo->nIndiceImballo++;
						continue;
					}
				}
			} else {
				/* 
				* controllo dimensionale  + volume
				*/
				if( Imballi.Imballo[pCollo->nIndiceImballo].nDim[0]<pRiga->nDim[0] || 
						Imballi.Imballo[pCollo->nIndiceImballo].nDim[1]<pRiga->nDim[1] ||
						Imballi.Imballo[pCollo->nIndiceImballo].nDim[2]<pRiga->nDim[2] ||
						pCollo->nVolume+pRiga->nVolumeCopia*(nRigaCopieCollo+max(1,pRiga->nPRQTKAR)) > Imballi.Imballo[pCollo->nIndiceImballo].nVolumeUtile){

					if(pCollo->nIndiceImballo+1==Imballi.nImballiAutomatici){
						bFine=TRUE;
						continue;
					} else {
						/*
						* provo con l'imballo piu' grande
						*/
						pCollo->nIndiceImballo++;
						continue;
					}
				}
			}
			/* gestione non spacchettamento */
			nRigaCopieCollo+=max(1,pRiga->nPRQTKAR);
		}
		/* fine controllo qtkar > 1 */
		/* se il collo e' vuoto e non sono riuscito a mettere la riga vuol dire che la riga non ci sta...la spezzo */
		/* se invece il collo non e' vuoto provo a chiudere il collo e usare un collo vuoto */

		if(nRigaCopieCollo==0){
			if(pCollo->nCopie){
				/*
				* non ci sta con qtkar .. provo a chiudere il collo ...
				*/
		#ifdef TRACE
				trace_out_vstr(0,"... NON CI STA CON QTKAR e collo non vuoto ... provo a chiudere il collo ");
		#endif
				return VOL_CHIUDI_COLLO;
			}
		}
	}

	if(nRigaCopieCollo==0){
		/* 
		* se ho fatto primo giro con qtkar ... e non e' andata bena provo con unita' singola ..
		* se non ho fatto il primo giro ... qtkar non presenta allora vado con la unita' singola comunque
		*/
		/*
		* provo a farci stare una copia alla volta (dimensioni singolo prodotto / peso)
		* se il collo non e' il piu' grande cambio il tipo imballo
		*/
		bFine=FALSE;
		while(!bFine && pCollo->nIndiceImballo<Imballi.nImballiAutomatici){
			/*
			* se ho finito le copie mi fermo (ci stanno tutte)
			*/
			if(nRigaCopieCollo==pRiga->nCopie){
				bFine=TRUE;
				continue;
			}
			/* 
			* controllo peso 
			*/
			if(pCollo->nPeso+pRiga->nPesoCopia*(nRigaCopieCollo+1) > Imballi.Imballo[pCollo->nIndiceImballo].nPesoMax){
				/* 
				* mi fermo perche' il peso e' gia troppo alto
				*/
				bFine=TRUE;
				continue;
			}
			/* rm 12-03-2013 : gestione altezza prodotto */
			if(pRiga->nProdottoInPiedi){
	#ifdef TRACE
					trace_out_vstr(1,"PP-P:[%s]: %3d copie %4dx%4dx%4d = %6d - V.C: %8d V.U: %8d",
						pRiga->szCodiceProdotto,
						pRiga->nCopieCaricate,
						pRiga->nLength,
						pRiga->nWidth,
						pRiga->nHeight,
						(pRiga->nLength* pRiga->nWidth* pRiga->nHeight * pRiga->nCopieCaricate)/1000,
						pCollo->nVolume,Imballi.Imballo[pCollo->nIndiceImballo].nVolumeUtile);
	#endif
				/* 
				* controllo dimensionale (ALTEZZA) + volume
				*/
				if( Imballi.Imballo[pCollo->nIndiceImballo].nHeight<pRiga->nHeight ||
						pCollo->nVolume+pRiga->nVolumeCopia*(nRigaCopieCollo+1) > Imballi.Imballo[pCollo->nIndiceImballo].nVolumeUtile){
	#ifdef TRACE
					trace_out_vstr(0,"... NON CI STA");
	#endif

					if(pCollo->nIndiceImballo+1==Imballi.nImballiAutomatici){
						bFine=TRUE;
						continue;
					} else {
						/*
						* provo con l'imballo piu' grande
						*/
						pCollo->nIndiceImballo++;
						continue;
					}
				}
			} else {
				/* 
				* controllo dimensionale  + volume
				*/
				if( Imballi.Imballo[pCollo->nIndiceImballo].nDim[0]<pRiga->nDim[0] || 
						Imballi.Imballo[pCollo->nIndiceImballo].nDim[1]<pRiga->nDim[1] ||
						Imballi.Imballo[pCollo->nIndiceImballo].nDim[2]<pRiga->nDim[2] ||
						pCollo->nVolume+pRiga->nVolumeCopia*(nRigaCopieCollo+1) > Imballi.Imballo[pCollo->nIndiceImballo].nVolumeUtile){

					if(pCollo->nIndiceImballo+1==Imballi.nImballiAutomatici){
						bFine=TRUE;
						continue;
					} else {
						/*
						* provo con l'imballo piu' grande
						*/
						pCollo->nIndiceImballo++;
						continue;
					}
				}
			}
			nRigaCopieCollo+=1;
		}
	}



	if(nRigaCopieCollo==pRiga->nCopie){
		/*
		* ci sta la riga completa, carico il collo e passo alla prossima
		*/
		pRiga->nCopieCaricate = pRiga->nCopie;
		pCollo->nVolume += pRiga->nVolumeTotale;
		pCollo->nPeso   += pRiga->nPesoTotale;
		pCollo->nCopie  += pRiga->nCopie;
		pCollo->nRighe++;

#ifdef TRACE_ELIMINATO
		trace_out_vstr(1,"P-P :[%s]: %3d copie %4dx%4dx%4d = %6d - V.C: %8d V.U: %8d",
			pRiga->szCodiceProdotto,
			pRiga->nCopieCaricate,
			pRiga->nLength,
			pRiga->nWidth,
			pRiga->nHeight,
			(pRiga->nLength* pRiga->nWidth* pRiga->nHeight * pRiga->nCopieCaricate)/1000,
			pCollo->nVolume,Imballi.Imballo[pCollo->nIndiceImballo].nVolumeUtile);
#endif
		return VOL_PRELIEVO_OK;
	} else if(nRigaCopieCollo){
		/*
		* a questo punto chiudo il collo e ne apro un altro
		*/
		pRiga->nCopieCaricate=nRigaCopieCollo;
		pCollo->nVolume += (nRigaCopieCollo * pRiga->nVolumeCopia);
		pCollo->nPeso   += nRigaCopieCollo * pRiga->nPesoCopia;
		pCollo->nCopie  += nRigaCopieCollo;
		pCollo->nRighe++;
#ifdef TRACE_ELIMINATO
	trace_out_vstr(1,"S-P :[%s]: %3d copie %4dx%4dx%4d = %6d - V.C: %8d V.U: %8d",
		pRiga->szCodiceProdotto,
		pRiga->nCopieCaricate,
		pRiga->nLength,
		pRiga->nWidth,
		pRiga->nHeight,
		(pRiga->nLength* pRiga->nWidth* pRiga->nHeight * pRiga->nCopieCaricate)/1000,
		pCollo->nVolume,Imballi.Imballo[pCollo->nIndiceImballo].nVolumeUtile);
#endif
		return VOL_RIGA_SPEZZATA;
	} else {
		/*
		* non ci sta neanche una copia : chiudo il collo e apro il successivo
		*/
		return VOL_CHIUDI_COLLO;
	}
}

void UpdateTables(void)
{
	PGresult *PGRes;

	PGRes=PGExecSQL(Cfg.nDebugVersion,"vacuum col_prod;");
	PQclear(PGRes);
	PGRes=PGExecSQL(Cfg.nDebugVersion,"vacuum ord_prod;");
	PQclear(PGRes);
	PGRes=PGExecSQL(Cfg.nDebugVersion,"vacuum rig_prod;");
	PQclear(PGRes);
}
