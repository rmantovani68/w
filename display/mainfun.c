/*
* mainfun.c
* funzioni 
* Easy Picking 3.0
* Copyright CNI srl 1999-2000
* Autore : Roberto Mantovani
*/
#include <gnome.h>
#include <signal.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#ifdef TRACE
	#include <trace.h>
#endif

#include <shared.h>
#include <ep-common.h>
#include <ep-db.h>
#include <gtk-support.h>
#include <dbfun-gtk.h>
#include <pmx_msq.h>

#include <proc_list.h>
#include <picking.h>

#include "main.h"
#include "mainstruct.h"
#include "mainext.h"
#include "mainfun.h"

#include "callbacks.h"



/*
* GTK-Error handling functions
*/
void gtk_error_handler(gchar *str)
{
}

void gtk_warning_handler(gchar *str)
{
}

void gtk_message_handler(gchar *str)
{
}


/*
* ReadConfiguration()
* lettura della configurazione dal file cni.cfg
*/
BOOL ReadConfiguration(char *szCfgFileName,BOOL bReadProcInfo)
{
	char szCurrentDirectory[128];

	/*
	* leggo il file di configurazione dalla directory corrente
	*/
	getcwd(szCurrentDirectory,sizeof(szCurrentDirectory));
	strcpy(Cfg.szCfgFileName,szCurrentDirectory);
	strcat(Cfg.szCfgFileName,"/");
	strcat(Cfg.szCfgFileName,szCfgFileName);

	if(!FileExists(Cfg.szCfgFileName)){
#ifdef TRACE
	trace_out_vstr_date(1,"Read Configuration : file %s not present",Cfg.szCfgFileName);
#endif
		return FALSE;
	}

	/* CFGItems viene dichiarato e inizializzato in mainvar.c */
	ReadCfgItems(CFGItems,Cfg.szCfgFileName,0);

	/*
	* Lettura dati processi
	*/
	if(bReadProcInfo){
		ReadProcInfo(ProcList,Cfg.szCfgFileName);
	}
	
	return TRUE;
}


void ExitApp()
{
#ifdef TRACE
	trace_out_vstr_date(1,"Stopped");
#endif

	/* rimuovo il timer per gestione eventi*/
	if(Cfg.nTimer) g_source_remove(Cfg.nTimer);
	if(Cfg.nBlinkingTimer) g_source_remove(Cfg.nBlinkingTimer);

#ifdef STATISTICS
	if(DBConnectionActive()){
		DBDisconnect();
	}
#endif
	FreeTablesInfo();

	if (!disconnect_linea_shm(&pDatiLinea)){
			trace_out_vstr(1,"Error Disconnecting From Shared Memory (%d)", Cfg.nLineaShmKey);
	}
	if (!disconnect_settori_shm(&pSettori)){
			trace_out_vstr(1,"Error Disconnecting From Shared Memory (%d)", Cfg.nSettoriShmKey);
	}

	DeleteMsgQ(DisplayList[Cfg.nSettoreIndex].nQNumber);

	/* chiudo la GUI */
	gtk_widget_destroy(main_window);
	gtk_main_quit();
}


BOOL connect_linea_shm(PLINEA_STRUCT *pDatiLinea)
{
	BOOL bOK=TRUE;
	/*
	* Creo la shared memory per la gestione della linea
	*/
	if((*pDatiLinea=(PLINEA_STRUCT)GetShm(Cfg.nLineaShmKey,sizeof(LINEA_STRUCT)))==NULL){
#ifdef TRACE
		trace_out_vstr_date(1,"Cannot Allocate Shared Memory [%d] !", Cfg.nLineaShmKey);
#endif
		bOK=FALSE;
	} else {
		//???pDatiLinea.nStatoLinea=LINEA_IN_STOP;
	}

	return bOK;
}



BOOL connect_settori_shm(PSETTORE *pSettori)
{
	BOOL bOK=TRUE;

	/*
	* Leggo la shared memory per la gestione della linea (SETTORI)
	*/
	if((*pSettori=(PSETTORE)GetShm(Cfg.nSettoriShmKey,Cfg.nNumeroSettori*sizeof(SETTORE)))==NULL){
#ifdef TRACE
		trace_out_vstr_date(1,"Cannot Get Shared Memory [%d] (SETTORI)", Cfg.nSettoriShmKey);
#endif
		bOK=FALSE;
	}
	return bOK;
}

BOOL disconnect_linea_shm(PLINEA_STRUCT *pDatiLinea)
{
	if(pDatiLinea){
		DetachShm((char *)*pDatiLinea);

		*pDatiLinea=(PLINEA_STRUCT)NULL;
		return TRUE;
	}
	return FALSE;
}

BOOL disconnect_settori_shm(PSETTORE *pSettori)
{
	if(pSettori){
		DetachShm((char *)*pSettori);

		*pSettori=(PSETTORE)NULL;
		return TRUE;
	}
	return FALSE;
}

void RefreshDisplayAsVoid(void)
{
	char szBuffer[256];
	strcpy(szBuffer,Cfg.szPathData);
	strcat(szBuffer,"/");
	strcat(szBuffer,Cfg.szDisplayCfgFileName);
	
	gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_ordine_collo")),szBuffer,"");
	gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_codice_operatore")),szBuffer," ");
	gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_dati_operatore")),szBuffer," ");
	gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_operazione")),szBuffer,"");
	gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_copie")),szBuffer,"");
	//gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_stato_linea")),szBuffer,"");
	gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_ubicazione")),szBuffer,"");
	gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_codice")),szBuffer,"");
	gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_titolo")),szBuffer,"");
}


void RefreshAsDisconnected(void)
{
	char szBuffer[256];
	strcpy(szBuffer,Cfg.szPathData);
	strcat(szBuffer,"/");
	strcat(szBuffer,Cfg.szDisplayCfgFileName);
	
	gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_ordine_collo")),szBuffer,"------ / ---");
	gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_codice_operatore")),szBuffer,"--");
	gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_dati_operatore")),szBuffer,"------");
	gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_operazione")),szBuffer,"----");
	gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_copie")),szBuffer,"---");
	gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_stato_linea")),szBuffer,"-----");
	gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_ubicazione")),szBuffer,"---------");
	gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_codice")),szBuffer,"---------");
	gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_titolo")),szBuffer,"---------");
}

void RefreshSettoreElements(void)
{
	char szBuffer[256];
	char szOrdine[256];
	strcpy(szBuffer,Cfg.szPathData);
	strcat(szBuffer,"/");
	strcat(szBuffer,Cfg.szDisplayCfgFileName);

	if(strlen(pSettori[Cfg.nSettoreIndex].szCodSped)){
		strcpy(szOrdine,SubStr(pSettori[Cfg.nSettoreIndex].szCodSped,4,6));
		strcat(szOrdine,"/");
		strcat(szOrdine,SubStr(pSettori[Cfg.nSettoreIndex].szCodSped,14,2));
	} else {
		strcpy(szOrdine," ");
	}

	
	gtk_widget_hide(get_widget(main_window,"frame_lettura_barcode_id"));

	gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_codice_operatore")),szBuffer,"%s",pSettori[Cfg.nSettoreIndex].szCodOperatore);
	gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_dati_operatore")),szBuffer,"%s",pSettori[Cfg.nSettoreIndex].szCognomeOperatore);
	
	if(pSettori[Cfg.nSettoreIndex].nAttesaBadge){
		gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_ordine_collo")),szBuffer," ");
		gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_operazione")),szBuffer,"ATTESA BADGE");
		gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_copie")),szBuffer," ");
		gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_codice_operatore")),szBuffer," ");
		gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_dati_operatore")),szBuffer," ");
	} else {

		switch(pSettori[Cfg.nSettoreIndex].nStatoSettore){

			case ATTESA:
				gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_ordine_collo")),szBuffer," ");
				gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_operazione")),szBuffer,"    ");
				gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_copie")),szBuffer," ");

			break;
			case PRELIEVO_IMBALLI:
				gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_ordine_collo")),szBuffer,"%s-%d",szOrdine,pSettori[Cfg.nSettoreIndex].nNumCollo);
				gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_operazione")),szBuffer,"IMB");
				gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_copie")),szBuffer,"%d",pSettori[Cfg.nSettoreIndex].nNumCollo);
				
			break;
			case PRELIEVO_COPIE:
				gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_ordine_collo")),szBuffer,"%s-%d",szOrdine,pSettori[Cfg.nSettoreIndex].nNumCollo);
				gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_operazione")),szBuffer,"PREL");
				gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_copie")),szBuffer,"%d",pSettori[Cfg.nSettoreIndex].nNumCopie);
				
			break;
			case PASSA_SETT_SUCC:
				gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_ordine_collo")),szBuffer,"%s-%d",szOrdine,pSettori[Cfg.nSettoreIndex].nNumCollo);
				gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_operazione")),szBuffer,"PASSA");
				gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_copie")),szBuffer," ");
			break;
			case CHIUSURA_COLLO:
				gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_ordine_collo")),szBuffer,"%s-%d",szOrdine,pSettori[Cfg.nSettoreIndex].nNumCollo);
				gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_operazione")),szBuffer,"CAMBIA");
				gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_copie")),szBuffer," ");
				
			break;
			case FINE_ORDINE:
				gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_ordine_collo")),szBuffer,"%s-%d",szOrdine,pSettori[Cfg.nSettoreIndex].nNumCollo);
				gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_operazione")),szBuffer,"FINE");
				gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_copie")),szBuffer," ");
				
			break;
			case DISABILITATO:
				gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_operazione")),szBuffer,"OFFLINE");
				gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_copie")),szBuffer," ");
				
			break;
			// BRI
			case LETTURA_BARCODE_ID:
				gtk_widget_show(get_widget(main_window,"frame_lettura_barcode_id"));
				gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_ordine_collo")),szBuffer,"%s-%d",szOrdine,pSettori[Cfg.nSettoreIndex].nNumCollo);
				gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_operazione")),szBuffer,"BRCD");
				gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_copie")),szBuffer,"%d",pSettori[Cfg.nSettoreIndex].nNumCopie);
				
			break;
		}
	}
	
}

void RefreshDatiProdottoElements(void)
{
	char szBuffer[256];
	strcpy(szBuffer,Cfg.szPathData);
	strcat(szBuffer,"/");
	strcat(szBuffer,Cfg.szDisplayCfgFileName);
	GtkWidget *ImgFronte;
	GtkWidget *ImgUp;

	/* simulo un 'lampeggio' */
	gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_ubicazione")),szBuffer," ");
	gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_codice")),szBuffer," ");
	gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_titolo")),szBuffer," ");

	gtk_update();

	gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_ubicazione")),szBuffer,"%s ",pSettori[Cfg.nSettoreIndex].szCodUbi);
	gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_codice")),szBuffer,"%s ",pSettori[Cfg.nSettoreIndex].szCodProd);
	gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_titolo")),szBuffer,"%s ",LeftStr(pSettori[Cfg.nSettoreIndex].szTitoloProd,20));

	gtk_update();

	ImgFronte=get_widget(main_window,"img_fronte");
	ImgUp=get_widget(main_window,"img_up");

	gtk_image_set_from_file(GTK_IMAGE(get_widget(main_window,"img_up")),"../pixmaps/none.png");
	gtk_image_set_from_file(GTK_IMAGE(get_widget(main_window,"img_fronte")),"../pixmaps/none.png");

	gtk_update();


	switch(pSettori[Cfg.nSettoreIndex].nStatoSettore){
		case ATTESA:
		break;
		case PRELIEVO_IMBALLI:


			gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_ubicazione")),szBuffer,"%s ",pSettori[Cfg.nSettoreIndex].szCodUbi);
			gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_codice")),szBuffer,"%s ",pSettori[Cfg.nSettoreIndex].szCodProd);
			gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_titolo")),szBuffer,"Imballo %s",pSettori[Cfg.nSettoreIndex].szCodProd);

			if(pSettori[Cfg.nSettoreIndex].nFronte==2){
				ImgFronte=get_widget(main_window,"img_fronte");
				ImgUp=get_widget(main_window,"img_up");
			} else {
				ImgFronte=get_widget(main_window,"img_up");
				ImgUp=get_widget(main_window,"img_fronte");
			}

			if(pSettori[Cfg.nSettoreIndex].nFronte==2){
				gtk_image_set_from_file(GTK_IMAGE(ImgFronte),"../pixmaps/left.png");
			} else {
				gtk_image_set_from_file(GTK_IMAGE(ImgFronte),"../pixmaps/right.png");
			}

			if(pSettori[Cfg.nSettoreIndex].nInPiedi){
				gtk_image_set_from_file(GTK_IMAGE(ImgUp),"../pixmaps/up.png");
			} else {
				gtk_image_set_from_file(GTK_IMAGE(ImgUp),"../pixmaps/none.png");
			}

			/*
			* Riutilizzo il posto per i prodotti in piedi
			* per gestire la segnalazione di imballo speciale
			*/
			if (pSettori[Cfg.nSettoreIndex].szImballoSpeciale[0]==IMBALLO_SPECIALE_ON) {
				gtk_image_set_from_file(GTK_IMAGE(ImgUp),"../pixmaps/imballo-speciale.png");
			} else {
				gtk_image_set_from_file(GTK_IMAGE(ImgUp),"../pixmaps/none.png");
			}
		break;
		case PRELIEVO_COPIE:
			gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_ubicazione")),szBuffer,"%s ",pSettori[Cfg.nSettoreIndex].szCodUbi);
			gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_codice")),szBuffer,"%s ",pSettori[Cfg.nSettoreIndex].szCodProd);
			gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_titolo")),szBuffer,"%s ",LeftStr(pSettori[Cfg.nSettoreIndex].szTitoloProd,20));

			if(pSettori[Cfg.nSettoreIndex].nFronte==2){
				ImgFronte=get_widget(main_window,"img_fronte");
				ImgUp=get_widget(main_window,"img_up");
			} else {
				ImgFronte=get_widget(main_window,"img_up");
				ImgUp=get_widget(main_window,"img_fronte");
			}

			if(pSettori[Cfg.nSettoreIndex].nInPiedi){
				gtk_image_set_from_file(GTK_IMAGE(ImgUp),"../pixmaps/up.png");
			} else {
				gtk_image_set_from_file(GTK_IMAGE(ImgUp),"../pixmaps/none.png");
			}
			if(pSettori[Cfg.nSettoreIndex].nFronte==2){
				gtk_image_set_from_file(GTK_IMAGE(ImgFronte),"../pixmaps/left.png");
			} else {
				gtk_image_set_from_file(GTK_IMAGE(ImgFronte),"../pixmaps/right.png");
			}
		break;
		case PASSA_SETT_SUCC:
		break;
		case CHIUSURA_COLLO:
		break;
		case FINE_ORDINE:
		break;
		case DISABILITATO:
		break;
		case LETTURA_BARCODE_ID:
			gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_qt_barcodeid")),szBuffer,"%d",pSettori[Cfg.nSettoreIndex].nQtBarcodeId);

			if (pSettori[Cfg.nSettoreIndex].szFlagBarcodeId[0]==CONTROLLO_BARCODE_OK || pSettori[Cfg.nSettoreIndex].szFlagBarcodeId[0]==CONTROLLO_BARCODE_FORZATO_PARZIALE || pSettori[Cfg.nSettoreIndex].szFlagBarcodeId[0]==CONTROLLO_BARCODE_DISABILITATO) {
				gtk_widget_set_background(get_widget(main_window,"eb_stato_barcodeid"),&cLGREEN);
				gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_stato_barcodeid")),szBuffer,"FINE LETTURE");
			} else if (pSettori[Cfg.nSettoreIndex].szFlagBarcodeId[0]==CONTROLLO_BARCODE_INCOMPLETO){
				gtk_widget_set_background(get_widget(main_window,"eb_stato_barcodeid"),&cLGREEN);
				gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_stato_barcodeid")),szBuffer,"LEGGI BARCODE");
			} else {
				gtk_widget_set_background(get_widget(main_window,"eb_stato_barcodeid"),&cLGREEN);
				gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_stato_barcodeid")),szBuffer," ");
			}
			if (!strcmp(pSettori[Cfg.nSettoreIndex].szBarcodeIdLetto,BARCODE_ERRORE)) {
				gtk_widget_set_background(get_widget(main_window,"eb_stato_barcodeid"),&cRED);
				gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_stato_barcodeid")),szBuffer,"GIA' LETTO");
			} else if (!strcmp(pSettori[Cfg.nSettoreIndex].szBarcodeIdLetto,BARCODE_QT_SUPERATA)) {
				gtk_widget_set_background(get_widget(main_window,"eb_stato_barcodeid"),&cRED);
				gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_stato_barcodeid")),szBuffer,"QT. SUPERATA");
			} else if (!strcmp(pSettori[Cfg.nSettoreIndex].szBarcodeIdLetto,BARCODE_LUNGH_ERRATA)) {
				gtk_widget_set_background(get_widget(main_window,"eb_stato_barcodeid"),&cRED);
				gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_stato_barcodeid")),szBuffer,"ERR. LUNGH");
			}
			
			if (pSettori[Cfg.nSettoreIndex].nQtBarcodeId>=pSettori[Cfg.nSettoreIndex].nNumCopie){
				gtk_widget_set_background(get_widget(main_window,"eb_qt_barcodeid"),&cLGREEN);
			} else {
				gtk_widget_set_background(get_widget(main_window,"eb_qt_barcodeid"),&cYELLOW);
			}
		break;
	}

	gtk_update();

	
}

void RefreshStatisticheElements(void)
{
#ifdef STATISTICS
	char szBuffer[256];
	DBresult *DBRes;
	int nTuples;
	strcpy(szBuffer,Cfg.szPathData);
	strcat(szBuffer,"/");
	strcat(szBuffer,Cfg.szDisplayCfgFileName);

	DBRes=DBExecQuery(Cfg.nDebugVersion>1,"select count(distinct ordprog), count(distinct ordprog||rpnmcol), count(*), sum(rpqtspe) from rig_prod,ubicazioni where rpcdubi=ubicazione and isola=%d and settore=%d and rpstato='%c';",Cfg.nIsola,Cfg.nSettore,RIGA_ELABORATA);
	if((nTuples=DBntuples(DBRes))){
		gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_stats_ordini_da_prelevare")),szBuffer,"%d",atoi(DBgetvalue(DBRes,0,0)));
		gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_stats_colli_da_prelevare")),szBuffer,"%d",atoi(DBgetvalue(DBRes,0,1)));
		gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_stats_righe_da_prelevare")),szBuffer,"%d",atoi(DBgetvalue(DBRes,0,2)));
		gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_stats_copie_da_prelevare")),szBuffer,"%d",atoi(DBgetvalue(DBRes,0,3)));
	}

	
	DBRes=DBExecQuery(Cfg.nDebugVersion>1,"select count(distinct ordprog), count(distinct ordprog||rpnmcol), count(*), sum(rpqtspe) from rig_prod,ubicazioni where rpcdubi=ubicazione and settore='%3d' and (rpstato='%c' or rpstato='%c');",Cfg.nIsola,Cfg.nSettore,RIGA_PRELEVATA,RIGA_EVASA);
	if((nTuples=DBntuples(DBRes))){
		gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_stats_ordini_prelevati")),szBuffer,"%d",atoi(DBgetvalue(DBRes,0,0)));
		gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_stats_colli_prelevati")),szBuffer,"%d",atoi(DBgetvalue(DBRes,0,1)));
		gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_stats_righe_prelevati")),szBuffer,"%d",atoi(DBgetvalue(DBRes,0,2)));
		gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_stats_copie_prelevati")),szBuffer,"%d",atoi(DBgetvalue(DBRes,0,3)));
	}
#endif
	
}

void RefreshLineaElements(void)
{
	char szBuffer[256];
	int nStatoLinea=LINEA_IN_STOP;
	strcpy(szBuffer,Cfg.szPathData);
	strcat(szBuffer,"/");
	strcat(szBuffer,Cfg.szDisplayCfgFileName);

	if(Cfg.nIsola==1){
		nStatoLinea=pDatiLinea->nStatoLinea_1;
	} else  if(Cfg.nIsola==2){
		nStatoLinea=pDatiLinea->nStatoLinea_2;
	}
	if(nStatoLinea==LINEA_IN_START){
		gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_stato_linea")),szBuffer,"START");
		gtk_widget_set_background(get_widget(main_window,"lb_stato_linea"),&cLGREEN);
	} else if(nStatoLinea==LINEA_IN_STOP){
		gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_stato_linea")),szBuffer,"STOP");
		gtk_widget_set_background(get_widget(main_window,"lb_stato_linea"),&cRED);
	} else {
		gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_stato_linea")),szBuffer,"WAIT");
		gtk_widget_set_background(get_widget(main_window,"lb_stato_linea"),&cORANGE);
	}
}

void RefreshStaticElements(void)
{
	char szBuffer[256];
	strcpy(szBuffer,Cfg.szPathData);
	strcat(szBuffer,"/");
	strcat(szBuffer,Cfg.szDisplayCfgFileName);
	
	gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_titolo_ordine_collo")),szBuffer,"ORDINE - COLLO");
	gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_titolo_settore")),szBuffer,"SETTORE");
	gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_settore")),szBuffer,"%2d",Cfg.nSettore);
	gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_titolo_operatore")),szBuffer,"OPERATORE");
	gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_titolo_ubicazione")),szBuffer,"UBICAZIONE");
	gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_titolo_codice")),szBuffer,"CODICE");
	gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_titolo_titolo")),szBuffer,"TITOLO");
	
	gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_stats_titolo_da_prelevare")),szBuffer,"DA PRELEVARE");
	gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_stats_titolo_prelevati")),szBuffer,"PRELEVATI");
	gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_stats_titolo_ordini")),szBuffer,"ORDINI");
	gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_stats_titolo_colli")),szBuffer,"COLLI");
	gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_stats_titolo_righe")),szBuffer,"RIGHE");
	gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_stats_titolo_copie")),szBuffer,"COPIE");
}

gint Blinking( gpointer data )
{
	static BOOL bOpBlinkOn;

	if(pSettori){
		if(pSettori[Cfg.nSettoreIndex].nOpBlinking==BLINK){

			if (bOpBlinkOn) {
				gtk_widget_set_background(get_widget(main_window,"eb_operazione"),&cRED);
				gtk_widget_set_background(get_widget(main_window,"eb_copie"),&cRED);
			} else {
				gtk_widget_set_background(get_widget(main_window,"eb_operazione"),&cBLACK);
				gtk_widget_set_background(get_widget(main_window,"eb_copie"),&cBLACK);
			}
			bOpBlinkOn=(!bOpBlinkOn);
			return(TRUE);
		}
	}
	
	gtk_widget_set_background(get_widget(main_window,"eb_operazione"),&cBLACK);
	gtk_widget_set_background(get_widget(main_window,"eb_copie"),&cBLACK);

	/* 
	* As this is a timeout function, return TRUE so that it
	* continues to get called 
	*/
	return(TRUE);
}

int GetSettore(int nIsola,int nSettore)
{
	int nIndex;

	if(pSettori){
		for(nIndex=0;nIndex<Cfg.nNumeroSettori;nIndex++){
			if(pSettori[nIndex].nIsola==nIsola && pSettori[nIndex].nSettore==nSettore){
				return nIndex;
			}
		}
	}
	return -1;
}

#ifdef DA_FARE
	char szUbicazioneSettori[256];
	char szUbicazione[256];

	strcpy(szUbicazioneSettori, pSettori[Cfg.nSettore].szCodUbi);


	if(strlen(pSettori[Cfg.nSettore].szCodUbi)){
		strncpy(szUbicazione, szUbicazioneSettori,1);szUbicazione[1]='\0';
		strcat(szUbicazione, ".");
		strncat(szUbicazione, szUbicazioneSettori+1,2 );szUbicazione[4]='\0';
		strcat(szUbicazione, ".");
		strncat(szUbicazione, szUbicazioneSettori+3,1 );szUbicazione[6]='\0';
		strcat(szUbicazione, ".");
		strncat(szUbicazione, szUbicazioneSettori+4,2 );szUbicazione[9]='\0';
		strcat(szUbicazione, ".");
		strncat(szUbicazione, szUbicazioneSettori+6,1 );szUbicazione[11]='\0';
	} else {
		strncpy(szUbicazione, "");
	}
#endif
