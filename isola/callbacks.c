/*
* callbacks.c
* Callbacks functions
* Easy Picking 3.0
* Copyright CNI srl 1999-2000
* Autori : Roberto Mantovani
*          Stefano Tarroni
*/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#ifdef TRACE
#include <trace.h>
#endif

#include <gtk/gtk.h>
#include <libpq-fe.h>
#include <time.h>

#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

#include <proc_list.h>
#include <pmx_msq.h>
#include <ca_file.h>
#include <dbfun.h>

#include "picking.h"
#include "mainstruct.h"
#include "mainext.h"
#include "mainfun.h"


/*
* Callback di selezione riga in lista ordini
*/
void on_lst_ordini_select_row(GtkCList *clist, gint row, gint column, GdkEvent *event, gpointer user_data)
{
	nLstOrdiniRowSelected=row;
}

gboolean on_main_window_destroy_event(GtkWidget * widget, GdkEvent * event, gpointer user_data)
{
	dlg_exit = create_dlg_exit();
	gtk_window_set_focus (GTK_WINDOW (dlg_exit), get_widget(dlg_exit,"pb_ok"));
	gtk_widget_show(dlg_exit);
	gtk_window_set_transient_for(GTK_WINDOW(dlg_exit),GTK_WINDOW(main_window));
  return TRUE;
}

/*
* void on_lst_sort_column_clicked (GtkCList *clist, gint nColumnNumber, gpointer user_data)
*
* Effettua un ordinamento sulla clist : utilizza la colonna (nColumnNumber) selezionata come discriminante;
* se la stessa colonna viene selezionanata di seguito l'ordinamento varia da ascendente a discentente.
*/
void on_lst_sort_column_clicked (GtkCList *clist, gint nColumnNumber, gpointer user_data)
{
	static int nOldColumnNumber=-1;
	static int nOrd;

	if (nColumnNumber!=nOldColumnNumber){
		nOrd=0;
		nOldColumnNumber=nColumnNumber;
	} else {
		nOrd++;
	}
	gtk_clist_set_sort_type(GTK_CLIST(clist),(nOrd%2==0)?GTK_SORT_ASCENDING:GTK_SORT_DESCENDING);
	gtk_clist_set_sort_column(GTK_CLIST(clist), nColumnNumber);
	gtk_clist_sort(GTK_CLIST(clist));
}

void on_mb_main_activate_current(GtkMenuShell * menushell, gboolean force_hide, gpointer user_data)
{

}


void on_pbm_exit_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	dlg_exit = create_dlg_exit();
	gtk_window_set_focus (GTK_WINDOW (dlg_exit), get_widget(dlg_exit,"pb_ok"));
	gtk_widget_show(dlg_exit);
	gtk_window_set_transient_for(GTK_WINDOW(dlg_exit),GTK_WINDOW(main_window));

}


void on_pbm_aggiorna_lista_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *sw_list=get_widget(main_window,"sw_list");

	RefreshListaOrdini(sw_list,NULL);

}


void on_pbm_visualizza_messaggi_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	static int ncounter=0;

	ncounter++;
	if (ncounter%2){
		gtk_widget_hide(GTK_WIDGET(get_widget(main_window,"sw_msgs")));
	} else {
		gtk_widget_show(GTK_WIDGET(get_widget(main_window,"sw_msgs")));
	}

}


void on_pbm_configurazione_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	EditConfigurazione();

}


void on_pbm_stampa_etichette_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *dlg_stampa_ordine=create_dlg_stampa_ordine();
	GList *lista;
	char *pszOrdineKey;
	char szOrdineKey[128];
	int nRowIndex;


	lista=g_list_first(GTK_CLIST(lst_ordini)->selection);

	if(lista==NULL){
		gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_stampa_ordine,"entry_ordine")),"");
	} else {
		nRowIndex=GPOINTER_TO_INT(lista->data);
		gtk_clist_get_text(GTK_CLIST(lst_ordini),nRowIndex,Cfg.nOrdineKeyField,&pszOrdineKey);
		strcpy(szOrdineKey,pszOrdineKey);
		gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_stampa_ordine,"entry_ordine")),szOrdineKey);
	}

	gtk_window_set_title(GTK_WINDOW(dlg_stampa_ordine),"Stampa Etichette");
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(get_widget(dlg_stampa_ordine,"sb_collo")),0);

	gtk_widget_show(dlg_stampa_ordine);
	gtk_window_set_focus (GTK_WINDOW (dlg_stampa_ordine), get_widget(dlg_stampa_ordine,"entry_ordine"));
	gtk_window_set_transient_for(GTK_WINDOW(dlg_stampa_ordine),GTK_WINDOW(main_window));
	gtk_signal_connect (GTK_OBJECT (get_widget(dlg_stampa_ordine,"pb_ok")), "clicked", GTK_SIGNAL_FUNC (do_stampa_etichette), dlg_stampa_ordine);

}


void on_pbm_stampa_ordine_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *dlg_stampa_ordine=create_dlg_stampa_ordine();
	GList *lista;
	char *pszOrdineKey;
	char szOrdineKey[128];
	int nRowIndex;


	lista=g_list_first(GTK_CLIST(lst_ordini)->selection);

	if(lista==NULL){
		gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_stampa_ordine,"entry_ordine")),"");
	} else {
		nRowIndex=GPOINTER_TO_INT(lista->data);
		gtk_clist_get_text(GTK_CLIST(lst_ordini),nRowIndex,Cfg.nOrdineKeyField,&pszOrdineKey);
		strcpy(szOrdineKey,pszOrdineKey);
		gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_stampa_ordine,"entry_ordine")),szOrdineKey);
	}

	gtk_window_set_title(GTK_WINDOW(dlg_stampa_ordine),"Stampa Ordine");
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(get_widget(dlg_stampa_ordine,"sb_collo")),0);

	gtk_widget_show(dlg_stampa_ordine);
	gtk_window_set_focus (GTK_WINDOW (dlg_stampa_ordine), get_widget(dlg_stampa_ordine,"entry_ordine"));
	gtk_window_set_transient_for(GTK_WINDOW(dlg_stampa_ordine),GTK_WINDOW(main_window));
	gtk_signal_connect (GTK_OBJECT (get_widget(dlg_stampa_ordine,"pb_ok")), "clicked", GTK_SIGNAL_FUNC (do_stampa_packing_list), dlg_stampa_ordine);
}

void on_pbm_stampa_pdf_activate             (GtkMenuItem     *menuitem, gpointer         user_data)
{
	GtkWidget *dlg_stampa_ordine=create_dlg_stampa_ordine();
	GList *lista;
	char *pszOrdineKey;
	char szOrdineKey[128];
	int nRowIndex;


	lista=g_list_first(GTK_CLIST(lst_ordini)->selection);

	if(lista==NULL){
		gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_stampa_ordine,"entry_ordine")),"");
	} else {
		nRowIndex=GPOINTER_TO_INT(lista->data);
		gtk_clist_get_text(GTK_CLIST(lst_ordini),nRowIndex,Cfg.nOrdineKeyField,&pszOrdineKey);
		strcpy(szOrdineKey,pszOrdineKey);
		gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_stampa_ordine,"entry_ordine")),szOrdineKey);
	}

	gtk_window_set_title(GTK_WINDOW(dlg_stampa_ordine),"Stampa Documento PDF");
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(get_widget(dlg_stampa_ordine,"sb_collo")),0);

	gtk_widget_show(dlg_stampa_ordine);
	gtk_window_set_focus (GTK_WINDOW (dlg_stampa_ordine), get_widget(dlg_stampa_ordine,"entry_ordine"));
	gtk_window_set_transient_for(GTK_WINDOW(dlg_stampa_ordine),GTK_WINDOW(main_window));
	gtk_signal_connect (GTK_OBJECT (get_widget(dlg_stampa_ordine,"pb_ok")), "clicked", GTK_SIGNAL_FUNC (do_stampa_pdf), dlg_stampa_ordine);

}


void on_pbm_edit_ordine_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	char *pszOrdineKey;

	dlg_edit_ordine = create_dlg_edit_ordine();

	gtk_clist_get_text(GTK_CLIST(lst_ordini),nLstOrdiniRowSelected,Cfg.nOrdineKeyField,&pszOrdineKey);
	strcpy(szOrdineInEdit,pszOrdineKey);
	gtk_window_set_focus (GTK_WINDOW (dlg_edit_ordine), get_widget(dlg_edit_ordine,"pb_ok"));
	gtk_widget_show(dlg_edit_ordine);
	gtk_window_set_transient_for(GTK_WINDOW(dlg_edit_ordine),GTK_WINDOW(main_window));

	EditOrdine(pszOrdineKey);

}



void on_pbm_about_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *dlg_about = create_dlg_about();
	gtk_window_set_focus (GTK_WINDOW (dlg_about), get_widget(dlg_about,"pb_ok"));
	gtk_widget_show(dlg_about);
	gtk_window_set_transient_for(GTK_WINDOW(dlg_about),GTK_WINDOW(main_window));

}


void on_pbm_help_activate(GtkMenuItem * menuitem, gpointer user_data)
{

}


void on_main_pb_config_clicked(GtkButton * button, gpointer user_data)
{
	EditConfigurazione();

}


void on_main_pb_print_clicked(GtkButton * button, gpointer user_data)
{

}


void on_main_pb_help_clicked(GtkButton * button, gpointer user_data)
{

}


void on_main_pb_edit_clicked(GtkButton * button, gpointer user_data)
{
	char *pszOrdineKey;

	dlg_edit_ordine = create_dlg_edit_ordine();

	gtk_clist_get_text(GTK_CLIST(lst_ordini),nLstOrdiniRowSelected,Cfg.nOrdineKeyField,&pszOrdineKey);
	strcpy(szOrdineInEdit,pszOrdineKey);
	gtk_window_set_focus (GTK_WINDOW (dlg_edit_ordine), get_widget(dlg_edit_ordine,"pb_ok"));
	gtk_widget_show(dlg_edit_ordine);
	gtk_window_set_transient_for(GTK_WINDOW(dlg_edit_ordine),GTK_WINDOW(main_window));

	EditOrdine(pszOrdineKey);

}


void on_main_pb_exit_clicked(GtkButton * button, gpointer user_data)
{
	dlg_exit = create_dlg_exit();
	gtk_window_set_focus (GTK_WINDOW (dlg_exit), get_widget(dlg_exit,"pb_ok"));
	gtk_widget_show(dlg_exit);
	gtk_window_set_transient_for(GTK_WINDOW(dlg_exit),GTK_WINDOW(main_window));

}


void on_dlg_about_pb_ok_clicked(GtkButton * button, gpointer user_data)
{
	gtk_widget_destroy(GTK_WIDGET(user_data));

}


void on_dlg_exit_pb_ok_clicked(GtkButton * button, gpointer user_data)
{
	ExitApp();

}


void on_dlg_exit_pb_cancel_clicked(GtkButton * button, gpointer user_data)
{
	gtk_widget_destroy(dlg_exit);
	dlg_exit=0;

}


void on_dlg_message_pb_cancel_clicked(GtkButton * button, gpointer user_data)
{

}


void on_dlg_edit_ordine_pb_ok_clicked(GtkButton * button, gpointer user_data)
{
	gtk_widget_destroy(dlg_edit_ordine);
	dlg_edit_ordine=0;

}


void on_dlg_edit_ordine_pb_edit_collo_clicked(GtkButton * button, gpointer user_data)
{
	static char szOrdineKey[80];
	GtkWidget *lst_edit_ordine;
	char *pszText;
	int nCollo;

	dlg_edit_collo=create_dlg_edit_collo();

	if((lst_edit_ordine=get_widget(dlg_edit_ordine,"lst_edit_ordine"))){
		if(GTK_CLIST(lst_edit_ordine)->selection){
			nColloInEdit=GPOINTER_TO_INT(GTK_CLIST(lst_edit_ordine)->selection->data);
		} else {
			nColloInEdit=0;
		}
		gtk_clist_get_text(GTK_CLIST(lst_edit_ordine),nColloInEdit,0,&pszText);
		strcpy(szOrdineKey,pszText);
		gtk_clist_get_text(GTK_CLIST(lst_edit_ordine),nColloInEdit,2,&pszText);
		nCollo=atoi(pszText);
		gtk_window_set_focus (GTK_WINDOW (dlg_edit_collo), get_widget(dlg_edit_collo,"pb_ok"));
		gtk_widget_show(dlg_edit_collo);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_edit_collo),GTK_WINDOW(dlg_edit_ordine));

		EditCollo(szOrdineKey,nCollo);
	}

}


void on_dlg_edit_ordine_pb_prev_clicked(GtkButton * button, gpointer user_data)
{
	int nOrdineIndex;
	char *pszOrdine;

	nOrdineIndex=GetOrdineIndex(szOrdineInEdit);

	if(nOrdineIndex>0){
		gtk_clist_get_text(GTK_CLIST(lst_ordini),nOrdineIndex-1,Cfg.nOrdineKeyField,&pszOrdine);
		strcpy(szOrdineInEdit,pszOrdine);
		EditOrdine(szOrdineInEdit);
		gtk_widget_set_sensitive(get_widget(dlg_edit_ordine,"pb_prev"),TRUE);
		gtk_widget_set_sensitive(get_widget(dlg_edit_ordine,"pb_next"),TRUE);
		gtk_update();
	} else {
		gtk_widget_set_sensitive(get_widget(dlg_edit_ordine,"pb_prev"),FALSE);
	}

}


void on_dlg_edit_ordine_pb_next_clicked(GtkButton * button, gpointer user_data)
{
	int nOrdineIndex;
	char *pszOrdine;

	nOrdineIndex=GetOrdineIndex(szOrdineInEdit);

	if(nOrdineIndex<(GTK_CLIST(lst_ordini)->rows)-1){
		gtk_clist_get_text(GTK_CLIST(lst_ordini),nOrdineIndex+1,Cfg.nOrdineKeyField,&pszOrdine);
		strcpy(szOrdineInEdit,pszOrdine);
		EditOrdine(szOrdineInEdit);
		gtk_widget_set_sensitive(get_widget(dlg_edit_ordine,"pb_next"),TRUE);
		gtk_widget_set_sensitive(get_widget(dlg_edit_ordine,"pb_prev"),TRUE);
		gtk_update();
	} else {
		gtk_widget_set_sensitive(get_widget(dlg_edit_ordine,"pb_next"),FALSE);
	}

}


void on_dlg_edit_ordine_pb_cancel_clicked(GtkButton * button, gpointer user_data)
{
	gtk_widget_destroy(dlg_edit_ordine);
	dlg_edit_ordine=0;

}


void on_dlg_config_pb_ok_clicked(GtkButton * button, gpointer user_data)
{
	gtk_widget_destroy(GTK_WIDGET(user_data));

}


void on_dlg_config_pb_apply_clicked(GtkButton * button, gpointer user_data)
{
	ApplyConfigurazione();
}


void on_dlg_config_pb_cancel_clicked(GtkButton * button, gpointer user_data)
{
	gtk_widget_destroy(GTK_WIDGET(user_data));

}


void on_dlg_edit_collo_pb_ok_clicked(GtkButton * button, gpointer user_data)
{
	gtk_widget_destroy(dlg_edit_collo);
	dlg_edit_collo=0;

}


void on_dlg_edit_collo_pb_prev_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *lst_edit_ordine;
	int nCollo;
	char *pszText;
	char szOrdineKey[128];

	if((lst_edit_ordine=get_widget(dlg_edit_ordine,"lst_edit_ordine"))){
		gtk_clist_get_text(GTK_CLIST(lst_edit_ordine),nColloInEdit-1,0,&pszText);
		strcpy(szOrdineKey,pszText);
		gtk_clist_get_text(GTK_CLIST(lst_edit_ordine),nColloInEdit-1,2,&pszText);
		nCollo=atoi(pszText);

		if(EditCollo(szOrdineKey,nCollo)){
			nColloInEdit--;
			gtk_widget_set_sensitive(get_widget(dlg_edit_collo,"pb_next"),TRUE);
			gtk_widget_set_sensitive(get_widget(dlg_edit_collo,"pb_prev"),TRUE);
		} else {
			gtk_widget_set_sensitive(get_widget(dlg_edit_collo,"pb_prev"),FALSE);
		}
	}
}


void on_dlg_edit_collo_pb_next_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *lst_edit_ordine;
	int nCollo;
	char *pszText;
	char szOrdineKey[128];

	if((lst_edit_ordine=get_widget(dlg_edit_ordine,"lst_edit_ordine"))){
		gtk_clist_get_text(GTK_CLIST(lst_edit_ordine),nColloInEdit+1,0,&pszText);
		strcpy(szOrdineKey,pszText);
		gtk_clist_get_text(GTK_CLIST(lst_edit_ordine),nColloInEdit+1,2,&pszText);
		nCollo=atoi(pszText);

		if(EditCollo(szOrdineKey,nCollo)){
			nColloInEdit++;
			gtk_widget_set_sensitive(get_widget(dlg_edit_collo,"pb_next"),TRUE);
			gtk_widget_set_sensitive(get_widget(dlg_edit_collo,"pb_prev"),TRUE);
		} else {
			gtk_widget_set_sensitive(get_widget(dlg_edit_collo,"pb_next"),FALSE);
		}
	}

}


void on_dlg_edit_collo_pb_cancel_clicked(GtkButton * button, gpointer user_data)
{
	gtk_widget_destroy(dlg_edit_collo);
	dlg_edit_collo=0;

}


void on_dlg_app_error_pb_ok_clicked(GtkButton * button, gpointer user_data)
{
	/* 
	* chiudo la GUI 
	*/
	gtk_main_quit();

}

void do_stampa_packing_list     (GtkButton       *button, gpointer         user_data)
{
	char szOrdine[128];
	char szBuffer[128];
	int nCollo;
	PGresult *PGRes = NULL;
//	PGresult *PGRes = NULL;
//	int nTuples;
//	char szFLPDF[80];
//	char szFLFTZ[80];

	strcpy(szOrdine,gtk_entry_get_text(GTK_ENTRY(get_widget(GTK_WIDGET(user_data),"entry_ordine"))));
	nCollo = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(get_widget(user_data,"sb_collo")));

	if(nCollo){
		/*
		* Cerco i dati del collo specificato
		* ma per ora lascio tutto come prima
		*/
//		PGRes=PGExecSQL(FALSE,"select cpstato,ordkey,cpnmcol,cptpfor,cpflpdf,cpflftz from col_prod where ordkey='%s' AND cpnmcol=%d;",szOrdine,nCollo);
//		nTuples=PQntuples(PGRes);
//		strcpy(szFLPDF,PQgetvalue(PGRes,0,4));
//		strcpy(szFLFTZ,PQgetvalue(PGRes,0,5));
//		PQclear(PGRes);
//		
//		if (szFLPDF[0]==COLLO_CON_PDF){
//		} else {
//		}
	
		sprintf(szBuffer,"%s,%d,%s", szOrdine, nCollo, Cfg.szPrinterName);
		SendMessage(PROC_PRINTLABEL,PROC_ISOLA,PRINT_PACKING_LIST_COLLO,szBuffer);
	} else {
		sprintf(szBuffer,"%s,%s", szOrdine, Cfg.szPrinterName);
		SendMessage(PROC_PRINTLABEL,PROC_ISOLA,PRINT_PACKING_LIST,szBuffer);
		sprintf(szBuffer,"%s,%s,0", szOrdine, Cfg.szBollePrinterName);
		PGRes=PGExecSQL(FALSE,"insert into  tasks ( CODICE_TIPO_TASK, STRINGA_PARAMETRI, stato) values (1, '%s',' ');", szBuffer);
		PQclear(PGRes);


	}

	gtk_widget_destroy(GTK_WIDGET(user_data));
}

void do_stampa_etichette     (GtkButton       *button, gpointer         user_data)
{
	char szOrdine[128];
	char szBuffer[128];
	int nCollo;
	PGresult *PGRes = NULL;
	int nTuples;
	char szFLPDF[80];
	char szFLFTZ[80];

	strcpy(szOrdine,gtk_entry_get_text(GTK_ENTRY(get_widget(GTK_WIDGET(user_data),"entry_ordine"))));
	nCollo = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(get_widget(user_data,"sb_collo")));

	if(nCollo){
		/*
		* Cerco i dati del collo specificato
		* per capire che tipo di etichette stampare
		*/
		PGRes=PGExecSQL(TRUE,"select cpstato,ordkey,cpnmcol,cptpfor,cpflpdf,cpflftz from col_prod where ordkey='%s' AND cpnmcol=%d;",szOrdine,nCollo);
		nTuples=PQntuples(PGRes);
		if(nTuples){
			strcpy(szFLPDF,PQgetvalue(PGRes,0,4));
			strcpy(szFLFTZ,PQgetvalue(PGRes,0,5));
			PQclear(PGRes);
			
			if (szFLPDF[0]==COLLO_CON_PDF){
				sprintf(szBuffer,"%s,%s",szOrdine,Cfg.szLabelPrinterName);
				SendMessage(PROC_PRINTLABEL,PROC_ISOLA,PRINT_RAC_PDF,szBuffer);
			} else {
				sprintf(szBuffer,"%s,%d,%s", szOrdine, nCollo, Cfg.szLabelPrinterName);
				SendMessage(PROC_PRINTLABEL,PROC_ISOLA,PRINT_RAC_COLLO,szBuffer);
			}
		}
	} else {
		sprintf(szBuffer,"%s,%s", szOrdine, Cfg.szLabelPrinterName);
		SendMessage(PROC_PRINTLABEL,PROC_ISOLA,PRINT_RAC,szBuffer);
	}

	gtk_widget_destroy(GTK_WIDGET(user_data));
}

void do_stampa_pdf (GtkButton       *button, gpointer         user_data)
{
	char szOrdine[128];
	char szOrdKey[128];
	char szBuffer[128];
	//int nCollo;
	PGresult *PGRes = NULL;
	int nTuples;
	char szFLPDF[80];
	char szFLFTZ[80];
	BOOL bStampa=FALSE;

	strcpy(szOrdKey,gtk_entry_get_text(GTK_ENTRY(get_widget(GTK_WIDGET(user_data),"entry_ordine"))));
	//nCollo = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(get_widget(user_data,"sb_collo")));

	/*
	* Cerco i dati dell'ordine specificato
	* per capire se ho un pdf da stampare
	*/
	PGRes=PGExecSQL(FALSE,"select ordkey, opflpdf, opflftz, ordprog from ord_prod where ordkey='%s';",szOrdKey);
	nTuples=PQntuples(PGRes);
	strcpy(szFLPDF,PQgetvalue(PGRes,0,1));
	strcpy(szFLFTZ,PQgetvalue(PGRes,0,2));
	strcpy(szOrdine,PQgetvalue(PGRes,0,3));
	PQclear(PGRes);
	
	switch(szFLPDF[0]){
		case ORDINE_CON_FATTURA:
			sprintf(szBuffer,"%s,%s",szOrdine,Cfg.szFatturePrinterName);
			bStampa=TRUE;
		break;
		case ORDINE_CON_DDT:
			sprintf(szBuffer,"%s,%s",szOrdine,Cfg.szBollePrinterName);
			bStampa=TRUE;
		break;
		default:
		break;
	}
	
	if (bStampa){
		if(!SendSockCmd(Cfg.nServicePort,Cfg.szSocksHost,CMD_PRINT_PDF,szBuffer)){
#ifdef TRACE
			trace_out_vstr_date(1,"Errore [%s] in spedizione msg:%s via socket",strerror(errno),szBuffer);
#endif
		} else {
			/*
			* Stampa OK...qualcosa da dire?
			*/
		}
	}

	gtk_widget_destroy(GTK_WIDGET(user_data));
}


void on_dlg_stampa_ordine_pb_cancel_clicked (GtkButton       *button, gpointer         user_data)
{

	gtk_widget_destroy(GTK_WIDGET(user_data));
}


