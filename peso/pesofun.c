/*
* pesofun.c
* funzioni
* Gestione Peso/Fine Linea
* Easy Picking 3.0
* Copyright CNI srl 1999-2000
* Autore : Roberto Mantovani
*/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <signal.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <libpq-fe.h>
#include <shared.h>
#include <time.h>

#include "interface.h"
#include "support.h"

#ifdef TRACE
	#include <trace.h>
#endif
#include <proc_list.h>
#include <ca_file.h>
#include <pmx_msq.h>
#include <dbfun.h>

#include "sockets.h"
#include "plcsock.h"

#include "picking.h"
#include "pesostruct.h"
#include "pesoext.h"
#include "pesofun.h"

#include "callbacks.h"

extern int errno;


/*
* GTK-Error handling functions
*/
void gtk_error_handler(gchar *str)
{
	gtk_text_printf(NULL,txt_msgs,str);
}

void gtk_warning_handler(gchar *str)
{
	gtk_text_printf(NULL,txt_msgs,str);
}

void gtk_message_handler(gchar *str)
{
	gtk_text_printf(NULL,txt_msgs,str);
}


/*
* ReadConfiguration()
* lettura della configurazione dal file cni.cfg
*/
void ReadConfiguration(void)
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

	xncGetFileString(szParagraph,"Language",        "ita",          Cfg.szLanguage,     80,Cfg.szCniCfg,NULL);
	xncGetFileString(szParagraph,"PathData",        "../data",      Cfg.szPathData,     80,Cfg.szCniCfg,NULL);
	xncGetFileString(szParagraph,"PathExport",      "../export",    Cfg.szPathExport,   80,Cfg.szCniCfg,NULL);
	xncGetFileString(szParagraph,"PathExe",         "./",           Cfg.szPathExe,      80,Cfg.szCniCfg,NULL);
	xncGetFileString(szParagraph,"PrinterName",     "lp",           Cfg.szPrinterName,  80,Cfg.szCniCfg,NULL);

	xncGetFileString(szParagraph,"ImpOrdiniFile",     "Ordini.txt",   Cfg.szImpOrdiniFile,     80,Cfg.szCniCfg,NULL);
	xncGetFileString(szParagraph,"ImpRigheFile",      "Ordini.txt",   Cfg.szImpRigheFile,      80,Cfg.szCniCfg,NULL);
	xncGetFileString(szParagraph,"ImpArticoliFile",   "Articoli.txt", Cfg.szImpArticoliFile,   80,Cfg.szCniCfg,NULL);
	xncGetFileString(szParagraph,"ImpUbicazioniFile", "Mappa.txt",    Cfg.szImpUbicazioniFile, 80,Cfg.szCniCfg,NULL);

	xncGetFileString(szParagraph,"ExpOrdiniFile",   "OrdCni.txt",     Cfg.szExpOrdiniFile,  80,Cfg.szCniCfg,NULL);
	xncGetFileString(szParagraph,"ExpColliFile",    "Colli.txt",      Cfg.szExpColliFile,   80,Cfg.szCniCfg,NULL);

	xncGetFileString(szParagraph,"SocksHost",          "naomi",       Cfg.szSocksHost,   80,Cfg.szCniCfg,NULL);

	Cfg.nShmKey      = xncGetFileInt(szParagraph,      "ShmKey",          256, Cfg.szCniCfg, NULL);
	Cfg.nServicePort = xncGetFileInt(szParagraph,      "ServicePort",    8050, Cfg.szCniCfg, NULL);
	
	Cfg.ulSuperPwd=(unsigned long)xncGetFileInt(szParagraph,"SupervisorPwd",  (int)CryptString("roberto"),Cfg.szCniCfg,NULL);
	Cfg.ulChiefPwd=(unsigned long)xncGetFileInt(szParagraph,"ChiefPwd",  (int)CryptString("roberto"),Cfg.szCniCfg,NULL);

	/*
	* lettura del file di configurazione del DataBase (PostgreSQL)
	*/
	strcpy(szParagraph,"DataBase Settings");

	xncGetFileString(szParagraph,"PGHost",    "localhost",Cfg.szPGHost,     80,Cfg.szCniCfg,NULL); 
	xncGetFileString(szParagraph,"PGPort",    "5432",     Cfg.szPGPort,     80,Cfg.szCniCfg,NULL); 
	xncGetFileString(szParagraph,"PGDataBase","picking",  Cfg.szPGDataBase, 80,Cfg.szCniCfg,NULL); 


	/*
	* Lettura dei parametri relativi ai processi 
	*/
	ReadProcInfo(Cfg.szCniCfg);

	/* 
	* configurazioni specifiche di peso 
	*/
	Cfg.nPesoDelay       = xncGetFileInt(ProcList[PROC_PESO].szProcName,"5elay",           100, Cfg.szCniCfg,NULL);
	Cfg.nBilanciaTimeout = xncGetFileInt(ProcList[PROC_PESO].szProcName,"BilanciaTimeout",2000, Cfg.szCniCfg,NULL);
	Cfg.nDebugVersion    = xncGetFileInt(ProcList[PROC_PESO].szProcName,"DebugVersion",      0, Cfg.szCniCfg,NULL);
	Cfg.nPLC             = xncGetFileInt(ProcList[PROC_PESO].szProcName,"PLC",               0, Cfg.szCniCfg,NULL);
	Cfg.nPLCService      = xncGetFileInt(ProcList[PROC_PESO].szProcName,"PLCService",     8039, Cfg.szCniCfg, NULL);
	xncGetFileString(ProcList[PROC_PESO].szProcName,"PLCHost","picking",Cfg.szPLCHost,80,Cfg.szCniCfg,NULL);
	xncGetFileString(ProcList[PROC_PESO].szProcName,"PrinterName","lp",Cfg.szPrinterName,80,Cfg.szCniCfg,NULL);
	xncGetFileString(ProcList[PROC_PESO].szProcName,"LabelPrinterName","lp",Cfg.szLabelPrinterName,80,Cfg.szCniCfg,NULL);

	Cfg.GestionePesatura = xncGetFileInt(ProcList[PROC_PESO].szProcName,"GestionePesatura",PESATURA_ORDINE,Cfg.szCniCfg,NULL);
	Cfg.nTolleranzaAss   = xncGetFileInt(ProcList[PROC_PESO].szProcName,"TolleranzaAssoluta",          150,Cfg.szCniCfg,NULL);
	Cfg.nTolleranzaPerc  = xncGetFileInt(ProcList[PROC_PESO].szProcName,"TolleranzaPercentuale",         2,Cfg.szCniCfg,NULL);
	Cfg.Tolleranza       = xncGetFileInt(ProcList[PROC_PESO].szProcName,"Tolleranza",  TOLLERANZA_ASSOLUTA,Cfg.szCniCfg,NULL);
	Cfg.nSogliaMax       = xncGetFileInt(ProcList[PROC_PESO].szProcName,"SogliaMassima",               200,Cfg.szCniCfg,NULL);
	Cfg.nSogliaMin       = xncGetFileInt(ProcList[PROC_PESO].szProcName,"SogliaMinima",                200,Cfg.szCniCfg,NULL);

	Cfg.nColloKeyField   = xncGetFileInt("Fine Linea", "ColloKeyField",    2, Cfg.szCniCfg, NULL);
	Cfg.nColloNumField   = xncGetFileInt("Fine Linea", "ColloNumField",    4, Cfg.szCniCfg, NULL);
	
	Cfg.GestionePesatura = xncGetFileInt(ProcList[PROC_PESO].szProcName,"GestionePesatura",PESATURA_ORDINE,Cfg.szCniCfg,NULL);

	/* esecuzuine automatica del comando cni2sap (rif. Warnier Gatto - SAP) */
	Cfg.nExecuteCni2Sap  = xncGetFileInt(ProcList[PROC_PESO].szProcName,"EseguiCni2Sap",1,Cfg.szCniCfg,NULL);

	/* rm 20-01-2004 : Abilitazione alla trasmissione dati a HOST */
	Cfg.nEnableTransmit  = xncGetFileInt(ProcList[PROC_PESO].szProcName,"EnableTransmit",1,Cfg.szCniCfg,NULL);

	/*
	* Lettura Lista variabili
	*/
	ReadVarList(Cfg.szCniCfg,PLCVars);


#ifdef TRACE
	trace_out_vstr_date(1,"Read Configuration");
	trace_out_vstr(1,"-------------------------------------------");
	trace_out_vstr(1,"Language           : [%s]", Cfg.szLanguage);
	trace_out_vstr(1,"PathData           : [%s]", Cfg.szPathData);
	trace_out_vstr(1,"PathExport         : [%s]", Cfg.szPathExport);
	trace_out_vstr(1,"PathExe            : [%s]", Cfg.szPathExe);
	trace_out_vstr(1,"PLCHost            : [%s]", Cfg.szPLCHost);
	trace_out_vstr(1,"PrinterName        : [%s]", Cfg.szPrinterName);
	trace_out_vstr(1,"PLC                : [%d]", Cfg.nPLC);
	trace_out_vstr(1,"PLCService         : [%d]", Cfg.nPLCService);
	trace_out_vstr(1,"DebugVersion       : [%d]", Cfg.nDebugVersion);
	trace_out_vstr(1,"BilanciaTimeout    : [%d]", Cfg.nBilanciaTimeout);
	trace_out_vstr(1,"SAP                : [%d]", Cfg.nSAP);
	trace_out_vstr(1,"Esecuzione cni2sap : [%s]", (Cfg.nExecuteCni2Sap==1)?"Attiva":"Disattivata");
	trace_out_vstr(1,"EnableTransmit     : [%s]", (Cfg.nEnableTransmit==1)?"Attiva":"Disattivata");
	trace_out_vstr(1,"-------------------------------------------");

#endif
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


void ExitApp()
{
	PQfinish(DBConn);

#ifdef TRACE
	trace_out_vstr_date(1,"Stopped");
#endif
	TerminateProcess(PROC_BARCODE);
	TerminateProcess(PROC_BARCODE_PZ);
	TerminateProcess(PROC_BILANCIA);
	TerminateProcess(PROC_BILSIM);
	TerminateProcess(PROC_PRINTLABEL);

	/* cancello la coda messaggi principale */
	DeleteMsgQ(ProcList[PROC_PESO].nQNumber);

	/* chiudo la GUI */
	gtk_widget_destroy(main_window);
	gtk_main_quit();
}




char *GetOrdiniKey(char *szBuffer)
{
	/*
	* DA RIFARE
	*/
	return szBuffer;
}

char *GetUbicazione(char *pszUbicazione,char *pszBuffer)
{
	strcpy(pszBuffer,SubStr(pszUbicazione,0,3)); strcat(pszBuffer,".");
	strcat(pszBuffer,SubStr(pszUbicazione,3,3)); strcat(pszBuffer,".");
	strcat(pszBuffer,SubStr(pszUbicazione,6,3)); strcat(pszBuffer,".");
	strcat(pszBuffer,SubStr(pszUbicazione,9,3));
	return pszBuffer;
}

char *GetTitoloProdotto(char *pszCodiceProdotto,char *pszBuffer)
{
	/*
	* DA RIFARE
	*/
	return pszBuffer;
}

void EditOrdine(char *szOrdineKey)
/*
* st 14-12-2000 
* visualizza i dati dell'ordine di produzione anziche' l'ordine ricevuto
*/
{
	GtkWidget *sw_edit_ordine=get_widget(dlg_edit_ordine,"sw_edit_ordine");
	GtkWidget *lst;
	PGresult *PGRes = NULL;
	BOOL bColloElaborato;
	char szParagraph[128];
	char szSelectCmd[2048];
	char szStato[128];
	char szBuffer[128];
	int nTuples;
	int nColli=0;
	int nRighe=0;
	int nCopie=0;

	/*
	* Cerco l'ordine di produzione specificato nella tabella ordini ricevuti
	*/
	if(Cfg.nSAP){
		PGRes=PGExecSQL(Cfg.nDebugVersion,"select ord_prod.opstato,ord_prod.opnmcll,ord_prod.opnmrgh,ord_prod.opnmcpe,ord_prod.ordprog,ric_ord.rodscli,ord_prod.opdteva,ord_prod.optmeva,ord_prod.oppspre,ord_prod.oppsrea from ric_ord,ord_prod where ord_prod.ordkey='%s' and ric_ord.ordprog=(substr(ord_prod.ordkey,1,10));",szOrdineKey);
	} else {
		PGRes=PGExecSQL(Cfg.nDebugVersion,"select ord_prod.opstato,ord_prod.opnmcll,ord_prod.opnmrgh,ord_prod.opnmcpe,ord_prod.ordprog,ric_ord.rodscli,ord_prod.opdteva,ord_prod.optmeva,ord_prod.oppspre,ord_prod.oppsrea from ric_ord,ord_prod where ord_prod.ordkey='%s' and ric_ord.ordprog=(substr(ord_prod.ordkey,1,6));",szOrdineKey);
	}

	nTuples=PQntuples(PGRes);

	szStato[0]='\0';

	/*
	* Trovato l'ordine che cercavo nella tabella ordini ricevuti
	*/
	if(nTuples){
		PGresult *PGResElab = NULL;

		/*
		* assegno i valori dell'ordine alle label
		*/
		strcpy(szStato,PQgetvalue(PGRes,0,0));
		nColli=atoi(PQgetvalue(PGRes,0,1));
		nRighe=atoi(PQgetvalue(PGRes,0,2));
		nCopie=atoi(PQgetvalue(PGRes,0,3));
		gtk_label_printf(get_widget(dlg_edit_ordine,"lb_NMCLL"),"%4d",nColli);
		gtk_label_printf(get_widget(dlg_edit_ordine,"lb_NMRGH"),"%4d",nRighe);
		gtk_label_printf(get_widget(dlg_edit_ordine,"lb_NMCPE"),"%4d",nCopie);
		gtk_label_printf(get_widget(dlg_edit_ordine,"lb_ORDPROG"),PQgetvalue(PGRes,0,4));
		gtk_label_printf(get_widget(dlg_edit_ordine,"lb_DSCLI1"),PQgetvalue(PGRes,0,5));
		gtk_label_printf(get_widget(dlg_edit_ordine,"lb_STATO"), GetStatoOrdine(szStato[0],szBuffer));
		gtk_label_printf(get_widget(dlg_edit_ordine,"lb_DATAEVA"), GetDateFromYYYYMMDD(PQgetvalue(PGRes,0,6),szDateBuffer));
		gtk_label_printf(get_widget(dlg_edit_ordine,"lb_ORAEVA"), GetTimeFromHHMMSS(PQgetvalue(PGRes,0,7),szDateBuffer));
		gtk_label_printf(get_widget(dlg_edit_ordine,"lb_PSPRE"), "%7.2fKg",(float)((float)atoi(PQgetvalue(PGRes,0,8))/(float)1000));
		gtk_label_printf(get_widget(dlg_edit_ordine,"lb_PSREA"), "%7.2fKg",(float)((float)atoi(PQgetvalue(PGRes,0,9))/(float)1000));

		/*
		* controllo se l'ordine e' gia stato elaborato e quindi formati i colli
		*/
		PGResElab=PGExecSQL(FALSE,"select * from ord_prod where ordkey='%s';",szOrdineKey);
		if(PQntuples(PGResElab)==0){
			/*
			* Ordine ricevuto ma non ancora elaborato -> visualizzo la lista delle righe
			*/
			bColloElaborato=FALSE;
			strcpy(szParagraph,"Edit Righe Ordine");
			xncGetFileString(szParagraph,"SELECT_CMD", "", szSelectCmd, sizeof(szSelectCmd),Cfg.szCniCfg,NULL); 
		} else {
			/*
			* Ordine in produzione -> visualizzo la lista dei colli
			*/
			bColloElaborato=TRUE;
			//strcpy(szParagraph,"Edit Colli Ordine");
			sprintf(szSelectCmd,"select col_prod.ordkey as \"ORDINE\", col_prod.cpstato as \"S\", col_prod.cpnmcol as \"NUM.COLLO\", col_prod.cptpfor as \"FORMATO\", col_prod.cpswlin as \"L\", col_prod.cpprgcl as \"SEGNAC.\", col_prod.cpnmrgh as \"RIGHE\", col_prod.cpnmcpe as \"COPIE\", col_prod.cppspre::float4/1000 as \"PESO D(Kg)\", col_prod.cppsrea::float4/1000 as \"PESO R(Kg)\",col_prod.cpdteva as \"D.EVA.\", col_prod.cptmeva as \"O.EVA\", col_prod.cpswfps as \"FP\"  from col_prod,ord_prod where col_prod.ordkey='%s' order by col_prod.ordkey,col_prod.cpnmcol;",szOrdineKey);

		}
		PQclear(PGResElab);
	}

	PQclear(PGRes);

	/*
	* Creazione clist widget su select
	*/
	CreateListFromSelect(dlg_edit_ordine,sw_edit_ordine,&lst,"lst_edit_ordine",GTK_SELECTION_SINGLE,szSelectCmd,szOrdineKey,0);

	strcpy(szOrdineInEdit,szOrdineKey);
}

BOOL RefreshDlgLetturaBarcodeId(void)
{
	GtkWidget *sw_list_barcode_id=get_widget(dlg_lettura_barcode_id,"sw_list_barcode_id");
	GtkWidget *lst;
	PGresult *PGRes = NULL;
	BOOL bColloElaborato;
	char szListName[128];
	char szParagraph[128];
	char szSelectCmd[2048];
	char szStatoLetture[8];
	char szBuffer[128];
	int nTuples;
	int nColli=0;
	int nRighe=0;
	int nCopie=0;
	char *pszOrdkey;
	char *pszCollo;
	char *pszCodProd;
	char *pszTitolo;
	int nCollo;
	int nLettureEffettuate=0;
	int nLetturePreviste  =0;
	int nDifferenza       =0;



	gtk_label_get(GTK_LABEL(get_widget(dlg_lettura_barcode_id,"lb_ORDKEY")),&pszOrdkey);
	gtk_label_get(GTK_LABEL(get_widget(dlg_lettura_barcode_id,"lb_NMCOL")),&pszCollo);
	nCollo=atoi(pszCollo);
	gtk_label_get(GTK_LABEL(get_widget(dlg_lettura_barcode_id,"lb_CDPRO")),&pszCodProd);
	gtk_label_get(GTK_LABEL(get_widget(dlg_lettura_barcode_id,"lb_DSTIT")),&pszTitolo);

	/*
	* Cerco quantita' in gioco 
	*/
	PGRes=PGExecSQL(Cfg.nDebugVersion,"select rpflbri,rpqtbri,rpqtspe,rpqtspe-rpqtbri from rig_prod where ordkey='%s' and rpnmcol=%d and rpcdpro='%s';",pszOrdkey,nCollo,pszCodProd);

	nTuples=PQntuples(PGRes);

	szStatoLetture[0]='\0';
	if(nTuples){
		PGresult *PGResElab = NULL;

		/*
		* assegno i valori dell'ordine alle label
		*/
		strcpy(szStatoLetture,PQgetvalue(PGRes,0,0));
		nLettureEffettuate=atoi(PQgetvalue(PGRes,0,1));
		nLetturePreviste  =atoi(PQgetvalue(PGRes,0,2));
		nDifferenza       =atoi(PQgetvalue(PGRes,0,3));

		gtk_label_printf(get_widget(dlg_lettura_barcode_id,"lb_barcode_id_previsti"),"%4d",nLetturePreviste);
		gtk_label_printf(get_widget(dlg_lettura_barcode_id,"lb_barcode_id_letti"),   "%4d",nLettureEffettuate);
		gtk_label_printf(get_widget(dlg_lettura_barcode_id,"lb_differenza_letture"), "%4d",nDifferenza);
		gtk_label_printf(get_widget(dlg_lettura_barcode_id,"lb_FLBRI"),szStatoLetture);
		sprintf(szSelectCmd,"select lbprbri as \"PROGR\",lbcdbri as \"BARCODE ID\",to_char(lbtmlet,'YYYY-MM-DD H24:MI') as \"DATA\",lbcodop as \"CD OP\",lbnomop as \"NOME OP\",lbcogop as \"COGNOME OP\" from letture_barcode_id_prodotto where ordkey='%s' and lbnmcol=%d and lbcdpro='%s' order by lbprbri desc;",pszOrdkey,nCollo,pszCodProd);


		strcpy(szListName,"lst_barcode_id");
		if((lst=get_widget(dlg_lettura_barcode_id,szListName))){
			gtk_widget_destroy(lst);
		}

		CreateListFromSelect(dlg_lettura_barcode_id,sw_list_barcode_id,&lst,szListName,GTK_SELECTION_SINGLE,szSelectCmd,"",0);
	}

	PQclear(PGRes);
//	strcpy(szOrdineInEdit,szOrdineKey);
	return TRUE;
}



BOOL RefreshDlgEditCollo(void)
{
	GtkWidget *sw_edit_collo=get_widget(dlg_edit_collo,"sw_edit_collo");
	GtkWidget *lst;
	PGresult *PGRes = NULL;
	BOOL bColloElaborato;
	char szOrdkey[128];
	char szListName[128];
	char szParagraph[128];
	char szSelectCmd[2048];
	char szStatoLetture[8];
	char szBuffer[128];
	int nTuples;
	int nColli=0;
	int nRighe=0;
	int nCopie=0;
	char *pszOrdkey;
	char *pszCollo;
	char *pszCodProd;
	char *pszTitolo;
	int nCollo;
	int nLettureEffettuate=0;
	int nLetturePreviste  =0;
	int nDifferenza       =0;

	gtk_label_get(GTK_LABEL(get_widget(dlg_edit_collo,"lb_ORDPROG")),&pszOrdkey);
	strcpy(szOrdkey,pszOrdkey);
	gtk_label_get(GTK_LABEL(get_widget(dlg_edit_collo,"lb_NMCOL")),&pszCollo);
	nCollo=atoi(pszCollo);

	/*
	* Lista Righe
	*/
	strcpy(szParagraph,"Edit Righe Collo");
	strcpy(szListName,"lst_righe_collo");

	/*
	* lettura della select corrispondente dal file di configurazione
	*/
	xncGetFileString(szParagraph,"SELECT_CMD",  "", szSelectCmd, sizeof(szSelectCmd),Cfg.szCniCfg,NULL); 

	if(lst=get_widget(dlg_edit_collo,szListName)){
		gtk_widget_destroy(lst);
	}
	/*
	* Creazione clist widget su select
	*/
	CreateListFromSelect(dlg_edit_collo,sw_edit_collo,&lst,szListName,GTK_SELECTION_SINGLE,szSelectCmd,szOrdkey,nCollo);

	lst_righe_collo=lst;

}





BOOL do_lettura_barcode_id (void)
{

	PGresult *PGRes;
	BOOL bOK=TRUE;
	char *pszOrdkey;
	char *pszCollo;
	char *pszCodProd;
	char *pszStatoLetture;
	char *pszBuffer;
	char *pszTitolo;
	char szBarcodeId[128];
	char szTmpBarcodeId[128];
	char szMsg[512];
	int nCollo;
	int nLettureEffettuate=0;
	int nLetturePreviste  =0;
	int nDifferenza       =0;
	char cStatoLetture;

	gtk_label_get(GTK_LABEL(get_widget(dlg_lettura_barcode_id,"lb_DSTIT")),&pszTitolo);
	gtk_label_get(GTK_LABEL(get_widget(dlg_lettura_barcode_id,"lb_FLBRI")),&pszStatoLetture);
	cStatoLetture=pszStatoLetture[0];
	gtk_label_get(GTK_LABEL(get_widget(dlg_lettura_barcode_id,"lb_ORDKEY")),&pszOrdkey);
	gtk_label_get(GTK_LABEL(get_widget(dlg_lettura_barcode_id,"lb_NMCOL")),&pszCollo);
	nCollo=atoi(pszCollo);
	gtk_label_get(GTK_LABEL(get_widget(dlg_lettura_barcode_id,"lb_CDPRO")),&pszCodProd);

	strcpy(szTmpBarcodeId, gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_lettura_barcode_id,"entry_barcode_id"))));
	if(strlen(szTmpBarcodeId) && szTmpBarcodeId[0] == '%'){
		strcpy(szBarcodeId,szTmpBarcodeId+1);
	} else {
		strcpy(szBarcodeId,szTmpBarcodeId);
	}

	gtk_label_get(GTK_LABEL(get_widget(dlg_lettura_barcode_id,"lb_barcode_id_previsti")),&pszBuffer);
	nLetturePreviste=atoi(pszBuffer);
	gtk_label_get(GTK_LABEL(get_widget(dlg_lettura_barcode_id,"lb_barcode_id_letti")),&pszBuffer);
	nLettureEffettuate=atoi(pszBuffer);
	gtk_label_get(GTK_LABEL(get_widget(dlg_lettura_barcode_id,"lb_differenza_letture")),&pszBuffer);
	nDifferenza=atoi(pszBuffer);

	gtk_entry_set_text (GTK_ENTRY(get_widget(dlg_lettura_barcode_id,"entry_barcode_id")), "");
	gtk_widget_grab_focus (get_widget(dlg_lettura_barcode_id,"entry_barcode_id"));

	/*
	* - se si tratta dell'ultima lettura di quelle previste devo impedirne altre
	* - se rileggo un barcode gia' letto 
	*/
#ifdef TRACE
	trace_out_vstr_date(1, "INIZIO do_lettura_barcode_id");
	trace_out_vstr_date(1, "Barcode Id: [%s]",szBarcodeId);
#endif
	sprintf(szMsg," ");
	if (nDifferenza<=0){
#ifdef TRACE
		trace_out_vstr_date(1, "Barcode Id: [%s] barcode non gestito perche' qt superata",szBarcodeId);
#endif
		sprintf(szMsg,"Barcode Id: [%s]\nbarcode non gestito perche' qt superata",szBarcodeId);
		bOK=FALSE;
	} else if ((strlen(szBarcodeId))!=BARCODE_ID_LENGTH){
#ifdef TRACE
		trace_out_vstr_date(1, "Barcode Id: [%s] barcode non gestito perche' lunghezza errata",szBarcodeId);
#endif
		sprintf(szMsg,"Barcode Id: [%s]\nbarcode non gestito perche' lunghezza errata,szBarcodeId",szBarcodeId);
		bOK=FALSE;
	} else if (cStatoLetture==CONTROLLO_BARCODE_OK || cStatoLetture==CONTROLLO_BARCODE_KO || cStatoLetture==CONTROLLO_BARCODE_DISABILITATO) {
#ifdef TRACE
		trace_out_vstr_date(1, "Barcode Id: [%s] barcode non gestito perche' stato riga non conforme alla lettura",szBarcodeId);
#endif
		sprintf(szMsg,"Barcode Id: [%s]\nbarcode non gestito perche' stato riga non conforme alla lettura",szBarcodeId);
		bOK=FALSE;
	}

	if (!bOK) {
		GtkWidget *lb_msg;
		dlg_message=create_dlg_message();
		lb_msg=get_widget(dlg_message,"lb_msg");
		gtk_label_printf(lb_msg,szMsg);
		gtk_window_set_title (GTK_WINDOW (dlg_message), "ERRORE");
		gtk_widget_show(dlg_message);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(dlg_elimina_collo));
		gtk_signal_connect (GTK_OBJECT (get_widget(dlg_message,"pb_ok")), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_pb_ok_clicked), dlg_message);

	} else {
		int nIndex;
		//char szSendBuffer[128];
		char szBuffer[128];
		BOOL bOK=FALSE;

#ifdef TRACE
		trace_out_vstr_date(1, "Barcode Id: [%s] lettura barcode id prodotto isola: %d settore: %d",szBarcodeId,0,0);
#endif

		PGRes=PGExecSQL(Cfg.nDebugVersion,"insert into letture_barcode_id_prodotto (lbcdbri , lbtmlet , lbnisol , lbnsett , lbcdpro , ordkey , lbnmcol , lbnmcpe , lbdstit , lbcodop , lbnomop , lbcogop ) \
																																				values ('%s',     'now',    %d,       %d,       '%s',     '%s',     %d,       %d,       '%s',     '%s',     '%s',     '%s');" ,
			szBarcodeId,
			0,
			0,
			pszCodProd,
			pszOrdkey,
			nCollo,
			1,
			pszTitolo,
			"0",
			"Pesatura",
			"Pesatura"
		);

		if(PQresultStatus(PGRes) == PGRES_COMMAND_OK){
		} else {
			sprintf(szMsg,"Barcode Id: [%s]\nbarcode non gestito per errore di inserimento",szBarcodeId);
			GtkWidget *lb_msg;
			dlg_message=create_dlg_message();
			lb_msg=get_widget(dlg_message,"lb_msg");
			gtk_label_printf(lb_msg,szMsg);
			gtk_window_set_title (GTK_WINDOW (dlg_message), "ERRORE");
			gtk_widget_show(dlg_message);
			gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(dlg_elimina_collo));
			gtk_signal_connect (GTK_OBJECT (get_widget(dlg_message,"pb_ok")), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_pb_ok_clicked), dlg_message);

			bOK=FALSE;
		}

		PQclear(PGRes);
	}


	// Ricalcolo il numero di pezzi letti e aggiorno Settori.nQtBarcodeId
	PGRes=PGExecSQL(Cfg.nDebugVersion,"select count(*) from letture_barcode_id_prodotto where lbcdpro='%s' and ordkey='%s' and lbnmcol=%d"
		,pszCodProd,pszOrdkey,nCollo
	);
	if(PQresultStatus(PGRes) == PGRES_TUPLES_OK){
		nLettureEffettuate=atoi(PQgetvalue(PGRes,0,0));
	} else {
	}
	PQclear(PGRes);

	if (nLettureEffettuate>=nLetturePreviste) {
		cStatoLetture=CONTROLLO_BARCODE_OK;
	}

	// Aggiorno rig_prod
	PGRes=PGExecSQL(Cfg.nDebugVersion,"update rig_prod set rpflbri='%c',rpqtbri=%d where rpcdpro='%s' and ordkey='%s' and rpnmcol=%d"
		,cStatoLetture, nLettureEffettuate ,pszCodProd,pszOrdkey,nCollo
	);
	PQclear(PGRes);

	CheckLettureBarcodeIDSuCollo(pszOrdkey, nCollo);

	RefreshDlgLetturaBarcodeId();


}



BOOL EditCollo(char *szOrdineKey,int nNumCollo)
{
	char szParagraph[128];
	char szSelectCmd[2048];
	char szListName[128];
	char szStato[128];
	char szBuffer[128];
	char *pszCDERR;
	GtkWidget *sw_edit_collo=get_widget(dlg_edit_collo,"sw_edit_collo");
	GtkWidget *lst;
	PGresult *PGRes = NULL;
	static BOOL bFirstTime=TRUE;
	int nTuples;
	BOOL bErrore;
	int nIndex;
	GdkColor *cColor;

	/*
	* Cerco i dati del collo specificato
	*/
	PGRes=PGExecSQL(FALSE,"select cpstato,ordkey,cpnmcol,cptpfor,cpdteva,cptmeva,cppspre,cppsrea from col_prod where ordkey='%s' AND cpnmcol=%d;",szOrdineKey,nNumCollo);

	nTuples=PQntuples(PGRes);

	szStato[0]='\0';
	/*
	* Trovato l'ordine che cercavo
	*/
	if(nTuples>=1){
		strcpy(szStato,PQgetvalue(PGRes,0,0));

		/* Stato Collo     */ gtk_label_printf(get_widget(dlg_edit_collo,"lb_STATO"), GetStatoCollo(szStato[0],szBuffer));
		/* Codice Ordine   */ gtk_label_printf(get_widget(dlg_edit_collo,"lb_ORDPROG"),PQgetvalue(PGRes,0,1));
		/* Numero Collo    */ gtk_label_printf(get_widget(dlg_edit_collo,"lb_NMCOL"),"%4d",atoi(PQgetvalue(PGRes,0,2)));
		/* Formato Collo   */ gtk_label_printf(get_widget(dlg_edit_collo,"lb_TPFOR"),PQgetvalue(PGRes,0,3));
		/* Data Evasione   */ gtk_label_printf(get_widget(dlg_edit_collo,"lb_DATAEVA"), GetDateFromYYYYMMDD(PQgetvalue(PGRes,0,4),szDateBuffer));
		/* Ora Evasione    */ gtk_label_printf(get_widget(dlg_edit_collo,"lb_ORAEVA"), GetTimeFromHHMMSS(PQgetvalue(PGRes,0,5),szDateBuffer));
		/* Peso Calcolato  */ gtk_label_printf(get_widget(dlg_edit_collo,"lb_PSPRE"), "%7.2fKg",(float)((float)atoi(PQgetvalue(PGRes,0,6))/(float)1000));
		/* Peso Reale      */ gtk_label_printf(get_widget(dlg_edit_collo,"lb_PSREA"), "%7.2fKg",(float)((float)atoi(PQgetvalue(PGRes,0,7))/(float)1000));

		/*
		* Lista Righe
		*/
		strcpy(szParagraph,"Edit Righe Collo");
		strcpy(szListName,"lst_righe_collo");
		/*
		* Reset Query
		*/
	} else {
		return FALSE;
	}

	PQclear(PGRes);

	/*
	* lettura della select corrispondente dal file di configurazione
	*/
	xncGetFileString(szParagraph,"SELECT_CMD",  "", szSelectCmd, sizeof(szSelectCmd),Cfg.szCniCfg,NULL); 

	strcpy(szListName,"lst_righe_collo");
	if(!bFirstTime && (lst=get_widget(dlg_edit_collo,szListName))){
		gtk_widget_destroy(lst);
	}
	bFirstTime=FALSE;
	/*
	* Creazione clist widget su select
	*/
	CreateListFromSelect(dlg_edit_collo,sw_edit_collo,&lst,szListName,GTK_SELECTION_SINGLE,szSelectCmd,szOrdineKey,nNumCollo);
	/*
	* callback per selezione riga
	*/
  gtk_signal_connect (GTK_OBJECT (lst), "select_row", GTK_SIGNAL_FUNC (on_lst_righe_collo_select_row), 0);
  gtk_signal_connect (GTK_OBJECT (lst), "unselect_row", GTK_SIGNAL_FUNC (on_lst_righe_collo_unselect_row), 0);

	gtk_widget_set_sensitive(get_widget(dlg_edit_collo,"pb_qta_errata"),FALSE);
	gtk_widget_set_sensitive(get_widget(dlg_edit_collo,"pb_mancante"),FALSE);
	gtk_widget_set_sensitive(get_widget(dlg_edit_collo,"pb_inversione"),FALSE);
	gtk_widget_set_sensitive(get_widget(dlg_edit_collo,"pb_riga_non_richiesta"),FALSE);
	gtk_widget_set_sensitive(get_widget(dlg_edit_collo,"pb_annulla_errore"),FALSE);

	lst_righe_collo=lst;

	for(nIndex=0;nIndex<(GTK_CLIST(lst)->rows);nIndex++){
		bErrore=TRUE;
		gtk_clist_get_text(GTK_CLIST(lst),nIndex,6,&pszCDERR);
		if(!strcmp(pszCDERR,"QTA_ERRATA")){
			cColor=&cBLUE;
		} else if(!strcmp(pszCDERR,"MANCANTE")){
			cColor=&cCYAN;
		} else if(!strcmp(pszCDERR,"INVERSIONE")){
			cColor=&cRED;
		} else if(!strcmp(pszCDERR,"NON_RICHIESTA")){
			cColor=&cGREEN;
		} else {
			bErrore=FALSE;
		}
		if(bErrore){
			gtk_clist_set_background(GTK_CLIST(lst),nIndex,cColor);
			gtk_clist_set_foreground(GTK_CLIST(lst),nIndex,&cWHITE);
		}
	}

	return TRUE;
}

char *GetStatoOrdine(char cStato,char *pszBuffer)
{
	switch(cStato){
		case ORDINE_RICEVUTO:
			strcpy(pszBuffer,"RICEVUTO");
		break;
		case ORDINE_SPEDITO:
			strcpy(pszBuffer,"SPEDITO");
		break;
		case ORDINE_ELABORATO:
			strcpy(pszBuffer,"ELABORATO");
		break;
		case ORDINE_STAMPATA_RAC:
			strcpy(pszBuffer,"STAMPATO");
		break;
		case ORDINE_IN_PRELIEVO:
			strcpy(pszBuffer,"IN PRELIEVO");
		break;
		case ORDINE_PRELEVATO:
			strcpy(pszBuffer,"PRELEVATO");
		break;
		case ORDINE_EVASO:
			strcpy(pszBuffer,"EVASO");
		break;
		case ORDINE_STAMPATA_XAB:
			strcpy(pszBuffer,"STAMPATO XAB");
		break;
		case ORDINE_STAMPATA_DIST:
			strcpy(pszBuffer,"STAMPATO DIST");
		break;
		case ORDINE_SPEDITO_HOST:
			strcpy(pszBuffer,"SPEDITO A HOST");
		break;
		default:
			strcpy(pszBuffer,"NON DEFINITO");
		break;
	}
	return pszBuffer;
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
		case COLLO_ELIMINATO:
			strcpy(pszBuffer,"ELIMINATO");
		break;
		case COLLO_PESATO:
			strcpy(pszBuffer,"PESATO");
		break;
		default:
			strcpy(pszBuffer,"NON DEFINITO");
		break;
	}
	return pszBuffer;
}

/*
* UpdateListFromSelect()
* Aggiorna una riga di una lista GTK da una select SQL
*/
int UpdateListFromSelect(GtkWidget *clist, char *szSqlCmd, char *szKey, int nNumCollo, int nKeyField, int nColloField)
{
	int nTuples;
	int nFields;
	int nIndex;
	int nRowIndex;
	int nFieldIndex;
	char szCmd[4096];
	char *pPtr;
	char *pszTextKey;
	char *pszTextCollo;
	char szBuffer[128];
	char szText[128];
	BOOL bColloPresente=FALSE;
	PGresult *PGRes = NULL;
	
	strcpy(szCmd,szSqlCmd);
	/*
	* Sosituisco i valori parametrici
	* %ORDKEY%   : Chiave
	* %NUMCOLLO% : Numero del collo
	*/
	if((pPtr=strstr(szSqlCmd,"%ORDKEY%"))){
		*pPtr='\0';
		strcpy(szCmd,szSqlCmd);
		strcat(szCmd,szKey);
		strcat(szCmd,pPtr+8);
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
#ifdef TRACE_ELIMINATO
	trace_out_vstr_date(1," UpdateListFromSelect : LANCIATA %s",szSqlCmd);
#endif
	PGRes=PGExecSQL(Cfg.nDebugVersion,szSqlCmd);
#ifdef TRACE_ELIMINATO
	trace_out_vstr_date(1," UpdateListFromSelect : TERMINATA");
#endif

	nTuples=PQntuples(PGRes);
	nFields=PQnfields(PGRes);

#ifdef TRACE
	trace_out_vstr_date(1," UpdateListFromSelect : Iniziato ciclo di refresh dati a video");
#endif
	if(nTuples==1){
		for(nIndex=0;nIndex<(GTK_CLIST(clist)->rows);nIndex++){
			gtk_clist_get_text(GTK_CLIST(clist),nIndex,nKeyField,&pszTextKey);
			gtk_clist_get_text(GTK_CLIST(clist),nIndex,nColloField,&pszTextCollo);
			if(!strcmp(szKey,pszTextKey) && (nNumCollo==atoi(pszTextCollo))){
				/*
				* trovata la riga
				*/
				bColloPresente=TRUE;
				for(nFieldIndex=0;nFieldIndex<nFields;nFieldIndex++){
					int nLength;

					if(PQftype(PGRes,nFieldIndex)==1042){
						/* si tratta di una stringa */
						nLength=PQgetlength(PGRes,0,nFieldIndex);
						sprintf(szText,"%*s",nLength,PQgetvalue(PGRes,0,nFieldIndex));
					} else if(PQftype(PGRes,nFieldIndex)==701){
						/* si tratta di un float */
						nLength=strlen(PQfname(PGRes,nFieldIndex));
						sprintf(szText,"%*.*f",nLength,3,(atof(PQgetvalue(PGRes,0,nFieldIndex))));
					} else if(PQftype(PGRes,nFieldIndex)==23){
						/* si tratta di un int */
						nLength=strlen(PQfname(PGRes,nFieldIndex));
						sprintf(szText,"%*d",nLength,atoi(PQgetvalue(PGRes,0,nFieldIndex)));
					} else {
						/* altro... lo tratto come una stringa */
						nLength=strlen(PQfname(PGRes,nFieldIndex));
						sprintf(szText,"%*s",nLength,PQgetvalue(PGRes,0,nFieldIndex));
					}
					/*
					* Setto il testo del campo
					*/
					gtk_clist_set_text(GTK_CLIST(clist),nIndex,nFieldIndex,szText);
				}
			}
		}
		if(!bColloPresente){
			char *RecordData[1][MAX_FIELDS_NUMBER];
			int nMallocSize;
			/*
			* aggiungo una riga alla lista GTK
			*/
			for(nFieldIndex=0;nFieldIndex<nFields;nFieldIndex++){
				/*
				* Alloco lo spazio per una tupla
				*/
				nMallocSize=max(100,PQgetlength(PGRes,0,nFieldIndex)+1);
				RecordData[0][nFieldIndex]=malloc(nMallocSize);
			}
			for(nFieldIndex=0;nFieldIndex<nFields;nFieldIndex++){
				int nLength;
				/*
				* Setto il testo del campo
				*/
				if(PQftype(PGRes,nFieldIndex)==1042){
					/* si tratta di una stringa */
					nLength=PQgetlength(PGRes,0,nFieldIndex);
					sprintf(RecordData[0][nFieldIndex],"%*s",nLength,PQgetvalue(PGRes,0,nFieldIndex));
				} else if(PQftype(PGRes,nFieldIndex)==701){
					/* si tratta di un float */
					nLength=strlen(PQfname(PGRes,nFieldIndex));
					sprintf(RecordData[0][nFieldIndex],"%*.*f",nLength,3,(atof(PQgetvalue(PGRes,0,nFieldIndex))));
				} else if(PQftype(PGRes,nFieldIndex)==23){
					/* si tratta di un int */
					sprintf(RecordData[0][nFieldIndex],"%*d",nLength,atoi(PQgetvalue(PGRes,0,nFieldIndex)));
				} else {
					/* altro... lo tratto come una stringa */
					nLength=strlen(PQfname(PGRes,nFieldIndex));
					sprintf(RecordData[0][nFieldIndex],"%*s",nLength,PQgetvalue(PGRes,0,nFieldIndex));
				}
			}

			nRowIndex=gtk_clist_append((GtkCList *) clist, RecordData[0]);
			gtk_clist_moveto((GtkCList *) clist, nRowIndex, -1, 0.9, 0);


			/* 
			* libero la memoria allocata 
			*/
			for(nFieldIndex=0;nFieldIndex<nFields;nFieldIndex++){
				free(RecordData[0][nFieldIndex]);
			}
		}
	}

#ifdef TRACE
	trace_out_vstr_date(1," UpdateListFromSelect : Terminato ciclo di refresh dati a video");
#endif
	
	PQclear(PGRes);

	return nTuples;
}

/*
* CreateListFromSelect()
* Crea una lista GTK da una select SQL
* ritorna il numero di tuple o -1 se errore
*/
int CreateListFromSelect(GtkWidget *super_parent, GtkWidget *parent, GtkWidget **clist, char *szCListName, int nSelectMode, char *szSqlCmd, char *szOrdProg, int nNumCollo)
{
	int nTuples;
	int nFields;
	int nIndex;
	int nColumnIndex;
	int nColumnWidth;
	char szColumnName[128];
	char szBuffer[128];
	int nFieldIndex;
	char *RecordData[1][MAX_FIELDS_NUMBER];
	char szCmd[4096];
	char *pPtr;
	PGresult *PGRes = NULL;
	
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
	PGRes=PGExecSQL(FALSE,szSqlCmd);

	nTuples=PQntuples(PGRes);
	nFields=PQnfields(PGRes);

	if(nTuples>=0){
		/*
		* Creazione clist widget 
		*/

		*clist = gtk_clist_new (nFields);
		gtk_widget_set_name (*clist,szCListName);
		gtk_widget_ref (*clist);
		gtk_object_set_data_full (GTK_OBJECT (super_parent), szCListName, *clist, (GtkDestroyNotify) gtk_widget_unref);
		gtk_container_add (GTK_CONTAINER (parent), *clist);

		for(nFieldIndex=0;nFieldIndex<nFields;nFieldIndex++){
			/*
			* impostazione titolo colonna : utilizzo la .._set_column_title per avere la possibilita'
			* di ottenere la larghezza ottimale in fase di visualizzazione
			*/ 
			gtk_clist_set_column_title(GTK_CLIST(*clist), nFieldIndex, PQfname(PGRes,nFieldIndex));
		}
		gtk_clist_column_titles_show (GTK_CLIST (*clist));
		gtk_clist_set_selection_mode (GTK_CLIST (*clist),nSelectMode);
	}
	if(nTuples){
		for(nFieldIndex=0;nFieldIndex<nFields;nFieldIndex++){
			/*
			* Alloco lo spazio per una tupla
			*/
			RecordData[0][nFieldIndex]=malloc(100);
		}
		/*
		* Aggiungo le tuple alla lista (una alla volta)
		*/
		for(nIndex=0;nIndex<nTuples;nIndex++){

			for(nFieldIndex=0;nFieldIndex<nFields;nFieldIndex++){
				int nLength;
				/*
				* Setto il testo del campo
				*/
				if(PQftype(PGRes,nFieldIndex)==1042){
					/* si tratta di una stringa */
					nLength=PQgetlength(PGRes,0,nFieldIndex);
					sprintf(RecordData[0][nFieldIndex],"%*s",nLength,PQgetvalue(PGRes,nIndex,nFieldIndex));
				} else if(PQftype(PGRes,nFieldIndex)==701){
					/* si tratta di un float */
					nLength=strlen(PQfname(PGRes,nFieldIndex));
					sprintf(RecordData[0][nFieldIndex],"%*.*f",nLength,3,(atof(PQgetvalue(PGRes,nIndex,nFieldIndex))));
				} else if(PQftype(PGRes,nFieldIndex)==23){
					/* si tratta di un int */
					nLength=strlen(PQfname(PGRes,nFieldIndex));
					sprintf(RecordData[0][nFieldIndex],"%*d",nLength,atoi(PQgetvalue(PGRes,nIndex,nFieldIndex)));
				} else {
					/* altro... lo tratto come una stringa */
					nLength=strlen(PQfname(PGRes,nFieldIndex));
					sprintf(RecordData[0][nFieldIndex],"%*s",nLength,PQgetvalue(PGRes,nIndex,nFieldIndex));
				}

			}
			gtk_clist_append( (GtkCList *) *clist, RecordData[0]);
		}
		/* 
		* libero la memoria allocata 
		*/
		for(nFieldIndex=0;nFieldIndex<nFields;nFieldIndex++){
			free(RecordData[0][nFieldIndex]);
		}
	}
	
	/*
	* + st + 04/10/2000
	* Aggiusto la larghezza delle colonne in modo da poter visualizzare completamente i dati contenuti;
	* Per le colonne vuote imposto una larghezza minima che permette di visualizzare completamente il titolo;
	*/
	for(nColumnIndex=0;nColumnIndex<nFields;nColumnIndex++){
		sprintf(szColumnName, gtk_clist_get_column_title(GTK_CLIST(*clist), nColumnIndex));
		nColumnWidth=10*strlen(StrTrimAll(szColumnName));
		gtk_clist_set_column_min_width(GTK_CLIST(*clist), nColumnIndex,	nColumnWidth);
		gtk_clist_set_column_width(GTK_CLIST(*clist), nColumnIndex, gtk_clist_optimal_column_width(GTK_CLIST(*clist),nColumnIndex));
	}
	/*
	* callback per ordinamento clist
	*/
	gtk_signal_connect (GTK_OBJECT (*clist), "click-column", GTK_SIGNAL_FUNC (on_lst_sort_column_clicked), NULL);
	
	if(nTuples>=0){
		gtk_widget_show (*clist);
	}
	PQclear(PGRes);

	return nTuples;
}

/*
* void RefreshListaColliScartati(GtkWidget *parent,char *szSelect)
*
*	aggiorna la lista colli contenuta in parent tramite il comando di select (szSelect) passatole;
* se szSelect == NULL la funzione utilizza una select di default;
*/
void RefreshListaColliScartati(GtkWidget *parent,char *szSelect)
{
	char szParagraph[128];
	char szSelectCmd[2048];

	if(lst_colli_scartati != NULL){
		gtk_widget_destroy(lst_colli_scartati);
		lst_colli_scartati = NULL;
	}

	if (szSelect==NULL){
		/*
		* lettura dal file di configurazione della select
		*/
		strcpy(szParagraph,"Fine Linea");
		xncGetFileString(szParagraph,"SELECT_CMD_SCARTATI",  "", szSelectCmd, sizeof(szSelectCmd),Cfg.szCniCfg,NULL); 
	} else {
		strcpy(szSelectCmd,szSelect);
	}

	/*
	* Creazione clist widget su select
	*/
	CreateListFromSelect(main_window,parent,&lst_colli_scartati,"lst_colli_scartati",GTK_SELECTION_SINGLE,szSelectCmd,NULL,0);

	/*
	* callback per selezione riga
	*/
	/*
  gtk_signal_connect (GTK_OBJECT (lst_colli_scartati), "select_row", GTK_SIGNAL_FUNC (on_lst_colli_scartati_select_row), 0);
  gtk_signal_connect (GTK_OBJECT (lst_colli_scartati), "unselect_row", GTK_SIGNAL_FUNC (on_lst_colli_scartati_unselect_row), 0);
	*/
  gtk_signal_connect (GTK_OBJECT (lst_colli_scartati), "select_row", GTK_SIGNAL_FUNC (on_lst_colli_select_row), 0);
  gtk_signal_connect (GTK_OBJECT (lst_colli_scartati), "unselect_row", GTK_SIGNAL_FUNC (on_lst_colli_unselect_row), 0);
}

/*
* void RefreshListaColli(GtkWidget *parent,char *szSelect)
*
*	aggiorna la lista colli contenuta in parent tramite il comando di select (szSelect) passatole;
* se szSelect == NULL la funzione utilizza una select di default;
*/
void RefreshListaColli(GtkWidget *parent,char *szSelect)
{
	char szParagraph[128];
	char szSelectCmd[2048];

	if(lst_colli != NULL){
		gtk_widget_destroy(lst_colli);
		lst_colli = NULL;
	}

	if (szSelect==NULL){
		/*
		* lettura dal file di configurazione della select
		*/
		strcpy(szParagraph,"Fine Linea");
		xncGetFileString(szParagraph,"SELECT_CMD",  "", szSelectCmd, sizeof(szSelectCmd),Cfg.szCniCfg,NULL); 
	} else {
		strcpy(szSelectCmd,szSelect);
	}

	/*
	* Creazione clist widget su select
	*/
	CreateListFromSelect(main_window,parent,&lst_colli,"lst_colli",GTK_SELECTION_SINGLE,szSelectCmd,NULL,0);

	/*
	* callback per selezione riga
	*/
  gtk_signal_connect (GTK_OBJECT (lst_colli), "select_row", GTK_SIGNAL_FUNC (on_lst_colli_select_row), 0);
  gtk_signal_connect (GTK_OBJECT (lst_colli), "unselect_row", GTK_SIGNAL_FUNC (on_lst_colli_unselect_row), 0);
}

/*
* int ColloInPallettizzatore(char *szBarcodeCollo)
* Gestione collo pallet
* Decide se pallettizzare o meno il collo in base al valore 
* del campo roflgpz(ric_ord) : 1 ->pallet 0->fuori
* Ritorna il valore di roflgpz o -1 se errore
*/
int ColloInPallettizzatore(char *szBarcodeCollo, int *nNC)
{
	PGresult *PGResCollo;
	int nRetVal;
	int nFlgPz;
	

	/* CERCO IL COLLO NELL'ARCHIVIO PRODUZIONE */
	PGResCollo=PGExecSQL(Cfg.nDebugVersion,"select c.ordprog,c.ordkey,c.cpnmcol,c.cpstato,r.roflgpz,c.cpprgcl from col_prod c,ric_ord r where c.ordprog=r.ordprog and c.cpbrcde='%s';",szBarcodeCollo);


	if(PQntuples(PGResCollo)){
		/* 
		* TROVATO IL COLLO
		*/
		char szOrdProg[80];
		char szOrdKey[80];
		char szStato[20];
		int nCollo;
		int nNumCopie;
		int nNumRighe;
		int nPesoCalc;
		int nPesoDiff;

		strcpy(szOrdProg,PQgetvalue(PGResCollo,0,0));
		strcpy(szOrdKey,PQgetvalue(PGResCollo,0,1));
		nCollo=atoi(PQgetvalue(PGResCollo,0,2));
		nFlgPz=atoi(PQgetvalue(PGResCollo,0,4));
		*nNC=atoi(PQgetvalue(PGResCollo,0,5));


		nRetVal=nFlgPz;

	} else {
		nRetVal=-1;
	}

	PQclear(PGResCollo);

	return nRetVal;
}


/*
* BOOL ColloInBilancia(char *szBarcodeColloBilancia,char *nPesoColloBilancia)
* assegna al collo il peso rilevato in bilancia ed effettua il controllo delle tolleranza.
* N.B. Nei calcoli l'unita' di misura del peso e' sempre il GRAMMO.
* ritorna: TRUE se il peso del collo rientra nei vincoli di tolleranza
*/
BOOL ColloInBilancia(char *szBarcodeColloBilancia,int nPesoColloBilancia)
{
	PGresult *PGResCollo;
	PGresult *PGRes;
	BOOL bColloConforme=TRUE;
	
#ifdef TRACE_ELIMINATO
	trace_out_vstr_date(1,"ColloInBilancia : LANCIATA select ordprog,ordkey,cpnmcol,cpnmcpe,cpnmrgh from col_prod where cpbrcde='%s';",szBarcodeColloBilancia); 
#endif

	/* CERCO IL COLLO NELL'ARCHIVIO PRODUZIONE */
	PGResCollo=PGExecSQL(Cfg.nDebugVersion,"select ordprog,ordkey,cpnmcol,cpnmcpe,cpnmrgh,cpstato from col_prod where cpbrcde='%s';",szBarcodeColloBilancia);


	if(PQntuples(PGResCollo)){
		/* 
		* TROVATO IL COLLO
		*/
		char szOrdProg[80];
		char szOrdKey[80];
		char szStato[20];
		int nCollo;
		int nNumCopie;
		int nNumRighe;
		int nPesoCalc;
		int nPesoDiff;
		char szOrdineDaScartare[128];
		BOOL bOrdineDaScartare=FALSE;
		BOOL bCheckColloLettuteBRI=TRUE;

		strcpy(szOrdProg,PQgetvalue(PGResCollo,0,0));
		strcpy(szOrdKey,PQgetvalue(PGResCollo,0,1));
		nCollo=atoi(PQgetvalue(PGResCollo,0,2));
		nNumCopie=atoi(PQgetvalue(PGResCollo,0,3));
		nNumRighe=atoi(PQgetvalue(PGResCollo,0,4));

		PGRes=PGExecSQL(TRUE,"select roprdsc from ric_ord where ordprog='%s';",szOrdProg);
		if (PQntuples(PGRes)==1){
			strcpy(szOrdineDaScartare,PQgetvalue(PGRes,0,0));
			bOrdineDaScartare=szOrdineDaScartare[0]==ORDINE_DA_SCARTARE;
		}
		PQclear(PGRes);

		/*
		* controllo che il collo non sia gia' evaso
		*/
		strcpy(szStato,PQgetvalue(PGResCollo,0,5));
		if(szStato[0]!=COLLO_EVASO && szStato[0]!=COLLO_SPEDITO_HOST){

			/*
			* calcolo il peso predeterminato del collo
			*/
			nPesoCalc=CalcolaPesoCollo(szOrdKey,nCollo);

			nPesoDiff=abs(nPesoCalc-nPesoColloBilancia);

			gtk_label_printf(get_widget(main_window,"lb_barcode"),"%s",szBarcodeColloBilancia); 
			gtk_label_printf(get_widget(main_window,"lb_codice"),"%s",szOrdKey); 
			gtk_label_printf(get_widget(main_window,"lb_num_articoli"),"%d",nNumCopie); 
			gtk_label_printf(get_widget(main_window,"lb_num_righe"),"%d",nNumRighe); 
			gtk_label_printf(get_widget(main_window,"lb_peso_calc"),"%7.3f",(float)nPesoCalc/(float)1000); 
			gtk_label_printf(get_widget(main_window,"lb_peso_rea"),"%7.3f",(float)nPesoColloBilancia/(float)1000); 
			gtk_label_printf(get_widget(main_window,"lb_peso_diff"),"%7.3f",(float)nPesoDiff/(float)1000); 

			/*
			* cambio lo stato del collo e le relative righe (collo pesato: W)
			*/
			PesaCollo(szOrdKey,nCollo,nPesoColloBilancia,nPesoCalc);

			/*
			* update della lista colli o aggiunge se nuovo collo
			*/
			UpdateCollo(szOrdKey,nCollo);
			
			/*
			* GESTIONE CONTROLLO PESO SU SINGOLO COLLO
			*/
			if(Cfg.GestionePesatura==PESATURA_COLLO){
				char *pszColloKey;
				char *pszColloNum;
				int nTolleranza;
				int nRigaListaColli=0;
				GdkColor *cColor;

				/*
				* controllo il peso del collo
				*/
				if (Cfg.Tolleranza==TOLLERANZA_ASSOLUTA){
					nTolleranza=Cfg.nTolleranzaAss;
				} else if (Cfg.Tolleranza==TOLLERANZA_PERCENTUALE){
					nTolleranza=nPesoCalc*Cfg.nTolleranzaPerc/100;
				} else if (Cfg.Tolleranza==TOLLERANZA_PERCENTUALE_SOGLIA_MASSIMA){
					nTolleranza=min(nPesoCalc*Cfg.nTolleranzaPerc/100,Cfg.nSogliaMax);
				}

				/*
				* Controllo le letture di barcode id previste e fatte
				* Uso un flag sul collo, che alla fin fine e' cio' che deve essere scartato o no
				*/
				if (!CheckLettureBarcodeIDSuCollo(szOrdKey,nCollo)){
					bCheckColloLettuteBRI=FALSE;
#ifdef TRACE
					trace_out_vstr_date(1,"Collo(%s - %d) LETTURE BRI NON CONFORMI",szOrdKey,nCollo);
#endif
				}


				/*
				* 04-03-2008 rm : imposto valore minimo di soglia
				*/
				nTolleranza=max(nTolleranza,Cfg.nSogliaMin);

				if(bOrdineDaScartare){
					/* Collo DA SCARTARE */	
					bColloConforme=FALSE;
					//cColor=&cGREYCYAN;
					cColor=&cLRED;
#ifdef TRACE
					trace_out_vstr_date(1,"Pesato Collo(%s - %d - R:%7.2f C:%7.2f) da scartare [diff:%d toll:%d]",szOrdKey,nCollo,(float)nPesoColloBilancia/(float)1000,(float)nPesoCalc/(float)1000,nPesoDiff,nTolleranza);
#endif
					/*
					*	COLLOQUIO CON PLC
					* rm 23-11-2004 :  Faccio comunque andare avanti il collo
					*/
					if(Cfg.nPLC){
						SendCmd(fdPLC,CMD_SET,"CS 0 0 1",TRUE);	/* Forzatura variabile - Fine lettura collo */
					}
					ScartaCollo(szOrdKey,nCollo);
					UpdateCollo(szOrdKey,nCollo);
					/*
					* update della lista colli scartati o aggiunge se nuovo collo
					*/
					UpdateColloScartato(szOrdKey,nCollo);
				} else if(!bCheckColloLettuteBRI){
					/* Collo DA SCARTARE */	
					bColloConforme=FALSE;
					//cColor=&cORANGE;
					cColor=&cLRED;
#ifdef TRACE
					trace_out_vstr_date(1,"Collo(%s - %d] da scartare per letture barcode id non conformi",szOrdKey,nCollo);
#endif
					/*
					*	COLLOQUIO CON PLC
					* rm 23-11-2004 :  Faccio comunque andare avanti il collo
					*/
					if(Cfg.nPLC){
						SendCmd(fdPLC,CMD_SET,"CS 0 0 1",TRUE);	/* Forzatura variabile - Fine lettura collo */
					}
					ScartaCollo(szOrdKey,nCollo);
					UpdateCollo(szOrdKey,nCollo);
					/*
					* update della lista colli scartati o aggiunge se nuovo collo
					*/
					UpdateColloScartato(szOrdKey,nCollo);
				} else if(nPesoDiff>nTolleranza){
					/* Collo FUORI TOLLERANZA */	
					bColloConforme=FALSE;
					//cColor=&cRED;
					cColor=&cLRED;
#ifdef TRACE
					trace_out_vstr_date(1,"Pesato Collo(%s - %d - R:%7.2f C:%7.2f) fuori tolleranza [diff:%d toll:%d]",szOrdKey,nCollo,(float)nPesoColloBilancia/(float)1000,(float)nPesoCalc/(float)1000,nPesoDiff,nTolleranza);
#endif
					/*
					*	COLLOQUIO CON PLC
					* rm 23-11-2004 :  Faccio comunque andare avanti il collo
					*/
					if(Cfg.nPLC){
						SendCmd(fdPLC,CMD_SET,"CS 0 0 1",TRUE);	/* Forzatura variabile - Fine lettura collo */
					}
					ScartaCollo(szOrdKey,nCollo);
					UpdateCollo(szOrdKey,nCollo);
					/*
					* update della lista colli scartati o aggiunge se nuovo collo
					*/
					UpdateColloScartato(szOrdKey,nCollo);
				} else {
					/* Collo IN TOLLERANZA */	
					bColloConforme=TRUE;
					//cColor=&cGREEN;
					cColor=&cLGREEN;


					EvadiCollo(szOrdKey,nCollo,FALSE);
					UpdateCollo(szOrdKey,nCollo);

					/*
					* rm 22-11-2004 : gestione collo singolo
					* controllo se un intero sottordine e' stato evaso
					*/
					CheckSottordineEvaso(szOrdKey);
					/*
					*	COLLOQUIO CON PLC
					* segnalo la fine dell'analisi del collo presente in bilancia (conforme)
					*/
					if(Cfg.nPLC){
						SendCmd(fdPLC,CMD_SET,"CB 0 0 1",TRUE);	/* Forzatura variabile - Fine lettura collo */
					}
#ifdef TRACE
					trace_out_vstr_date(1,"Pesato Collo (%s - %d) conforme [diff:%d toll:%d]",szOrdKey,nCollo,nPesoDiff,nTolleranza);
#endif
				}
				/*
				* gestione prodotti pericolosi
				* rm 13-09-2006 : se il collo contiene prodotti con flag prperic=1
				* lo marco : giallo se buono , cyan se scartato
				* rm 11-12-2015 : se prodotto scartato per letture barcode identifier non gestisco il prodotto pericoloso
				*/
				if(bCheckColloLettuteBRI){
					if(ColloPericoloso(szOrdKey,nCollo)){
						if(bColloConforme){
							//cColor=&cYELLOW;
							cColor=&cLGREEN;
							gtk_text_printf(&cRED,txt_msgs,"ATTENZIONE : COLLO [%s.%d] - CONFORME - contiene prodotti pericolosi.\n",szOrdKey,nCollo);
						} else {
							//cColor=&cCYAN;
							cColor=&cLRED;
							gtk_text_printf(&cRED,txt_msgs,"ATTENZIONE : COLLO [%s.%d] - SCARTATO - contiene prodotti pericolosi.\n",szOrdKey,nCollo);
						}
					}
				}

				/*
				* update a video del collo
				*/
				while(nRigaListaColli<(GTK_CLIST(lst_colli)->rows)){
						
					gtk_clist_get_text(GTK_CLIST(lst_colli),nRigaListaColli,Cfg.nColloKeyField,&pszColloKey);
					gtk_clist_get_text(GTK_CLIST(lst_colli),nRigaListaColli,Cfg.nColloNumField,&pszColloNum);

					if (!(strcmp(pszColloKey,szOrdKey)) && atoi(pszColloNum)==nCollo){
						gtk_clist_set_background(GTK_CLIST(lst_colli),nRigaListaColli,cColor);
						//gtk_clist_set_foreground(GTK_CLIST(lst_colli),nRigaListaColli,&cWHITE);
						gtk_clist_set_foreground(GTK_CLIST(lst_colli),nRigaListaColli,&cBLACK);
					}
					nRigaListaColli++;
				}
				
			} else if(Cfg.GestionePesatura==PESATURA_ORDINE){
				/*
				* GESTIONE CONTROLLO PESO SU ORDINE DI PRODUZIONE
				*/
				/*
				* controllo se un intero sottordine e' stato evaso
				*/
				CheckSottordineEvaso(szOrdKey);

				if(Cfg.nPLC){
					SendCmd(fdPLC,CMD_SET,"CB 0 0 1",TRUE);	/* Forzatura variabile - Fine lettura collo */
				}
			}
		} else {
			/* il collo e' gia' stato evaso */
			ShowColloEvaso(szOrdProg,nCollo);
		}

	} else {
		/* barcode non corrispondente ad alcun collo */
		gtk_text_printf(&cRED,txt_msgs,"[%s] barcode non corrispondente a nessun collo in archivio produzione\n",szBarcodeColloBilancia); 
#ifdef TRACE
		trace_out_vstr_date(1,"[%s] barcode non corrispondente a nessun collo in archivio produzione",szBarcodeColloBilancia); 
#endif
		ShowDlgRequest(nPesoColloBilancia,szBarcodeColloBilancia,BARCODE_MSG_NOT_FOUND);
	}

	PQclear(PGResCollo);

	return bColloConforme;
}

void UpdateCollo(char *szOrdKey,int nCollo)
{
	char szSqlCmd[2048];

	if(!xncGetFileString("Fine Linea","UPDATE_CMD", "", szSqlCmd, sizeof(szSqlCmd),Cfg.szCniCfg,NULL)){
#ifdef TRACE
		trace_out_vstr_date(1,"UpdateCollo() : xncGetFileString() : errore [%s]",strerror(errno));
#endif
	}
	UpdateListFromSelect(lst_colli,szSqlCmd,szOrdKey,nCollo,Cfg.nColloKeyField,Cfg.nColloNumField);
}

void UpdateColloScartato(char *szOrdKey,int nCollo)
{
	char szSqlCmd[2048];

	if(!xncGetFileString("Fine Linea","UPDATE_CMD_SCARTATI", "", szSqlCmd, sizeof(szSqlCmd),Cfg.szCniCfg,NULL)){
#ifdef TRACE
		trace_out_vstr_date(1,"UpdateColloScartato() : xncGetFileString() : errore [%s]",strerror(errno));
#endif
	}
	UpdateListFromSelect(lst_colli_scartati,szSqlCmd,szOrdKey,nCollo,Cfg.nColloKeyField,Cfg.nColloNumField);
}

/*******************************************************
* VERSIONE CON GESTIONE PDF
********************************************************/
			
/*
* int CalcolaPesoCollo(char *szOrdKey,int nCollo)
*	calcola il peso del collo (in grammi) avente codice szOrdKey
*/
int CalcolaPesoCollo(char *szOrdKey,int nCollo)
{
	PGresult *PGResRighe;
	PGresult *PGResImb;
	int nRigheCollo;
	int nPesoCollo=0;
	int nIndex;
	int nPesoImballo=0;
	char szCodiceImballo[80];


	PGResRighe=PGExecSQL(FALSE,"select rpcdpro,rpqtspe from rig_prod where ordkey='%s' and rpnmcol=%d;",szOrdKey,nCollo);
	nRigheCollo=PQntuples(PGResRighe);

	if (nRigheCollo!=0){

		for(nIndex=0;nIndex<nRigheCollo;nIndex++){
			PGresult *PGResArticoli;
			char szCodProd[80];
			int nQtaArticolo;
			int nPesoArticolo;

			strcpy(szCodProd,PQgetvalue(PGResRighe,nIndex,0));
			nQtaArticolo=atoi(PQgetvalue(PGResRighe,nIndex,1));
			
			PGResArticoli=PGExecSQL(FALSE,"select prpesgr from articoli where prcdpro='%s';",szCodProd);
			nPesoArticolo=atoi(PQgetvalue(PGResArticoli,0,0));
			PQclear(PGResArticoli);

			nPesoCollo+=(nPesoArticolo*nQtaArticolo);
		}

		/*
		* sommo il peso dell'imballo
		*/
		PGResImb=PGExecSQL(FALSE,"select cptpfor from col_prod where ordkey='%s' and cpnmcol=%d;",szOrdKey,nCollo);
		strcpy(szCodiceImballo,PQgetvalue(PGResImb,0,0));
		PQclear(PGResImb);
		
		PGResImb=PGExecSQL(FALSE,"select tara from imballi where codice='%s';",szCodiceImballo);
		if(PQntuples(PGResImb)){
			nPesoImballo=atoi(PQgetvalue(PGResImb,0,0));
		}
		PQclear(PGResImb);

		nPesoCollo+=(nPesoImballo);

	} else {
		/*
		* controllo che non sia un collo aggiunto in bilancia
		*
		* controllo anche che non sia un collo fittizio nel qual caso 
		* il peso è zero ma senza errore
		*/
		PGresult *PGResColloAdd;
		PGresult *PGResCollo;
		int nProgCollo=0;
		int nProgColloAdd=0;
		char szCPFLFTZ[80];

		PGResColloAdd=PGExecSQL(Cfg.nDebugVersion,"select last_value from progcollo_add");
		PGResCollo=PGExecSQL(Cfg.nDebugVersion,"select cpprgcl,cpflftz,cptpfor from col_prod where ordkey='%s' and cpnmcol=%d;",szOrdKey,nCollo);
		if(PGResColloAdd && PGResCollo){
			nProgColloAdd=atoi(PQgetvalue(PGResColloAdd,0,0));
			nProgCollo=atoi(PQgetvalue(PGResCollo,0,0));
			strcpy(szCPFLFTZ,PQgetvalue(PGResCollo,0,1));
			strcpy(szCodiceImballo,PQgetvalue(PGResCollo,0,2));
		}
		PQclear(PGResColloAdd);
		PQclear(PGResCollo);
		if (szCPFLFTZ[0]==COLLO_FITTIZIO){
			PGResImb=PGExecSQL(FALSE,"select tara from imballi where codice='%s';",szCodiceImballo);
			if(PQntuples(PGResImb)){
				nPesoImballo=atoi(PQgetvalue(PGResImb,0,0));
			}
			PQclear(PGResImb);

			nPesoCollo+=(nPesoImballo);
			gtk_text_printf(NULL,txt_msgs,"Collo Fittizio [%s - %d]\n",szOrdKey,nCollo); 
#ifdef TRACE
			trace_out_vstr_date(1,"CalcolaPesoCollo(%s,%d) - Collo Fittizio",szOrdKey,nCollo); 
#endif
		} else {
			if (nProgCollo>=nProgColloAdd){
				/*
				* Un collo aggiunto in bilancia
				*/
				nPesoCollo=0;

			} else {
				/* ERRORE */
				gtk_text_printf(NULL,txt_msgs,"Errore in select su rig_prod [%s - %d]\n",szOrdKey,nCollo); 
#ifdef TRACE
				trace_out_vstr_date(1,"CalcolaPesoCollo(%s,%d) - Fallita select su rig_prod",szOrdKey,nCollo); 
#endif
			}
		}
	}
	PQclear(PGResRighe);

	return nPesoCollo;
}

/*******************************************************
* FINE VERSIONE CON GESTIONE PDF
********************************************************/
			


/*
* BOOL CambiaStatoSottordine(char *szOrdKey, char cStato)
* cambia lo stato del sottordine in PRODUZIONE in cStato
* ritorna: TRUE se update effettuato.
*/
BOOL CambiaStatoSottordine(char *szOrdKey, char cStato)
{
	BOOL bRetVal=TRUE;
	PGresult *PGRes = NULL;

	PGRes=PGExecSQL(FALSE,"update ord_prod set opstato='%c' where ordkey='%s';",cStato,szOrdKey);
	if (!PGRes || PQresultStatus(PGRes) != PGRES_COMMAND_OK ) {
		bRetVal=FALSE;
		gtk_text_printf(NULL,txt_msgs,"Errore in update su ord_prod [%s]\n",szOrdKey);
#ifdef TRACE
		trace_out_vstr_date(1,"CambiaStatoSottordine(%s,%c) - Fallito update su ric_ord",szOrdKey,cStato);
#endif
	}
	PQclear(PGRes);

	return(bRetVal);
}


/*
* BOOL CambiaStatoOrdine(char *szOrdProg, char cStato)
* cambia lo stato del sottordine in PRODUZIONE in cStato
* ritorna: TRUE se update effettuato.
*/
BOOL CambiaStatoOrdine(char *szOrdProg, char cStato)
{
	BOOL bRetVal=TRUE;
	PGresult *PGRes = NULL;

	PGRes=PGExecSQL(Cfg.nDebugVersion,"update ric_ord set rostato='%c' where ordprog='%s';",cStato,szOrdProg);
	if (!PGRes || PQresultStatus(PGRes) != PGRES_COMMAND_OK ) {
		bRetVal=FALSE;
		gtk_text_printf(NULL,txt_msgs,"CambiStatoOrdine: Errore in update su ric_ord [%s - Stato %c]\n",szOrdProg,cStato);
#ifdef TRACE
		trace_out_vstr_date(1,"CambiaStatoOrdine(%s,%c) - Fallito update su ric_ord",szOrdProg,cStato);
#endif
	}
	PQclear(PGRes);

	return(bRetVal);
}

BOOL SetFlagBarcodeIdCollo(char *szOrdKey,int nCollo, char cStato)
{
	BOOL bRetVal=TRUE;
	PGresult *PGRes = NULL;

	PGRes=PGExecSQL(Cfg.nDebugVersion,"update col_prod set cpflbri='%c' where ordkey='%s' and cpnmcol=%d;",cStato,szOrdKey,nCollo);
	if (!PGRes || PQresultStatus(PGRes) != PGRES_COMMAND_OK ) {
		bRetVal=FALSE;
//		gtk_text_printf(NULL,txt_msgs,": Errore in update su ric_ord [%s - Stato %c]\n",szOrdProg,cStato);
#ifdef TRACE
		trace_out_vstr_date(1,"SetFlagBarcodeIdCollo (%s,%d,%c) - Fallito update su col_prod",szOrdKey,nCollo,cStato);
#endif
	}
	PQclear(PGRes);

	return(bRetVal);
}


/*
* BOOL EvadiCollo(char *szOrdKey,int nCollo,BOOL bForzatura)
* setta lo stato del collo (e delle corripondenti righe)in EVASO
* gli assegna il peso reale e la data-ora di evasione
* ritorna: TRUE se update effettuato.
*/
BOOL EvadiCollo(char *szOrdKey,int nCollo,BOOL bForzatura)
{
	BOOL bRetVal=TRUE;
	PGresult *PGResCollo = NULL;
	PGresult *PGRes;
	char cStatoCollo=COLLO_EVASO;
	char cStatoRiga=RIGA_EVASA;
	char szTimeBuffer[128];
	char szDateBuffer[128];

#ifdef TRACE
	trace_out_vstr_date(1,"EvadiCollo [%s][%d][%s]",szOrdKey,nCollo,bForzatura?"FORZATO":"NORMALE");
#endif

//	GetTimeHHMMSS(time((long *)0),szTimeBuffer);
//	GetDateYYYYMMDD(time((long *)0),szDateBuffer);
	GetTimeHHMMSSFromDB(szTimeBuffer);
	GetDateYYYYMMDDFromDB(szDateBuffer);

	PGResCollo=PGExecSQL(Cfg.nDebugVersion,"update col_prod set cpstato='%c',cpdteva='%s',cptmeva='%s',cpcdflg='%s' where ordkey='%s' AND cpnmcol = %d;",cStatoCollo,szDateBuffer,szTimeBuffer,bForzatura?"F":" ",szOrdKey,nCollo);
	if (!PGResCollo || PQresultStatus(PGResCollo) != PGRES_COMMAND_OK ) {
		bRetVal=FALSE;
#ifdef TRACE
		trace_out_vstr_date(1,"Fallito update su col_prod [%s - %d]",szOrdKey,nCollo);
#endif
	} else {
		PGRes=PGExecSQL(FALSE,"update rig_prod set rpstato='%c' where ordkey='%s' AND rpnmcol = %d;",cStatoRiga,szOrdKey,nCollo);
		PQclear(PGRes);
	}

	PQclear(PGResCollo);

	return(bRetVal);
}

/*
* BOOL ScartaCollo(char *szOrdKey,int nCollo)
* setta lo stato del collo (e delle corripondenti righe)in SCARTATO
* gli assegna il peso reale e la data-ora di evasione
* ritorna: TRUE se update effettuato.
*/
BOOL ScartaCollo(char *szOrdKey,int nCollo)
{
	BOOL bRetVal=TRUE;
	PGresult *PGResCollo = NULL;
	PGresult *PGRes;
	char cStatoCollo=COLLO_SCARTATO;
	char cStatoRiga=RIGA_SCARTATA;
	char szTimeBuffer[128];
	char szDateBuffer[128];

#ifdef TRACE
	trace_out_vstr_date(1,"ScartaCollo [%s][%d]",szOrdKey,nCollo);
#endif

	PGResCollo=PGExecSQL(Cfg.nDebugVersion,"update col_prod set cpstato='%c' where ordkey='%s' AND cpnmcol = %d;",cStatoCollo,szOrdKey,nCollo);
	if (!PGResCollo || PQresultStatus(PGResCollo) != PGRES_COMMAND_OK ) {
		bRetVal=FALSE;
#ifdef TRACE
		trace_out_vstr_date(1,"Fallito update su col_prod [%s - %d]",szOrdKey,nCollo);
#endif
	} else {
		PGRes=PGExecSQL(FALSE,"update rig_prod set rpstato='%c' where ordkey='%s' AND rpnmcol = %d;",cStatoRiga,szOrdKey,nCollo);
		PQclear(PGRes);
	}

	PQclear(PGResCollo);

	return(bRetVal);
}

/*
* BOOL PesaCollo(char *szOrdKey,int nCollo,int nPesoReale,int nPesoCalcolato)
* setta lo stato del collo (e delle corripondenti righe) in PESATO
* gli assegna il peso reale e la data-ora di evasione
* ritorna: TRUE se update effettuato.
*/
BOOL PesaCollo(char *szOrdKey,int nCollo,int nPesoReale,int nPesoCalcolato)
{
	BOOL bRetVal=TRUE;
	PGresult *PGResCollo = NULL;
	PGresult *PGRes;
	char cStatoCollo=COLLO_PESATO;
	char cStatoRiga=RIGA_PESATA;
	char szTimeBuffer[128];
	char szDateBuffer[128];

#ifdef TRACE
	trace_out_vstr_date(1,"PesaCollo [%s][%d-%d]",szOrdKey,nCollo,nPesoReale,nPesoCalcolato);
#endif

//	GetTimeHHMMSS(time((long *)0),szTimeBuffer);
//	GetDateYYYYMMDD(time((long *)0),szDateBuffer);
	GetTimeHHMMSSFromDB(szTimeBuffer);
	GetDateYYYYMMDDFromDB(szDateBuffer);

	PGResCollo=PGExecSQL(Cfg.nDebugVersion,"update col_prod set cpstato='%c',cppsrea=%d,cppspre=%d,cpdteva='%s',cptmeva='%s' where ordkey='%s' AND cpnmcol = %d;",cStatoCollo,nPesoReale,nPesoCalcolato,szDateBuffer,szTimeBuffer,szOrdKey,nCollo);
	if (!PGResCollo || PQresultStatus(PGResCollo) != PGRES_COMMAND_OK ) {
		bRetVal=FALSE;
#ifdef TRACE
		trace_out_vstr_date(1,"Fallito update su col_prod [%s - %d]",szOrdKey,nCollo);
#endif
	} else {
		PGRes=PGExecSQL(Cfg.nDebugVersion,"update rig_prod set rpstato='%c' where ordkey='%s' AND rpnmcol = %d;",cStatoRiga,szOrdKey,nCollo);
		PQclear(PGRes);
	}

	PQclear(PGResCollo);

	return(bRetVal);
}

/*
* BOOL CheckSottordineEvaso(szOrdKey)
* controlla che il sottordine szOrdKey sia evaso (-> tutti i suoi colli AUTOMATICI);
* settando il colore delle rispettive righe nella lista colli.
* ritorna: TRUE se il sottordine e' evaso completamente ed
*               il suo peso rientra nei vincoli di tolleranza.
*/
BOOL CheckSottordineEvaso(char *szOrdKey)
{
	PGresult *PGRes;
	BOOL bRetVal=TRUE;
	int nColliAutomatici;
	int nPesoCalcolatoOrdine;
	int nPesoRealeOrdine;
	char szCodiceOrdine[64];
	char szBuffer[128];
	char szOrdineDaScartare[128];
	BOOL bOrdineDaScartare=FALSE;
	int nIndex;
	char cStatoColli;

	if(Cfg.GestionePesatura==PESATURA_ORDINE){
		cStatoColli=COLLO_PESATO;
	} else {
		cStatoColli=COLLO_EVASO;
	}
	
	PGRes=PGExecSQL(FALSE,"select opnmcla,oppspre,roprdsc from ord_prod,ric_ord where ord_prod.ordkey='%s' and ord_prod.ordprog=ric_ord.ordprog;",szOrdKey);
	if (PQntuples(PGRes)==1){
		nColliAutomatici=atoi(PQgetvalue(PGRes,0,0));
		nPesoCalcolatoOrdine=atoi(PQgetvalue(PGRes,0,1));
		strcpy(szOrdineDaScartare,PQgetvalue(PGRes,0,2));
		bOrdineDaScartare=szOrdineDaScartare[0]==ORDINE_DA_SCARTARE;

	} else {
		bRetVal=FALSE;
	}
	PQclear(PGRes);

	if(bRetVal){

		/* verifico se sottordine evaso */
		PGRes=PGExecSQL(TRUE,"select cpnmcol,cppsrea from col_prod where ordkey='%s' and cpstato='%c' and cpswlin!='1';",szOrdKey,cStatoColli);

		if (PQresultStatus(PGRes)==PGRES_TUPLES_OK){

			if (PQntuples(PGRes)==nColliAutomatici){

				/*
				* tutti i colli AUTOMATICI del sottordine sono stati pesati/evasi
				*/
				char *pszColloKey;
				int nRigaListaColli=0;

				/* 
				* st - 4/12/2000
				* ricavo anche i colli marchiati eliminati (D) in fine linea per calcolare il peso dell'ordine
				*/
				PQclear(PGRes);
				PGRes=PGExecSQL(TRUE,"select cpnmcol,cppsrea from col_prod where ordkey='%s' and cpstato in ('%c','%c') and cpswlin!='1';",szOrdKey,cStatoColli,COLLO_ELIMINATO);

				nColliAutomatici=PQntuples(PGRes);

				nPesoRealeOrdine=0;
				for(nIndex=0;nIndex<nColliAutomatici;nIndex++){
					nPesoRealeOrdine+=atoi(PQgetvalue(PGRes,nIndex,1));
				}

				/*
				* GESTIONE CONTROLLO PESO SU ORDINE DI PRODUZIONE
				*/
				if(Cfg.GestionePesatura==PESATURA_ORDINE){
					int nTolleranza;
					GdkColor *cColor;

					/*
					* Controllo peso sottordine
					*/
					if (Cfg.Tolleranza==TOLLERANZA_ASSOLUTA){
						nTolleranza=Cfg.nTolleranzaAss;
					} else if (Cfg.Tolleranza==TOLLERANZA_PERCENTUALE){
						nTolleranza=nPesoCalcolatoOrdine*Cfg.nTolleranzaPerc/100;
					} else if (Cfg.Tolleranza==TOLLERANZA_PERCENTUALE_SOGLIA_MASSIMA){
						nTolleranza=min(nPesoCalcolatoOrdine*Cfg.nTolleranzaPerc/100,Cfg.nSogliaMax);
					}

					/*
					* Visualizzo il peso dell'ordine(calcolato, reale e differenza) nelle label
					*/
					gtk_label_printf(get_widget(main_window,"lb_barcode"),""); 
					gtk_label_printf(get_widget(main_window,"lb_num_articoli"),""); 
					gtk_label_printf(get_widget(main_window,"lb_num_righe"),""); 
					gtk_label_printf(get_widget(main_window,"lb_peso_calc"),"%7.3f",(float)nPesoCalcolatoOrdine/(float)1000); 
					gtk_label_printf(get_widget(main_window,"lb_peso_rea"),"%7.3f",(float)nPesoRealeOrdine/(float)1000); 
					gtk_label_printf(get_widget(main_window,"lb_peso_diff"),"%7.3f",(float)(abs(nPesoCalcolatoOrdine-nPesoRealeOrdine))/(float)1000); 

					/*
					* Verifico predestinazione allo scarto o peso fuori tolleranza
					*/
					if(bOrdineDaScartare || (abs(nPesoRealeOrdine - nPesoCalcolatoOrdine)>nTolleranza)){
						/* Ordine FUORI TOLLERANZA */	
						PGresult *PGResUpdate;
						bRetVal=FALSE;
						if(bOrdineDaScartare){
#ifdef TRACE
							trace_out_vstr_date(1,"CheckSottordineEvaso(%s) - Sottordine scartato in console",szOrdKey);
#endif
							cColor=&cGREYCYAN;
						} else {
#ifdef TRACE
							trace_out_vstr_date(1,"CheckSottordineEvaso(%s) - Sottordine fuori tolleranza [diff:%d toll:%d]",szOrdKey,abs(nPesoRealeOrdine - nPesoCalcolatoOrdine),nTolleranza);
#endif
							cColor=&cRED;
						}

						/*
						* st - 28/01/2002 se il sottordine e' fuori tolleranza marchio i colli in Scartati
						* riferimento Fabio Casnici
						*/
						PGResUpdate=PGExecSQL(Cfg.nDebugVersion,"update col_prod set cpstato='%c' where ordkey='%s';",COLLO_SCARTATO,szOrdKey);
						if(!PGResUpdate || PQresultStatus(PGResUpdate) != PGRES_COMMAND_OK ) {
							gtk_text_printf(NULL,txt_msgs,"errore in update stato colli [ordine %s]\n",szOrdKey);
#ifdef TRACE
							trace_out_vstr_date(1,"errore in update stato colli [ordine %s]\n",szOrdKey);
#endif
						}
						PQclear(PGResUpdate);

						/*
						*	COLLOQUIO CON PLC
						* segnalo l'evasione di un sottordine da scartare perche' fuori tolleranza
						*/
						if(Cfg.nPLC){
							SendCmd(fdPLC,CMD_SET,"CS 0 0 1",TRUE);	/* Forzatura variabile - Sottordine Scartato */
						}
			
					} else {
						/* Ordine IN TOLLERANZA */	
						PGresult *PGResUpdate;
						bRetVal=TRUE;
						cColor=&cGREEN;
						
#ifdef TRACE
						trace_out_vstr_date(1,"CheckSottordineEvaso(%s) - Sottordine conforme [diff:%d toll:%d]",szOrdKey,abs(nPesoRealeOrdine - nPesoCalcolatoOrdine),nTolleranza);
#endif
						/*
						* st - 31/10/2001 evado i colli dell'ordine
						*/
						PGResUpdate=PGExecSQL(Cfg.nDebugVersion,"update col_prod set cpstato='%c' where ordkey='%s';",COLLO_EVASO,szOrdKey);
						if(!PGResUpdate || PQresultStatus(PGResUpdate) != PGRES_COMMAND_OK ) {
							gtk_text_printf(NULL,txt_msgs,"errore in update stato colli [ordine %s]\n",szOrdKey);
#ifdef TRACE
							trace_out_vstr_date(1,"errore in update stato colli [ordine %s]\n",szOrdKey);
#endif
						}
						PQclear(PGResUpdate);

						/*
						*	COLLOQUIO CON PLC
						* segnalo l'evasione di un sottordine (conforme)
						*/
						if(Cfg.nPLC){
							SendCmd(fdPLC,CMD_SET,"FO 0 0 1",TRUE);	/* Forzatura variabile - Fine Ordine */
						}

					}

					/*
					* update colore a video dei colli appartenenti al sottordine
					*/
					while(nRigaListaColli<(GTK_CLIST(lst_colli)->rows)){
							
						gtk_clist_get_text(GTK_CLIST(lst_colli),nRigaListaColli,Cfg.nColloKeyField,&pszColloKey);

						if (!(strcmp(pszColloKey,szOrdKey))){
							gtk_clist_set_background(GTK_CLIST(lst_colli),nRigaListaColli,cColor);
							gtk_clist_set_foreground(GTK_CLIST(lst_colli),nRigaListaColli,&cWHITE);
						}
						nRigaListaColli++;
					}
				} else {
					/*
					* gestione collo singolo
					*/

					/*
					*	COLLOQUIO CON PLC
					* segnalo il passaggio di un collo
					*/
					if(Cfg.nPLC){
						SendCmd(fdPLC,CMD_SET,"FO 0 0 1",TRUE);	/* Forzatura variabile - Fine Ordine */
					}
				}

				if(bRetVal){
					/*
					* setto lo stato del sottordine in EVASO
					*/
					CambiaStatoSottordine(szOrdKey,ORDINE_EVASO);

					/*
					* setto il peso del sottordine: gli assegno data e ora di evasione
					*/
					SettaPesoSottordine(szOrdKey,nPesoRealeOrdine);

					/*
					* controllo se l'intero ordine corrispondente e' evaso
					*/
					if(Cfg.nSAP){
						strcpy(szCodiceOrdine,SubStr(szOrdKey,0,10));
					} else {
						strcpy(szCodiceOrdine,SubStr(szOrdKey,0,6));
					}
					if(CheckOrdineEvaso(szCodiceOrdine)){
						sprintf(szBuffer,"%s,%c",szCodiceOrdine,ORDINE_EVASO);
						if(!SendSockCmd(Cfg.nServicePort,Cfg.szSocksHost,CMD_UPDATE_ORDINE,szBuffer)){
	#ifdef TRACE
							trace_out_vstr_date(1,"Errore in spedizione msg:%s via socket",szBuffer);
	#endif
						}
					}
				}

			}
		} else {
			gtk_text_printf(NULL,txt_msgs,"Errore in select ordini in prelievo su col_prod [%s]\n",szOrdKey);
#ifdef TRACE
			trace_out_vstr_date(1,"CheckSottordineEvaso(%s) - Fallita select su col_prod",szOrdKey);
#endif
		}	
		PQclear(PGRes);
	}

	return bRetVal;
}


/*
* BOOL CheckOrdineEvaso(szOrdProg)
* controlla che l'ordine szOrdProg sia evaso (-> tutti i suoi sottordini);
* settando il colore delle rispettive righe nella lista colli.
* ritorna: TRUE se l'ordine e' evaso completamente.
*/
BOOL CheckOrdineEvaso(char *szOrdProg)
{
	PGresult *PGRes;
	BOOL bRetVal=TRUE;
	BOOL bOK=TRUE;
	BOOL bOrdineEvaso=FALSE;

#ifdef TRACE
	trace_out_vstr_date(1,"-- CheckOrdineEvaso(%s) - Inizio",szOrdProg);
#endif
	
	PGRes=PGExecSQL(Cfg.nDebugVersion,"select oppsrea from ord_prod where ordprog='%s' and opstato!='%c';",szOrdProg,ORDINE_EVASO);

	if (PQresultStatus(PGRes)==PGRES_TUPLES_OK){

		if (PQntuples(PGRes)==0){
			/*
			* tutti i sottordini sono stati evasi -> l'ordine e' evaso
			*/
			int nPesoRealeOrdine;
			int nPesoCalcolatoOrdine;
			PGresult *PGResOrdine;

			bOrdineEvaso=TRUE;

			PGResOrdine=PGExecSQL(Cfg.nDebugVersion,"select ropspre from ric_ord where ordprog='%s';",szOrdProg);

			if(PQntuples(PGResOrdine)){
				nPesoCalcolatoOrdine=atoi(PQgetvalue(PGResOrdine,0,0));
			}
			PQclear(PGResOrdine);

#ifdef TRACE
			trace_out_vstr_date(1,"  CheckOrdineEvaso : Lanciata CalcPesoRealeOrdine(%s)",szOrdProg);
#endif
			nPesoRealeOrdine=CalcPesoRealeOrdine(szOrdProg);

			/*
			* cambio lo stato dell'ordine, data e ora evasione
			*/
#ifdef TRACE
			trace_out_vstr_date(1,"  CheckOrdineEvaso : Lanciata CambiaStatoOrdine(%s)",szOrdProg);
#endif
			CambiaStatoOrdine(szOrdProg,ORDINE_EVASO);

		} else {
			bRetVal=FALSE;
		}
	} else {
		gtk_text_printf(NULL,txt_msgs,"Errore in select ordini in prelievo su ord_prod [%s]\n",szOrdProg);
#ifdef TRACE
		trace_out_vstr_date(1,"CheckOrdineEvaso(%s) - Fallita select su ord_prod",szOrdProg);
#endif
		bRetVal=FALSE;
	}	

	PQclear(PGRes);

	/*
	* in modalita' SAP occorre evadere l'ordine appena evaso
	* se l'opzione di esecuzione automatica e' attiva (Cfg.nExecuteCni2Sap)
	* rm 20-01-2004 : aggiunta abilitazione/disabilitazione invio dati a host
	*/
	if(bOrdineEvaso && Cfg.nSAP){
		char szOrdiniFile[128];
		char szColliFile[128];
		char szOrdiniFileBackup[128];
		char szColliFileBackup[128];
		BOOL bRetVal=TRUE;
		PGresult *PGResColli;
		PGresult *PGResUpdate;
		PGresult *PGResOrdini;
		int nNumeroColli,nIndexColli,nColliSpediti;
		char szSQLCmdUpdate[1024];
		char szCommand[1024];
		int nRC=0;

		FILE *fpOrdini=NULL;
		FILE *fpColli=NULL;

		if(Cfg.nExecuteCni2Sap){
			/*
			* per SAP occorre lanciare un comando ogni volta che si evade un ordine 
			*/
			sprintf(szOrdiniFile,"%s/%s",Cfg.szPathExport,StrTrimAll(Cfg.szExpOrdiniFile));
			sprintf(szColliFile,"%s/%s",Cfg.szPathExport,StrTrimAll(Cfg.szExpColliFile));

			/*
			* rm 20-01-2004 : aggiunta abilitazione/disabilitazione invio dati a host
			*/
			if(Cfg.nEnableTransmit){
				if((fpOrdini=fopen(szOrdiniFile,"a"))==NULL){
#ifdef TRACE
					trace_out_vstr_date(1,"SpedizioneCedola() : Errore in apertura file ordini [%s]",szOrdiniFile);
#endif
					gtk_text_printf(&cCYAN,txt_msgs,"SpedizioneCedola() : Errore in apertura file ordini [%s]\n",szOrdiniFile);
					bRetVal=FALSE;
				}
				if((fpColli =fopen(szColliFile,"a"))==NULL){
#ifdef TRACE
					trace_out_vstr_date(1,"SpedizioneCedola() : Errore in apertura file colli [%s]",szColliFile);
#endif
					gtk_text_printf(&cCYAN,txt_msgs,"SpedizioneCedola() : Errore in apertura file colli [%s]\n",szColliFile);
					bRetVal=FALSE;
				}
			}

			if(bRetVal){

				PGResOrdini=PGExecSQL(Cfg.nDebugVersion,"select ordprog,ronmcll,ropsrea,ronelab from ric_ord where ordprog='%s';",szOrdProg);	
				if(PQntuples(PGResOrdini)){

					/* selezioni i colli relativi all'ordine */
					PGResColli=PGExecSQL(Cfg.nDebugVersion,"select ordprog,cptpfor,cpprgcl from col_prod where ordprog='%s' order by ordkey,cpnmcol;",szOrdProg);	

					if ((nNumeroColli=PQntuples(PGResColli))){
						for (nIndexColli=0;nIndexColli<nNumeroColli;nIndexColli++){
							/*
							* rm 20-01-2004 : aggiunta abilitazione/disabilitazione invio dati a host
							*/
							if(Cfg.nEnableTransmit){
								/*
								* spedizione a Host dei colli relativi all'ordine
								*/
								WriteAsciiRecord(fpColli, &tColCni,PGResColli,nIndexColli);
								nColliSpediti++;
							}
						}

						/*
						* cambio lo stato dei colli relativi all'ordine(E -> H)
						*/
						sprintf(szSQLCmdUpdate,"update col_prod set cpstato='%c' where ordprog='%s';", COLLO_SPEDITO_HOST, szOrdProg);
						PGResUpdate=PGExecSQL(Cfg.nDebugVersion,szSQLCmdUpdate);
						PQclear(PGResUpdate);

						/*
						* cambio lo stato delle righe relative all'ordine(E -> H)
						*/
						sprintf(szSQLCmdUpdate,"update rig_prod set rpstato='%c' where ordprog='%s';", ORDINE_SPEDITO_HOST, szOrdProg);
						PGResUpdate=PGExecSQL(Cfg.nDebugVersion,szSQLCmdUpdate);
						PQclear(PGResUpdate);

					}

					PQclear(PGResColli);


					/*
					* rm 20-01-2004 : aggiunta abilitazione/disabilitazione invio dati a host
					*/
					if(Cfg.nEnableTransmit){
						/*
						* spedizione dell'ordine a Host
						*/
						WriteAsciiRecord(fpOrdini, &tOrdCni,PGResOrdini,0);
					}
				} else {
					bOK=FALSE;
#ifdef TRACE
					trace_out_vstr_date(1,"Errore in selezione ordine [%s]",szOrdProg);
#endif
					gtk_text_printf(&cRED,txt_msgs,"Errore in selezione ordine [%s]\n",szOrdProg);
				}
				PQclear(PGResOrdini);

				/*
				* rm 20-01-2004 : aggiunta abilitazione/disabilitazione invio dati a host
				*/
				if(Cfg.nEnableTransmit){
					fclose(fpOrdini);
					fclose(fpColli);
				}

				if(bOK){
					nRC=0;
					/*
					* rm 20-01-2004 : aggiunta abilitazione/disabilitazione invio dati a host
					*/
					if(Cfg.nEnableTransmit){
						sprintf(szCommand,"%s/cni2sap %s %s > %s/cni2sap.err",Cfg.szPathExe,szOrdiniFile,szColliFile,Cfg.szPathExe);
#ifdef TRACE
						trace_out_vstr_date(1,"ESEGUO %s",szCommand);
#endif
						nRC=system(szCommand);
#ifdef TRACE
						trace_out_vstr_date(1,"ESEGUITO %s",szCommand);
#endif
					}

					if(nRC==0){

						/*
						* cambio lo stato dell'ordine ricevuto (E -> H)
						*/
						sprintf(szSQLCmdUpdate,"update ric_ord set rostato='%c' where ordprog='%s';", ORDINE_SPEDITO_HOST, szOrdProg);
						PGResUpdate=PGExecSQL(FALSE,szSQLCmdUpdate);
						/*
						* cambio lo stato dell'ordine di produzione (E -> H)
						*/
						sprintf(szSQLCmdUpdate,"update ord_prod set opstato='%c' where ordprog='%s';", ORDINE_SPEDITO_HOST, szOrdProg);
						PGResUpdate=PGExecSQL(FALSE,szSQLCmdUpdate);

						/* update colli a video */
						PGResColli=PGExecSQL(Cfg.nDebugVersion,"select ordkey,cpnmcol from col_prod where ordprog='%s' order by ordkey,cpnmcol;",szOrdProg);	
						if((nNumeroColli=PQntuples(PGResColli))){
							for (nIndexColli=0;nIndexColli<nNumeroColli;nIndexColli++){
								/*
								* Aggiorna stato colli a video
								*/
								UpdateCollo(PQgetvalue(PGResColli,nIndexColli,0),atoi(PQgetvalue(PGResColli,nIndexColli,1)));
							}
						}
						PQclear(PGResColli);

						/*
						* rm 20-01-2004 : aggiunta abilitazione/disabilitazione invio dati a host
						*/
						if(Cfg.nEnableTransmit){
							sprintf(szCommand,"mv -f %s %s.old",szOrdiniFile,szOrdiniFile);
							system(szCommand);

							sprintf(szCommand,"mv -f %s %s.old",szColliFile,szColliFile);
							system(szCommand);
						}
						
					} else {
#ifdef TRACE
						trace_out_vstr_date(1,"Errore [%d] in esecuzione comando cni2sap",nRC);
#endif
						gtk_text_printf(&cRED,txt_msgs,"Errore [%d] in esecuzione comando cni2sap\n",nRC);
					}
				}

				gtk_update();
			} else {
#ifdef TRACE
				trace_out_vstr_date(1,"Errore in apertura File di Export Dati (ordine %s)",szOrdProg);
#endif
				gtk_text_printf(&cRED,txt_msgs,"Errore in apertura File di Export Dati (ordine %s)\n",szOrdProg);
			}
		} else {
			/*
			* rm 20-01-2004 : aggiunta abilitazione/disabilitazione invio dati a host
			*/
			if(Cfg.nEnableTransmit){
				/* 
				* se non e' attiva l'opzione di esecuzione del comando cni2sap
				* significa che i dati di evasione vengono accodati ai file in oggetto
				* e gli ordini e i colli vengono settati comunque spediti a host
				*/
				sprintf(szOrdiniFile,"%s/%s",Cfg.szPathExport,StrTrimAll(Cfg.szExpOrdiniFile));
				sprintf(szColliFile,"%s/%s",Cfg.szPathExport,StrTrimAll(Cfg.szExpColliFile));
				sprintf(szOrdiniFileBackup,"%s/%s.backup",Cfg.szPathExport,StrTrimAll(Cfg.szExpOrdiniFile));
				sprintf(szColliFileBackup,"%s/%s.backup",Cfg.szPathExport,StrTrimAll(Cfg.szExpColliFile));

				/* rinomino il file ordini e colli */
				if(FileExists(szOrdiniFile)){
					rename(szOrdiniFile,szOrdiniFileBackup);
					rename(szColliFile,szColliFileBackup);
				}
			}

			/*
			* rm 20-01-2004 : aggiunta abilitazione/disabilitazione invio dati a host
			*/
			if(Cfg.nEnableTransmit){
				if((fpOrdini=fopen(szOrdiniFileBackup,"a"))==NULL){
#ifdef TRACE
					trace_out_vstr_date(1,"EvasioneOrdine() : Errore in apertura file ordini [%s] [%s]",szOrdiniFileBackup,strerror(errno));
#endif
					gtk_text_printf(&cCYAN,txt_msgs,"EvasioneOrdine() : Errore in apertura file ordini [%s]\n",szOrdiniFileBackup);
					bRetVal=FALSE;
				}
				if(bRetVal && (fpColli =fopen(szColliFileBackup,"a"))==NULL){
#ifdef TRACE
					trace_out_vstr_date(1,"EvasioneOrdine() : Errore in apertura file colli [%s] [%s]",szColliFileBackup,strerror(errno));
#endif
					gtk_text_printf(&cCYAN,txt_msgs,"EvasioneOrdine() : Errore in apertura file colli [%s]\n",szColliFileBackup);
					bRetVal=FALSE;
				}
			}

			if(bRetVal){

				PGResOrdini=PGExecSQL(Cfg.nDebugVersion,"select ordprog,ronmcll,ropsrea,ronelab from ric_ord where ordprog='%s';",szOrdProg);	
				if(PQntuples(PGResOrdini)){

					/* selezioni i colli relativi all'ordine */
					PGResColli=PGExecSQL(Cfg.nDebugVersion,"select ordprog,cptpfor,cpprgcl from col_prod where ordprog='%s' order by ordkey,cpnmcol;",szOrdProg);	

					if ((nNumeroColli=PQntuples(PGResColli))){
						for (nIndexColli=0;nIndexColli<nNumeroColli;nIndexColli++){
							/*
							* rm 20-01-2004 : aggiunta abilitazione/disabilitazione invio dati a host
							*/
							if(Cfg.nEnableTransmit){
								/*
								* spedizione a Host dei colli relativi all'ordine
								*/
								WriteAsciiRecord(fpColli, &tColCni,PGResColli,nIndexColli);
							}
							nColliSpediti++;
						}

						/*
						* cambio lo stato dei colli relativi all'ordine(E -> H)
						*/
						sprintf(szSQLCmdUpdate,"update col_prod set cpstato='%c' where ordprog='%s';", COLLO_SPEDITO_HOST, szOrdProg);
						PGResUpdate=PGExecSQL(Cfg.nDebugVersion,szSQLCmdUpdate);
						PQclear(PGResUpdate);

						/*
						* cambio lo stato delle righe relative all'ordine(E -> H)
						*/
						sprintf(szSQLCmdUpdate,"update rig_prod set rpstato='%c' where ordprog='%s';", ORDINE_SPEDITO_HOST, szOrdProg);
						PGResUpdate=PGExecSQL(Cfg.nDebugVersion,szSQLCmdUpdate);
						PQclear(PGResUpdate);

					}

					PQclear(PGResColli);


					/*
					* rm 20-01-2004 : aggiunta abilitazione/disabilitazione invio dati a host
					*/
					if(Cfg.nEnableTransmit){
						/*
						* spedizione dell'ordine a Host
						*/
						WriteAsciiRecord(fpOrdini, &tOrdCni,PGResOrdini,0);
					}
				} else {
					bOK=FALSE;
#ifdef TRACE
					trace_out_vstr_date(1,"Errore in selezione ordine [%s]",szOrdProg);
#endif
					gtk_text_printf(&cRED,txt_msgs,"Errore in selezione ordine [%s]\n",szOrdProg);
				}
				PQclear(PGResOrdini);

				/*
				* rm 20-01-2004 : aggiunta abilitazione/disabilitazione invio dati a host
				*/
				if(Cfg.nEnableTransmit){
					fclose(fpOrdini);
					fclose(fpColli);
				}

				if(bOK){

					/*
					* cambio lo stato dell'ordine ricevuto (E -> H)
					*/
					sprintf(szSQLCmdUpdate,"update ric_ord set rostato='%c' where ordprog='%s';", ORDINE_SPEDITO_HOST, szOrdProg);
					PGResUpdate=PGExecSQL(Cfg.nDebugVersion,szSQLCmdUpdate);
					/*
					* cambio lo stato dell'ordine di produzione (E -> H)
					*/
					sprintf(szSQLCmdUpdate,"update ord_prod set opstato='%c' where ordprog='%s';", ORDINE_SPEDITO_HOST, szOrdProg);
					PGResUpdate=PGExecSQL(Cfg.nDebugVersion,szSQLCmdUpdate);

					/* update colli a video */
					PGResColli=PGExecSQL(Cfg.nDebugVersion,"select ordkey,cpnmcol from col_prod where ordprog='%s' order by ordkey,cpnmcol;",szOrdProg);	
					if((nNumeroColli=PQntuples(PGResColli))){
						for (nIndexColli=0;nIndexColli<nNumeroColli;nIndexColli++){
							/*
							* Aggiorna stato colli a video
							*/
							UpdateCollo(PQgetvalue(PGResColli,nIndexColli,0),atoi(PQgetvalue(PGResColli,nIndexColli,1)));
						}
					}
					PQclear(PGResColli);

					/*
					* rm 20-01-2004 : aggiunta abilitazione/disabilitazione invio dati a host
					*/
					if(Cfg.nEnableTransmit){
						/* ri-rinomino il file ordini e colli */
						if(FileExists(szOrdiniFileBackup)){
							rename(szOrdiniFileBackup,szOrdiniFile);
							rename(szColliFileBackup,szColliFile);
						}
					}
				}

				gtk_update();
			}
		}
	}
#ifdef TRACE
	trace_out_vstr_date(1,"-- CheckOrdineEvaso(%s) - Fine",szOrdProg);
#endif
	return bRetVal;
}


/*
* int CalcPesoRealeOrdine(char *szOrdProg)
* calcola il peso reale dell'ordine szOrdProg sommando i pesi dei suoi
* sottordini; inserisce il valore ottenuto nel DataBase assieme alla
* data e ora di evasione.
* ritorna: il peso reale dell'ordine
*/
int CalcPesoRealeOrdine(char *szOrdProg)
{
	PGresult *PGRes;
	PGresult *PGResUpdate;
	int nSottordini;
	int nIndex;
	int nPesoRealeOrdine=0;
	char szTimeBuffer[128];
	char szDateBuffer[128];

	PGRes=PGExecSQL(TRUE,"select oppsrea from ord_prod where ordprog='%s';",szOrdProg);
	if (PQresultStatus(PGRes) == PGRES_TUPLES_OK){

		nSottordini=PQntuples(PGRes);

		for(nIndex=0;nIndex<nSottordini;nIndex++){
			nPesoRealeOrdine+=(atoi(PQgetvalue(PGRes,nIndex,0)));
		}

//		GetTimeHHMMSS(time((long *)0),szTimeBuffer);
//		GetDateYYYYMMDD(time((long *)0),szDateBuffer);
		GetTimeHHMMSSFromDB(szTimeBuffer);
		GetDateYYYYMMDDFromDB(szDateBuffer);

		PGResUpdate=PGExecSQL(TRUE,"update ric_ord set ropsrea=%d,rodteva='%s',rotmeva='%s' where ordprog='%s';",nPesoRealeOrdine,szDateBuffer,szTimeBuffer,szOrdProg);
		if (!PGResUpdate || PQresultStatus(PGResUpdate) != PGRES_COMMAND_OK ) {
			gtk_text_printf(NULL,txt_msgs,"errore in settaggio peso [ordine %s] : %7.3f Kg\n",szOrdProg,(float)((float)nPesoRealeOrdine/(float)1000));
#ifdef TRACE
			trace_out_vstr_date(1,"fallito settaggio peso [ordine %s] : %7.3f Kg\n",szOrdProg,(float)((float)nPesoRealeOrdine/(float)1000));
#endif
		}
		PQclear(PGResUpdate);
	} else {
		gtk_text_printf(NULL,txt_msgs,"Errore in select ordini in su col_prod [%s]\n",szOrdProg);
#ifdef TRACE
		trace_out_vstr_date(1,"CalcPesoRealeOrdine(%s) - Fallita select su col_prod",szOrdProg);
#endif
	}	
	PQclear(PGRes);

	return nPesoRealeOrdine;
}


/*
* void SettaPesoSottordine(char *szOrdKey)
* inserisce nel DataBase il peso reale del sottordine szOrdKey,
* la data e l'ora di evasione.
*/
BOOL SettaPesoSottordine(char *szOrdKey, int nPesoOrdine)
{
	PGresult *PGRes;
	char szTimeBuffer[128];
	char szDateBuffer[128];
	BOOL bRetVal=TRUE;
	
//	GetTimeHHMMSS(time((long *)0),szTimeBuffer);
//	GetDateYYYYMMDD(time((long *)0),szDateBuffer);
	GetTimeHHMMSSFromDB(szTimeBuffer);
	GetDateYYYYMMDDFromDB(szDateBuffer);

	PGRes=PGExecSQL(TRUE,"update ord_prod set oppsrea=%d,opdteva='%s',optmeva='%s' where ordkey='%s';",nPesoOrdine,szDateBuffer,szTimeBuffer,szOrdKey);
	if (!PGRes|| PQresultStatus(PGRes) != PGRES_COMMAND_OK ) {
		bRetVal=FALSE;
		gtk_text_printf(NULL,txt_msgs,"errore in settaggio peso [sottordine %s] : %7.3f Kg\n",szOrdKey,(float)((float)nPesoOrdine/(float)1000));
#ifdef TRACE
		trace_out_vstr_date(1,"fallito settaggio peso [sottordine %s] : %7.3f Kg\n",szOrdKey,(float)((float)nPesoOrdine/(float)1000));
#endif
	}
	PQclear(PGRes);

	return bRetVal;
}


/*
* void EditConfigurazione(void)
* visualizza i dati di configurazione
*/
void EditConfigurazione(void)
{
	GtkWidget *sb_tolleranza_assoluta;
	GtkWidget *sb_tolleranza_percentuale;
	GtkWidget *sb_soglia_massima;
	GtkWidget *sb_soglia_minima;
	char szMainTimeout[64]; 
	char szBilanciaTimeout[64]; 
	char szDeviceBarcode[64];
	char szParametriBarcode[64];
	char szDeviceBarcodePz[64];
	char szParametriBarcodePz[64];
	char szDeviceBilancia[64];
	char szParametriBilancia[64];
	char szPlcService[64];

	dlg_config = create_dlg_config();

	/*
	* GENERALE
	*/ 
	sprintf(szMainTimeout,"%d",Cfg.nPesoDelay);
	sprintf(szBilanciaTimeout,"%d",Cfg.nBilanciaTimeout);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(get_widget(dlg_config,"sb_timeout_generale")),Cfg.nPesoDelay);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(get_widget(dlg_config,"sb_timeout_bilancia")),Cfg.nBilanciaTimeout);

	/*
	* GESIONE PESATURA - TOLLERANZA PESO
	*/
  if (Cfg.GestionePesatura==PESATURA_ORDINE){
		gtk_widget_activate(get_widget(dlg_config,"rb_pesatura_ordine"));
	} else if (Cfg.GestionePesatura==PESATURA_COLLO){
		gtk_widget_activate(get_widget(dlg_config,"rb_pesatura_collo"));
	}
	
	sb_tolleranza_assoluta=get_widget(dlg_config,"sb_tolleranza_assoluta");
	sb_tolleranza_percentuale=get_widget(dlg_config,"sb_tolleranza_percentuale");
	sb_soglia_massima=get_widget(dlg_config,"sb_soglia_massima");
	sb_soglia_minima=get_widget(dlg_config,"sb_soglia_minima");
	
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(sb_tolleranza_assoluta),Cfg.nTolleranzaAss);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(sb_tolleranza_percentuale),Cfg.nTolleranzaPerc);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(sb_soglia_massima),Cfg.nSogliaMax);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(sb_soglia_minima),Cfg.nSogliaMin);

	if (Cfg.Tolleranza==TOLLERANZA_ASSOLUTA){
		gtk_widget_activate(get_widget(dlg_config,"rb_tolleranza_assoluta"));
	} else if (Cfg.Tolleranza==TOLLERANZA_PERCENTUALE){
		gtk_widget_activate(get_widget(dlg_config,"rb_tolleranza_percentuale"));
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(get_widget(dlg_config,"cb_soglia_massima")),FALSE);
		gtk_widget_set_sensitive(get_widget(dlg_config,"sb_soglia_massima"),FALSE);
	} else if (Cfg.Tolleranza==TOLLERANZA_PERCENTUALE_SOGLIA_MASSIMA){
		gtk_widget_activate(get_widget(dlg_config,"rb_tolleranza_percentuale"));
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(get_widget(dlg_config,"cb_soglia_massima")),TRUE);
		gtk_widget_set_sensitive(get_widget(dlg_config,"sb_soglia_massima"),TRUE);
	}
	
	gtk_widget_set_sensitive(get_widget(dlg_config,"sb_tolleranza_assoluta"), GTK_TOGGLE_BUTTON(get_widget(dlg_config,"rb_tolleranza_assoluta"))->active?TRUE:FALSE);
	gtk_widget_set_sensitive(get_widget(dlg_config,"sb_tolleranza_percentuale"), GTK_TOGGLE_BUTTON(get_widget(dlg_config,"rb_tolleranza_percentuale"))->active?TRUE:FALSE);

	/*
	* Esecuzione Automatica dell'utility cni2sap
	*/
	gtk_widget_set_sensitive(get_widget(dlg_config,"cb_cni2sap"),Cfg.nSAP?TRUE:FALSE);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(get_widget(dlg_config,"cb_cni2sap")),Cfg.nExecuteCni2Sap?TRUE:FALSE);

	
	/*
	* BARCODE
	*/
	xncGetFileString(ProcList[PROC_BARCODE].szProcName,"CommDevice", "/dev/ttyS1", szDeviceBarcode,    80 ,Cfg.szCniCfg,NULL); 
	xncGetFileString(ProcList[PROC_BARCODE].szProcName,"CommParams", "9600,n,8,1", szParametriBarcode, 80 ,Cfg.szCniCfg,NULL); 
	gtk_entry_set_text (GTK_ENTRY (get_widget(dlg_config,"entry_device_barcode")), szDeviceBarcode);
	gtk_entry_set_text (GTK_ENTRY (get_widget(dlg_config,"entry_parametri_barcode")), szParametriBarcode);

	/*
	* BARCODE PLZ
	*/
	xncGetFileString(ProcList[PROC_BARCODE_PZ].szProcName,"CommDevice", "/dev/ttyS1", szDeviceBarcodePz,    80 ,Cfg.szCniCfg,NULL); 
	xncGetFileString(ProcList[PROC_BARCODE_PZ].szProcName,"CommParams", "9600,n,8,1", szParametriBarcode, 80 ,Cfg.szCniCfg,NULL); 
	gtk_entry_set_text (GTK_ENTRY (get_widget(dlg_config,"entry_device_barcode_pz")), szDeviceBarcodePz);
	gtk_entry_set_text (GTK_ENTRY (get_widget(dlg_config,"entry_parametri_barcode_pz")), szParametriBarcodePz);

	/*
	* BILANCIA
	*/
	xncGetFileString(ProcList[PROC_BILANCIA].szProcName,"CommDevice", "/dev/ttyS0", szDeviceBilancia,    80 ,Cfg.szCniCfg,NULL); 
	xncGetFileString(ProcList[PROC_BILANCIA].szProcName,"CommParams", "9600,n,8,1", szParametriBilancia, 80 ,Cfg.szCniCfg,NULL); 
	gtk_entry_set_text (GTK_ENTRY (get_widget(dlg_config,"entry_device_bilancia")), szDeviceBilancia);
	gtk_entry_set_text (GTK_ENTRY (get_widget(dlg_config,"entry_parametri_bilancia")), szParametriBilancia);
	
	/*
	* STAMPANTI
	*/
	gtk_entry_set_text (GTK_ENTRY (get_widget(dlg_config,"entry_printer_name")), Cfg.szPrinterName);
	gtk_entry_set_text (GTK_ENTRY (get_widget(dlg_config,"entry_label_printer_name")), Cfg.szLabelPrinterName);

	/*
	* PLC
	*/
	UpdateVarList(get_widget(dlg_config,"lst_plc_var"),TRUE);
	sprintf(szPlcService,"%d",Cfg.nPLCService);
	gtk_entry_set_text (GTK_ENTRY (get_widget(dlg_config,"entry_plc_host_name")), Cfg.szPLCHost);
	gtk_entry_set_text (GTK_ENTRY (get_widget(dlg_config,"entry_plc_service")), szPlcService);

	gtk_window_set_focus (GTK_WINDOW (dlg_config), get_widget(dlg_config,"pb_ok"));
	gtk_widget_show(dlg_config);
	gtk_window_set_transient_for(GTK_WINDOW(dlg_config),GTK_WINDOW(main_window));

}



/*
* void ApplyConfigurazione(void)
* visualizza i dati di configurazione
*/
void ApplyConfigurazione(void)
{
	char szDeviceBarcode[64];
	char szParametriBarcode[64];
	char szDeviceBarcodePz[64];
	char szParametriBarcodePz[64];
	char szDeviceBilancia[64];
	char szParametriBilancia[64];
	
	/*
	* memorizzo le informazioni relative ai TIMEOUT
	*/ 
	Cfg.nPesoDelay       = gtk_spin_button_get_value_as_int((GtkSpinButton *)get_widget(dlg_config,"sb_timeout_generale"));
	Cfg.nBilanciaTimeout = gtk_spin_button_get_value_as_int((GtkSpinButton *)get_widget(dlg_config,"sb_timeout_bilancia"));
	xncPutFileInt(ProcList[PROC_PESO].szProcName, "Delay",           Cfg.nPesoDelay,Cfg.szCniCfg,NULL);
	xncPutFileInt(ProcList[PROC_PESO].szProcName, "BilanciaTimeout", Cfg.nBilanciaTimeout,Cfg.szCniCfg,NULL);

	/*
	* memorizzo le informazioni relative alla TOLLERANZA PESO
	*/
  if (GTK_TOGGLE_BUTTON(get_widget(dlg_config,"rb_pesatura_ordine"))->active){
		Cfg.GestionePesatura=PESATURA_ORDINE;
	} else if (GTK_TOGGLE_BUTTON(get_widget(dlg_config,"rb_pesatura_collo"))->active){
		Cfg.GestionePesatura=PESATURA_COLLO;
	}
	xncPutFileInt(ProcList[PROC_PESO].szProcName, "GestionePesatura",Cfg.GestionePesatura,Cfg.szCniCfg,NULL);

	Cfg.nTolleranzaAss  = gtk_spin_button_get_value_as_int((GtkSpinButton *)get_widget(dlg_config,"sb_tolleranza_assoluta"));
	Cfg.nTolleranzaPerc = gtk_spin_button_get_value_as_int((GtkSpinButton *)get_widget(dlg_config,"sb_tolleranza_percentuale"));
	Cfg.nSogliaMax      = gtk_spin_button_get_value_as_int((GtkSpinButton *)get_widget(dlg_config,"sb_soglia_massima"));
	Cfg.nSogliaMin      = gtk_spin_button_get_value_as_int((GtkSpinButton *)get_widget(dlg_config,"sb_soglia_minima"));
	if (GTK_TOGGLE_BUTTON (get_widget(dlg_config,"rb_tolleranza_assoluta"))->active){
		Cfg.Tolleranza = TOLLERANZA_ASSOLUTA;
	} else if (GTK_TOGGLE_BUTTON (get_widget(dlg_config,"cb_soglia_massima"))->active){
		Cfg.Tolleranza = TOLLERANZA_PERCENTUALE_SOGLIA_MASSIMA;
	} else {
		Cfg.Tolleranza = TOLLERANZA_PERCENTUALE;
	}
	xncPutFileInt(ProcList[PROC_PESO].szProcName, "TolleranzaPercentuale",Cfg.nTolleranzaPerc,Cfg.szCniCfg,NULL);
	xncPutFileInt(ProcList[PROC_PESO].szProcName, "TolleranzaAssoluta",   Cfg.nTolleranzaAss,Cfg.szCniCfg,NULL);
	xncPutFileInt(ProcList[PROC_PESO].szProcName, "Tolleranza",           Cfg.Tolleranza,Cfg.szCniCfg,NULL);
	xncPutFileInt(ProcList[PROC_PESO].szProcName, "SogliaMassima",        Cfg.nSogliaMax,Cfg.szCniCfg,NULL);
	xncPutFileInt(ProcList[PROC_PESO].szProcName, "SogliaMinima",         Cfg.nSogliaMin,Cfg.szCniCfg,NULL);

	/*
	* Esecuzione Automatica dell'utility cni2sap
	*/
	if(Cfg.nSAP){
		if (GTK_TOGGLE_BUTTON (get_widget(dlg_config,"cb_cni2sap"))->active){
			Cfg.nExecuteCni2Sap=1;
		} else {
			Cfg.nExecuteCni2Sap=0;
		}
	}
	xncPutFileInt(ProcList[PROC_PESO].szProcName, "EseguiCni2Sap",        Cfg.nExecuteCni2Sap,Cfg.szCniCfg,NULL);

	/*
	* barcode
	*/
	sprintf(szDeviceBarcode, gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_device_barcode"))));
	sprintf(szParametriBarcode, gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_parametri_barcode"))));
	xncPutFileString(ProcList[PROC_BARCODE].szProcName,"CommDevice", szDeviceBarcode,Cfg.szCniCfg,NULL); 
	xncPutFileString(ProcList[PROC_BARCODE].szProcName,"CommParams", szParametriBarcode,Cfg.szCniCfg,NULL); 

	/*
	* barcode plz
	*/
	sprintf(szDeviceBarcodePz, gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_device_barcode_pz"))));
	sprintf(szParametriBarcodePz, gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_parametri_barcode_pz"))));
	// xncPutFileString(ProcList[PROC_BARCODE_PZ].szProcName,"CommDevice", szDeviceBarcodePz,Cfg.szCniCfg,NULL); 
	// xncPutFileString(ProcList[PROC_BARCODE_PZ].szProcName,"CommParams", szParametriBarcodePz,Cfg.szCniCfg,NULL); 

	/*
	* bilancia
	*/
	sprintf(szDeviceBilancia, gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_device_bilancia"))));
	sprintf(szParametriBilancia, gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_parametri_bilancia"))));
	xncPutFileString(ProcList[PROC_BILANCIA].szProcName,"CommDevice",szDeviceBilancia,Cfg.szCniCfg,NULL); 
	xncPutFileString(ProcList[PROC_BILANCIA].szProcName,"CommParams",szParametriBilancia,Cfg.szCniCfg,NULL); 

	/*
	* stampanti
	*/
	sprintf(Cfg.szPrinterName, gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_printer_name"))));
	sprintf(Cfg.szLabelPrinterName, gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_label_printer_name"))));
	xncPutFileString(ProcList[PROC_BILANCIA].szProcName,"PrinterName",Cfg.szPrinterName,Cfg.szCniCfg,NULL); 
	xncPutFileString(ProcList[PROC_BILANCIA].szProcName,"LabelPrinterName",Cfg.szLabelPrinterName,Cfg.szCniCfg,NULL); 

	/*
	* PLC
	*/
	Cfg.nPLCService=atoi(gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_plc_service"))));
	sprintf(Cfg.szPLCHost,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_plc_host_name"))));
	xncPutFileString(ProcList[PROC_BILANCIA].szProcName,"PLCHost",Cfg.szPLCHost,Cfg.szCniCfg,NULL); 
	xncPutFileInt(ProcList[PROC_PESO].szProcName, "PLCService",   Cfg.nPLCService,Cfg.szCniCfg,NULL);
}

void ShowPLCMsg(char *pszMsg)
{
	if(dlg_plc_msg==(GtkWidget *)0){
		dlg_plc_msg=create_dlg_plc_msg();
	}

	gtk_label_set_text (GTK_LABEL (get_widget(dlg_plc_msg,"lb_msg")),pszMsg);
	gtk_widget_show(dlg_plc_msg);
	gtk_window_set_transient_for(GTK_WINDOW(dlg_plc_msg),GTK_WINDOW(main_window));
	gtk_window_set_focus (GTK_WINDOW (dlg_plc_msg), get_widget(dlg_plc_msg,"pb_ok"));
}

void ShowColloScartato(char *pszOrdProg)
{
	dlg_scarto=create_dlg_scarto();

	gtk_label_set_text (GTK_LABEL (get_widget(dlg_scarto,"lb_ordine")),pszOrdProg);
	gtk_widget_show(dlg_scarto);
	gtk_window_set_transient_for(GTK_WINDOW(dlg_scarto),GTK_WINDOW(main_window));
	gtk_window_set_focus (GTK_WINDOW (dlg_scarto), get_widget(dlg_scarto,"pb_ok"));
}


void ShowDlgRequest(int nPeso,char *szBarcode,char *szMsg)
{
	char szBuffer[80];

	/*
	* PEO - Wed Oct 23 16:38:13 CEST 2002
	* pezza per velocizzare l'inserimento manuale del barcode
	* in occasione della rottura dello scanner ... evviva !!!
	*/
	if(strlen(szBarcodeColloBilancia)){
		gtk_entry_set_text (GTK_ENTRY (get_widget(dlg_barcode,"txf_barcode")),szBarcode);
	} else {
		gtk_entry_set_text (GTK_ENTRY (get_widget(dlg_barcode,"txf_barcode")),"24151");
	}
	sprintf(szBuffer,"%d",nPeso);
	gtk_entry_set_text (GTK_ENTRY (get_widget(dlg_barcode,"txf_peso")), szBuffer);
	gtk_label_set_text (GTK_LABEL (get_widget(dlg_barcode,"lb_barcode_msg")),szMsg);
	gtk_widget_show(dlg_barcode);
	gtk_window_set_transient_for(GTK_WINDOW(dlg_barcode),GTK_WINDOW(main_window));
	gtk_window_set_focus (GTK_WINDOW (dlg_barcode), get_widget(dlg_barcode,"txf_barcode"));
}


BOOL SendSockCmd(int nServicePort,char *szHostName, int nCmd,char *szParams)
{
	int nFd;
	char szMessage[256];

	if(nCmd<NUM_SOCK_CMDS){
		if(xncScktOpen("", "tcp", nServicePort, szHostName, &nFd)){
			strcpy(szMessage,pszSockCmds[nCmd]);
			if(szParams && strlen(szParams)){
				strcat(szMessage,",");
				strcat(szMessage,szParams);
			}
			xncScktSend(&nFd, szMessage,strlen(szMessage));
			xncScktClose(&nFd);
			return TRUE;
		}
	}
	return FALSE;
}


/*
* void do_aggiungi_collo(char *szOrdine, int nNumeroColli, char *szFormatoCollo);
*
* Aggiunge negli archivi di produzione nNumColli colli appartenenti all'ordine OrdKey
* di formato szFormatoCollo; il peso reale e predeterminato vengono settati a 0.
* I campi Righe e Copie vengono inseriti con valore nullo.
* I colli aggiunti vengono evasi automaticamente.
*/
void do_aggiungi_collo(char *szOrdKey, int nNumColli, char *szFormatoCollo)
{
	PGresult *PGResOrdine;
	GtkWidget *lb_msg;

	/*
	* controllo parametri passati in ingresso alla funzione
	*/
	PGResOrdine=PGExecSQL(Cfg.nDebugVersion,"select opnmcla,ordprog,opnelab from ord_prod where ordkey ~* '%s';",szOrdKey);
	if ((nNumColli<=0) || (PQntuples(PGResOrdine)!=1) || (!strcmp(szOrdKey,"\0"))){
			
		dlg_message=create_dlg_message();
		dlg_aggiungi_collo=create_dlg_aggiungi_collo();
		lb_msg=get_widget(dlg_message,"lb_msg");
		gtk_label_printf(lb_msg,"IMPOSSIBILE EFFETTUARE L'INSERIMENTO;\nControllare l'esattezza del codice ordine\ne del numero colli");
		gtk_window_set_title (GTK_WINDOW (dlg_message), "Aggiunta colli in fine linea");
		gtk_widget_show(dlg_message);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(dlg_aggiungi_collo));
		gtk_signal_connect (GTK_OBJECT (get_widget(dlg_message,"pb_ok")), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_pb_ok_clicked), dlg_message);
		PQclear(PGResOrdine);
	} else { 
		/*
		* aggiungo i colli negli archivi col_prod, ord_prod, ric_ord
		* i colli (di peso nullo) vengono evasi automaticamente
		*/
		PGresult *PGRes;
		int nCollo;
		int nIndex;
		char szBuffer[512];
		
		PGRes=PGExecSQL(Cfg.nDebugVersion,"update ord_prod set opnmcla=opnmcla+%d,opnmcll=opnmcll+%d where ordkey='%s';",nNumColli,nNumColli,szOrdKey);
		PQclear(PGRes);
		if(Cfg.nSAP){
			PGRes=PGExecSQL(Cfg.nDebugVersion,"update ric_ord set ronmcll=ronmcll+%d where ordprog='%s';",nNumColli,SubStr(szOrdKey,0,10));
		} else {
			PGRes=PGExecSQL(Cfg.nDebugVersion,"update ric_ord set ronmcll=ronmcll+%d where ordprog='%s';",nNumColli,SubStr(szOrdKey,0,6));
		}
		PQclear(PGRes);
		/*
		* seleziono il numero dei colli dell'ordine, per ricavare il cpnmcol da assegnare al collo che inserisco
		*/
		PGRes=PGExecSQL(Cfg.nDebugVersion,"select max(cpnmcol) from col_prod where ordkey='%s';",szOrdKey);
		if (PQntuples(PGRes)){
			nCollo=atoi(PQgetvalue(PGRes,0,0));
		}
		PQclear(PGRes);
		
		for(nIndex=0;nIndex<nNumColli;nIndex++){
			char szBarcode[64]="";

			nCollo++;

			strcpy(szBarcode,ColloBarcode(szOrdKey,nCollo));
				
			PGRes=PGExecSQL(Cfg.nDebugVersion,"insert into col_prod (ordprog,ordkey,cpnelab,cpnmcol,cptpfor,cppspre,cppsrea,cpnmrgh,cpnmcpe,cpswlin,cpbrcde) values ('%s','%s','%s',%d,'%s',0,0,0,0,'0','%s');",PQgetvalue(PGResOrdine,0,1),szOrdKey,PQgetvalue(PGResOrdine,0,2),nCollo,StrTrimRight(szFormatoCollo),szBarcode);
#ifdef TRACE
			trace_out_vstr_date(1,"Aggiunto collo %d nell'ordine %s [formato %s]",nCollo,szOrdKey,szFormatoCollo);
#endif
			/* evado il collo */
			EvadiCollo(szOrdKey,nCollo,FALSE);

			/*
			* update della lista colli o aggiunge se nuovo collo
			*/
			UpdateCollo(szOrdKey,nCollo);

			gtk_clist_unselect_all(GTK_CLIST(lst_colli));
			
			sprintf(szBuffer,"%s,%d,%s", szOrdKey, nCollo, Cfg.szPrinterName);
			SendMessage(PROC_PRINTLABEL,PROC_PESO,PRINT_RAC_COLLO_ADD,szBuffer);
			PQclear(PGRes);
		}
		/* chiudo la finestra */
		gtk_widget_destroy(GTK_WIDGET(dlg_aggiungi_collo));
	}
}


/*
* void do_elimina_collo(char *szOrdKey, char *szSegnacollo)
* Elimina dall'ordine di produzione szOrdKey il collo avente segnacollo=szSegnacollo
*/
void do_elimina_collo(char *szOrdKey, char *szSegnacollo)
{
	PGresult *PGResOrdine;
	GtkWidget *lb_msg;

	/*
	* controllo parametri passati in ingresso
	*/
	PGResOrdine=PGExecSQL(Cfg.nDebugVersion,"select cpnmcol from col_prod where ordkey='%s' and cpprgcl='%s';",szOrdKey,szSegnacollo);
	if (!(strcmp(szOrdKey,"\0")) || !(strcmp(szSegnacollo,"\0")) || (PQntuples(PGResOrdine)==0)){
		dlg_message=create_dlg_message();
		lb_msg=get_widget(dlg_message,"lb_msg");
		gtk_label_printf(lb_msg,"IMPOSSIBILE EFFETTUARE L'ELIMINAZIONE;\nControllare l'esattezza del codice ordine e del segnacollo");
		gtk_window_set_title (GTK_WINDOW (dlg_message), "Eliminazione colli in fine linea");
		gtk_widget_show(dlg_message);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(dlg_elimina_collo));
		gtk_signal_connect (GTK_OBJECT (get_widget(dlg_message,"pb_ok")), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_pb_ok_clicked), dlg_message);
	} else {
		/*
		* elimino il collo dall'ordine 
		*/
		PGresult *PGRes;
		int nRigaListaColli=0;
		BOOL bTrovato=FALSE;
		char *pszColloKey;
		char *pszNumCollo;
		int nCollo;

		nCollo=atoi(PQgetvalue(PGResOrdine,0,0));
		PGRes=PGExecSQL(Cfg.nDebugVersion,"select opnmcla from ord_prod where ordkey='%s';",szOrdKey);

		/*
		* Verifico che il sottordine abbia piu' di un collo
		*/
		if (atoi(PQgetvalue(PGRes,0,0))>1){
			PQclear(PGRes);

			PGRes=PGExecSQL(Cfg.nDebugVersion,"update ord_prod set opnmcll=opnmcll-1, opnmcla=opnmcla-1 where ordkey='%s';",szOrdKey);
			PQclear(PGRes);
			if(Cfg.nSAP){
				PGRes=PGExecSQL(Cfg.nDebugVersion,"update ric_ord set ronmcll=ronmcll-1 where ordprog='%s';",SubStr(szOrdKey,0,10));
			} else {
				PGRes=PGExecSQL(Cfg.nDebugVersion,"update ric_ord set ronmcll=ronmcll-1 where ordprog='%s';",SubStr(szOrdKey,0,6));
			}
			PQclear(PGRes);

			/*
			* eliminazione collo da archivio colli di produzione
			*/
			PGRes=PGExecSQL(Cfg.nDebugVersion,"delete from col_prod where ordkey='%s' and cpprgcl='%s';",szOrdKey,szSegnacollo);
			PQclear(PGRes);
			
			/*
			* update colore a video (BLUE) del collo eliminato
			*/
			while(!bTrovato && nRigaListaColli<(GTK_CLIST(lst_colli)->rows)){
					
				gtk_clist_get_text(GTK_CLIST(lst_colli),nRigaListaColli,Cfg.nColloKeyField,&pszColloKey);
				gtk_clist_get_text(GTK_CLIST(lst_colli),nRigaListaColli,Cfg.nColloNumField,&pszNumCollo);

				if (!(strcmp(pszColloKey,szOrdKey)) && (nCollo==atoi(pszNumCollo))){
					gtk_clist_set_background(GTK_CLIST(lst_colli),nRigaListaColli,&cBLUE);
					gtk_clist_set_foreground(GTK_CLIST(lst_colli),nRigaListaColli,&cWHITE);
					bTrovato=TRUE;
					gtk_clist_unselect_all(GTK_CLIST(lst_colli));
				}
				nRigaListaColli++;
			}

			gtk_text_printf(&cBLUE,txt_msgs,"Eliminato collo %s nell'ordine %s",szSegnacollo,szOrdKey);
#ifdef TRACE
			trace_out_vstr_date(1,"Eliminato collo %s nell'ordine %s",szSegnacollo,szOrdKey);
#endif
			/* chiudo la finestra */
			gtk_widget_destroy(GTK_WIDGET(dlg_elimina_collo));
		} else {
			PQclear(PGRes);
			
			dlg_message=create_dlg_message();
			lb_msg=get_widget(dlg_message,"lb_msg");
			gtk_label_printf(lb_msg,"IMPOSSIBILE EFFETTUARE LA CANCELLAZIONE\nL'ordine e' formato da un solo collo");
			gtk_window_set_title (GTK_WINDOW (dlg_message), "Eliminazione colli in fine linea");
			gtk_widget_show(dlg_message);
			gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(dlg_elimina_collo));
			gtk_signal_connect (GTK_OBJECT (get_widget(dlg_message,"pb_ok")), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_pb_ok_clicked), dlg_message);
		}
	}
	PQclear(PGResOrdine);
}

void ShowColloEvaso(char *szOrdProg,int nCollo)
{
	PGresult *PGRes;
	char szBuffer[128];
	char szData[40];
	char szOra[40];

	dlg_message=create_dlg_message();
	szData[0]='\0';
	szOra[0]='\0';

	PGRes=PGExecSQL(Cfg.nDebugVersion,"select cpdteva,cptmeva from col_prod where ordprog='%s' and cpnmcol=%d;",szOrdProg,nCollo);
	if (PQresultStatus(PGRes)==PGRES_TUPLES_OK && (PQntuples(PGRes)==1)){
		strcpy(szData,GetDateFromYYYYMMDD(PQgetvalue(PGRes,0,0),szBuffer));
		strcpy(szOra,GetTimeFromHHMMSS(PQgetvalue(PGRes,0,1),szBuffer));
	}
	PQclear(PGRes);

#ifdef TRACE
			trace_out_vstr_date(1,"Collo %s - %d gia' evaso in data %s - %s",szOrdProg,nCollo,szData,szOra);
#endif

	gtk_label_printf(get_widget(dlg_message,"lb_msg"),"Attenzione: il collo %s - %d\n e' gia' stato evaso in data %s - %s",szOrdProg,nCollo,szData,szOra);
	gtk_window_set_title (GTK_WINDOW (dlg_message), "Collo gia' Evaso");
	gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));
	gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
	gtk_signal_connect (GTK_OBJECT (get_widget(dlg_message,"pb_ok")), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_pb_ok_clicked), dlg_message);
	gtk_widget_show(dlg_message);
}


/*
* Creazione del Barcode collo
* 20 caratteri 
*/
char *ColloBarcode(char *pszOrdKey,int nCollo)
{
	static char szBuffer[80];
	char szNumCollo[30];

	sprintf(szNumCollo,"%03d",nCollo);

	if(Cfg.nSAP){
		strcpy(szBuffer,SubStr(pszOrdKey,0,10));
		strcat(szBuffer,SubStr(pszOrdKey,11,1));          /* Isola        1 caratteri */
		strcat(szBuffer,SubStr(pszOrdKey,13,2));          /* SottoOrdine  2 caratteri */
	} else {
		strcpy(szBuffer,SubStr(pszOrdKey,0,6));
		strcat(szBuffer,SubStr(pszOrdKey,7,1));           /* Isola        1 caratteri */
		strcat(szBuffer,SubStr(pszOrdKey,9,2));           /* SottoOrdine  2 caratteri */
	}
	strcat(szBuffer,szNumCollo);                        /* Numero Collo 3 caratteri */

	return (szBuffer);
}

/*
* BOOL ColloPericoloso(char *szOrdKey,int nCollo)
*/
BOOL ColloPericoloso(char *szOrdKey,int nCollo)
{
	PGresult *PGResRighe;
	int nRigheCollo;
	int nIndex;
	BOOL bFound=FALSE;


	PGResRighe=PGExecSQL(FALSE,"select rpcdpro,prperic from rig_prod, articoli where ordkey='%s' and rpnmcol=%d and prcdpro=rpcdpro;",szOrdKey,nCollo);
	nRigheCollo=PQntuples(PGResRighe);

	if (nRigheCollo!=0){
		nIndex=0;
		while(!bFound && nIndex<nRigheCollo){
			if(atoi(PQgetvalue(PGResRighe,nIndex,1))){
#ifdef TRACE
				trace_out_vstr_date(1,"COLLO [%s.%d] : Prodotto %s pericoloso",szOrdKey,nCollo,PQgetvalue(PGResRighe,nIndex,0));
#endif
				gtk_text_printf(&cRED,txt_msgs,"COLLO [%s.%d] : Prodotto %s pericoloso\n",szOrdKey,nCollo,PQgetvalue(PGResRighe,nIndex,0));
				bFound=TRUE;
			}
			nIndex++;
		}
	}
	PQclear(PGResRighe);

	return bFound;
}


BOOL CheckLettureBarcodeIDSuCollo(char *szOrdKey,int nCollo)
{
	PGresult *PGResRighe;
	int nRigheCollo;
	int nIndex;
	int nColloDaVerificare;
	BOOL bRetValue=FALSE;


	PGResRighe=PGExecSQL(TRUE,"select cpflbri from col_prod where ordkey='%s' and cpnmcol=%d and cpflbri in ('%c');",szOrdKey,nCollo,CONTROLLO_BARCODE_INCOMPLETO);
	nColloDaVerificare=PQntuples(PGResRighe);
	PQclear(PGResRighe);
	if (nColloDaVerificare==0) {
		bRetValue=TRUE;
	} else {


		PGResRighe=PGExecSQL(TRUE,"select rpcdpro,rpflbri,rpqtbri from rig_prod, articoli where prswbri='%c' and  rpflbri in ('%c','%c') and ordkey='%s' and rpnmcol=%d and prcdpro=rpcdpro;",CONTROLLO_BARCODE_ON,CONTROLLO_BARCODE_INCOMPLETO,CONTROLLO_BARCODE_FORZATO_PARZIALE,szOrdKey,nCollo);
		nRigheCollo=PQntuples(PGResRighe);
		PQclear(PGResRighe);

		/*
		* Se ho righe ancora da verificare setto il flag del collo a incompleto
		*/
		if (nRigheCollo!=0){
			
	#ifdef TRACE
			trace_out_vstr_date(1,"COLLO [%s.%d] con quantita di letture BRI non conforme ",szOrdKey,nCollo);
	#endif
			if (SetFlagBarcodeIdCollo(szOrdKey,nCollo,CONTROLLO_BARCODE_INCOMPLETO)) {
				bRetValue=FALSE;
			}
		} else {
			if (SetFlagBarcodeIdCollo(szOrdKey,nCollo,CONTROLLO_BARCODE_OK)) {
				bRetValue=TRUE;
			}
		}
	}

	return bRetValue;
}


void GetPLCMsg(int nMsgCode,int nMsgClass)
{
	PGresult *PGRes = NULL;
	PGresult *PGResInsert = NULL;
	char szMsgTxt[128];
	int nFieldIndex;
	int nRowIndex;

#ifdef TRACE
	trace_out_vstr_date(1,"Arrivato msg [%d.%d]",nMsgCode,nMsgClass);
#endif

	PGRes=PGExecSQL(Cfg.nDebugVersion>1,"select msgtext from plc_msg where msgcode=%d and msgclass=%d;",nMsgCode,nMsgClass);
	if(PQntuples(PGRes)){
		strcpy(szMsgTxt,PQgetvalue(PGRes,0,0));
#ifdef TRACE
		trace_out_vstr_date(1,"Arrivato msg [%d.%d.%s]",nMsgCode,nMsgClass,szMsgTxt);
#endif
		gtk_text_printf(&cRED,txt_msgs,"%s\n",szMsgTxt);
		ShowPLCMsg(szMsgTxt);
		
		PGResInsert = PGExecSQL(Cfg.nDebugVersion>1, "insert into plc_msg_history (msgdate, msgcode, msgclass, msgtext) values ('now',%d,%d,'%s');",
			nMsgCode,nMsgClass,szMsgTxt);
		if(!PGResInsert || PQresultStatus(PGResInsert) != PGRES_COMMAND_OK){
#ifdef TRACE
			trace_out_vstr_date(1,"Fallita storicizzazione messaggio da PLC [%d][%d] - %s",nMsgCode,nMsgClass,szMsgTxt);
#endif
		}
		PQclear(PGResInsert);

	}
	PQclear(PGRes);
}
