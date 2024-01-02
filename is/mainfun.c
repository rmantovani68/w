/*
* mainfun.c
* funzioni 
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
#include <time.h>
#include <unistd.h>
#include <libpq-fe.h>
#include <shared.h>

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

#include "picking.h"
#include "mainstruct.h"
#include "mainext.h"
#include "mainfun.h"

#include "callbacks.h"


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

	/* SAP */
	Cfg.nSAP=xncGetFileInt(szParagraph,"SAP",0,Cfg.szCniCfg,NULL);
	
	xncGetFileString(szParagraph,"Language",             "ita",       Cfg.szLanguage,             80,Cfg.szCniCfg,NULL);
	xncGetFileString(szParagraph,"PathData",             "../data",   Cfg.szPathData,             80,Cfg.szCniCfg,NULL);
	xncGetFileString(szParagraph,"PathExport",           "../export", Cfg.szPathExport,           80,Cfg.szCniCfg,NULL);
	xncGetFileString(szParagraph,"PathExe",              "./",        Cfg.szPathExe,              80,Cfg.szCniCfg,NULL);


	Cfg.nServicePort       = xncGetFileInt(szParagraph,      "ServicePort",    8050, Cfg.szCniCfg, NULL);
	Cfg.nDebugVersion      = xncGetFileInt(szParagraph,      "DebugVersion",      0, Cfg.szCniCfg,NULL);


	/*
	* lettura del file di configurazione del DataBase (PostgreSQL)
	*/
	strcpy(szParagraph,"DataBase Settings");

	xncGetFileString(szParagraph,"PGHost",    "localhost",Cfg.szPGHost,     80,Cfg.szCniCfg,NULL); 
	xncGetFileString(szParagraph,"PGPort",    "5432",     Cfg.szPGPort,     80,Cfg.szCniCfg,NULL); 
	xncGetFileString(szParagraph,"PGDataBase","picking",  Cfg.szPGDataBase, 80,Cfg.szCniCfg,NULL); 


	/* 
	* configurazioni specifiche di Main 
	*/
	Cfg.nMainDelay        = xncGetFileInt(ProcList[PROC_ISOLA].szProcName,"Delay",100,Cfg.szCniCfg,NULL);
	Cfg.nRefreshDelay     = xncGetFileInt(ProcList[PROC_ISOLA].szProcName,"RefreshDelay",60000,Cfg.szCniCfg,NULL);
	Cfg.nOrdineKeyField   = xncGetFileInt(ProcList[PROC_ISOLA].szProcName, "OrdineKeyField",    2, Cfg.szCniCfg, NULL);
	Cfg.nIsola            = xncGetFileInt(ProcList[PROC_ISOLA].szProcName, "Isola",    1, Cfg.szCniCfg, NULL);

	xncGetFileString(ProcList[PROC_ISOLA].szProcName,"PrinterName",          "lp",        Cfg.szPrinterName,          80,Cfg.szCniCfg,NULL);
	xncGetFileString(ProcList[PROC_ISOLA].szProcName,"LabelPrinterName",     "lp",        Cfg.szLabelPrinterName,     80,Cfg.szCniCfg,NULL);
	xncGetFileString(ProcList[PROC_ISOLA].szProcName,"BollePrinterName",       "lp",        Cfg.szBollePrinterName,       80,Cfg.szCniCfg,NULL);
	xncGetFileString(ProcList[PROC_ISOLA].szProcName,"FatturePrinterName",       "lp",        Cfg.szFatturePrinterName,       80,Cfg.szCniCfg,NULL);
	xncGetFileString(ProcList[PROC_ISOLA].szProcName,"SocksHost",        "wella1",        Cfg.szSocksHost,            80,Cfg.szCniCfg,NULL);


#ifdef TRACE
	trace_out_vstr_date(1,"Read Configuration");
	trace_out_vstr(1,"-------------------------------------------");
	trace_out_vstr(1,"Language           : [%s]", Cfg.szLanguage);
	trace_out_vstr(1,"PathData           : [%s]", Cfg.szPathData);
	trace_out_vstr(1,"PathExport         : [%s]", Cfg.szPathExport);
	trace_out_vstr(1,"PathExe            : [%s]", Cfg.szPathExe);
	trace_out_vstr(1,"DebugVersion       : [%d]", Cfg.nDebugVersion);
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
	TerminateProcess(PROC_PRINTLABEL);
	TerminateProcess(PROC_SOCK);

	/* cancello la coda messaggi principale */
	DeleteMsgQ(ProcList[PROC_ISOLA].nQNumber);

	/* chiudo la GUI */
	if(dlg_exit) gtk_widget_destroy(dlg_exit);
	gtk_widget_destroy(main_window);
	gtk_main_quit();
}




/*
* void RefreshListaOrdini(GtkWidget *parent,char *szSelect)
*
*	aggiorna la lista ordini contenuta in parent tramite il comando di select (szSelect) passatogli;
* se szSelect== NULL la funzione utilizza una select di default;
*/
void RefreshListaOrdini(GtkWidget *parent,char *szSelect)
{
	char szParagraph[128];
	char szSelectCmd[2048];

	if(lst_ordini != NULL){
		gtk_widget_destroy(lst_ordini);
		lst_ordini = NULL;
	}

	if (szSelect==NULL){
		/*
		* lettura dal file di configurazione della select
		*/
		strcpy(szParagraph,ProcList[PROC_ISOLA].szProcName);
		xncGetFileString(szParagraph,"SELECT_CMD", "", szSelectCmd, sizeof(szSelectCmd),Cfg.szCniCfg,NULL); 
	} else {
		strcpy(szSelectCmd,szSelect);
	}

	/*
	* Creazione clist widget su select
	*/
	CreateListFromSelect(main_window,parent,&lst_ordini,"lst_ordini",GTK_SELECTION_MULTIPLE,szSelectCmd,NULL,0);
	//gtk_window_set_focus(GTK_WINDOW(main_window),GTK_WIDGET(lst_ordini));
	
	/*
	* callback per selezione riga
	*/
  gtk_signal_connect (GTK_OBJECT (lst_ordini), "select_row", GTK_SIGNAL_FUNC (on_lst_ordini_select_row), 0);
}

int GetOrdineIndex(char *pszOrdine)
{
	int nIndex=0;
	char *pszText;

	while(gtk_clist_get_text(GTK_CLIST(lst_ordini),nIndex,Cfg.nOrdineKeyField,&pszText)){
		if(!strcmp(pszOrdine,pszText)){
			return(nIndex);
		}
		nIndex++;
	}
	return(-1);
}

void EditOrdine(char *szOrdineKey)
{
	GtkWidget *sw_edit_ordine=get_widget(dlg_edit_ordine,"sw_edit_ordine");
	GtkWidget *lst;
	PGresult *PGRes = NULL;
	BOOL bColloElaborato;
	char szParagraph[128];
	char szSelectCmd[2048];
	char szStato[128];
	char szBuffer[128];
	//char szOPFLPDF[128];
	char szOPFLFTZ[128];
	int nTuples;
	int nColli=0;
	int nRighe=0;
	int nCopie=0;

	/*
	* Cerco l'ordine specificato nella tabella ordini ricevuti
	*/
	PGRes=PGExecSQL(FALSE,"select ord_prod.opstato,ord_prod.opnmcll,ord_prod.opnmrgh,ord_prod.opnmcpe,ord_prod.ordprog,ric_ord.rodscli,ord_prod.opdteva,ord_prod.optmeva,ord_prod.oppspre,ord_prod.oppsrea,ord_prod.opflpdf,ord_prod.opflftz from ord_prod,ric_ord where ric_ord.ordprog=ord_prod.ordprog and ord_prod.ordkey='%s';",szOrdineKey);

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
		strcpy(szOPFLFTZ,PQgetvalue(PGRes,0,10));
		strcpy(szOPFLFTZ,PQgetvalue(PGRes,0,11));
		gtk_label_printf(get_widget(dlg_edit_ordine,"lb_FLPDF"),GetStatoOrdine(szStato[0],szBuffer) );
		gtk_label_printf(get_widget(dlg_edit_ordine,"lb_FLFTZ"),GetStatoOrdine(szStato[0],szBuffer) );

		/*
		* controllo se l'ordine e' gia stato elaborato e quindi formati i colli
		*/
		PGResElab=PGExecSQL(FALSE,"select ordkey,opflftz from ord_prod where ordkey='%s';",szOrdineKey);
		if(PQntuples(PGResElab)){
			/*
			* Ordine in produzione -> visualizzo ls lista dei colli
			*/
			bColloElaborato=TRUE;
			strcpy(szParagraph,"Isola Edit Colli Ordine");
			gtk_widget_set_sensitive(get_widget(dlg_edit_ordine,"pb_edit_collo"),TRUE);
		}
		PQclear(PGResElab);
	}

	PQclear(PGRes);

	/*
	* lettura della select dal file di configurazione
	*/
	xncGetFileString(szParagraph,"SELECT_CMD", "", szSelectCmd, sizeof(szSelectCmd),Cfg.szCniCfg,NULL); 

	/*
	* Se la lista colli o righe esiste 
	*/
	if((lst=get_widget(dlg_edit_ordine,"lst_edit_ordine"))){
		gtk_widget_destroy(lst);
	}

	/*
	* Creazione clist widget su select
	*/
	CreateListFromSelect(dlg_edit_ordine,sw_edit_ordine,&lst,"lst_edit_ordine",GTK_SELECTION_SINGLE,szSelectCmd,szOrdineKey,0);

	strcpy(szOrdineInEdit,szOrdineKey);
}


void ListaDistribuzione(void)
{
	//GtkWidget *sw_distribuzione=get_widget(dlg_distribuzione,"sw_distribuzione");
	GtkWidget *lst=get_widget(dlg_distribuzione,"clist_distribuzione");
	PGresult *PGRes;
	PGresult *PGResOrd;
	char *RecordData[1][MAX_FIELDS_NUMBER];
	int nIndex;
	int nOrdiniLanciati=0;
	int nOrdiniEvasi=0;

	for(nIndex=0;nIndex<10;nIndex++){
		RecordData[0][nIndex]=malloc(100);
	}
	PGRes=PGExecSQL(Cfg.nDebugVersion,"select ric_ord.ronelab,count(ric_ord.ronelab),sum(ric_ord.ronmcpe),sum(ric_ord.ronmrgh),sum(ric_ord.ronmcll),sum(ric_ord.ropspre) from ric_ord  group by ric_ord.ronelab order by ric_ord.ronelab;");
	for(nIndex=0;nIndex<PQntuples(PGRes);nIndex++){
		/* ordini lanciati */
		nOrdiniLanciati=0;
		PGResOrd=PGExecSQL(Cfg.nDebugVersion,"select count(ric_ord.ordprog) from ric_ord where ronelab='%s' and rostato!='%c';",PQgetvalue(PGRes,nIndex,0),ORDINE_RICEVUTO);
		if(PQntuples(PGResOrd)){
			nOrdiniLanciati=atoi(PQgetvalue(PGResOrd,0,0));
		}
		PQclear(PGResOrd);

		/* ordini evasi */
		nOrdiniEvasi=0;
		PGResOrd=PGExecSQL(Cfg.nDebugVersion,"select count(ric_ord.ordprog) from ric_ord where ronelab='%s' and rostato='%c';",PQgetvalue(PGRes,nIndex,0),ORDINE_EVASO);
		if(PQntuples(PGResOrd)){
			nOrdiniEvasi=atoi(PQgetvalue(PGResOrd,0,0));
		}
		PQclear(PGResOrd);

		strcpy(RecordData[0][0],PQgetvalue(PGRes,nIndex,0));
		sprintf(RecordData[0][1],"%5d",atoi(PQgetvalue(PGRes,nIndex,1)));
		sprintf(RecordData[0][2],"%5d",nOrdiniLanciati);
		sprintf(RecordData[0][3],"%5d",nOrdiniEvasi);
		sprintf(RecordData[0][4],"%5d",atoi(PQgetvalue(PGRes,nIndex,2)));
		sprintf(RecordData[0][5],"%5d",atoi(PQgetvalue(PGRes,nIndex,3)));
		sprintf(RecordData[0][6],"%5d",atoi(PQgetvalue(PGRes,nIndex,4)));
		sprintf(RecordData[0][7],"%7.2f",(float)((float)atoi(PQgetvalue(PGRes,nIndex,5))/(float)1000));

		gtk_clist_append(GTK_CLIST(lst), RecordData[0]);

	}
	PQclear(PGRes);

	for(nIndex=0;nIndex<10;nIndex++){
		free(RecordData[0][nIndex]);
	}
}

BOOL EditCollo(char *szOrdineKey,int nNumCollo)
{
	char szParagraph[128];
	char szSelectCmd[2048];
	char szListName[128];
	char szStato[128];
	char szBuffer[128];
	GtkWidget *sw_edit_collo=get_widget(dlg_edit_collo,"sw_edit_collo");
	GtkWidget *lst;
	PGresult *PGRes = NULL;
	int nTuples;

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
		strcpy(szParagraph,"Isola Edit Righe Collo");
		strcpy(szListName,"lst_righe_collo");
		/*
		* Reset Query
		*/
		PQclear(PGRes);
	} else {
		PQclear(PGRes);
		return FALSE;
	}

	/*
	* lettura della select corrispondente dal file di configurazione
	*/
	xncGetFileString(szParagraph,"SELECT_CMD",  "", szSelectCmd, sizeof(szSelectCmd),Cfg.szCniCfg,NULL); 

	strcpy(szListName,"lst_righe_collo");
	if((lst=get_widget(dlg_edit_collo,szListName))){
		gtk_widget_destroy(lst);
	}
	/*
	* Creazione clist widget su select
	*/
	CreateListFromSelect(dlg_edit_collo,sw_edit_collo,&lst,szListName,GTK_SELECTION_SINGLE,szSelectCmd,szOrdineKey,nNumCollo);

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
		case COLLO_SPEDITO_HOST:
			strcpy(pszBuffer,"SPEDITO");
		break;
		default:
			strcpy(pszBuffer,"NON DEFINITO");
		break;
	}
	return pszBuffer;
}

void UpdateOrdine(char *szOrdineKey)
{
	char szParagraph[128];
	char szUpdateCmd[2048];
	GtkWidget *lst_ordini=get_widget(main_window,"lst_ordini");

	strcpy(szParagraph,ProcList[PROC_ISOLA].szProcName);
	/*
	* lettura della select corrispondente dal file di configurazione
	*/
	xncGetFileString(szParagraph,"UPDATE_CMD",  "", szUpdateCmd, sizeof(szUpdateCmd),Cfg.szCniCfg,NULL); 

	UpdateListFromSelect(lst_ordini, szUpdateCmd, szOrdineKey, Cfg.nOrdineKeyField);
}

/*
* UpdateListFromSelect()
* Aggiorna una riga di una lista GTK da una select SQL
*/
int UpdateListFromSelect(GtkWidget *clist, char *szSqlCmd, char *szKey, int nKeyField)
{
	int nTuples;
	int nFields;
	int nIndex;
	int nFieldIndex;
	char szCmd[4096];
	char szSqlTmp[4096];
	char *pPtr;
	char *pszText;
	PGresult *PGRes = NULL;
	char szText[256];
	
	strcpy(szSqlTmp,szSqlCmd);
	strcpy(szCmd,szSqlCmd);

	/*
	* Sostituisco i valori parametrici
	* %KEY%   : Chiave
	*/
	if((pPtr=strstr(szSqlTmp,"%KEY%"))){
		*pPtr='\0';
		strcpy(szCmd,szSqlTmp);
		strcat(szCmd,szKey);
		strcat(szCmd,pPtr+5);
	}
	
	/*
	* Eseguo la select specificata
	*/
	PGRes=PGExecSQL(FALSE,szCmd);

	nTuples=PQntuples(PGRes);
	nFields=PQnfields(PGRes);

	if(nTuples==1){
		for(nIndex=0;nIndex<(GTK_CLIST(clist)->rows);nIndex++){
			gtk_clist_get_text(GTK_CLIST(clist),nIndex,nKeyField,&pszText);
			if(!strcmp(szKey,pszText)){
				for(nFieldIndex=0;nFieldIndex<nFields;nFieldIndex++){
					int nLength;

					if(PQftype(PGRes,nFieldIndex)==1042){
						/* si tratta di una stringa */
						nLength=PQgetlength(PGRes,0,nFieldIndex);
						sprintf(szText,"%*s",nLength,PQgetvalue(PGRes,0,nFieldIndex));
					} else if(PQftype(PGRes,nFieldIndex)==701){
						/* si tratta di un float */
						nLength=strlen(PQfname(PGRes,nFieldIndex));
						sprintf(szText,"%*.*f",nLength,3,atof(PQgetvalue(PGRes,0,nFieldIndex)));
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
	}
	
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
	if((pPtr=strstr(szSqlCmd,"%ISOLA%"))){
		*pPtr='\0';
		strcpy(szCmd,szSqlCmd);
		sprintf(szBuffer,"%d",Cfg.nIsola);
		strcat(szCmd,szBuffer);
		strcat(szCmd,pPtr+7);
	}
	strcpy(szSqlCmd,szCmd);
	
	/*
	* Eseguo la select specificata
	*/
	PGRes=PGExecSQL(Cfg.nDebugVersion,szSqlCmd);

	nTuples=PQntuples(PGRes);
	nFields=PQnfields(PGRes);

	if(nTuples>=0){
		/*
		* Creazione clist widget 
		*/

		*clist = gtk_clist_new (nFields);
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
					sprintf(RecordData[0][nFieldIndex],"%*.*f",nLength,3,atof(PQgetvalue(PGRes,nIndex,nFieldIndex)));
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
		//fprintf(stdout,"\nColonna %d ->\tOid : %3d\tSize : %3d\tLenght : %3d",nColumnIndex,PQftype(PGRes,nColumnIndex),PQfsize(PGRes,nColumnIndex),PQgetlength(PGRes,0,nColumnIndex));
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

BOOL SendSockCmd(int nServicePort,char *szHostName, int nCmd,char *szParams)
{
	int nFd=-1;
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


void EditConfigurazione(void)
{	
	dlg_config=create_dlg_config();
	
	/*
	* GENERAL
	*/
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(get_widget(dlg_config,"sb_main_delay")),Cfg.nMainDelay);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(get_widget(dlg_config,"sb_isola")),Cfg.nIsola);
	/*
	* tempo di refrsh lista ordini a video
	*/
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(get_widget(dlg_config,"sb_refresh_delay")),Cfg.nRefreshDelay);

	if (Cfg.nDebugVersion==1){
		gtk_widget_activate(get_widget(dlg_config,"cb_debug"));
	}

	/*
	* DIRECTORY
	*/
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_config,"entry_pathdata")),   Cfg.szPathData);
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_config,"entry_pathexe")),    Cfg.szPathExe);
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_config,"entry_pathexport")), Cfg.szPathExport);

	/*
	* DATABASE
	*/
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_config,"entry_pghost")),     Cfg.szPGHost);
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_config,"entry_pgport")),     Cfg.szPGPort);
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_config,"entry_pgdatabase")), Cfg.szPGDataBase);

	/*
	* STAMPANTI
	*/
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_config,"entry_printer_name")),        Cfg.szPrinterName);
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_config,"entry_label_printer_name")),  Cfg.szLabelPrinterName);

	gtk_window_set_focus (GTK_WINDOW (dlg_config), get_widget(dlg_config,"pb_ok"));
	gtk_widget_show(dlg_config);
	gtk_window_set_transient_for(GTK_WINDOW(dlg_config),GTK_WINDOW(main_window));
}



void ApplyConfigurazione(void)
{
	/*
	* GENERAL
	*/

	Cfg.nMainDelay = gtk_spin_button_get_value_as_int((GtkSpinButton *)get_widget(dlg_config,"sb_main_delay"));
	Cfg.nIsola = gtk_spin_button_get_value_as_int((GtkSpinButton *)get_widget(dlg_config,"sb_isola"));
	Cfg.nRefreshDelay = gtk_spin_button_get_value_as_int((GtkSpinButton *)get_widget(dlg_config,"sb_refresh_delay"));

	if (GTK_TOGGLE_BUTTON(get_widget(dlg_config,"cb_debug"))->active){
		Cfg.nDebugVersion=1;
	} else {
		Cfg.nDebugVersion=0;
	}
	
	/*
	* DIRECTORY
	*/
	strcpy(Cfg.szPathData,          gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_pathdata"))));
	strcpy(Cfg.szPathExe,           gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_pathexe"))));
	strcpy(Cfg.szPathExport,        gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_pathexport"))));


	/*
	* DATABASE
	*/
	strcpy(Cfg.szPGHost,         gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_pghost"))));
	strcpy(Cfg.szPGPort,         gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_pgport"))));
	strcpy(Cfg.szPGDataBase,     gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_pgdatabase"))));

	/*
	* STAMPANTI
	*/
	strcpy(Cfg.szPrinterName,       gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_printer_name"))));
	strcpy(Cfg.szLabelPrinterName,  gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_label_printer_name"))));

	/*
	* delay applicazione
	*/
	xncPutFileInt(ProcList[PROC_ISOLA].szProcName, "Delay", Cfg.nMainDelay,Cfg.szCniCfg,NULL);
	/*
	* Isola gestita
	*/
	xncPutFileInt(ProcList[PROC_ISOLA].szProcName, "Isola", Cfg.nIsola,Cfg.szCniCfg,NULL);
	/*
	* tempo di refrsh lista ordini a video
	*/
	xncPutFileInt(ProcList[PROC_ISOLA].szProcName, "RefreshDelay", Cfg.nRefreshDelay,Cfg.szCniCfg,NULL);

	xncPutFileInt("General Settings", "DebugVersion", Cfg.nDebugVersion,Cfg.szCniCfg,NULL);

	xncPutFileString("General Settings", "PathData",   Cfg.szPathData,   Cfg.szCniCfg, NULL); 
	xncPutFileString("General Settings", "PathExport", Cfg.szPathExport, Cfg.szCniCfg, NULL); 
	xncPutFileString("General Settings", "PathExe",    Cfg.szPathExe,    Cfg.szCniCfg, NULL); 

	xncPutFileString(ProcList[PROC_ISOLA].szProcName, "PrinterName",       Cfg.szPrinterName,       Cfg.szCniCfg, NULL); 
	xncPutFileString(ProcList[PROC_ISOLA].szProcName, "LabelPrinterName",  Cfg.szLabelPrinterName,  Cfg.szCniCfg, NULL); 

	xncPutFileString("DataBase Settings", "PGHost",        Cfg.szPGHost,       Cfg.szCniCfg, NULL); 
	xncPutFileString("DataBase Settings", "PGPort",        Cfg.szPGPort,       Cfg.szCniCfg, NULL); 
	xncPutFileString("DataBase Settings", "PGDataBase",    Cfg.szPGDataBase,   Cfg.szCniCfg, NULL); 
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
