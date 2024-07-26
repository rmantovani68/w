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
	* rm+st 13-02-2001 : modifiche per secondi invii
	* lettura lista stampanti utilizzate
	*/
	ReadPrinters(Cfg.szCniCfg);

	/*
	* lettura del file di configurazione
	*/
	strcpy(szParagraph,"General Settings");

	/* SAP */
	Cfg.nSAP = xncGetFileInt(szParagraph, "SAP", 0, Cfg.szCniCfg, NULL);

	xncGetFileString(szParagraph,"Language",             "ita",                   Cfg.szLanguage,             80,Cfg.szCniCfg,NULL);
	xncGetFileString(szParagraph,"PathData",             "../data",               Cfg.szPathData,             80,Cfg.szCniCfg,NULL);
	xncGetFileString(szParagraph,"PathStampe",           "../stampe",             Cfg.szPathStampe,           80,Cfg.szCniCfg,NULL);
	xncGetFileString(szParagraph,"PathExport",           "../export_sap",         Cfg.szPathExport,           80,Cfg.szCniCfg,NULL);
	xncGetFileString(szParagraph,"PathExportPdf",        "../export_sap/pdf",     Cfg.szPathExportPdf,        80,Cfg.szCniCfg,NULL);
	xncGetFileString(szParagraph,"PathStorePdf",        "../export_sap/pdfstr",   Cfg.szPathStorePdf,         80,Cfg.szCniCfg,NULL);
	xncGetFileString(szParagraph,"PathExe",              "./",                    Cfg.szPathExe,              80,Cfg.szCniCfg,NULL);
	xncGetFileString(szParagraph,"PrinterConsole",       "lp",                    Cfg.szPrinterConsole,       80,Cfg.szCniCfg,NULL);
	xncGetFileString(szParagraph,"PrinterIsola1",        "lp",                    Cfg.szPrinterIsola1,        80,Cfg.szCniCfg,NULL);
	xncGetFileString(szParagraph,"PrinterIsola2",        "lp",                    Cfg.szPrinterIsola2,        80,Cfg.szCniCfg,NULL);
	xncGetFileString(szParagraph,"PrinterBilancia",      "lp",                    Cfg.szPrinterBilancia,      80,Cfg.szCniCfg,NULL);
	xncGetFileString(szParagraph,"LabelPrinterConsole",  "lp",                    Cfg.szLabelPrinterConsole,  80,Cfg.szCniCfg,NULL);
	xncGetFileString(szParagraph,"LabelPrinterIsola1",   "lp",                    Cfg.szLabelPrinterIsola1,   80,Cfg.szCniCfg,NULL);
	xncGetFileString(szParagraph,"LabelPrinterIsola2",   "lp",                    Cfg.szLabelPrinterIsola2,   80,Cfg.szCniCfg,NULL);
	xncGetFileString(szParagraph,"FatturePrinterIsola1", "lp",                    Cfg.szFatturePrinterIsola1, 80,Cfg.szCniCfg,NULL);
	xncGetFileString(szParagraph,"FatturePrinterIsola2", "lp",                    Cfg.szFatturePrinterIsola2, 80,Cfg.szCniCfg,NULL);
	xncGetFileString(szParagraph,"BollePrinterIsola1",   "lp",                    Cfg.szBollePrinterIsola1,   80,Cfg.szCniCfg,NULL);
	xncGetFileString(szParagraph,"BollePrinterIsola2",   "lp",                    Cfg.szBollePrinterIsola2,   80,Cfg.szCniCfg,NULL);
	xncGetFileString(szParagraph,"LabelPrinterBilancia", "lp",                    Cfg.szLabelPrinterBilancia, 80,Cfg.szCniCfg,NULL);

	xncGetFileString(szParagraph,"ImpOrdiniBertello", "ordini_bertello.txt",      Cfg.szImpOrdiniBertello,    80,Cfg.szCniCfg,NULL);
	xncGetFileString(szParagraph,"ImpColliBertello",  "colli_bertello.txt",       Cfg.szImpColliBertello,     80,Cfg.szCniCfg,NULL);
	xncGetFileString(szParagraph,"ImpRigheBertello",  "righe_bertello.txt",       Cfg.szImpRigheBertello,     80,Cfg.szCniCfg,NULL);

	xncGetFileString(szParagraph,"ImpOrdiniBertelloR", "ordini_bertello_n.txt",      Cfg.szImpOrdiniBertelloB,    80,Cfg.szCniCfg,NULL);
	xncGetFileString(szParagraph,"ImpRigheBertelloR",  "righe_bertello_n.txt",       Cfg.szImpRigheBertelloB,     80,Cfg.szCniCfg,NULL);
	xncGetFileString(szParagraph,"ImpOrdiniBertelloB", "ordini_bertello_n.txt",      Cfg.szImpOrdiniBertelloR,    80,Cfg.szCniCfg,NULL);
	xncGetFileString(szParagraph,"ImpRigheBertelloB",  "righe_bertello_n.txt",       Cfg.szImpRigheBertelloR,     80,Cfg.szCniCfg,NULL);

	xncGetFileString(szParagraph,"ImpOrdiniFile",     "Ordini.txt",               Cfg.szImpOrdiniFile,        80,Cfg.szCniCfg,NULL);
	xncGetFileString(szParagraph,"ImpRigheFile",      "Ordini.txt",               Cfg.szImpRigheFile,         80,Cfg.szCniCfg,NULL);
	xncGetFileString(szParagraph,"ImpArticoliFile",   "Articoli.txt",             Cfg.szImpArticoliFile,      80,Cfg.szCniCfg,NULL);
	xncGetFileString(szParagraph,"ImpUbicazioniFile", "Mappa.txt",                Cfg.szImpUbicazioniFile,    80,Cfg.szCniCfg,NULL);

	xncGetFileString(szParagraph,"ExpOrdiniFile",   "OrdCni.txt",                 Cfg.szExpOrdiniFile,    80,Cfg.szCniCfg,NULL);
	xncGetFileString(szParagraph,"ExpColliFile",    "Colli.txt",                  Cfg.szExpColliFile,     80,Cfg.szCniCfg,NULL);
	xncGetFileString(szParagraph,"ExpDettColliFile","dett_colli.txt",             Cfg.szExpDettColliFile, 80,Cfg.szCniCfg,NULL);

	xncGetFileString(szParagraph,"ExpDatiBertello", "IL_DATA.txt",                 Cfg.szExpDatiBertello, 80,Cfg.szCniCfg,NULL);

	Cfg.nOrdiniKeyField       = xncGetFileInt("Tabella Ordini", "OrdiniKeyField",        4, Cfg.szCniCfg, NULL);
	Cfg.nOrdiniStatusField    = xncGetFileInt("Tabella Ordini", "OrdiniStatusField",     0, Cfg.szCniCfg, NULL);
	Cfg.nOrdiniPdfStatusField = xncGetFileInt("Tabella Ordini", "OrdiniPdfStatusField",  1, Cfg.szCniCfg, NULL);
	Cfg.nOrdiniFtzStatusField = xncGetFileInt("Tabella Ordini", "OrdiniFtzStatusField", 14, Cfg.szCniCfg, NULL);
	Cfg.nOrdiniElabField      = xncGetFileInt("Tabella Ordini", "OrdiniElabField",       3, Cfg.szCniCfg, NULL);

	Cfg.nOrdinamento          = xncGetFileInt(szParagraph,      "Ordinamento",           0, Cfg.szCniCfg, NULL);
	Cfg.nShmKey               = xncGetFileInt(szParagraph,      "ShmKey",              256, Cfg.szCniCfg, NULL);
	Cfg.nServicePort          = xncGetFileInt(szParagraph,      "ServicePort",        8050, Cfg.szCniCfg, NULL);
	Cfg.nChiusuraCollo        = xncGetFileInt(szParagraph,      "ChiusuraCollo",         1, Cfg.szCniCfg,NULL);
	Cfg.nImballiUbicati       = xncGetFileInt(szParagraph,      "ImballiUbicati",        0, Cfg.szCniCfg,NULL);
	Cfg.nAttesaPDFOrdineCompleto=xncGetFileInt(szParagraph,     "AttesaPDFOrdineCompleto",1,Cfg.szCniCfg,NULL);
	Cfg.nStampaPL             = xncGetFileInt(szParagraph,      "StampaPL",              0, Cfg.szCniCfg,NULL);
	Cfg.nStampaRAC            = xncGetFileInt(szParagraph,      "StampaRAC",             0, Cfg.szCniCfg,NULL);
	Cfg.nDebugVersion         = xncGetFileInt(szParagraph,      "DebugVersion",          0, Cfg.szCniCfg,NULL);
	Cfg.nMaxColliOrdine       = xncGetFileInt(szParagraph,      "MaxColliOrdine",        0, Cfg.szCniCfg,NULL);
	Cfg.nAdvancedInvoicing    = xncGetFileInt(szParagraph,      "AdvancedInvoicing",     0, Cfg.szCniCfg,NULL);
	/*
	* TRUCCO INFAME PER GESTIONE MENU TOGGLE (DA SISTEMARE!!!)
	*/
	Cfg.nVisualizzaSpeditiHost = 1;

	Cfg.nSogliaControlloBarcode            = xncGetFileInt(szParagraph,      "SogliaControlloBarcode",    12, Cfg.szCniCfg,NULL);
	Cfg.nGestioneBadge                     = xncGetFileInt(szParagraph,      "GestioneBadge",              0, Cfg.szCniCfg,NULL);

	/*
	* Gestione progressivo lancio (azzerato annualmente)
	*/
	Cfg.nAnno=xncGetFileInt(ProcList[PROC_MAIN].szProcName,"Anno",0,Cfg.szCniCfg,NULL);
	Cfg.nProgressivoLancio =xncGetFileInt(ProcList[PROC_MAIN].szProcName,      "ProgressivoLancio", 0, Cfg.szCniCfg, NULL);
	if (atoi(LeftStr(GetDateYYYYMMDD(time((long *)0),szDateBuffer),4))!=Cfg.nAnno){
		/*
		* Anno cambiato
		* Azzero il progressivo lancio
		*/
		Cfg.nAnno= atoi(LeftStr(GetDateYYYYMMDD(time((long *)0),szDateBuffer),4));
		/*
		* Assegno al progressivo lancio il valore di base 
		*/
		Cfg.nProgressivoLancio=LOW_PRIORITY;

		xncPutFileInt(ProcList[PROC_MAIN].szProcName,"Anno",Cfg.nAnno,Cfg.szCniCfg,NULL);
		xncPutFileInt(ProcList[PROC_MAIN].szProcName,"ProgressivoLancio",Cfg.nProgressivoLancio,Cfg.szCniCfg,NULL);
	}
	
	/*
	* Password
	*/
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
	* configurazioni specifiche di Main 
	*/
	Cfg.nMainDelay=xncGetFileInt(ProcList[PROC_MAIN].szProcName,"Delay",100,Cfg.szCniCfg,NULL);

	Cfg.nSaveStateDelay=xncGetFileInt(ProcList[PROC_MAIN].szProcName,"SaveStateDelay",100,Cfg.szCniCfg,NULL);

#ifdef TRACE
	trace_out_vstr_date(1,"Read Configuration");
	trace_out_vstr(1,"-------------------------------------------");
	trace_out_vstr(1,"Language           : [%s]", Cfg.szLanguage);
	trace_out_vstr(1,"PathData           : [%s]", Cfg.szPathData);
	trace_out_vstr(1,"PathStampe         : [%s]", Cfg.szPathStampe);
	trace_out_vstr(1,"PathExport         : [%s]", Cfg.szPathExport);
	trace_out_vstr(1,"PathExe            : [%s]", Cfg.szPathExe);
	trace_out_vstr(1,"DebugVersion       : [%d]", Cfg.nDebugVersion);
	trace_out_vstr(1,"AdvancedInvoicing  : [%d]", Cfg.nAdvancedInvoicing);
	trace_out_vstr(1,"MaxColliOrdine     : [%d]", Cfg.nMaxColliOrdine);
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

int ReadPrinters(char *szCfgFile)
{
	char szBuffer[256];
	char szParagraph[80];
	char szField[80];
	char *pPtr;
	char *szSeparator=",\t\n";
	int nIndex;

	strcpy(szParagraph,"Printers");
	nIndex=0;
	sprintf(szField,"Printer_%02d",nIndex);
	while(xncGetFileString(szParagraph,szField,"",szBuffer,sizeof(szBuffer),szCfgFile,NULL)){
		if(strlen(szBuffer)){
			/* Nome Coda di Stampa */
			if((pPtr=strtok(szBuffer,szSeparator))!=NULL){
				strcpy(Printers[nIndex].szName,pPtr);
			} else {return(-1);}
			/* Descrizione */
			if((pPtr=strtok(NULL,szSeparator))!=NULL){
				strcpy(Printers[nIndex].szDescription,pPtr);
			} else {return(-1);}
		}
		nIndex++;
		sprintf(szField,"Printer_%02d",nIndex);
	}
	Printers[nIndex].szName[0]='\0';
	return nIndex;
}

void ExitApp()
{
	if(DBConn) PQfinish(DBConn);

#ifdef TRACE
	trace_out_vstr_date(1,"Stopped");
#endif
	TerminateProcess(PROC_SETTORI);
	TerminateProcess(PROC_IOS);
	TerminateProcess(PROC_VOLUM);
	TerminateProcess(PROC_PRINTLABEL);
	TerminateProcess(PROC_SOCK);
	TerminateProcess(PROC_AUTOTASK);

	/* cancello la coda messaggi principale */
	DeleteMsgQ(ProcList[PROC_MAIN].nQNumber);

	if(pDatiLinea){
		DetachShm((char *)pDatiLinea);

		if(!RemoveShm(Cfg.nShmKey,sizeof(LINEA_STRUCT))){
#ifdef TRACE
			trace_out_vstr(1,"Error Removing Shared Memory (%d)", Cfg.nShmKey);
#endif
		}
	}

	/* chiudo la GUI */
	gtk_widget_destroy(dlg_exit);
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
	char szKey[128];
	char szSelectCmd[2048];
  int nRigaLista=0;
	char *pszOrdiniStatus;
	char *pszOrdiniPdfStatus;
	char *pszOrdiniFtzStatus;
	char *pszOrdiniKey;
	char szTmpBuff[128];
	char cStato;
	char cPdfStato;
	char cFtzStato;
	GdkColor *cBgColor;
	GdkColor *cFgColor;

#ifdef TRACE
		trace_out_vstr_date(1, "RefreshListaOrdini: INIZIO");
#endif

	if(lst_ordini != NULL){
		gtk_widget_destroy(lst_ordini);
		lst_ordini = NULL;
	}

	if (szSelect==NULL){
		/*
		* lettura dal file di configurazione della select
		*/
		strcpy(szParagraph,"Tabella Ordini");
		if(Cfg.nVisualizzaSpeditiHost){
			xncGetFileString(szParagraph,"SELECT_CMD", "", szSelectCmd, sizeof(szSelectCmd),Cfg.szCniCfg,NULL); 
		} else {
			xncGetFileString(szParagraph,"SELECT_CMD_NO_HOST", "", szSelectCmd, sizeof(szSelectCmd),Cfg.szCniCfg,NULL); 
		}
	} else {
		strcpy(szSelectCmd,szSelect);
	}
#ifdef TRACE
		trace_out_vstr_date(1, "RefreshListaOrdini: LETTURA CFG");
#endif

#ifdef TRACE
		trace_out_vstr_date(1, "RefreshListaOrdini: INIZIO CreateListFromSelect");
#endif
	/*
	* Creazione clist widget su select
	*/
	CreateListFromSelect(main_window,parent,&lst_ordini,"lst_ordini",GTK_SELECTION_MULTIPLE,szSelectCmd,NULL,0);
#ifdef TRACE
		trace_out_vstr_date(1, "RefreshListaOrdini: FINE CreateListFromSelect");
#endif
	
	/*
	* callback per selezione riga
	*/
  gtk_signal_connect (GTK_OBJECT (lst_ordini), "select_row", GTK_SIGNAL_FUNC (on_lst_ordini_select_row), 0);
	
/*******************************************************
* VERSIONE CON GESTIONE PDF
********************************************************/
	/*
	* Aggiungo al refresh la colorazione della lista in base allo stato
	* dell'ordine e del flag pdf
	*/
#ifdef TRACE
		trace_out_vstr_date(1, "RefreshListaOrdini: INIZIO COLORAZIONE");
#endif
	

	while(nRigaLista<(GTK_CLIST(lst_ordini)->rows)){

		gtk_clist_get_text(GTK_CLIST(lst_ordini),nRigaLista,Cfg.nOrdiniStatusField,&pszOrdiniStatus);    
		strcpy(szTmpBuff, pszOrdiniStatus); StrTrimAll(szTmpBuff); 
		cStato=szTmpBuff[0];
		gtk_clist_get_text(GTK_CLIST(lst_ordini),nRigaLista,Cfg.nOrdiniPdfStatusField,&pszOrdiniPdfStatus);
		strcpy(szTmpBuff, pszOrdiniPdfStatus); StrTrimAll(szTmpBuff); 
		cPdfStato=szTmpBuff[0];
		gtk_clist_get_text(GTK_CLIST(lst_ordini),nRigaLista,Cfg.nOrdiniFtzStatusField,&pszOrdiniFtzStatus);
		strcpy(szTmpBuff, pszOrdiniFtzStatus); StrTrimAll(szTmpBuff); 
		cFtzStato=szTmpBuff[0];
		gtk_clist_get_text(GTK_CLIST(lst_ordini),nRigaLista,Cfg.nOrdiniKeyField,&pszOrdiniKey);
		strcpy(szKey,pszOrdiniKey);
		cFgColor=&cBLACK;
		cBgColor=&cWHITE;


		switch(cStato){
			case ORDINE_RICEVUTO:
			{
#ifdef TRACE
				trace_out_vstr_date(1, "RefreshListaOrdini: Ordine %s Stato %c Pdf %c", szKey, cStato, cPdfStato);
				trace_out_vstr_date(1, " - %d - %s - ", Cfg.nOrdiniPdfStatusField,pszOrdiniPdfStatus);
#endif
				if (Cfg.nAttesaPDFOrdineCompleto==1){
					switch(cPdfStato){
						case ORDINE_CON_FATTURA:
						case ORDINE_CON_DDT:
						case ORDINE_SENZA_PDF:
							//cFgColor=&cLGREEN;
						break;
						default:
						{
#ifdef TRACE
							trace_out_vstr_date(1, "Colore LRED");
#endif
							cFgColor=&cLRED;
						}
						break;
					}
				}
			}
			break;
			case ORDINE_SPEDITO:
			{
#ifdef TRACE
				trace_out_vstr_date(1, "RefreshListaOrdini: Ordine %s Stato %c Pdf %c", szKey, cStato, cPdfStato);
				trace_out_vstr_date(1, " - %d - %s - ", Cfg.nOrdiniPdfStatusField,pszOrdiniPdfStatus);
#endif
				if (Cfg.nAttesaPDFOrdineCompleto==1){
					switch(cPdfStato){
						case ORDINE_CON_FATTURA:
						case ORDINE_CON_DDT:
						case ORDINE_SENZA_PDF:
							//cFgColor=&cLGREEN;
						break;
						default:
						{
#ifdef TRACE
							trace_out_vstr_date(1, "Colore LRED");
#endif
							cFgColor=&cLRED;
						}
						break;
					}
				}
			}
			break;
			case ORDINE_ELABORATO:
			{
#ifdef TRACE
				trace_out_vstr_date(1, "RefreshListaOrdini: Ordine %s Stato %c Pdf %c", szKey, cStato, cPdfStato);
				trace_out_vstr_date(1, " - %d - %s - ", Cfg.nOrdiniPdfStatusField,pszOrdiniPdfStatus);
#endif
				if (Cfg.nAttesaPDFOrdineCompleto==1){
					switch(cPdfStato){
						case ORDINE_CON_FATTURA:
						case ORDINE_CON_DDT:
						case ORDINE_SENZA_PDF:
							//cFgColor=&cLGREEN;
						break;
						default:
						{
#ifdef TRACE
							trace_out_vstr_date(1, "Colore LRED");
#endif
							cFgColor=&cLRED;
						}
						break;
					}
				}
			}
			break;
			case ORDINE_STAMPATA_RAC:
			break;
			case ORDINE_IN_PRELIEVO:
			break;
			case ORDINE_INCOMPLETO_IN_PRELIEVO:
			break;
			case ORDINE_INCOMPLETO_TERMINATO:
			case ORDINE_INCOMPLETO_TERMINATO_SPEDITO:
			{
				switch(cPdfStato){
					case ORDINE_CON_FATTURA:
					case ORDINE_CON_DDT:
						cFgColor=&cLGREEN;
					break;
					default:
						cFgColor=&cLRED;
					break;
				}
			}
			break;
			case ORDINE_INCOMPLETO_TERMINATO_IN_PRELIEVO:
			break;
			case ORDINE_PRELEVATO:
			break;
			case ORDINE_EVASO:
			break;
			case ORDINE_STAMPATA_XAB:
			break;
			case ORDINE_STAMPATA_DIST:
			break;
			case ORDINE_SPEDITO_HOST:
			break;
			default:
			break;
		}
		switch(cFtzStato){
			case ORDINE_FITTIZIO:
				cBgColor=&cLGREY;
			break;
			default:
			break;
		}

		gtk_clist_set_background(GTK_CLIST(lst_ordini),nRigaLista,cBgColor);
		gtk_clist_set_foreground(GTK_CLIST(lst_ordini),nRigaLista,cFgColor);
		nRigaLista++;
	}

#ifdef TRACE
		trace_out_vstr_date(1, "RefreshListaOrdini: FINE COLORAZIONE");
#endif
	
/*******************************************************
* FINE VERSIONE CON GESTIONE PDF
********************************************************/
}


int GetOrdineIndex(char *pszOrdine)
{
	int nIndex=0;
	char *pszText;

	while(gtk_clist_get_text(GTK_CLIST(lst_ordini),nIndex,Cfg.nOrdiniKeyField,&pszText)){
		if(!strcmp(pszOrdine,pszText)){
			return(nIndex);
		}
		nIndex++;
	}
	return(-1);
}


/*
* void EditOrdine(char *szOrdineKey)
*	visualizza i dati dell'ordine;
* effettua la ricerca nell'archivio storico se la variabile globale bStrorico=TRUE
* altrimenti negli archivi di produzione
*/
void EditOrdine(char *szOrdineKey)
{
	GtkWidget *sw_edit_ordine=get_widget(dlg_edit_ordine,"sw_edit_ordine");
	GtkWidget *lst;
	PGresult *PGRes = NULL;
	char szParagraph[128];
	char szSelectCmd[2048];
	char szStato[128];
	char szBuffer[128];
	int nTuples;
	int nColli=0;
	int nRighe=0;
	int nCopie=0;

	/*
	* Cerco l'ordine specificato nella tabella ordini ricevuti
	*/
	if (!bStorico){
		PGRes=PGExecSQL(FALSE,"select rostato,ronmcll,ronmrgh,ronmcpe,ordprog,rodscli,rodteva,rotmeva,ropspre,ropsrea from ric_ord where ordprog='%s';",szOrdineKey);
	} else {
		PGRes=PGExecSQL(FALSE,"select rostato,ronmcll,ronmrgh,ronmcpe,ordprog,rodscli,rodteva,rotmeva,ropspre,ropsrea from ric_stor where ordprog='%s';",szOrdineKey);
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
		if (!bStorico){
			PGResElab=PGExecSQL(FALSE,"select * from ord_prod where ordprog='%s';",szOrdineKey);
		} else {
			PGResElab=PGExecSQL(FALSE,"select * from ord_stor where ordprog='%s';",szOrdineKey);
		}
		if(PQntuples(PGResElab)==0){
			/*
			* Ordine ricevuto ma non ancora elaborato -> visualizzo la lista delle righe
			*/
			strcpy(szParagraph,"Edit Righe Ordine");
			/*
			* disattivo il pulsante di edit dei colli
			*/
			gtk_widget_set_sensitive(get_widget(dlg_edit_ordine,"pb_edit_collo"),FALSE);
		} else {
			/*
			* Ordine in produzione -> visualizzo ls lista dei colli
			*/
			strcpy(szParagraph,"Edit Colli Ordine");
			gtk_widget_set_sensitive(get_widget(dlg_edit_ordine,"pb_edit_collo"),TRUE);
		}
		PQclear(PGResElab);
	}

	PQclear(PGRes);

	/*
	* lettura della select dal file di configurazione
	*/
	if (!bStorico){
		xncGetFileString(szParagraph,"SELECT_CMD", "", szSelectCmd, sizeof(szSelectCmd),Cfg.szCniCfg,NULL); 
	} else {
		xncGetFileString(szParagraph,"SELECT_CMD_STOR", "", szSelectCmd, sizeof(szSelectCmd),Cfg.szCniCfg,NULL); 
	}

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
	GtkWidget *lst=get_widget(dlg_distribuzione,"clist_distribuzione");
	PGresult *PGRes;
	PGresult *PGResOrd;
	char *RecordData[1][MAX_FIELDS_NUMBER];
	int nIndex;
	int nOrdiniTotali=0;
	int nOrdiniLanciati=0;
	int nOrdiniEvasi=0;
	int nOrdiniPdf=0;
	int nOrdiniIncompleti=0;
	int nFlgPz=0;

	gtk_clist_clear(GTK_CLIST(lst));

	for(nIndex=0;nIndex<20;nIndex++){
		RecordData[0][nIndex]=malloc(100);
	}
	PGRes=PGExecSQL(Cfg.nDebugVersion>1,"select ric_ord.ronelab,count(ric_ord.ronelab),sum(ric_ord.ronmcll),sum(ric_ord.ronmprs),sum(ric_ord.ronmrgh),sum(ric_ord.ronmcpe),sum(ric_ord.ropspre), max(ric_ord.roflgpz), max(ric_ord.roflamz) from ric_ord  group by ric_ord.ronelab order by ric_ord.ronelab;");
	for(nIndex=0;nIndex<PQntuples(PGRes);nIndex++){
		/* ordini totali */
		nOrdiniTotali=atoi(PQgetvalue(PGRes,nIndex,1));
		nFlgPz=atoi(PQgetvalue(PGRes,nIndex,7));
		
		/* ordini con PDF */
		nOrdiniPdf=0;
		PGResOrd=PGExecSQL(Cfg.nDebugVersion>1,"select count(ric_ord.ordprog) from ric_ord where ronelab='%s' and roflpdf in ('%c','%c');",PQgetvalue(PGRes,nIndex,0),ORDINE_CON_FATTURA,ORDINE_CON_DDT);
		if(PQntuples(PGResOrd)){
			nOrdiniPdf=atoi(PQgetvalue(PGResOrd,0,0));
		}
		PQclear(PGResOrd);

		/* ordini lanciati */
		nOrdiniLanciati=0;
		PGResOrd=PGExecSQL(Cfg.nDebugVersion>1,"select count(ric_ord.ordprog) from ric_ord where ronelab='%s' and rostato!='%c';",PQgetvalue(PGRes,nIndex,0),ORDINE_RICEVUTO);
		if(PQntuples(PGResOrd)){
			nOrdiniLanciati=atoi(PQgetvalue(PGResOrd,0,0));
		}
		PQclear(PGResOrd);

		/* ordini incompleti */
		nOrdiniIncompleti=0;
		PGResOrd=PGExecSQL(Cfg.nDebugVersion>1,"select count(ric_ord.ordprog) from ric_ord where ronelab='%s' and rostato='%c';",PQgetvalue(PGRes,nIndex,0),ORDINE_INCOMPLETO_TERMINATO);
		if(PQntuples(PGResOrd)){
			nOrdiniIncompleti=atoi(PQgetvalue(PGResOrd,0,0));
		}
		PQclear(PGResOrd);

		/* ordini evasi */
		nOrdiniEvasi=0;
		PGResOrd=PGExecSQL(Cfg.nDebugVersion>1,"select count(ric_ord.ordprog) from ric_ord where ronelab='%s' and (rostato='%c' or rostato='%c');",PQgetvalue(PGRes,nIndex,0),ORDINE_EVASO,ORDINE_SPEDITO_HOST);
		if(PQntuples(PGResOrd)){
			nOrdiniEvasi=atoi(PQgetvalue(PGResOrd,0,0));
		}
		PQclear(PGResOrd);

		strcpy(RecordData[0][0],PQgetvalue(PGRes,nIndex,0));
		sprintf(RecordData[0][1],"%5d",nOrdiniTotali);
		sprintf(RecordData[0][2],"%5d",nOrdiniPdf);
		sprintf(RecordData[0][3],"%5d",nOrdiniLanciati);
		sprintf(RecordData[0][4],"%5d",nOrdiniIncompleti);
		sprintf(RecordData[0][5],"%5d",nOrdiniEvasi);
		sprintf(RecordData[0][6],"%5d",nOrdiniTotali-nOrdiniEvasi);
		sprintf(RecordData[0][7],"%5d",atoi(PQgetvalue(PGRes,nIndex,2)));
		sprintf(RecordData[0][8],"%5d",atoi(PQgetvalue(PGRes,nIndex,3)));
		sprintf(RecordData[0][9],"%5d",atoi(PQgetvalue(PGRes,nIndex,4)));
		sprintf(RecordData[0][10],"%5d",atoi(PQgetvalue(PGRes,nIndex,5)));
		sprintf(RecordData[0][11],"%12.3f",(float)((float)atoi(PQgetvalue(PGRes,nIndex,6))/(float)1000));
		sprintf(RecordData[0][12],"%5d",nFlgPz);
		sprintf(RecordData[0][13],"%5d",atoi(PQgetvalue(PGRes,nIndex,8)));

		gtk_clist_append(GTK_CLIST(lst), RecordData[0]);

	}
	PQclear(PGRes);

	gtk_clist_set_column_min_width(GTK_CLIST(lst), 0,	150);
	gtk_clist_set_column_width(GTK_CLIST(lst), 0,	150);

	for(nIndex=0;nIndex<20;nIndex++){
		free(RecordData[0][nIndex]);
	}
}

/*
* BOOL EditCollo(char *szOrdineKey,int nNumCollo)
*	visualizza i dato del collo selezionato nella dlg_edit_ordine
* se la variabile globale bStorico=TRUE effettua la ricerca negli archivi di storico;
* altrimenti negli archivi di produzione
*/
BOOL EditCollo(char *szOrdineKey,int nNumCollo)
{
	char *pszCDERR;
	GdkColor *cColor;
	char szParagraph[128];
	char szSelectCmd[2048];
	char szListName[128];
	char szStato[128];
	char szBuffer[128];
	GtkWidget *sw_edit_collo=get_widget(dlg_edit_collo,"sw_edit_collo");
	GtkWidget *lst;
	PGresult *PGRes = NULL;
	int nTuples;
	int nIndex;
	BOOL bErrore;

	/*
	* Cerco i dati del collo specificato
	*/
	if (!bStorico){
		PGRes=PGExecSQL(FALSE,"select cpstato,ordkey,cpnmcol,cptpfor,cpdteva,cptmeva,cppspre,cppsrea from col_prod where ordkey='%s' AND cpnmcol=%d;",szOrdineKey,nNumCollo);
	} else {
		PGRes=PGExecSQL(FALSE,"select cpstato,ordkey,cpnmcol,cptpfor,cpdteva,cptmeva,cppspre,cppsrea from col_stor where ordkey='%s' AND cpnmcol=%d;",szOrdineKey,nNumCollo);
	}

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
		PQclear(PGRes);
	} else {
		PQclear(PGRes);
		return FALSE;
	}

	/*
	* lettura della select corrispondente dal file di configurazione
	*/
	if (!bStorico){
		xncGetFileString(szParagraph,"SELECT_CMD",  "", szSelectCmd, sizeof(szSelectCmd),Cfg.szCniCfg,NULL); 
	} else {
		xncGetFileString(szParagraph,"SELECT_CMD_STOR",  "", szSelectCmd, sizeof(szSelectCmd),Cfg.szCniCfg,NULL); 
	}

	strcpy(szListName,"lst_righe_collo");
	if((lst=get_widget(dlg_edit_collo,szListName))){
		gtk_widget_destroy(lst);
	}
	/*
	* Creazione clist widget su select
	*/
	if(-1 != CreateListFromSelect(dlg_edit_collo,sw_edit_collo,&lst,szListName,GTK_SELECTION_SINGLE,szSelectCmd,szOrdineKey,nNumCollo)){

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

		for(nIndex=0;nIndex<(GTK_CLIST(lst)->rows);nIndex++){
			bErrore=TRUE;
			gtk_clist_get_text(GTK_CLIST(lst),nIndex,6,&pszCDERR);
			if(pszCDERR){
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
		case ORDINE_INCOMPLETO_IN_PRELIEVO:
			strcpy(pszBuffer,"INC IN P");
		break;
		case ORDINE_INCOMPLETO_TERMINATO:
			strcpy(pszBuffer,"INC TERM");
		break;
		case ORDINE_INCOMPLETO_TERMINATO_IN_PRELIEVO:
			strcpy(pszBuffer,"INC TERM IN P");
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
		case ORDINE_SPEDITO_HOST:
			strcpy(pszBuffer,"SPEDITO A HOST");
		break;
		default:
			strcpy(pszBuffer,"NON DEFINITO");
		break;
	}
	return pszBuffer;
}

int GetNumColliOrdProd(char *szOrdKey)
{
	PGresult *PGRes;
	int nTuple;
	int nColli=0;

	PGRes=PGExecSQL(Cfg.nDebugVersion>1,"select opnmcll from ord_prod where ordkey='%s';",szOrdKey);
	nTuple=PQntuples(PGRes);
	if(nTuple){
		nColli = atoi(PQgetvalue(PGRes,0,0));
	}
	PQclear(PGRes);

	return nColli;
}

int GetAmazonFlagFromRicOrd(char *szOrdKey)
{
	PGresult *PGRes;
	int nTuple;
	int nAmazon;
	/*
	* Setto lo stato a un default
	*/
	nAmazon=0;

	/*
	* Verifico l'esistenza dell'ordine nel db, il tipo di doc da stampare (F,D)
	*/
	PGRes=PGExecSQL(Cfg.nDebugVersion>1,"select roflamz from ric_ord where ordprog in (select ordprog from ord_prod where ordkey='%s');",szOrdKey);
	nTuple=PQntuples(PGRes);
	if(nTuple){
		nAmazon = atoi(PQgetvalue(PGRes,0,0));
	}else{
#ifdef TRACE
		trace_out_vstr_date(1, "L'ordine [%s] non e' presente nella tabella ord_prod",szOrdKey);
#endif
	}
	PQclear(PGRes);

	return nAmazon;
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
	* Verifico l'esistenza dell'ordine nel db, il tipo di doc da stampare (F,D)
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


void UpdateOrdine(char *szOrdineKey)
{
	char szParagraph[128];
	char szUpdateCmd[2048];
	GtkWidget *lst_ordini=get_widget(main_window,"lst_ordini");

#ifdef TRACE
	trace_out_vstr_date(1,"UpdateOrdine()");
#endif

	strcpy(szParagraph,"Tabella Ordini");
	/*
	* lettura della select corrispondente dal file di configurazione
	*/
	xncGetFileString(szParagraph,"UPDATE_CMD",  "", szUpdateCmd, sizeof(szUpdateCmd),Cfg.szCniCfg,NULL); 

	UpdateListFromSelect(lst_ordini, szUpdateCmd, szOrdineKey, Cfg.nOrdiniKeyField);

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
	char szStatusText[256];
	char szPdfStatusText[256];
	char szFtzStatusText[256];
	PGresult *PGRes = NULL;
	char szText[256];
	char cStato;
	char cPdfStato;
	char cFtzStato;
	GdkColor *cBgColor;
	GdkColor *cFgColor;
	
	strcpy(szSqlTmp,szSqlCmd);
	strcpy(szCmd,szSqlCmd);

	/*
	* Sostituisco i valori parametrici
	* %ORDPROG%   : Chiave Ordine
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
					int nFType=PQftype(PGRes,nFieldIndex);
#ifdef TRACE
					//trace_out_vstr_date(Cfg.nDebugVersion>3,"Ordine %s con PQftype %d \n",szKey,nFType);
#endif
					/*
					* LG: ricavo anche i valori di stato e pdf
					*/
					if (Cfg.nOrdiniStatusField==nFieldIndex) {
						sprintf(szStatusText,"%s",PQgetvalue(PGRes,0,nFieldIndex));
					}
					if (Cfg.nOrdiniPdfStatusField==nFieldIndex) {
						sprintf(szPdfStatusText,"%s",PQgetvalue(PGRes,0,nFieldIndex));
					}
					if (Cfg.nOrdiniFtzStatusField==nFieldIndex) {
						sprintf(szFtzStatusText,"%s",PQgetvalue(PGRes,0,nFieldIndex));
					}
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

				cStato=szStatusText[0];
				cPdfStato=szPdfStatusText[0];
				cFtzStato=szFtzStatusText[0];
				cFgColor=&cBLACK;
				cBgColor=&cWHITE;



/*******************************************************
* VERSIONE CON GESTIONE PDF
********************************************************/
				/*
				* Aggiungo la colorazione dell'item della lista in base allo stato
				* dell'ordine e del flag pdf
				*/
				switch(cStato){
					case ORDINE_RICEVUTO:
					break;
					case ORDINE_SPEDITO:
					break;
					case ORDINE_ELABORATO:
					break;
					case ORDINE_STAMPATA_RAC:
					break;
					case ORDINE_IN_PRELIEVO:
					break;
					case ORDINE_INCOMPLETO_IN_PRELIEVO:
					break;
					case ORDINE_INCOMPLETO_TERMINATO:
					case ORDINE_INCOMPLETO_TERMINATO_SPEDITO:
					{
						switch(cPdfStato){
							case ORDINE_CON_FATTURA:
							case ORDINE_CON_DDT:
								cFgColor=&cLGREEN;
							break;
							default:
								cFgColor=&cLRED;
							break;
						}
					}
					break;
					case ORDINE_INCOMPLETO_TERMINATO_IN_PRELIEVO:
					break;
					case ORDINE_PRELEVATO:
					break;
					case ORDINE_EVASO:
					break;
					case ORDINE_STAMPATA_XAB:
					break;
					case ORDINE_STAMPATA_DIST:
					break;
					case ORDINE_SPEDITO_HOST:
					break;
					default:
					break;
				}
				switch(cFtzStato){
					case ORDINE_FITTIZIO:
						cBgColor=&cLGREY;
					break;
					default:
					break;
				}

				gtk_clist_set_background(GTK_CLIST(clist),nIndex,cBgColor);
				gtk_clist_set_foreground(GTK_CLIST(clist),nIndex,cFgColor);
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
		sprintf(szColumnName, "%s", gtk_clist_get_column_title(GTK_CLIST(*clist), nColumnIndex));
		nColumnWidth=10*strlen(StrTrimAll(szColumnName));
		gtk_clist_set_column_min_width(GTK_CLIST(*clist), nColumnIndex,	nColumnWidth);
		gtk_clist_set_column_width(GTK_CLIST(*clist), nColumnIndex,	nColumnWidth);
		//gtk_clist_set_column_width(GTK_CLIST(*clist), nColumnIndex, gtk_clist_optimal_column_width(GTK_CLIST(*clist),nColumnIndex));
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

BOOL DatiDaRicevere(void)
{
	char szOrdini[128];
	char szRighe[128];
	char szArticoli[128];
	char szUbicazioni[128];

	char szOrdiniBertello[128];
	char szColliBertello[128];
	char szRigheBertello[128];

	char szOrdiniBertelloB[128];
	char szRigheBertelloB[128];
	char szOrdiniBertelloR[128];
	char szRigheBertelloR[128];

	sprintf(szOrdini,"%s/%s",Cfg.szPathExport,StrTrimAll(Cfg.szImpOrdiniFile));
	sprintf(szRighe,"%s/%s",Cfg.szPathExport,StrTrimAll(Cfg.szImpRigheFile));
	sprintf(szArticoli,"%s/%s",Cfg.szPathExport,StrTrimAll(Cfg.szImpArticoliFile));
	sprintf(szUbicazioni,"%s/%s",Cfg.szPathExport,StrTrimAll(Cfg.szImpUbicazioniFile));

	sprintf(szOrdiniBertello,   "%s/%s", Cfg.szPathExport, StrTrimAll(Cfg.szImpOrdiniBertello));
	sprintf(szColliBertello,    "%s/%s", Cfg.szPathExport, StrTrimAll(Cfg.szImpColliBertello));
	sprintf(szRigheBertello,    "%s/%s", Cfg.szPathExport, StrTrimAll(Cfg.szImpRigheBertello));

	sprintf(szOrdiniBertelloB,   "%s/%s", Cfg.szPathExport, StrTrimAll(Cfg.szImpOrdiniBertelloB));
	sprintf(szRigheBertelloB,    "%s/%s", Cfg.szPathExport, StrTrimAll(Cfg.szImpRigheBertelloB));
	sprintf(szOrdiniBertelloR,   "%s/%s", Cfg.szPathExport, StrTrimAll(Cfg.szImpOrdiniBertelloR));
	sprintf(szRigheBertelloR,    "%s/%s", Cfg.szPathExport, StrTrimAll(Cfg.szImpRigheBertelloR));

	return (
		FileExists(szOrdini) || FileExists(szRighe) || FileExists(szArticoli) || FileExists(szUbicazioni) ||
     	FileExists(szOrdiniBertello) || FileExists(szColliBertello) || FileExists(szRigheBertello) ||
	 	FileExists(szOrdiniBertelloB) || FileExists(szRigheBertelloB) ||
	 	FileExists(szOrdiniBertelloR) || FileExists(szRigheBertelloR)
	);
}

/*
* void RicezioneCedola()
*/
void RicezioneCedola(void)
{
	FILE *fp;
	char szCommand[256];
	GtkWidget *lb_1;
	GtkWidget *lb_2;
	GtkWidget *pr_testate;
	GtkWidget *pr_righe;
	GtkWidget *pr_articoli;
	GtkWidget *pr_ubicazioni;
	GtkWidget *rl_testate;
	GtkWidget *rl_righe;
	GtkWidget *rl_articoli;
	GtkWidget *rl_ubicazioni;
	GtkWidget *lb_testate;
	GtkWidget *lb_righe;
	GtkWidget *lb_articoli;
	GtkWidget *lb_ubicazioni;

	GtkWidget *pr[11];
	GtkWidget *rl[11];
	GtkWidget *lb[11];
	char *pszFiles[11];
	DBSTRUCT db[11];
	PGresult *PGRes;
	PGresult *PGResNMCPE;
	PGresult *PGResOrdini;
	int nTuples;
	int nRighe;
	int nCopie;
	int nPreseProdotto;
	int nCopieProdotto;
	int nPrese;
	int nPeso;
	int nIndex=0;
	int nRigaIndex=0;
	char szBuffer[128];
	char szOrdProg[128];
	char szOrdini[128];
	char szRighe[128];
	char szArticoli[128];
	char szUbicazioni[128];
	char szOrdiniBertello[128];
	char szColliBertello[128];
	char szRigheBertello[128];

	char szOrdiniBertelloB[128];
	char szRigheBertelloB[128];
	char szOrdiniBertelloR[128];
	char szRigheBertelloR[128];

	lb_1=get_widget(dlg_import,"lb_1");
	lb_2=get_widget(dlg_import,"lb_2");

	pr_testate    = get_widget(dlg_import,"pr_testate");
	pr_righe      = get_widget(dlg_import,"pr_righe");
	pr_articoli   = get_widget(dlg_import,"pr_catalogo");
	pr_ubicazioni = get_widget(dlg_import,"pr_ubicazioni");

	rl_testate    = get_widget(dlg_import,"rl_testate");
	rl_righe      = get_widget(dlg_import,"rl_righe");
	rl_articoli   = get_widget(dlg_import,"rl_catalogo");
	rl_ubicazioni = get_widget(dlg_import,"rl_ubicazioni");

	lb_testate    = get_widget(dlg_import,"lb_testate");
	lb_righe      = get_widget(dlg_import,"lb_righe");
	lb_articoli   = get_widget(dlg_import,"lb_catalogo");
	lb_ubicazioni = get_widget(dlg_import,"lb_ubicazioni");

	sprintf(szOrdiniBertello,   "%s/%s", Cfg.szPathExport, StrTrimAll(Cfg.szImpOrdiniBertello));
	sprintf(szColliBertello,    "%s/%s", Cfg.szPathExport, StrTrimAll(Cfg.szImpColliBertello));
	sprintf(szRigheBertello,    "%s/%s", Cfg.szPathExport, StrTrimAll(Cfg.szImpRigheBertello));
	sprintf(szOrdiniBertelloB,  "%s/%s", Cfg.szPathExport, StrTrimAll(Cfg.szImpOrdiniBertelloB));
	sprintf(szRigheBertelloB,   "%s/%s", Cfg.szPathExport, StrTrimAll(Cfg.szImpRigheBertelloB));
	sprintf(szOrdiniBertelloR,  "%s/%s", Cfg.szPathExport, StrTrimAll(Cfg.szImpOrdiniBertelloR));
	sprintf(szRigheBertelloR,   "%s/%s", Cfg.szPathExport, StrTrimAll(Cfg.szImpRigheBertelloR));

	sprintf(szOrdini,           "%s/%s", Cfg.szPathExport, StrTrimAll(Cfg.szImpOrdiniFile));
	sprintf(szRighe,            "%s/%s", Cfg.szPathExport, StrTrimAll(Cfg.szImpRigheFile));
	sprintf(szArticoli,         "%s/%s", Cfg.szPathExport, StrTrimAll(Cfg.szImpArticoliFile));
	sprintf(szUbicazioni,       "%s/%s", Cfg.szPathExport, StrTrimAll(Cfg.szImpUbicazioniFile));

	rl[0]=rl_testate;
	rl[1]=rl_righe;
	rl[2]=rl_articoli;
	rl[3]=rl_ubicazioni;
	rl[4]=NULL;
	rl[5]=NULL;
	rl[6]=NULL;
	rl[7]=NULL;
	rl[8]=NULL;
	rl[9]=NULL;
	rl[10]=NULL;

	pr[0]=pr_testate;
	pr[1]=pr_righe;
	pr[2]=pr_articoli;
	pr[3]=pr_ubicazioni;
	pr[4]=NULL;
	pr[5]=NULL;
	pr[6]=NULL;
	pr[7]=NULL;
	pr[8]=NULL;
	pr[9]=NULL;
	pr[10]=NULL;

	lb[0]=lb_testate;
	lb[1]=lb_righe;
	lb[2]=lb_articoli;
	lb[3]=lb_ubicazioni;
	lb[4]=NULL;
	lb[5]=NULL;
	lb[6]=NULL;
	lb[7]=NULL;
	lb[8]=NULL;
	lb[9]=NULL;
	lb[10]=NULL;

	pszFiles[0]=szOrdini;
	pszFiles[1]=szRighe;
	pszFiles[2]=szArticoli;
	pszFiles[3]=szUbicazioni;
	pszFiles[4]=szOrdiniBertello;
	pszFiles[5]=szColliBertello;
	pszFiles[6]=szRigheBertello;
	pszFiles[7]=szOrdiniBertelloB;
	pszFiles[8]=szRigheBertelloB;
	pszFiles[9]=szOrdiniBertelloR;
	pszFiles[10]=szRigheBertelloR;

	db[0]=tRicOrd;
	db[1]=tRicArt;
	db[2]=tArticoli;
	db[3]=tUbicazioni;
	db[4]=tOrdiniBertello;
	db[5]=tColliBertello;
	db[6]=tRigheBertello;
	db[7]=tOrdiniBertelloB;
	db[8]=tRigheBertelloB;
	db[9]=tOrdiniBertelloR;
	db[10]=tRigheBertelloR;
	
	gtk_label_printf(lb_1,"Ricezione Dati in corso");
	gtk_label_printf(lb_2,"Attendere la fine della procedura");

	gtk_widget_set_sensitive(get_widget(dlg_import,"pb_ok"),FALSE);
	gtk_widget_set_sensitive(get_widget(dlg_import,"pb_cancel"),FALSE);

	PGRes=PGExecSQL(Cfg.nDebugVersion,"BEGIN WORK;");
	PQclear(PGRes);

	for(nIndex=0; nIndex<11; nIndex++){
		if((fp=fopen(pszFiles[nIndex],"r"))!=NULL){
			int nValue=0;
			char szBuffer[1024];
			BOOL bUpdate;

			switch(nIndex){
				case 0:
					/* Ordini - Inserimento SENZA Update */
					bUpdate=FALSE;
				break;
				case 1:
					/* Righe -  Inserimento SENZA Update */
					bUpdate=FALSE;
				break;
				case 2:
					/* Articoli -  Inserimento CON update */
					bUpdate=TRUE;
				break;
				case 3:
					/* Ubicazioni -  Inserimento CON update */
					bUpdate=TRUE;
					/*
					* st - 03/12/2001
					* gestione SAP: su richiesta di Warnier Gatto pulisco i codici delle ubicazioni,
					* lasciando soltanto gli imballi
					*/
					if(Cfg.nSAP){
#if 0
						PGresult *PGClearUbi;

						PGClearUbi=PGExecSQL(Cfg.nDebugVersion,"update ubicazioni set codprod='' where codprod !~* 'H00';");
						if(PQresultStatus(PGClearUbi)!=PGRES_COMMAND_OK){
							gtk_text_printf(NULL,txt_msgs,"Rilevato errore in aggiornamento ubicazioni\nContattare l'assistenza\n");
#ifdef TRACE
							trace_out_vstr_date(1,"Rilevato errore in aggiornamento ubicazioni");
#endif
						}
						PQclear(PGClearUbi);
#endif
					}
				break;

				case 4:
					/* Ordini Bertello - Inserimento SENZA Update */
					bUpdate=FALSE;
				break;
				case 5:
					/* Colli Bertello - Inserimento SENZA Update */
					bUpdate=FALSE;
				break;
				case 6:
					/* Righe Bertello - Inserimento SENZA Update */
					bUpdate=FALSE;
				break;
				case 7:
				case 8:
				case 9:
				case 10:
					/* Ordini Bertello B/R - Inserimento SENZA Update */
					/* Righe Bertello B/R - Inserimento SENZA Update */
					bUpdate=FALSE;
				break;
			}

#ifdef TRACE
			trace_out_vstr_date(1,"RicezioneCedola() : Lanciata Ricezione di %s",pszFiles[nIndex]);
#endif

			while(fgets(szBuffer,sizeof(szBuffer)-1,fp)){
				if(UpdateAsciiRecord(szBuffer,&db[nIndex],bUpdate)){
					nValue++;
				} else {
#ifdef TRACE
					trace_out_vstr_date(1,"RicezioneCedola() : errore in UpdateAsciiRecord %s : [%s]",pszFiles[nIndex], szBuffer);
#endif
				}
				if(!(nValue%10)){
					if (pr[nIndex]) gtk_progress_set_value(GTK_PROGRESS(pr[nIndex]),(gfloat)nValue);
					gtk_update();
				}
				memset(szBuffer,0,sizeof(szBuffer));
			}
			fclose(fp);

			if (pr[nIndex]) gtk_progress_set_value(GTK_PROGRESS(pr[nIndex]),(gfloat)nValue);
			/*
			* da verificare
			*/
			if (rl[nIndex]) gtk_ruler_set_range(GTK_RULER(rl[nIndex]),(gfloat)0,(gfloat)nValue,(gfloat)0,(gfloat)nValue);
			
			gtk_update();
		} else {
			if(lb[nIndex]) gtk_label_printf(lb[nIndex],"File [%s] non presente",pszFiles[nIndex]);
		}
	}

	/* ordini bertello */
	PGRes=PGExecSQL(Cfg.nDebugVersion,"update ordini_bertello set rostato='%c' where rostato is null;", ORDINE_ELABORATO);
	PQclear(PGRes);
	/* colli bertello */
	PGRes=PGExecSQL(Cfg.nDebugVersion,"update colli_bertello set cpstato='%c' where cpprgcl is null;", COLLO_ELABORATO);
	PGRes=PGExecSQL(Cfg.nDebugVersion,"update colli_bertello set cpprgcl = nextval('progcollo') where cpprgcl is null;");
	PQclear(PGRes);
	/* righe bertello */
	PGRes=PGExecSQL(Cfg.nDebugVersion,"update righe_bertello set rpstato='%c' where rpstato is null;", RIGA_ELABORATA);
	PQclear(PGRes);


	/* ordini bertello B/R */
	PGRes=PGExecSQL(Cfg.nDebugVersion,"update ordini_bertello_b set rostato='%c', rotmrcz='now' where rostato is null;", ORDINE_RICEVUTO);
	PQclear(PGRes);
	PGRes=PGExecSQL(Cfg.nDebugVersion,"update ordini_bertello_r set rostato='%c', rotmrcz='now' where rostato is null;", ORDINE_RICEVUTO);
	PQclear(PGRes);
	/* righe bertello B/R */
	PGRes=PGExecSQL(Cfg.nDebugVersion,"update righe_bertello_b set rpstato='%c', rptmrcz='now' where rpstato is null;", RIGA_RICEVUTA);
	PQclear(PGRes);
	PGRes=PGExecSQL(Cfg.nDebugVersion,"update righe_bertello_r set rpstato='%c', rptmrcz='now' where rpstato is null;", RIGA_RICEVUTA);
	PQclear(PGRes);


	PGRes=PGExecSQL(Cfg.nDebugVersion,"COMMIT WORK;");
	PQclear(PGRes);

	PGRes=PGExecSQL(Cfg.nDebugVersion,"BEGIN WORK;");
	PQclear(PGRes);
	/*
	* Ricalcolo numero righe, numero copie e peso dell'ordine ricevuto
	*/
	PGResOrdini=PGExecSQL(FALSE,"select ordprog,ronmcpe,ronmrgh from ric_ord where ronmcpe is NULL AND ronmrgh is NULL;");
	nTuples=PQntuples(PGResOrdini);

	for(nIndex=0;nIndex<nTuples;nIndex++){
		strcpy(szOrdProg,PQgetvalue(PGResOrdini,nIndex,0));
		PGResNMCPE=PGExecSQL(Cfg.nDebugVersion,"select r.raqtord,a.prpesgr,a.prqtprs from ric_art as r, articoli as a where r.racdpro=a.prcdpro AND ordprog='%s';",szOrdProg);
		nRighe=PQntuples(PGResNMCPE);
		nCopie=0;
		nPrese=0;
		nPeso=0;
		for(nRigaIndex=0;nRigaIndex<nRighe;nRigaIndex++){
			nCopie+=atoi(PQgetvalue(PGResNMCPE,nRigaIndex,0));
			nCopieProdotto=atoi(PQgetvalue(PGResNMCPE,nRigaIndex,0));
			nPreseProdotto=atoi(PQgetvalue(PGResNMCPE,nRigaIndex,2));
			if(nPreseProdotto){
				nPrese+=((nCopieProdotto/nPreseProdotto)+(nCopieProdotto%nPreseProdotto?1:0));
			}
			nPeso+=atoi(PQgetvalue(PGResNMCPE,nRigaIndex,0))*atoi(PQgetvalue(PGResNMCPE,nRigaIndex,1));
		}
		PQclear(PGResNMCPE);
		PGResNMCPE=PGExecSQL(Cfg.nDebugVersion,"update ric_ord set ronmcpe=%d,ronmrgh=%d,ronmprs=%d,ropspre=%d,rostato=' ' where ordprog='%s';",nCopie,nRighe,nPrese,nPeso,szOrdProg);
		PQclear(PGResNMCPE);
	}
	PQclear(PGResOrdini);

/*******************************************************
* VERSIONE CON GESTIONE PDF
********************************************************/
	
	/*
	* Cerco gli ordini fittizi e li marco.
	*/
	PGResOrdini=PGExecSQL(FALSE,"select ordprog from ric_ord where ordprog not in (select distinct ordprog from ric_art);");
	nTuples=PQntuples(PGResOrdini);

	for(nIndex=0;nIndex<nTuples;nIndex++){
		strcpy(szOrdProg,PQgetvalue(PGResOrdini,nIndex,0));
		/*
		* Setto gli ordini fittizi, dovranno solo essere stampati
		*/
		PGResNMCPE=PGExecSQL(Cfg.nDebugVersion,"update ric_ord set roflftz='%c' where ordprog='%s';",ORDINE_FITTIZIO,szOrdProg);
#ifdef TRACE
		trace_out_vstr_date(1,"RicezioneCedola() : Ordine [%s] fittizio",szOrdProg);
#endif
		PQclear(PGResNMCPE);
	}
	PQclear(PGResOrdini);

	
/*******************************************************
* FINE VERSIONE CON GESTIONE PDF
********************************************************/

	/*
	* Stampa etichette bertello
	*/
	PGResOrdini=PGExecSQL(Cfg.nDebugVersion,"select ordprog from ordini_bertello where rostato='%c' order by ordprog;",ORDINE_ELABORATO);
	nTuples=PQntuples(PGResOrdini);
	for(nIndex=0;nIndex<nTuples;nIndex++){
		strcpy(szOrdProg,PQgetvalue(PGResOrdini,nIndex,0));

		if(OrdineBertelloConOrdiniDiLinea(szOrdProg)){
#ifdef TRACE
			trace_out_vstr_date(1,"ordine bertello con colli di linea (%s)",szOrdProg);
#endif
			/* se ordine bertello CON ordini di linea allora segue il normale giro ordini */
			/* cambio stato ordini bertello */
			PGRes=PGExecSQL(Cfg.nDebugVersion,"update ordini_bertello set rostato='%c' where ordprog='%s';", ORDINE_STAMPATA_RAC,szOrdProg);
			PQclear(PGRes);
		}
	}

	
	PGRes=PGExecSQL(Cfg.nDebugVersion,"COMMIT WORK;");
	PQclear(PGRes);

	gtk_update();


	/* rinomino il file di import ordini */
	sprintf(szCommand,"mv -f %s %s.old",szOrdini,szOrdini);
	system(szCommand);
	/* rinomino il file di import articoli */
	sprintf(szCommand,"mv -f %s %s.old",szArticoli,szArticoli);
	system(szCommand);
	/* rinomino il file di import mappa */
	sprintf(szCommand,"mv -f %s %s.old",szUbicazioni,szUbicazioni);
	system(szCommand);
	/* rinomino il file di import ordini bertello */
	sprintf(szCommand,"mv -f %s %s.old",szOrdiniBertello,szOrdiniBertello);
	system(szCommand);
	/* rinomino il file di import colli bertello */
	sprintf(szCommand,"mv -f %s %s.old",szColliBertello,szColliBertello);
	system(szCommand);
	/* rinomino il file di import righe bertello */
	sprintf(szCommand,"mv -f %s %s.old",szRigheBertello,szRigheBertello);
	system(szCommand);

	/* rinomino il file di import ordini bertello nuovi */
	sprintf(szCommand,"mv -f %s %s.old",szOrdiniBertelloB,szOrdiniBertelloB);
	system(szCommand);
	/* rinomino il file di import righe bertello nuovi */
	sprintf(szCommand,"mv -f %s %s.old",szRigheBertelloB,szRigheBertelloB);
	system(szCommand);
	/* rinomino il file di import ordini bertello nuovi */
	sprintf(szCommand,"mv -f %s %s.old",szOrdiniBertelloR,szOrdiniBertelloR);
	system(szCommand);
	/* rinomino il file di import righe bertello nuovi */
	sprintf(szCommand,"mv -f %s %s.old",szRigheBertelloR,szRigheBertelloR);
	system(szCommand);


	/*
	* st 18-12-2000
	* controllo che nel file mappa-ubicazioni non siano presenti codici prodotto duplicati
	*/
	if (CheckUbicazioni()==FALSE){
		dlg_message=create_dlg_message();
		gtk_label_printf(get_widget(dlg_message,"lb_msg"),"ATTENZIONE!! SONO PRESENTI PRODOTTI DUPLICATI NELLA MAPPA UBICAZIONI.\nControllare e ricevere nuovamente da Host il file %s",szUbicazioni);
		gtk_window_set_title (GTK_WINDOW (dlg_message), "Ricezione Dati");
		gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
		gtk_signal_connect (GTK_OBJECT (get_widget(dlg_message,"pb_ok")), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_pb_ok_clicked), dlg_message);
		gtk_widget_show(dlg_message);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(dlg_import));
	}
	gtk_label_printf(lb_1,"Procedura di ricezione dati terminata");
	gtk_label_printf(lb_2,"Premere Cancel per uscire");
	gtk_widget_set_sensitive(get_widget(dlg_import,"pb_cancel"),TRUE);
	gtk_window_set_focus(GTK_WINDOW (dlg_import), get_widget(dlg_import,"pb_cancel"));
	gtk_update();
	gtk_signal_connect (GTK_OBJECT (get_widget(dlg_import,"pb_cancel")), "clicked", GTK_SIGNAL_FUNC (on_dlg_import_pb_cancel_clicked), dlg_import);
}

void SpedizioneCedola(char *szAperturaFile)
{
	GtkWidget *pr_ordini;
	GtkWidget *pr_colli;
	GtkWidget *rl_ordini;
	GtkWidget *rl_colli;
	GtkWidget *lb_ordini;
	GtkWidget *lb_colli;
	GtkWidget *lb_1;
	GtkWidget *lb_2;

	char szCommand[256];
	char szOrdiniFile[128];
	char szColliFile[128];
	char szOrdprog[128];
	char szROFLFTZ[128];

	int nNumeroOrdini=0;
	int nNumeroColli=0;
	int nIndexColli;
	int nIndexOrdini;
	int nColliSpediti;

	char szSQLCmdOrdini[1024];
	char szSQLCmdColli[1024];
	char szSQLCmdUpdate[1024];
	
	PGresult *PGResOrdini;
	PGresult *PGResColli;
	PGresult *PGResUpdate;
	PGresult *PGRes;
	PGresult *PGResFittizio;

	FILE *fpOrdini=NULL;
	FILE *fpColli=NULL;

	/* rm 03-12-2003 : modifica per test di evasione rif. Leali */
	BOOL bCambiaStato=FALSE;

	sprintf(szOrdiniFile,"%s/%s",Cfg.szPathExport,StrTrimAll(Cfg.szExpOrdiniFile));
	sprintf(szColliFile,"%s/%s",Cfg.szPathExport,StrTrimAll(Cfg.szExpColliFile));

	if((fpOrdini=fopen(szOrdiniFile,szAperturaFile))==NULL){
#ifdef TRACE
		trace_out_vstr_date(1,"SpedizioneCedola() : Errore in apertura file ordini [%s]",szOrdiniFile);
#endif
		gtk_text_printf("CYAN",txt_msgs,"SpedizioneCedola() : Errore in apertura file ordini [%s]\n",szOrdiniFile);
		return;
	}
	if((fpColli =fopen(szColliFile,szAperturaFile))==NULL){
#ifdef TRACE
		trace_out_vstr_date(1,"SpedizioneCedola() : Errore in apertura file colli [%s]",szColliFile);
#endif
		gtk_text_printf("CYAN",txt_msgs,"SpedizioneCedola() : Errore in apertura file colli [%s]\n",szColliFile);
		return;
	}

	lb_1=get_widget(dlg_export,"lb_1");
	lb_2=get_widget(dlg_export,"lb_2");

	pr_ordini = get_widget(dlg_export,"pr_ordini");
	pr_colli  = get_widget(dlg_export,"pr_colli");

	rl_ordini = get_widget(dlg_export,"rl_ordini");
	rl_colli  = get_widget(dlg_export,"rl_colli");

	lb_ordini = get_widget(dlg_export,"lb_ordini");
	lb_colli  = get_widget(dlg_export,"lb_colli");

	lb_1=get_widget(dlg_export,"lb_1");
	lb_2=get_widget(dlg_export,"lb_2");

	gtk_label_printf(lb_1,"Spedizione Dati in corso");
	gtk_label_printf(lb_2,"Attendere la fine della procedura");
	gtk_widget_set_sensitive(get_widget(dlg_export,"pb_ok"),FALSE);
	gtk_widget_set_sensitive(get_widget(dlg_export,"pb_cancel"),FALSE);
	gtk_update();



	/*
	* seleziono gli ordini in stato E, per ogni ordine spedisco prima i colli
	* poi l'ordine. Gli ordini fittizi, wella non li vuole, quindi li metto in stato
	* H senza metterli nel file
	* ATTENZIONE !!! Lasciare la query così com'è ! 4 campi !!! (vedi WriteAsciiRecord()...)
	*/
	if(bCambiaStato){
		sprintf(szSQLCmdOrdini,"select ordprog,ronmcll,ropsrea,ronelab from ric_ord where rostato='%c' order by ronelab,ordprog;",ORDINE_EVASO);	
		PGResOrdini=PGExecSQL(FALSE,szSQLCmdOrdini);
	} else {
		sprintf(szSQLCmdOrdini,"select ordprog,ronmcll,ropsrea,ronelab from ric_ord where rostato in ('%c','%c','%c','%c','%c','%c','%c','%c') order by ronelab,ordprog;",
			ORDINE_ELABORATO,
			ORDINE_STAMPATA_RAC,
			ORDINE_IN_PRELIEVO,
			ORDINE_INCOMPLETO_IN_PRELIEVO,
			ORDINE_INCOMPLETO_TERMINATO,
			ORDINE_INCOMPLETO_TERMINATO_SPEDITO,
			ORDINE_INCOMPLETO_TERMINATO_IN_PRELIEVO,
			ORDINE_PRELEVATO);
		PGResOrdini=PGExecSQL(FALSE,szSQLCmdOrdini);
	}

	if((nNumeroOrdini=PQntuples(PGResOrdini))){

		PGRes=PGExecSQL(Cfg.nDebugVersion,"BEGIN WORK;");
		PQclear(PGRes);

#ifdef TRACE
		trace_out_vstr_date(1,"SpedizioneCedola() : Lanciata Spedizione Dati (%d:Ordini) a Host",nNumeroOrdini);
#endif

		nColliSpediti=0;
		for (nIndexOrdini=0;nIndexOrdini<nNumeroOrdini;nIndexOrdini++){
			/*
			* selezioni i colli relativi all'ordine
			*/
			strcpy(szOrdprog,PQgetvalue(PGResOrdini,nIndexOrdini,0));

			// Stato fittizio dell'ordine
			PGResFittizio=PGExecSQL(FALSE,"select roflftz from ric_ord where ordprog='%s';",szOrdprog);
			if(PQresultStatus(PGResFittizio)==PGRES_COMMAND_OK){
				strcpy(szROFLFTZ,PQgetvalue(PGResFittizio,0,0));
			} else {
				/* errore */
				strcpy(szROFLFTZ," ");
			}
			PQclear(PGResFittizio);

			sprintf(szSQLCmdColli,"select ordprog,cptpfor,cpprgcl from col_prod where ordprog='%s' order by ordkey,cpnmcol;",szOrdprog);	
			PGResColli=PGExecSQL(FALSE,szSQLCmdColli);

			if ((nNumeroColli=PQntuples(PGResColli))){
				for (nIndexColli=0;nIndexColli<nNumeroColli;nIndexColli++){
					/*
					* spedizione a Host dei colli relativi all'ordine
					*/
					if (szROFLFTZ[0]==ORDINE_FITTIZIO){
					} else {
						WriteAsciiRecord(fpColli, &tColCni,PGResColli,nIndexColli);
					}
					gtk_progress_set_value(GTK_PROGRESS(pr_colli),(gfloat)nColliSpediti);
					gtk_update();
					nColliSpediti++;
				}

				/*
				* cambio lo stato dei colli relativi all'ordine(E -> H)
				*/
				if(bCambiaStato){
					sprintf(szSQLCmdUpdate,"update col_prod set cpstato='%c' where ordprog='%s';", COLLO_SPEDITO_HOST, szOrdprog);
					PGResUpdate=PGExecSQL(Cfg.nDebugVersion,szSQLCmdUpdate); PQclear(PGResUpdate);
				}

				/*
				* cambio lo stato delle righe relative all'ordine(E -> H)
				*/
				if(bCambiaStato){
					sprintf(szSQLCmdUpdate,"update rig_prod set rpstato='%c' where ordprog='%s';", ORDINE_SPEDITO_HOST, szOrdprog);
					PGResUpdate=PGExecSQL(Cfg.nDebugVersion,szSQLCmdUpdate); PQclear(PGResUpdate);
				}

			}

			PQclear(PGResColli);

			/*
			* spedizione dell'ordine a Host
			*/
			if (szROFLFTZ[0]==ORDINE_FITTIZIO){
			} else {
				WriteAsciiRecord(fpOrdini, &tOrdCni,PGResOrdini,nIndexOrdini);
			}

			/*
			* cambio lo stato dell'ordine ricevuto (E -> H)
			*/
			if(bCambiaStato){
				sprintf(szSQLCmdUpdate,"update ric_ord set rostato='%c' where ordprog='%s';", ORDINE_SPEDITO_HOST, szOrdprog);
				PGResUpdate=PGExecSQL(Cfg.nDebugVersion,szSQLCmdUpdate);PQclear(PGResUpdate);
				/*
				* cambio lo stato dell'ordine di produzione (E -> H)
				*/
				sprintf(szSQLCmdUpdate,"update ord_prod set opstato='%c' where ordprog='%s';", ORDINE_SPEDITO_HOST, szOrdprog);
				PGResUpdate=PGExecSQL(Cfg.nDebugVersion,szSQLCmdUpdate);PQclear(PGResUpdate);
			}

			UpdateOrdine(szOrdprog);

			gtk_progress_set_value(GTK_PROGRESS(pr_ordini),(gfloat)nIndexOrdini);
			gtk_update();
		}
		gtk_progress_set_value(GTK_PROGRESS(pr_ordini),(gfloat)nIndexOrdini);
		gtk_update();

		gtk_progress_set_value(GTK_PROGRESS(pr_colli),(gfloat)nColliSpediti);
		gtk_update();

#ifdef TRACE
		trace_out_vstr_date(1,"Spediti a Host %d ordini",nNumeroOrdini);
#endif

		PGRes=PGExecSQL(Cfg.nDebugVersion,"COMMIT WORK;");
		PQclear(PGRes);
	}

	PQclear(PGResOrdini);

	if(fpColli) fclose(fpColli);
	if(fpOrdini) fclose(fpOrdini);

	/*
	* st - 27-03-2001 mi salvo le 2 spedizioni precedenti
	*/
	sprintf(szCommand,"mv -f %s.pre %s.pre_pre",szOrdiniFile,szOrdiniFile);
	system(szCommand);
	sprintf(szCommand,"mv -f %s.pre %s.pre_pre",szColliFile,szColliFile);
	system(szCommand);

	sprintf(szCommand,"mv -f %s.old %s.pre",szOrdiniFile,szOrdiniFile);
	system(szCommand);
	sprintf(szCommand,"mv -f %s.old %s.pre",szColliFile,szColliFile);
	system(szCommand);

	/* 
	* 23-03-2001 rm : copia di sicurezza 
	*/	
	sprintf(szCommand,"cp %s %s.old",szOrdiniFile,szOrdiniFile);
	system(szCommand);

	sprintf(szCommand,"cp %s %s.old",szColliFile,szColliFile);
	system(szCommand);

	gtk_label_printf(lb_1,"Procedura di spedizione dati terminata");
	gtk_label_printf(lb_2,"Premere Cancel per uscire");
	gtk_widget_set_sensitive(get_widget(dlg_export,"pb_cancel"),TRUE);
	gtk_update();

	/*
	* Spedire messaggio a peso per il refresh della lista ??
	*/
	gtk_signal_connect (GTK_OBJECT(get_widget(dlg_export,"pb_cancel")), "clicked", GTK_SIGNAL_FUNC (on_dlg_export_pb_cancel_clicked), dlg_export);
}


BOOL OrdineBertelloConOrdiniDiLinea(char *szOrdProg)
{
	PGresult *PGRes;
	BOOL rc=FALSE;

	PGRes=PGExecSQL(Cfg.nDebugVersion,"select ordprog from ric_ord where ordprog='%s';",szOrdProg);
	if(PQntuples(PGRes)){
		rc = TRUE;
	}
	PQclear(PGRes);

	return rc;
}

/*
* Stampa etichette bertello
* rm 02-07-2022 - solo degli ordini bertello senza ordini di linea
*/
BOOL StampaOrdiniBertelloSenzaOrdinidiLinea(void)
{
	PGresult *PGResOrdini;
	PGresult *PGResUpdate;
	int nTuples;
	int nIndex;
	char szOrdProg[64];
	char szBuffer[256];
	BOOL rc=FALSE;

	PGResOrdini=PGExecSQL(Cfg.nDebugVersion,"select ordprog from ordini_bertello where rostato='%c' order by ordprog;",ORDINE_ELABORATO);
	nTuples=PQntuples(PGResOrdini);
	for(nIndex=0;nIndex<nTuples;nIndex++){
		strcpy(szOrdProg,PQgetvalue(PGResOrdini,nIndex,0));

		if(!OrdineBertelloConOrdiniDiLinea(szOrdProg)){
			/* ordine bertello senza colli di linea - lo stampo */
			sprintf(szBuffer,"%s,%s",szOrdProg,Cfg.szLabelPrinterIsola1);
#ifdef TRACE
			trace_out_vstr_date(1,"print rac amazon bertello SENZA colli di linea (%s)",szBuffer);
#endif
		 	SendMessage(PROC_PRINTLABEL,PROC_MAIN,PRINT_RAC_AMAZON_BERTELLO,szBuffer);
			PGResUpdate=PGExecSQL(Cfg.nDebugVersion,"update ordini_bertello set rostato='%c' where ordprog='%s';", ORDINE_STAMPATA_RAC,szOrdProg);
			PQclear(PGResUpdate);
			rc=TRUE;
		}
	}
	PQclear(PGResOrdini);

	return rc;
}

BOOL LanciaOrdine(char *pszOrdineKey,char *szStato)
{
	BOOL bRetValueRicOrd=0;
	BOOL bRetValueOrdProd;
	PGresult *PGRes;
	PGresult *PGResBertello;
	int nProgressivoLancio=0;
	int nProgressivoPrecedenza=0;
	int nAmazon=0;
	int nIncompleto=0;
	int nBertello=0;
	int nLanciaOrdine=0;


	/*
	* rm 02-07-2022 10:28:40 : gestione ordini bertello senza ordine di linea associato
	* si tratta di ordini contenenti solo colli bertello / colli interi
	*/
	StampaOrdiniBertelloSenzaOrdinidiLinea();


	/*
	* Ricavo il progressivo lancio
	* LG: col nuovo progressivo precedenza avra' un minor impatto sui lanci!!!
	*/
	PGRes=PGExecSQL(Cfg.nDebugVersion>2,"select roprgln, roprgpr, roflamz, roflinc from ric_ord where ordprog='%s';",pszOrdineKey);

	if(PQntuples(PGRes)){
		nProgressivoLancio=atoi(PQgetvalue(PGRes,0,0));
		nProgressivoPrecedenza=atoi(PQgetvalue(PGRes,0,1));
		if (nProgressivoLancio==0){
			/*
			* Il progressivo non e' mai stato registrato e quindi si tratta della prima volta
			*/
			Cfg.nProgressivoLancio++;
			nProgressivoLancio=Cfg.nProgressivoLancio;

			xncPutFileInt(ProcList[PROC_MAIN].szProcName,"ProgressivoLancio",Cfg.nProgressivoLancio,Cfg.szCniCfg,NULL);
		}
		
	}
	PQclear(PGRes);

	PGRes=PGExecSQL(Cfg.nDebugVersion>2,"select roflamz, roflinc from ric_ord where ordprog='%s';",pszOrdineKey);
	if(PQntuples(PGRes)){
		nAmazon=atoi(PQgetvalue(PGRes,0,0));
		nIncompleto=atoi(PQgetvalue(PGRes,0,1));

		if(nAmazon && nIncompleto){
			PGResBertello=PGExecSQL(Cfg.nDebugVersion>2,"select ordprog from ordini_bertello where ordprog='%s';",pszOrdineKey);
			nBertello = PQntuples(PGResBertello);
			PQclear(PGResBertello);
		}
	}
	PQclear(PGRes);

	if(nAmazon && nIncompleto && nBertello<=0){
		GtkWidget *dlg_msg;
		int nRC;
		char szBuffer[128];

		sprintf(szBuffer,"Ordine Amazon [%s] Incompleto, lancio comunque ?", pszOrdineKey);

		dlg_msg = gtk_message_dialog_new (GTK_WINDOW(main_window),0,GTK_MESSAGE_QUESTION,GTK_BUTTONS_YES_NO, "%s", szBuffer);
		gtk_window_set_title (GTK_WINDOW (dlg_msg), "Ordine Amazon incompleto");
		nRC = gtk_dialog_run (GTK_DIALOG (dlg_msg));
		switch(nRC){
			case GTK_RESPONSE_YES:
				nLanciaOrdine=1;
			break;
		}
		gtk_widget_destroy(dlg_msg);
	}


	if(nAmazon && nIncompleto && nBertello<=0 && nLanciaOrdine==0){
		gtk_text_printf(NULL,txt_msgs,"Ordine amazon incompleto [%s] non lanciato\n",pszOrdineKey);
#ifdef TRACE
		trace_out_vstr_date(1,"Ordine amazon incompleto [%s] non lanciato",pszOrdineKey);
#endif
	} else {
		PGRes=PGExecSQL(Cfg.nDebugVersion,"update ric_ord set rostato='%s',roprgln=%d where ordprog='%s';",szStato,nProgressivoLancio,pszOrdineKey);
		bRetValueRicOrd = (PQresultStatus(PGRes)==PGRES_COMMAND_OK);
		PQclear(PGRes);

		/*
		*	Attenzione, devo stare attento a non cambiare lo stato di ordini gia' evasi o in prelievo
		* Se ho due sottoordini rischio che per cambiare il primo vado a cambiare anche il secondo
		* che magari e' gia' stato finito o e' normalmente in prelievo!!!
		*/
		/*
		* Richiesta Fabio
		* Si vuole che gli ordini lanciati abbiano precedenza rispetto a quelli lanciati successivamente
		* anche se il progressivo lancio e' piu' basso. Infatti sorgeva il problema di elaborazioni
		* lanciate per i colli con pdf che sorpassavano quelle in linea con progressivo piu' alto
		* Aggiungo alla tabella ord_prod un progressivo precedenza assegnato al lancio da parte dell'
		* operatore, non come l'altro progressivo che se esiste gia' non si aggiorna..
		*/
		switch(szStato[0]){
			case ORDINE_INCOMPLETO_TERMINATO_SPEDITO:
			{
				PGRes=PGExecSQL(Cfg.nDebugVersion,"update ord_prod set opstato='%s',opprgpr=%d where ordprog='%s' and opstato='%c';",szStato,nProgressivoPrecedenza,pszOrdineKey,ORDINE_INCOMPLETO_TERMINATO);
				bRetValueOrdProd = (PQresultStatus(PGRes)==PGRES_COMMAND_OK);
				PQclear(PGRes);
			}
			break;
			default:
			break;
		}
	}
	return (bRetValueRicOrd);
}


/*
* rm : start/stop differenziato per isola 
*/
void StartLinea(int nIsola)
{
	PGresult *PGRes;
	char szBuffer[128];


	switch(nIsola){
		case 0:
			SendMessage(PROC_SETTORI, PROC_MAIN, START_LINEA, "1");
			SendMessage(PROC_SETTORI, PROC_MAIN, START_LINEA, "2");

			/*
			* Metto lo stato in wait in attesa che settori metta in start
			*/

			gtk_label_printf(get_widget(main_window,"lb_stato_linea_1"),"WAIT");
			ChangeTextColor(get_widget(main_window,"lb_stato_linea_1"),&cORANGE);
			//set_pixmap(get_widget(main_window,"pixmap_stato_linea_1"),"led_orange_on.xpm");
			gtk_image_set_from_stock(GTK_IMAGE(get_widget(main_window,"pixmap_stato_linea_1")), "gtk-refresh" ,GTK_ICON_SIZE_BUTTON);

			gtk_label_printf(get_widget(main_window,"lb_stato_linea_2"),"WAIT");
			ChangeTextColor(get_widget(main_window,"lb_stato_linea_2"),&cORANGE);
			//set_pixmap(get_widget(main_window,"pixmap_stato_linea_2"),"led_orange_on.xpm");
			gtk_image_set_from_stock(GTK_IMAGE(get_widget(main_window,"pixmap_stato_linea_2")), "gtk-refresh" ,GTK_ICON_SIZE_BUTTON);

			PGRes=PGExecSQL(Cfg.nDebugVersion,"insert into eventi values ('START','now',%d);",1);
			PQclear(PGRes);

			PGRes=PGExecSQL(Cfg.nDebugVersion,"insert into eventi values ('START','now',%d);",2);
			PQclear(PGRes);
		break;
		case 1:
		case 2:
			sprintf(szBuffer,"%d",nIsola);
			SendMessage(PROC_SETTORI, PROC_MAIN, START_LINEA, szBuffer);

			/*
			* Metto lo stato in wait in attesa che settori metta in start
			*/
			sprintf(szBuffer,"lb_stato_linea_%d",nIsola);
			gtk_label_printf(get_widget(main_window,szBuffer),"WAIT");
			ChangeTextColor(get_widget(main_window,szBuffer),&cORANGE);

			sprintf(szBuffer,"pixmap_stato_linea_%d",nIsola);
			//set_pixmap(get_widget(main_window,szBuffer),"led_orange_on.xpm");
			gtk_image_set_from_stock(GTK_IMAGE(get_widget(main_window,szBuffer)), "gtk-refresh" ,GTK_ICON_SIZE_BUTTON);

			PGRes=PGExecSQL(Cfg.nDebugVersion,"insert into eventi values ('START','now',%d);",nIsola);
			PQclear(PGRes);
		break;
	}
}

void StopLinea(int nIsola)
{
	PGresult *PGRes;
	char szBuffer[128];


	switch(nIsola){
		case 0:
			SendMessage(PROC_SETTORI, PROC_MAIN, STOP_LINEA, "1");
			SendMessage(PROC_SETTORI, PROC_MAIN, STOP_LINEA, "2");

			PGRes=PGExecSQL(Cfg.nDebugVersion,"insert into eventi values ('STOP','now',%d);",1);
			PQclear(PGRes);

			PGRes=PGExecSQL(Cfg.nDebugVersion,"insert into eventi values ('STOP','now',%d);",2);
			PQclear(PGRes);
		break;
		case 1:
		case 2:
			sprintf(szBuffer,"%d",nIsola);
			SendMessage(PROC_SETTORI, PROC_MAIN, STOP_LINEA, szBuffer);

			PGRes=PGExecSQL(Cfg.nDebugVersion,"insert into eventi values ('STOP','now',%d);",nIsola);
			PQclear(PGRes);
		break;
	}

}


void RicezioneOrdini(void)
{
	FILE *fp;
	char szCommand[256];
	GtkWidget *lb_1;
	GtkWidget *lb_2;
	GtkWidget *pr_testate;
	GtkWidget *pr_righe;
	GtkWidget *rl_testate;
	GtkWidget *rl_righe;
	GtkWidget *lb_testate;
	GtkWidget *lb_righe;

	GtkWidget *pr[2];
	GtkWidget *rl[2];
	GtkWidget *lb[2];
	char *pszFiles[2];
	DBSTRUCT db[2];
	PGresult *PGRes;
	PGresult *PGResNMCPE;
	PGresult *PGResOrdini;
	int nTuples;
	int nRighe;
	int nPreseProdotto;
	int nCopieProdotto;
	int nPrese;
	int nCopie;
	int nPeso;
	char szOrdProg[128];
	char szOrdini[128];
	char szRighe[128];
	int nIndex=0;
	int nRigaIndex=0;

	lb_1=get_widget(dlg_import_ordini,"lb_1");
	lb_2=get_widget(dlg_import_ordini,"lb_2");

	pr_testate    = get_widget(dlg_import_ordini,"pr_testate");
	pr_righe      = get_widget(dlg_import_ordini,"pr_righe");
	rl_testate    = get_widget(dlg_import_ordini,"rl_testate");
	rl_righe      = get_widget(dlg_import_ordini,"rl_righe");
	lb_testate    = get_widget(dlg_import_ordini,"lb_testate");
	lb_righe      = get_widget(dlg_import_ordini,"lb_righe");

	sprintf(szOrdini,    "%s/%s", Cfg.szPathExport, StrTrimAll(Cfg.szImpOrdiniFile));
	sprintf(szRighe,     "%s/%s", Cfg.szPathExport, StrTrimAll(Cfg.szImpRigheFile));

	rl[0]=rl_testate;
	rl[1]=rl_righe;
	pr[0]=pr_testate;
	pr[1]=pr_righe;
	lb[0]=lb_testate;
	lb[1]=lb_righe;

	pszFiles[0]=szOrdini;
	pszFiles[1]=szRighe;

	db[0]=tRicOrd;
	db[1]=tRicArt;
	
	gtk_label_printf(lb_1,"Ricezione Dati in corso");
	gtk_label_printf(lb_2,"Attendere la fine della procedura");
	gtk_widget_set_sensitive(get_widget(dlg_import_ordini,"pb_ok"),FALSE);
	gtk_widget_set_sensitive(get_widget(dlg_import_ordini,"pb_cancel"),FALSE);
	gtk_update();

	PGRes=PGExecSQL(Cfg.nDebugVersion,"BEGIN WORK;");
	PQclear(PGRes);

	for(nIndex=0; nIndex<2; nIndex++){
		if((fp=fopen(pszFiles[nIndex],"r"))!=NULL){
			int nValue=0;
			char szBuffer[1024];

#ifdef TRACE
			trace_out_vstr_date(1,"RicezioneOrdini() : Lanciata Ricezione di %s",pszFiles[nIndex]);
#endif

			/* Ordini e Righe - Inserimento SENZA Update */
			while(fgets(szBuffer,sizeof(szBuffer)-1,fp)){
				if(UpdateAsciiRecord(szBuffer,&db[nIndex],FALSE)){
					nValue++;
				}
				gtk_progress_set_value(GTK_PROGRESS(pr[nIndex]),(gfloat)nValue);
				gtk_update();
				memset(szBuffer,0,sizeof(szBuffer));
			}
			fclose(fp);

			/*
			gtk_progress_set_value(GTK_PROGRESS(pr[nIndex]),(gfloat)nValue);
			gtk_update();
			*/

		} else {
			gtk_label_printf(lb[nIndex],"File [%s] non presente",pszFiles[nIndex]);
		}
	}

	PGRes=PGExecSQL(Cfg.nDebugVersion,"COMMIT WORK;");
	PQclear(PGRes);

	PGRes=PGExecSQL(Cfg.nDebugVersion,"BEGIN WORK;");
	PQclear(PGRes);
	/*
	* Ricalcolo numero righe, numero copie e peso dell'ordine ricevuto
	*/
	PGResOrdini=PGExecSQL(Cfg.nDebugVersion,"select ordprog,ronmcpe,ronmrgh from ric_ord where ronmcpe is NULL AND ronmrgh is NULL;");
	nTuples=PQntuples(PGResOrdini);

	for(nIndex=0;nIndex<nTuples;nIndex++){
		strcpy(szOrdProg,PQgetvalue(PGRes,nIndex,0));
		PGResNMCPE=PGExecSQL(Cfg.nDebugVersion,"select r.raqtord,a.prpesgr,a.prqtprs from ric_art as r, articoli as a where r.racdpro=a.prcdpro AND ordprog='%s';",szOrdProg);
		nRighe=PQntuples(PGResNMCPE);
		nCopie=0;
		nPrese=0;
		nPeso=0;
		for(nRigaIndex=0;nRigaIndex<nRighe;nRigaIndex++){
			nCopie+=atoi(PQgetvalue(PGResNMCPE,nRigaIndex,0));
			nCopieProdotto=atoi(PQgetvalue(PGResNMCPE,nRigaIndex,0));
			nPreseProdotto=atoi(PQgetvalue(PGResNMCPE,nRigaIndex,2));
			if(nPreseProdotto){
				nPrese+=((nCopieProdotto/nPreseProdotto)+(nCopieProdotto%nPreseProdotto?1:0));
			}
			nPeso+=atoi(PQgetvalue(PGResNMCPE,nRigaIndex,0))*atoi(PQgetvalue(PGResNMCPE,nRigaIndex,1));
		}
		PQclear(PGResNMCPE);
		PGResNMCPE=PGExecSQL(Cfg.nDebugVersion,"update ric_ord set ronmcpe=%d,ronmrgh=%d,ronmprs=%d,ropspre=%d,rostato=' ' where ordprog='%s';",nCopie,nRighe,nPrese,nPeso,szOrdProg);
		PQclear(PGResNMCPE);
	}
	PQclear(PGResOrdini);

	PGRes=PGExecSQL(Cfg.nDebugVersion,"COMMIT WORK;");
	PQclear(PGRes);

/*******************************************************
* VERSIONE CON GESTIONE PDF
********************************************************/
	
	/*
	* Cerco gli ordini fittizi e li marco.
	*/
	PGResOrdini=PGExecSQL(Cfg.nDebugVersion,"select ordprog from ric_ord where ordprog not in (select distinct ordprog from ric_art);");
	nTuples=PQntuples(PGResOrdini);

	for(nIndex=0;nIndex<nTuples;nIndex++){
		strcpy(szOrdProg,PQgetvalue(PGResOrdini,nIndex,0));
		/*
		* Setto gli ordini fittizi, dovranno solo essere stampati
		*/
		PGResNMCPE=PGExecSQL(Cfg.nDebugVersion,"update ric_ord set roflftz='%c' where ordprog='%s';",ORDINE_FITTIZIO,szOrdProg);
#ifdef TRACE
		trace_out_vstr_date(1,"RicezioneCedola() : Ordine [%s] fittizio",szOrdProg);
#endif
		PQclear(PGResNMCPE);
	}
	PQclear(PGResOrdini);

	PGRes=PGExecSQL(Cfg.nDebugVersion,"COMMIT WORK;");
	PQclear(PGRes);

	gtk_update();
	
/*******************************************************
* FINE VERSIONE CON GESTIONE PDF
********************************************************/

	gtk_update();


	/* rinomino il file di import ordini */
	sprintf(szCommand,"mv -f %s %s.old",szOrdini,szOrdini);
	system(szCommand);

	gtk_label_printf(get_widget(dlg_import_ordini,"lb_testate"),"");
	gtk_label_printf(get_widget(dlg_import_ordini,"lb_righe"),"");
	gtk_label_printf(lb_1,"Procedura di ricezione dati terminata");
	gtk_label_printf(lb_2,"Premere Cancel per uscire");
	gtk_widget_set_sensitive(get_widget(dlg_import_ordini,"pb_cancel"),TRUE);
	gtk_window_set_focus(GTK_WINDOW (dlg_import_ordini), get_widget(dlg_import_ordini,"pb_cancel"));
	gtk_update();

	gtk_signal_connect (GTK_OBJECT(get_widget(dlg_import_ordini,"pb_cancel")), "clicked", GTK_SIGNAL_FUNC (on_dlg_import_ordini_pb_cancel_clicked), dlg_import_ordini);

}


void RicezioneArticoli(void)
{
	FILE *fp;
	char szCommand[256];
	GtkWidget *lb_1;
	GtkWidget *lb_2;
	GtkWidget *pr_articoli;
	GtkWidget *rl_articoli;
	GtkWidget *lb_articoli;

	DBSTRUCT dbArticoli;
	PGresult *PGRes;

	char szFileArticoli[128];

	lb_1=get_widget(dlg_import_file,"lb_1");
	lb_2=get_widget(dlg_import_file,"lb_2");

	pr_articoli   = get_widget(dlg_import_file,"pr_file");
	rl_articoli   = get_widget(dlg_import_file,"rl_file");
	lb_articoli   = get_widget(dlg_import_file,"lb_file");
	dbArticoli=tArticoli;

	sprintf(szFileArticoli,  "%s/%s", Cfg.szPathExport, StrTrimAll(Cfg.szImpArticoliFile));

	gtk_label_printf(lb_1,"Ricezione Articoli in corso");
	gtk_label_printf(lb_2,"Attendere la fine della procedura");

	gtk_widget_set_sensitive(get_widget(dlg_import_file,"pb_ok"),FALSE);
	gtk_widget_set_sensitive(get_widget(dlg_import_file,"pb_cancel"),FALSE);

	PGRes=PGExecSQL(Cfg.nDebugVersion,"BEGIN WORK;");
	PQclear(PGRes);

	if((fp=fopen(szFileArticoli,"r"))!=NULL){
		int nValue=0;
		char szBuffer[1024];

#ifdef TRACE
		trace_out_vstr_date(1,"RicezioneArticoli() : Lanciata Ricezione di %s",szFileArticoli);
#endif

		while(fgets(szBuffer,sizeof(szBuffer)-1,fp)){
			if(UpdateAsciiRecord(szBuffer,&dbArticoli,TRUE)){
				nValue++;
			}
			gtk_progress_set_value(GTK_PROGRESS(pr_articoli),(gfloat)nValue);
			gtk_update();
			memset(szBuffer,0,sizeof(szBuffer));
		}
		fclose(fp);

	} else {
		gtk_label_printf(lb_articoli,"File [%s] non presente",szFileArticoli);
	}

	PGRes=PGExecSQL(Cfg.nDebugVersion,"COMMIT WORK;");
	PQclear(PGRes);

	/* rinomino il file di import articoli */
	sprintf(szCommand,"mv -f %s %s.old",szFileArticoli,szFileArticoli);
	system(szCommand);

	gtk_label_printf(get_widget(dlg_import_file,"lb_file"),"");
	gtk_label_printf(lb_1,"Procedura di ricezione articoli terminata");
	gtk_label_printf(lb_2,"Premere Cancel per uscire");
	gtk_widget_set_sensitive(get_widget(dlg_import_file,"pb_cancel"),TRUE);
	gtk_window_set_focus(GTK_WINDOW (dlg_import_file), get_widget(dlg_import_file,"pb_cancel"));
	gtk_update();

	gtk_signal_connect (GTK_OBJECT (get_widget(dlg_import_file,"pb_cancel")), "clicked", GTK_SIGNAL_FUNC (on_dlg_import_file_pb_cancel_clicked), dlg_import_file);
}


void RicezioneUbicazioni(void)
{
	FILE *fp;
	char szCommand[256];
	GtkWidget *lb_1;
	GtkWidget *lb_2;
	GtkWidget *pr_ubicazioni;
	GtkWidget *rl_ubicazioni;
	GtkWidget *lb_ubicazioni;

	DBSTRUCT dbUbicazioni;
	PGresult *PGRes;

	char szFileUbicazioni[128];

	lb_1=get_widget(dlg_import_file,"lb_1");
	lb_2=get_widget(dlg_import_file,"lb_2");

	pr_ubicazioni = get_widget(dlg_import_file,"pr_file");
	rl_ubicazioni = get_widget(dlg_import_file,"rl_file");
	lb_ubicazioni = get_widget(dlg_import_file,"lb_file");
	dbUbicazioni=tUbicazioni;

	sprintf(szFileUbicazioni,  "%s/%s", Cfg.szPathExport, StrTrimAll(Cfg.szImpUbicazioniFile));

	gtk_label_printf(lb_1,"Ricezione Ubicazioni in corso");
	gtk_label_printf(lb_2,"Attendere la fine della procedura");

	gtk_widget_set_sensitive(get_widget(dlg_import_file,"pb_ok"),FALSE);
	gtk_widget_set_sensitive(get_widget(dlg_import_file,"pb_cancel"),FALSE);

	if((fp=fopen(szFileUbicazioni,"r"))!=NULL){
		int nValue=0;
		char szBuffer[1024];

		/*
		* st - 03/12/2001
		* gestione SAP: su richiesta di Warnier Gatto pulisco i codici delle ubicazioni,
		* lasciando soltanto gli imballi
		*/
		if(Cfg.nSAP){
			PGresult *PGClearUbi;

			PGClearUbi=PGExecSQL(Cfg.nDebugVersion,"update ubicazioni set codprod='' where codprod !~* 'H00';");
			if(PQresultStatus(PGClearUbi)!=PGRES_COMMAND_OK){
				gtk_text_printf(NULL,txt_msgs,"Rilevato errore in aggiornamento ubicazioni\nContattare l'assistenza\n");
#ifdef TRACE
				trace_out_vstr_date(1,"Rilevato errore in aggiornamento ubicazioni");
#endif
			}
			PQclear(PGClearUbi);
		}

		PGRes=PGExecSQL(Cfg.nDebugVersion,"BEGIN WORK;");
		PQclear(PGRes);

#ifdef TRACE
		trace_out_vstr_date(1,"RicezioneUbicazioni() : Lanciata Ricezione di %s",szFileUbicazioni);
#endif

		while(fgets(szBuffer,sizeof(szBuffer)-1,fp)){
			if(UpdateAsciiRecord(szBuffer,&dbUbicazioni,TRUE)){
				nValue++;
			}
			gtk_progress_set_value(GTK_PROGRESS(pr_ubicazioni),(gfloat)nValue);
			gtk_update();
			memset(szBuffer,0,sizeof(szBuffer));
		}
		fclose(fp);
		
		PGRes=PGExecSQL(Cfg.nDebugVersion,"COMMIT WORK;");
		PQclear(PGRes);

		/* rinomino il file di import ubicazioni */
		sprintf(szCommand,"mv -f %s %s.old",szFileUbicazioni,szFileUbicazioni);
		system(szCommand);

		/*
		* st 18-12-2000
		* controllo che nel file mappa-ubicazioni non siano presenti codici prodotto duplicati
		*/
		if (CheckUbicazioni()==FALSE){
			dlg_message=create_dlg_message();
			gtk_label_printf(get_widget(dlg_message,"lb_msg"),"ATTENZIONE!! SONO PRESENTI PRODOTTI DUPLICATI NELLA MAPPA UBICAZIONI.\nControllare e ricevere nuovamente da Host il file %s",szFileUbicazioni);
			gtk_window_set_title (GTK_WINDOW (dlg_message), "Ricezione Dati");
			gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
			gtk_signal_connect (GTK_OBJECT (get_widget(dlg_message,"pb_ok")), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_pb_ok_clicked), dlg_message);
			gtk_widget_show(dlg_message);
			gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(dlg_import_file));
		}
		gtk_label_printf(get_widget(dlg_import_file,"lb_file"),"");
		gtk_label_printf(lb_1,"Procedura di ricezione ubicazioni terminata");
		gtk_label_printf(lb_2,"Premere Cancel per uscire");
		gtk_widget_set_sensitive(get_widget(dlg_import_file,"pb_cancel"),TRUE);
		gtk_window_set_focus(GTK_WINDOW (dlg_import_file), get_widget(dlg_import_file,"pb_cancel"));
		gtk_update();

		gtk_signal_connect (GTK_OBJECT (get_widget(dlg_import_file,"pb_cancel")), "clicked", GTK_SIGNAL_FUNC (on_dlg_import_file_pb_cancel_clicked), dlg_import_file);

	} else {
		gtk_label_printf(lb_ubicazioni,"File [%s] non presente",szFileUbicazioni);
	}
}


void EditConfigurazione(void)
{	
	GtkWidget *lst;
	char szSelectCmd[1024];
	dlg_config=create_dlg_config();
	
	/*
	* GENERAL
	*/
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(get_widget(dlg_config,"sb_main_delay")),Cfg.nMainDelay);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(get_widget(dlg_config,"sb_max_colli")),Cfg.nMaxColliOrdine);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(get_widget(dlg_config,"sb_main_gestione_badge")),Cfg.nGestioneBadge);

	if (Cfg.nDebugVersion==1){
		printf("Attivo cb_debug\n");
		gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON(get_widget(dlg_config,"cb_debug")), TRUE);
	}
	if (Cfg.nAdvancedInvoicing==1){
		printf("Attivo cb_advanced_invoicing\n");
		gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON(get_widget(dlg_config,"cb_advanced_invoicing")), TRUE);
	}

	/*
	* STAMPA PACKING LIST
	*/
	if (Cfg.nStampaPL==STAMPA_AGHI){
		gtk_widget_activate(get_widget(dlg_config,"rb_stampa_aghi"));
#ifdef TRACE
		trace_out_vstr_date(1,"stampa ad aghi");
#endif
	} else if (Cfg.nStampaPL==STAMPA_LASER){
		gtk_widget_activate(get_widget(dlg_config,"rb_stampa_laser"));
#ifdef TRACE
		trace_out_vstr_date(1,"stampa laser");
#endif
	}
	/*
	* STAMPA RAC
	*/
	if (Cfg.nStampaRAC==DOPO_IL_LANCIO){
		gtk_widget_activate(get_widget(dlg_config,"rb_all_rac"));
	} else if (Cfg.nStampaRAC==INIZIO_PRELIEVO){
		gtk_widget_activate(get_widget(dlg_config,"rb_settori"));
	}
	/*
	* GESTIONE PRELIEVO
	*/
  if (Cfg.nChiusuraCollo==1){
		gtk_widget_activate(get_widget(dlg_config,"rb_gestione_collo"));
	} else {
		gtk_widget_activate(get_widget(dlg_config,"rb_gestione_ordine"));
	}
	/*
	* GESTIONE IMBALLI
	*/
	if (Cfg.nImballiUbicati==1){
		gtk_widget_activate(get_widget(dlg_config,"rb_imballi_ubicati"));
	} else {
		gtk_widget_activate(get_widget(dlg_config,"rb_imballi_non_ubicati"));
	}

	/*
	* GESTIONE ATTESA PDF
	*/
	if (Cfg.nAttesaPDFOrdineCompleto==1){
		gtk_widget_activate(get_widget(dlg_config,"rb_attesa_pdf_ordine_completo"));
	} else {
		gtk_widget_activate(get_widget(dlg_config,"rb_attesa_pdf_ultimo_collo"));
	}

	/*
	* DIRECTORY
	*/
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_config,"entry_pathdata")),   Cfg.szPathData);
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_config,"entry_pathexe")),    Cfg.szPathExe);
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_config,"entry_pathexport")), Cfg.szPathExport);

	/*
	* FILE
	*/
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_config,"entry_imp_ordini")),     Cfg.szImpOrdiniFile);
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_config,"entry_imp_righe")),      Cfg.szImpRigheFile);
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_config,"entry_imp_articoli")),   Cfg.szImpArticoliFile);
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_config,"entry_imp_ubicazioni")), Cfg.szImpUbicazioniFile);
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_config,"entry_exp_ordini")),     Cfg.szExpOrdiniFile);
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_config,"entry_exp_colli")),      Cfg.szExpColliFile);

	/*
	* DATABASE
	*/
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_config,"entry_pghost")),     Cfg.szPGHost);
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_config,"entry_pgport")),     Cfg.szPGPort);
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_config,"entry_pgdatabase")), Cfg.szPGDataBase);

	/*
	* STAMPANTI
	*/
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_config,"entry_printer_console")),        Cfg.szPrinterConsole);
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_config,"entry_label_printer_console")),  Cfg.szLabelPrinterConsole);
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_config,"entry_printer_isola1")),         Cfg.szPrinterIsola1);
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_config,"entry_label_printer_isola1")),   Cfg.szLabelPrinterIsola1);
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_config,"entry_printer_isola2")),         Cfg.szPrinterIsola2);
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_config,"entry_label_printer_isola2")),   Cfg.szLabelPrinterIsola2);
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_config,"entry_printer_bilancia")),       Cfg.szPrinterBilancia);
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_config,"entry_label_printer_bilancia")), Cfg.szLabelPrinterBilancia);

	/*
	* Lista Imballi
	*/
	if((lst=get_widget(dlg_config,"lst_imballi"))){
		gtk_widget_destroy(lst);
	}
	strcpy(szSelectCmd,"select CODICE, DESCRIZ, LUNGHEZZA, ALTEZZA, LARGHEZZA, TARA, PESOMAX, VOLPERC1, VOLPERC2, TPLAVOR from imballi order by codice;");
	CreateListFromSelect(dlg_config,get_widget(dlg_config,"sw_lista_imballi"),&lst,"lst_imballi",GTK_SELECTION_SINGLE,szSelectCmd,NULL,0);
	gtk_signal_connect (GTK_OBJECT (get_widget(dlg_config,"lst_imballi")), "select_row", GTK_SIGNAL_FUNC (on_lista_imballi_row_selected), 0);

	gtk_window_set_focus (GTK_WINDOW (dlg_config), get_widget(dlg_config,"pb_ok"));
	gtk_widget_show(dlg_config);
	gtk_window_set_transient_for(GTK_WINDOW(dlg_config),GTK_WINDOW(main_window));

	/*
	* Varie
	*/
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(get_widget(dlg_config,"sb_main_soglia_barcode_id")),Cfg.nSogliaControlloBarcode);
}

void ApplyConfigurazione(void)
{
	/*
	* GENERAL
	*/
	Cfg.nMainDelay = gtk_spin_button_get_value_as_int((GtkSpinButton *)get_widget(dlg_config,"sb_main_delay"));
	Cfg.nMaxColliOrdine = gtk_spin_button_get_value_as_int((GtkSpinButton *)get_widget(dlg_config,"sb_max_colli"));
	Cfg.nGestioneBadge = gtk_spin_button_get_value_as_int((GtkSpinButton *)get_widget(dlg_config,"sb_main_gestione_badge"));

	if (GTK_TOGGLE_BUTTON(get_widget(dlg_config,"cb_debug"))->active){
		Cfg.nDebugVersion=1;
	} else {
		Cfg.nDebugVersion=0;
	}
	if (GTK_TOGGLE_BUTTON(get_widget(dlg_config,"cb_advanced_invoicing"))->active){
		Cfg.nAdvancedInvoicing=1;
	} else {
		Cfg.nAdvancedInvoicing=0;
	}
	
	/*
	* GESTIONE COLLO
	*/
  if (GTK_TOGGLE_BUTTON(get_widget(dlg_config,"rb_gestione_collo"))->active){
		Cfg.nChiusuraCollo=1;
	} else {
		Cfg.nChiusuraCollo=0;
	}

	/*
	* IMBALLI UBICATI
	*/
  if (GTK_TOGGLE_BUTTON(get_widget(dlg_config,"rb_imballi_ubicati"))->active){
		Cfg.nImballiUbicati=1;
	} else {
		Cfg.nImballiUbicati=0;
	}

	/*
	* GESTIONE ATTESA PDF
	*/
  if (GTK_TOGGLE_BUTTON(get_widget(dlg_config,"rb_attesa_pdf_ordine_completo"))->active){
		Cfg.nAttesaPDFOrdineCompleto=1;
	} else {
		Cfg.nAttesaPDFOrdineCompleto=0;
	}

	/*
	* STAMPA PL
	*/
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(get_widget(dlg_config,"rb_stampa_aghi")))){
#ifdef TRACE
		trace_out_vstr_date(1,"stampa ad aghi");
#endif
		Cfg.nStampaPL=STAMPA_AGHI;
	} else {
#ifdef TRACE
		trace_out_vstr_date(1,"stampa laser");
#endif
		Cfg.nStampaPL=STAMPA_LASER;
	}
	
	/*
	* STAMPA RAC
	*/
  if (GTK_TOGGLE_BUTTON(get_widget(dlg_config,"rb_all_rac"))->active){
		Cfg.nStampaRAC=DOPO_IL_LANCIO;
	} else {
		Cfg.nStampaRAC=INIZIO_PRELIEVO;
	}


	/*
	* DIRECTORY
	*/
	strcpy(Cfg.szPathData,          gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_pathdata"))));
	strcpy(Cfg.szPathExe,           gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_pathexe"))));
	strcpy(Cfg.szPathExport,        gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_pathexport"))));


	/*
	* FILE
	*/
	strcpy(Cfg.szImpOrdiniFile,     gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_imp_ordini"))));
	strcpy(Cfg.szImpRigheFile,      gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_imp_righe"))));
	strcpy(Cfg.szImpArticoliFile,   gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_imp_articoli"))));
	strcpy(Cfg.szImpUbicazioniFile, gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_imp_ubicazioni"))));
	strcpy(Cfg.szExpOrdiniFile,     gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_exp_ordini"))));
	strcpy(Cfg.szExpColliFile,      gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_exp_colli"))));


	/*
	* DATABASE
	*/
	strcpy(Cfg.szPGHost,         gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_pghost"))));
	strcpy(Cfg.szPGPort,         gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_pgport"))));
	strcpy(Cfg.szPGDataBase,     gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_pgdatabase"))));

	/*
	* STAMPANTI
	*/
	strcpy(Cfg.szPrinterConsole,       gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_printer_console"))));
	strcpy(Cfg.szLabelPrinterConsole,  gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_label_printer_console"))));
	strcpy(Cfg.szPrinterIsola1,        gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_printer_isola1"))));
	strcpy(Cfg.szLabelPrinterIsola1,   gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_label_printer_isola1"))));
	strcpy(Cfg.szPrinterIsola2,        gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_printer_isola2"))));
	strcpy(Cfg.szLabelPrinterIsola2,   gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_label_printer_isola2"))));
	strcpy(Cfg.szPrinterBilancia,      gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_printer_bilancia"))));
	strcpy(Cfg.szLabelPrinterBilancia, gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_label_printer_bilancia"))));

	/*
	* Varie
	*/
	Cfg.nSogliaControlloBarcode = gtk_spin_button_get_value_as_int((GtkSpinButton *)get_widget(dlg_config,"sb_main_soglia_barcode_id"));

	xncPutFileInt(ProcList[PROC_MAIN].szProcName, "Delay", Cfg.nMainDelay,Cfg.szCniCfg,NULL);

	xncPutFileInt("General Settings", "DebugVersion", Cfg.nDebugVersion,Cfg.szCniCfg,NULL);
	xncPutFileInt("General Settings", "AdvancedInvoicing", Cfg.nAdvancedInvoicing,Cfg.szCniCfg,NULL);
	xncPutFileInt("General Settings", "MaxColliOrdine", Cfg.nMaxColliOrdine,Cfg.szCniCfg,NULL);
	xncPutFileInt("General Settings", "StampaPL", Cfg.nStampaPL,Cfg.szCniCfg,NULL);
	xncPutFileInt("General Settings", "StampaRAC", Cfg.nStampaRAC,Cfg.szCniCfg,NULL);
	xncPutFileInt("General Settings", "ChiusuraCollo", Cfg.nChiusuraCollo,Cfg.szCniCfg,NULL);
	xncPutFileInt("General Settings", "ImballiUbicati", Cfg.nImballiUbicati,Cfg.szCniCfg,NULL);
	xncPutFileInt("General Settings", "AttesaPDFOrdineCompleto", Cfg.nAttesaPDFOrdineCompleto,Cfg.szCniCfg,NULL);
	xncPutFileInt("General Settings", "SogliaControlloBarcode", Cfg.nSogliaControlloBarcode,Cfg.szCniCfg,NULL);
	xncPutFileInt("General Settings", "GestioneBadge", Cfg.nGestioneBadge,Cfg.szCniCfg,NULL);

	xncPutFileString("General Settings", "PathData",   Cfg.szPathData,   Cfg.szCniCfg, NULL); 
	xncPutFileString("General Settings", "PathStampe", Cfg.szPathStampe, Cfg.szCniCfg, NULL); 
	xncPutFileString("General Settings", "PathExport", Cfg.szPathExport, Cfg.szCniCfg, NULL); 
	xncPutFileString("General Settings", "PathExe",    Cfg.szPathExe,    Cfg.szCniCfg, NULL); 
	xncPutFileString("General Settings", "ImpOrdiniFile",     Cfg.szImpOrdiniFile,     Cfg.szCniCfg, NULL); 
	xncPutFileString("General Settings", "ImpRigheFile",      Cfg.szImpRigheFile,      Cfg.szCniCfg, NULL); 
	xncPutFileString("General Settings", "ImpArticoliFile",   Cfg.szImpArticoliFile,   Cfg.szCniCfg, NULL); 
	xncPutFileString("General Settings", "ImpUbicazioniFile", Cfg.szImpUbicazioniFile, Cfg.szCniCfg, NULL); 
	xncPutFileString("General Settings", "ExpOrdiniFile",     Cfg.szExpOrdiniFile,     Cfg.szCniCfg, NULL); 
	xncPutFileString("General Settings", "ExpColliFile",      Cfg.szExpColliFile,      Cfg.szCniCfg, NULL); 
	xncPutFileString("General Settings", "ExpDettColliFile",  Cfg.szExpDettColliFile,      Cfg.szCniCfg, NULL); 

	xncPutFileString("General Settings", "PrinterConsole",       Cfg.szPrinterConsole,       Cfg.szCniCfg, NULL); 
	xncPutFileString("General Settings", "LabelPrinterConsole",  Cfg.szLabelPrinterConsole,  Cfg.szCniCfg, NULL); 
	xncPutFileString("General Settings", "PrinterIsola1",        Cfg.szPrinterIsola1,        Cfg.szCniCfg, NULL); 
	xncPutFileString("General Settings", "LabelPrinterIsola1",   Cfg.szLabelPrinterIsola1,   Cfg.szCniCfg, NULL); 
	xncPutFileString("General Settings", "PrinterIsola2",        Cfg.szPrinterIsola2,        Cfg.szCniCfg, NULL); 
	xncPutFileString("General Settings", "LabelPrinterIsola2",   Cfg.szLabelPrinterIsola2,   Cfg.szCniCfg, NULL); 
	xncPutFileString("General Settings", "PrinterBilancia",      Cfg.szPrinterBilancia,      Cfg.szCniCfg, NULL); 
	xncPutFileString("General Settings", "LabelPrinterBilancia", Cfg.szLabelPrinterBilancia, Cfg.szCniCfg, NULL); 

	xncPutFileString("DataBase Settings", "PGHost",        Cfg.szPGHost,       Cfg.szCniCfg, NULL); 
	xncPutFileString("DataBase Settings", "PGPort",        Cfg.szPGPort,       Cfg.szCniCfg, NULL); 
	xncPutFileString("DataBase Settings", "PGDataBase",    Cfg.szPGDataBase,   Cfg.szCniCfg, NULL); 

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
		gtk_text_printf(NULL,txt_msgs,"Errore in update su ord_prod [%s]\n",szOrdProg);
#ifdef TRACE
		trace_out_vstr_date(1,"CambiaStatoOrdine(%s,%c) - Fallito update su ord_prod",szOrdProg,cStato);
#endif
	}
	PQclear(PGRes);

	return(bRetVal);
}


#define RIGA_TOTALI         0
#define RIGA_CON_PDF        1
#define RIGA_LANCIATI       2
#define RIGA_ELABORATI      3
#define RIGA_IN_PRELIEVO    4
#define RIGA_INCOMPLETI     5
#define RIGA_TERMINATI      6
#define RIGA_EVASI          7
#define RIGA_SPEDITI        8
#define RIGA_DA_FARE        9
#define TOTALE_RIGHE       10 

#define COLONNA_ORDINI      0
#define COLONNA_COLLI       1
#define COLONNA_PRESE       2
#define COLONNA_RIGHE       3
#define COLONNA_PEZZI       4
#define TOTALE_COLONNE      5

char *szTitoli[]={
	"TOTALI",
	"CON PDF",
	"LANCIATI",
	"ELABORATI",
	"IN PRELIEVO",
	"INCOMPLETI",
	"TERMINATI",
	"EVASI",
	"SPEDITI A HOST",
	"DA FARE"
};

void RefreshDatiIsola(int nIsola,GtkWidget *clist)
{
	char szStato[256];
	char szPdfStato[256];
	PGresult *PGRes = NULL;
	PGresult *PGResColli = NULL;
	int nIndex;
	int nOrdini;
	int nConteggi[20][20];
	int nRigaIndex;
	int nColIndex;
	char *RecordData[1][MAX_FIELDS_NUMBER];

	for(nRigaIndex=0;nRigaIndex<TOTALE_RIGHE;nRigaIndex++){
		for(nColIndex=0;nColIndex<TOTALE_COLONNE;nColIndex++){
			nConteggi[nRigaIndex][nColIndex]=0;
		}
	}

	for(nIndex=0;nIndex<20;nIndex++){
		RecordData[0][nIndex]=malloc(100);
	}

	if(nIsola){
		PGRes=PGExecSQL(Cfg.nDebugVersion,"select op.ordkey,op.opnmcll,op.opnmprs,op.opnmrgh,op.opnmcpe,op.opstato,op.opflpdf from ord_prod as op where op.opcdisl=%d;",nIsola);
		PGResColli=PGExecSQL(Cfg.nDebugVersion,"select sum(cpnmrgh),sum(cpnmcpe) from ord_prod as op, col_prod as cp where op.ordprog=cp.ordprog and op.opcdisl=%d and opstato='L' and cpflpdf='P' ;",nIsola);
	} else {
		PGRes=PGExecSQL(Cfg.nDebugVersion,"select op.ordkey,op.opnmcll,op.opnmprs,op.opnmrgh,op.opnmcpe,op.opstato,op.opflpdf from ord_prod as op;");
		PGResColli=PGExecSQL(Cfg.nDebugVersion,"select sum(cpnmrgh),sum(cpnmcpe) from ord_prod as op, col_prod as cp where op.ordprog=cp.ordprog  and opstato='L' and cpflpdf='P' ;");
	}
	nOrdini=PQntuples(PGRes);
	for(nIndex=0;nIndex<nOrdini;nIndex++){
		strcpy(szStato,PQgetvalue(PGRes,nIndex,5));
		strcpy(szPdfStato,PQgetvalue(PGRes,nIndex,6));
		switch(szStato[0]){
			case ORDINE_SPEDITO_HOST:
				nConteggi[RIGA_SPEDITI][COLONNA_ORDINI]++;
				nConteggi[RIGA_SPEDITI][COLONNA_COLLI]+=atoi(PQgetvalue(PGRes,nIndex,1));
				/* wella: prese=pezzi+colli */
				nConteggi[RIGA_SPEDITI][COLONNA_PRESE]+=(atoi(PQgetvalue(PGRes,nIndex,1))+atoi(PQgetvalue(PGRes,nIndex,4)));
				nConteggi[RIGA_SPEDITI][COLONNA_RIGHE]+=atoi(PQgetvalue(PGRes,nIndex,3));
				nConteggi[RIGA_SPEDITI][COLONNA_PEZZI]+=atoi(PQgetvalue(PGRes,nIndex,4));
			case ORDINE_EVASO:
				nConteggi[RIGA_EVASI][COLONNA_ORDINI]++;
				nConteggi[RIGA_EVASI][COLONNA_COLLI]+=atoi(PQgetvalue(PGRes,nIndex,1));
				/* wella: prese=pezzi+colli */
				nConteggi[RIGA_EVASI][COLONNA_PRESE]+=(atoi(PQgetvalue(PGRes,nIndex,1))+atoi(PQgetvalue(PGRes,nIndex,4)));
				nConteggi[RIGA_EVASI][COLONNA_RIGHE]+=atoi(PQgetvalue(PGRes,nIndex,3));
				nConteggi[RIGA_EVASI][COLONNA_PEZZI]+=atoi(PQgetvalue(PGRes,nIndex,4));
			case ORDINE_PRELEVATO:
				nConteggi[RIGA_TERMINATI][COLONNA_ORDINI]++;
				nConteggi[RIGA_TERMINATI][COLONNA_COLLI]+=atoi(PQgetvalue(PGRes,nIndex,1));
				/* wella: prese=pezzi+colli */
				nConteggi[RIGA_TERMINATI][COLONNA_PRESE]+=(atoi(PQgetvalue(PGRes,nIndex,1))+atoi(PQgetvalue(PGRes,nIndex,4)));
				nConteggi[RIGA_TERMINATI][COLONNA_RIGHE]+=atoi(PQgetvalue(PGRes,nIndex,3));
				nConteggi[RIGA_TERMINATI][COLONNA_PEZZI]+=atoi(PQgetvalue(PGRes,nIndex,4));
			case ORDINE_INCOMPLETO_TERMINATO:
				nConteggi[RIGA_INCOMPLETI][COLONNA_ORDINI]++;
				nConteggi[RIGA_INCOMPLETI][COLONNA_COLLI]+=atoi(PQgetvalue(PGRes,nIndex,1));
				/* wella: prese=pezzi+colli */
				nConteggi[RIGA_INCOMPLETI][COLONNA_PRESE]+=(atoi(PQgetvalue(PGRes,nIndex,1))+atoi(PQgetvalue(PGRes,nIndex,4)));
				nConteggi[RIGA_INCOMPLETI][COLONNA_RIGHE]+=atoi(PQgetvalue(PGRes,nIndex,3));
				nConteggi[RIGA_INCOMPLETI][COLONNA_PEZZI]+=atoi(PQgetvalue(PGRes,nIndex,4));
			case ORDINE_IN_PRELIEVO:
			case ORDINE_INCOMPLETO_IN_PRELIEVO:
			case ORDINE_INCOMPLETO_TERMINATO_IN_PRELIEVO:
				nConteggi[RIGA_IN_PRELIEVO][COLONNA_ORDINI]++;
				nConteggi[RIGA_IN_PRELIEVO][COLONNA_COLLI]+=atoi(PQgetvalue(PGRes,nIndex,1));
				/* wella: prese=pezzi+colli */
				nConteggi[RIGA_IN_PRELIEVO][COLONNA_PRESE]+=(atoi(PQgetvalue(PGRes,nIndex,1))+atoi(PQgetvalue(PGRes,nIndex,4)));
				nConteggi[RIGA_IN_PRELIEVO][COLONNA_RIGHE]+=atoi(PQgetvalue(PGRes,nIndex,3));
				nConteggi[RIGA_IN_PRELIEVO][COLONNA_PEZZI]+=atoi(PQgetvalue(PGRes,nIndex,4));
			case ORDINE_ELABORATO:
				nConteggi[RIGA_ELABORATI][COLONNA_ORDINI]++;
				nConteggi[RIGA_ELABORATI][COLONNA_COLLI]+=atoi(PQgetvalue(PGRes,nIndex,1));
				/* wella: prese=pezzi+colli */
				nConteggi[RIGA_ELABORATI][COLONNA_PRESE]+=(atoi(PQgetvalue(PGRes,nIndex,1))+atoi(PQgetvalue(PGRes,nIndex,4)));
				nConteggi[RIGA_ELABORATI][COLONNA_RIGHE]+=atoi(PQgetvalue(PGRes,nIndex,3));
				nConteggi[RIGA_ELABORATI][COLONNA_PEZZI]+=atoi(PQgetvalue(PGRes,nIndex,4));
			case ORDINE_SPEDITO:
			case ORDINE_INCOMPLETO_TERMINATO_SPEDITO:
				nConteggi[RIGA_LANCIATI][COLONNA_ORDINI]++;
				nConteggi[RIGA_LANCIATI][COLONNA_COLLI]+=atoi(PQgetvalue(PGRes,nIndex,1));
				/* wella: prese=pezzi+colli */
				nConteggi[RIGA_LANCIATI][COLONNA_PRESE]+=(atoi(PQgetvalue(PGRes,nIndex,1))+atoi(PQgetvalue(PGRes,nIndex,4)));
				nConteggi[RIGA_LANCIATI][COLONNA_RIGHE]+=atoi(PQgetvalue(PGRes,nIndex,3));
				nConteggi[RIGA_LANCIATI][COLONNA_PEZZI]+=atoi(PQgetvalue(PGRes,nIndex,4));
			default:
				nConteggi[RIGA_TOTALI][COLONNA_ORDINI]++;
				nConteggi[RIGA_TOTALI][COLONNA_COLLI]+=atoi(PQgetvalue(PGRes,nIndex,1));
				/* wella: prese=pezzi+colli */
				nConteggi[RIGA_TOTALI][COLONNA_PRESE]+=(atoi(PQgetvalue(PGRes,nIndex,1))+atoi(PQgetvalue(PGRes,nIndex,4)));
				nConteggi[RIGA_TOTALI][COLONNA_RIGHE]+=atoi(PQgetvalue(PGRes,nIndex,3));
				nConteggi[RIGA_TOTALI][COLONNA_PEZZI]+=atoi(PQgetvalue(PGRes,nIndex,4));
			break;
		}
		/* rm 24-11-2004 : nuovo conteggio 'da fare' */
		switch(szStato[0]){
			case ORDINE_ELABORATO:
				nConteggi[RIGA_DA_FARE][COLONNA_ORDINI]++;
				nConteggi[RIGA_DA_FARE][COLONNA_COLLI]+=atoi(PQgetvalue(PGRes,nIndex,1));
				/* wella: prese=pezzi+colli */
				nConteggi[RIGA_DA_FARE][COLONNA_PRESE]+=(atoi(PQgetvalue(PGRes,nIndex,1))+atoi(PQgetvalue(PGRes,nIndex,4)));
				nConteggi[RIGA_DA_FARE][COLONNA_RIGHE]+=atoi(PQgetvalue(PGRes,nIndex,3));
				nConteggi[RIGA_DA_FARE][COLONNA_PEZZI]+=atoi(PQgetvalue(PGRes,nIndex,4));
			break;
		}
		switch(szPdfStato[0]){
			case ORDINE_CON_FATTURA:
			case ORDINE_CON_DDT:
				nConteggi[RIGA_CON_PDF][COLONNA_ORDINI]++;
				nConteggi[RIGA_CON_PDF][COLONNA_COLLI]+=atoi(PQgetvalue(PGRes,nIndex,1));
				/* wella: prese=pezzi+colli */
				nConteggi[RIGA_CON_PDF][COLONNA_PRESE]+=(atoi(PQgetvalue(PGRes,nIndex,1))+atoi(PQgetvalue(PGRes,nIndex,4)));
				nConteggi[RIGA_CON_PDF][COLONNA_RIGHE]+=atoi(PQgetvalue(PGRes,nIndex,3));
				nConteggi[RIGA_CON_PDF][COLONNA_PEZZI]+=atoi(PQgetvalue(PGRes,nIndex,4));
			break;
		}
	}
	/* rm 24-11-2004 : nuovo conteggio 'da fare' */
	nConteggi[RIGA_DA_FARE][COLONNA_ORDINI]++;
	nConteggi[RIGA_DA_FARE][COLONNA_COLLI]+=1;
	/* wella: prese=pezzi+colli */
	if(PQntuples(PGResColli)){
		nConteggi[RIGA_DA_FARE][COLONNA_PRESE]+=(atoi(PQgetvalue(PGResColli,0,1)))+1;
		nConteggi[RIGA_DA_FARE][COLONNA_RIGHE]+=atoi(PQgetvalue(PGResColli,0,0));
		nConteggi[RIGA_DA_FARE][COLONNA_PEZZI]+=atoi(PQgetvalue(PGResColli,0,1));
	}
	PQclear(PGRes);
	PQclear(PGResColli);

//	nConteggi[RIGA_IN_PRELIEVO][COLONNA_ORDINI]-=nConteggi[RIGA_TERMINATI][COLONNA_ORDINI];
//	nConteggi[RIGA_IN_PRELIEVO][COLONNA_COLLI] -=nConteggi[RIGA_TERMINATI][COLONNA_COLLI];
//	nConteggi[RIGA_IN_PRELIEVO][COLONNA_PRESE] -=nConteggi[RIGA_TERMINATI][COLONNA_PRESE];
//	nConteggi[RIGA_IN_PRELIEVO][COLONNA_RIGHE] -=nConteggi[RIGA_TERMINATI][COLONNA_RIGHE];
//	nConteggi[RIGA_IN_PRELIEVO][COLONNA_PEZZI] -=nConteggi[RIGA_TERMINATI][COLONNA_PEZZI];

	nConteggi[RIGA_IN_PRELIEVO][COLONNA_ORDINI]-=nConteggi[RIGA_INCOMPLETI][COLONNA_ORDINI];
	nConteggi[RIGA_IN_PRELIEVO][COLONNA_COLLI] -=nConteggi[RIGA_INCOMPLETI][COLONNA_COLLI];
	nConteggi[RIGA_IN_PRELIEVO][COLONNA_PRESE] -=nConteggi[RIGA_INCOMPLETI][COLONNA_PRESE];
	nConteggi[RIGA_IN_PRELIEVO][COLONNA_RIGHE] -=nConteggi[RIGA_INCOMPLETI][COLONNA_RIGHE];
	nConteggi[RIGA_IN_PRELIEVO][COLONNA_PEZZI] -=nConteggi[RIGA_INCOMPLETI][COLONNA_PEZZI];

	gtk_clist_clear(GTK_CLIST(clist));

	for(nRigaIndex=0;nRigaIndex<TOTALE_RIGHE;nRigaIndex++){
		strcpy(RecordData[0][0],szTitoli[nRigaIndex]);

		for(nColIndex=0;nColIndex<TOTALE_COLONNE;nColIndex++){
			sprintf(RecordData[0][nColIndex+1],"%5d",nConteggi[nRigaIndex][nColIndex]);
		}
		gtk_clist_append(GTK_CLIST(clist), RecordData[0]);
	}
	/* 
	* libero la memoria allocata 
	*/
	for(nIndex=0;nIndex<20;nIndex++){
		free(RecordData[0][nIndex]);
	}
}


void RefreshElaborazioni(GtkWidget *lst,int nIsola)
{
	PGresult *PGRes;
	PGresult *PGResOrd;
	char *RecordData[1][MAX_FIELDS_NUMBER];
	int nIndex;
	int nOrdiniTotali=0;
	int nOrdiniLanciati=0;
	int nOrdiniEvasi=0;
	int nOrdiniPdf=0;
	int nOrdiniIncompleti=0;

	gtk_clist_clear(GTK_CLIST(lst));

	for(nIndex=0;nIndex<20;nIndex++){
		RecordData[0][nIndex]=malloc(100);
	}
	if(nIsola){
		PGRes=PGExecSQL(Cfg.nDebugVersion>3,"select ord_prod.opnelab,count(ord_prod.opnelab),sum(ord_prod.opnmcll),sum(ord_prod.opnmprs),sum(ord_prod.opnmrgh),sum(ord_prod.opnmcpe),sum(ord_prod.oppspre) from ord_prod  where opcdisl=%d  group by ord_prod.opnelab order by ord_prod.opnelab;",nIsola);
	} else {
		PGRes=PGExecSQL(Cfg.nDebugVersion>3,"select ric_ord.ronelab,count(ric_ord.ronelab),sum(ric_ord.ronmcll),sum(ric_ord.ronmprs),sum(ric_ord.ronmrgh),sum(ric_ord.ronmcpe),sum(ric_ord.ropspre)  from ric_ord  group by ric_ord.ronelab order by ric_ord.ronelab;");
	}
	for(nIndex=0;nIndex<PQntuples(PGRes);nIndex++){
		/* ordini totali */
		nOrdiniTotali=atoi(PQgetvalue(PGRes,nIndex,1));
		
		/* ordini lanciati */
		nOrdiniLanciati=0;
		if(nIsola){
			PGResOrd=PGExecSQL(Cfg.nDebugVersion>3,"select count(ord_prod.ordprog) from ord_prod where opnelab='%s' and (opstato='%c' or opstato='%c' or opstato='%c' or opstato='%c' or opstato='%c' or opstato='%c') and opcdisl=%d;",PQgetvalue(PGRes,nIndex,0),ORDINE_SPEDITO,ORDINE_INCOMPLETO_TERMINATO_SPEDITO,ORDINE_ELABORATO,ORDINE_IN_PRELIEVO,ORDINE_INCOMPLETO_IN_PRELIEVO,ORDINE_INCOMPLETO_TERMINATO_IN_PRELIEVO,nIsola);
		} else {
			PGResOrd=PGExecSQL(Cfg.nDebugVersion>3,"select count(ric_ord.ordprog) from ric_ord where ronelab='%s' and (rostato='%c' or rostato='%c' or rostato='%c' or rostato='%c' or rostato='%c' or rostato='%c');",PQgetvalue(PGRes,nIndex,0),ORDINE_SPEDITO,ORDINE_INCOMPLETO_TERMINATO_SPEDITO,ORDINE_ELABORATO,ORDINE_IN_PRELIEVO,ORDINE_INCOMPLETO_IN_PRELIEVO,ORDINE_INCOMPLETO_TERMINATO_IN_PRELIEVO);
		}
		if(PQntuples(PGResOrd)){
			nOrdiniLanciati=atoi(PQgetvalue(PGResOrd,0,0));
		}
		PQclear(PGResOrd);

		/* ordini evasi */
		nOrdiniEvasi=0;
		if(nIsola){
			PGResOrd=PGExecSQL(Cfg.nDebugVersion>3,"select count(ord_prod.ordprog) from ord_prod where opnelab='%s' and (opstato='%c' or opstato='%c') and opcdisl=%d;",PQgetvalue(PGRes,nIndex,0),ORDINE_EVASO,ORDINE_SPEDITO_HOST,nIsola);
		} else {
			PGResOrd=PGExecSQL(Cfg.nDebugVersion>3,"select count(ric_ord.ordprog) from ric_ord where ronelab='%s' and (rostato='%c' or rostato='%c');",PQgetvalue(PGRes,nIndex,0),ORDINE_EVASO,ORDINE_SPEDITO_HOST);
		}
		if(PQntuples(PGResOrd)){
			nOrdiniEvasi=atoi(PQgetvalue(PGResOrd,0,0));
		}
		PQclear(PGResOrd);
		
		/* ordini incompleti */
		nOrdiniIncompleti=0;
		if(nIsola){
			PGResOrd=PGExecSQL(Cfg.nDebugVersion>3,"select count(ord_prod.ordprog) from ord_prod where opnelab='%s' and opstato='%c' and opcdisl=%d;",PQgetvalue(PGRes,nIndex,0),ORDINE_INCOMPLETO_TERMINATO,nIsola);
		} else {
			PGResOrd=PGExecSQL(Cfg.nDebugVersion>3,"select count(ric_ord.ordprog) from ric_ord where ronelab='%s' and rostato='%c' ;",PQgetvalue(PGRes,nIndex,0),ORDINE_INCOMPLETO_TERMINATO);
		}
		if(PQntuples(PGResOrd)){
			nOrdiniIncompleti=atoi(PQgetvalue(PGResOrd,0,0));
		}
		PQclear(PGResOrd);
		
		/* ordini con PDF */
		nOrdiniPdf=0;
		if(nIsola){
			PGResOrd=PGExecSQL(Cfg.nDebugVersion>3,"select count(ord_prod.ordprog) from ord_prod where opnelab='%s' and (opflpdf='%c' or opflpdf='%c') and opcdisl=%d;",PQgetvalue(PGRes,nIndex,0),ORDINE_CON_FATTURA,ORDINE_CON_DDT,nIsola);
		} else {
			PGResOrd=PGExecSQL(Cfg.nDebugVersion>3,"select count(ric_ord.ordprog) from ric_ord where ronelab='%s' and (rostato='%c' or rostato='%c');",PQgetvalue(PGRes,nIndex,0),ORDINE_CON_FATTURA,ORDINE_CON_DDT);
		}
		if(PQntuples(PGResOrd)){
			nOrdiniPdf=atoi(PQgetvalue(PGResOrd,0,0));
		}
		PQclear(PGResOrd);

		strcpy(RecordData[0][0],PQgetvalue(PGRes,nIndex,0));                                             /* ELABORAZIONE */
		sprintf(RecordData[0][1],"%5d",nOrdiniTotali);                                                   /* ORDINI */
		sprintf(RecordData[0][2],"%5d",nOrdiniPdf);                                                 /* LANCIATI */
		sprintf(RecordData[0][3],"%5d",nOrdiniLanciati);                                                 /* LANCIATI */
		sprintf(RecordData[0][4],"%5d",nOrdiniIncompleti);                                                 /* LANCIATI */
		sprintf(RecordData[0][5],"%5d",nOrdiniEvasi);                                                    /* EVASI */
		sprintf(RecordData[0][6],"%5d",nOrdiniTotali-nOrdiniEvasi);                                      /* DA EVADERE */
		sprintf(RecordData[0][7],"%5d",atoi(PQgetvalue(PGRes,nIndex,2)));                                /* COLLI */
		sprintf(RecordData[0][8],"%5d",atoi(PQgetvalue(PGRes,nIndex,2))+atoi(PQgetvalue(PGRes,nIndex,5)));
		//sprintf(RecordData[0][6],"%5d",atoi(PQgetvalue(PGRes,nIndex,3)));                              /* Wella : PRESE = PEZZI+COLLI*/
		sprintf(RecordData[0][9],"%5d",atoi(PQgetvalue(PGRes,nIndex,4)));                                /* RIGHE */
		sprintf(RecordData[0][10],"%5d",atoi(PQgetvalue(PGRes,nIndex,5)));                                /* PEZZI */
		sprintf(RecordData[0][11],"%12.3f",(float)((float)atoi(PQgetvalue(PGRes,nIndex,6))/(float)1000)); /* PESO  */

		gtk_clist_append(GTK_CLIST(lst), RecordData[0]);

	}
	PQclear(PGRes);

	for(nIndex=0;nIndex<20;nIndex++){
		free(RecordData[0][nIndex]);
	}
}


void RefreshOperatoriSettori(void)
{
	char szSelectCmd[2048];
	GtkWidget *sw_list;
	GtkWidget *lst;

	sw_list = get_widget(main_window,"sw_lista_settori_operatori");

	/*
	* refresh lista a video
	*/
	if((lst=get_widget(sw_list,"lst"))){
		gtk_widget_destroy(lst);
	}

	strcpy(szSelectCmd,"select a.opisola as \"ISOLA\", a.opset as \"SETTORE\", a.opcdope as \"OPERATORE\", op.opnmope as \"NOME                            \", op.opcgope as \"COGNOME                 \", a.dtmod as \"DATA ASSEGNAZIONE\", a.tmmod as \"ORA ASSEGNAZIONE\" from opeset a, operatori op where a.opcdope=op.opcdope order by opisola,opset;");

	CreateListFromSelect(main_window,sw_list,&lst,"lst_opeset",GTK_SELECTION_SINGLE,szSelectCmd,NULL,0);
}


BOOL StampaStatistica(char *szFromDate,char *szToDate,char *szFileName)
{
	PGresult *PGRes;
	PGresult *PGResSett;
	int nElab,nIsole,nSettori,nIndex,nSettoreIndex;
	FILE *fp;
	int nTotaleSpedizioni=0;
	int nTotalePSPre=0;
	int nTotalePSRea=0;
	int nTotali[10];
	for(nIndex=0;nIndex<10;nIndex++){
		nTotali[nIndex]=0;
	}

	PGRes=PGExecSQL(Cfg.nDebugVersion,"select distinct(ric_stor.ronelab),count(ric_stor.ronelab),sum(ric_stor.ropspre),sum(ric_stor.ropsrea) from ric_stor where (ric_stor.rodteva<='%s' and ric_stor.rodteva>='%s') group by ric_stor.ronelab,ric_stor.rodteva;",szToDate,szFromDate);
	nElab=PQntuples(PGRes);

	if ((fp=fopen(szFileName,"w"))==(FILE *)NULL) {
		return FALSE;
	}

	for(nIndex=0;nIndex<nElab;nIndex++){
		nTotaleSpedizioni+=atoi(PQgetvalue(PGRes,nIndex,1));
		nTotalePSPre+=atoi(PQgetvalue(PGRes,nIndex,2));
		nTotalePSRea+=atoi(PQgetvalue(PGRes,nIndex,3));
	}
	PQclear(PGRes);

	fprintf(fp,"Statistiche di prelievo per data\n\n");
	fprintf(fp,"Dalla Data : %s\n",GetDateFromYYYYMMDD(szFromDate,szDateBuffer));
	fprintf(fp,"Alla Data  : %s\n",GetDateFromYYYYMMDD(szToDate,szDateBuffer));
	fprintf(fp,"Numero Elaborazioni Evase : %5d\n",nElab);
	fprintf(fp,"Numero Ordini Evasi       : %5d\n",nTotaleSpedizioni);
	fprintf(fp,"Peso Calcolato Ordini     : %8.3f\n",(float)((float)(nTotalePSPre)/(float)1000));
	fprintf(fp,"Peso Rilevato Ordini      : %8.3f\n\n",(float)((float)(nTotalePSRea)/(float)1000));


	fprintf(fp,"            Sottordini Colli   Linee   Prese\n");
	fprintf(fp,"----------- ---------- ------- ------- -------\n");


	PGRes=PGExecSQL(Cfg.nDebugVersion,"select distinct(ord_stor.opcdisl),count(ord_stor.opcdisl),sum(ord_stor.opnmcll),sum(ord_stor.opnmrgh::int4),sum(ord_stor.opnmcpe::int4) from ord_stor where (ord_stor.opdteva<='%s' and ord_stor.opdteva>='%s') group by ord_stor.opcdisl;",szToDate,szFromDate);
	nIsole=PQntuples(PGRes);
	for(nIndex=0;nIndex<nIsole;nIndex++){
		fprintf(fp," Isola %d         %5d %7d %7d %7d\n",
			atoi(PQgetvalue(PGRes,nIndex,0)),                                     /* ISOLA      */
			atoi(PQgetvalue(PGRes,nIndex,1)),                                     /* SOTTORDINI */
			atoi(PQgetvalue(PGRes,nIndex,2)),                                     /* COLLI      */
			atoi(PQgetvalue(PGRes,nIndex,3)),                                     /* LINEE      */
			(atoi(PQgetvalue(PGRes,nIndex,4))+atoi(PQgetvalue(PGRes,nIndex,2)))); /* PRESE=copie+colli */

		nTotali[1]+=atoi(PQgetvalue(PGRes,nIndex,1));                                    /* TOT SOTTORDINI */
		nTotali[2]+=atoi(PQgetvalue(PGRes,nIndex,2));                                    /* TOT COLLI      */
		nTotali[3]+=atoi(PQgetvalue(PGRes,nIndex,3));                                    /* TOT LINEE      */
		nTotali[4]+=(atoi(PQgetvalue(PGRes,nIndex,4))+atoi(PQgetvalue(PGRes,nIndex,2))); /* TOT PRESE      */
	}
	fprintf(fp,"----------- ---------- ------- ------- -------\n");
	fprintf(fp,"                 %5d %7d %7d %7d\n\n",
		nTotali[1], nTotali[2], nTotali[3],nTotali[4]);

	for(nIndex=0;nIndex<nIsole;nIndex++){
		PGResSett=PGExecSQL(Cfg.nDebugVersion,"select distinct(substr(rig_stor.rpcdubi,2,2)),sum(rig_stor.rpqtspe::int4) from rig_stor,ord_stor where rig_stor.ordkey=ord_stor.ordkey and ord_stor.opcdisl=%d and (ord_stor.opdteva<='%s' and ord_stor.opdteva>='%s') group by substr(rig_stor.rpcdubi,2,2);",atoi(PQgetvalue(PGRes,nIndex,0)),szToDate,szFromDate );
		nSettori=PQntuples(PGResSett);
		fprintf(fp,"Prese su Isola %d divise per settore\n", atoi(PQgetvalue(PGRes,nIndex,0)));
		fprintf(fp,"-----------------------------------\n");
		for(nSettoreIndex=0;nSettoreIndex<nSettori;nSettoreIndex++){
			/* se imballi ubicati e settore=1 cosidero anche gli imballi */
			if ((nSettoreIndex==0) && (Cfg.nImballiUbicati)){
				fprintf(fp,"Settore %2d : %5d\n",atoi(PQgetvalue(PGResSett,nSettoreIndex,0)),atoi(PQgetvalue(PGResSett,nSettoreIndex,1))+atoi(PQgetvalue(PGRes,nIndex,2)));
			} else {
				fprintf(fp,"Settore %2d : %5d\n",atoi(PQgetvalue(PGResSett,nSettoreIndex,0)),atoi(PQgetvalue(PGResSett,nSettoreIndex,1)));
			}
		}
		fprintf(fp,"-----------------------------------\n\n");
		PQclear(PGResSett);
	}
	PQclear(PGRes);


	fclose(fp);
	return TRUE;	
}


/*
* BOOL CheckUbicazioni(void)
* st 18-12-2000
* controlla che nella tabella Ubicazioni non siano presenti codici doppi, a parte gli imballi.
* Ritorna TRUE se tutto OK
*         FALSE altrimenti
*/
BOOL CheckUbicazioni(void)
{
	PGresult *PGResUbicazioni,*PGResArticoli;
	int nUbicazioni,nCountProdotto,nIndex;
	char szCodice[256];
	BOOL bRetVal=TRUE;

	PGResUbicazioni=PGExecSQL(Cfg.nDebugVersion,"select count(codprod),codprod from ubicazioni where codprod!='' group by codprod order by count(codprod);");
	nUbicazioni=PQntuples(PGResUbicazioni);
	if (nUbicazioni){
		for(nIndex=0;nIndex<nUbicazioni;nIndex++){
			nCountProdotto=atoi(PQgetvalue(PGResUbicazioni,nIndex,0));
			if (nCountProdotto>1){
			/* controllo che non sia un imballo */
				sprintf(szCodice,"%s",PQgetvalue(PGResUbicazioni,nIndex,1));
				PGResArticoli=PGExecSQL(Cfg.nDebugVersion,"select codice from imballi where codice='%s';",szCodice);
				if (PQntuples(PGResArticoli)==0){
					/*
					* non e' un imballo, segnalo il codice prodotto
					*/
					bRetVal=FALSE;
				}
				PQclear(PGResArticoli);
			}
		}
	}
	PQclear(PGResUbicazioni);
	return bRetVal;
}

BOOL SetPriority(char *pszOrdineKey,int nPrio)
{
	BOOL bRetValue=FALSE;
	PGresult *PGRes;
	int nProgressivoLancio=0;

	PGRes=PGExecSQL(FALSE,"select opprgln from ord_prod where ordprog='%s';",pszOrdineKey);
	if(PQntuples(PGRes)){
		nProgressivoLancio=atoi(PQgetvalue(PGRes,0,0));
		if (nProgressivoLancio!=0){
			/* ok, ordine lanciato */
			bRetValue = TRUE;
		}
	}
	PQclear(PGRes);

	if(bRetValue){
		PGRes=PGExecSQL(Cfg.nDebugVersion,"update ric_ord set roprgln=%d , roprgpr=%d where ordprog='%s';", nPrio,nPrio,pszOrdineKey);
		bRetValue = (PQresultStatus(PGRes)==PGRES_COMMAND_OK);
		PQclear(PGRes);
		if(bRetValue){
			PGRes=PGExecSQL(Cfg.nDebugVersion,"update ord_prod set opprgln=%d , opprgpr=%d where ordprog='%s';", nPrio,nPrio,pszOrdineKey);
			bRetValue = (PQresultStatus(PGRes)==PGRES_COMMAND_OK);
			PQclear(PGRes);
		}
	}

	return bRetValue;
}

void RefreshListaAnagOperatori(void)
{
	char szSelectCmd[2048];
	GtkWidget *lst;

	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_operatori,"entry_op_codice")),"");
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_operatori,"entry_op_nome")),"");
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_operatori,"entry_op_cognome")),"");
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_operatori,"entry_op_indirizzo")),"");
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_operatori,"entry_op_localita")),"");
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_operatori,"entry_op_provincia")),"");
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_operatori,"entry_op_cap")),"");
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_operatori,"entry_op_telefono")),"");

	/*
	* refresh lista a video
	*/
	if((lst=get_widget(dlg_operatori,"lst_opeana"))){
		gtk_widget_destroy(lst);
	}
	strcpy(szSelectCmd,"select opcdope as \"CODICE\",opnmope as \"NOME\",opcgope as \"COGNOME\",opinope as \"INDIRIZZO\",oploope as \"LOCALITA'\",opprope as \"PROV\",opcpope as \"CAP\",opnmtel as \"TELEFONO\" from operatori order by opcgope,opnmope;");
	CreateListFromSelect(dlg_operatori,get_widget(dlg_operatori,"sw_anagrafica_operatori"),&lst,"lst_opeana",GTK_SELECTION_SINGLE,szSelectCmd,NULL,0);
  gtk_signal_connect (GTK_OBJECT (get_widget(dlg_operatori,"lst_opeana")), "select_row", GTK_SIGNAL_FUNC (on_lista_opeana_row_selected), 0);
}

void RefreshListaProdotti(GtkWidget *dlg)
{
	char szSelectCmd[2048];
	GtkWidget *lst;
	GtkWidget *parent=get_widget(dlg,"sw_prodotti");

	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg,"entry_pr_codice")),"");
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg,"entry_pr_descrizione")),"");
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg,"entry_pr_lunghezza")),"");
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg,"entry_pr_larghezza")),"");
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg,"entry_pr_altezza")),"");
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg,"entry_pr_peso")),"");


	/*
	* refresh lista a video
	*/
	if((lst=get_widget(dlg,"lst_prodotti"))){
		gtk_widget_destroy(lst);
	}
	strcpy(szSelectCmd,"select prcdpro as \"CODICE\", prdescr as \"DESCRIZIONE\",prlungh as \"LUNGHEZZA\",prlargh as \"LARGHEZZA\",praltez as \"ALTEZZA\",prpesgr as \"PESO\",prqtkar as \"PROD X PACCO\",prflgpp as \"IN PIEDI\",prswbri as \"BID\",prswpsp as \"IS\",u.isola as \"ISOLA\",u.settore as \"SETTORE\",u.locazione as \"LOCAZIONE\" from articoli,ubicazioni u where articoli.prcdpro=u.codprod order by prcdpro;");
	CreateListFromSelect(dlg,parent,&lst,"lst_prodotti",GTK_SELECTION_SINGLE,szSelectCmd,NULL,0);
  gtk_signal_connect (GTK_OBJECT (lst), "select_row", GTK_SIGNAL_FUNC (on_lst_prodotti_row_selected), dlg);
}

void RefreshListaProdottiPericolosi(GtkWidget *dlg)
{
	char szSelectCmd[2048];
	GtkWidget *lst;
	GtkWidget *parent=get_widget(dlg,"sw_prodotti_pericolosi");

	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg,"entry_ppcdpro")),"");
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg,"entry_ppcdper")),"");

	/*
	* refresh lista a video
	*/
	if((lst=get_widget(dlg,"lst_prodotti_pericolosi"))){
		gtk_widget_destroy(lst);
	}
	strcpy(szSelectCmd,"select ppcdpro as \"CODICE\", ppcdper as \"CODICE PERICOLOSITA'\" from prodotti_pericolosi order by ppcdpro;");
	CreateListFromSelect(dlg,parent,&lst,"lst_prodotti_pericolosi",GTK_SELECTION_SINGLE,szSelectCmd,NULL,0);
  gtk_signal_connect (GTK_OBJECT (lst), "select_row", GTK_SIGNAL_FUNC (on_lst_prodotti_pericolosi_row_selected), dlg);
}

/*
* rm+g 28-03-2008 : gestione stampe di ordini lanciati ed evasi
*/
void CheckStampaOrdineEvaso(char *pszOrdine)
{
	PGresult *PGRes = NULL;
	int nTuples;
	int nIndex;
	char szStato[128];
	char szOrdKey[128];
	char szElaborazione[128];
	int nIsola;

	PGRes=PGExecSQL(Cfg.nDebugVersion,"select ordkey,opstato,opcdisl,opnelab from ord_prod where ordprog='%s';",pszOrdine);
	nTuples=PQntuples(PGRes);
	for(nIndex=0;nIndex<nTuples;nIndex++){
		strcpy(szOrdKey,PQgetvalue(PGRes,nIndex,0));
		strcpy(szStato,PQgetvalue(PGRes,nIndex,1));
		nIsola=atoi(PQgetvalue(PGRes,nIndex,2));
		strcpy(szElaborazione,PQgetvalue(PGRes,nIndex,3));

		if(szStato[0]==ORDINE_EVASO){
			/* 
			* se ordine analizzato ed evaso correttamente da volum
			* stampo tutti i docs dell'ordine (etichette/packinglist/pdf) e basta
			*/
			StampaCompletaOrdine(nIsola, szElaborazione,szOrdKey);
		}
	}
	PQclear(PGRes);
}


BOOL ScartaOrdine(char *pszOrdineKey,char *pszStato)
{
	PGresult *PGRes;
	BOOL bRetValue;

	PGRes=PGExecSQL(Cfg.nDebugVersion,"update ric_ord set roprdsc='%s' where ordprog='%s';",pszStato,pszOrdineKey);
	bRetValue = (PQresultStatus(PGRes)==PGRES_COMMAND_OK);
	PQclear(PGRes);
	return bRetValue;
}

BOOL InviaOrdine(char *pszOrdineKey,FILE *fp)
{
	PGresult *PGRes;
	BOOL bOK=TRUE;

	/*
	* rm 20-01-2004 : invi il peso TEORICO 
	*/
	PGRes=PGExecSQL(Cfg.nDebugVersion,"select ordprog,ronmcll,ropspre,ronelab from ric_ord where ordprog='%s';",pszOrdineKey);	
	if(PQntuples(PGRes)){
		/*
		* spedizione dell'ordine a Host
		*/
		WriteAsciiRecord(fp, &tOrdCni,PGRes,0);
	} else {
		bOK=FALSE;
#ifdef TRACE
		trace_out_vstr_date(1,"Errore in selezione ordine [%s]",pszOrdineKey);
#endif
		gtk_text_printf("RED",txt_msgs,"Errore in selezione ordine [%s]\n",pszOrdineKey);
	}
	PQclear(PGRes);
	return bOK;
}

BOOL InviaColliOrdine(char *pszOrdineKey,FILE *fp)
{
	PGresult *PGResColli;
	BOOL bOK=TRUE;
	int nNumeroColli,nIndexColli,nColliSpediti=0;


	PGResColli=PGExecSQL(Cfg.nDebugVersion,"select ordprog,cptpfor,cpprgcl from col_prod where ordprog='%s' order by ordkey,cpnmcol;",pszOrdineKey);	

	if ((nNumeroColli=PQntuples(PGResColli))){
		for (nIndexColli=0;nIndexColli<nNumeroColli;nIndexColli++){
			/*
			* spedizione a Host dei colli relativi all'ordine
			*/
			WriteAsciiRecord(fp, &tColCni,PGResColli,nIndexColli);
			nColliSpediti++;
		}

	}

	PQclear(PGResColli);

	return bOK;
}


/*
* 17/06/2019 rm : gestione amazon
*/
BOOL InviaDettaglioColliOrdine(char *pszOrdineKey,FILE *fp)
{
	PGresult *PGResDettColli;
	BOOL bOK=TRUE;
	int nNumeroRighe,nIndex,nRigheSpedite=0;

	/* seleziono sia i colli di linea che i colli bertello */

	PGResDettColli=PGExecSQL(Cfg.nDebugVersion,"select \
		c.ORDPROG, c.ORDKEY, RPNMCOL, RPCDPRO, RPQTSPE, RPCDUBI, RPQTPES, \
		'009905617600'||CPPRGCL||sscc_checkdigit('009905617600'||CPPRGCL) as rpcdssc from col_prod c,rig_prod r \
		where c.ordprog='%s' and r.ordprog=c.ordprog and r.ordkey=c.ordkey and r.rpnmcol=c.cpnmcol \
		union all select \
		c.ORDPROG, ' ' as ORDKEY, RPNMCOL, RPCDPRO, RPNMCPE as RPQTSPE, ' ' as RPCDUBI, RPPESGR as RPQTPES, \
		'009905617600'||CPPRGCL||sscc_checkdigit('009905617600'||CPPRGCL) as rpcdssc from colli_bertello c,righe_bertello r \
		where c.ordprog='%s' and r.ordprog=c.ordprog and r.rpnmcol=c.cpnmcol\
		order by 1,2,3;", pszOrdineKey,pszOrdineKey);

	if ((nNumeroRighe=PQntuples(PGResDettColli))){
		for (nIndex=0;nIndex<nNumeroRighe;nIndex++){
			/*
			* spedizione a Host dei colli relativi all'ordine
			*/
			WriteAsciiRecord(fp, &tDettColCni,PGResDettColli,nIndex);
			nRigheSpedite++;
		}

	}

	PQclear(PGResDettColli);

	return bOK;
}

/*
* 2024-06-29 11:47:48 rm : gestione bertello
*/
int InviaDatiBertello(FILE *fp)
{
	PGresult *PGRes;
	BOOL bOK=TRUE;
	int nNumeroRighe,nIndex,nRigheSpedite=0;

	/* seleziono sia le righe di rifornimento che le righe di prelievo */
	PGRes=PGExecSQL(Cfg.nDebugVersion,"insert into bertello_liste_movimentazione  \
		(BLINAME,BLIDESC,BLIPRIO,BLIAREA,BLITPMV,BLICDPR,BLICDLT,BLIQTRC,BLICDEC,BLIENDC) \
			select \
                o.ROTRORD,CASE WHEN o.RONELAB = '          ' THEN '' ELSE 'PW:'||o.RONELAB||'/C:'||o.ORDPROG END,0,'1','P',r.RPCDPRO,r.RPCDLOT,r.RPNMCPE,'DFA','|' \
                from ordini_bertello_b o,righe_bertello_b r where r.rotrord=o.rotrord and r.rpstato='%c' \
			union all \
			select \
                o.ROTRORD,' ',0,'1','D',r.RPCDPRO,r.RPCDLOT,r.RPNMCPE,'DFA','|' \
                from ordini_bertello_r o,righe_bertello_r r where r.rotrord=o.rotrord and r.rpstato='%c';", RIGA_RICEVUTA, RIGA_RICEVUTA);

	nNumeroRighe = atoi(PQcmdTuples(PGRes));
	int nResultStatus = PQresultStatus(PGRes);
	PQclear(PGRes);

	if (!nNumeroRighe){
		if (nResultStatus != PGRES_COMMAND_OK ) {
			gtk_text_printf(NULL,txt_msgs,"Errore in scrittura dati per bertello\n");
#ifdef TRACE
			trace_out_vstr_date(1,"Errore in scrittura dati per bertello");
#endif
			bOK = FALSE;
			nNumeroRighe = -1;
		}
		return nNumeroRighe;
	} 

	/* seleziono sia i colli di linea che i colli bertello */

	PGRes = PGExecSQL(Cfg.nDebugVersion,"select * from bertello_liste_movimentazione where BLSTATO is null");

	if ((nNumeroRighe=PQntuples(PGRes))){
		for (nIndex=0;nIndex<nNumeroRighe;nIndex++){
			/*
			* spedizione a Host dei colli relativi all'ordine
			*/
			WriteAsciiRecord(fp, &tBertelloListeMovimentazione,PGRes,nIndex);
			nRigheSpedite++;
		}
	}

	PQclear(PGRes);

	/* set stato ordini / righe bertello prelievo / rifornimento */
	PGRes = PGExecSQL(Cfg.nDebugVersion,"update ordini_bertello_b set rostato = '%c', rotminv='now'  where rostato = '%c'", ORDINE_INVIATO, ORDINE_RICEVUTO);
	PQclear(PGRes);
	PGRes = PGExecSQL(Cfg.nDebugVersion,"update righe_bertello_b set rpstato = '%c', rptminv='now'  where rpstato = '%c'", RIGA_INVIATA, RIGA_RICEVUTA);
	PQclear(PGRes);
	PGRes = PGExecSQL(Cfg.nDebugVersion,"update ordini_bertello_r set rostato = '%c', rotminv='now'  where rostato = '%c'", ORDINE_INVIATO, ORDINE_RICEVUTO);
	PQclear(PGRes);
	PGRes = PGExecSQL(Cfg.nDebugVersion,"update righe_bertello_r set rpstato = '%c', rptminv='now'  where rpstato = '%c'", RIGA_INVIATA, RIGA_RICEVUTA);
	PQclear(PGRes);
	PGRes = PGExecSQL(Cfg.nDebugVersion,"update bertello_liste_movimentazione set blstato = '%c', bltminv='now' where blstato is NULL", ORDINE_INVIATO);
	PQclear(PGRes);

	return nNumeroRighe;
}






/*
* rm+gg 28-03-2008 : stampa documenti (etichette/packing list/pdf) ordine
*/
void StampaCompletaOrdine(int nIsola, char *szElaborazione,char *szOrdKey)
{
	char cFlagPdf;
	char szLabelPrinter[256];
	char szPrinter[256];
	char szPdfPrinter[256];
	char szBuffer[256];

	/*
	* Ricavo il flag pdf per sapere cosa e dove stampare
	*/
	cFlagPdf=GetPdfFlagFromOrdProd(szOrdKey);


	if(Cfg.nSAP){
		UpdateOrdine(LeftStr(szOrdKey,10));
	} else {
		UpdateOrdine(LeftStr(szOrdKey,6));
	}

	switch(nIsola){
		default:
		case 1:
		{
			sprintf(szLabelPrinter,"%s",Cfg.szLabelPrinterIsola1);
			sprintf(szPrinter,"%s",Cfg.szPrinterIsola1);
			switch(cFlagPdf){
				case ORDINE_CON_FATTURA:
					sprintf(szPdfPrinter,"%s",Cfg.szFatturePrinterIsola1);
				break;
				case ORDINE_CON_DDT:
					sprintf(szPdfPrinter,"%s",Cfg.szBollePrinterIsola1);
				break;
				default:
					sprintf(szPdfPrinter,"%s",Cfg.szFatturePrinterIsola1);
				break;
			}
		}
		break;
		case 2:
		{
			sprintf(szLabelPrinter,"%s",Cfg.szLabelPrinterIsola2);
			sprintf(szPrinter,"%s",Cfg.szPrinterIsola2);
			switch(cFlagPdf){
				case ORDINE_CON_FATTURA:
					sprintf(szPdfPrinter,"%s",Cfg.szFatturePrinterIsola2);
				break;
				case ORDINE_CON_DDT:
					sprintf(szPdfPrinter,"%s",Cfg.szBollePrinterIsola2);
				break;
				default:
					sprintf(szPdfPrinter,"%s",Cfg.szFatturePrinterIsola2);
				break;
			}
		}
		break;
	}

	if(strlen(szOldOrdKey[nIsola]) && strlen(szOldElaborazione[nIsola]) && strcmp(szElaborazione,szOldElaborazione[nIsola])){

		sprintf(szBuffer,"%s,%s",szOldOrdKey[nIsola],szLabelPrinter);
		SendMessage(PROC_PRINTLABEL,PROC_MAIN,PRINT_FINE_ELABORAZIONE,szBuffer);

		sprintf(szBuffer,"%s,%s",szOrdKey,szLabelPrinter);
		SendMessage(PROC_PRINTLABEL,PROC_MAIN,PRINT_INIZIO_ELABORAZIONE,szBuffer);
	}
	strcpy(szOldOrdKey[nIsola],szOrdKey);
	strcpy(szOldElaborazione[nIsola],szElaborazione);

	/*
	* Spedizione messaggi di stampa etichette e packing list a printlabel
	*/
	sprintf(szBuffer,"%s,%s",szOrdKey,szPrinter);
	SendMessage(PROC_PRINTLABEL,PROC_MAIN,PRINT_PACKING_LIST,szBuffer);
	
	sprintf(szBuffer,"%s,%s",szOrdKey,szLabelPrinter);
	SendMessage(PROC_PRINTLABEL,PROC_MAIN,PRINT_RAC,szBuffer);

	/*
	 * Non e' detto che il pdf debba sempre e comunque essere stampato
	 * esistono ordini senza pdf e ordini che potrei forzare
	 */
	switch(cFlagPdf){
		case ORDINE_CON_FATTURA:
		case ORDINE_CON_DDT:
			//sprintf(szBuffer,"%s",szLabelPrinter);
			sprintf(szBuffer,"%s,%s",LeftStr(szOrdKey,10),szLabelPrinter);
			SendMessage(PROC_PRINTLABEL,PROC_MAIN,PRINT_RAC_CONTIENE_DOC,szBuffer);
			sprintf(szBuffer,"%s,%s",LeftStr(szOrdKey,10),szPdfPrinter);
			SendMessage(PROC_PRINTLABEL,PROC_MAIN,PRINT_PDF,szBuffer);
		break;
		default:
		break;
	}
}
