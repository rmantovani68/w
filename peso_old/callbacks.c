/*
* Callbacks.c
* Moudlo: Bilancia
* Easy Picking 3.0 - Wella Rebuild
* Copyright CNI srl 1999-2000
* Autori : Roberto Mantovani 
*          Stefano Tarroni
* Data Creazione : 16-10-2000 
*/


#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#ifdef TRACE
#include <trace.h>
#endif

#include <gtk/gtk.h>
#include <libpq-fe.h>

#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

#include <proc_list.h>
#include <pmx_msq.h>
#include <ca_file.h>
#include <dbfun.h>

#include "plcsock.h"

#include "picking.h"
#include "pesostruct.h"
#include "pesoext.h"
#include "pesofun.h"


/*********************************************************
************     DLG_EXIT          ***********************
*********************************************************/
void on_dlg_exit_pb_ok_clicked              (GtkButton       *button, gpointer         user_data)
{
	ExitApp();
}


void on_dlg_exit_pb_cancel_clicked          (GtkButton       *button, gpointer         user_data)
{
	gtk_widget_destroy(GTK_WIDGET(user_data));
}


/*********************************************************
************     DLG_MESSAGE       ***********************
*********************************************************/
void on_dlg_message_pb_ok_clicked           (GtkButton       *button, gpointer         user_data)
{
	gtk_widget_destroy(GTK_WIDGET(user_data));
}


void on_dlg_message_pb_cancel_clicked       (GtkButton       *button, gpointer         user_data)
{
	gtk_widget_destroy(GTK_WIDGET(user_data));
}


/*********************************************************
************     DLG_INFO          ***********************
*********************************************************/
void on_dlg_info_pb_ok_clicked              (GtkButton       *button, gpointer         user_data)
{
	gtk_widget_destroy(GTK_WIDGET(user_data));
}


/*********************************************************
************     DLG_CONFIG        ***********************
*********************************************************/

void on_dlg_config_rb_tolleranza_assoluta_toggled (GtkToggleButton *togglebutton, gpointer         user_data)
{
	gtk_widget_set_sensitive( get_widget(dlg_config,"sb_tolleranza_assoluta"), GTK_TOGGLE_BUTTON(get_widget(dlg_config,"rb_tolleranza_assoluta"))->active?TRUE:FALSE);
	gtk_widget_set_sensitive( get_widget(dlg_config,"sb_soglia_massima"),FALSE);
}


void on_dlg_config_rb_tolleranza_percentuale_toggled (GtkToggleButton *togglebutton, gpointer         user_data)
{
	gtk_widget_set_sensitive( get_widget(dlg_config,"sb_tolleranza_percentuale"), GTK_TOGGLE_BUTTON(get_widget(dlg_config,"rb_tolleranza_percentuale"))->active?TRUE:FALSE);
	gtk_widget_set_sensitive( get_widget(dlg_config,"cb_soglia_massima"), GTK_TOGGLE_BUTTON(get_widget(dlg_config,"rb_tolleranza_percentuale"))->active?TRUE:FALSE);
	gtk_widget_set_sensitive( get_widget(dlg_config,"sb_soglia_massima"), GTK_TOGGLE_BUTTON(get_widget(dlg_config,"cb_soglia_massima"))->active?TRUE:FALSE);
	//gtk_widget_set_sensitive( get_widget(dlg_config,"sb_soglia_massima"), Cfg.Tolleranza==TOLLERANZA_PERCENTUALE_SOGLIA_MASSIMA?TRUE:FALSE);
}


void on_dlg_config_rb_pesatura_ordine_toggled (GtkToggleButton *togglebutton, gpointer         user_data)
{
}


void on_dlg_config_rb_pesatura_collo_toggled (GtkToggleButton *togglebutton, gpointer         user_data)
{
}


void on_dlg_config_cb_soglia_massima_toggled (GtkToggleButton *togglebutton, gpointer         user_data)
{
	gtk_widget_set_sensitive(get_widget(dlg_config,"sb_soglia_massima"),GTK_TOGGLE_BUTTON(get_widget(dlg_config,"cb_soglia_massima"))->active?TRUE:FALSE);
}


void on_dlg_config_pb_ok_clicked            (GtkButton       *button, gpointer         user_data)
{
	gtk_widget_destroy(user_data);
}


void on_dlg_config_pb_apply_clicked         (GtkButton       *button, gpointer         user_data)
{
	ApplyConfigurazione();
}


void on_dlg_config_pb_refresh_plc_clicked   (GtkButton       *button, gpointer         user_data)
{
	UpdateVarList(get_widget(dlg_config,"lst_plc_var"),TRUE);
}


void on_dlg_config_pb_cancel_clicked        (GtkButton       *button, gpointer         user_data)
{
	gtk_widget_destroy(GTK_WIDGET(user_data));
}


/*********************************************************
************     DLG_APP_ERROR     ***********************
*********************************************************/
void on_dlg_app_error_pb_ok_clicked         (GtkButton       *button, gpointer         user_data)
{
	/* chiudo la GUI */
	gtk_main_quit();
}


/*********************************************************
************     MAIN_WINDOW       ***********************
*********************************************************/
gboolean on_main_window_destroy_event (GtkWidget *widget, GdkEvent *event, gpointer user_data) 
{
	dlg_exit = create_dlg_exit();
	gtk_window_set_focus (GTK_WINDOW (dlg_exit), get_widget(dlg_exit,"pb_ok"));
	gtk_widget_show(dlg_exit);
	gtk_window_set_transient_for(GTK_WINDOW(dlg_exit),GTK_WINDOW(main_window));
  return TRUE;
}

/*
* Callback di selezione riga in lista colli
*/
void on_lst_colli_select_row(GtkCList *clist, gint row, gint column, GdkEvent *event, gpointer user_data)
{
	GList *lista;
	PGresult *PGRes;
	char *pszColloKey;
	char szColloKey[128];
	char szStato[128];
	int nRowIndex;
	BOOL bOK=TRUE;

	/*
	* se il collo è stato selezionato a video -> riempio i campi della dlg
	*/
	lista=g_list_first(GTK_CLIST(lst_colli)->selection);

	/*
	* visualizzo i dati del collo solo se è stata selezionata una sola riga della lista
	*/
	if((lista==NULL)){
		bOK=FALSE;
	} else {
		nRowIndex=GPOINTER_TO_INT(lista->data);
		gtk_clist_get_text(GTK_CLIST(lst_colli),nRowIndex,Cfg.nColloKeyField,&pszColloKey);
		strcpy(szColloKey,pszColloKey);
		PGRes=PGExecSQL(Cfg.nDebugVersion,"select opstato from ord_prod where ordkey='%s';",szColloKey);
		if (PQntuples(PGRes)>0){
			strcpy(szStato,PQgetvalue(PGRes,0,0));
			if(szStato[0]!=ORDINE_EVASO){
				bOK=FALSE;
			}
		} else {
			bOK=FALSE;
		}
		PQclear(PGRes);
	}
	gtk_widget_set_sensitive(get_widget(main_window,"pb_ordine_controllato"),!bOK);

	nLstColliRowSelected=(row-1);
}

void on_lst_colli_unselect_row(GtkCList *clist, gint row, gint column, GdkEvent *event, gpointer user_data)
{
	gtk_widget_set_sensitive(get_widget(main_window,"pb_ordine_controllato"),FALSE);
}


/*********************************************************
************    TOOLBAR BUTTONS    ***********************
*********************************************************/
void on_main_window_pb_elimina_collo_clicked  (GtkButton *button, gpointer user_data)
{
	GList *lista;
	PGresult *PGRes;
	char *pszColloKey;
	char *pszNumCollo;
	char szColloKey[128];
	int nNumCollo;
	int nRowIndex;

	dlg_elimina_collo = create_dlg_elimina_collo();

	/*
	* se il collo è stato selezionato a video -> riempio i campi della dlg
	*/
	lista=g_list_first(GTK_CLIST(lst_colli)->selection);

	/*
	* visualizzo i dati del collo solo se è stata selezionata una sola riga della lista
	*/
	if((lista==NULL) || g_list_length(lista)>1){
		gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_elimina_collo,"entry_ordine")),"");
		gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_elimina_collo,"entry_segnacollo")),"");
	} else {
		nRowIndex=GPOINTER_TO_INT(lista->data);
		gtk_clist_get_text(GTK_CLIST(lst_colli),nRowIndex,Cfg.nColloKeyField,&pszColloKey);
		gtk_clist_get_text(GTK_CLIST(lst_colli),nRowIndex,Cfg.nColloNumField,&pszNumCollo);
		strcpy(szColloKey,pszColloKey);
		nNumCollo=atoi(pszNumCollo);
		PGRes=PGExecSQL(Cfg.nDebugVersion,"select cpprgcl from col_prod where ordkey='%s' and cpnmcol=%d;",szColloKey,nNumCollo);
		if (PQntuples(PGRes)>0){
			gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_elimina_collo,"entry_ordine")),szColloKey);
			gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_elimina_collo,"entry_segnacollo")),PQgetvalue(PGRes,0,0));
		} else {
			gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_elimina_collo,"entry_ordine")),"");
			gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_elimina_collo,"entry_segnacollo")),"");
		}
		PQclear(PGRes);
	}

	gtk_window_set_focus (GTK_WINDOW (dlg_elimina_collo), get_widget(dlg_exit,"entry_ordine"));
	gtk_widget_show(dlg_elimina_collo);
	gtk_window_set_transient_for(GTK_WINDOW(dlg_elimina_collo),GTK_WINDOW(main_window));
}


void on_main_window_pb_aggiungi_collo_clicked (GtkButton *button, gpointer user_data)
{
	GList *lista=NULL;
	GList *listaColli=NULL;
	GtkWidget *ComboColli;
	PGresult *PGRes;
	int nRowIndex,nIndex;
	char *pszColloKey;
	char szColloKey[128];

	dlg_aggiungi_collo = create_dlg_aggiungi_collo();

	/*
	* se il collo è stato selezionato a video -> riempio il campo ordine della dlg
	* solo se una sola riga della lista è selezionata
	*/
	lista=g_list_first(GTK_CLIST(lst_colli)->selection);

	if((lista==NULL) || g_list_length(lista)>1){
		gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_aggiungi_collo,"entry_ordine")),"");
	} else {
		nRowIndex=GPOINTER_TO_INT(lista->data);
		gtk_clist_get_text(GTK_CLIST(lst_colli),nRowIndex,Cfg.nColloKeyField,&pszColloKey);
		strcpy(szColloKey,pszColloKey);
		gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_aggiungi_collo,"entry_ordine")),szColloKey);
	}

	/* riempio il combo con i formati dei colli disponibili */
	ComboColli=get_widget(dlg_aggiungi_collo,"combo_formato_collo");
	PGRes=PGExecSQL(Cfg.nDebugVersion,"select codice from imballi;");
	for(nIndex=0;nIndex<PQntuples(PGRes);nIndex++){
		listaColli=g_list_append(listaColli,PQgetvalue(PGRes,nIndex,0));
	}
	gtk_combo_set_popdown_strings(GTK_COMBO(ComboColli),listaColli);
	g_list_free(listaColli);
	PQclear(PGRes);

	gtk_window_set_focus (GTK_WINDOW (dlg_aggiungi_collo), get_widget(dlg_exit,"entry_ordine"));
	gtk_widget_show(dlg_aggiungi_collo);
	gtk_window_set_transient_for(GTK_WINDOW(dlg_aggiungi_collo),GTK_WINDOW(main_window));
}


void on_main_window_pb_edit_clicked         (GtkButton       *button, gpointer         user_data)
{
	static char szColloKey[80];
	char *pszText;
	int nCollo;

	dlg_edit_collo=create_dlg_edit_collo();

	if(GTK_CLIST(lst_colli)->selection){
		nColloInEdit=GPOINTER_TO_INT(GTK_CLIST(lst_colli)->selection->data);
	} else {
		nColloInEdit=0;
	}
	gtk_clist_get_text(GTK_CLIST(lst_colli),nColloInEdit,Cfg.nColloKeyField,&pszText);
	strcpy(szColloKey,pszText);
	gtk_clist_get_text(GTK_CLIST(lst_colli),nColloInEdit,Cfg.nColloNumField,&pszText);
	nCollo=atoi(pszText);
	gtk_window_set_focus (GTK_WINDOW (dlg_edit_collo), get_widget(dlg_edit_collo,"pb_next"));
	gtk_widget_show(dlg_edit_collo);
	gtk_window_set_transient_for(GTK_WINDOW(dlg_edit_collo),GTK_WINDOW(main_window));

	EditCollo(szColloKey,nCollo);
}


void on_main_window_pb_stampa_clicked       (GtkButton       *button, gpointer         user_data)
{
	GList *lista;
	int nRowIndex;
	char *pszColloKey;
	char *pszColloNum;
	int nNumCollo;
	char szColloKey[128];

	GtkWidget *dlg_stampa_ordine=create_dlg_stampa_ordine();

	/*
	* se il collo è stato selezionato a video -> riempio il campo ordine della dlg
	*/
	lista=g_list_first(GTK_CLIST(lst_colli)->selection);

	if(lista==NULL){
		gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_stampa_ordine,"entry_ordine")),"");
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(get_widget(dlg_stampa_ordine,"sb_collo")),0);
		gtk_window_set_focus (GTK_WINDOW (dlg_stampa_ordine), get_widget(dlg_stampa_ordine,"entry_ordine"));
	} else {
		nRowIndex=GPOINTER_TO_INT(lista->data);
		gtk_clist_get_text(GTK_CLIST(lst_colli),nRowIndex,Cfg.nColloKeyField,&pszColloKey);
		gtk_clist_get_text(GTK_CLIST(lst_colli),nRowIndex,Cfg.nColloNumField,&pszColloNum);
		strcpy(szColloKey,pszColloKey);
		nNumCollo=atoi(pszColloNum);
		gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_stampa_ordine,"entry_ordine")),szColloKey);
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(get_widget(dlg_stampa_ordine,"sb_collo")), (float)nNumCollo);
		gtk_window_set_focus (GTK_WINDOW (dlg_stampa_ordine), get_widget(dlg_stampa_ordine,"pb_ok"));
	}

	gtk_window_set_title(GTK_WINDOW(dlg_stampa_ordine),"Stampa Etichette");
	gtk_widget_show(dlg_stampa_ordine);
	gtk_window_set_transient_for(GTK_WINDOW(dlg_stampa_ordine),GTK_WINDOW(main_window));
	gtk_signal_connect (GTK_OBJECT (get_widget(dlg_stampa_ordine,"pb_ok")), "clicked", GTK_SIGNAL_FUNC (do_stampa_etichette), dlg_stampa_ordine);
}


void on_main_window_pb_settings_clicked     (GtkButton       *button, gpointer         user_data)
{
	EditConfigurazione();
}


void on_main_window_pb_ordine_ok_clicked    (GtkButton       *button, gpointer         user_data)
{
	GList *lista;
	char *pszColloKey;
	int nRowIndex;

	lista=g_list_first(GTK_CLIST(lst_colli)->selection);
	/*
	* controllo che sia stata selezionata una sola riga della lista
	*/
	if((lista==NULL) || g_list_length(lista)>1){
		/*
		* 0 o + di un riga selezionate
		*/
		dlg_message=create_dlg_message();
		if (lista==NULL){
			gtk_label_printf(get_widget(dlg_message,"lb_msg"),"Nessuna riga della lista è selezionata");
		} else {
			gtk_label_printf(get_widget(dlg_message,"lb_msg"),"Selezionare soltanto una riga della lista");
		}
		gtk_window_set_title (GTK_WINDOW (dlg_message), "Controllo Ordine");
		gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
		gtk_widget_show(dlg_message);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));
		gtk_signal_connect (GTK_OBJECT (get_widget(dlg_message,"pb_ok")), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_pb_ok_clicked), dlg_message);

	} else {
		nRowIndex=GPOINTER_TO_INT(lista->data);
		gtk_clist_get_text(GTK_CLIST(lst_colli),nRowIndex,Cfg.nColloKeyField,&pszColloKey);
		dlg_message=create_dlg_message();
		gtk_label_printf(get_widget(dlg_message,"lb_msg"),"Confermi l'evasione dell'ordine %s",pszColloKey);
		gtk_window_set_title (GTK_WINDOW (dlg_message), "Controllo Ordine");
		gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
		gtk_widget_show(dlg_message);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));
		gtk_signal_connect (GTK_OBJECT (get_widget(dlg_message,"pb_ok")), "clicked", GTK_SIGNAL_FUNC (do_ordine_controllato), pszColloKey);
	}
}


void on_main_window_pb_exit_clicked         (GtkButton       *button, gpointer         user_data)
{
	dlg_exit = create_dlg_exit();
	gtk_window_set_focus (GTK_WINDOW (dlg_exit), get_widget(dlg_exit,"pb_ok"));
	gtk_widget_show(dlg_exit);
	gtk_window_set_transient_for(GTK_WINDOW(dlg_exit),GTK_WINDOW(main_window));
}


/*********************************************************
**************   MENUBAR OPTIONS    **********************
*********************************************************/
void on_pbm_aggiungi_collo_activate         (GtkMenuItem     *menuitem, gpointer         user_data)
{
	GtkWidget *ComboColli;
	GList *listaColli=NULL;
	PGresult *PGRes;
	int nIndex;

	dlg_aggiungi_collo = create_dlg_aggiungi_collo();
	gtk_window_set_focus (GTK_WINDOW (dlg_aggiungi_collo), get_widget(dlg_exit,"entry_ordine"));

	/* riempio il combo con i formati dei colli disponibili */
	ComboColli=get_widget(dlg_aggiungi_collo,"combo_formato_collo");
	PGRes=PGExecSQL(Cfg.nDebugVersion,"select codice from imballi;");
	for(nIndex=0;nIndex<PQntuples(PGRes);nIndex++){
		listaColli=g_list_append(listaColli,PQgetvalue(PGRes,nIndex,0));
	}
	gtk_combo_set_popdown_strings(GTK_COMBO(ComboColli),listaColli);
	g_list_free(listaColli);
	PQclear(PGRes);

	gtk_widget_show(dlg_aggiungi_collo);
	gtk_window_set_transient_for(GTK_WINDOW(dlg_aggiungi_collo),GTK_WINDOW(main_window));
}


void on_pbm_elimina_collo_activate          (GtkMenuItem     *menuitem, gpointer         user_data)
{
	dlg_elimina_collo = create_dlg_elimina_collo();
	gtk_window_set_focus (GTK_WINDOW (dlg_elimina_collo), get_widget(dlg_exit,"entry_ordine"));
	gtk_widget_show(dlg_elimina_collo);
	gtk_window_set_transient_for(GTK_WINDOW(dlg_elimina_collo),GTK_WINDOW(main_window));
}


void on_pbm_ordine_controllato_activate  (GtkMenuItem *menuitem, gpointer user_data)
{
	GList *lista;
	char *pszColloKey;
	int nRowIndex;

	lista=g_list_first(GTK_CLIST(lst_colli)->selection);
	/*
	* controllo che sia stata selezionata una sola riga della lista
	*/
	if((lista==NULL) || g_list_length(lista)>1){
		/*
		* 0 o + di un riga selezionate
		*/
		dlg_message=create_dlg_message();
		if (lista==NULL){
			gtk_label_printf(get_widget(dlg_message,"lb_msg"),"Nessuna riga della lista è selezionata");
		} else {
			gtk_label_printf(get_widget(dlg_message,"lb_msg"),"Selezionare soltanto una riga della lista");
		}
		gtk_window_set_title (GTK_WINDOW (dlg_message), "Controllo Ordine");
		gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
		gtk_widget_show(dlg_message);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));
		gtk_signal_connect (GTK_OBJECT (get_widget(dlg_message,"pb_ok")), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_pb_ok_clicked), dlg_message);

	} else {
		nRowIndex=GPOINTER_TO_INT(lista->data);
		gtk_clist_get_text(GTK_CLIST(lst_colli),nRowIndex,Cfg.nColloKeyField,&pszColloKey);
		dlg_message=create_dlg_message();
		gtk_label_printf(get_widget(dlg_message,"lb_msg"),"Confermi l'evasione dell'ordine %s",pszColloKey);
		gtk_window_set_title (GTK_WINDOW (dlg_message), "Controllo Ordine");
		gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
		gtk_widget_show(dlg_message);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));
		gtk_signal_connect (GTK_OBJECT (get_widget(dlg_message,"pb_ok")), "clicked", GTK_SIGNAL_FUNC (do_ordine_controllato), pszColloKey);
	}
}


void do_ordine_controllato(GtkButton *button, gpointer user_data)
{
	int nRigaListaColli=0;
	int nCollo=0;
	char *pszColloKey;
	char szColloKey[64];
	char *pszNumCollo;
	BOOL bOK=TRUE;
	GList *lista;
	int nRowIndex;
	char szCodiceOrdine[64];
	char szCodice[64];

	/*
	* ricavo il codice del sottordine da evadere
	*/
	lista=g_list_first(GTK_CLIST(lst_colli)->selection);
	nRowIndex=GPOINTER_TO_INT(lista->data);
	gtk_clist_get_text(GTK_CLIST(lst_colli),nRowIndex,Cfg.nColloKeyField,&pszColloKey);

	strcpy(szCodiceOrdine,pszColloKey);
	
#ifdef TRACE
	trace_out_vstr_date(1,"do_ordine_controllato (%s) - INIZIO -",szCodiceOrdine);
#endif

	while(nRigaListaColli<(GTK_CLIST(lst_colli)->rows)){
			
		gtk_clist_get_text(GTK_CLIST(lst_colli),nRigaListaColli,Cfg.nColloKeyField,&pszColloKey);
		gtk_clist_get_text(GTK_CLIST(lst_colli),nRigaListaColli,Cfg.nColloNumField,&pszNumCollo);

		nCollo=atoi(pszNumCollo);
		strcpy(szColloKey,pszColloKey);

		if (!(strcmp(szColloKey,szCodiceOrdine))){
			if(EvadiCollo(szColloKey,nCollo)){
				gtk_clist_set_background(GTK_CLIST(lst_colli),nRigaListaColli,&cGREEN);
				gtk_clist_set_foreground(GTK_CLIST(lst_colli),nRigaListaColli,&cWHITE);
			} else {
				bOK=FALSE;
#ifdef TRACE
				trace_out_vstr_date(1,"Fallita Evasione Collo [%s][%d]",szColloKey,nCollo);
#endif
				gtk_text_printf(&cRED,txt_msgs,"Fallita Evasione Collo [%s][%d]\n",szColloKey,nCollo);
			}
		}
		nRigaListaColli++;
	}

	/*
	* se i colli sono stati evasi correttamente evado l'ordine (sottordine)
	*/
	if(bOK){
		/*
		* st - 31/10/2001 evado l'ordine
		*/
		PGresult *PGRes;
		char szTimeBuffer[128];
		char szDateBuffer[128];
		char szBuffer[128];
		int nColliAutomatici,nIndex,nPesoRealeOrdine;

#ifdef TRACE
		trace_out_vstr_date(1,"do_ordine_controllato - [evasione ordine %s]",szCodiceOrdine);
#endif
		
		GetTimeHHMMSS(time((long *)0),szTimeBuffer);
		GetDateYYYYMMDD(time((long *)0),szDateBuffer);

		/*
		* ricavo il peso reale dell'ordine 
		*/
		PGRes=PGExecSQL(Cfg.nDebugVersion,"select cppsrea from col_prod where ordkey='%s' and cpstato in ('%c','%c') and cpswlin!='1';",szCodiceOrdine,COLLO_EVASO,COLLO_ELIMINATO);

		nColliAutomatici=PQntuples(PGRes);

		nPesoRealeOrdine=0;
		for(nIndex=0;nIndex<nColliAutomatici;nIndex++){
			nPesoRealeOrdine+=atoi(PQgetvalue(PGRes,nIndex,0));
		}
		PQclear(PGRes);

#ifdef TRACE
		trace_out_vstr_date(1,"Evado l'ordine assegnandogli il peso e l'ora di evasione",szCodiceOrdine);
#endif
		/*
		* assegno al sott'ordine il peso e l'ora di evasione
		*/
		if(!SettaPesoSottordine(szCodiceOrdine,nPesoRealeOrdine)){
			gtk_text_printf(&cRED,txt_msgs,"Fallita evasione ordine [%s]\n",szCodiceOrdine);
#ifdef TRACE
			trace_out_vstr_date(1,"Fallita evasione ordine [%s]",szCodiceOrdine);
#endif
		} else {
			/*
			* evado il sottordine
			*/
#ifdef TRACE
			trace_out_vstr_date(1,"Evado il sottordine: CambiaStatoSottordine(%s,%c)",szCodiceOrdine,ORDINE_EVASO);
#endif
			CambiaStatoSottordine(szCodiceOrdine,ORDINE_EVASO);

			/*
			* controllo se l'intero ordine corrispondente è evaso
			*/
			if(Cfg.nSAP){
				strcpy(szCodice,LeftStr(szCodiceOrdine,10));
			} else {
				strcpy(szCodice,LeftStr(szCodiceOrdine,6));
			}
#ifdef TRACE
			trace_out_vstr_date(1,"Lanciata CheckOrdineEvaso(%s)",szCodice);
#endif
			if(CheckOrdineEvaso(szCodice)){
				sprintf(szBuffer,"%s,%c",szCodice,ORDINE_EVASO);
				if(!SendSockCmd(Cfg.nServicePort,Cfg.szSocksHost,CMD_UPDATE_ORDINE,szBuffer)){
#ifdef TRACE
					trace_out_vstr_date(1,"Errore in spedizione msg:%s via socket",szBuffer);
#endif
				}
			}
		}
	}

	gtk_clist_unselect_all(GTK_CLIST(lst_colli));

	if (dlg_message != (GtkWidget*)NULL) gtk_widget_destroy(dlg_message);

#ifdef TRACE
	trace_out_vstr_date(1,"do_ordine_controllato (%s) - FINE -",szCodiceOrdine);
#endif
}


void on_pbm_exit_activate               (GtkMenuItem     *menuitem, gpointer         user_data)
{
	dlg_exit = create_dlg_exit();
	gtk_window_set_focus (GTK_WINDOW (dlg_exit), get_widget(dlg_exit,"pb_ok"));
	gtk_widget_show(dlg_exit);
	gtk_window_set_transient_for(GTK_WINDOW(dlg_exit),GTK_WINDOW(main_window));
}


void on_pbm_seleziona_tutto_activate        (GtkMenuItem     *menuitem, gpointer         user_data)
{
	gtk_clist_select_all(GTK_CLIST(lst_colli));
}

void on_pbm_deseleziona_tutto_activate      (GtkMenuItem     *menuitem, gpointer         user_data)
{
	gtk_clist_unselect_all(GTK_CLIST(lst_colli));
}

void on_pbm_configurazione_activate     (GtkMenuItem     *menuitem, gpointer         user_data)
{
	EditConfigurazione();
}


void on_pbm_about_activate               (GtkMenuItem     *menuitem, gpointer         user_data)
{
	dlg_info = create_dlg_info();
	gtk_window_set_focus (GTK_WINDOW (dlg_info), get_widget(dlg_info,"pb_ok"));
	gtk_widget_show(dlg_info);
	gtk_window_set_transient_for(GTK_WINDOW(dlg_info),GTK_WINDOW(main_window));
}


void on_pbm_debug_variabili_plc_activate (GtkMenuItem     *menuitem, gpointer         user_data)
{
	gtk_window_set_focus (GTK_WINDOW (dlg_debug_var), get_widget(dlg_debug_var,"pb_cancel"));
	gtk_window_set_transient_for(GTK_WINDOW(dlg_debug_var),GTK_WINDOW(main_window));
	gtk_widget_show(dlg_debug_var);
}


void on_pbm_stampa_etichette_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *dlg_stampa_ordine=create_dlg_stampa_ordine();

	gtk_window_set_title(GTK_WINDOW(dlg_stampa_ordine),"Stampa Etichette");
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_stampa_ordine,"entry_ordine")),"");
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(get_widget(dlg_stampa_ordine,"sb_collo")),0);

	gtk_widget_show(dlg_stampa_ordine);
	gtk_window_set_focus (GTK_WINDOW (dlg_stampa_ordine), get_widget(dlg_stampa_ordine,"entry_ordine"));
	gtk_window_set_transient_for(GTK_WINDOW(dlg_stampa_ordine),GTK_WINDOW(main_window));
	gtk_signal_connect (GTK_OBJECT (get_widget(dlg_stampa_ordine,"pb_ok")), "clicked", GTK_SIGNAL_FUNC (do_stampa_etichette), dlg_stampa_ordine);

}


void on_pbm_stampa_ordine_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *dlg_stampa_ordine=create_dlg_stampa_ordine();

	gtk_window_set_title(GTK_WINDOW(dlg_stampa_ordine),"Stampa Ordine");
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_stampa_ordine,"entry_ordine")),"");
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(get_widget(dlg_stampa_ordine,"sb_collo")),0);

	gtk_widget_show(dlg_stampa_ordine);
	gtk_window_set_focus (GTK_WINDOW (dlg_stampa_ordine), get_widget(dlg_stampa_ordine,"entry_ordine"));
	gtk_window_set_transient_for(GTK_WINDOW(dlg_stampa_ordine),GTK_WINDOW(main_window));
	gtk_signal_connect (GTK_OBJECT (get_widget(dlg_stampa_ordine,"pb_ok")), "clicked", GTK_SIGNAL_FUNC (do_stampa_packing_list), dlg_stampa_ordine);
}


/*********************************************************
************   DLG_EDIT_ORDINE     ***********************
*********************************************************/
void on_dlg_edit_ordine_pb_prev_clicked     (GtkButton       *button, gpointer         user_data)
{

}


void on_dlg_edit_ordine_pb_next_clicked     (GtkButton       *button, gpointer         user_data)
{

}


void on_dlg_edit_ordine_pb_ok_clicked       (GtkButton       *button, gpointer         user_data)
{
	gtk_widget_destroy(GTK_WIDGET(user_data));
}


void on_dlg_edit_ordine_pb_cancel_clicked   (GtkButton       *button, gpointer         user_data)
{
	gtk_widget_destroy(GTK_WIDGET(user_data));
	gtk_widget_destroy(GTK_WIDGET(dlg_edit_collo));
}


/*********************************************************
************   DLG_EDIT_COLLO      ***********************
*********************************************************/
void on_dlg_edit_collo_pb_prev_clicked      (GtkButton       *button, gpointer         user_data)
{
	int nCollo;
	char *pszText;
	char szColloKey[128];

	gtk_clist_get_text(GTK_CLIST(lst_colli),nColloInEdit-1,Cfg.nColloKeyField,&pszText);
	strcpy(szColloKey,pszText);
	gtk_clist_get_text(GTK_CLIST(lst_colli),nColloInEdit-1,Cfg.nColloNumField,&pszText);
	nCollo=atoi(pszText);

	if(EditCollo(szColloKey,nCollo)){
		nColloInEdit--;
		gtk_widget_set_sensitive(get_widget(dlg_edit_collo,"pb_next"),TRUE);
		gtk_widget_set_sensitive(get_widget(dlg_edit_collo,"pb_prev"),TRUE);
	} else {
		gtk_widget_set_sensitive(get_widget(dlg_edit_collo,"pb_prev"),FALSE);
		gtk_window_set_focus (GTK_WINDOW (dlg_edit_collo), get_widget(dlg_edit_collo,"pb_next"));
	}
}


void on_dlg_edit_collo_pb_next_clicked      (GtkButton       *button, gpointer         user_data)
{
	int nCollo;
	char *pszText;
	char szColloKey[128];

	gtk_clist_get_text(GTK_CLIST(lst_colli),nColloInEdit+1,Cfg.nColloKeyField,&pszText);
	strcpy(szColloKey,pszText);
	gtk_clist_get_text(GTK_CLIST(lst_colli),nColloInEdit+1,Cfg.nColloNumField,&pszText);
	nCollo=atoi(pszText);

	if(EditCollo(szColloKey,nCollo)){
		nColloInEdit++;
		gtk_widget_set_sensitive(get_widget(dlg_edit_collo,"pb_next"),TRUE);
		gtk_widget_set_sensitive(get_widget(dlg_edit_collo,"pb_prev"),TRUE);
	} else {
		gtk_widget_set_sensitive(get_widget(dlg_edit_collo,"pb_next"),FALSE);
		gtk_window_set_focus (GTK_WINDOW (dlg_edit_collo), get_widget(dlg_edit_collo,"pb_prev"));
	}
}


void on_dlg_edit_collo_pb_edit_ordine_clicked (GtkButton       *button, gpointer         user_data)
{
	char *pszText;
	char szColloOrdprog[128];

	gtk_clist_get_text(GTK_CLIST(lst_colli),nColloInEdit,Cfg.nColloKeyField,&pszText);
	strcpy(szColloOrdprog,pszText);

	dlg_edit_ordine = create_dlg_edit_ordine();

	gtk_window_set_focus (GTK_WINDOW (dlg_edit_ordine), get_widget(dlg_edit_ordine,"pb_ok"));
	gtk_widget_set_sensitive(get_widget(dlg_edit_ordine,"pb_next"),FALSE);
	gtk_widget_set_sensitive(get_widget(dlg_edit_ordine,"pb_prev"),FALSE);
	gtk_widget_show(dlg_edit_ordine);
	gtk_window_set_transient_for(GTK_WINDOW(dlg_edit_ordine),GTK_WINDOW(dlg_edit_collo));
	gtk_window_set_transient_for(GTK_WINDOW(dlg_edit_ordine),GTK_WINDOW(main_window));

	EditOrdine(szColloOrdprog);
}

void on_dlg_edit_collo_pb_ok_clicked      (GtkButton       *button, gpointer         user_data)
{
	gtk_widget_destroy(GTK_WIDGET(user_data));
}


void on_dlg_edit_collo_pb_cancel_clicked      (GtkButton       *button, gpointer         user_data)
{
	gtk_widget_destroy(GTK_WIDGET(user_data));
}


/*********************************************************
************     DLG_BARCODE       ***********************
*********************************************************/
void on_dlg_barcode_pb_ok_clicked           (GtkButton       *button, gpointer         user_data)
{
	char szBuffer[80];

	/*
	* acquisisco i valori di barcode e peso dalle entry della dlg_barcode
	*/
	strcpy(szBarcodeColloBilancia,gtk_entry_get_text(GTK_ENTRY (get_widget(dlg_barcode,"txf_barcode"))));
	strcpy(szBuffer,gtk_entry_get_text(GTK_ENTRY (get_widget(dlg_barcode,"txf_peso"))));
	nPesoColloBilancia=atoi(szBuffer);
	
	gtk_widget_hide(dlg_barcode);

	ColloInBilancia(szBarcodeColloBilancia,nPesoColloBilancia);
}


void on_dlg_barcode_pb_cancel_clicked       (GtkButton       *button, gpointer         user_data)
{
	/*
	*	COLLOQUIO CON PLC
	* faccio avanzare comunque il collo
	*/
	if(Cfg.nPLC){
		SendCmd(fdPLC,CMD_SET,"CB 0 0 1",TRUE);	/* Forzatura variabile - Fine lettura collo */
	}

	gtk_widget_hide(dlg_barcode);
}


/*********************************************************
************      DLG_TRACE        ***********************
*********************************************************/
void on_dlg_trace_pb_cancel_clicked         (GtkButton       *button, gpointer         user_data)
{

}


/*********************************************************
************     DLG_ADD_VAR       ***********************
*********************************************************/
void on_dlg_add_var_pb_add_clicked          (GtkButton       *button, gpointer         user_data)
{

}


void on_dlg_add_var_pb_cancel_clicked       (GtkButton       *button, gpointer         user_data)
{

}


void on_dlg_add_var_pb_force_clicked        (GtkButton       *button, gpointer         user_data)
{

}


/*********************************************************
************      DLG_FORCE        ***********************
*********************************************************/
void on_dlg_force_var_pb_cancel_clicked     (GtkButton       *button, gpointer         user_data)
{

}


/*********************************************************
************     DLG_DEBUG_VAR     ***********************
*********************************************************/
void on_dlg_debug_var_pb_force_clicked      (GtkButton       *button, gpointer         user_data)
{

}


void on_dlg_debug_var_pb_cancel_clicked     (GtkButton       *button, gpointer         user_data)
{
	gtk_widget_hide(dlg_debug_var);
}


/*********************************************************
************     DLG_TEST_HOST     ***********************
*********************************************************/
void on_dlg_test_host_pb_invia_messaggio_clicked (GtkButton       *button, gpointer         user_data)
{

}


void on_dlg_test_host_pb_richiesta_peso_clicked (GtkButton       *button, gpointer         user_data)
{

}


void on_dlg_test_host_pb_cancel_clicked     (GtkButton       *button, gpointer         user_data)
{

}


/*********************************************************
************     DLG_PLC_ERROR     ***********************
*********************************************************/
void on_dlg_plc_error_pb_ok_clicked(GtkButton *button, gpointer user_data)
{
	int nPLCError;

	gtk_widget_destroy(dlg_plc_error);
	if((nPLCError=PLCConnect(&fdPLC,Cfg.szPLCHost))==FALSE){
		gtk_label_printf( lb_status_msg, "Errore in connessione PLC");
		Cfg.nPLC=0;
		dlg_plc_error = create_dlg_plc_error();
		gtk_window_set_focus (GTK_WINDOW (dlg_plc_error), get_widget(dlg_plc_error,"pb_ok"));
		gtk_widget_show (dlg_plc_error);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_plc_error),GTK_WINDOW(main_window));
	}
}

void on_dlg_plc_error_pb_cancel_clicked(GtkButton *button, gpointer user_data)
{
	gtk_widget_destroy(dlg_plc_error);
	Cfg.nPLC=0;
}


/*********************************************************
************   DLG_STAMPA_ORDINE    **********************
*********************************************************/
void do_stampa_packing_list     (GtkButton       *button, gpointer         user_data)
{
	char szOrdine[128];
	char szBuffer[128];
	int nCollo;

	strcpy(szOrdine,gtk_entry_get_text(GTK_ENTRY(get_widget(GTK_WIDGET(user_data),"entry_ordine"))));
	nCollo = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(get_widget(user_data,"sb_collo")));

	if(nCollo){
		sprintf(szBuffer,"%s,%d,%s", szOrdine, nCollo, Cfg.szPrinterName);
		SendMessage(PROC_PRINTLABEL,PROC_PESO,PRINT_PACKING_LIST_COLLO,szBuffer);
	} else {
		sprintf(szBuffer,"%s,%s", szOrdine, Cfg.szPrinterName);
		SendMessage(PROC_PRINTLABEL,PROC_PESO,PRINT_PACKING_LIST,szBuffer);
	}

	gtk_widget_destroy(GTK_WIDGET(user_data));
}

void do_stampa_etichette     (GtkButton       *button, gpointer         user_data)
{
	char szOrdine[128];
	char szBuffer[128];
	int nCollo;

	strcpy(szOrdine,gtk_entry_get_text(GTK_ENTRY(get_widget(GTK_WIDGET(user_data),"entry_ordine"))));
	nCollo = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(get_widget(user_data,"sb_collo")));

	if(nCollo){
		sprintf(szBuffer,"%s,%d,%s", szOrdine, nCollo, Cfg.szPrinterName);
		SendMessage(PROC_PRINTLABEL,PROC_PESO,PRINT_RAC_COLLO,szBuffer);
	} else {
		sprintf(szBuffer,"%s,%s", szOrdine, Cfg.szPrinterName);
		SendMessage(PROC_PRINTLABEL,PROC_PESO,PRINT_RAC,szBuffer);
	}

	gtk_widget_destroy(GTK_WIDGET(user_data));
}


void on_dlg_stampa_ordine_pb_cancel_clicked (GtkButton       *button, gpointer         user_data)
{
	gtk_widget_destroy(GTK_WIDGET(user_data));
}


/*********************************************************
************    ALTRE CALLBACKS     **********************
*********************************************************/
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


/*********************************************************
************   DLG_AGGIUNGI_COLLO   **********************
*********************************************************/
void on_dlg_aggiungi_collo_pb_aggiungi_clicked (GtkButton       *button, gpointer         user_data)
{
	char szOrdine[128];
	char szFormatoCollo[64];
	int nNumeroColli;

	strcpy(szOrdine,gtk_entry_get_text(GTK_ENTRY (get_widget(dlg_aggiungi_collo,"entry_ordine"))));
	nNumeroColli = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(get_widget(dlg_aggiungi_collo,"sb_num_colli")));
	strcpy(szFormatoCollo,gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(get_widget(dlg_aggiungi_collo,"combo_formato_collo"))->entry)));

	do_aggiungi_collo(szOrdine, nNumeroColli, szFormatoCollo);
}


void on_dlg_aggiungi_collo_pb_cancel_clicked (GtkButton       *button, gpointer         user_data)
{
	gtk_widget_destroy(GTK_WIDGET(user_data));
}


/*********************************************************
************   DLG_ELIMINA_COLLO   ***********************
*********************************************************/
void on_dlg_elimina_collo_pb_elimina_clicked (GtkButton       *button, gpointer         user_data)
{
	char szOrdine[128];
	char szSegnacollo[128];


	strcpy(szOrdine,    gtk_entry_get_text(GTK_ENTRY (get_widget(dlg_elimina_collo,"entry_ordine"))));
	strcpy(szSegnacollo,gtk_entry_get_text(GTK_ENTRY (get_widget(dlg_elimina_collo,"entry_segnacollo"))));

	/* controllo che l'ordine corrispondente non sia già stato evaso o spedito a SAP */

	if(Cfg.nSAP){
		PGresult *PGres;
		GtkWidget *lb_msg;

		PGres=PGExecSQL(Cfg.nDebugVersion,"select ordkey from ord_prod where ordkey='%s' and opstato not in ('%c','%c');",szOrdine,ORDINE_EVASO,ORDINE_SPEDITO_HOST);
		if (PQresultStatus(PGres)==PGRES_TUPLES_OK && (PQntuples(PGres)>0)){
			do_elimina_collo(szOrdine, szSegnacollo);
		} else {
			dlg_message=create_dlg_message();
			lb_msg=get_widget(dlg_message,"lb_msg");
			gtk_label_printf(lb_msg,"IMPOSSIBILE EFFETTUARE L'ELIMINAZIONE;\nL'ordine %s è già stato evaso",szOrdine);
			gtk_window_set_title (GTK_WINDOW (dlg_message), "Eliminazione colli in fine linea");
			gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(dlg_elimina_collo));
			gtk_signal_connect (GTK_OBJECT (get_widget(dlg_message,"pb_ok")), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_pb_ok_clicked), dlg_message);
			gtk_widget_show(dlg_message);
		}
		PQclear(PGres);

	} else {
		do_elimina_collo(szOrdine, szSegnacollo);
	}
}


void on_dlg_elimina_collo_pb_canel_clicked  (GtkButton       *button, gpointer         user_data)
{
	gtk_widget_destroy(GTK_WIDGET(user_data));
}



/*********************************************************
************       DLG_SCARTO      ***********************
*********************************************************/
void on_dlg_scarto_pb_ok_clicked(GtkButton *button, gpointer user_data)
{
	/*
	*	COLLOQUIO CON PLC
	* faccio avanzare comunque il collo
	*/
	if(Cfg.nPLC){
		SendCmd(fdPLC,CMD_SET,"CB 0 0 1",TRUE);	/* Forzatura variabile - Fine lettura collo */
	}

	gtk_widget_hide(user_data);
}


