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

char szSelectOrdiniCmd[256];
char szLancioOrdiniCmd[256];
char szSelectOrdiniCondition[256];
char szFiltroSelezione[256];


/*
* Callback di selezione riga in lista ordini
*/
void on_lst_ordini_select_row(GtkCList *clist, gint row, gint column, GdkEvent *event, gpointer user_data)
{
	nLstOrdiniRowSelected=row;
}


/*
* Callback di selezione riga in lista imballi
*/
void on_lista_imballi_row_selected(GtkCList *clist, gint row, gint column, GdkEvent *event, gpointer user_data)
{
	char *pszText;
	char szText[128];

	gtk_clist_get_text(GTK_CLIST(get_widget(dlg_config,"lst_imballi")),row,0,&pszText);
	sprintf(szText,"%s",pszText);
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_config,"entry_codice_imb")),szText);

	gtk_clist_get_text(GTK_CLIST(get_widget(dlg_config,"lst_imballi")),row,1,&pszText);
	sprintf(szText,"%s",pszText);
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_config,"entry_descrizione_imb")),szText);

	gtk_clist_get_text(GTK_CLIST(get_widget(dlg_config,"lst_imballi")),row,2,&pszText);
	sprintf(szText,"%d",atoi(pszText));
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_config,"entry_lunghezza_imb")),szText);

	gtk_clist_get_text(GTK_CLIST(get_widget(dlg_config,"lst_imballi")),row,3,&pszText);
	sprintf(szText,"%d",atoi(pszText));
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_config,"entry_altezza_imb")),szText);

	gtk_clist_get_text(GTK_CLIST(get_widget(dlg_config,"lst_imballi")),row,4,&pszText);
	sprintf(szText,"%d",atoi(pszText));
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_config,"entry_larghezza_imb")),szText);

	gtk_clist_get_text(GTK_CLIST(get_widget(dlg_config,"lst_imballi")),row,5,&pszText);
	sprintf(szText,"%d",atoi(pszText));
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_config,"entry_tara_imb")),szText);

	gtk_clist_get_text(GTK_CLIST(get_widget(dlg_config,"lst_imballi")),row,6,&pszText);
	sprintf(szText,"%d",atoi(pszText));
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_config,"entry_pesomax_imb")),szText);

	gtk_clist_get_text(GTK_CLIST(get_widget(dlg_config,"lst_imballi")),row,7,&pszText);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(get_widget(dlg_config,"sb_volperc_imb_1")),(gdouble)atoi(pszText));

	gtk_clist_get_text(GTK_CLIST(get_widget(dlg_config,"lst_imballi")),row,8,&pszText);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(get_widget(dlg_config,"sb_volperc_imb_2")),(gdouble)atoi(pszText));

	gtk_clist_get_text(GTK_CLIST(get_widget(dlg_config,"lst_imballi")),row,9,&pszText);
	sprintf(szText,"%d",atoi(pszText));
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_config,"entry_tp_lavor_imb")),szText);
}


/*
* Callback di selezione riga in lista anagrafica operatori
*/
void on_lista_opeana_row_selected(GtkCList *clist, gint row, gint column, GdkEvent *event, gpointer user_data)
{
	char *pszText;
	char szText[128];

	gtk_clist_get_text(GTK_CLIST(get_widget(dlg_operatori,"lst_opeana")),row,0,&pszText);
	sprintf(szText,"%s",pszText);
	StrTrimAll(szText);
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_operatori,"entry_op_codice")),szText);

	gtk_clist_get_text(GTK_CLIST(get_widget(dlg_operatori,"lst_opeana")),row,1,&pszText);
	sprintf(szText,"%s",pszText);
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_operatori,"entry_op_nome")),szText);

	gtk_clist_get_text(GTK_CLIST(get_widget(dlg_operatori,"lst_opeana")),row,2,&pszText);
	sprintf(szText,"%s",pszText);
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_operatori,"entry_op_cognome")),szText);

	gtk_clist_get_text(GTK_CLIST(get_widget(dlg_operatori,"lst_opeana")),row,3,&pszText);
	sprintf(szText,"%s",pszText);
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_operatori,"entry_op_indirizzo")),szText);

	gtk_clist_get_text(GTK_CLIST(get_widget(dlg_operatori,"lst_opeana")),row,4,&pszText);
	sprintf(szText,"%s",pszText);
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_operatori,"entry_op_localita")),szText);

	gtk_clist_get_text(GTK_CLIST(get_widget(dlg_operatori,"lst_opeana")),row,5,&pszText);
	sprintf(szText,"%s",pszText);
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_operatori,"entry_op_provincia")),szText);

	gtk_clist_get_text(GTK_CLIST(get_widget(dlg_operatori,"lst_opeana")),row,6,&pszText);
	sprintf(szText,"%s",pszText);
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_operatori,"entry_op_cap")),szText);

	gtk_clist_get_text(GTK_CLIST(get_widget(dlg_operatori,"lst_opeana")),row,7,&pszText);
	sprintf(szText,"%s",pszText);
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_operatori,"entry_op_telefono")),szText);
}

/*
* Callback di selezione riga in lista anagrafica prodotti
*/
void on_lst_prodotti_row_selected(GtkCList *clist, gint row, gint column, GdkEvent *event, gpointer user_data)
{
	char *pszText;
	char szText[128];
	GtkWidget *dlg_prodotti=GTK_WIDGET(user_data);

	gtk_clist_get_text(GTK_CLIST(get_widget(dlg_prodotti,"lst_prodotti")),row,0,&pszText);
	strcpy(szText,pszText); StrTrimAll(szText); gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_prodotti,"entry_pr_codice")),szText);

	gtk_clist_get_text(GTK_CLIST(get_widget(dlg_prodotti,"lst_prodotti")),row,1,&pszText);
	strcpy(szText,pszText); StrTrimAll(szText); gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_prodotti,"entry_pr_descrizione")),szText);

	gtk_clist_get_text(GTK_CLIST(get_widget(dlg_prodotti,"lst_prodotti")),row,2,&pszText);
	strcpy(szText,pszText); StrTrimAll(szText); gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_prodotti,"entry_pr_lunghezza")),szText);

	gtk_clist_get_text(GTK_CLIST(get_widget(dlg_prodotti,"lst_prodotti")),row,3,&pszText);
	strcpy(szText,pszText); StrTrimAll(szText); gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_prodotti,"entry_pr_larghezza")),szText);

	gtk_clist_get_text(GTK_CLIST(get_widget(dlg_prodotti,"lst_prodotti")),row,4,&pszText);
	strcpy(szText,pszText); StrTrimAll(szText); gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_prodotti,"entry_pr_altezza")),szText);

	gtk_clist_get_text(GTK_CLIST(get_widget(dlg_prodotti,"lst_prodotti")),row,5,&pszText);
	strcpy(szText,pszText); StrTrimAll(szText); gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_prodotti,"entry_pr_peso")),szText);

	gtk_clist_get_text(GTK_CLIST(get_widget(dlg_prodotti,"lst_prodotti")),row,6,&pszText);
	strcpy(szText,pszText); StrTrimAll(szText); gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_prodotti,"entry_pr_qtkar")),szText);

	gtk_clist_get_text(GTK_CLIST(get_widget(dlg_prodotti,"lst_prodotti")),row,7,&pszText);
	strcpy(szText,pszText); StrTrimAll(szText); 
  if (atoi(szText)==1){
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (get_widget(dlg_prodotti,"cb_prodotto_in_piedi")), TRUE);
	} else {
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (get_widget(dlg_prodotti,"cb_prodotto_in_piedi")), FALSE);
	}

	gtk_clist_get_text(GTK_CLIST(get_widget(dlg_prodotti,"lst_prodotti")),row,8,&pszText);
	strcpy(szText,pszText); StrTrimAll(szText); 
  if (szText[0]==CONTROLLO_BARCODE_ON){
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (get_widget(dlg_prodotti,"cb_controllo_barcode_id")), TRUE);
	} else {
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (get_widget(dlg_prodotti,"cb_controllo_barcode_id")), FALSE);
	}

	gtk_clist_get_text(GTK_CLIST(get_widget(dlg_prodotti,"lst_prodotti")),row,9,&pszText);
	strcpy(szText,pszText); StrTrimAll(szText); 
  if (szText[0]==IMBALLO_SPECIALE_ON){
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (get_widget(dlg_prodotti,"cb_imballo_speciale")), TRUE);
	} else {
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (get_widget(dlg_prodotti,"cb_imballo_speciale")), FALSE);
	}
}

/*
* Callback di selezione riga in lista prodotti_pericolosi
*/
void on_lst_prodotti_pericolosi_row_selected(GtkCList *clist, gint row, gint column, GdkEvent *event, gpointer user_data)
{
	char *pszText;
	char szText[128];
	GtkWidget *dlg_prodotti=GTK_WIDGET(user_data);

	gtk_clist_get_text(GTK_CLIST(get_widget(dlg_prodotti,"lst_prodotti_pericolosi")),row,0,&pszText);
	strcpy(szText,pszText); StrTrimAll(szText); gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_prodotti,"entry_ppcdpro")),szText);

	gtk_clist_get_text(GTK_CLIST(get_widget(dlg_prodotti,"lst_prodotti_pericolosi")),row,1,&pszText);
	strcpy(szText,pszText); StrTrimAll(szText); gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_prodotti,"entry_ppcdper")),szText);
}


/*
* Callback di selezione riga in lista operatori->settori di prelievo
*/
void on_lista_opeset_row_selected(GtkCList *clist, gint row, gint column, GdkEvent *event, gpointer user_data)
{
	char *pszText;
	char szText[128];

	gtk_clist_get_text(GTK_CLIST(get_widget(dlg_operatori,"lst_opeset")),row,0,&pszText);
	sprintf(szText,"%d",atoi(pszText));
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_operatori,"entry_isola")),szText);

	gtk_clist_get_text(GTK_CLIST(get_widget(dlg_operatori,"lst_opeset")),row,1,&pszText);
	sprintf(szText,"%d",atoi(pszText));
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_operatori,"entry_settore")),szText);

	gtk_clist_get_text(GTK_CLIST(get_widget(dlg_operatori,"lst_opeset")),row,2,&pszText);
	//sprintf(szText,"%0d",atoi(pszText));
	strcpy(szText,pszText);
	if (atoi(szText)==0){
		szText[0]='\0';
	}
	gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(get_widget(dlg_operatori,"combo_operatore"))->entry),szText);
	//gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_operatori,"entry_operatore")),szText);
}

/*
* ------------------------------------------------------------
*                   INIZIO MENU CALLBACKS
* ------------------------------------------------------------
*/
void on_pbm_pulizia_dati_ricezione_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *lb_msg;
	GtkWidget *pb_ok;

	dlg_message=create_dlg_message();

	lb_msg=get_widget(dlg_message,"lb_msg");
	pb_ok=get_widget(dlg_message,"pb_ok");

	gtk_label_printf(lb_msg,"CONFERMI LA CANCELLAZIONE\nDEGLI ARCHIVI DI RICEZIONE ?");
  gtk_window_set_title (GTK_WINDOW (dlg_message), "Cancellazione Archivi di Ricezione");
	gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
	gtk_widget_show(dlg_message);
	gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));
	/*
	* callback per pressione pulsante
	*/
  gtk_signal_connect (GTK_OBJECT (pb_ok), "clicked", GTK_SIGNAL_FUNC (on_delete_ricezione_pb_ok_clicked), dlg_message);
}


void on_pbm_pulizia_dati_produzione_activate(GtkMenuItem * menuitem, gpointer user_data) 
{
	GtkWidget *lb_msg;
	GtkWidget *pb_ok;

	dlg_message=create_dlg_message();

	
	lb_msg=get_widget(dlg_message,"lb_msg");
	pb_ok=get_widget(dlg_message,"pb_ok");

	gtk_label_printf(lb_msg,"CONFERMI LA CANCELLAZIONE\nDEGLI ARCHIVI DI PRODUZIONE ?");
  gtk_window_set_title (GTK_WINDOW (dlg_message), "Cancellazione Archivi di Produzione");
	gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
	gtk_widget_show(dlg_message);
	gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));
	/*
	* callback per pressione pulsante
	*/
  gtk_signal_connect (GTK_OBJECT (pb_ok), "clicked", GTK_SIGNAL_FUNC (on_delete_produzione_pb_ok_clicked), dlg_message);
}


void on_pbm_aggiorna_archivi_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *lb_msg;
	GtkWidget *pb_ok;

	dlg_message=create_dlg_message();

	lb_msg=get_widget(dlg_message,"lb_msg");
	pb_ok=get_widget(dlg_message,"pb_ok");

	gtk_label_printf(lb_msg,"L'operazione di aggiornamento riorganizza gli indici del DataBase.\nE' consigliabile effettuarla a impianto fermo in quanto comporta un rallentamento del sistema\n\nConfermi aggiornamento ?");
  gtk_window_set_title (GTK_WINDOW (dlg_message), "aggiornamento archivi");
	gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
	gtk_widget_show(dlg_message);
	gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));

	/*
	* callback per pressione pulsante
	*/
  gtk_signal_connect (GTK_OBJECT (pb_ok), "clicked", GTK_SIGNAL_FUNC (on_dlg_aggiorna_archivi_pb_ok_clicked), dlg_message);
}


void on_pbm_spedizione_dati_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	//GtkWidget *dlg_message;
	GtkWidget *pb_ok;
	GtkWidget *pr_ordini;
	GtkWidget *pr_colli;
	GtkWidget *rl_ordini;
	GtkWidget *rl_colli;
	GtkWidget *lb_ordini;
	GtkWidget *lb_colli;
	GtkWidget *lb_1;
	GtkWidget *lb_2;


	char szOrdiniFile[128];
	char szColliFile[128];

	int nValuesOrdini;
	int nValuesColli;

	PGresult *PGResOrdini;
	char szSQLCmdOrdini[1024];
	/* rm 03-12-2003 : modifica per test di evasione rif. Leali */
	BOOL bCambiaStato=FALSE;
	
	sprintf(szOrdiniFile,"%s/%s",Cfg.szPathExport,StrTrimAll(Cfg.szExpOrdiniFile));
	sprintf(szColliFile,"%s/%s",Cfg.szPathExport,StrTrimAll(Cfg.szExpColliFile));


	dlg_export=create_dlg_export();

	lb_1=get_widget(dlg_export,"lb_1");
	lb_2=get_widget(dlg_export,"lb_2");

	pr_ordini  = get_widget(dlg_export,"pr_ordini");
	pr_colli   = get_widget(dlg_export,"pr_colli");

	rl_ordini  = get_widget(dlg_export,"rl_ordini");
	rl_colli   = get_widget(dlg_export,"rl_colli");

	lb_ordini  = get_widget(dlg_export,"lb_ordini");
	lb_colli   = get_widget(dlg_export,"lb_colli");

	nValuesOrdini=0;
	nValuesColli=0;

	/*
	* seleziono il numero di ORDINI e COLLI EVASI
	*/
	if(bCambiaStato){
		sprintf(szSQLCmdOrdini,"select count(ordprog), sum(ronmcll) from ric_ord where rostato='%c';",ORDINE_EVASO);	
	} else {
		sprintf(szSQLCmdOrdini,"select count(ordprog), sum(ronmcll) from ric_ord where rostato in ('%c','%c','%c','%c');",
			ORDINE_ELABORATO,
			ORDINE_STAMPATA_RAC,
			ORDINE_IN_PRELIEVO,
			ORDINE_PRELEVATO);
	}

	PGResOrdini=PGExecSQL(FALSE,szSQLCmdOrdini);
	
	if (PQntuples(PGResOrdini) && (nValuesOrdini=atoi(PQgetvalue(PGResOrdini,0,0)))!=0){
			gtk_ruler_set_range(GTK_RULER(rl_ordini),(gfloat)0,(gfloat)nValuesOrdini,(gfloat)0,(gfloat)nValuesOrdini);
			gtk_progress_configure(GTK_PROGRESS(pr_ordini),(gfloat)0,(gfloat)0,(gfloat)nValuesOrdini);
			gtk_label_printf(lb_ordini,"%d Ordini", atoi(PQgetvalue(PGResOrdini,0,0)));
	} else {
			gtk_label_printf(lb_ordini,"Non sono presenti Ordini Evasi");
	}

	if (PQntuples(PGResOrdini) && (nValuesColli=atoi(PQgetvalue(PGResOrdini,0,1)))!=0){
			gtk_ruler_set_range(GTK_RULER(rl_colli),(gfloat)0,(gfloat)nValuesColli,(gfloat)0,(gfloat)nValuesColli);
			gtk_progress_configure(GTK_PROGRESS(pr_colli),(gfloat)0,(gfloat)0,(gfloat)nValuesColli);
			gtk_label_printf(lb_colli,"%d Colli", atoi(PQgetvalue(PGResOrdini,0,1)));
	} else {
			gtk_label_printf(lb_colli,"Non sono presenti Colli Evasi");
	}

	PQclear(PGResOrdini);

	gtk_widget_show(dlg_export);
	gtk_window_set_transient_for(GTK_WINDOW(dlg_export),GTK_WINDOW(main_window));

	if ((nValuesColli==0) && (nValuesOrdini==0)){
		/*
		* Non sono presenti dati da inviare a host
		*/

		gtk_label_printf(lb_1,"Non sono presenti dati da trasmettere\nPremere cancel per annullare l'operazione");
		gtk_widget_set_sensitive(get_widget(dlg_export,"pb_ok"),FALSE);
	} else {

		if (FileExists(szOrdiniFile) || FileExists(szColliFile)){
			/*
			* file di export ancora presenti
			*/
			dlg_export_message=create_dlg_export_message();	
			gtk_widget_show(dlg_export_message);
			gtk_window_set_focus (GTK_WINDOW (dlg_export_message), get_widget(dlg_export_message,"pb_aggiungi"));
			gtk_window_set_transient_for(GTK_WINDOW(dlg_export_message),GTK_WINDOW(main_window));
			gtk_window_set_transient_for(GTK_WINDOW(dlg_export_message),GTK_WINDOW(dlg_export));
			gtk_signal_connect (GTK_OBJECT (get_widget(dlg_export_message,"pb_aggiungi")), "clicked", GTK_SIGNAL_FUNC (on_dlg_export_message_add), dlg_export_message);
			gtk_signal_connect (GTK_OBJECT (get_widget(dlg_export_message,"pb_overwrite")),"clicked", GTK_SIGNAL_FUNC (on_dlg_export_message_overwrite), dlg_export_message);

		} else {
			gtk_label_printf(lb_1,"Sono presenti dati da trasmettere");
			gtk_label_printf(lb_2,"Premere Ok per iniziare la spedizione");

			pb_ok=get_widget(dlg_export,"pb_ok");
			gtk_signal_connect (GTK_OBJECT (pb_ok), "clicked", GTK_SIGNAL_FUNC (on_dlg_export_do_spedizione_cedola), dlg_export);
		}
	}
}

void on_dlg_export_message_add (GtkButton *button, gpointer user_data)
{
	gtk_widget_destroy(GTK_WIDGET(user_data));
	gtk_window_set_transient_for(GTK_WINDOW(dlg_export),GTK_WINDOW(main_window));
	SpedizioneCedola("a");
}

void on_dlg_export_message_overwrite(GtkButton *button, gpointer user_data)
{
	gtk_widget_destroy(GTK_WIDGET(user_data));
	gtk_window_set_transient_for(GTK_WINDOW(dlg_export),GTK_WINDOW(main_window));
	SpedizioneCedola("w");
}

void on_dlg_export_message_pb_cancel_clicked (gpointer user_data,GtkButton *button)
{
	gtk_widget_destroy(GTK_WIDGET(user_data));
	gtk_widget_destroy(GTK_WIDGET(dlg_export));
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


void on_pbm_codice_spedizione_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	char szParagraph[128];
	char szCurrentDirectory[128];
	char szCniCfg[128];
	char szSelectCmd[2048];
	
	getcwd(szCurrentDirectory,sizeof(szCurrentDirectory));
	
	strcpy(szCniCfg,szCurrentDirectory);
	strcat(szCniCfg,"/cni.cfg");
	/*
	* lettura dal file di configurazione della select
	*/
	strcpy(szParagraph,"Tabella Ordini");
	xncGetFileString(szParagraph,"SELECT_CMD_SPEDIZIONE", "", szSelectCmd, sizeof(szSelectCmd),szCniCfg,NULL); 

	RefreshListaOrdini(sw_list,szSelectCmd);
}


void on_pbm_linea_di_instradamento_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	char szParagraph[128];
	char szCurrentDirectory[128];
	char szCniCfg[128];
	char szSelectCmd[2048];
	
	getcwd(szCurrentDirectory,sizeof(szCurrentDirectory));
	
	strcpy(szCniCfg,szCurrentDirectory);
	strcat(szCniCfg,"/cni.cfg");
	/*
	* lettura dal file di configurazione della select
	*/
	strcpy(szParagraph,"Tabella Ordini");
	xncGetFileString(szParagraph,"SELECT_CMD_INSTRADAMENTO", "", szSelectCmd, sizeof(szSelectCmd),szCniCfg,NULL); 

	RefreshListaOrdini(sw_list,szSelectCmd);
}


void on_pbm_seleziona_tutto_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gtk_clist_select_all (GTK_CLIST (lst_ordini));
}


void on_pbm_deseleziona_tutto_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	gtk_clist_unselect_all (GTK_CLIST (lst_ordini));
}

void on_pbm_seleziona_elaborazione_activate (GtkMenuItem     *menuitem, gpointer         user_data)
{
	/*
	* Impostazione numero settori e isole
	*/
	GtkWidget *dlg=create_dlg_select_elaborazione();
	gtk_widget_show(dlg);
	gtk_window_set_transient_for(GTK_WINDOW(dlg),GTK_WINDOW(main_window));

}

void on_dlg_select_elaborazione_pb_ok_clicked (gpointer user_data,GtkButton *button)
{
	GtkWidget *dlg=GTK_WIDGET(user_data);
	int nRows;
	int nRowIndex;
	char *pszElaborazione;
	char szElaborazione[256];

	gtk_clist_unselect_all (GTK_CLIST (lst_ordini));

	nRows=GTK_CLIST(lst_ordini)->rows;

	strcpy(szElaborazione,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg,"entry_elaborazione"))));

	for(nRowIndex=0;nRowIndex<nRows;nRowIndex++){
		gtk_clist_get_text(GTK_CLIST(lst_ordini),nRowIndex,Cfg.nOrdiniElabField,&pszElaborazione);
		if(!strcmp(szElaborazione,pszElaborazione)){
			gtk_clist_select_row(GTK_CLIST(lst_ordini), nRowIndex, -1);
		}
	}
	gtk_widget_destroy(dlg);
}


void on_pbm_reset_stato_ordine_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GList *lista;

	lista=g_list_first(GTK_CLIST(lst_ordini)->selection);

	if(lista==NULL){
		/*
		* nessun ordine della lista e' selezionato
		*/
		GtkWidget *lb_msg;
		GtkWidget *pb_ok;

		dlg_message=create_dlg_message();

		lb_msg=get_widget(dlg_message,"lb_msg");
		pb_ok=get_widget(dlg_message,"pb_ok");
		gtk_label_printf(lb_msg,"Nessun ordine selezionato");
		gtk_window_set_title (GTK_WINDOW (dlg_message), "Reset Stato Ordini");
		gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
		gtk_signal_connect (GTK_OBJECT (pb_ok), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_pb_ok_clicked), dlg_message);
		gtk_widget_show(dlg_message);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));

	} else {
		char *pszOrdProg;
		char szOrdProg[128];
		int nRowIndex;
		int nOrdini=0;
		char szSQLCmd[512];
		PGresult *PGResOrdini;
		PGresult *PGRes;
		PGresult *PGResNMCPE;
		char cStatoOrdini=ORDINE_RICEVUTO;

		do {
			int nRighe;
			int nCopie;
			int nPeso;
			int nTuples;
			int nRigaIndex;
			int nIndex;
			nRowIndex=GPOINTER_TO_INT(lista->data);
			gtk_clist_get_text(GTK_CLIST(lst_ordini),nRowIndex,Cfg.nOrdiniKeyField,&pszOrdProg);
			strcpy(szOrdProg,pszOrdProg);
			
			/*
			* Ricalcolo numero righe, numero copie e peso dell'ordine che sto resettando
			*/
			PGRes=PGExecSQL(FALSE,"BEGIN WORK;");
			PQclear(PGRes);
			PGResOrdini=PGExecSQL(FALSE,"select ordprog,ronmcpe,ronmrgh from ric_ord where ordprog='%s';",szOrdProg);
			nTuples=PQntuples(PGResOrdini);

			for(nIndex=0;nIndex<nTuples;nIndex++){
				PGResNMCPE=PGExecSQL(FALSE,"select r.raqtord,a.prpesgr from ric_art as r, articoli as a where r.racdpro=a.prcdpro AND ordprog='%s';",szOrdProg);
				nRighe=PQntuples(PGResNMCPE);
				nCopie=0;
				nPeso=0;
				for(nRigaIndex=0;nRigaIndex<nRighe;nRigaIndex++){
					nCopie+=atoi(PQgetvalue(PGResNMCPE,nRigaIndex,0));
					nPeso+=atoi(PQgetvalue(PGResNMCPE,nRigaIndex,0))*atoi(PQgetvalue(PGResNMCPE,nRigaIndex,1));
				}
				PQclear(PGResNMCPE);
				PGResNMCPE=PGExecSQL(Cfg.nDebugVersion,"update ric_ord set ronmcpe=%d,ronmrgh=%d,ropspre=%d where ordprog='%s';",nCopie,nRighe,nPeso,szOrdProg);
				PQclear(PGResNMCPE);
			}
			PGRes=PGExecSQL(FALSE,"COMMIT WORK;");
			PQclear(PGRes);

			/*
			* resetto lo stato degli ordini (in ric_ord e ord_prod) e delle relative righe e colli
			*/
			sprintf(szSQLCmd,"update ord_prod set opstato='%c' where ordprog='%s';",cStatoOrdini,szOrdProg);
			PGRes=PGExecSQL(Cfg.nDebugVersion,szSQLCmd);
			PQclear(PGRes);
			sprintf(szSQLCmd,"update rig_prod set rpstato='%c' where ordprog='%s';",cStatoOrdini,szOrdProg);
			PGRes=PGExecSQL(Cfg.nDebugVersion,szSQLCmd);
			PQclear(PGRes);
			sprintf(szSQLCmd,"update col_prod set opstato='%c' where ordprog='%s';",cStatoOrdini,szOrdProg);
			PGRes=PGExecSQL(Cfg.nDebugVersion,szSQLCmd);
			PQclear(PGRes);

			sprintf(szSQLCmd,"update ric_ord  set rostato='%c',ronmcll=NULL where ordprog='%s';",cStatoOrdini,szOrdProg);
			PGResOrdini=PGExecSQL(Cfg.nDebugVersion,szSQLCmd);
			
			if(atoi(PQcmdTuples(PGResOrdini))==0){
				gtk_text_printf(NULL,txt_msgs,"Fallito reset Ordine %s\n",szOrdProg);
			} else { 
				nOrdini++;
				/*
				* Update a video
				*/
				UpdateOrdine(szOrdProg);
				if(Cfg.nDebugVersion){
					gtk_text_printf(NULL,txt_msgs,"Resettato Ordine %s\n",szOrdProg);
				}
			}

			PQclear(PGResOrdini);
			gtk_update();

		} while((lista=g_list_next(lista)));
		gtk_text_printf(NULL,txt_msgs,"Resettati %d Ordini\n",nOrdini);

#ifdef TRACE
		trace_out_vstr_date(1,"Resettati %d Ordini\n",nOrdini);
#endif

	}
}


void on_pbm_lancio_ordini_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GList *lista;
	char *pszOrdineKey;
	char szOrdineKey[128];
	int nRowIndex;
	int nOrdini=0;
	char szStato[80];
	char szStatoOrdine[80];
	char cStatoSpedito;
	PGresult *PGRes = NULL;
	BOOL bVOLUM=FALSE;
	int nProgressivoPrecedenza=0;

	sprintf(szStato,"%c",ORDINE_SPEDITO);

	lista=g_list_first(GTK_CLIST(lst_ordini)->selection);
	if(lista==NULL){
		return;
	}

	/*
	* Richiesta Fabio
	* Si vuole che gli ordini lanciati abbiano precedenza rispetto a quelli lanciati successivamente
	* anche se il progressivo lancio e' piu' basso. Infatti sorgeva il problema di elaborazioni
	* lanciate per i colli con pdf che sorpassavano quelle in linea con progressivo piu' alto
	* Aggiungo alla tabella ric_ord e ord_prod un progressivo precedenza assegnato al lancio da parte dell'
	* operatore, non come l'altro progressivo che se esiste gia' non si aggiorna.
	*/
	/*
	* Nuovo progressivo di precedenza
	*/
	PGRes=PGExecSQL(Cfg.nDebugVersion>2,"select nextval ('prog_lancio_precedenza');");
	nProgressivoPrecedenza=atoi(PQgetvalue(PGRes,0,0));
	PQclear(PGRes);
	
	do {
		nRowIndex=GPOINTER_TO_INT(lista->data);
		gtk_clist_get_text(GTK_CLIST(lst_ordini),nRowIndex,Cfg.nOrdiniKeyField,&pszOrdineKey);
		strcpy(szOrdineKey,pszOrdineKey);

		//Update su ric_ord	
		PGRes=PGExecSQL(Cfg.nDebugVersion,"update ric_ord set roprgpr=%d where ordprog='%s';",nProgressivoPrecedenza,pszOrdineKey);
		PQclear(PGRes);
	
		
		PGRes=PGExecSQL(Cfg.nDebugVersion>1,"select rostato from ric_ord where ordprog='%s';",szOrdineKey);
		strcpy(szStatoOrdine,PQgetvalue(PGRes,0,0));
		cStatoSpedito=szStatoOrdine[0];
		PQclear(PGRes);

/*******************************************************
* VERSIONE CON GESTIONE PDF
********************************************************/


		switch(cStatoSpedito){
			case ORDINE_RICEVUTO:
//			case ORDINE_SPEDITO:
			{
				sprintf(szStato,"%c",ORDINE_SPEDITO);
				bVOLUM=TRUE;
			}
			break;
			case ORDINE_INCOMPLETO_TERMINATO:
//			case ORDINE_INCOMPLETO_TERMINATO_SPEDITO:
			{
				sprintf(szStato,"%c",ORDINE_INCOMPLETO_TERMINATO_SPEDITO);
			}
			break;
			default:
				continue;
			break;

			
		}

/*******************************************************
* FINE VERSIONE CON GESTIONE PDF
********************************************************/
																		 

		if(!LanciaOrdine(szOrdineKey,szStato)){
			gtk_text_printf(NULL,txt_msgs,"Errore in lancio ordine [%s]\n",szOrdineKey);
		} else {
			nOrdini++;
			gtk_clist_set_text(GTK_CLIST(lst_ordini),nRowIndex,Cfg.nOrdiniStatusField,szStato);
		}
	} while((lista=g_list_next(lista)));
	gtk_text_printf(NULL,txt_msgs,"Lanciati [%d] ordini\n",nOrdini);
	if (bVOLUM) {
		SendMessage(PROC_VOLUM, PROC_MAIN, START_ANALISI, NULL);
	}
}

void on_pbm_forza_ordine_senza_documento_activate (GtkMenuItem     *menuitem, gpointer         user_data)
{
	GList *lista;
	char *pszOrdineKey;
	char szOrdineKey[128];
	int nRowIndex;
	int nOrdini=0;
	char szStato[80];
	char szStatoOrdine[80];
	char szFlagPdf[80];
	char cStatoSpedito;
	char cFlagPdf;
	PGresult *PGRes = NULL;

	sprintf(szStato,"%c",ORDINE_SENZA_PDF);

	lista=g_list_first(GTK_CLIST(lst_ordini)->selection);
	if(lista==NULL){
		/*
		* nessun ordine della lista e' selezionato
		*/
		GtkWidget *lb_msg;
		GtkWidget *pb_ok;

		dlg_message=create_dlg_message();

		lb_msg=get_widget(dlg_message,"lb_msg");
		pb_ok=get_widget(dlg_message,"pb_ok");
		gtk_label_printf(lb_msg,"Nessun ordine selezionato");
		gtk_window_set_title (GTK_WINDOW (dlg_message), "Reset Stato Ordini");
		gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
		gtk_signal_connect (GTK_OBJECT (pb_ok), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_pb_ok_clicked), dlg_message);
		gtk_widget_show(dlg_message);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));
	} else {
		do {
			nRowIndex=GPOINTER_TO_INT(lista->data);
			gtk_clist_get_text(GTK_CLIST(lst_ordini),nRowIndex,Cfg.nOrdiniKeyField,&pszOrdineKey);
			strcpy(szOrdineKey,pszOrdineKey);
			
			PGRes=PGExecSQL(Cfg.nDebugVersion>1,"select rostato, roflpdf from ric_ord where ordprog='%s';",szOrdineKey);
			strcpy(szStatoOrdine,PQgetvalue(PGRes,0,0));
			strcpy(szFlagPdf,PQgetvalue(PGRes,0,1));
			cStatoSpedito=szStatoOrdine[0];
			cFlagPdf=szFlagPdf[0];
			PQclear(PGRes);


			if (Cfg.nAttesaPDFOrdineCompleto==1){
				switch(cStatoSpedito){
					case ORDINE_RICEVUTO:
					case ORDINE_SPEDITO:
					case ORDINE_ELABORATO:
					{
						if (cFlagPdf==ORDINE_IN_ATTESA_PDF){
							PGRes=PGExecSQL(Cfg.nDebugVersion>1,"update ric_ord set roflpdf='%c' where ordprog='%s';",ORDINE_SENZA_PDF,szOrdineKey);
							PQclear(PGRes);
							PGRes=PGExecSQL(Cfg.nDebugVersion>1,"update ord_prod set opflpdf='%c' where ordprog='%s';",ORDINE_SENZA_PDF,szOrdineKey);
							PQclear(PGRes);
							/*
							* Non devo togliere il flag al pdf se no settori:InizioPrelievoSettore non
							* fa passare l'ordine!!!!
							*/
							//PGRes=PGExecSQL(Cfg.nDebugVersion>1,"update col_prod set cpflpdf='%c' where ordprog='%s';",COLLO_SENZA_PDF,szOrdineKey);
							//PQclear(PGRes);
							nOrdini++;
							gtk_clist_set_text(GTK_CLIST(lst_ordini),nRowIndex,Cfg.nOrdiniPdfStatusField,szStato);
						}
					}
					break;
				}
			} else {
				switch(cStatoSpedito){
					case ORDINE_INCOMPLETO_TERMINATO:
					case ORDINE_INCOMPLETO_TERMINATO_SPEDITO:
					{
						if (cFlagPdf==ORDINE_IN_ATTESA_PDF){
							PGRes=PGExecSQL(Cfg.nDebugVersion>1,"update ric_ord set roflpdf='%c' where ordprog='%s';",ORDINE_SENZA_PDF,szOrdineKey);
							PQclear(PGRes);
							PGRes=PGExecSQL(Cfg.nDebugVersion>1,"update ord_prod set opflpdf='%c' where ordprog='%s';",ORDINE_SENZA_PDF,szOrdineKey);
							PQclear(PGRes);
							/*
							* Non devo togliere il flag al pdf se no settori:InizioPrelievoSettore non
							* fa passare l'ordine!!!!
							*/
							//PGRes=PGExecSQL(Cfg.nDebugVersion>1,"update col_prod set cpflpdf='%c' where ordprog='%s';",COLLO_SENZA_PDF,szOrdineKey);
							//PQclear(PGRes);
							nOrdini++;
							gtk_clist_set_text(GTK_CLIST(lst_ordini),nRowIndex,Cfg.nOrdiniPdfStatusField,szStato);
						}
					}
					break;
				}
			}


			
		} while((lista=g_list_next(lista)));

		gtk_text_printf(NULL,txt_msgs,"Documento forzato per [%d] ordini\n",nOrdini);

	}

}


void on_pbm_stampa_documenti_activate       (GtkMenuItem     *menuitem, gpointer         user_data)
{

	GList *lista;
	char *pszOrdineKey;
	char szOrdineKey[128];
	int nRowIndex;
	int nOrdini=0;
	//char szStato[80];
	char szStatoOrdine[80];
	char szFlagPdf[80];
	char cStatoSpedito;
	char cFlagPdf;
	char szPdfPrinter[128];
	char szBuffer[128];
	PGresult *PGRes = NULL;


	lista=g_list_first(GTK_CLIST(lst_ordini)->selection);
	if(lista==NULL){
		/*
		* nessun ordine della lista e' selezionato
		*/
		GtkWidget *lb_msg;
		GtkWidget *pb_ok;

		dlg_message=create_dlg_message();

		lb_msg=get_widget(dlg_message,"lb_msg");
		pb_ok=get_widget(dlg_message,"pb_ok");
		gtk_label_printf(lb_msg,"Nessun ordine selezionato");
		gtk_window_set_title (GTK_WINDOW (dlg_message), "Stampa Documenti Ordini");
		gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
		gtk_signal_connect (GTK_OBJECT (pb_ok), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_pb_ok_clicked), dlg_message);
		gtk_widget_show(dlg_message);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));
	} else {

		do {
			nRowIndex=GPOINTER_TO_INT(lista->data);
			gtk_clist_get_text(GTK_CLIST(lst_ordini),nRowIndex,Cfg.nOrdiniKeyField,&pszOrdineKey);
			strcpy(szOrdineKey,pszOrdineKey);
			
			PGRes=PGExecSQL(Cfg.nDebugVersion>1,"select rostato, roflpdf from ric_ord where ordprog='%s';",szOrdineKey);
			strcpy(szStatoOrdine,PQgetvalue(PGRes,0,0));
			strcpy(szFlagPdf,PQgetvalue(PGRes,0,1));
			cStatoSpedito=szStatoOrdine[0];
			cFlagPdf=szFlagPdf[0];
			PQclear(PGRes);

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
			sprintf(szBuffer,"%s,%s",szOrdineKey,szPdfPrinter);
			SendMessage(PROC_PRINTLABEL,PROC_MAIN,PRINT_PDF,szBuffer);


			nOrdini++;
			
		} while((lista=g_list_next(lista)));

		gtk_text_printf(NULL,txt_msgs,"Stampati documenti per [%d] ordini\n",nOrdini);

	}
}



void on_pbm_statistiche_carico_settori_activate (GtkMenuItem     *menuitem, gpointer         user_data)
{
	char szParagraph[128];
	char szSelectCmd[256];
	char szListName[256];
	GtkWidget *dlg_carico_settori = create_dlg_carico_settori();
	GtkWidget *sw_elaborazioni=get_widget(dlg_carico_settori,"sw_elaborazioni");
	GtkWidget *lst;
	GtkWidget *rl_widget;
	int nIsolaIndex;
	char szSettore[64];

	/*
	* lettura del file di configurazione
	*/
	strcpy(szParagraph,"Carico Settori");
	xncGetFileString(szParagraph,"SELECT_CMD",  "", szSelectCmd, sizeof(szSelectCmd),Cfg.szCniCfg,NULL); 

	strcpy(szListName,"lst_elaborazioni");
	if((lst=get_widget(dlg_carico_settori,szListName))){
		gtk_widget_destroy(lst);
	}

	CreateListFromSelect(dlg_carico_settori,sw_elaborazioni,&lst,"lst_elaborazioni",GTK_SELECTION_MULTIPLE,szSelectCmd,NULL,0);

	for(nIsolaIndex=1;nIsolaIndex<=2;nIsolaIndex++){
		sprintf(szSettore,"rl_%d",nIsolaIndex);
		if((rl_widget=get_widget(dlg_carico_settori,szSettore))){
			gtk_ruler_set_range(GTK_RULER(rl_widget),100,(gfloat)0,(gfloat)0,100);
		}
	}

	gtk_window_set_focus (GTK_WINDOW (dlg_carico_settori), get_widget(dlg_carico_settori,"pb_apply"));
	gtk_widget_show(dlg_carico_settori);
	gtk_window_set_transient_for(GTK_WINDOW(dlg_carico_settori),GTK_WINDOW(main_window));
}


void on_pbm_start_impianto_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	do_start_impianto();
}


void on_pbm_stop_impianto_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	do_stop_impianto();
}


void on_pbm_reset_settori_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *lb_msg;
	GtkWidget *pb_ok;

	/*
	* Visualizza finestra di conferma reset
	*/
	dlg_message=create_dlg_message();

	lb_msg=get_widget(dlg_message,"lb_msg");
	pb_ok=get_widget(dlg_message,"pb_ok");

	gtk_label_printf(lb_msg,"Confermi il reset dei settori");
	gtk_window_set_title (GTK_WINDOW (dlg_message), "Reset Settori");

	gtk_widget_show(dlg_message);
	gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));
	/*
	* callback per pressione pulsante
	*/
	gtk_signal_connect (GTK_OBJECT (pb_ok), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_pb_reset_settori), dlg_message);

}


void on_pbm_reset_di_un_settore_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	/*
	* Impostazione numero settori e isole
	*/
	dlg_reset_settore=create_dlg_reset_settore();
	gtk_widget_show(dlg_reset_settore);
	gtk_window_set_transient_for(GTK_WINDOW(dlg_reset_settore),GTK_WINDOW(main_window));

}


void on_pbm_edit_ordine_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	char *pszOrdineKey;

	dlg_edit_ordine = create_dlg_edit_ordine();

	gtk_clist_get_text(GTK_CLIST(lst_ordini),nLstOrdiniRowSelected,Cfg.nOrdiniKeyField,&pszOrdineKey);
	strcpy(szOrdineInEdit,pszOrdineKey);
	gtk_window_set_focus (GTK_WINDOW (dlg_edit_ordine), get_widget(dlg_edit_ordine,"pb_ok"));
	gtk_widget_show(dlg_edit_ordine);
	gtk_window_set_transient_for(GTK_WINDOW(dlg_edit_ordine),GTK_WINDOW(main_window));

	/* visualizzo in dlg_edit_ordine dati di produzione */
	bStorico=FALSE;
	EditOrdine(pszOrdineKey);
}


void on_pbm_distribuzione_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	dlg_distribuzione = create_dlg_distribuzione();
	gtk_window_set_focus (GTK_WINDOW (dlg_distribuzione), get_widget(dlg_distribuzione,"pb_ok"));
	gtk_widget_show(dlg_distribuzione);
	gtk_window_set_transient_for(GTK_WINDOW(dlg_distribuzione),GTK_WINDOW(main_window));
	ListaDistribuzione();
}



void on_pbm_aggiorna_lista_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *sw_list=get_widget(main_window,"sw_list");

	RefreshListaOrdini(sw_list,NULL);
}


void on_pbm_carica_stato_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *lb_msg;
	GtkWidget *pb_ok;

	dlg_message=create_dlg_message();

	lb_msg=get_widget(dlg_message,"lb_msg");
	pb_ok=get_widget(dlg_message,"pb_ok");

	gtk_label_printf(lb_msg,"PREMERE OK PER CONFERMARE IL CARICAMENTO DELLO STATO DELLA LINEA");
  gtk_window_set_title (GTK_WINDOW (dlg_message), "Caricamento stato linea");
	gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
	gtk_widget_show(dlg_message);
	gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));
	/*
	* callback per pressione pulsante
	*/
  gtk_signal_connect (GTK_OBJECT (pb_ok), "clicked", GTK_SIGNAL_FUNC (on_carica_stato_linea_pb_ok_clicked), dlg_message);

}


void on_pbm_salva_stato_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *lb_msg;
	GtkWidget *pb_ok;

	dlg_message=create_dlg_message();

	lb_msg=get_widget(dlg_message,"lb_msg");
	pb_ok=get_widget(dlg_message,"pb_ok");

	gtk_label_printf(lb_msg,"PREMERE OK PER CONFERMARE IL SALVATAGGIO DELLO STATO DELLA LINEA");
  gtk_window_set_title (GTK_WINDOW (dlg_message), "Salvataggio stato linea");
	gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
	gtk_widget_show(dlg_message);
	gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));
	/*
	* callback per pressione pulsante
	*/
  gtk_signal_connect (GTK_OBJECT (pb_ok), "clicked", GTK_SIGNAL_FUNC (on_salva_stato_linea_pb_ok_clicked), dlg_message);
}


void on_pbm_exit_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	dlg_exit = create_dlg_exit();
	gtk_window_set_focus (GTK_WINDOW (dlg_exit), get_widget(dlg_exit,"pb_ok"));
	gtk_widget_show(dlg_exit);
	gtk_window_set_transient_for(GTK_WINDOW(dlg_exit),GTK_WINDOW(main_window));
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

void on_pbm_stampa_da_storico_activate      (GtkMenuItem     *menuitem, gpointer         user_data)
{
	dlg_storico = create_dlg_storico();
	gtk_window_set_title (GTK_WINDOW (dlg_storico), "Stampa Ordini in Archivio Storico");
	gtk_window_set_focus (GTK_WINDOW (dlg_storico), get_widget(dlg_storico,"entry_ordkey"));
	gtk_widget_show(dlg_storico);
	gtk_window_set_transient_for(GTK_WINDOW(dlg_storico),GTK_WINDOW(main_window));

	gtk_signal_connect (GTK_OBJECT(get_widget(dlg_storico,"pb_ok")), "clicked", GTK_SIGNAL_FUNC (on_dlg_stampa_storico_pb_ok_clicked), dlg_storico);
}

void on_pbm_ricerca_in_storico_activate     (GtkMenuItem     *menuitem, gpointer         user_data)
{
	dlg_storico = create_dlg_storico();
	gtk_window_set_title (GTK_WINDOW (dlg_storico), "Stampa Packing-List ordini da Archivio Storico");
	gtk_window_set_focus (GTK_WINDOW (dlg_storico), get_widget(dlg_storico,"entry_ordkey"));
	gtk_widget_show(dlg_storico);
	gtk_window_set_transient_for(GTK_WINDOW(dlg_storico),GTK_WINDOW(main_window));

	gtk_signal_connect (GTK_OBJECT(get_widget(dlg_storico,"pb_ok")), "clicked", GTK_SIGNAL_FUNC (on_dlg_ricerca_storico_pb_ok_clicked), dlg_storico);
}

void on_pbm_configurazione_activate(GtkMenuItem * menuitem, gpointer user_data) 
{
	EditConfigurazione();
}

void on_dlg_config_pb_modifica_imb_clicked  (GtkButton       *button, gpointer         user_data)
{
	PGresult *PGRes;
	GtkWidget *lst;
	char szBuffer[128];
	char szSqlCmd[1024];

	strcpy(szSqlCmd,"update imballi set descriz='");
	strcat(szSqlCmd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_descrizione_imb"))));
	strcat(szSqlCmd,"', lunghezza=");
	strcat(szSqlCmd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_lunghezza_imb"))));
	strcat(szSqlCmd,", altezza=");
	strcat(szSqlCmd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_altezza_imb"))));
	strcat(szSqlCmd,", larghezza=");
	strcat(szSqlCmd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_larghezza_imb"))));
	strcat(szSqlCmd,", tara=");
	strcat(szSqlCmd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_tara_imb"))));
	strcat(szSqlCmd,", pesomax=");
	strcat(szSqlCmd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_pesomax_imb"))));
	strcat(szSqlCmd,", volperc1=");
	sprintf(szBuffer,"%d",gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(get_widget(dlg_config,"sb_volperc_imb_1"))));
	strcat(szSqlCmd,szBuffer);
	strcat(szSqlCmd,", volperc2=");
	sprintf(szBuffer,"%d",gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(get_widget(dlg_config,"sb_volperc_imb_2"))));
	strcat(szSqlCmd,szBuffer);
	strcat(szSqlCmd,", tplavor=");
	strcat(szSqlCmd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_tp_lavor_imb"))));
	strcat(szSqlCmd," where codice='");
	strcat(szSqlCmd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_codice_imb"))));
	strcat(szSqlCmd,"';");
	
	PGRes=PGExecSQL(Cfg.nDebugVersion,szSqlCmd);
	if (PQresultStatus(PGRes) == PGRES_COMMAND_OK ) {
#ifdef TRACE
		trace_out_vstr_date(1,"Modificato imballo [%s]",gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_codice_imb"))));
#endif
	} else {
#ifdef TRACE
		trace_out_vstr_date(1,"Fallita modifica su parametri imballo [%s]",gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_codice_imb"))));
#endif
	}
	PQclear(PGRes);

	if((lst=get_widget(dlg_config,"lst_imballi"))){
		gtk_widget_destroy(lst);
	}
	strcpy(szSqlCmd,"select CODICE, DESCRIZ, LUNGHEZZA, ALTEZZA, LARGHEZZA, TARA, PESOMAX, VOLPERC1, VOLPERC2, TPLAVOR from imballi order by codice;");
	CreateListFromSelect(dlg_config,get_widget(dlg_config,"sw_lista_imballi"),&lst,"lst_imballi",GTK_SELECTION_SINGLE,szSqlCmd,NULL,0);
  gtk_signal_connect (GTK_OBJECT (get_widget(dlg_config,"lst_imballi")), "select_row", GTK_SIGNAL_FUNC (on_lista_imballi_row_selected), 0);
}

void on_dlg_config_pb_aggiungi_imb_clicked  (GtkButton       *button, gpointer         user_data)
{
	PGresult *PGRes;
	GtkWidget *lst;
	char szBuffer[128];
	char szSqlCmd[1024];

	strcpy(szSqlCmd,"insert into imballi ( CODICE,   DESCRIZ,  LUNGHEZZA, ALTEZZA,  LARGHEZZA,	TARA,     	PESOMAX,  	VOLPERC1, VOLPERC2,	TPLAVOR	) values ('");
	strcat(szSqlCmd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_codice_imb"))));
	strcat(szSqlCmd,"', '");
	strcat(szSqlCmd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_descrizione_imb"))));
	strcat(szSqlCmd,"', ");
	strcat(szSqlCmd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_lunghezza_imb"))));
	strcat(szSqlCmd,", ");
	strcat(szSqlCmd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_altezza_imb"))));
	strcat(szSqlCmd,", ");
	strcat(szSqlCmd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_larghezza_imb"))));
	strcat(szSqlCmd,", ");
	strcat(szSqlCmd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_tara_imb"))));
	strcat(szSqlCmd,", ");
	strcat(szSqlCmd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_pesomax_imb"))));
	strcat(szSqlCmd,", ");
	sprintf(szBuffer,"%d",gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(get_widget(dlg_config,"entry_volperc_imb_1"))));
	strcat(szSqlCmd,szBuffer);
	strcat(szSqlCmd,", ");
	sprintf(szBuffer,"%d",gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(get_widget(dlg_config,"entry_volperc_imb_2"))));
	strcat(szSqlCmd,szBuffer);
	strcat(szSqlCmd,", ");
	strcat(szSqlCmd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_tp_lavor_imb"))));
	strcat(szSqlCmd,");");
	
	PGRes=PGExecSQL(Cfg.nDebugVersion,szSqlCmd);
	if (PQresultStatus(PGRes) == PGRES_COMMAND_OK ) {
#ifdef TRACE
		trace_out_vstr_date(1,"Inserito imballo [%s]",gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_codice_imb"))));
#endif
	} else {
#ifdef TRACE
		trace_out_vstr_date(1,"Fallito inserimento imballo [%s]",gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_codice_imb"))));
#endif
	}
	PQclear(PGRes);

	if((lst=get_widget(dlg_config,"lst_imballi"))){
		gtk_widget_destroy(lst);
	}
	strcpy(szSqlCmd,"select CODICE, DESCRIZ, LUNGHEZZA, ALTEZZA, LARGHEZZA, TARA, PESOMAX, VOLPERC1, VOLPERC2, TPLAVOR from imballi order by codice;");
	CreateListFromSelect(dlg_config,get_widget(dlg_config,"sw_lista_imballi"),&lst,"lst_imballi",GTK_SELECTION_SINGLE,szSqlCmd,NULL,0);
  gtk_signal_connect (GTK_OBJECT (get_widget(dlg_config,"lst_imballi")), "select_row", GTK_SIGNAL_FUNC (on_lista_imballi_row_selected), 0);
}

void on_dlg_config_pb_elimina_imb_clicked   (GtkButton       *button, gpointer         user_data)
{
	GtkWidget *lb_msg;
	GtkWidget *pb_ok;
	char szImballo[40];

	dlg_message=create_dlg_message();

	lb_msg=get_widget(dlg_message,"lb_msg");
	pb_ok=get_widget(dlg_message,"pb_ok");
	sprintf(szImballo,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_codice_imb"))));

	if(strlen(szImballo)!=0){
		gtk_label_printf(lb_msg,"CONFERMI L'ELIMINAZIONE\nDELL'IMBALLO %s",szImballo);
		gtk_window_set_title (GTK_WINDOW (dlg_message), "Elimina Imballo");
		gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
		gtk_widget_show(dlg_message);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));
		/*
		* callback per pressione pulsante
		*/
		gtk_signal_connect (GTK_OBJECT (pb_ok), "clicked", GTK_SIGNAL_FUNC (on_dlg_config_do_elimina_imballo), dlg_message);
	} else {
		gtk_label_printf(lb_msg,"NESSUN IMBALLO \nSELEZIONATO",szImballo);
		gtk_window_set_title (GTK_WINDOW (dlg_message), "Elimina Imballo");
		gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
		gtk_widget_show(dlg_message);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));
		gtk_signal_connect (GTK_OBJECT (pb_ok), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_pb_ok_clicked), dlg_message);
	}
}

void on_dlg_config_do_elimina_imballo (GtkButton *button, gpointer user_data)
{
	GtkWidget *lst;
	PGresult *PGRes;
	char szSqlCmd[1024];

	gtk_widget_destroy(dlg_message);

	strcpy(szSqlCmd,"delete from imballi where codice='");
	strcat(szSqlCmd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_codice_imb"))));
	strcat(szSqlCmd,"';");

	PGRes=PGExecSQL(Cfg.nDebugVersion,szSqlCmd);
	if (PQresultStatus(PGRes) == PGRES_COMMAND_OK ) {
#ifdef TRACE
		trace_out_vstr_date(1,"Eliminato imballo [%s]",gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_codice_imb"))));
#endif
	} else {
#ifdef TRACE
		trace_out_vstr_date(1,"Fallita eliminazione imballo [%s]",gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_config,"entry_codice_imb"))));
#endif
	}
	PQclear(PGRes);

	if((lst=get_widget(dlg_config,"lst_imballi"))){
		gtk_widget_destroy(lst);
	}
	strcpy(szSqlCmd,"select CODICE, DESCRIZ, LUNGHEZZA, ALTEZZA, LARGHEZZA, TARA, PESOMAX, VOLPERC1, VOLPERC2, TPLAVOR  from imballi order by codice;");

	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_config,"entry_codice_imb")),"");
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_config,"entry_descrizione_imb")),"");
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_config,"entry_lunghezza_imb")),"");
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_config,"entry_altezza_imb")),"");
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_config,"entry_larghezza_imb")),"");
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_config,"entry_tara_imb")),"");
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_config,"entry_pesomax_imb")),"");
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(get_widget(dlg_config,"sb_volperc_imb_1")),(gdouble)0);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(get_widget(dlg_config,"sb_volperc_imb_2")),(gdouble)0);
	gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_config,"entry_tp_lavor_imb")),"");

	CreateListFromSelect(dlg_config,get_widget(dlg_config,"sw_lista_imballi"),&lst,"lst_imballi",GTK_SELECTION_SINGLE,szSqlCmd,NULL,0);
  gtk_signal_connect (GTK_OBJECT (get_widget(dlg_config,"lst_imballi")), "select_row", GTK_SIGNAL_FUNC (on_lista_imballi_row_selected), 0);
}


void on_pbm_operatori_activate (GtkMenuItem     *menuitem, gpointer         user_data)
{
	GtkWidget *lst;
	GtkWidget *ComboOperatore;
	GList *listaOperatori=NULL;
	PGresult *PGRes;
	char szSelectCmd[1024];
	int nIndex;

	dlg_operatori = create_dlg_operatori();
	/*
	* Lista Operatori - Ubicazione
	*/
	if((lst=get_widget(dlg_operatori,"lst_opeset"))){
		gtk_widget_destroy(lst);
	}
	strcpy(szSelectCmd,"select a.opisola as \"ISOLA\", a.opset as \"SETTORE\", a.opcdope as \"OPERATORE\", op.opnmope as \"NOME\", op.opcgope as \"COGNOME\", a.dtmod as \"DATA ASSEGNAZIONE\", a.tmmod as \"ORA ASSEGNAZIONE\" from opeset a, operatori op where a.opcdope=op.opcdope order by opisola,opset;");
	CreateListFromSelect(dlg_operatori,get_widget(dlg_operatori,"sw_lista_opeset"),&lst,"lst_opeset",GTK_SELECTION_SINGLE,szSelectCmd,NULL,0);
  gtk_signal_connect (GTK_OBJECT (get_widget(dlg_operatori,"lst_opeset")), "select_row", GTK_SIGNAL_FUNC (on_lista_opeset_row_selected), 0);

	/*
	* riempio il combo contenente i codici operatori
	*/
	ComboOperatore=get_widget(dlg_operatori,"combo_operatore");
	PGRes=PGExecSQL(Cfg.nDebugVersion,"select opcdope from operatori order by opcdope;");
	for(nIndex=0;nIndex<PQntuples(PGRes);nIndex++){
		listaOperatori=g_list_append(listaOperatori,PQgetvalue(PGRes,nIndex,0));
	}
	gtk_combo_set_popdown_strings(GTK_COMBO(ComboOperatore),listaOperatori);
	g_list_free(listaOperatori);
	PQclear(PGRes);

	/*
	* Lista Anagrafica Operatori
	*/
	if((lst=get_widget(dlg_operatori,"lst_opeana"))){
		gtk_widget_destroy(lst);
	}
	strcpy(szSelectCmd,"select opcdope as \"CODICE\",opnmope as \"NOME\",opcgope as \"COGNOME\",opinope as \"INDIRIZZO\",oploope as \"LOCALITA'\",opprope as \"PROV\",opcpope as \"CAP\",opnmtel as \"TELEFONO\" from operatori order by opcgope,opnmope;");
	CreateListFromSelect(dlg_operatori,get_widget(dlg_operatori,"sw_anagrafica_operatori"),&lst,"lst_opeana",GTK_SELECTION_SINGLE,szSelectCmd,NULL,0);
  gtk_signal_connect (GTK_OBJECT (get_widget(dlg_operatori,"lst_opeana")), "select_row", GTK_SIGNAL_FUNC (on_lista_opeana_row_selected), 0);

	gtk_window_set_focus (GTK_WINDOW (dlg_operatori), get_widget(dlg_operatori,"pb_ok"));
	gtk_window_set_transient_for(GTK_WINDOW(dlg_operatori),GTK_WINDOW(main_window));
	gtk_widget_show(dlg_operatori);
}

void on_pbm_prodotti_activate               (GtkMenuItem     *menuitem, gpointer         user_data)
{
	GtkWidget *dlg_prodotti;

	dlg_prodotti = create_dlg_prodotti();

	/*
	* Lista Anagrafica Prodotti
	*/
	RefreshListaProdotti(dlg_prodotti);

	gtk_window_set_focus (GTK_WINDOW (dlg_prodotti), get_widget(dlg_prodotti,"pb_ok"));
	gtk_window_set_transient_for(GTK_WINDOW(dlg_prodotti),GTK_WINDOW(main_window));
	gtk_widget_show(dlg_prodotti);
}

void on_pbm_prodotti_pericolosi_activate    (GtkMenuItem     *menuitem, gpointer         user_data)
{
	GtkWidget *dlg_prodotti_pericolosi;

	dlg_prodotti_pericolosi = create_dlg_prodotti_pericolosi();

	/*
	* Lista Anagrafica Prodotti
	*/
	RefreshListaProdottiPericolosi(dlg_prodotti_pericolosi);

	gtk_window_set_focus (GTK_WINDOW (dlg_prodotti_pericolosi), get_widget(dlg_prodotti_pericolosi,"pb_ok"));
	gtk_window_set_transient_for(GTK_WINDOW(dlg_prodotti_pericolosi),GTK_WINDOW(main_window));
	gtk_widget_show(dlg_prodotti_pericolosi);

}


void on_mb_main_activate_current (GtkMenuShell *menushell, gboolean force_hide, gpointer user_data)
{

}


/*
* ------------------------------------------------------------
*                   FINE MENU CALLBACKS
* ------------------------------------------------------------
*                   INIZIO TOOLBOX CALLBACKS
* ------------------------------------------------------------
*/
void on_main_pb_carico_clicked(GtkButton *button, gpointer user_data)
{
	char szParagraph[128];
	char szSelectCmd[256];
	char szListName[256];
	GtkWidget *dlg_carico_settori = create_dlg_carico_settori();
	GtkWidget *sw_elaborazioni=get_widget(dlg_carico_settori,"sw_elaborazioni");
	GtkWidget *lst;
	GtkWidget *rl_widget;
	int nIsolaIndex;
	char szSettore[64];

	/*
	* lettura del file di configurazione
	*/
	strcpy(szParagraph,"Carico Settori");
	xncGetFileString(szParagraph,"SELECT_CMD",  "", szSelectCmd, sizeof(szSelectCmd),Cfg.szCniCfg,NULL); 

	strcpy(szListName,"lst_elaborazioni");
	if((lst=get_widget(dlg_carico_settori,szListName))){
		gtk_widget_destroy(lst);
	}

	CreateListFromSelect(dlg_carico_settori,sw_elaborazioni,&lst,"lst_elaborazioni",GTK_SELECTION_MULTIPLE,szSelectCmd,NULL,0);

	for(nIsolaIndex=1;nIsolaIndex<=2;nIsolaIndex++){
		sprintf(szSettore,"rl_%d",nIsolaIndex);
		if((rl_widget=get_widget(dlg_carico_settori,szSettore))){
			gtk_ruler_set_range(GTK_RULER(rl_widget),100,(gfloat)0,(gfloat)0,100);
		}
	}

	gtk_window_set_focus (GTK_WINDOW (dlg_carico_settori), get_widget(dlg_carico_settori,"pb_apply"));
	gtk_widget_show(dlg_carico_settori);
	gtk_window_set_transient_for(GTK_WINDOW(dlg_carico_settori),GTK_WINDOW(main_window));
}

void on_main_pb_start_clicked(GtkButton * button, gpointer user_data)
{
	do_start_impianto();
}


void on_main_pb_stop_clicked(GtkButton * button, gpointer user_data)
{
	do_stop_impianto();
}


void on_main_pb_select_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *cb_field;
	GtkWidget *cb_expression;
	GtkWidget *txf_select;
	GList *items = NULL;
	PGresult *PGRes = NULL;
	int nFields;
	int nFieldIndex;

	dlg_select=create_dlg_select();

	txf_select=get_widget(dlg_select,"txf_select");
	
	/*
	* Reset Select Ordini da lanciare
	*/
	strcpy(szSelectOrdiniCmd,"select * from ric_ord where ");
	sprintf(szLancioOrdiniCmd,"update ric_ord set rostato='%c' where ",ORDINE_SPEDITO);
	strcpy(szSelectOrdiniCondition,"");
	strcpy(szFiltroSelezione," AND (rostato is NULL OR rostato=' ')");
	gtk_editable_delete_text(GTK_EDITABLE(txf_select),0,-1);
	gtk_text_printf(NULL,txf_select,szSelectOrdiniCmd);

	cb_expression=get_widget(dlg_select,"cb_expression");

	items = g_list_append (items, "Uguale a");
	items = g_list_append (items, "Maggiore di");
	items = g_list_append (items, "Minore di");
	items = g_list_append (items, "Diverso da");

	gtk_combo_set_popdown_strings (GTK_COMBO (cb_expression), items);

	g_list_free(items); items=NULL;

	cb_field=get_widget(dlg_select,"cb_field");

	PGRes=PGExecSQL(FALSE,"select * from ric_ord;");
	nFields=PQnfields(PGRes);
	for(nFieldIndex=0;nFieldIndex<nFields;nFieldIndex++){
		items = g_list_append (items, PQfname(PGRes,nFieldIndex));
	}
	gtk_combo_set_popdown_strings (GTK_COMBO (cb_field), items);

	g_list_free(items); items=NULL;

	gtk_window_set_focus (GTK_WINDOW (dlg_select), get_widget(dlg_select,"pb_select_all"));
	gtk_widget_set_sensitive(get_widget(dlg_select,"pb_lancio"),FALSE);
	gtk_widget_set_sensitive(get_widget(dlg_select,"pb_lista_articoli"),FALSE);
	gtk_widget_set_sensitive(get_widget(dlg_select,"pb_and"),FALSE);
	gtk_widget_set_sensitive(get_widget(dlg_select,"pb_or"),FALSE);
	gtk_widget_show(dlg_select);
	gtk_window_set_transient_for(GTK_WINDOW(dlg_select),GTK_WINDOW(main_window));
}


void on_main_pb_config_clicked(GtkButton * button, gpointer user_data)
{
	EditConfigurazione();
}


void on_main_pb_help_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *dlg_about = create_dlg_about();
	gtk_window_set_focus (GTK_WINDOW (dlg_about), get_widget(dlg_about,"pb_ok"));
	gtk_widget_show(dlg_about);
	gtk_window_set_transient_for(GTK_WINDOW(dlg_about),GTK_WINDOW(main_window));
}

void on_main_pb_edit_clicked (GtkButton *button, gpointer user_data)
{
	char *pszOrdineKey;

	dlg_edit_ordine = create_dlg_edit_ordine();

	gtk_clist_get_text(GTK_CLIST(lst_ordini),nLstOrdiniRowSelected,Cfg.nOrdiniKeyField,&pszOrdineKey);
	strcpy(szOrdineInEdit,pszOrdineKey);
	gtk_window_set_focus (GTK_WINDOW (dlg_edit_ordine), get_widget(dlg_edit_ordine,"pb_ok"));
	gtk_widget_show(dlg_edit_ordine);
	gtk_window_set_transient_for(GTK_WINDOW(dlg_edit_ordine),GTK_WINDOW(main_window));

	/* visualizzo in dlg_edit_ordine dati di produzione */
	bStorico=FALSE;
	EditOrdine(pszOrdineKey);
}

void on_main_pb_exit_clicked(GtkButton * button, gpointer user_data)
{
	dlg_exit = create_dlg_exit();
	gtk_window_set_focus (GTK_WINDOW (dlg_exit), get_widget(dlg_exit,"pb_ok"));
	gtk_widget_show(dlg_exit);
	gtk_window_set_transient_for(GTK_WINDOW(dlg_exit),GTK_WINDOW(main_window));
}

/*
* ------------------------------------------------------------
*                   FINE TOOLBOX CALLBACKS
* ------------------------------------------------------------
*                   INIZIO DIALOGS CALLBACKS
* ------------------------------------------------------------
*/


void on_dlg_exit_pb_ok_clicked(GtkButton * button, gpointer user_data)
{
	ExitApp();

}


void on_dlg_exit_pb_cancel_clicked(GtkButton * button, gpointer user_data)
{
	gtk_widget_destroy(dlg_exit);
	dlg_exit=0;

}


void on_dlg_about_pb_ok_clicked(gpointer user_data,GtkButton *button)
{
	gtk_widget_destroy(GTK_WIDGET(user_data));
}

void on_dlg_about_pb_cancel_clicked(gpointer user_data,GtkButton *button)
{
	gtk_widget_destroy(GTK_WIDGET(user_data));
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


void on_dlg_edit_ordine_pb_cancel_clicked(GtkButton * button, gpointer user_data)
{
	gtk_widget_destroy(dlg_edit_ordine);
	dlg_edit_ordine=0;
}


void on_dlg_edit_ordine_pb_next_clicked(GtkButton * button, gpointer user_data)
{
	int nOrdineIndex;
	char *pszOrdine;

	nOrdineIndex=GetOrdineIndex(szOrdineInEdit);

	if(nOrdineIndex<(GTK_CLIST(lst_ordini)->rows)-1){
		gtk_clist_get_text(GTK_CLIST(lst_ordini),nOrdineIndex+1,Cfg.nOrdiniKeyField,&pszOrdine);
		strcpy(szOrdineInEdit,pszOrdine);
		EditOrdine(szOrdineInEdit);
		gtk_widget_set_sensitive(get_widget(dlg_edit_ordine,"pb_next"),TRUE);
		gtk_widget_set_sensitive(get_widget(dlg_edit_ordine,"pb_prev"),TRUE);
		gtk_update();
	} else {
		gtk_widget_set_sensitive(get_widget(dlg_edit_ordine,"pb_next"),FALSE);
	}
}


void on_dlg_edit_ordine_pb_prev_clicked(GtkButton * button, gpointer user_data)
{
	int nOrdineIndex;
	char *pszOrdine;

	nOrdineIndex=GetOrdineIndex(szOrdineInEdit);

	if(nOrdineIndex>0){
		gtk_clist_get_text(GTK_CLIST(lst_ordini),nOrdineIndex-1,Cfg.nOrdiniKeyField,&pszOrdine);
		strcpy(szOrdineInEdit,pszOrdine);
		EditOrdine(szOrdineInEdit);
		gtk_widget_set_sensitive(get_widget(dlg_edit_ordine,"pb_prev"),TRUE);
		gtk_widget_set_sensitive(get_widget(dlg_edit_ordine,"pb_next"),TRUE);
		gtk_update();
	} else {
		gtk_widget_set_sensitive(get_widget(dlg_edit_ordine,"pb_prev"),FALSE);
	}
}


void on_dlg_config_pb_ok_clicked(gpointer user_data,GtkButton *button)
{
	gtk_widget_destroy(GTK_WIDGET(user_data));
}


void on_dlg_config_pb_apply_clicked(GtkButton * button, gpointer user_data)
{
	ApplyConfigurazione();
}


void on_dlg_config_pb_cancel_clicked(gpointer user_data,GtkButton *button)
{
	gtk_widget_destroy(GTK_WIDGET(user_data));
}


void on_dlg_edit_collo_pb_ok_clicked(GtkButton * button, gpointer user_data)
{
	gtk_widget_destroy(dlg_edit_collo);
	dlg_edit_collo=0;
}


void on_dlg_edit_collo_pb_cancel_clicked(GtkButton * button, gpointer user_data)
{
	gtk_widget_destroy(dlg_edit_collo);
	dlg_edit_collo=0;
}


void on_dlg_edit_collo_pb_next_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *lst_edit_ordine;
	int nCollo;
	char *pszText;
	char szOrdineKey[128];

	if((lst_edit_ordine=get_widget(dlg_edit_ordine,"lst_edit_ordine"))){
		gtk_clist_get_text(GTK_CLIST(lst_edit_ordine),nColloInEdit+1,0,&pszText);
		if(pszText){
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
}

void on_dlg_edit_collo_pb_prev_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *lst_edit_ordine;
	int nCollo;
	char *pszText;
	char szOrdineKey[128];

	if((lst_edit_ordine=get_widget(dlg_edit_ordine,"lst_edit_ordine"))){
		gtk_clist_get_text(GTK_CLIST(lst_edit_ordine),nColloInEdit-1,0,&pszText);
		if(pszText){
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
}

void on_dlg_distribuzione_pb_ok_clicked     (GtkButton *button, gpointer user_data)
{
	gtk_widget_destroy(dlg_distribuzione);
	dlg_distribuzione=0;
}


void on_dlg_distribuzione_pb_elimina_elaborazione (GtkButton *button, gpointer user_data)
{
	GList *lista;

	lista=g_list_first(GTK_CLIST(get_widget(dlg_distribuzione,"clist_distribuzione"))->selection);

	if(lista==NULL){
		/*
		* nessuna elaborazione della lista e' stata selezionata
		*/
		dlg_message=create_dlg_message();

		gtk_label_printf (get_widget(dlg_message,"lb_msg"),"NESSUNA ELABORAZIONE SELEZIONATA !");
		gtk_window_set_title (GTK_WINDOW (dlg_message), "Elimina Elaborazione");
		gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
		gtk_signal_connect (GTK_OBJECT (get_widget(dlg_message,"pb_ok")), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_pb_ok_clicked), dlg_message);
		gtk_widget_show(dlg_message);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(dlg_distribuzione));

	} else {
		int nRowIndex;
		int nOrdiniElab;
		char *pszNumeroElab;
		char szNumeroElab[64];
		PGresult *PGRes;

		nRowIndex=GPOINTER_TO_INT(lista->data);
		gtk_clist_get_text(GTK_CLIST(get_widget(dlg_distribuzione,"clist_distribuzione")),nRowIndex,0,&pszNumeroElab);
		strcpy(szNumeroElab,pszNumeroElab);

		/*
		* Controllo che l'elaborazione abbia al massimo ordini analizzati
		*/
		PGRes=PGExecSQL(Cfg.nDebugVersion,"select count(ordprog) from ord_prod where opnelab='%s' and opstato not in (' ','A');",szNumeroElab);
		nOrdiniElab=atoi(PQgetvalue(PGRes,0,0));
		PQclear(PGRes);
		if(nOrdiniElab!=0){
			/*
			* l'elaborazione non puo' essere eliminata in quanto contiene ordini gia' elaborati
			*/
			dlg_message=create_dlg_message();

			gtk_label_printf(get_widget(dlg_message,"lb_msg"),"L'elaborazione non puo' essere eliminata in quanto contiene ordini gia' prelevati");
			gtk_window_set_title (GTK_WINDOW (dlg_message), "Elimina Elaborazione");
			gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
			gtk_signal_connect (GTK_OBJECT(get_widget(dlg_message,"pb_ok")), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_pb_ok_clicked), dlg_message);
			gtk_widget_show(dlg_message);
			gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(dlg_distribuzione));

		} else {
			/*
			* chiedo conferma prima di cancellare l'elaborazione
			*/
			dlg_message=create_dlg_message();

			gtk_label_printf(get_widget(dlg_message,"lb_msg"),"Premere ok per confermare l'eliminazione dell'elaborazione %s",szNumeroElab);
			gtk_window_set_title (GTK_WINDOW (dlg_message), "Elimina Elaborazione");
			gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
			gtk_signal_connect (GTK_OBJECT(get_widget(dlg_message,"pb_ok")), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_elimina_elaborazione_clicked), dlg_message);
			gtk_widget_show(dlg_message);
			gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(dlg_distribuzione));
		}
	}
}


void on_dlg_message_elimina_elaborazione_clicked (GtkButton *button, gpointer user_data)
{
	PGresult *PGResOrdini;
	PGresult *PGRes;
	int nOrdini;
	int nIndex;
	int nRowIndex;
	char szElab[64];
	char *pszNumeroElab;
	char szCodiceOrdine[64];
	GList *lista;

	lista=g_list_first(GTK_CLIST(get_widget(dlg_distribuzione,"clist_distribuzione"))->selection);
	nRowIndex=GPOINTER_TO_INT(lista->data);
	gtk_clist_get_text(GTK_CLIST(get_widget(dlg_distribuzione,"clist_distribuzione")),nRowIndex,0,&pszNumeroElab);
	strcpy(szElab,pszNumeroElab);
	g_list_free(lista);
	
	gtk_widget_destroy(user_data);

	PGResOrdini=PGExecSQL(Cfg.nDebugVersion,"select ordprog from ric_ord where ronelab='%s';",szElab);
	nOrdini=PQntuples(PGResOrdini);
	/* inizio la cancellazione dei dati dagli archivi */
	for(nIndex=0;nIndex<nOrdini;nIndex++){
		strcpy(szCodiceOrdine,PQgetvalue(PGResOrdini,nIndex,0));
		/* ric_art */
		PGRes=PGExecSQL(Cfg.nDebugVersion,"delete from ric_art where ordprog='%s';",szCodiceOrdine);
		PQclear(PGRes);
		/* rig_prod */
		PGRes=PGExecSQL(Cfg.nDebugVersion,"delete from rig_prod where ordprog='%s';",szCodiceOrdine);
		PQclear(PGRes);
	}
	/* col_prod */
	PGRes=PGExecSQL(Cfg.nDebugVersion,"delete from col_prod where opnelab='%s';",szElab);
	PQclear(PGRes);
	/* ord_prod */
	PGRes=PGExecSQL(Cfg.nDebugVersion,"delete from ord_prod where opnelab='%s';",szElab);
	PQclear(PGRes);
	/* ric_ord */
	PGRes=PGExecSQL(Cfg.nDebugVersion,"delete from ric_ord where ronelab='%s';",szElab);
	PQclear(PGRes);

#ifdef TRACE
	trace_out_vstr_date(1,"Eliminata elaborazione %s [%d ordini]",user_data,nIndex+1);
#endif
	
	RefreshListaOrdini(sw_list,NULL);
	ListaDistribuzione();

	PQclear(PGResOrdini);
}

void on_dlg_distribuzione_pb_cancel_clicked (GtkButton *button, gpointer user_data)
{
	gtk_widget_destroy(dlg_distribuzione);
	dlg_distribuzione=0;
}

void on_dlg_distribuzione_pb_lancia_clicked (GtkButton *button, gpointer user_data)
{
	GList *lista;

	lista=g_list_first(GTK_CLIST(get_widget(dlg_distribuzione,"clist_distribuzione"))->selection);

	if(lista==NULL){
		/*
		* nessuna elaborazione della lista e' stata selezionata
		*/
		GtkWidget *lb_msg;
		GtkWidget *pb_ok;

		dlg_message=create_dlg_message();

		lb_msg=get_widget(dlg_message,"lb_msg");
		pb_ok=get_widget(dlg_message,"pb_ok");
		gtk_label_printf(lb_msg,"NESSUNA ELABORAZIONE SELEZIONATA !");
		gtk_window_set_title (GTK_WINDOW (dlg_message), "Lancio Elaborazioni");
		gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
		gtk_signal_connect (GTK_OBJECT (pb_ok), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_pb_ok_clicked), dlg_message);
		gtk_widget_show(dlg_message);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));

	} else {
		char szSqlCmd[1024];
		char *pszNumeroElab;
		char *pPtr;
		int nRowIndex;
		GtkWidget *lst;
		PGresult *PGRes = NULL;
		int nOrdiniTotali;

		sprintf(szSqlCmd,"select rostato as \"S\",roflpdf as \"D\",ronelab as \"ELABORAZIONE\",ordprog as \"CODICE\",rodscli as \"RAGIONE SOCIALE\",roprcli as \"PRV\",\
											ronmcll as \"COLLI\",ronmrgh as \"RIGHE\",ronmcpe as \"COPIE\",ropspre::float4/1000 as \"PESO PRE.(Kg)\" from ric_ord where ronelab in (");
		do {
			nRowIndex=GPOINTER_TO_INT(lista->data);
			gtk_clist_get_text(GTK_CLIST(get_widget(dlg_distribuzione,"clist_distribuzione")),nRowIndex,0,&pszNumeroElab);
			strcat(szSqlCmd,"'");
			strcat(szSqlCmd,pszNumeroElab);
			strcat(szSqlCmd,"', ");
		} while((lista=g_list_next(lista)));
		pPtr=strrchr(szSqlCmd,',');
		*pPtr='\0';
		/*
		 * Richiesta Fabio:
		 * Gli ordini per i quali non ho il pdf non vanno in stato L ma rimangono in Z
		 * quindi non devo fare altro che toglierli da sta lista?
		 */
		//strcat(szSqlCmd,") and (rostato='S' or rostato='L' or rostato='Z' or rostato=' ' or rostato is NULL) order by ordprog;");
		strcat(szSqlCmd,") and ((rostato=' ' or rostato is NULL) or (rostato='Z' and roflpdf!=' ')) order by ordprog;");

		PGRes=PGExecSQL(FALSE,szSqlCmd);
		nOrdiniTotali=PQntuples(PGRes);
		PQclear(PGRes);

		g_list_free(lista);
		
		dlg_lancio_distribuzioni=create_dlg_lancio_distribuzioni();
	  gtk_label_printf(get_widget(dlg_lancio_distribuzioni,"lb_message"),"PREMERE OK PER CONFERMARE IL LANCIO DEL SEGUENTE GRUPPO DI ORDINI [%d]",nOrdiniTotali);
		CreateListFromSelect(dlg_lancio_distribuzioni,get_widget(dlg_lancio_distribuzioni,"sw_lancio_distribuzioni"),&lst,"lst_lancio_distribuzioni",GTK_SELECTION_SINGLE,szSqlCmd,NULL,0);
		gtk_widget_show(dlg_lancio_distribuzioni);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_lancio_distribuzioni),GTK_WINDOW(main_window));
		gtk_window_set_transient_for(GTK_WINDOW(dlg_lancio_distribuzioni),GTK_WINDOW(dlg_distribuzione));
		gtk_signal_connect (GTK_OBJECT (get_widget(dlg_lancio_distribuzioni,"pb_ok")), "clicked", GTK_SIGNAL_FUNC (do_lancio_elaborazione), dlg_lancio_distribuzioni);
	}
}


void on_dlg_distribuzione_pb_lancia_senza_documento_clicked (GtkButton       *button, gpointer         user_data)
{
	GList *lista;

	lista=g_list_first(GTK_CLIST(get_widget(dlg_distribuzione,"clist_distribuzione"))->selection);

	if(lista==NULL){
		/*
		* nessuna elaborazione della lista e' stata selezionata
		*/
		GtkWidget *lb_msg;
		GtkWidget *pb_ok;

		dlg_message=create_dlg_message();

		lb_msg=get_widget(dlg_message,"lb_msg");
		pb_ok=get_widget(dlg_message,"pb_ok");
		gtk_label_printf(lb_msg,"NESSUNA ELABORAZIONE SELEZIONATA !");
		gtk_window_set_title (GTK_WINDOW (dlg_message), "Lancio Elaborazioni");
		gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
		gtk_signal_connect (GTK_OBJECT (pb_ok), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_pb_ok_clicked), dlg_message);
		gtk_widget_show(dlg_message);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));

	} else {
		char szSqlCmd[1024];
		char *pszNumeroElab;
		char *pPtr;
		int nRowIndex;
		GtkWidget *lst;
		PGresult *PGRes = NULL;
		int nOrdiniTotali;

		sprintf(szSqlCmd,"select rostato as \"S\",roflpdf as \"D\",ronelab as \"ELABORAZIONE\",ordprog as \"CODICE\",rodscli as \"RAGIONE SOCIALE\",roprcli as \"PRV\",\
											ronmcll as \"COLLI\",ronmrgh as \"RIGHE\",ronmcpe as \"COPIE\",ropspre::float4/1000 as \"PESO PRE.(Kg)\" from ric_ord where ronelab in (");
		do {
			nRowIndex=GPOINTER_TO_INT(lista->data);
			gtk_clist_get_text(GTK_CLIST(get_widget(dlg_distribuzione,"clist_distribuzione")),nRowIndex,0,&pszNumeroElab);
			strcat(szSqlCmd,"'");
			strcat(szSqlCmd,pszNumeroElab);
			strcat(szSqlCmd,"', ");
		} while((lista=g_list_next(lista)));
		pPtr=strrchr(szSqlCmd,',');
		*pPtr='\0';
		/*
		 * Richiesta Fabio:
		 * Gli ordini per i quali non ho il pdf non vanno in stato L ma rimangono in Z
		 * quindi non devo fare altro che toglierli da sta lista?
		 */
		//strcat(szSqlCmd,") and (rostato='S' or rostato='L' or rostato='Z' or rostato=' ' or rostato is NULL) order by ordprog;");
		strcat(szSqlCmd,") and ((rostato=' ' or rostato is NULL) or (rostato='Z' and roflpdf!=' ')) order by ordprog;");

		PGRes=PGExecSQL(FALSE,szSqlCmd);
		nOrdiniTotali=PQntuples(PGRes);
		PQclear(PGRes);

		g_list_free(lista);
		
		dlg_lancio_distribuzioni=create_dlg_lancio_distribuzioni();
	  gtk_label_printf(get_widget(dlg_lancio_distribuzioni,"lb_message"),"PREMERE OK PER CONFERMARE IL LANCIO DEL SEGUENTE GRUPPO DI ORDINI [%d] SENZA DOCUMENTO",nOrdiniTotali);
		CreateListFromSelect(dlg_lancio_distribuzioni,get_widget(dlg_lancio_distribuzioni,"sw_lancio_distribuzioni"),&lst,"lst_lancio_distribuzioni",GTK_SELECTION_SINGLE,szSqlCmd,NULL,0);
		gtk_widget_show(dlg_lancio_distribuzioni);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_lancio_distribuzioni),GTK_WINDOW(main_window));
		gtk_window_set_transient_for(GTK_WINDOW(dlg_lancio_distribuzioni),GTK_WINDOW(dlg_distribuzione));
		gtk_signal_connect (GTK_OBJECT (get_widget(dlg_lancio_distribuzioni,"pb_ok")), "clicked", GTK_SIGNAL_FUNC (do_lancio_elaborazione_senza_documento), dlg_lancio_distribuzioni);
	}

}


void on_dlg_distribuzione_pb_anticipa_consuntivo (GtkButton       *button, gpointer         user_data)
{
	GList *lista;

	lista=g_list_first(GTK_CLIST(get_widget(dlg_distribuzione,"clist_distribuzione"))->selection);

	if(lista==NULL){
		/*
		* nessuna elaborazione della lista e' stata selezionata
		*/
		GtkWidget *lb_msg;
		GtkWidget *pb_ok;

		dlg_message=create_dlg_message();

		lb_msg=get_widget(dlg_message,"lb_msg");
		pb_ok=get_widget(dlg_message,"pb_ok");
		gtk_label_printf(lb_msg,"NESSUNA ELABORAZIONE SELEZIONATA !");
		gtk_window_set_title (GTK_WINDOW (dlg_message), "Consuntivazione Elaborazioni");
		gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
		gtk_signal_connect (GTK_OBJECT (pb_ok), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_pb_ok_clicked), dlg_message);
		gtk_widget_show(dlg_message);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));

	} else {
		char szSqlCmd[1024];
		char *pszNumeroElab;
		char *pPtr;
		int nRowIndex;
		GtkWidget *lst;
		PGresult *PGRes = NULL;
		int nOrdiniTotali;
		int nOrdiniLanciati;

		/* 
		* rm 20-01-2004 : conto gli ordini 
		*/
		sprintf(szSqlCmd,"select rostato as \"S\",ronelab as \"ELABORAZIONE\",ordprog as \"CODICE\",rodscli as \"RAGIONE SOCIALE\",roprcli as \"PRV\",\
											ronmcll as \"COLLI\",ronmrgh as \"RIGHE\",ronmcpe as \"COPIE\",ropspre::float4/1000 as \"PESO PRE.(Kg)\" from ric_ord where ronelab in (");
		do {
			nRowIndex=GPOINTER_TO_INT(lista->data);
			gtk_clist_get_text(GTK_CLIST(get_widget(dlg_distribuzione,"clist_distribuzione")),nRowIndex,0,&pszNumeroElab);
			strcat(szSqlCmd,"'");
			strcat(szSqlCmd,pszNumeroElab);
			strcat(szSqlCmd,"', ");
		} while((lista=g_list_next(lista)));
		pPtr=strrchr(szSqlCmd,',');
		*pPtr='\0';
		strcat(szSqlCmd,") order by ordprog;");
		PGRes=PGExecSQL(FALSE,szSqlCmd);
		nOrdiniTotali=PQntuples(PGRes);
		PQclear(PGRes);

		lista=g_list_first(GTK_CLIST(get_widget(dlg_distribuzione,"clist_distribuzione"))->selection);


		sprintf(szSqlCmd,"select rostato as \"S\",ronelab as \"ELABORAZIONE\",ordprog as \"CODICE\",rodscli as \"RAGIONE SOCIALE\",roprcli as \"PRV\",\
											ronmcll as \"COLLI\",ronmrgh as \"RIGHE\",ronmcpe as \"COPIE\",ropspre::float4/1000 as \"PESO PRE.(Kg)\" from ric_ord where ronelab in (");
		do {
			nRowIndex=GPOINTER_TO_INT(lista->data);
			gtk_clist_get_text(GTK_CLIST(get_widget(dlg_distribuzione,"clist_distribuzione")),nRowIndex,0,&pszNumeroElab);
			strcat(szSqlCmd,"'");
			strcat(szSqlCmd,pszNumeroElab);
			strcat(szSqlCmd,"', ");
		} while((lista=g_list_next(lista)));
		pPtr=strrchr(szSqlCmd,',');
		*pPtr='\0';
		strcat(szSqlCmd,") and rostato in (' ','A','R','P','T','E','H','J','Z','L','W','X') order by ordprog;");

		PGRes=PGExecSQL(Cfg.nDebugVersion>1,szSqlCmd);
		nOrdiniLanciati=PQntuples(PGRes);
		PQclear(PGRes);

		g_list_free(lista);

		/* 
		* se il numero di ordini totali corrisponde ai lanciati
		* allora posso consuntivare 
		*/
		if(nOrdiniTotali==nOrdiniLanciati){
			dlg_lancio_distribuzioni=create_dlg_lancio_distribuzioni();
	    gtk_label_printf(get_widget(dlg_lancio_distribuzioni,"lb_message"),"PREMERE OK PER CONFERMARE LA CONSUNTIVAZIONE DEL SEGUENTE GRUPPO DI ORDINI [%d]",nOrdiniTotali);
			CreateListFromSelect(dlg_lancio_distribuzioni,get_widget(dlg_lancio_distribuzioni,"sw_lancio_distribuzioni"),&lst,"lst_lancio_distribuzioni",GTK_SELECTION_SINGLE,szSqlCmd,NULL,0);
			gtk_widget_show(dlg_lancio_distribuzioni);
			gtk_window_set_transient_for(GTK_WINDOW(dlg_lancio_distribuzioni),GTK_WINDOW(main_window));
			gtk_window_set_transient_for(GTK_WINDOW(dlg_lancio_distribuzioni),GTK_WINDOW(dlg_distribuzione));
			gtk_signal_connect (GTK_OBJECT (get_widget(dlg_lancio_distribuzioni,"pb_ok")), "clicked", GTK_SIGNAL_FUNC (do_consuntivo_elaborazione), dlg_lancio_distribuzioni);
		} else {
			/*
			* nessuna elaborazione della lista e' stata selezionata
			*/
			GtkWidget *lb_msg;
			GtkWidget *pb_ok;

			dlg_message=create_dlg_message();

			lb_msg=get_widget(dlg_message,"lb_msg");
			pb_ok=get_widget(dlg_message,"pb_ok");
			gtk_label_printf(lb_msg,"ERRORE IN SELEZIONE ORDINI DA CONSUNTIVARE !");
			gtk_window_set_title (GTK_WINDOW (dlg_message), "Consuntivazione Elaborazioni");
			gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
			gtk_signal_connect (GTK_OBJECT (pb_ok), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_pb_ok_clicked), dlg_message);
			gtk_widget_show(dlg_message);
			gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));
		}
	}

}
void on_dlg_distribuzione_pb_pallet_clicked(gpointer user_data,GtkButton *button)
{
	GList *lista;

	lista=g_list_first(GTK_CLIST(get_widget(dlg_distribuzione,"clist_distribuzione"))->selection);

	if(lista==NULL){
		/*
		* nessuna elaborazione della lista e' stata selezionata
		*/
		GtkWidget *lb_msg;
		GtkWidget *pb_ok;

		dlg_message=create_dlg_message();

		lb_msg=get_widget(dlg_message,"lb_msg");
		pb_ok=get_widget(dlg_message,"pb_ok");
		gtk_label_printf(lb_msg,"NESSUNA ELABORAZIONE SELEZIONATA !");
		gtk_window_set_title (GTK_WINDOW (dlg_message), "Marcatura Elaborazioni");
		gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
		gtk_signal_connect (GTK_OBJECT (pb_ok), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_pb_ok_clicked), dlg_message);
		gtk_widget_show(dlg_message);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));

	} else {
		char szSqlCmd[1024];
		char *pszNumeroElab;
		char *pPtr;
		int nRowIndex;
		GtkWidget *lst;
		PGresult *PGRes = NULL;
		int nOrdiniTotali;
		int nOrdiniLanciati;

		/* 
		* rm 20-01-2004 : conto gli ordini 
		*/
		sprintf(szSqlCmd,"select rostato as \"S\",ronelab as \"ELABORAZIONE\",ordprog as \"CODICE\",rodscli as \"RAGIONE SOCIALE\",roprcli as \"PRV\",\
											ronmcll as \"COLLI\",ronmrgh as \"RIGHE\",ronmcpe as \"COPIE\",ropspre::float4/1000 as \"PESO PRE.(Kg)\" from ric_ord where ronelab in (");
		do {
			nRowIndex=GPOINTER_TO_INT(lista->data);
			gtk_clist_get_text(GTK_CLIST(get_widget(dlg_distribuzione,"clist_distribuzione")),nRowIndex,0,&pszNumeroElab);
			strcat(szSqlCmd,"'");
			strcat(szSqlCmd,pszNumeroElab);
			strcat(szSqlCmd,"', ");
		} while((lista=g_list_next(lista)));
		pPtr=strrchr(szSqlCmd,',');
		*pPtr='\0';
		strcat(szSqlCmd,") order by ordprog;");
		PGRes=PGExecSQL(FALSE,szSqlCmd);
		nOrdiniTotali=PQntuples(PGRes);
		PQclear(PGRes);

		lista=g_list_first(GTK_CLIST(get_widget(dlg_distribuzione,"clist_distribuzione"))->selection);


		sprintf(szSqlCmd,"select rostato as \"S\",ronelab as \"ELABORAZIONE\",ordprog as \"CODICE\",rodscli as \"RAGIONE SOCIALE\",roprcli as \"PRV\",\
											ronmcll as \"COLLI\",ronmrgh as \"RIGHE\",ronmcpe as \"COPIE\",ropspre::float4/1000 as \"PESO PRE.(Kg)\" from ric_ord where ronelab in (");
		do {
			nRowIndex=GPOINTER_TO_INT(lista->data);
			gtk_clist_get_text(GTK_CLIST(get_widget(dlg_distribuzione,"clist_distribuzione")),nRowIndex,0,&pszNumeroElab);
			strcat(szSqlCmd,"'");
			strcat(szSqlCmd,pszNumeroElab);
			strcat(szSqlCmd,"', ");
		} while((lista=g_list_next(lista)));
		pPtr=strrchr(szSqlCmd,',');
		*pPtr='\0';
		strcat(szSqlCmd,") order by ordprog;");

		PGRes=PGExecSQL(Cfg.nDebugVersion>1,szSqlCmd);
		nOrdiniLanciati=PQntuples(PGRes);
		PQclear(PGRes);

		g_list_free(lista);

		/* 
		* se il numero di ordini totali corrisponde ai lanciati
		* allora posso consuntivare 
		*/
		if(nOrdiniTotali==nOrdiniLanciati){
			dlg_lancio_distribuzioni=create_dlg_lancio_distribuzioni();
	    gtk_label_printf(get_widget(dlg_lancio_distribuzioni,"lb_message"),"PREMERE OK PER CONFERMARE LA MARCATURA DEL SEGUENTE GRUPPO DI ORDINI [%d]",nOrdiniTotali);
			CreateListFromSelect(dlg_lancio_distribuzioni,get_widget(dlg_lancio_distribuzioni,"sw_lancio_distribuzioni"),&lst,"lst_lancio_distribuzioni",GTK_SELECTION_SINGLE,szSqlCmd,NULL,0);
			gtk_widget_show(dlg_lancio_distribuzioni);
			gtk_window_set_transient_for(GTK_WINDOW(dlg_lancio_distribuzioni),GTK_WINDOW(main_window));
			gtk_window_set_transient_for(GTK_WINDOW(dlg_lancio_distribuzioni),GTK_WINDOW(dlg_distribuzione));
			gtk_signal_connect (GTK_OBJECT (get_widget(dlg_lancio_distribuzioni,"pb_ok")), "clicked", GTK_SIGNAL_FUNC (do_marca_elaborazione_pallet), dlg_lancio_distribuzioni);
		} else {
			/*
			* nessuna elaborazione della lista e' stata selezionata
			*/
			GtkWidget *lb_msg;
			GtkWidget *pb_ok;

			dlg_message=create_dlg_message();

			lb_msg=get_widget(dlg_message,"lb_msg");
			pb_ok=get_widget(dlg_message,"pb_ok");
			gtk_label_printf(lb_msg,"ERRORE IN SELEZIONE ORDINI DA MARCARE !");
			gtk_window_set_title (GTK_WINDOW (dlg_message), "Marcatura Elaborazioni");
			gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
			gtk_signal_connect (GTK_OBJECT (pb_ok), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_pb_ok_clicked), dlg_message);
			gtk_widget_show(dlg_message);
			gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));
		}
	}

}


void on_dlg_distribuzione_pb_fuori_pallet_clicked(gpointer user_data,GtkButton *button)
{
	GList *lista;

	lista=g_list_first(GTK_CLIST(get_widget(dlg_distribuzione,"clist_distribuzione"))->selection);

	if(lista==NULL){
		/*
		* nessuna elaborazione della lista e' stata selezionata
		*/
		GtkWidget *lb_msg;
		GtkWidget *pb_ok;

		dlg_message=create_dlg_message();

		lb_msg=get_widget(dlg_message,"lb_msg");
		pb_ok=get_widget(dlg_message,"pb_ok");
		gtk_label_printf(lb_msg,"NESSUNA ELABORAZIONE SELEZIONATA !");
		gtk_window_set_title (GTK_WINDOW (dlg_message), "Marcatura Elaborazioni");
		gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
		gtk_signal_connect (GTK_OBJECT (pb_ok), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_pb_ok_clicked), dlg_message);
		gtk_widget_show(dlg_message);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));

	} else {
		char szSqlCmd[1024];
		char *pszNumeroElab;
		char *pPtr;
		int nRowIndex;
		GtkWidget *lst;
		PGresult *PGRes = NULL;
		int nOrdiniTotali;
		int nOrdiniLanciati;

		/* 
		* rm 20-01-2004 : conto gli ordini 
		*/
		sprintf(szSqlCmd,"select rostato as \"S\",ronelab as \"ELABORAZIONE\",ordprog as \"CODICE\",rodscli as \"RAGIONE SOCIALE\",roprcli as \"PRV\",\
											ronmcll as \"COLLI\",ronmrgh as \"RIGHE\",ronmcpe as \"COPIE\",ropspre::float4/1000 as \"PESO PRE.(Kg)\" from ric_ord where ronelab in (");
		do {
			nRowIndex=GPOINTER_TO_INT(lista->data);
			gtk_clist_get_text(GTK_CLIST(get_widget(dlg_distribuzione,"clist_distribuzione")),nRowIndex,0,&pszNumeroElab);
			strcat(szSqlCmd,"'");
			strcat(szSqlCmd,pszNumeroElab);
			strcat(szSqlCmd,"', ");
		} while((lista=g_list_next(lista)));
		pPtr=strrchr(szSqlCmd,',');
		*pPtr='\0';
		strcat(szSqlCmd,") order by ordprog;");
		PGRes=PGExecSQL(FALSE,szSqlCmd);
		nOrdiniTotali=PQntuples(PGRes);
		PQclear(PGRes);

		lista=g_list_first(GTK_CLIST(get_widget(dlg_distribuzione,"clist_distribuzione"))->selection);


		sprintf(szSqlCmd,"select rostato as \"S\",ronelab as \"ELABORAZIONE\",ordprog as \"CODICE\",rodscli as \"RAGIONE SOCIALE\",roprcli as \"PRV\",\
											ronmcll as \"COLLI\",ronmrgh as \"RIGHE\",ronmcpe as \"COPIE\",ropspre::float4/1000 as \"PESO PRE.(Kg)\" from ric_ord where ronelab in (");
		do {
			nRowIndex=GPOINTER_TO_INT(lista->data);
			gtk_clist_get_text(GTK_CLIST(get_widget(dlg_distribuzione,"clist_distribuzione")),nRowIndex,0,&pszNumeroElab);
			strcat(szSqlCmd,"'");
			strcat(szSqlCmd,pszNumeroElab);
			strcat(szSqlCmd,"', ");
		} while((lista=g_list_next(lista)));
		pPtr=strrchr(szSqlCmd,',');
		*pPtr='\0';
		strcat(szSqlCmd,") order by ordprog;");

		PGRes=PGExecSQL(Cfg.nDebugVersion>1,szSqlCmd);
		nOrdiniLanciati=PQntuples(PGRes);
		PQclear(PGRes);

		g_list_free(lista);

		/* 
		* se il numero di ordini totali corrisponde ai lanciati
		* allora posso consuntivare 
		*/
		if(nOrdiniTotali==nOrdiniLanciati){
			dlg_lancio_distribuzioni=create_dlg_lancio_distribuzioni();
	    gtk_label_printf(get_widget(dlg_lancio_distribuzioni,"lb_message"),"PREMERE OK PER CONFERMARE LA MARCATURA DEL SEGUENTE GRUPPO DI ORDINI [%d]",nOrdiniTotali);
			CreateListFromSelect(dlg_lancio_distribuzioni,get_widget(dlg_lancio_distribuzioni,"sw_lancio_distribuzioni"),&lst,"lst_lancio_distribuzioni",GTK_SELECTION_SINGLE,szSqlCmd,NULL,0);
			gtk_widget_show(dlg_lancio_distribuzioni);
			gtk_window_set_transient_for(GTK_WINDOW(dlg_lancio_distribuzioni),GTK_WINDOW(main_window));
			gtk_window_set_transient_for(GTK_WINDOW(dlg_lancio_distribuzioni),GTK_WINDOW(dlg_distribuzione));
			gtk_signal_connect (GTK_OBJECT (get_widget(dlg_lancio_distribuzioni,"pb_ok")), "clicked", GTK_SIGNAL_FUNC (do_marca_elaborazione_fuori_pallet), dlg_lancio_distribuzioni);
		} else {
			/*
			* nessuna elaborazione della lista e' stata selezionata
			*/
			GtkWidget *lb_msg;
			GtkWidget *pb_ok;

			dlg_message=create_dlg_message();

			lb_msg=get_widget(dlg_message,"lb_msg");
			pb_ok=get_widget(dlg_message,"pb_ok");
			gtk_label_printf(lb_msg,"ERRORE IN SELEZIONE ORDINI DA MARCARE !");
			gtk_window_set_title (GTK_WINDOW (dlg_message), "Marcatura Elaborazioni");
			gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
			gtk_signal_connect (GTK_OBJECT (pb_ok), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_pb_ok_clicked), dlg_message);
			gtk_widget_show(dlg_message);
			gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));
		}
	}


}



gboolean on_main_window_destroy_event (GtkWidget *widget, GdkEvent *event, gpointer user_data) 
{
	dlg_exit = create_dlg_exit();
	gtk_window_set_focus (GTK_WINDOW (dlg_exit), get_widget(dlg_exit,"pb_ok"));
	gtk_widget_show(dlg_exit);
	gtk_window_set_transient_for(GTK_WINDOW(dlg_exit),GTK_WINDOW(main_window));
  return TRUE;
}

void on_dlg_expand_pb_ok_clicked (gpointer user_data,GtkButton *button)
{
	char szField[128];
	char szExpression[128];
	char *szValue;
	GtkWidget *cb_field=get_widget(dlg_select,"cb_field");
	GtkWidget *cb_expression=get_widget(dlg_select,"cb_expression");
	GtkWidget *dlg_expand=(GtkWidget *)user_data;
	GtkWidget *lst_expand=get_widget(dlg_expand,"lst_expand");
	GtkWidget *txf_select=get_widget(dlg_select,"txf_select");
	int nRowSelected;
	int nType;
	PGresult *PGRes = NULL;

	strcpy(szField,gtk_entry_get_text (GTK_ENTRY (GTK_COMBO(cb_field)->entry)));
	strcpy(szExpression,gtk_entry_get_text (GTK_ENTRY (GTK_COMBO(cb_expression)->entry)));
	if(!strcmp(szExpression,"Uguale a")){
		strcpy(szExpression," = ");
	} else if(!strcmp(szExpression,"Maggiore di")){
		strcpy(szExpression," > ");
	} else if(!strcmp(szExpression,"Minore di")){
		strcpy(szExpression," < ");
	} else if(!strcmp(szExpression,"Diverso da")){
		strcpy(szExpression," != ");
	}

	/*
	* Ricavo il tipo del campo selezionato
	*/
	PGRes=PGExecSQL(FALSE,"select %s from ric_ord;",szField);
	nType=PQftype(PGRes,0);
	/*
	gtk_text_printf(NULL,txt_msgs,"Tipo : %d",nType);
	*/
	PQclear(PGRes);

	if((GTK_CLIST(lst_expand)->selection)){
		nRowSelected=GPOINTER_TO_INT(GTK_CLIST(lst_expand)->selection->data);
		/*
		* Costruzione Select Ordini da lanciare
		*/
		strcat(szSelectOrdiniCondition,szField);
		strcat(szSelectOrdiniCondition,szExpression);
		strcat(szSelectOrdiniCondition,"'");
		gtk_clist_get_text(GTK_CLIST(lst_expand),nRowSelected,0,&szValue);
		strcat(szSelectOrdiniCondition,szValue);
		strcat(szSelectOrdiniCondition,"'");
	}
	gtk_editable_delete_text(GTK_EDITABLE(txf_select),0,-1);
	gtk_text_printf(NULL,txf_select,"%s%s",szSelectOrdiniCmd,szSelectOrdiniCondition);
	gtk_widget_set_sensitive(get_widget(dlg_select,"pb_and"),TRUE);
	gtk_widget_set_sensitive(get_widget(dlg_select,"pb_or"),TRUE);

	gtk_widget_destroy(dlg_expand);
}


void on_dlg_expand_pb_cancel_clicked (gpointer user_data,GtkButton *button)
{
	gtk_widget_destroy(GTK_WIDGET(user_data));
}


void on_dlg_import_do_ricezione_cedola (GtkButton *button, gpointer user_data)
{
	RicezioneCedola();

	/*
	* st 29-08-01 aggiunto l'aggiornamento delle relazioni in ricezione per
	* ovviare ad un rallentamento del sistema dopo ogni ricezione
	* PGRes=PGExecSQL(Cfg.nDebugVersion,"vacuum ric_art;");
	* PQclear(PGRes);
	* PGRes=PGExecSQL(Cfg.nDebugVersion,"vacuum ric_ord;");
	* PQclear(PGRes);
	* PGRes=PGExecSQL(Cfg.nDebugVersion,"vacuum ubicazioni;");
	* PQclear(PGRes);
	* PGRes=PGExecSQL(Cfg.nDebugVersion,"vacuum articoli;");
	* PQclear(PGRes);
	*/
}


void on_dlg_export_do_spedizione_cedola (GtkButton *button, gpointer user_data)
{
	SpedizioneCedola("a");
}


void on_dlg_import_pb_cancel_clicked (gpointer user_data,GtkButton *button)
{
	gtk_widget_destroy(GTK_WIDGET(user_data));
	RefreshListaOrdini(sw_list,NULL);
}

void on_delete_ricezione_pb_ok_clicked(GtkButton *button, gpointer user_data)
{
	PGresult *PGRes=NULL;

	gtk_widget_destroy(GTK_WIDGET(user_data));

	PGRes=PGExecSQL(FALSE,"BEGIN;"); PQclear(PGRes);
	PGRes=PGExecSQL(FALSE,"delete from ric_ord;"); PQclear(PGRes);
	PGRes=PGExecSQL(FALSE,"delete from ric_art;"); PQclear(PGRes);
	PGRes=PGExecSQL(FALSE,"COMMIT;"); PQclear(PGRes);
	PGRes=PGExecSQL(FALSE,"vacuum analyze ric_art;"); PQclear(PGRes);
	PGRes=PGExecSQL(FALSE,"vacuum analyze ric_ord;"); PQclear(PGRes);

	RefreshListaOrdini(sw_list,NULL);
}

void on_delete_produzione_pb_ok_clicked(GtkButton *button, gpointer user_data)
{
	PGresult *PGRes=NULL;

	gtk_widget_destroy(GTK_WIDGET(user_data));

	PGRes=PGExecSQL(FALSE,"BEGIN;"); PQclear(PGRes);
	PGRes=PGExecSQL(FALSE,"delete from ord_prod;"); PQclear(PGRes);
	PGRes=PGExecSQL(FALSE,"delete from col_prod;"); PQclear(PGRes);
	PGRes=PGExecSQL(FALSE,"delete from rig_prod;"); PQclear(PGRes);
	PGRes=PGExecSQL(FALSE,"COMMIT;"); PQclear(PGRes);
	PGRes=PGExecSQL(FALSE,"vacuum analyze ord_prod;"); PQclear(PGRes);
	PGRes=PGExecSQL(FALSE,"vacuum analyze col_prod;"); PQclear(PGRes);
	PGRes=PGExecSQL(FALSE,"vacuum analyze rig_prod;"); PQclear(PGRes);
	/*
	* in ric_ord resetto il numero di colli degli evetuali ordini in produzione
	*/	
	PGRes=PGExecSQL(FALSE,"update ric_ord set ronmcll=NULL"); PQclear(PGRes);

	RefreshListaOrdini(sw_list,NULL);
}

/*
* void on_dlg_aggiorna_archivi_pb_ok_clicked (GtkButton *button, gpointer user_data)
* Effettua un aggiornamento degli indici del BD
*/
void on_dlg_aggiorna_archivi_pb_ok_clicked (GtkButton *button, gpointer user_data)
{
	GtkWidget *lb_msg;
	GtkWidget *pb_ok;
	GtkWidget *pb_cancel;
	PGresult *PGRes=NULL;

	lb_msg=get_widget(GTK_WIDGET(user_data),"lb_msg");
	pb_ok=get_widget(GTK_WIDGET(user_data),"pb_ok");
	pb_cancel=get_widget(GTK_WIDGET(user_data),"pb_cancel");

	gtk_label_printf(lb_msg,"Aggiornamento DataBase in corso.\n\nAttendere la fine della procedura");
	gtk_text_printf("GREEN",txt_msgs,"%s %s : Aggiornamento Dati\n", GetDate(time((long *)0),szDateBuffer), GetTime(time((long *)0),szTimeBuffer));
	gtk_widget_set_sensitive(get_widget(GTK_WIDGET(user_data),"pb_ok"),FALSE);
	gtk_widget_set_sensitive(get_widget(GTK_WIDGET(user_data),"pb_cancel"),FALSE);
	gtk_update();
#ifdef TRACE
		trace_out_vstr_date(1,"Lanciato aggiornamento indici database");
#endif

	/* rm 14-10-2002 : eliminata per eccessiva lentezza causa tabelle di storico ... */
	/* PGRes=PGExecSQL(FALSE,"VACUUM ANALYZE;"); */

	gtk_label_printf(lb_msg,"Aggiornamento Ordini di produzione in corso"); gtk_update();
	gtk_text_printf("GREEN",txt_msgs,"Ordini di produzione.."); gtk_update();
	PGRes=PGExecSQL(FALSE,"vacuum analyze ord_prod;");
	if (PQresultStatus(PGRes) == PGRES_COMMAND_OK ) {
		gtk_label_printf(lb_msg,"Aggiornamento Ordini di produzione terminato");
		gtk_text_printf("GREEN",txt_msgs,"... Terminato\n");
	} else {
		gtk_label_printf(lb_msg,"Aggiornamento Ordini di produzione fallito\n\nContattare l'assistenza");
		gtk_text_printf("RED",txt_msgs,"... Fallito, contattare l'assistenza\n");
	}
	PQclear(PGRes);
	gtk_update();

	gtk_label_printf(lb_msg,"Aggiornamento Colli di produzione in corso"); gtk_update();
	gtk_text_printf("GREEN",txt_msgs,"Colli di produzione..."); gtk_update();
	PGRes=PGExecSQL(FALSE,"vacuum analyze col_prod;"); 
	if (PQresultStatus(PGRes) == PGRES_COMMAND_OK ) {
		gtk_label_printf(lb_msg,"Aggiornamento Colli di produzione terminato");
		gtk_text_printf("GREEN",txt_msgs,"... Terminato\n");
	} else {
		gtk_label_printf(lb_msg,"Aggiornamento Colli di produzione fallito\n\nContattare l'assistenza");
		gtk_text_printf("RED",txt_msgs,"... Fallito, contattare l'assistenza\n");
	}
	PQclear(PGRes);
	gtk_update();

	gtk_label_printf(lb_msg,"Aggiornamento Righe di produzione in corso"); gtk_update();
	gtk_text_printf("GREEN",txt_msgs,"Righe di produzione..."); gtk_update();
	PGRes=PGExecSQL(FALSE,"vacuum analyze rig_prod;"); 
	if (PQresultStatus(PGRes) == PGRES_COMMAND_OK ) {
		gtk_label_printf(lb_msg,"Aggiornamento Righe di produzione terminato");
		gtk_text_printf("GREEN",txt_msgs,"... Terminato\n");
	} else {
		gtk_label_printf(lb_msg,"Aggiornamento Righe di produzione fallito\n\nContattare l'assistenza");
		gtk_text_printf("RED",txt_msgs,"... Fallito, contattare l'assistenza\n");
	}
	PQclear(PGRes);
	gtk_update();

	gtk_label_printf(lb_msg,"Aggiornamento Ordini Ricevuti in corso"); gtk_update();
	gtk_text_printf("GREEN",txt_msgs,"Ordini Ricevuti......."); gtk_update();
	PGRes=PGExecSQL(FALSE,"vacuum analyze ric_ord;"); 
	if (PQresultStatus(PGRes) == PGRES_COMMAND_OK ) {
		gtk_label_printf(lb_msg,"Aggiornamento Ordini Ricevuti terminato");
		gtk_text_printf("GREEN",txt_msgs,"... Terminato\n");
	} else {
		gtk_label_printf(lb_msg,"Aggiornamento Ordini Ricevuti fallito\n\nContattare l'assistenza");
		gtk_text_printf("RED",txt_msgs,"... Fallito, contattare l'assistenza\n");
	}
	PQclear(PGRes);
	gtk_update();

	gtk_label_printf(lb_msg,"Aggiornamento Righe Ricevute in corso"); gtk_update();
	gtk_text_printf("GREEN",txt_msgs,"Righe Ricevute........"); gtk_update();
	PGRes=PGExecSQL(FALSE,"vacuum analyze ric_art;"); 
	if (PQresultStatus(PGRes) == PGRES_COMMAND_OK ) {
		gtk_label_printf(lb_msg,"Aggiornamento Righe Ricevute terminato");
		gtk_text_printf("GREEN",txt_msgs,"... Terminato\n");
	} else {
		gtk_label_printf(lb_msg,"Aggiornamento Righe Ricevute fallito\n\nContattare l'assistenza");
		gtk_text_printf("RED",txt_msgs,"... Fallito, contattare l'assistenza\n");
	}
	PQclear(PGRes);
	gtk_update();

	gtk_label_printf(lb_msg,"Aggiornamento Ubicazioni in corso"); gtk_update();
	gtk_text_printf("GREEN",txt_msgs,"Ubicazioni............"); gtk_update();
	PGRes=PGExecSQL(FALSE,"vacuum analyze ubicazioni;"); 
	if (PQresultStatus(PGRes) == PGRES_COMMAND_OK ) {
		gtk_label_printf(lb_msg,"Aggiornamento Ubicazioni terminato");
		gtk_text_printf("GREEN",txt_msgs,"... Terminato\n");
	} else {
		gtk_label_printf(lb_msg,"Aggiornamento Ubicazioni fallito\n\nContattare l'assistenza");
		gtk_text_printf("RED",txt_msgs,"... Fallito, contattare l'assistenza\n");
	}
	PQclear(PGRes);
	gtk_update();

	gtk_label_printf(lb_msg,"Aggiornamento Articoli di Magazzino in corso"); gtk_update();
	gtk_text_printf("GREEN",txt_msgs,"Articoli di Magazzino."); gtk_update();
	PGRes=PGExecSQL(FALSE,"vacuum analyze articoli;"); 
	if (PQresultStatus(PGRes) == PGRES_COMMAND_OK ) {
		gtk_label_printf(lb_msg,"Aggiornamento Articoli di Magazzino terminato");
		gtk_text_printf("GREEN",txt_msgs,"... Terminato\n");
	} else {
		gtk_label_printf(lb_msg,"Aggiornamento Articoli di Magazzino fallito\n\nContattare l'assistenza");
		gtk_text_printf("RED",txt_msgs,"... Fallito, contattare l'assistenza\n");
	}
	PQclear(PGRes);
	gtk_update();
	
#ifdef TRACE
	trace_out_vstr_date(1,"Terminato aggiornamento indici database");
#endif
	gtk_text_printf("GREEN",txt_msgs,"%s %s : Terminato.\n", GetDate(time((long *)0),szDateBuffer), GetTime(time((long *)0),szTimeBuffer));

	gtk_widget_set_sensitive(get_widget(GTK_WIDGET(user_data),"pb_cancel"),TRUE);
}

void on_dlg_message_pb_ok_clicked (GtkButton *button, gpointer user_data)
{
	gtk_widget_destroy(GTK_WIDGET(user_data));
	dlg_message=0;
}

void on_dlg_message_pb_cancel_clicked (gpointer user_data, GtkButton *button)
{
	gtk_widget_destroy(GTK_WIDGET(user_data));
}

void on_dlg_message_sock_display_msg_box(GtkButton *button, gpointer user_data)
{
	gtk_widget_destroy(GTK_WIDGET(user_data));
}

void on_dlg_app_error_pb_ok_clicked(GtkButton *button, gpointer user_data)
{
	/* 
	* chiudo la GUI 
	*/
	gtk_main_quit();
}

void on_pbm_ricezione_ordini_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	GtkWidget *lb_msg;
	GtkWidget *pb_ok;
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
	int nValues[2];
	DBSTRUCT db[2];
	FILE *fp;

	char szOrdini[128];
	char szRighe[128];
	int nIndex;

	sprintf(szOrdini,"%s/%s",Cfg.szPathExport,StrTrimAll(Cfg.szImpOrdiniFile));
	sprintf(szRighe,"%s/%s",Cfg.szPathExport,StrTrimAll(Cfg.szImpRigheFile));

	pszFiles[0]=szOrdini;
	pszFiles[1]=szRighe;

	db[0]=tRicOrd;
	db[1]=tRicArt;

	if(FileExists(szOrdini) || FileExists(szRighe)){
		dlg_import_ordini=create_dlg_import_ordini();

		lb_1=get_widget(dlg_import_ordini,"lb_1");
		lb_2=get_widget(dlg_import_ordini,"lb_2");

		pr_testate    = get_widget(dlg_import_ordini,"pr_testate");
		pr_righe      = get_widget(dlg_import_ordini,"pr_righe");
		rl_testate    = get_widget(dlg_import_ordini,"rl_testate");
		rl_righe      = get_widget(dlg_import_ordini,"rl_righe");
		lb_testate    = get_widget(dlg_import_ordini,"lb_testate");
		lb_righe      = get_widget(dlg_import_ordini,"lb_righe");

		lb[0]=lb_testate;
		lb[1]=lb_righe;
		rl[0]=rl_testate;
		rl[1]=rl_righe;
		pr[0]=pr_testate;
		pr[1]=pr_righe;

		nValues[0]=0;
		nValues[1]=0;

		for(nIndex=0; nIndex<2; nIndex++){
			if((fp=fopen(pszFiles[nIndex],"r"))!=NULL){
				char szBuffer[1024];
				while(fgets(szBuffer,sizeof(szBuffer)-1,fp)){
					/*
					* Assegnare qui i campi  (array di nome,valore)
					*/
					if(IsAsciiRecord(szBuffer,&db[nIndex])){
						nValues[nIndex]++;
					}
				}
				fclose(fp);
				gtk_ruler_set_range(GTK_RULER(rl[nIndex]),(gfloat)0,(gfloat)nValues[nIndex],(gfloat)0,(gfloat)nValues[nIndex]);
				gtk_progress_configure(GTK_PROGRESS(pr[nIndex]),(gfloat)0,(gfloat)0,(gfloat)nValues[nIndex]);
				gtk_label_printf(lb[nIndex],"%d",nValues[nIndex]);
				gtk_update();
			} else {
				gtk_label_printf(lb[nIndex],"File [%s] non presente",pszFiles[nIndex]);
			}
		}

		gtk_window_set_transient_for(GTK_WINDOW(dlg_import_ordini),GTK_WINDOW(main_window));
		gtk_label_printf(lb_2,"Premere Ok per iniziare la ricezione");
		gtk_window_set_focus(GTK_WINDOW (dlg_import_ordini), get_widget(dlg_import_ordini,"pb_ok"));
		gtk_widget_show(dlg_import_ordini);

		pb_ok=get_widget(dlg_import_ordini,"pb_ok");
		gtk_signal_connect (GTK_OBJECT (pb_ok), "clicked", GTK_SIGNAL_FUNC (on_dlg_import_ordini_do_ricezione_ordini), dlg_import);

	} else {
		/*
		* Visualizza messaggio di errore
		*/
		dlg_message=create_dlg_message();

		lb_msg=get_widget(dlg_message,"lb_msg");
		pb_ok=get_widget(dlg_message,"pb_ok");

		gtk_label_printf(lb_msg,"Non sono presenti ordini da ricevere");
		gtk_window_set_title (GTK_WINDOW (dlg_message), "Ricezione Dati");

		gtk_widget_show(dlg_message);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));
		/*
		* callback per pressione pulsante
		*/
		gtk_signal_connect (GTK_OBJECT (pb_ok), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_pb_ok_clicked), dlg_message);

	}

}

void on_dlg_import_ordini_do_ricezione_ordini (GtkButton *button, gpointer user_data)
{
	RicezioneOrdini();
}

void on_pbm_ricezione_articoli_activate (GtkMenuItem *menuitem, gpointer user_data) 
{
	GtkWidget *lb_msg;
	GtkWidget *lb_1;
	GtkWidget *lb_2;
	GtkWidget *pr_articoli;
	GtkWidget *rl_articoli;
	GtkWidget *lb_articoli;

	int nNumeroArticoli;
	char szFileArticoli[128];
	DBSTRUCT dbArticoli;
	FILE *fp;

	sprintf(szFileArticoli,"%s/%s",Cfg.szPathExport,StrTrimAll(Cfg.szImpArticoliFile));

	dbArticoli=tArticoli;

	if((fp=fopen(szFileArticoli,"r"))!=NULL){
		char szBuffer[1024];
		nNumeroArticoli=0;
		
		dlg_import_file=create_dlg_import_file();
		lb_1=get_widget(dlg_import_file,"lb_1");
		lb_2=get_widget(dlg_import_file,"lb_2");
		pr_articoli   = get_widget(dlg_import_file,"pr_file");
		rl_articoli   = get_widget(dlg_import_file,"rl_file");
		lb_articoli   = get_widget(dlg_import_file,"lb_file");

		while(fgets(szBuffer,sizeof(szBuffer)-1,fp)){
			if(IsAsciiRecord(szBuffer,&dbArticoli)){
				nNumeroArticoli++;
			}
		}
		fclose(fp);

		gtk_ruler_set_range(GTK_RULER(rl_articoli),(gfloat)0,(gfloat)nNumeroArticoli,(gfloat)0,(gfloat)nNumeroArticoli);
		gtk_progress_configure(GTK_PROGRESS(pr_articoli),(gfloat)0,(gfloat)0,(gfloat)nNumeroArticoli);
		gtk_label_printf(get_widget(dlg_import_file,"lb_file"),"%d Articoli da ricevere",nNumeroArticoli);
		gtk_label_printf(lb_2,"Premere Ok per iniziare la ricezione");
		gtk_window_set_focus(GTK_WINDOW (dlg_import_file), get_widget(dlg_import_file,"pb_ok"));

		gtk_widget_show(dlg_import_file);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_import_file),GTK_WINDOW(main_window));

		gtk_signal_connect (GTK_OBJECT (get_widget(dlg_import_file,"pb_ok")), "clicked", GTK_SIGNAL_FUNC (on_dlg_import_file_do_ricezione_articoli), dlg_import_file);

	} else {
		/*
		* Visualizza messaggio di errore
		*/
		dlg_message=create_dlg_message();

		lb_msg=get_widget(dlg_message,"lb_msg");

		gtk_label_printf(lb_msg,"Non sono presenti Articoli da ricevere");
		gtk_window_set_title (GTK_WINDOW (dlg_message), "Ricezione Articoli");

		gtk_widget_show(dlg_message);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));
		/*
		* callback per pressione pulsante
		*/
		gtk_signal_connect (GTK_OBJECT (get_widget(dlg_message,"pb_ok")), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_pb_ok_clicked), dlg_message);
	}
}

void on_pbm_ricezione_ubicazioni_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	GtkWidget *lb_msg;
	GtkWidget *lb_1;
	GtkWidget *lb_2;
	GtkWidget *pr_ubicazioni;
	GtkWidget *rl_ubicazioni;
	GtkWidget *lb_ubicazioni;

	int nNumeroUbicazioni;
	char szFileUbicazioni[128];
	DBSTRUCT dbUbicazioni;
	FILE *fp;

	sprintf(szFileUbicazioni,"%s/%s",Cfg.szPathExport,StrTrimAll(Cfg.szImpUbicazioniFile));

	dbUbicazioni=tUbicazioni;

	if((fp=fopen(szFileUbicazioni,"r"))!=NULL){
		char szBuffer[1024];
		nNumeroUbicazioni=0;
		
		dlg_import_file=create_dlg_import_file();
		lb_1=get_widget(dlg_import_file,"lb_1");
		lb_2=get_widget(dlg_import_file,"lb_2");
		pr_ubicazioni = get_widget(dlg_import_file,"pr_file");
		rl_ubicazioni = get_widget(dlg_import_file,"rl_file");
		lb_ubicazioni = get_widget(dlg_import_file,"lb_file");

		while(fgets(szBuffer,sizeof(szBuffer)-1,fp)){
			if(IsAsciiRecord(szBuffer,&dbUbicazioni)){
				nNumeroUbicazioni++;
			}
		}
		fclose(fp);

		gtk_ruler_set_range(GTK_RULER(rl_ubicazioni),(gfloat)0,(gfloat)nNumeroUbicazioni,(gfloat)0,(gfloat)nNumeroUbicazioni);
		gtk_progress_configure(GTK_PROGRESS(pr_ubicazioni),(gfloat)0,(gfloat)0,(gfloat)nNumeroUbicazioni);
		gtk_label_printf(get_widget(dlg_import_file,"lb_file"),"%d Ubicazioni da ricevere",nNumeroUbicazioni);
		gtk_label_printf(lb_2,"Premere Ok per iniziare la ricezione");
		gtk_window_set_focus(GTK_WINDOW (dlg_import_file), get_widget(dlg_import_file,"pb_ok"));

		gtk_widget_show(dlg_import_file);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_import_file),GTK_WINDOW(main_window));

		gtk_signal_connect (GTK_OBJECT (get_widget(dlg_import_file,"pb_ok")), "clicked", GTK_SIGNAL_FUNC (on_dlg_import_file_do_ricezione_ubicazioni), dlg_import_file);

	} else {
		/*
		* Visualizza messaggio di errore
		*/
		dlg_message=create_dlg_message();

		lb_msg=get_widget(dlg_message,"lb_msg");

		gtk_label_printf(lb_msg,"Non sono presenti Ubicazioni da ricevere");
		gtk_window_set_title (GTK_WINDOW (dlg_message), "Ricezione Ubicazioni");

		gtk_widget_show(dlg_message);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));
		/*
		* callback per pressione pulsante
		*/
		gtk_signal_connect (GTK_OBJECT (get_widget(dlg_message,"pb_ok")), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_pb_ok_clicked), dlg_message);
	}
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

/*
* void on_pbm_ricezione_tutto_activate(GtkMenuItem * menuitem, gpointer user_data)
*/
void on_pbm_ricezione_tutto_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	GtkWidget *dlg_message;
	GtkWidget *lb_msg;
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

	GtkWidget *pr[4];
	GtkWidget *rl[4];
	GtkWidget *lb[4];
	char *pszFiles[4];
	char *pszTitoli[4];
	int nValues[4];
	DBSTRUCT db[4];
	FILE *fp;

	char szOrdiniTitolo[128];
	char szRigheTitolo[128];
	char szArticoliTitolo[128];
	char szUbicazioniTitolo[128];

	char szOrdini[128];
	char szRighe[128];
	char szArticoli[128];
	char szUbicazioni[128];

	char szOrdiniBertello[128];
	char szColliBertello[128];
	char szRigheBertello[128];

	int nIndex;

	sprintf(szOrdiniTitolo,"Ordini");
	sprintf(szRigheTitolo,"Righe");
	sprintf(szArticoliTitolo,"Anagrafica Articoli");
	sprintf(szUbicazioniTitolo,"Mappa Ubicazioni");

	sprintf(szOrdini,"%s/%s",Cfg.szPathExport,StrTrimAll(Cfg.szImpOrdiniFile));
	sprintf(szRighe,"%s/%s",Cfg.szPathExport,StrTrimAll(Cfg.szImpRigheFile));
	sprintf(szArticoli,"%s/%s",Cfg.szPathExport,StrTrimAll(Cfg.szImpArticoliFile));
	sprintf(szUbicazioni,"%s/%s",Cfg.szPathExport,StrTrimAll(Cfg.szImpUbicazioniFile));

	sprintf(szOrdiniBertello,   "%s/%s", Cfg.szPathExport, StrTrimAll(Cfg.szImpOrdiniBertello));
	sprintf(szColliBertello,    "%s/%s", Cfg.szPathExport, StrTrimAll(Cfg.szImpColliBertello));
	sprintf(szRigheBertello,    "%s/%s", Cfg.szPathExport, StrTrimAll(Cfg.szImpRigheBertello));

	pszTitoli[0]=szOrdiniTitolo;
	pszTitoli[1]=szRigheTitolo;
	pszTitoli[2]=szArticoliTitolo;
	pszTitoli[3]=szUbicazioniTitolo;

	pszFiles[0]=szOrdini;
	pszFiles[1]=szRighe;
	pszFiles[2]=szArticoli;
	pszFiles[3]=szUbicazioni;

	db[0]=tRicOrd;
	db[1]=tRicArt;
	db[2]=tArticoli;
	db[3]=tUbicazioni;

	if(FileExists(szOrdini) || FileExists(szRighe) || FileExists(szArticoli) || FileExists(szUbicazioni) ||
     FileExists(szOrdiniBertello) || FileExists(szColliBertello) || FileExists(szRigheBertello)){
		dlg_import=create_dlg_import();

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

		lb[0]=lb_testate;
		lb[1]=lb_righe;
		lb[2]=lb_articoli;
		lb[3]=lb_ubicazioni;

		rl[0]=rl_testate;
		rl[1]=rl_righe;
		rl[2]=rl_articoli;
		rl[3]=rl_ubicazioni;

		pr[0]=pr_testate;
		pr[1]=pr_righe;
		pr[2]=pr_articoli;
		pr[3]=pr_ubicazioni;

		nValues[0]=0;
		nValues[1]=0;
		nValues[2]=0;
		nValues[3]=0;


		for(nIndex=0; nIndex<4; nIndex++){
			if((fp=fopen(pszFiles[nIndex],"r"))!=NULL){
				char szBuffer[1024];
				while(fgets(szBuffer,sizeof(szBuffer)-1,fp)){
					/*
					* Assegnare qui i campi  (array di nome,valore)
					*/
					if(IsAsciiRecord(szBuffer,&db[nIndex])){
						nValues[nIndex]++;
					}
				}
				fclose(fp);
				gtk_ruler_set_range(GTK_RULER(rl[nIndex]),(gfloat)0,(gfloat)nValues[nIndex],(gfloat)0,(gfloat)nValues[nIndex]);
				gtk_progress_configure(GTK_PROGRESS(pr[nIndex]),(gfloat)0,(gfloat)0,(gfloat)nValues[nIndex]);
				gtk_label_printf(lb[nIndex],"%s : [%d] records",pszTitoli[nIndex],nValues[nIndex]);
			} else {
				gtk_label_printf(lb[nIndex],"File [%s] non presente",pszFiles[nIndex]);
			}
		}

		gtk_window_set_transient_for(GTK_WINDOW(dlg_import),GTK_WINDOW(main_window));
		gtk_label_printf(lb_2,"Premere Ok per iniziare la ricezione");
		gtk_window_set_focus(GTK_WINDOW (dlg_import), get_widget(dlg_import,"pb_ok"));
		gtk_signal_connect (GTK_OBJECT (get_widget(dlg_import,"pb_ok")), "clicked", GTK_SIGNAL_FUNC (on_dlg_import_do_ricezione_cedola), dlg_import);
		gtk_widget_show(dlg_import);

	} else {
		/*
		* Visualizza messaggio di errore
		*/
		dlg_message=create_dlg_message();

		lb_msg=get_widget(dlg_message,"lb_msg");
		
		gtk_label_printf(lb_msg,"Non sono presenti dati da ricevere");
		gtk_window_set_title (GTK_WINDOW (dlg_message), "Ricezione Dati");

		gtk_widget_show(dlg_message);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));
		/*
		* callback per pressione pulsante
		*/
		gtk_signal_connect (GTK_OBJECT (get_widget(dlg_message,"pb_ok")), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_pb_ok_clicked), dlg_message);

	}
}




void on_dlg_import_file_pb_cancel_clicked (gpointer user_data,GtkButton *button)
{
	GtkWidget *sw_list=get_widget(main_window,"sw_list");
	gtk_widget_destroy(GTK_WIDGET(user_data));
	RefreshListaOrdini(sw_list,NULL);
}


void on_dlg_import_file_do_ricezione_articoli (GtkButton *button, gpointer user_data)
{
	RicezioneArticoli();
}


void on_dlg_import_file_do_ricezione_ubicazioni (GtkButton *button, gpointer user_data)
{
	RicezioneUbicazioni();
}


void on_dlg_import_ordini_pb_cancel_clicked (gpointer user_data,GtkButton *button)
{
	GtkWidget *sw_list=get_widget(main_window,"sw_list");
	gtk_widget_destroy(GTK_WIDGET(user_data));
	RefreshListaOrdini(sw_list,NULL);
}



void on_dlg_export_pb_cancel_clicked (gpointer user_data,GtkButton *button)
{
	gtk_widget_destroy(GTK_WIDGET(user_data));
}


void on_pbm_setta_stato_ordine_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	GList *lista;

	lista=g_list_first(GTK_CLIST(lst_ordini)->selection);

	if(lista==NULL){
		/*
		* nessun ordine della lista e' selezionato
		*/
		GtkWidget *lb_msg;
		GtkWidget *pb_ok;

		dlg_message=create_dlg_message();

		lb_msg=get_widget(dlg_message,"lb_msg");
		pb_ok=get_widget(dlg_message,"pb_ok");
		gtk_label_printf(lb_msg,"Nessun ordine selezionato");
		gtk_window_set_title (GTK_WINDOW (dlg_message), "Set Stato Ordini");
		gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
		gtk_signal_connect (GTK_OBJECT (pb_ok), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_pb_ok_clicked), dlg_message);
		gtk_widget_show(dlg_message);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));

	} else {
		dlg_set_ordini=create_dlg_set_ordini();
		gtk_window_set_focus (GTK_WINDOW (dlg_set_ordini), get_widget(dlg_set_ordini,"pb_ok"));
		gtk_widget_show(dlg_set_ordini);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_set_ordini),GTK_WINDOW(main_window));
	}
}

void on_dlg_set_ordini_pb_ok_clicked (gpointer user_data,GtkButton *button)
{
	GList *lista;
	char *pszOrdineKey;
	char szOrdineKey[128];
	int nRowIndex;
	int nOrdini=0;
	char szSQLCmd[512];
	PGresult *PGRes;
	char cStatoOrdini;  /* stato ordini selezionato da dlg_set_ordini */

	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(get_widget(dlg_set_ordini,"rb_ricevuto")))){
		cStatoOrdini=ORDINE_RICEVUTO;     
//	} else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(get_widget(dlg_set_ordini,"rb_prelevato")))){
//		cStatoOrdini=ORDINE_PRELEVATO;     
//	} else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(get_widget(dlg_set_ordini,"rb_spedito")))){
//		cStatoOrdini=ORDINE_SPEDITO;     
	} else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(get_widget(dlg_set_ordini,"rb_evaso")))){
		cStatoOrdini=ORDINE_EVASO;     
	} else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(get_widget(dlg_set_ordini,"rb_incompleto")))){
		//cStatoOrdini=ORDINE_INCOMPLETO_TERMINATO;
		cStatoOrdini=ORDINE_EVASO;
//	} else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(get_widget(dlg_set_ordini,"rb_elaborato")))){
//		cStatoOrdini=ORDINE_ELABORATO;     
//	} else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(get_widget(dlg_set_ordini,"rb_stampato_rac")))){
//		cStatoOrdini=ORDINE_STAMPATA_RAC;     
//	} else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(get_widget(dlg_set_ordini,"rb_in_prelievo")))){
//		cStatoOrdini=ORDINE_IN_PRELIEVO ;     
	} else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(get_widget(dlg_set_ordini,"rb_spedito_host")))){
		cStatoOrdini=ORDINE_SPEDITO_HOST;     
	} 

	lista=g_list_first(GTK_CLIST(lst_ordini)->selection);

	do {
		nRowIndex=GPOINTER_TO_INT(lista->data);
		gtk_clist_get_text(GTK_CLIST(lst_ordini),nRowIndex,Cfg.nOrdiniKeyField,&pszOrdineKey);
		strcpy(szOrdineKey,pszOrdineKey);
		
		/*
		* Setto lo stato degli ordini(ricevuti ed in produzione), delle righe e dei colli
		*/
		/*
		* Se si tratta di una evasione setto la data, l'ora e il peso reale
		*/
		if(cStatoOrdini==ORDINE_EVASO || cStatoOrdini==ORDINE_SPEDITO_HOST){
			sprintf(szSQLCmd,"update ord_prod set opstato='%c',oppsrea=oppspre,opdteva='%s',optmeva='%s' where ordprog='%s';",
				cStatoOrdini,
				GetDateYYYYMMDD(time((long *)0),szDateBuffer),
				GetTimeHHMMSS(time((long *)0),szTimeBuffer),
				szOrdineKey);
		} else {
			sprintf(szSQLCmd,"update ord_prod set opstato='%c' where ordprog='%s';",cStatoOrdini,szOrdineKey);
		}
		PGRes=PGExecSQL(Cfg.nDebugVersion,szSQLCmd);
		PQclear(PGRes);

		/*
		* Se si tratta di una evasione setto la data, l'ora e il peso reale
		*/
		if(cStatoOrdini==ORDINE_EVASO || cStatoOrdini==ORDINE_SPEDITO_HOST){
			sprintf(szSQLCmd,"update col_prod set cpstato='%c',cppsrea=cppspre,cpdteva='%s',cptmeva='%s' where ordprog='%s';",
				cStatoOrdini,
				GetDateYYYYMMDD(time((long *)0),szDateBuffer),
				GetTimeHHMMSS(time((long *)0),szTimeBuffer),
				szOrdineKey);
		} else {
			sprintf(szSQLCmd,"update col_prod set cpstato='%c' where ordprog='%s';",cStatoOrdini,szOrdineKey);
		}
		PGRes=PGExecSQL(FALSE,szSQLCmd);
		PQclear(PGRes);

		sprintf(szSQLCmd,"update rig_prod set rpstato='%c' where ordprog='%s';",cStatoOrdini,szOrdineKey);
		PGRes=PGExecSQL(FALSE,szSQLCmd);
		PQclear(PGRes);

		/*
		* Se si tratta di una evasione setto la data, l'ora e il peso reale
		*/
		if(cStatoOrdini==ORDINE_EVASO || cStatoOrdini==ORDINE_SPEDITO_HOST){
			sprintf(szSQLCmd,"update ric_ord set rostato='%c',ropsrea=ropspre,rodteva='%s',rotmeva='%s' where ordprog='%s';",
				cStatoOrdini,
				GetDateYYYYMMDD(time((long *)0),szDateBuffer),
				GetTimeHHMMSS(time((long *)0),szTimeBuffer),
				szOrdineKey);
		} else {
			sprintf(szSQLCmd,"update ric_ord set rostato='%c' where ordprog='%s';",cStatoOrdini,szOrdineKey);
		}
		PGRes=PGExecSQL(FALSE,szSQLCmd);
		
		if(atoi(PQcmdTuples(PGRes))==0){
			gtk_text_printf(NULL,txt_msgs,"Fallito settaggio stato %c [Ordine %s]\n",cStatoOrdini,szOrdineKey);
		} else { 
			nOrdini++;
			/*
			* Update a video
			*/
			UpdateOrdine(szOrdineKey);
		}

		PQclear(PGRes);

	} while((lista=g_list_next(lista)));
	gtk_text_printf(NULL,txt_msgs,"Settati %d Ordini in Stato [%c]\n",nOrdini,cStatoOrdini);

#ifdef TRACE
	trace_out_vstr_date(1,"Settati %d Ordini in Stato [%c]\n",nOrdini,cStatoOrdini);
#endif

	gtk_widget_destroy(GTK_WIDGET(user_data));
}

void on_dlg_set_ordini_pb_cancel_clicked (gpointer user_data,GtkButton *button)
{
	gtk_widget_destroy(GTK_WIDGET(user_data));
}


void on_dlg_reset_settore_pb_resetta_clicked (GtkButton *button, gpointer user_data)
{
	char szBuffer[128];
	GtkWidget *sp_isola     = get_widget(dlg_reset_settore,"sp_isola");
	GtkWidget *sp_settore   = get_widget(dlg_reset_settore,"sp_settore");
	int nIsola   = gtk_spin_button_get_value_as_int((GtkSpinButton *)sp_isola);
	int nSettore = gtk_spin_button_get_value_as_int((GtkSpinButton *)sp_settore);

	sprintf(szBuffer,"%d,%d",nIsola,nSettore);
	SendMessage(PROC_SETTORI, PROC_MAIN, RESET_SETTORE, szBuffer);

	gtk_widget_destroy(GTK_WIDGET(dlg_reset_settore));
}


void on_dlg_reset_settore_pb_cancel_clicked (gpointer user_data,GtkButton *button)
{
	gtk_widget_destroy(GTK_WIDGET(user_data));
}


void on_dlg_message_pb_reset_settori (GtkButton *button, gpointer user_data)
{
	SendMessage(PROC_SETTORI, PROC_MAIN, RESET_ALL, NULL);
	gtk_widget_destroy(GTK_WIDGET(dlg_message));
}


void do_lancio_elaborazione (GtkButton *button, gpointer user_data)
{
	/*
	* Lancio gli ordini delle elaborazioni selezionate
	*/
	GList *lista;
	char *pszNumElaborazione;
	char szNumElaborazione[128];
	char szStato[80];
	char szOrdine[64];
	int nRowIndex;
	int nOrdini=0;
	int nIndex;
	int nElaborazioni;
	PGresult *PGRes;
	PGresult *PGResUpdate;
	char cStatoSpedito;
	BOOL bVOLUM=FALSE;
	int nProgressivoPrecedenza=0;
	char szTransitPoint[128];
	char szFlagPallet[128];

	sprintf(szStato,"%c",ORDINE_SPEDITO);

	lista=g_list_first(GTK_CLIST(get_widget(dlg_distribuzione,"clist_distribuzione"))->selection);
	if(lista==NULL){
		return;
	}

	/*
	* Richiesta Fabio
	* Si vuole che gli ordini lanciati abbiano precedenza rispetto a quelli lanciati successivamente
	* anche se il progressivo lancio e' piu' basso. Infatti sorgeva il problema di elaborazioni
	* lanciate per i colli con pdf che sorpassavano quelle in linea con progressivo piu' alto
	* Aggiungo alla tabella ric_ord e ord_prod un progressivo precedenza assegnato al lancio da parte dell'
	* operatore, non come l'altro progressivo che se esiste gia' non si aggiorna.
	*/
	/*
	* Nuovo progressivo di precedenza
	*/
	PGRes=PGExecSQL(Cfg.nDebugVersion>2,"select nextval ('prog_lancio_precedenza');");
	nProgressivoPrecedenza=atoi(PQgetvalue(PGRes,0,0));
	PQclear(PGRes);
	

/*******************************************************
* VERSIONE CON GESTIONE PDF
********************************************************/

	
	do {
		nRowIndex=GPOINTER_TO_INT(lista->data);
		gtk_clist_get_text(GTK_CLIST(get_widget(dlg_distribuzione,"clist_distribuzione")),nRowIndex,0,&pszNumElaborazione);
		strcpy(szNumElaborazione,pszNumElaborazione);
		/*
		 * Richiesta Fabio: non metto in stato L gli ordini in Z ma in aspettativa PDF
		 */
		PGRes=PGExecSQL(Cfg.nDebugVersion,"select ordprog,rostato,roflpdf,roflgpz,rotrnsp from ric_ord where ronelab='%s' and ((rostato ='%c' or rostato is NULL) or (rostato='%c' and roflpdf!='%c')) order by ordprog;",szNumElaborazione,ORDINE_RICEVUTO,ORDINE_INCOMPLETO_TERMINATO,ORDINE_IN_ATTESA_PDF);




		nElaborazioni=PQntuples(PGRes);
		for (nIndex=0; nIndex<nElaborazioni; nIndex++){
			sprintf(szOrdine,"%s",PQgetvalue(PGRes,nIndex,0));
			cStatoSpedito=PQgetvalue(PGRes,nIndex,1)[0];

			sprintf(szTransitPoint,"%s",PQgetvalue(PGRes,nIndex,4));
			sprintf(szFlagPallet,"%s",PQgetvalue(PGRes,nIndex,3));
			/*
			 * Richiesta Fabio: non lancio ordini se pallet=1 e transit point = 31 o 32 o 35
			 */
			if(atoi(szFlagPallet) == 1 && ( atoi(szTransitPoint) == 31 || atoi(szTransitPoint) == 32 || atoi(szTransitPoint) == 35 ) ){

				gtk_text_printf("RED",txt_msgs,"ORDINE %s NON A PALLET (31/32/35)\n",szOrdine);

			} else {

				//Update su ric_ord	
				PGResUpdate=PGExecSQL(Cfg.nDebugVersion>2,"update ric_ord set roprgpr=%d where ordprog='%s';",nProgressivoPrecedenza,szOrdine);
				PQclear(PGResUpdate);

				switch(cStatoSpedito){
					case ORDINE_RICEVUTO:
					{
						sprintf(szStato,"%c",ORDINE_SPEDITO);
						bVOLUM=TRUE;
					}
					break;
					case ORDINE_INCOMPLETO_TERMINATO:
					{
						sprintf(szStato,"%c",ORDINE_INCOMPLETO_TERMINATO_SPEDITO);
					}
					break;
					default:
						// Esco dal ciclo
						continue;
					break;
				}
	 
				/*******************************************************
				* FINE VERSIONE CON GESTIONE PDF
				********************************************************/

			
				if(!LanciaOrdine(szOrdine,szStato)){
					gtk_text_printf(NULL,txt_msgs,"Errore in lancio ordine [%s]\n",szOrdine);
				} else {
					nOrdini++;
				}
			}
		}
		gtk_text_printf(NULL,txt_msgs,"Lanciata elaborazione %s [%d ordini]\n",szNumElaborazione,nOrdini);
#ifdef TRACE
		trace_out_vstr_date(1,"Lanciata elaborazione %s [%d ordini]\n",szNumElaborazione,nOrdini);
#endif
	} while((lista=g_list_next(lista)));

	SendMessage(PROC_VOLUM, PROC_MAIN, START_ANALISI, NULL);
	gtk_widget_destroy(GTK_WIDGET(dlg_lancio_distribuzioni));
	gtk_widget_destroy(GTK_WIDGET(dlg_distribuzione));
}

void do_lancio_elaborazione_senza_documento (GtkButton *button, gpointer user_data)
{
	/*
	* Lancio gli ordini delle elaborazioni selezionate
	*/
	GList *lista;
	char *pszNumElaborazione;
	char szNumElaborazione[128];
	char szStato[80];
	char szOrdine[64];
	int nRowIndex;
	int nOrdini=0;
	int nIndex;
	int nElaborazioni;
	PGresult *PGRes;
	PGresult *PGResUpdate;
	char cStatoSpedito;
	BOOL bVOLUM=FALSE;
	int nProgressivoPrecedenza=0;
	char szTransitPoint[128];
	char szFlagPallet[128];

	sprintf(szStato,"%c",ORDINE_SPEDITO);

	lista=g_list_first(GTK_CLIST(get_widget(dlg_distribuzione,"clist_distribuzione"))->selection);
	if(lista==NULL){
		return;
	}

	/*
	* Richiesta Fabio
	* Si vuole che gli ordini lanciati abbiano precedenza rispetto a quelli lanciati successivamente
	* anche se il progressivo lancio e' piu' basso. Infatti sorgeva il problema di elaborazioni
	* lanciate per i colli con pdf che sorpassavano quelle in linea con progressivo piu' alto
	* Aggiungo alla tabella ric_ord e ord_prod un progressivo precedenza assegnato al lancio da parte dell'
	* operatore, non come l'altro progressivo che se esiste gia' non si aggiorna.
	*/
	/*
	* Nuovo progressivo di precedenza
	*/
	PGRes=PGExecSQL(Cfg.nDebugVersion>2,"select nextval ('prog_lancio_precedenza');");
	nProgressivoPrecedenza=atoi(PQgetvalue(PGRes,0,0));
	PQclear(PGRes);
	

/*******************************************************
* VERSIONE CON GESTIONE PDF
********************************************************/

	
	do {
		nRowIndex=GPOINTER_TO_INT(lista->data);
		gtk_clist_get_text(GTK_CLIST(get_widget(dlg_distribuzione,"clist_distribuzione")),nRowIndex,0,&pszNumElaborazione);
		strcpy(szNumElaborazione,pszNumElaborazione);
		/*
		 * Richiesta Fabio: non metto in stato L gli ordini in Z ma in aspettativa PDF
		 */
		PGRes=PGExecSQL(Cfg.nDebugVersion,"select ordprog,rostato,roflpdf,roflgpz,rotrnsp from ric_ord where ronelab='%s' and ((rostato ='%c' or rostato is NULL) or (rostato='%c' and roflpdf!='%c')) order by ordprog;",szNumElaborazione,ORDINE_RICEVUTO,ORDINE_INCOMPLETO_TERMINATO,ORDINE_IN_ATTESA_PDF);
		nElaborazioni=PQntuples(PGRes);
		for (nIndex=0; nIndex<nElaborazioni; nIndex++){
			sprintf(szOrdine,"%s",PQgetvalue(PGRes,nIndex,0));
			cStatoSpedito=PQgetvalue(PGRes,nIndex,1)[0];

			sprintf(szTransitPoint,"%s",PQgetvalue(PGRes,nIndex,4));
			sprintf(szFlagPallet,"%s",PQgetvalue(PGRes,nIndex,3));
			/*
			 * Richiesta Fabio: non lancio ordini se pallet=1 e transit point = 31 o 32 o 35
			 */
			if(atoi(szFlagPallet) == 1 && ( atoi(szTransitPoint) == 31 || atoi(szTransitPoint) == 32 || atoi(szTransitPoint) == 35 ) ){

				gtk_text_printf("RED",txt_msgs,"ORDINE %s NON A PALLET (31/32/35)\n",szOrdine);

			} else {

				//Update su ric_ord	
				// In piu' del normale lancia elaborazione metto il flag pdf a N
				PGResUpdate=PGExecSQL(Cfg.nDebugVersion>2,"update ric_ord set roprgpr=%d, roflpdf='%c' where ordprog='%s';",nProgressivoPrecedenza,ORDINE_SENZA_PDF,szOrdine);
				PQclear(PGResUpdate);

				switch(cStatoSpedito){
					case ORDINE_RICEVUTO:
					{
						sprintf(szStato,"%c",ORDINE_SPEDITO);
						bVOLUM=TRUE;
					}
					break;
					case ORDINE_INCOMPLETO_TERMINATO:
					{
						sprintf(szStato,"%c",ORDINE_INCOMPLETO_TERMINATO_SPEDITO);
					}
					break;
					default:
						// Esco dal ciclo
						continue;
					break;
				}
	 
				/*******************************************************
				* FINE VERSIONE CON GESTIONE PDF
				********************************************************/

			
				if(!LanciaOrdine(szOrdine,szStato)){
					gtk_text_printf(NULL,txt_msgs,"Errore in lancio ordine [%s]\n",szOrdine);
				} else {
					nOrdini++;
				}
			}
		}
		gtk_text_printf(NULL,txt_msgs,"Lanciata elaborazione %s [%d ordini]\n",szNumElaborazione,nOrdini);
#ifdef TRACE
		trace_out_vstr_date(1,"Lanciata elaborazione %s [%d ordini]\n",szNumElaborazione,nOrdini);
#endif
	} while((lista=g_list_next(lista)));

	SendMessage(PROC_VOLUM, PROC_MAIN, START_ANALISI, NULL);
	gtk_widget_destroy(GTK_WIDGET(dlg_lancio_distribuzioni));
	gtk_widget_destroy(GTK_WIDGET(dlg_distribuzione));
}


/*
* 17/06/2019 rm : modifiche per amazon - dettaglio colli
*/
void do_consuntivo_elaborazione (GtkButton *button, gpointer user_data)
{
	/*
	* Lancio gli ordini delle elaborazioni selezionate
	*/
	GList *lista;
	char *pszNumElaborazione;
	char szNumElaborazione[128];
	char szStato[80];
	char szOrdine[64];
	int nRowIndex;
	int nOrdini=0;
	int nIndex;
	int nElaborazioni;
	PGresult *PGRes;
	char szTmpOrdiniFile[128];
	char szTmpColliFile[128];
	char szTmpDettColliFile[128];
	char szOrdiniFile[128];
	char szColliFile[128];
	char szDettColliFile[128];
	char szOrdiniFileBackup[128];
	char szColliFileBackup[128];
	char szDettColliFileBackup[128];
	BOOL bRetVal=TRUE;
	FILE *fpOrdini=0;
	FILE *fpColli=0;
	FILE *fpDettColli=0;

	sprintf(szStato,"%c",ORDINE_SPEDITO);

	lista=g_list_first(GTK_CLIST(get_widget(dlg_distribuzione,"clist_distribuzione"))->selection);
	if(lista==NULL){
		return;
	}
	do {
		nRowIndex=GPOINTER_TO_INT(lista->data);
		gtk_clist_get_text(GTK_CLIST(get_widget(dlg_distribuzione,"clist_distribuzione")),nRowIndex,0,&pszNumElaborazione);
		strcpy(szNumElaborazione,pszNumElaborazione);
		/*
		 * Richiesta Ferrari: diversamente da quanto chiesto inizialmente consuntivo
		 * anche gli ordini fittizi
		 */
		//PGRes=PGExecSQL(FALSE,"select ordprog from ric_ord where ronelab='%s' and rostato in ('A','V','R','P','T','E','H','J','Z','L','W','X') and roflftz!='F' order by ordprog;",szNumElaborazione);
		PGRes=PGExecSQL(FALSE,"select ordprog from ric_ord where ronelab='%s' and rostato in ('A',' ','R','P','T','E','H','J','Z','L','W','X') order by ordprog;",szNumElaborazione);

		nElaborazioni=PQntuples(PGRes);

		if(nElaborazioni){
			sprintf(szTmpOrdiniFile,"%s/%s",Cfg.szPathExport,"tmpordcni.txt");
			sprintf(szTmpColliFile,"%s/%s",Cfg.szPathExport,"tmpcolli.txt");
			sprintf(szTmpDettColliFile,"%s/%s",Cfg.szPathExport,"tmpdettcolli.txt");

			sprintf(szOrdiniFile,"%s/%s",Cfg.szPathExport,StrTrimAll(Cfg.szExpOrdiniFile));
			sprintf(szColliFile,"%s/%s",Cfg.szPathExport,StrTrimAll(Cfg.szExpColliFile));
			sprintf(szDettColliFile,"%s/%s",Cfg.szPathExport,StrTrimAll(Cfg.szExpDettColliFile));

			sprintf(szOrdiniFileBackup,"%s/%s.backup",Cfg.szPathExport,StrTrimAll(Cfg.szExpOrdiniFile));
			sprintf(szColliFileBackup,"%s/%s.backup",Cfg.szPathExport,StrTrimAll(Cfg.szExpColliFile));
			sprintf(szDettColliFileBackup,"%s/%s.backup",Cfg.szPathExport,StrTrimAll(Cfg.szExpDettColliFile));

			if(FileExists(szTmpOrdiniFile)){
				unlink(szTmpOrdiniFile);
			}
			if(FileExists(szTmpColliFile)){
				unlink(szTmpColliFile);
			}
			if(FileExists(szTmpDettColliFile)){
				unlink(szTmpDettColliFile);
			}

			/* verifico presenza dati in attesa di ricezione da host */
			if(FileExists(szOrdiniFile)){
				/* mi fermo e do errore */
				PQclear(PGRes);
#ifdef TRACE
				trace_out_vstr_date(1,"Errore : file spedizione ordini [%s] Presente",szOrdiniFile);
#endif
				gtk_text_printf("CYAN",txt_msgs,"Errore : Altra spedizione in corso [%s]\n",szOrdiniFile);
				bRetVal=FALSE;
				return ;
			}

			/* verifico presenza dati in attesa di ricezione da host */
			if(FileExists(szColliFile)){
				/* mi fermo e do errore */
				PQclear(PGRes);
#ifdef TRACE
				trace_out_vstr_date(1,"Errore : file spedizione colli [%s] Presente",szColliFile);
#endif
				gtk_text_printf("CYAN",txt_msgs,"Errore : Altra spedizione in corso [%s]\n",szColliFile);
				bRetVal=FALSE;
				return ;
			}
			/* verifico presenza dati in attesa di ricezione da host */
			if(FileExists(szDettColliFile)){
				/* mi fermo e do errore */
				PQclear(PGRes);
#ifdef TRACE
				trace_out_vstr_date(1,"Errore : file spedizione dettaglio colli [%s] Presente",szDettColliFile);
#endif
				gtk_text_printf("CYAN",txt_msgs,"Errore : Altra spedizione in corso [%s]\n",szDettColliFile);
				bRetVal=FALSE;
				return ;
			}

			if((fpOrdini=fopen(szTmpOrdiniFile,"w"))==NULL){
#ifdef TRACE
				trace_out_vstr_date(1,"Errore in apertura file ordini [%s] [%s]",szTmpOrdiniFile,strerror(errno));
#endif
				gtk_text_printf("CYAN",txt_msgs,"Errore in apertura file ordini [%s]\n",szTmpOrdiniFile);
				bRetVal=FALSE;
			} else {
				for (nIndex=0; nIndex<nElaborazioni; nIndex++){
					sprintf(szOrdine,"%s",PQgetvalue(PGRes,nIndex,0));
					if(!InviaOrdine(szOrdine,fpOrdini)){
						gtk_text_printf(NULL,txt_msgs,"Errore in consuntivazione ordine [%s]\n",szOrdine);
					} else {
						nOrdini++;
					}
				}
			}
			if((fpColli=fopen(szTmpColliFile,"w"))==NULL){
#ifdef TRACE
				trace_out_vstr_date(1,"Errore in apertura file colli [%s] [%s]",szTmpColliFile,strerror(errno));
#endif
				gtk_text_printf("CYAN",txt_msgs,"Errore in apertura file colli [%s]\n",szTmpColliFile);
				bRetVal=FALSE;
			} else {
				for (nIndex=0; nIndex<nElaborazioni; nIndex++){
					sprintf(szOrdine,"%s",PQgetvalue(PGRes,nIndex,0));
					if(!InviaColliOrdine(szOrdine,fpColli)){
						gtk_text_printf(NULL,txt_msgs,"Errore in consuntivazione colli ordine [%s]\n",szOrdine);
					}
				}
			}
			if((fpDettColli=fopen(szTmpDettColliFile,"w"))==NULL){
#ifdef TRACE
				trace_out_vstr_date(1,"Errore in apertura file colli [%s] [%s]",szTmpDettColliFile,strerror(errno));
#endif
				gtk_text_printf("CYAN",txt_msgs,"Errore in apertura file dettaglio colli [%s]\n",szTmpDettColliFile);
				bRetVal=FALSE;
			} else {
				for (nIndex=0; nIndex<nElaborazioni; nIndex++){
					sprintf(szOrdine,"%s",PQgetvalue(PGRes,nIndex,0));
					if(!InviaDettaglioColliOrdine(szOrdine,fpDettColli)){
						gtk_text_printf(NULL,txt_msgs,"Errore in consuntivazione dettaglio colli ordine [%s]\n",szOrdine);
					}
				}
			}
		}
		PQclear(PGRes);
		


		if(bRetVal && nElaborazioni){
			if(fpOrdini) fclose(fpOrdini);
			if(fpColli) fclose(fpColli);
			if(fpDettColli) fclose(fpDettColli);


			/* rinomino il file ordine */
			if(fpOrdini) rename(szTmpOrdiniFile,szOrdiniFile);
			/* rinomino il file colli */
			if(fpColli) rename(szTmpColliFile,szColliFile);
			/* rinomino il file dettaglio colli */
			if(fpDettColli) rename(szTmpDettColliFile,szDettColliFile);

			gtk_text_printf(NULL,txt_msgs,"Consuntivata elaborazione %s [%d ordini]\n",szNumElaborazione,nOrdini);
#ifdef TRACE
			trace_out_vstr_date(1,"Consuntivata elaborazione %s [%d ordini]\n",szNumElaborazione,nOrdini);
#endif
		}
	} while((lista=g_list_next(lista)));

	gtk_widget_destroy(GTK_WIDGET(dlg_lancio_distribuzioni));
	gtk_widget_destroy(GTK_WIDGET(dlg_distribuzione));
}

/*
* rm+gg 09-06-2008 : richiesta di Rodighiero
* Lancio di una elaborazione con nessun settaggiode dello stato degli ordini
* la cosa viene ottenuta specificando uno stato ordine diverso da ORDINE_SPEDITO (S)
* in modo che la analisi volumetrica gestisca diversamente la cosa ...
*/
void do_lancio_consuntivo_elaborazione (GtkButton *button, gpointer user_data)
{
	/*
	* Lancio gli ordini delle elaborazioni selezionate
	*/
	GList *lista;
	char *pszNumElaborazione;
	char szNumElaborazione[128];
	char szStato[80];
	char szOrdine[64];
	int nRowIndex;
	int nOrdini=0;
	int nIndex;
	int nElaborazioni=0;
	PGresult *PGRes;
	GtkWidget *dlg_lancio_consuntivo=GTK_WIDGET(user_data);

	sprintf(szStato,"%c",ORDINE_SPEDITO_DA_CONSUNTIVARE);

	lista=g_list_first(GTK_CLIST(get_widget(dlg_distribuzione,"clist_distribuzione"))->selection);
	if(lista==NULL){
		return;
	}
	do {
		nRowIndex=GPOINTER_TO_INT(lista->data);
		gtk_clist_get_text(GTK_CLIST(get_widget(dlg_distribuzione,"clist_distribuzione")),nRowIndex,0,&pszNumElaborazione);
		strcpy(szNumElaborazione,pszNumElaborazione);
		PGRes=PGExecSQL(FALSE,"select ordprog from ric_ord where ronelab='%s' and (rostato =' ' or rostato='S' or rostato is NULL) order by ordprog;",szNumElaborazione);
		nElaborazioni=PQntuples(PGRes);
		for (nIndex=0; nIndex<nElaborazioni; nIndex++){
			sprintf(szOrdine,"%s",PQgetvalue(PGRes,nIndex,0));
			if(!LanciaOrdine(szOrdine,szStato)){
				gtk_text_printf(NULL,txt_msgs,"Errore in lancio ordine [%s]\n",szOrdine);
			} else {
				nOrdini++;
			}
		}
		PQclear(PGRes);
		gtk_text_printf(NULL,txt_msgs,"Lanciata elaborazione %s [%d ordini]\n",szNumElaborazione,nOrdini);
#ifdef TRACE
		trace_out_vstr_date(1,"Lanciata elaborazione %s [%d ordini]\n",szNumElaborazione,nOrdini);
#endif
	} while((lista=g_list_next(lista)));

	SendMessage(PROC_VOLUM, PROC_MAIN, START_ANALISI, NULL);
	gtk_widget_destroy(GTK_WIDGET(dlg_distribuzione));
	gtk_widget_destroy(GTK_WIDGET(dlg_lancio_distribuzioni));
	gtk_widget_destroy(GTK_WIDGET(dlg_lancio_consuntivo));
}

/*
* Lancio di una elaborazione con evasione degli ordini
* la cosa viene ottenuta specificando uno stato ordine diverso da ORDINE_SPEDITO (S)
* in modo che la analisi volumetrica gestisca diversamente la cosa ...
*/
void do_lancio_evasione_elaborazione (GtkButton *button, gpointer user_data)
{
	/*
	* Lancio gli ordini delle elaborazioni selezionate
	*/
	GList *lista;
	char *pszNumElaborazione;
	char szNumElaborazione[128];
	char szStato[80];
	char szOrdine[64];
	int nRowIndex;
	int nOrdini=0;
	int nIndex;
	int nElaborazioni=0;
	PGresult *PGRes;
	GtkWidget *dlg_lancio_evasione=GTK_WIDGET(user_data);

	sprintf(szStato,"%c",ORDINE_SPEDITO_DA_EVADERE);

	lista=g_list_first(GTK_CLIST(get_widget(dlg_distribuzione,"clist_distribuzione"))->selection);
	if(lista==NULL){
		return;
	}
	do {
		nRowIndex=GPOINTER_TO_INT(lista->data);
		gtk_clist_get_text(GTK_CLIST(get_widget(dlg_distribuzione,"clist_distribuzione")),nRowIndex,0,&pszNumElaborazione);
		strcpy(szNumElaborazione,pszNumElaborazione);
		PGRes=PGExecSQL(FALSE,"select ordprog from ric_ord where ronelab='%s' and (rostato =' ' or rostato='S' or rostato is NULL) order by ordprog;",szNumElaborazione);
		nElaborazioni=PQntuples(PGRes);
		for (nIndex=0; nIndex<nElaborazioni; nIndex++){
			sprintf(szOrdine,"%s",PQgetvalue(PGRes,nIndex,0));
			if(!LanciaOrdine(szOrdine,szStato)){
				gtk_text_printf(NULL,txt_msgs,"Errore in lancio ordine [%s]\n",szOrdine);
			} else {
				nOrdini++;
			}
		}
		PQclear(PGRes);
		gtk_text_printf(NULL,txt_msgs,"Lanciata elaborazione %s [%d ordini]\n",szNumElaborazione,nOrdini);
#ifdef TRACE
		trace_out_vstr_date(1,"Lanciata elaborazione %s [%d ordini]\n",szNumElaborazione,nOrdini);
#endif
	} while((lista=g_list_next(lista)));

	SendMessage(PROC_VOLUM, PROC_MAIN, START_ANALISI, NULL);
	gtk_widget_destroy(GTK_WIDGET(dlg_distribuzione));
	gtk_widget_destroy(GTK_WIDGET(dlg_lancio_distribuzioni));
	gtk_widget_destroy(GTK_WIDGET(dlg_lancio_evasione));
}


void on_dlg_lancio_distribuzioni_pb_cancel_clicked (gpointer user_data,GtkButton *button)
{
	gtk_widget_destroy(GTK_WIDGET(user_data));
}


/*------------*/
/* DLG_SELECT */
/*------------*/

void on_dlg_select_pb_ok_clicked            (gpointer user_data,GtkButton *button)
{
	gtk_widget_destroy(GTK_WIDGET(user_data));
}

void on_dlg_select_pb_lista_articoli_clicked (GtkButton       *button, gpointer         user_data)
{

}

void on_dlg_select_pb_cancel_clicked        (gpointer user_data,GtkButton *button)
{
	gtk_widget_destroy(GTK_WIDGET(user_data));
}

void on_dlg_select_pb_expand_clicked        (GtkButton       *button, gpointer         user_data)
{
	char szField[128];
	GtkWidget *cb_field=get_widget(dlg_select,"cb_field");
	GtkWidget *dlg_expand=create_dlg_expand();
	GtkWidget *sw_expand;
	GtkWidget *lst;
	char szSelectCmd[256];

	gtk_window_set_focus (GTK_WINDOW (dlg_expand), get_widget(dlg_expand,"pb_ok"));
	gtk_widget_show(dlg_expand);
	gtk_window_set_transient_for(GTK_WINDOW(dlg_expand),GTK_WINDOW(dlg_select));

	sw_expand=get_widget(dlg_expand,"sw_expand");

	strcpy(szField,gtk_entry_get_text (GTK_ENTRY (GTK_COMBO(cb_field)->entry)));

	sprintf(szSelectCmd,"select %s,count(%s) from ric_ord group by %s order by %s",szField,szField,szField,szField);

	CreateListFromSelect(dlg_expand,sw_expand,&lst,"lst_expand",GTK_SELECTION_SINGLE,szSelectCmd,NULL,0);
}

void on_dlg_select_pb_and_clicked           (GtkButton       *button, gpointer         user_data)
{
	GtkWidget *txf_select=get_widget(dlg_select,"txf_select");

	strcat(szSelectOrdiniCondition," AND ");
	gtk_editable_delete_text(GTK_EDITABLE(txf_select),0,-1);
	gtk_text_printf(NULL,txf_select,"%s%s",szSelectOrdiniCmd,szSelectOrdiniCondition);
	gtk_widget_set_sensitive(get_widget(dlg_select,"pb_and"),FALSE);
	gtk_widget_set_sensitive(get_widget(dlg_select,"pb_or"),FALSE);
}

void on_dlg_select_pb_start_sel_clicked     (GtkButton       *button, gpointer         user_data)
{
	int nTuples;
	GtkWidget *sw_select=get_widget(dlg_select,"sw_select");
	GtkWidget *lst;
	PGresult *PGRes;

	strcat(szSelectOrdiniCmd,szSelectOrdiniCondition);
	strcat(szSelectOrdiniCmd,szFiltroSelezione);
	strcat(szSelectOrdiniCmd,";");

	if((lst=get_widget(dlg_select,"lst_select"))){
		gtk_widget_destroy(lst);
	}

	nTuples=CreateListFromSelect(dlg_select,sw_select,&lst,"lst_select",GTK_SELECTION_SINGLE,szSelectOrdiniCmd,NULL,0);

	PGRes=PGExecSQL(Cfg.nDebugVersion,szSelectOrdiniCmd);
	if(PQresultStatus(PGRes) == PGRES_TUPLES_OK ){
		gtk_label_printf(get_widget(dlg_select,"lb_selection"),"Selezionati %d ordini",nTuples);
		if (nTuples){
		gtk_widget_set_sensitive(get_widget(dlg_select,"pb_lancio"),TRUE);
		gtk_widget_set_sensitive(get_widget(dlg_select,"pb_lista_articoli"),TRUE);
		}
	} else {
		gtk_label_printf(get_widget(dlg_select,"lb_selection"),"SELECT ERRATA");
		gtk_widget_set_sensitive(get_widget(dlg_select,"pb_lancio"),FALSE);
		gtk_widget_set_sensitive(get_widget(dlg_select,"pb_lista_articoli"),FALSE);
		if((lst=get_widget(dlg_select,"lst_select"))){
			gtk_widget_destroy(lst);
		}
	}
	PQclear(PGRes);
	/*
	* Reset Select Ordini e Lancio Ordini
	*/
	strcpy(szSelectOrdiniCmd,"select * from ric_ord where ");
	sprintf(szLancioOrdiniCmd,"update ric_ord set rostato='%c' where ",ORDINE_SPEDITO);
}

void on_dlg_select_pb_reset_sel_clicked     (GtkButton       *button, gpointer         user_data)
{
	GtkWidget *txf_select=get_widget(dlg_select,"txf_select");

	strcpy(szSelectOrdiniCmd,"select * from ric_ord where ");
	strcpy(szSelectOrdiniCondition,"");

	gtk_label_printf(get_widget(dlg_select,"lb_selection"),"");

	sprintf(szLancioOrdiniCmd,"update ric_ord set rostato='%c' where ",ORDINE_SPEDITO);
	gtk_editable_delete_text(GTK_EDITABLE(txf_select),0,-1);
	gtk_text_printf(NULL,txf_select,"%s%s",szSelectOrdiniCmd,szSelectOrdiniCondition);
	gtk_widget_set_sensitive(get_widget(dlg_select,"pb_lancio"),FALSE);
	gtk_widget_set_sensitive(get_widget(dlg_select,"pb_lista_articoli"),FALSE);
}

void on_dlg_select_pb_select_all_clicked    (GtkButton       *button, gpointer         user_data)
{
	GtkWidget *txf_select=get_widget(dlg_select,"txf_select");

	strcat(szSelectOrdiniCondition,"TRUE");
	gtk_editable_delete_text(GTK_EDITABLE(txf_select),0,-1);
	gtk_text_printf(NULL,txf_select,"%s%s",szSelectOrdiniCmd,szSelectOrdiniCondition);
}

void on_dlg_select_pb_or_clicked            (GtkButton       *button, gpointer         user_data)
{
	GtkWidget *txf_select=get_widget(dlg_select,"txf_select");

	strcat(szSelectOrdiniCondition," OR ");
	gtk_editable_delete_text(GTK_EDITABLE(txf_select),0,-1);
	gtk_text_printf(NULL,txf_select,"%s%s",szSelectOrdiniCmd,szSelectOrdiniCondition);
	gtk_widget_set_sensitive(get_widget(dlg_select,"pb_and"),FALSE);
	gtk_widget_set_sensitive(get_widget(dlg_select,"pb_or"),FALSE);
}

void on_dlg_select_pb_lancio_clicked(GtkButton * button, gpointer user_data)
{
	PGresult *PGRes;
	int nTuples;

	strcat(szLancioOrdiniCmd,szSelectOrdiniCondition);
	strcat(szLancioOrdiniCmd,szFiltroSelezione);
	strcat(szLancioOrdiniCmd,";");

	PGRes=PGExecSQL(FALSE,szLancioOrdiniCmd);

	/*
	* Reset Select Ordini e Lancio Ordini
	*/
	strcpy(szSelectOrdiniCmd,"select * from ric_ord where ");
	sprintf(szLancioOrdiniCmd,"update ric_ord set rostato='%c' where ",ORDINE_SPEDITO);
	strcpy(szSelectOrdiniCondition,"");
	nTuples=atoi(PQcmdTuples(PGRes));
	gtk_label_printf(get_widget(dlg_select,"lb_selection"),"Lanciati %d ordini",nTuples);
	PQclear(PGRes);

	SendMessage(PROC_VOLUM, PROC_MAIN, START_ANALISI, NULL);
}


/*--------------------*/
/* DLG_CARICO_SETTORI */
/*--------------------*/

void on_dlg_carico_settori_pb_apply_clicked (GtkButton       *button, gpointer         user_data)
{
	GtkWidget *dlg_carico_settori = (GtkWidget *)user_data;
	GtkWidget *lst=get_widget(dlg_carico_settori,"lst_elaborazioni");
	GtkWidget *rb_copie=get_widget(dlg_carico_settori,"rb_copie");
	//GtkWidget *rb_prese=get_widget(dlg_carico_settori,"rb_prese");
	PGresult *PGRes;
	char *szElaborazione;
	int nSettore[10][20];
	GtkWidget *lb_widget;
	GtkWidget *pr_widget;
	GtkWidget *rl_widget;
	char szSettore[128];
	char szSqlCmd[1024];
	int nMaxValue=0;
	int nIndex;
	int nRiga;
	int nIsolaIndex;
	int nSettoreIndex;
	BOOL bFirst=TRUE;
	GList *lista;

	for(nIsolaIndex=1;nIsolaIndex<=2;nIsolaIndex++){
		for(nSettoreIndex=1;nSettoreIndex<=8;nSettoreIndex++){
			nSettore[nIsolaIndex][nSettoreIndex]=0;
			sprintf(szSettore,"pr_settore_%d_%d",nIsolaIndex,nSettoreIndex);
			if((pr_widget=get_widget(dlg_carico_settori,szSettore))){
				gtk_progress_set_value(GTK_PROGRESS(pr_widget),(gfloat)nSettore[nIsolaIndex][nSettoreIndex]);
			}
			sprintf(szSettore,"lb_settore_%d_%d",nIsolaIndex,nSettoreIndex);
			if((lb_widget=get_widget(dlg_carico_settori,szSettore))){
				gtk_label_printf(lb_widget,"%d",nSettore[nIsolaIndex][nSettoreIndex]);
			}
		}
	}

	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(rb_copie))){
		strcpy(szSqlCmd,"select u.isola,u.settore,sum(ra.raqtord) from ubicazioni as u,ric_art as ra,ric_ord as ro where u.codprod=ra.racdpro and ro.ordprog=ra.ordprog and (");
	} else {
		strcpy(szSqlCmd,"select u.isola,u.settore,count(ra.raqtord) from ubicazioni as u,ric_art as ra,ric_ord as ro where u.codprod=ra.racdpro and ro.ordprog=ra.ordprog and (");
	}
	nRiga=0;
	lista=g_list_first(GTK_CLIST(lst)->selection);
	if(lista==NULL){
		return;
	}
	do {
		nRiga=GPOINTER_TO_INT(lista->data);
		if(!bFirst){
			strcat(szSqlCmd," or ");
		} else {
			bFirst=FALSE;
		}
		gtk_clist_get_text(GTK_CLIST(lst),nRiga,0,&szElaborazione);
		strcat(szSqlCmd," ro.ronelab='");
		strcat(szSqlCmd,szElaborazione);
		strcat(szSqlCmd,"'");
	} while((lista=g_list_next(lista)));

	strcat(szSqlCmd,") group by u.isola,u.settore;");

	PGRes=PGExecSQL(Cfg.nDebugVersion,szSqlCmd);
	if(PQntuples(PGRes)){
		for(nIndex=0;nIndex<PQntuples(PGRes);nIndex++){
			nSettore[atoi(PQgetvalue(PGRes,nIndex,0))][atoi(PQgetvalue(PGRes,nIndex,1))]=atoi(PQgetvalue(PGRes,nIndex,2));
		}
		for(nIsolaIndex=1;nIsolaIndex<=2;nIsolaIndex++){
			for(nSettoreIndex=1;nSettoreIndex<=8;nSettoreIndex++){
				sprintf(szSettore,"lb_settore_%d_%d",nIsolaIndex,nSettoreIndex);
				if((lb_widget=get_widget(dlg_carico_settori,szSettore))){
					gtk_label_printf(lb_widget,"%d",nSettore[nIsolaIndex][nSettoreIndex]);
					nMaxValue=max(nMaxValue,nSettore[nIsolaIndex][nSettoreIndex]);
				}
			}
		}
		for(nIsolaIndex=1;nIsolaIndex<=2;nIsolaIndex++){
			sprintf(szSettore,"rl_%d",nIsolaIndex);
			if((rl_widget=get_widget(dlg_carico_settori,szSettore))){
				gtk_ruler_set_range(GTK_RULER(rl_widget),(gfloat)nMaxValue,(gfloat)0,(gfloat)0,(gfloat)nMaxValue);
			}
			for(nSettoreIndex=1;nSettoreIndex<=8;nSettoreIndex++){
				sprintf(szSettore,"pr_settore_%d_%d",nIsolaIndex,nSettoreIndex);
				if((pr_widget=get_widget(dlg_carico_settori,szSettore))){
					gtk_progress_configure(GTK_PROGRESS(pr_widget),(gfloat)0,(gfloat)0,(gfloat)nMaxValue);
					gtk_progress_set_value(GTK_PROGRESS(pr_widget),(gfloat)nSettore[nIsolaIndex][nSettoreIndex]);
				}
			}
		}
	}
	PQclear(PGRes);
}




void on_dlg_carico_settori_pb_cancel_clicked (gpointer user_data,GtkButton *button)
{
	gtk_widget_destroy(GTK_WIDGET(user_data));
}

void on_salva_stato_linea_pb_ok_clicked(GtkButton *button, gpointer user_data)
{
	char szFileName[128];

	sprintf(szFileName,"%s/settori.stato",Cfg.szPathData);
	SendMessage(PROC_SETTORI, PROC_MAIN, SAVE_STATE, szFileName);
	gtk_widget_destroy(GTK_WIDGET(user_data));
}

void on_carica_stato_linea_pb_ok_clicked(GtkButton *button, gpointer user_data)
{
	char szFileName[128];

	sprintf(szFileName,"%s/settori.stato",Cfg.szPathData);
	SendMessage(PROC_SETTORI, PROC_MAIN, RESTORE_STATE, szFileName);
	gtk_widget_destroy(GTK_WIDGET(user_data));
}



void on_pbm_storicizzazione_dati_activate   (GtkMenuItem     *menuitem, gpointer         user_data)
{
	GtkWidget *lb_msg;
	GtkWidget *lb_1;
	GtkWidget *lb_2;
	GtkWidget *pr_work;
	GtkWidget *rl_work;
	GtkWidget *lb_work;
	PGresult *PGRes;
	int nNumeroRecords;

		
	dlg_import_file=create_dlg_import_file();
	lb_1=get_widget(dlg_import_file,"lb_1");
	lb_2=get_widget(dlg_import_file,"lb_2");
	pr_work   = get_widget(dlg_import_file,"pr_file");
	rl_work   = get_widget(dlg_import_file,"rl_file");
	lb_work   = get_widget(dlg_import_file,"lb_file");

	PGRes=PGExecSQL(FALSE,"select ordprog from ric_ord where rostato='%c';",ORDINE_SPEDITO_HOST);
	nNumeroRecords=PQntuples(PGRes);
	PQclear(PGRes);

	if(nNumeroRecords){
		gtk_ruler_set_range(GTK_RULER(rl_work),(gfloat)0,(gfloat)nNumeroRecords,(gfloat)0,(gfloat)nNumeroRecords);
		gtk_progress_configure(GTK_PROGRESS(pr_work),(gfloat)0,(gfloat)0,(gfloat)nNumeroRecords);
		gtk_label_printf(get_widget(dlg_import_file,"lb_file"),"%d Ordini da storicizzare",nNumeroRecords);
		gtk_label_printf(lb_1,"%d Ordini da storicizzare",nNumeroRecords);
		gtk_label_printf(lb_2,"Premere Ok per iniziare la ricezione");
		gtk_window_set_focus(GTK_WINDOW (dlg_import_file), get_widget(dlg_import_file,"pb_ok"));

		gtk_window_set_title (GTK_WINDOW (dlg_import_file), "Storicizzazione Ordini");
		gtk_widget_show(dlg_import_file);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_import_file),GTK_WINDOW(main_window));
		gtk_signal_connect (GTK_OBJECT (get_widget(dlg_import_file,"pb_ok")), "clicked", GTK_SIGNAL_FUNC (do_storicizzazione_dati), dlg_import);
	} else {
		/*
		* Visualizza messaggio di errore
		*/
		dlg_message=create_dlg_message();

		lb_msg=get_widget(dlg_message,"lb_msg");

		gtk_label_printf(lb_msg,"Non sono presenti Ordini da storicizzare");
		gtk_window_set_title (GTK_WINDOW (dlg_message), "Storicizzazione Ordini");

		gtk_widget_show(dlg_message);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));
		/*
		* callback per pressione pulsante
		*/
		gtk_signal_connect (GTK_OBJECT (get_widget(dlg_message,"pb_ok")), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_pb_ok_clicked), dlg_message);
	}

}


void do_storicizzazione_dati(GtkButton *button, gpointer user_data)
{
	GtkWidget *lb_1;
	GtkWidget *lb_2;
	GtkWidget *pr_work;
	GtkWidget *rl_work;
	GtkWidget *lb_work;
	PGresult *PGRes;
	PGresult *PGResRighe;
	PGresult *PGResColli;
	PGresult *PGResOrdProd;
	PGresult *PGResOrdini;
	int nNumeroRecords;
	int nIndex;
	char szCommand[256];
	char szPathPdfStore[256];
	char szFileName[256];
	char szFLPDF[256];
	char szFLFTZ[256];

	lb_1=get_widget(dlg_import_file,"lb_1");
	lb_2=get_widget(dlg_import_file,"lb_2");
	pr_work   = get_widget(dlg_import_file,"pr_file");
	rl_work   = get_widget(dlg_import_file,"rl_file");
	lb_work   = get_widget(dlg_import_file,"lb_file");

	gtk_label_printf(lb_2,"Attendere la fine dell'operazione");
	gtk_widget_set_sensitive(get_widget(dlg_import_file,"pb_ok"),FALSE);
	gtk_update();

	PGRes=PGExecSQL(Cfg.nDebugVersion,"BEGIN WORK;");
	PQclear(PGRes);


	/*
	* Storicizzazione 
	*/
	PGRes=PGExecSQL(Cfg.nDebugVersion,"select ordprog,roflpdf,roflftz from ric_ord where rostato='%c';",ORDINE_SPEDITO_HOST);
	nNumeroRecords=PQntuples(PGRes);

	for(nIndex=0;nIndex<nNumeroRecords;nIndex++){
		/*
		* controllo che sia avvenuto l'inserimento nelle tabelle di storico
		* prima di cacellare la tuple dagli archivi di produzione
		*/

		/*
		* Controllo lo stato di ordine fittizio
		*/
		strcpy(szFLFTZ,PQgetvalue(PGRes,nIndex,2));

		/*
		* Righe
		*/
		if (szFLFTZ[0]==ORDINE_FITTIZIO){
		} else {
			PGResRighe=PGExecSQL(Cfg.nDebugVersion,"insert into rig_stor select * from rig_prod where ordprog='%s';",PQgetvalue(PGRes,nIndex,0));
			if(PGResRighe && PQresultStatus(PGResRighe) == PGRES_COMMAND_OK){
				PQclear(PGResRighe);
				PGResRighe=PGExecSQL(Cfg.nDebugVersion,"delete from rig_prod where ordprog='%s';",PQgetvalue(PGRes,nIndex,0));
			} else {
	#ifdef TRACE
				trace_out_vstr_date(1,"Fallita storicizzazione righe [ordine %s]",PQgetvalue(PGRes,nIndex,0));
	#endif
				gtk_text_printf(NULL,txt_msgs,"Fallita storicizzazione righe [ordine %s]",PQgetvalue(PGRes,nIndex,0));
			}
			PQclear(PGResRighe);
		}

		/*
		* Colli
		*/
		PGResColli=PGExecSQL(Cfg.nDebugVersion,"insert into col_stor select * from col_prod where ordprog='%s';",PQgetvalue(PGRes,nIndex,0));
		if(PGResColli && PQresultStatus(PGResColli) == PGRES_COMMAND_OK){
			PQclear(PGResColli);
			PGResColli=PGExecSQL(Cfg.nDebugVersion,"delete from col_prod where ordprog='%s';",PQgetvalue(PGRes,nIndex,0));
		} else {
#ifdef TRACE
			trace_out_vstr_date(1,"Fallita storicizzazione colli [ordine %s]",PQgetvalue(PGRes,nIndex,0));
#endif
			gtk_text_printf(NULL,txt_msgs,"Fallita storicizzazione colli [ordine %s]",PQgetvalue(PGRes,nIndex,0));
		}
		PQclear(PGResColli);
		
		/*
		* Ordini di produzione
		*/
		PGResOrdProd=PGExecSQL(Cfg.nDebugVersion,"insert into ord_stor select * from ord_prod where ordprog='%s';",PQgetvalue(PGRes,nIndex,0));
		if(PGResOrdProd && PQresultStatus(PGResOrdProd) == PGRES_COMMAND_OK){
			PQclear(PGResOrdProd);
			PGResOrdProd=PGExecSQL(Cfg.nDebugVersion,"delete from ord_prod where ordprog='%s';",PQgetvalue(PGRes,nIndex,0));
		} else {
#ifdef TRACE
			trace_out_vstr_date(1,"Fallita storicizzazione ordini di produzione [ordine %s]",PQgetvalue(PGRes,nIndex,0));
#endif
			gtk_text_printf(NULL,txt_msgs,"Fallita storicizzazione ordini di produzione [ordine %s]",PQgetvalue(PGRes,nIndex,0));
		}
		PQclear(PGResOrdProd);
		
		/*
		* Ordini e Articoli Ricevuti
		*/
		PGResOrdini=PGExecSQL(Cfg.nDebugVersion,"insert into ric_stor select * from ric_ord where ordprog='%s';",PQgetvalue(PGRes,nIndex,0));
		if(PGResOrdini && PQresultStatus(PGResOrdini) == PGRES_COMMAND_OK){
			PQclear(PGResOrdini);
			PGResOrdini=PGExecSQL(Cfg.nDebugVersion," delete from ric_ord where ordprog='%s';",PQgetvalue(PGRes,nIndex,0));
			PQclear(PGResOrdini);


			PGResOrdini=PGExecSQL(Cfg.nDebugVersion," delete from ric_art where ordprog='%s';",PQgetvalue(PGRes,nIndex,0));
			gtk_progress_set_value(GTK_PROGRESS(pr_work),(gfloat)nIndex);
			gtk_update();
		} else {
#ifdef TRACE
			trace_out_vstr_date(1,"Fallita storicizzazione ordini ricevuti [ordine %s]",PQgetvalue(PGRes,nIndex,0));
#endif
			gtk_text_printf(NULL,txt_msgs,"Fallita storicizzazione ordini ricevuti [ordine %s]",PQgetvalue(PGRes,nIndex,0));
		}
		PQclear(PGResOrdini);

		/* rm 02-07-2022 : storicizzo anche ordini bertello */
		PGResOrdini=PGExecSQL(Cfg.nDebugVersion," delete from ordini_bertello where ordprog='%s';",PQgetvalue(PGRes,nIndex,0));
		PQclear(PGResOrdini);

		PGResOrdini=PGExecSQL(Cfg.nDebugVersion," delete from colli_bertello where ordprog='%s';",PQgetvalue(PGRes,nIndex,0));
		PQclear(PGResOrdini);

		PGResOrdini=PGExecSQL(Cfg.nDebugVersion," delete from righe_bertello where ordprog='%s';",PQgetvalue(PGRes,nIndex,0));
		PQclear(PGResOrdini);

		/*
		* Cerco files di stampa Pdf e li cancello 
		*/
		strcpy(szFLPDF,PQgetvalue(PGRes,nIndex,1));
		if (szFLPDF[0]!=' '){
			sprintf(szPathPdfStore,"%s",Cfg.szPathStorePdf);
			sprintf(szFileName,"%s/%c%s.pdf",szPathPdfStore,szFLPDF[0],PQgetvalue(PGRes,nIndex,0));
			sprintf(szCommand,"rm -f %s",szFileName);
			system(szCommand);
#ifdef TRACE
			//trace_out_vstr_date(1,"Ordine [%s]: Cancellazione file PDF",PQgetvalue(PGRes,nIndex,0));
#endif
		}
	}

	PQclear(PGRes);

	PGRes=PGExecSQL(Cfg.nDebugVersion,"COMMIT WORK;");
	PQclear(PGRes);

	RefreshListaOrdini(sw_list,NULL);

	gtk_progress_set_value(GTK_PROGRESS(pr_work),(gfloat)nIndex);
	gtk_label_printf(lb_1,"Procedura di storicizzazione dati terminata");
	gtk_label_printf(lb_2,"Premere Cancel per uscire");
	gtk_widget_set_sensitive(get_widget(dlg_import_file,"pb_cancel"),TRUE);
	gtk_window_set_focus(GTK_WINDOW (dlg_import_file), get_widget(dlg_import_ordini,"pb_cancel"));
	gtk_update();
}



void on_pbm_visualizza_ordini_spediti_host_activate (GtkMenuItem     *menuitem, gpointer         user_data)
{
	GtkWidget *sw_list=get_widget(main_window,"sw_list");

	if( Cfg.nVisualizzaSpeditiHost==1){
		Cfg.nVisualizzaSpeditiHost=0;
	} else {
		Cfg.nVisualizzaSpeditiHost=1;
	}
	RefreshListaOrdini(sw_list,NULL);
}


void on_pbm_test_ios_activate               (GtkMenuItem     *menuitem, gpointer         user_data)
{
	/*
	* Impostazione numero settori e isole
	*/
	GtkWidget *dlg_test_ios=create_dlg_test_ios();

	gtk_widget_show(dlg_test_ios);

}


void on_pkdis_pb_send_clicked               (gpointer user_data,GtkButton *button)
{
	int nComando=strtol(gtk_entry_get_text(GTK_ENTRY (get_widget(user_data,"entry_comando"))),NULL,16);
	int nNumBytes=atoi(gtk_entry_get_text(GTK_ENTRY (get_widget(user_data,"entry_numbytes"))));
	int nByte1=strtol(gtk_entry_get_text(GTK_ENTRY (get_widget(user_data,"entry_byte_1"))),NULL,16);
	int nByte2=strtol(gtk_entry_get_text(GTK_ENTRY (get_widget(user_data,"entry_byte_2"))),NULL,16);
	int nByte3=strtol(gtk_entry_get_text(GTK_ENTRY (get_widget(user_data,"entry_byte_3"))),NULL,16);
	int nByte4=strtol(gtk_entry_get_text(GTK_ENTRY (get_widget(user_data,"entry_byte_4"))),NULL,16);
	char szBuffer[128];

	sprintf(szBuffer,"0,0,2,%x,%x,%x,%x,%x,%x",
		nComando, nNumBytes,
		nByte1, nByte2, nByte3, nByte4);

	SendMessage(PROC_IOS, PROC_MAIN,IOS_TEST_CMD, szBuffer);
}


void on_dlg_test_ios_pb_ok_clicked          (gpointer user_data,GtkButton *button)
{
	gtk_widget_destroy(GTK_WIDGET(user_data));
}


void on_dlg_test_ios_pb_cancel_clicked      (gpointer user_data,GtkButton *button)
{
	gtk_widget_destroy(GTK_WIDGET(user_data));
}


/*------------*/
/* DLG_STORICO */
/*------------*/

void on_dlg_ricerca_storico_pb_ok_clicked   (GtkButton *button, gpointer user_data)
{
	char szOrdProg[128];
	PGresult *PGRes;

	sprintf(szOrdProg,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_storico,"entry_ordkey"))));
	PGRes=PGExecSQL(Cfg.nDebugVersion,"select ordprog from ord_stor where ordprog='%s';",szOrdProg);
	if (PQntuples(PGRes)==0){
		gtk_label_printf(get_widget(dlg_storico,"lb_msg"),"ORDINE NON PRESENTE\nNELL'ARCHIVIO STORICO");
	} else {
		dlg_edit_ordine = create_dlg_edit_ordine();

		gtk_window_set_focus (GTK_WINDOW (dlg_edit_ordine), get_widget(dlg_edit_ordine,"pb_ok"));
		gtk_widget_set_sensitive(get_widget(dlg_edit_ordine,"pb_next"),FALSE);
		gtk_widget_set_sensitive(get_widget(dlg_edit_ordine,"pb_prev"),FALSE);
		gtk_widget_show(dlg_edit_ordine);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_edit_ordine),GTK_WINDOW(dlg_storico));
		gtk_window_set_transient_for(GTK_WINDOW(dlg_edit_ordine),GTK_WINDOW(main_window));
	
		/* visualizzo in dlg_edit_ordine dati di storico */
		bStorico=TRUE;
		EditOrdine(szOrdProg);
	}
	PQclear(PGRes);
}


void on_dlg_stampa_storico_pb_ok_clicked    (GtkButton *button, gpointer user_data)
{
	char szOrdProg[128];
	char szBuffer[128];
	PGresult *PGRes;

	sprintf(szOrdProg,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_storico,"entry_ordkey"))));
	PGRes=PGExecSQL(Cfg.nDebugVersion,"select ordprog from ord_stor where ordprog='%s';",szOrdProg);
	if (PQntuples(PGRes)==0){
		gtk_label_printf(get_widget(dlg_storico,"lb_msg"),"ORDINE NON PRESENTE\nNELL'ARCHIVIO STORICO");
	} else {
		sprintf(szBuffer,"%s,%s", szOrdProg, Cfg.szPrinterConsole);
		SendMessage(PROC_PRINTLABEL, PROC_MAIN, PRINT_PACKING_LIST_STORICO, szBuffer);
		gtk_widget_destroy(GTK_WIDGET(dlg_storico));
	}
	PQclear(PGRes);
}


void on_dlg_storico_pb_cancel_clicked       (gpointer user_data,GtkButton *button)
{
	gtk_widget_destroy(GTK_WIDGET(user_data));
}





void on_dlg_stampa_pb_stampa_clicked        (GtkButton       *button, gpointer         user_data)
{
	char szFileName[128];
	char szCommand[256];

	sprintf(szFileName,"%s/stampa.txt",Cfg.szPathData);
	sprintf(szCommand,"pr -f -h 'CNI - Stampa statistica di prelievo' %s | lpr -P%s",szFileName,Cfg.szPrinterConsole);
	system(szCommand);
}


void on_pbm_stat_prelievo_activate          (GtkMenuItem     *menuitem, gpointer         user_data)
{
	guint nAnno,nMese,nGiorno;
	GtkWidget *dlg_stat_prelievo=create_dlg_stat_prelievo();

	gtk_widget_show(dlg_stat_prelievo);
	gtk_window_set_transient_for(GTK_WINDOW(dlg_stat_prelievo),GTK_WINDOW(main_window));

	gtk_calendar_get_date(GTK_CALENDAR(get_widget(dlg_stat_prelievo,"cal_from_date")),&nAnno,&nMese,&nGiorno);
	gtk_label_printf(get_widget(dlg_stat_prelievo,"lb_from_date"),"%2d-%2d-%2d",nGiorno,nMese+1,nAnno);

	gtk_calendar_get_date(GTK_CALENDAR(get_widget(dlg_stat_prelievo,"cal_to_date")),&nAnno,&nMese,&nGiorno);
	gtk_label_printf(get_widget(dlg_stat_prelievo,"lb_to_date"),"%2d-%2d-%2d",nGiorno,nMese+1,nAnno);
}


void on_dlg_stat_prelievo_cal_from_date_day_selected (GtkCalendar     *calendar, gpointer         user_data)
{
	guint nAnno,nMese,nGiorno;

	gtk_calendar_get_date(calendar,&nAnno,&nMese,&nGiorno);
	gtk_label_printf(get_widget((GtkWidget *)user_data,"lb_from_date"),"%2d-%2d-%2d",nGiorno,nMese+1,nAnno);
}


void on_dlg_stat_prelievo_cal_to_date_day_selected (GtkCalendar     *calendar, gpointer         user_data)
{
	guint nAnno,nMese,nGiorno;

	gtk_calendar_get_date(calendar,&nAnno,&nMese,&nGiorno);
	gtk_label_printf(get_widget((GtkWidget *)user_data,"lb_to_date"),"%2d-%2d-%2d",nGiorno,nMese+1,nAnno);
}


void on_dlg_stat_prelievo_pb_ok_clicked (gpointer user_data,GtkButton *button)
{
	GtkWidget *dlg_stampa;
	char szFromDate[128];
	char szToDate[128];
	char szFileName[128];
	guint nAnno,nMese,nGiorno;


	gtk_calendar_get_date(GTK_CALENDAR(get_widget((GtkWidget *)user_data,"cal_from_date")),&nAnno,&nMese,&nGiorno);
	sprintf(szFromDate,"%04d%02d%02d",nAnno,nMese+1,nGiorno);

	gtk_calendar_get_date(GTK_CALENDAR(get_widget((GtkWidget *)user_data,"cal_to_date")),&nAnno,&nMese,&nGiorno);
	sprintf(szToDate,"%04d%02d%02d",nAnno,nMese+1,nGiorno);

	dlg_stampa=create_dlg_stampa();
	sprintf(szFileName,"%s/stampa.txt",Cfg.szPathData);
	StampaStatistica(szFromDate,szToDate,szFileName);
	gtk_text_read_file(get_widget(dlg_stampa,"txt_stampa"),szFileName);

	gtk_widget_show(dlg_stampa);
	gtk_window_set_transient_for(GTK_WINDOW(dlg_stampa),GTK_WINDOW(user_data));
}


void on_pbm_cambia_priorita (GtkMenuItem *menuitem, gpointer user_data)
{
	GtkWidget *dlg_set_prio = create_dlg_set_prio();
	GtkSpinButton *sp_prio = (GtkSpinButton *)get_widget(dlg_set_prio,"sp_prio");

	gtk_entry_set_editable(&(sp_prio->entry),FALSE);
	gtk_widget_show(dlg_set_prio);
	gtk_window_set_transient_for(GTK_WINDOW(dlg_set_prio),GTK_WINDOW(main_window));
	gtk_window_set_focus (GTK_WINDOW (dlg_set_prio), get_widget(dlg_set_prio,"sp_prio"));
}


void on_dlg_set_prio_pb_ok_clicked          (gpointer user_data,GtkButton *button)
{
	GtkWidget *sp_prio     = get_widget(user_data,"sp_prio");
	int nPrio   = gtk_spin_button_get_value_as_int((GtkSpinButton *)sp_prio);
	GList *lista;
	char *pszOrdineKey;
	char szOrdineKey[128];
	int nRowIndex;
	int nOrdini=0;

	lista=g_list_first(GTK_CLIST(lst_ordini)->selection);
	if(lista==NULL){
		/*
		* nessun ordine selezionato
		*/
		GtkWidget *lb_msg;
		GtkWidget *pb_ok;

		gtk_widget_destroy(GTK_WIDGET(user_data));

		dlg_message=create_dlg_message();
		lb_msg=get_widget(dlg_message,"lb_msg");
		pb_ok=get_widget(dlg_message,"pb_ok");

		gtk_label_printf(lb_msg,"NESSUN ORDINE SELEZIONATO");
		gtk_window_set_title (GTK_WINDOW (dlg_message), "Modifica Priorita' Ordini");
		gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
		gtk_widget_show(dlg_message);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));
		gtk_signal_connect (GTK_OBJECT (pb_ok), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_pb_ok_clicked), dlg_message);
		return;
	}
	do {
		nRowIndex=GPOINTER_TO_INT(lista->data);
		gtk_clist_get_text(GTK_CLIST(lst_ordini),nRowIndex,Cfg.nOrdiniKeyField,&pszOrdineKey);
		strcpy(szOrdineKey,pszOrdineKey);

		if(!SetPriority(szOrdineKey,nPrio)){
			gtk_text_printf(NULL,txt_msgs,"Errore in cambio priorita' ordine [%s]\n",szOrdineKey);
#ifdef TRACE
			trace_out_vstr_date(1,"Errore in cambio priorita'[%d] ordine %s\n",nPrio,szOrdineKey);
#endif
		} else {
			nOrdini++;
			UpdateOrdine(szOrdineKey);
		}
	} while((lista=g_list_next(lista)));
	gtk_text_printf(NULL,txt_msgs,"Settata priorita' [%d] a %d ordini\n",nPrio,nOrdini);
#ifdef TRACE
	trace_out_vstr_date(1,"Settata priorita' [%d] a %d ordini\n",nPrio,nOrdini);
#endif

	gtk_widget_destroy(GTK_WIDGET(user_data));
}


/*---------------*/
/* DLG_OPERATORI */
/*---------------*/
/*
* void on_dlg_operatori_pb_modifica_op_clicked (GtkButton       *button, gpointer         user_data)
*/
void on_dlg_operatori_pb_modifica_op_clicked (GtkButton       *button, gpointer         user_data)
{
	PGresult *PGRes;
	char szCodiceOp[40];
	char szSqlCmd[2048];

	strcpy(szCodiceOp,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_operatori,"entry_op_codice"))));
	StrTrimAll(szCodiceOp);

	if(strlen(szCodiceOp)!=0){
		strcpy(szSqlCmd,"update operatori set opnmope='");
		strcat(szSqlCmd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_operatori,"entry_op_nome"))));
		strcat(szSqlCmd,"', opcgope='");
		strcat(szSqlCmd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_operatori,"entry_op_cognome"))));
		strcat(szSqlCmd,"', opinope='");
		strcat(szSqlCmd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_operatori,"entry_op_indirizzo"))));
		strcat(szSqlCmd,"', oploope='");
		strcat(szSqlCmd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_operatori,"entry_op_localita"))));
		strcat(szSqlCmd,"', opprope='");
		strcat(szSqlCmd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_operatori,"entry_op_provincia"))));
		strcat(szSqlCmd,"', opcpope='");
		strcat(szSqlCmd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_operatori,"entry_op_cap"))));
		strcat(szSqlCmd,"', opnmtel='");
		strcat(szSqlCmd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_operatori,"entry_op_telefono"))));
		strcat(szSqlCmd,"' where opcdope='");
		strcat(szSqlCmd,szCodiceOp);
		strcat(szSqlCmd,"';");
		
		PGRes=PGExecSQL(Cfg.nDebugVersion,szSqlCmd);
		if (PQresultStatus(PGRes) == PGRES_COMMAND_OK ) {
	#ifdef TRACE
			trace_out_vstr_date(1,"Modificato operatore [%s]",szCodiceOp);
	#endif
		} else {
	#ifdef TRACE
			trace_out_vstr_date(1,"Fallita modifica su angrafica operatore [%s]",szCodiceOp);
	#endif
		}
		PQclear(PGRes);

		RefreshListaAnagOperatori();
	} else {
		/*
		* nessun operatore selezionato
		*/
		GtkWidget *lb_msg;
		GtkWidget *pb_ok;

		dlg_message=create_dlg_message();
		lb_msg=get_widget(dlg_message,"lb_msg");
		pb_ok=get_widget(dlg_message,"pb_ok");

		gtk_label_printf(lb_msg,"NESSUN OPERATORE SELEZIONATO");
		gtk_window_set_title (GTK_WINDOW (dlg_message), "Modifica Operatore");
		gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
		gtk_widget_show(dlg_message);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));
		gtk_signal_connect (GTK_OBJECT (pb_ok), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_pb_ok_clicked), dlg_message);
	}
}


/*
* void on_dlg_operatori_pb_aggiungi_op_clicked (GtkButton *button, gpointer user_data)
*/
void on_dlg_operatori_pb_aggiungi_op_clicked (GtkButton *button, gpointer user_data)
{
	PGresult *PGRes;
	char szCodiceOp[40];
	char szSqlCmd[1024];

	strcpy(szCodiceOp,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_operatori,"entry_op_codice"))));
	StrTrimAll(szCodiceOp);
	if(strlen(szCodiceOp)!=0){

		sprintf(szSqlCmd,"select opcdope from operatori where opcdope='%s';",szCodiceOp);
		PGRes=PGExecSQL(Cfg.nDebugVersion,szSqlCmd);
		if (PQntuples(PGRes)!=0){
			/*
			* codice operatore gia' presente
			*/
			GtkWidget *lb_msg;
			GtkWidget *pb_ok;

			dlg_message=create_dlg_message();
			lb_msg=get_widget(dlg_message,"lb_msg");
			pb_ok=get_widget(dlg_message,"pb_ok");

			PQclear(PGRes);

			gtk_label_printf(lb_msg,"CODICE OPERATORE\nGIA' PRESENTE");
			gtk_window_set_title (GTK_WINDOW (dlg_message), "Aggiungi Operatore");
			gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
			gtk_widget_show(dlg_message);
			gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));
			gtk_signal_connect (GTK_OBJECT (pb_ok), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_pb_ok_clicked), dlg_message);
		} else {
			PQclear(PGRes);
			strcpy(szSqlCmd,"insert into operatori values ('");
			strcat(szSqlCmd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_operatori,"entry_op_codice"))));
			strcat(szSqlCmd,"', '");
			strcat(szSqlCmd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_operatori,"entry_op_nome"))));
			strcat(szSqlCmd,"', '");
			strcat(szSqlCmd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_operatori,"entry_op_cognome"))));
			strcat(szSqlCmd,"', '");
			strcat(szSqlCmd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_operatori,"entry_op_indirizzo"))));
			strcat(szSqlCmd,"', '");
			strcat(szSqlCmd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_operatori,"entry_op_localita"))));
			strcat(szSqlCmd,"', '");
			strcat(szSqlCmd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_operatori,"entry_op_provincia"))));
			strcat(szSqlCmd,"', '");
			strcat(szSqlCmd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_operatori,"entry_op_cap"))));
			strcat(szSqlCmd,"', '");
			strcat(szSqlCmd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_operatori,"entry_op_telefono"))));
			strcat(szSqlCmd,"');");
			
			PGRes=PGExecSQL(Cfg.nDebugVersion,szSqlCmd);
			if (PQresultStatus(PGRes) == PGRES_COMMAND_OK ) {
		#ifdef TRACE
				trace_out_vstr_date(1,"Inserito operatore [%s]",gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_operatori,"entry_op_codice"))));
		#endif
			} else {
		#ifdef TRACE
				trace_out_vstr_date(1,"Fallito inserimento operatore [%s]",gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_operatori,"entry_op_codice"))));
		#endif
			}
			PQclear(PGRes);

			RefreshListaAnagOperatori();
		}

	} else {
		/*
		* nessun operatore selezionato
		*/
		GtkWidget *lb_msg;
		GtkWidget *pb_ok;

		dlg_message=create_dlg_message();
		lb_msg=get_widget(dlg_message,"lb_msg");
		pb_ok=get_widget(dlg_message,"pb_ok");

		gtk_label_printf(lb_msg,"INSERIRE IL CODICE OPERATORE");
		gtk_window_set_title (GTK_WINDOW (dlg_message), "Aggiungi Operatore");
		gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
		gtk_widget_show(dlg_message);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));
		gtk_signal_connect (GTK_OBJECT (pb_ok), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_pb_ok_clicked), dlg_message);
	}
}


/*
* void on_dlg_operatori_pb_elimina_op_clicked (GtkButton *button, gpointer user_data)
*/
void on_dlg_operatori_pb_elimina_op_clicked (GtkButton *button, gpointer user_data)
{
	GtkWidget *lb_msg;
	GtkWidget *pb_ok;
	char szCodOpe[40];

	dlg_message=create_dlg_message();

	lb_msg=get_widget(dlg_message,"lb_msg");
	pb_ok=get_widget(dlg_message,"pb_ok");
	sprintf(szCodOpe,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_operatori,"entry_op_codice"))));
	if (strlen(szCodOpe)!=0){
		gtk_label_printf(lb_msg,"CONFERMI L'ELIMINAZIONE\nDELL'OPERATORE %s",szCodOpe);
		gtk_window_set_title (GTK_WINDOW (dlg_message), "Elimina Operatore");
		gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
		gtk_widget_show(dlg_message);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));
		/*
		* callback per pressione pulsante
		*/
		gtk_signal_connect (GTK_OBJECT (pb_ok), "clicked", GTK_SIGNAL_FUNC (on_dlg_operatori_do_elimina_operatore), dlg_message);
	} else {
		gtk_label_printf(lb_msg,"NESSUN OPERATORE SELEZIONATO");
		gtk_window_set_title (GTK_WINDOW (dlg_message), "Elimina Operatore");
		gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
		gtk_widget_show(dlg_message);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));
		gtk_signal_connect (GTK_OBJECT (pb_ok), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_pb_ok_clicked), dlg_message);
	}
}


/*
* void on_dlg_operatori_do_elimina_operatore (GtkButton *button, gpointer user_data)
*/
void on_dlg_operatori_do_elimina_operatore (GtkButton *button, gpointer user_data)
{
	PGresult *PGRes;
	char szSqlCmd[1024];

	gtk_widget_destroy(dlg_message);

	strcpy(szSqlCmd,"delete from operatori where opcdope='");
	strcat(szSqlCmd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_operatori,"entry_op_codice"))));
	strcat(szSqlCmd,"';");

	PGRes=PGExecSQL(Cfg.nDebugVersion,szSqlCmd);
	if (PQresultStatus(PGRes) == PGRES_COMMAND_OK ) {
#ifdef TRACE
		trace_out_vstr_date(1,"Eliminato operatore [%s]",gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_operatori,"entry_op_codice"))));
#endif
	} else {
#ifdef TRACE
		trace_out_vstr_date(1,"Fallita eliminazione operatore [%s]",gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_operatori,"entry_op_codice"))));
#endif
	}
	PQclear(PGRes);
	
	RefreshListaAnagOperatori();
}


/*
* void on_dlg_operatori_pb_assegna_locazione_clicked (GtkButton *button, gpointer user_data)
*/
void on_dlg_operatori_pb_assegna_locazione_clicked (GtkButton *button, gpointer user_data)
{
	GtkWidget *lst;
	char szIsola[40];
	char szSettore[40];
	char szOperatore[40];
	char szSelectCmd[1024];
	char szBuffer[256];
	//char szData[80];
	//char szOra[80];
	PGresult *PGRes;

	strcpy(szIsola,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_operatori,"entry_isola"))));
	strcpy(szSettore,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_operatori,"entry_settore"))));
	//strcpy(szOperatore,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_operatori,"entry_operatore"))));
	strcpy(szOperatore,gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(get_widget(dlg_operatori,"combo_operatore"))->entry)));
	StrTrimAll(szOperatore);

	/* controllo che il codice operatore sia presente nell'anagrafica operatori */
	sprintf(szSelectCmd,"select opcdope from operatori where opcdope='%s';",szOperatore);
	PGRes=PGExecSQL(Cfg.nDebugVersion,szSelectCmd);
	if(PQntuples(PGRes)!=0){
		PQclear(PGRes);
		PGRes=PGExecSQL(Cfg.nDebugVersion,"update opeset set opcdope=\'%s\',dtmod=\'%s\',tmmod=\'%s\' where opisola=%s and opset=%s;"
									 ,szOperatore
									 ,GetDateYYYYMMDD(time((long *)0),szDateBuffer)
									 ,GetTimeHHMMSS(time((long *)0),szTimeBuffer)
									 ,szIsola
									 ,szSettore);
		if(atoi(PQcmdTuples(PGRes))==0){
	#ifdef TRACE
			trace_out_vstr_date(1,"Gestione Operatori: fallito settaggio ubicazione [Isola %s - Settore %s] operatore %s",szIsola,szSettore,szOperatore);
	#endif
		} else {
			//gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_operatori,"entry_operatore")),szOperatore);
			/*
			* Refresh lista Operatori - Ubicazione
			*/
			if((lst=get_widget(dlg_operatori,"lst_opeset"))){
				gtk_widget_destroy(lst);
			}
			strcpy(szSelectCmd,"select a.opisola as \"ISOLA\", a.opset as \"SETTORE\", a.opcdope as \"OPERATORE\", op.opnmope as \"NOME\", op.opcgope as \"COGNOME\", a.dtmod as \"DATA ASSEGNAZIONE\", a.tmmod as \"ORA ASSEGNAZIONE\" from opeset a, operatori op  where a.opcdope=op.opcdope order by opisola,opset;");
			CreateListFromSelect(dlg_operatori,get_widget(dlg_operatori,"sw_lista_opeset"),&lst,"lst_opeset",GTK_SELECTION_SINGLE,szSelectCmd,NULL,0);
			gtk_signal_connect (GTK_OBJECT (get_widget(dlg_operatori,"lst_opeset")), "select_row", GTK_SIGNAL_FUNC (on_lista_opeset_row_selected), 0);

			sprintf(szBuffer,"%s,%s",szIsola,szSettore);
			SendMessage(PROC_SETTORI, PROC_MAIN, SET_BADGE, szBuffer);
		}
		PQclear(PGRes);
	} else {
		/* codice operatore inesistente */
		GtkWidget *lb_msg;
		GtkWidget *pb_ok;

		dlg_message=create_dlg_message();
		lb_msg=get_widget(dlg_message,"lb_msg");
		pb_ok=get_widget(dlg_message,"pb_ok");

		gtk_label_printf(lb_msg,"CODICE OPERATORE\nNON VALIDO");
		gtk_window_set_title (GTK_WINDOW (dlg_message), "Assegna Ubicazione");
		gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
		gtk_widget_show(dlg_message);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));
		gtk_signal_connect (GTK_OBJECT (pb_ok), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_pb_ok_clicked), dlg_message);
	}
	/* aggiorno la situazione operatori/settori in settori */
	SendMessage(PROC_SETTORI, PROC_MAIN, REFRESH_DATA, NULL);
}


/*
* void on_dlg_operatori_pb_vouta_locazione_clicked (GtkButton *button, gpointer user_data)
*/
void on_dlg_operatori_pb_vouta_locazione_clicked (GtkButton *button, gpointer user_data)
{
	GtkWidget *lst;
	char szIsola[40];
	char szSettore[40];
	char szOperatore[40];
	char szSelectCmd[1024];
	char szBuffer[256];
	//char szData[40];
	//char szOra[40];
	PGresult *PGRes;

	strcpy(szIsola,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_operatori,"entry_isola"))));
	strcpy(szSettore,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_operatori,"entry_settore"))));
	//strcpy(szOperatore,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_operatori,"entry_operatore"))));

	PGRes=PGExecSQL(Cfg.nDebugVersion,"update opeset set opcdope=\'0000\', dtmod=\'%s\',tmmod=\'%s\' where opisola=%s and opset=%s;"
								,GetDateYYYYMMDD(time((long *)0),szDateBuffer)
								,GetTimeHHMMSS(time((long *)0),szTimeBuffer)
								,szIsola
								,szSettore);
	if(atoi(PQcmdTuples(PGRes))==0){
#ifdef TRACE
		trace_out_vstr_date(1,"Gestione Operatori: fallito reset ubicazione [Isola %s - Settore %s] operatore %s",szIsola,szSettore,szOperatore);
#endif
	} else {
		gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(get_widget(dlg_operatori,"combo_operatore"))->entry),"");
		//gtk_entry_set_text(GTK_ENTRY(get_widget(dlg_operatori,"entry_operatore")),"");
		/*
		* Refresh lista Operatori - Ubicazione
		*/
		if((lst=get_widget(dlg_operatori,"lst_opeset"))){
			gtk_widget_destroy(lst);
		}
		strcpy(szSelectCmd,"select a.opisola as \"ISOLA\", a.opset as \"SETTORE\", a.opcdope as \"OPERATORE\", op.opnmope as \"NOME\", op.opcgope as \"COGNOME\", a.dtmod as \"DATA ASSEGNAZIONE\", a.tmmod as \"ORA ASSEGNAZIONE\" from opeset a, operatori op  where a.opcdope=op.opcdope order by opisola,opset;");
		CreateListFromSelect(dlg_operatori,get_widget(dlg_operatori,"sw_lista_opeset"),&lst,"lst_opeset",GTK_SELECTION_SINGLE,szSelectCmd,NULL,0);
		gtk_signal_connect (GTK_OBJECT (get_widget(dlg_operatori,"lst_opeset")), "select_row", GTK_SIGNAL_FUNC (on_lista_opeset_row_selected), 0);

		/* aggiorno la situazione operatori/settori in settori */
		SendMessage(PROC_SETTORI, PROC_MAIN, REFRESH_DATA, NULL);
		sprintf(szBuffer,"%s,%s",szIsola,szSettore);
		SendMessage(PROC_SETTORI, PROC_MAIN, RESET_BADGE, szBuffer);
	}
	PQclear(PGRes);
}

void on_dlg_lancio_consuntivo_pb_lancio_clicked ( gpointer         user_data,GtkButton       *button)
{
	GList *lista;

	lista=g_list_first(GTK_CLIST(get_widget(dlg_distribuzione,"clist_distribuzione"))->selection);

	if(lista==NULL){
		/*
		* nessuna elaborazione della lista e' stata selezionata
		*/
		GtkWidget *lb_msg;
		GtkWidget *pb_ok;

		dlg_message=create_dlg_message();

		lb_msg=get_widget(dlg_message,"lb_msg");
		pb_ok=get_widget(dlg_message,"pb_ok");
		gtk_label_printf(lb_msg,"NESSUNA ELABORAZIONE SELEZIONATA !");
		gtk_window_set_title (GTK_WINDOW (dlg_message), "Lancio Elaborazioni");
		gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
		gtk_signal_connect (GTK_OBJECT (pb_ok), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_pb_ok_clicked), dlg_message);
		gtk_widget_show(dlg_message);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));

	} else {
		char szSqlCmd[1024];
		char *pszNumeroElab;
		char *pPtr;
		int nRowIndex;
		GtkWidget *lst;
		PGresult *PGRes = NULL;
		int nOrdiniTotali;

		sprintf(szSqlCmd,"select rostato as \"S\",ronelab as \"ELABORAZIONE\",ordprog as \"CODICE\",rodscli as \"RAGIONE SOCIALE\",roprcli as \"PRV\",\
											ronmcll as \"COLLI\",ronmrgh as \"RIGHE\",ronmcpe as \"COPIE\",ropspre::float4/1000 as \"PESO PRE.(Kg)\" from ric_ord where ronelab in (");
		do {
			nRowIndex=GPOINTER_TO_INT(lista->data);
			gtk_clist_get_text(GTK_CLIST(get_widget(dlg_distribuzione,"clist_distribuzione")),nRowIndex,0,&pszNumeroElab);
			strcat(szSqlCmd,"'");
			strcat(szSqlCmd,pszNumeroElab);
			strcat(szSqlCmd,"', ");
		} while((lista=g_list_next(lista)));
		pPtr=strrchr(szSqlCmd,',');
		*pPtr='\0';
		strcat(szSqlCmd,") and (rostato='S' or rostato=' ' or rostato is NULL) order by ordprog;");

		PGRes=PGExecSQL(FALSE,szSqlCmd);
		nOrdiniTotali=PQntuples(PGRes);
		PQclear(PGRes);

		g_list_free(lista);
		
		dlg_lancio_distribuzioni=create_dlg_lancio_distribuzioni();
	  gtk_label_printf(get_widget(dlg_lancio_distribuzioni,"lb_message"),"PREMERE OK PER CONFERMARE IL LANCIO E L'EVASIONE DEL SEGUENTE GRUPPO DI ORDINI [%d]",nOrdiniTotali);
		CreateListFromSelect(dlg_lancio_distribuzioni,get_widget(dlg_lancio_distribuzioni,"sw_lancio_distribuzioni"),&lst,"lst_lancio_distribuzioni",GTK_SELECTION_SINGLE,szSqlCmd,NULL,0);
		gtk_widget_show(dlg_lancio_distribuzioni);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_lancio_distribuzioni),GTK_WINDOW(main_window));
		gtk_window_set_transient_for(GTK_WINDOW(dlg_lancio_distribuzioni),GTK_WINDOW(dlg_distribuzione));
		gtk_signal_connect (GTK_OBJECT(get_widget(dlg_lancio_distribuzioni,"pb_ok")), "clicked", GTK_SIGNAL_FUNC (do_lancio_consuntivo_elaborazione), user_data);
	}

}

void on_dlg_lancio_evasione_pb_lancio_clicked (gpointer         user_data, GtkButton       *button)
{
	GList *lista;

	lista=g_list_first(GTK_CLIST(get_widget(dlg_distribuzione,"clist_distribuzione"))->selection);

	if(lista==NULL){
		/*
		* nessuna elaborazione della lista e' stata selezionata
		*/
		GtkWidget *lb_msg;
		GtkWidget *pb_ok;

		dlg_message=create_dlg_message();

		lb_msg=get_widget(dlg_message,"lb_msg");
		pb_ok=get_widget(dlg_message,"pb_ok");
		gtk_label_printf(lb_msg,"NESSUNA ELABORAZIONE SELEZIONATA !");
		gtk_window_set_title (GTK_WINDOW (dlg_message), "Lancio Elaborazioni");
		gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
		gtk_signal_connect (GTK_OBJECT (pb_ok), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_pb_ok_clicked), dlg_message);
		gtk_widget_show(dlg_message);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));

	} else {
		char szSqlCmd[1024];
		char *pszNumeroElab;
		char *pPtr;
		int nRowIndex;
		GtkWidget *lst;
		PGresult *PGRes = NULL;
		int nOrdiniTotali;

		sprintf(szSqlCmd,"select rostato as \"S\",ronelab as \"ELABORAZIONE\",ordprog as \"CODICE\",rodscli as \"RAGIONE SOCIALE\",roprcli as \"PRV\",\
											ronmcll as \"COLLI\",ronmrgh as \"RIGHE\",ronmcpe as \"COPIE\",ropspre::float4/1000 as \"PESO PRE.(Kg)\" from ric_ord where ronelab in (");
		do {
			nRowIndex=GPOINTER_TO_INT(lista->data);
			gtk_clist_get_text(GTK_CLIST(get_widget(dlg_distribuzione,"clist_distribuzione")),nRowIndex,0,&pszNumeroElab);
			strcat(szSqlCmd,"'");
			strcat(szSqlCmd,pszNumeroElab);
			strcat(szSqlCmd,"', ");
		} while((lista=g_list_next(lista)));
		pPtr=strrchr(szSqlCmd,',');
		*pPtr='\0';
		strcat(szSqlCmd,") and (rostato='S' or rostato=' ' or rostato is NULL) order by ordprog;");

		PGRes=PGExecSQL(FALSE,szSqlCmd);
		nOrdiniTotali=PQntuples(PGRes);
		PQclear(PGRes);

		g_list_free(lista);
		
		dlg_lancio_distribuzioni=create_dlg_lancio_distribuzioni();
	  gtk_label_printf(get_widget(dlg_lancio_distribuzioni,"lb_message"),"PREMERE OK PER CONFERMARE IL LANCIO E L'EVASIONE DEL SEGUENTE GRUPPO DI ORDINI [%d]",nOrdiniTotali);
		CreateListFromSelect(dlg_lancio_distribuzioni,get_widget(dlg_lancio_distribuzioni,"sw_lancio_distribuzioni"),&lst,"lst_lancio_distribuzioni",GTK_SELECTION_SINGLE,szSqlCmd,NULL,0);
		gtk_widget_show(dlg_lancio_distribuzioni);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_lancio_distribuzioni),GTK_WINDOW(main_window));
		gtk_window_set_transient_for(GTK_WINDOW(dlg_lancio_distribuzioni),GTK_WINDOW(dlg_distribuzione));
		gtk_signal_connect (GTK_OBJECT(get_widget(dlg_lancio_distribuzioni,"pb_ok")), "clicked", GTK_SIGNAL_FUNC (do_lancio_evasione_elaborazione), user_data);
	}

}


void on_dlg_distribuzione_pb_lancia_evadi_clicked (GtkButton       *button, gpointer         user_data)
{
	GtkWidget *dlg_lancio_evasione = create_dlg_lancio_evasione();

	gtk_widget_show(dlg_lancio_evasione);
	gtk_window_set_transient_for(GTK_WINDOW(dlg_lancio_evasione),GTK_WINDOW(main_window));
	gtk_window_set_transient_for(GTK_WINDOW(dlg_lancio_evasione),GTK_WINDOW(dlg_distribuzione));
}

void on_dlg_distribuzione_pb_lancia_consuntivo_clicked (GtkButton       *button, gpointer         user_data)
{
	GtkWidget *dlg_lancio_consuntivo = create_dlg_lancio_consuntivo();

	gtk_widget_show(dlg_lancio_consuntivo);
	gtk_window_set_transient_for(GTK_WINDOW(dlg_lancio_consuntivo),GTK_WINDOW(main_window));
	gtk_window_set_transient_for(GTK_WINDOW(dlg_lancio_consuntivo),GTK_WINDOW(dlg_distribuzione));
}



void on_dlg_prodotti_pb_modifica_pr_clicked (gpointer user_data,GtkButton *button)
{
	PGresult *PGRes;
	char szCodProd[40];
	char szSqlCmd[2048];
	GtkWidget *dlg_prodotti=GTK_WIDGET(user_data);

	strcpy(szCodProd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_prodotti,"entry_pr_codice"))));
	StrTrimAll(szCodProd);

	if(strlen(szCodProd)!=0){
		strcpy(szSqlCmd,"update articoli set prdescr='"); strcat(szSqlCmd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_prodotti,"entry_pr_descrizione"))));
		strcat(szSqlCmd,"', prlungh='"); strcat(szSqlCmd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_prodotti,"entry_pr_lunghezza"))));
		strcat(szSqlCmd,"', prlargh='"); strcat(szSqlCmd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_prodotti,"entry_pr_larghezza"))));
		strcat(szSqlCmd,"', praltez='"); strcat(szSqlCmd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_prodotti,"entry_pr_altezza"))));
		strcat(szSqlCmd,"', prpesgr='"); strcat(szSqlCmd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_prodotti,"entry_pr_peso"))));
		strcat(szSqlCmd,"', prqtkar='"); strcat(szSqlCmd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_prodotti,"entry_pr_qtkar"))));
		strcat(szSqlCmd,"', prflgpp="); strcat(szSqlCmd,GTK_TOGGLE_BUTTON(get_widget(dlg_prodotti,"cb_prodotto_in_piedi"))->active?"1":"0");
		strcat(szSqlCmd," , prswbri='"); strcat(szSqlCmd,GTK_TOGGLE_BUTTON(get_widget(dlg_prodotti,"cb_controllo_barcode_id"))->active?"C":" ");
		strcat(szSqlCmd,"', prswpsp='"); strcat(szSqlCmd,GTK_TOGGLE_BUTTON(get_widget(dlg_prodotti,"cb_imballo_speciale"))->active?"S":" ");
		strcat(szSqlCmd,"' where prcdpro='"); strcat(szSqlCmd,szCodProd);
		strcat(szSqlCmd,"';");
		
		PGRes=PGExecSQL(Cfg.nDebugVersion,szSqlCmd);
		if (PQresultStatus(PGRes) == PGRES_COMMAND_OK ) {
	#ifdef TRACE
			trace_out_vstr_date(1,"Modificato prodotto [%s]",szCodProd);
	#endif
		} else {
	#ifdef TRACE
			trace_out_vstr_date(1,"[%s]",szSqlCmd);
			trace_out_vstr_date(1,"Fallita modifica su prodotto [%s]",szCodProd);
	#endif
		}
		PQclear(PGRes);

		RefreshListaProdotti(dlg_prodotti);
	} else {
		/*
		* nessuna riga selezionata
		*/
		GtkWidget *lb_msg;
		GtkWidget *pb_ok;

		dlg_message=create_dlg_message();
		lb_msg=get_widget(dlg_message,"lb_msg");
		pb_ok=get_widget(dlg_message,"pb_ok");

		gtk_label_printf(lb_msg,"NESSUN PRODOTTO SELEZIONATO");
		gtk_window_set_title (GTK_WINDOW (dlg_message), "Modifica Prodotto");
		gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
		gtk_widget_show(dlg_message);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));
		gtk_signal_connect (GTK_OBJECT (pb_ok), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_pb_ok_clicked), dlg_message);
	}

}


void on_dlg_prodotti_pb_aggiungi_pr_clicked (gpointer user_data,GtkButton *button)
{
	PGresult *PGRes;
	char szCodProd[40];
	char szSqlCmd[1024];
	GtkWidget *dlg_prodotti=GTK_WIDGET(user_data);

	strcpy(szCodProd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_prodotti,"entry_pr_codice"))));
	StrTrimAll(szCodProd);
	if(strlen(szCodProd)!=0){

		sprintf(szSqlCmd,"select prcdpro from articoli where prcdpro='%s';",szCodProd);
		PGRes=PGExecSQL(Cfg.nDebugVersion,szSqlCmd);
		if (PQntuples(PGRes)!=0){
			/*
			* codice prodotto gia' presente
			*/
			GtkWidget *lb_msg;
			GtkWidget *pb_ok;

			dlg_message=create_dlg_message();
			lb_msg=get_widget(dlg_message,"lb_msg");
			pb_ok=get_widget(dlg_message,"pb_ok");

			PQclear(PGRes);

			gtk_label_printf(lb_msg,"CODICE PRODOTTO\nGIA' PRESENTE");
			gtk_window_set_title (GTK_WINDOW (dlg_message), "Aggiungi Prodotto");
			gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
			gtk_widget_show(dlg_message);
			gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));
			gtk_signal_connect (GTK_OBJECT (pb_ok), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_pb_ok_clicked), dlg_message);
		} else {
			PQclear(PGRes);
			strcpy(szSqlCmd,"insert into articoli ( \
				prcdpro , prdescr , prlungh , prlargh , \
				praltez , prswalt , prpesgr , prqtkar , \
				prflgpp , prswbri , prswpsp ,) values ('");
			strcat(szSqlCmd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_prodotti,"entry_pr_codice")))); strcat(szSqlCmd,"', '");
			strcat(szSqlCmd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_prodotti,"entry_pr_descrizione")))); strcat(szSqlCmd,"', '");
			strcat(szSqlCmd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_prodotti,"entry_pr_lunghezza")))); strcat(szSqlCmd,"', '");
			strcat(szSqlCmd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_prodotti,"entry_pr_larghezza")))); strcat(szSqlCmd,"', '");
			strcat(szSqlCmd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_prodotti,"entry_pr_altezza")))); strcat(szSqlCmd,"', '");
			strcat(szSqlCmd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_prodotti,"entry_pr_peso")))); strcat(szSqlCmd,"', ");
			strcat(szSqlCmd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_prodotti,"entry_pr_qtkar")))); strcat(szSqlCmd,"', ");
			strcat(szSqlCmd,GTK_TOGGLE_BUTTON(get_widget(dlg_prodotti,"cb_prodotto_in_piedi"))->active?"1":"0");strcat(szSqlCmd,", '");
			strcat(szSqlCmd,GTK_TOGGLE_BUTTON(get_widget(dlg_prodotti,"cb_controllo_barcode_id"))->active?"C":" ");strcat(szSqlCmd,", '");
			strcat(szSqlCmd,GTK_TOGGLE_BUTTON(get_widget(dlg_prodotti,"cb_imballo_speciale"))->active?"S":" ");strcat(szSqlCmd,"');");
			
			PGRes=PGExecSQL(Cfg.nDebugVersion,szSqlCmd);
			if (PQresultStatus(PGRes) == PGRES_COMMAND_OK ) {
#ifdef TRACE
				trace_out_vstr_date(1,"Inserito prodotto [%s]",gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_prodotti,"entry_pr_codice"))));
#endif
			} else {
#ifdef TRACE
				trace_out_vstr_date(1,"Fallito inserimento prodotto [%s]",gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_prodotti,"entry_pr_codice"))));
#endif
			}
			PQclear(PGRes);

			RefreshListaProdotti(dlg_prodotti);
		}

	} else {
		/*
		* nessun prodotto selezionato
		*/
		GtkWidget *lb_msg;
		GtkWidget *pb_ok;

		dlg_message=create_dlg_message();
		lb_msg=get_widget(dlg_message,"lb_msg");
		pb_ok=get_widget(dlg_message,"pb_ok");

		gtk_label_printf(lb_msg,"INSERIRE IL CODICE PRODOTTO");
		gtk_window_set_title (GTK_WINDOW (dlg_message), "Aggiungi Prodotto");
		gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
		gtk_widget_show(dlg_message);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));
		gtk_signal_connect (GTK_OBJECT (pb_ok), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_pb_ok_clicked), dlg_message);
	}

}


void on_dlg_prodotti_pb_elimina_pr_clicked  (gpointer user_data,GtkButton *button)
{
	GtkWidget *lb_msg;
	GtkWidget *pb_ok;
	char szCodProd[40];
	GtkWidget *dlg_prodotti=GTK_WIDGET(user_data);

	dlg_message=create_dlg_message();

	lb_msg=get_widget(dlg_message,"lb_msg");
	pb_ok=get_widget(dlg_message,"pb_ok");
	strcpy(szCodProd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_prodotti,"entry_pr_codice"))));
	StrTrimAll(szCodProd);

	if (strlen(szCodProd)!=0){
		gtk_label_printf(lb_msg,"CONFERMI L'ELIMINAZIONE\nDEL PRODOTTO %s",szCodProd);
		gtk_window_set_title (GTK_WINDOW (dlg_message), "Elimina Prodotto");
		gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
		gtk_widget_show(dlg_message);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));
		/*
		* callback per pressione pulsante
		*/
		gtk_signal_connect (GTK_OBJECT (pb_ok), "clicked", GTK_SIGNAL_FUNC (do_elimina_prodotto), dlg_prodotti);
	} else {
		gtk_label_printf(lb_msg,"NESSUN PRODOTTO SELEZIONATO");
		gtk_window_set_title (GTK_WINDOW (dlg_message), "Elimina Prodotto");
		gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
		gtk_widget_show(dlg_message);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));
		gtk_signal_connect (GTK_OBJECT (pb_ok), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_pb_ok_clicked), dlg_message);
	}

}



/*
* 
*/
void do_elimina_prodotto (GtkButton *button, gpointer user_data)
{
	PGresult *PGRes;
	char *pszText;
	GtkWidget *dlg_prodotti=GTK_WIDGET(user_data);
	GtkWidget *lst_prodotti=get_widget(dlg_prodotti,"lst_prodotti");
	char szCodProd[128];
	int nRiga,nIndex;
	
	strcpy(szCodProd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_prodotti,"entry_pr_codice"))));

	gtk_widget_destroy(dlg_message);

	PGRes=PGExecSQL(Cfg.nDebugVersion,"delete from articoli where rpcdpro='%s';",szCodProd);
	if (PQresultStatus(PGRes) == PGRES_COMMAND_OK ) {
#ifdef TRACE
		trace_out_vstr_date(1,"Eliminato prodotto [%s]",szCodProd);
#endif
		
		nIndex=0;
		nRiga=-1;
		while(gtk_clist_get_text(GTK_CLIST(lst_prodotti),nIndex,0,&pszText)){
			if(!strcmp(szCodProd,pszText)){
				nRiga=nIndex;
			}
			nIndex++;
		}
		if(nRiga!=-1){
			gtk_clist_moveto(GTK_CLIST(lst_prodotti), nRiga, -1, 0.5, 0);
			gtk_clist_select_row(GTK_CLIST(lst_prodotti), nRiga, -1);
			gtk_clist_remove(GTK_CLIST(lst_prodotti), nRiga);
		}
	} else {
#ifdef TRACE
		trace_out_vstr_date(1,"Fallita eliminazione prodotto [%s]",szCodProd);
#endif
	}
	PQclear(PGRes);
}

/*
* 
*/
void do_elimina_prodotto_pericoloso (GtkButton *button, gpointer user_data)
{
	PGresult *PGRes;
	char *pszText;
	GtkWidget *dlg=GTK_WIDGET(user_data);
	GtkWidget *lst=get_widget(dlg,"lst_prodotti_pericolosi");
	char szCodProd[128];
	int nRiga,nIndex;
	
	strcpy(szCodProd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg,"entry_ppcdpro"))));
	StrTrimAll(szCodProd);

	gtk_widget_destroy(dlg_message);

	PGRes=PGExecSQL(Cfg.nDebugVersion,"delete from prodotti_pericolosi where ppcdpro='%s';",szCodProd);
	if (PQresultStatus(PGRes) == PGRES_COMMAND_OK ) {
#ifdef TRACE
		trace_out_vstr_date(1,"Eliminato prodotto [%s]",szCodProd);
#endif
		
		nIndex=0;
		nRiga=-1;
		while(gtk_clist_get_text(GTK_CLIST(lst),nIndex,0,&pszText)){
			if(!strcmp(szCodProd,StrTrimAll(pszText))){
				nRiga=nIndex;
			}
			nIndex++;
		}
		if(nRiga!=-1){
			gtk_clist_moveto(GTK_CLIST(lst), nRiga, -1, 0.5, 0);
			gtk_clist_select_row(GTK_CLIST(lst), nRiga, -1);
			gtk_clist_remove(GTK_CLIST(lst), nRiga);
		}
	} else {
#ifdef TRACE
		trace_out_vstr_date(1,"Fallita eliminazione prodotto [%s]",szCodProd);
#endif
	}
	PQclear(PGRes);
}

void on_dlg_prodotti_pb_cerca_pr_clicked    (gpointer user_data,GtkButton *button)
{
	char *pszText;
	GtkWidget *dlg_prodotti=GTK_WIDGET(user_data);
	GtkWidget *lst_prodotti=get_widget(dlg_prodotti,"lst_prodotti");
	char szCodProd[128];
	int nRiga,nIndex;
	
	strcpy(szCodProd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg_prodotti,"entry_pr_codice"))));
	StrTrimAll(szCodProd);

	nIndex=0;
	nRiga=-1;
	while(gtk_clist_get_text(GTK_CLIST(lst_prodotti),nIndex,0,&pszText)){
		if(!strcmp(szCodProd,StrTrimAll(pszText))){
			nRiga=nIndex;
		}
		nIndex++;
	}
	if(nRiga!=-1){
		gtk_clist_moveto(GTK_CLIST(lst_prodotti), nRiga, -1, 0.5, 0);
		gtk_clist_select_row(GTK_CLIST(lst_prodotti), nRiga, -1);
	} else {
		GtkWidget *dlg_message=create_dlg_message();
		GtkWidget *lb_msg=get_widget(dlg_message,"lb_msg");
		GtkWidget *pb_ok=get_widget(dlg_message,"pb_ok");

		gtk_label_printf(lb_msg,"NESSUN PRODOTTO TROVATO");
		gtk_window_set_title (GTK_WINDOW (dlg_message), "Ricerca Prodotto");
		gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
		gtk_widget_show(dlg_message);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));
		gtk_signal_connect (GTK_OBJECT (pb_ok), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_pb_ok_clicked), dlg_message);
	}

}


void on_pbm_scarto_ordini_activate(GtkMenuItem *menuitem, gpointer user_data)
{
	GList *lista;
	char *pszOrdineKey;
	char szOrdineKey[128];
	int nRowIndex;
	int nOrdini=0;
	char szStato[80];

	sprintf(szStato,"%c",ORDINE_DA_SCARTARE);

	lista=g_list_first(GTK_CLIST(lst_ordini)->selection);
	if(lista==NULL){
		return;
	}
	do {
		nRowIndex=GPOINTER_TO_INT(lista->data);
		gtk_clist_get_text(GTK_CLIST(lst_ordini),nRowIndex,Cfg.nOrdiniKeyField,&pszOrdineKey);
		strcpy(szOrdineKey,pszOrdineKey);

		if(!ScartaOrdine(szOrdineKey,szStato)){
			gtk_text_printf(NULL,txt_msgs,"Predisposto allo scarto in bilancia ordine [%s]\n",szOrdineKey);
		} else {
			nOrdini++;
			//gtk_clist_set_text(GTK_CLIST(lst_ordini),nRowIndex,Cfg.nOrdiniStatusField,szStato);
		}
	} while((lista=g_list_next(lista)));
	gtk_text_printf(NULL,txt_msgs,"Predisposti allo scarto [%d] ordini\n",nOrdini);
}


void on_pbm_invio_dati_activate(GtkMenuItem *menuitem, gpointer user_data)
{
	GList *lista;
	GtkWidget *lb_msg;
	GtkWidget *pb_ok;

	dlg_message=create_dlg_message();

	gtk_window_set_title (GTK_WINDOW (dlg_message), "Invio dati A SAP");
	lb_msg=get_widget(dlg_message,"lb_msg");
	pb_ok=get_widget(dlg_message,"pb_ok");

	lista=g_list_first(GTK_CLIST(lst_ordini)->selection);

	if(lista==NULL){
		/*
		* nessun ordine della lista e' selezionato
		*/
		gtk_label_printf(lb_msg,"NESSUN ORDINE SELEZIONATO");
		gtk_signal_connect (GTK_OBJECT (pb_ok), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_pb_ok_clicked), dlg_message);
	} else {
		gtk_label_printf(lb_msg,"CONFERMI LA SPEDIZIONE A SAP\nDEGLI ORDINI SELEZIONATI?");
		gtk_signal_connect (GTK_OBJECT (pb_ok), "clicked", GTK_SIGNAL_FUNC (do_invio_dati_to_sap), dlg_message);
	}

	gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
	gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));
	gtk_widget_show(dlg_message);
}


void do_invio_dati_to_sap (GtkButton *button, gpointer user_data)
{
	BOOL bOrdineSpedito=FALSE;
	BOOL bOrdineEvaso=FALSE;
	BOOL bOrdineFittizio=FALSE;
	BOOL bRetVal=TRUE;
	GList *lista;

	lista=g_list_first(GTK_CLIST(lst_ordini)->selection);

	if(lista!=NULL){
		char *pszOrdProg;
		char szOrdProg[128];
		char szStato[64];
		char szROFLFTZ[64];
		int nRowIndex;
		int nOrdiniSpediti=0;
		int nOrdiniSelezionati=0;
		int nRC;
		char szCommand[512];
		PGresult *PGResOrdini;
		PGresult *PGResColli;
		PGresult *PGRes;

		FILE *fpOrdini=NULL;
		FILE *fpColli=NULL;

		char szOrdiniFile[128];
		char szColliFile[128];

		sprintf(szOrdiniFile,"%s/%s",Cfg.szPathExport,StrTrimAll(Cfg.szExpOrdiniFile));
		sprintf(szColliFile,"%s/%s",Cfg.szPathExport,StrTrimAll(Cfg.szExpColliFile));

		if((fpOrdini=fopen(szOrdiniFile,"a"))==NULL){
#ifdef TRACE
			trace_out_vstr_date(1,"SpedizioneCedola() : Errore in apertura file ordini [%s]",szOrdiniFile);
#endif
			gtk_text_printf("CYAN",txt_msgs,"SpedizioneCedola() : Errore in apertura file ordini [%s]\n",szOrdiniFile);
			bRetVal=FALSE;
		}
		if((fpColli =fopen(szColliFile,"a"))==NULL){
#ifdef TRACE
			trace_out_vstr_date(1,"SpedizioneCedola() : Errore in apertura file colli [%s]",szColliFile);
#endif
			gtk_text_printf("CYAN",txt_msgs,"SpedizioneCedola() : Errore in apertura file colli [%s]\n",szColliFile);
			bRetVal=FALSE;
		}

		if(bRetVal){
			bRetVal=FALSE;

			do {
				szStato[0]='\0';

				/* seleziono il codice e lo stato dell'ordine */
				nRowIndex=GPOINTER_TO_INT(lista->data);
				gtk_clist_get_text(GTK_CLIST(lst_ordini),nRowIndex,Cfg.nOrdiniKeyField,&pszOrdProg);
				strcpy(szOrdProg,pszOrdProg);

				nOrdiniSelezionati++;

				/* ricavo lo stato e la "fittiziosità" dell'ordine */
				PGRes=PGExecSQL(Cfg.nDebugVersion,"select rostato,roflftz from ric_ord where ordprog='%s';",szOrdProg);
				if(PQresultStatus(PGRes)==PGRES_TUPLES_OK && PQntuples(PGRes)){
					strcpy(szStato,PQgetvalue(PGRes,0,0));
					strcpy(szROFLFTZ,PQgetvalue(PGRes,0,1));
					switch(szROFLFTZ[0]){
						case ORDINE_FITTIZIO:
							bOrdineFittizio=TRUE;
						break;
						default:
							bOrdineFittizio=FALSE;
						break;
					}
					switch(szStato[0]){
						case ORDINE_SPEDITO_HOST:
							bOrdineSpedito=TRUE;
							bRetVal=TRUE;
						break;
						case ORDINE_EVASO:
							bOrdineEvaso=TRUE;
							bRetVal=TRUE;
						break;
						default:
#ifdef TRACE
							trace_out_vstr_date(1,"Impossibile Spedire Ordine %s -> stato %c",szOrdProg,szStato[0]);
#endif
							gtk_text_printf("RED",txt_msgs,"Impossibile Spedire Ordine %s -> stato %c",szOrdProg,szStato[0]);
						break;
					}
				}
				PQclear(PGRes);

				if(bOrdineEvaso || bOrdineSpedito){
					int nIndexColli;
					PGresult *PGResUpdate;

					/* seleziono i dati dell'ordine da spedire */
					PGResOrdini=PGExecSQL(Cfg.nDebugVersion,"select ordprog,ronmcll,ropsrea,ronelab from ric_ord where ordprog='%s';",szOrdProg);	
					/* selezioni i colli relativi all'ordine */
					PGResColli=PGExecSQL(Cfg.nDebugVersion>1,"select ordprog,cptpfor,cpprgcl from col_prod where cpstato='%c' AND ordprog='%s' order by ordkey,cpnmcol;",szStato[0],szOrdProg);	

					if((PQntuples(PGResColli)>0) && (PQresultStatus(PGResOrdini)==PGRES_TUPLES_OK)){
						for (nIndexColli=0;nIndexColli<PQntuples(PGResColli);nIndexColli++){
							/*
							* spedizione a Host dei colli relativi all'ordine
							*/
							if (bOrdineFittizio){
							} else {
								WriteAsciiRecord(fpColli, &tColCni,PGResColli,nIndexColli);
							}
						}

						/* spedizione dell'ordine a Host */
						if (bOrdineFittizio){
						} else {
							WriteAsciiRecord(fpOrdini, &tOrdCni,PGResOrdini,0);
						}
						nOrdiniSpediti++;

						/*
						* se l'ordine e' evaso (non e' ancora stato spedito) -> cambio lo stato dei colli/righe in H (spedito a HOST)
						*/
						if(bOrdineEvaso){
							/* cambio lo stato dei colli appartenenti all'ordine (E -> H) */
							PGResUpdate=PGExecSQL(Cfg.nDebugVersion>1,"update col_prod set cpstato='%c' where ordprog='%s';", COLLO_SPEDITO_HOST, szOrdProg);
							PQclear(PGResUpdate);

							/* cambio lo stato delle righe appartenenti all'ordine (E -> H) */
							PGResUpdate=PGExecSQL(Cfg.nDebugVersion>1,"update rig_prod set rpstato='%c' where ordprog='%s';", ORDINE_SPEDITO_HOST, szOrdProg);
							PQclear(PGResUpdate);

							/* cambio lo stato dell'ordine (E -> H) */
							PGResUpdate=PGExecSQL(Cfg.nDebugVersion>1,"update ric_ord set rostato='%c' where ordprog='%s';", ORDINE_SPEDITO_HOST, szOrdProg);
							PQclear(PGResUpdate);

							/* cambio lo stato dell'ordine (E -> H) */
							PGResUpdate=PGExecSQL(Cfg.nDebugVersion>1,"update ord_prod set opstato='%c' where ordprog='%s';", ORDINE_SPEDITO_HOST, szOrdProg);
							PQclear(PGResUpdate);

							/* update ordine a video */
							UpdateOrdine(szOrdProg);
						}
					}

					PQclear(PGResOrdini);
					PQclear(PGResColli);
				}
			} while((lista=g_list_next(lista)));

			fclose(fpOrdini);
			fclose(fpColli);

			if(bRetVal){
				sprintf(szCommand,"%s/cni2sap %s %s > %s/cni2sap.err",Cfg.szPathExe,szOrdiniFile,szColliFile,Cfg.szPathExport);
#ifdef TRACE
				trace_out_vstr_date(1,"ESEGUO %s",szCommand);
#endif

				nRC=system(szCommand);

#ifdef TRACE
				trace_out_vstr_date(1,"ESEGUITO %s",szCommand);
#endif

				if(nRC==0){

					sprintf(szCommand,"mv -f %s %s.old",szOrdiniFile,szOrdiniFile);
					system(szCommand);

					sprintf(szCommand,"mv -f %s %s.old",szColliFile,szColliFile);
					system(szCommand);

					gtk_text_printf(NULL,txt_msgs,"Spediti a SAP %d Ordini su %d selezionati\n",nOrdiniSpediti,nOrdiniSelezionati);
#ifdef TRACE
					trace_out_vstr_date(1,"Spediti a SAP %d Ordini su %d selezionati",nOrdiniSpediti,nOrdiniSelezionati);
#endif
				} else {
					/* rinomimo i files di export in .err per offrire la possibilita' di rispedire i dati */
					sprintf(szCommand,"mv -f %s %s.err",szOrdiniFile,szOrdiniFile);
					system(szCommand);

					sprintf(szCommand,"mv -f %s %s.err",szColliFile,szColliFile);
					system(szCommand);

#ifdef TRACE
					trace_out_vstr_date(1,"Errore [%d] in esecuzione comando cni2sap",nRC);
#endif
					gtk_text_printf("RED",txt_msgs,"Errore [%d] in esecuzione comando cni2sap\nContattare il responsabile SAP\ned eventualmente reinviare i dati\n",nRC);
				}
			}


		} else {
#ifdef TRACE
			trace_out_vstr_date(1,"Errore in apertura Files di Export Dati");
#endif
			gtk_text_printf("RED",txt_msgs,"Errore in apertura Files di Export Dati\n");
		}
	} else {
#ifdef TRACE
		trace_out_vstr_date(1,"Nessun Ordine Selezionato a video");
#endif
		gtk_text_printf("RED",txt_msgs,"Nessun Ordine Selezionato a video\n");
	}

	gtk_widget_destroy(dlg_message);
}


void on_pbm_crea_dischetto_activate (GtkMenuItem *menuitem, gpointer user_data)
{
	GtkWidget *lb_msg;
	GtkWidget *pb_ok;
	BOOL bRetVal=TRUE;

	FILE *fpOrdini=NULL;
	FILE *fpColli=NULL;

	char szOrdiniFile[128];
	char szColliFile[128];

	dlg_message=create_dlg_message();

	gtk_window_set_title (GTK_WINDOW (dlg_message), "Copia dati SAP su floppy");
	lb_msg=get_widget(dlg_message,"lb_msg");
	pb_ok=get_widget(dlg_message,"pb_ok");

	sprintf(szOrdiniFile,"%s/%s.err",Cfg.szPathExport,StrTrimAll(Cfg.szExpOrdiniFile));
	sprintf(szColliFile,"%s/%s.err",Cfg.szPathExport,StrTrimAll(Cfg.szExpColliFile));

	if((fpOrdini=fopen(szOrdiniFile,"r"))==NULL){
#ifdef TRACE
		trace_out_vstr_date(1,"on_pbm_crea_dischetto_activate: Errore in apertura file ordini [%s]",szOrdiniFile);
#endif
		bRetVal=FALSE;
	} else {
		fclose(fpOrdini);
	}

	if((fpColli =fopen(szColliFile,"r"))==NULL){
#ifdef TRACE
		trace_out_vstr_date(1,"on_pbm_crea_dischetto_activate: Errore in apertura file colli [%s]",szColliFile);
#endif
		bRetVal=FALSE;
	} else {
		fclose(fpColli);
	}

	if(!bRetVal){
		/*
		* non sono presenti i files di export (ordini e colli)
		*/
		gtk_label_printf(lb_msg,"Non Sono presenti i files di export da copiare sul floppy");
		gtk_signal_connect (GTK_OBJECT (pb_ok), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_pb_ok_clicked), dlg_message);
	} else {
		gtk_label_printf(lb_msg,"CONFERMI LA COPIA DEI DATI SUL FLOPPY?\nASSICURARSI CHE SIA PRESENTE IL FLOPPY NEL DRIVE A");
		gtk_signal_connect (GTK_OBJECT (pb_ok), "clicked", GTK_SIGNAL_FUNC (do_copia_dati_to_floppy_for_sap), dlg_message);
	}

	gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
	gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));
	gtk_widget_show(dlg_message);
}

void do_copia_dati_to_floppy_for_sap(GtkButton *button, gpointer user_data)
{
	BOOL bRetVal=TRUE;
	int nRC;
	char szCommand[512];

	FILE *fpOrdini=NULL;
	FILE *fpColli=NULL;

	char szOrdiniFile[128];
	char szColliFile[128];
	char szOrdiniFileErr[128];
	char szColliFileErr[128];

	sprintf(szOrdiniFileErr,"%s/%s.err",Cfg.szPathExport,StrTrimAll(Cfg.szExpOrdiniFile));
	sprintf(szColliFileErr,"%s/%s.err",Cfg.szPathExport,StrTrimAll(Cfg.szExpColliFile));

	sprintf(szOrdiniFile,"%s/%s",Cfg.szPathExport,StrTrimAll(Cfg.szExpOrdiniFile));
	sprintf(szColliFile,"%s/%s",Cfg.szPathExport,StrTrimAll(Cfg.szExpColliFile));

	if((fpOrdini=fopen(szOrdiniFileErr,"r"))==NULL){
#ifdef TRACE
		trace_out_vstr_date(1,"do_copia_dati_to_floppy_for_sap: Errore in apertura file ordini [%s]",szOrdiniFile);
#endif
		gtk_text_printf("CYAN",txt_msgs,"Errore in apertura file ordini [%s]\n",szOrdiniFile);
		bRetVal=FALSE;
	}
	if((fpColli =fopen(szColliFileErr,"r"))==NULL){
#ifdef TRACE
		trace_out_vstr_date(1,"do_copia_dati_to_floppy_for_sap: Errore in apertura file colli [%s]",szColliFile);
#endif
		gtk_text_printf("CYAN",txt_msgs,"Errore in apertura file colli [%s]\n",szColliFile);
		bRetVal=FALSE;
	}

	if(bRetVal){

		fclose(fpOrdini);
		fclose(fpColli);

		sprintf(szCommand,"mcopy %s a:%s",szOrdiniFileErr,StrTrimAll(Cfg.szExpOrdiniFile));
#ifdef TRACE
		trace_out_vstr_date(1,"ESEGUO %s",szCommand);
#endif
		nRC=system(szCommand);

		sprintf(szCommand,"mcopy %s a:%s",szColliFileErr,StrTrimAll(Cfg.szExpColliFile));
#ifdef TRACE
		trace_out_vstr_date(1,"ESEGUO %s",szCommand);
#endif
		nRC=system(szCommand);

		if(nRC==0){

			/* rinomino i files di errore in .old */
			sprintf(szCommand,"mv -f %s %s.old",szOrdiniFileErr,szOrdiniFile);
			system(szCommand);

			sprintf(szCommand,"mv -f %s %s.old",szColliFileErr,szColliFile);
			system(szCommand);

			gtk_text_printf(NULL,txt_msgs,"copia dati effettuata con successo\n");
#ifdef TRACE
			trace_out_vstr_date(1,"copia dati effettuata con successo");
#endif
		} else {
#ifdef TRACE
			trace_out_vstr_date(1,"copia dati su floppy fallita");
#endif
			gtk_text_printf("RED",txt_msgs,"copia dati su floppy fallita\n");
		}
	} else {
#ifdef TRACE
		trace_out_vstr_date(1,"Errore in apertura Files di Export Dati");
#endif
		gtk_text_printf("RED",txt_msgs,"Errore in apertura Files di Export Dati\n");
	}

	gtk_widget_destroy(dlg_message);
}


void on_pbm_reinvio_dati_to_sap_activate    (GtkMenuItem     *menuitem, gpointer         user_data)
{
	GtkWidget *lb_msg;
	GtkWidget *pb_ok;
	BOOL bRetVal=TRUE;

	FILE *fpOrdini=NULL;
	FILE *fpColli=NULL;

	char szOrdiniFile[128];
	char szColliFile[128];

	dlg_message=create_dlg_message();

	gtk_window_set_title (GTK_WINDOW (dlg_message), "Reinvio dati A SAP");
	lb_msg=get_widget(dlg_message,"lb_msg");
	pb_ok=get_widget(dlg_message,"pb_ok");

	sprintf(szOrdiniFile,"%s/%s.err",Cfg.szPathExport,StrTrimAll(Cfg.szExpOrdiniFile));
	sprintf(szColliFile,"%s/%s.err",Cfg.szPathExport,StrTrimAll(Cfg.szExpColliFile));

	if((fpOrdini=fopen(szOrdiniFile,"r"))==NULL){
#ifdef TRACE
		trace_out_vstr_date(1,"on_pbm_reinvio_dati_to_sap_activate: Errore in apertura file ordini [%s]",szOrdiniFile);
#endif
		bRetVal=FALSE;
	} else {
		fclose(fpOrdini);
	}

	if((fpColli =fopen(szColliFile,"r"))==NULL){
#ifdef TRACE
		trace_out_vstr_date(1,"on_pbm_reinvio_dati_to_sap_activate: Errore in apertura file colli [%s]",szColliFile);
#endif
		bRetVal=FALSE;
	} else {
		fclose(fpColli);
	}

	if(!bRetVal){
		/*
		* non sono presenti i files di export (ordini e colli)
		*/
		gtk_label_printf(lb_msg,"Non Sono presenti i files di export da reinviare a SAP");
		gtk_signal_connect (GTK_OBJECT (pb_ok), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_pb_ok_clicked), dlg_message);
	} else {
		gtk_label_printf(lb_msg,"CONFERMI LA RISPEDIZIONE DEI DATI A SAP?");
		gtk_signal_connect (GTK_OBJECT (pb_ok), "clicked", GTK_SIGNAL_FUNC (do_reinvio_dati_to_sap), dlg_message);
	}

	gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
	gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));
	gtk_widget_show(dlg_message);
}

void do_reinvio_dati_to_sap (GtkButton *button, gpointer user_data)
{
	BOOL bRetVal=TRUE;
	int nRC;
	char szCommand[512];

	FILE *fpOrdini=NULL;
	FILE *fpColli=NULL;

	char szOrdiniFile[128];
	char szColliFile[128];
	char szOrdiniFileErr[128];
	char szColliFileErr[128];

	sprintf(szOrdiniFileErr,"%s/%s.err",Cfg.szPathExport,StrTrimAll(Cfg.szExpOrdiniFile));
	sprintf(szColliFileErr,"%s/%s.err",Cfg.szPathExport,StrTrimAll(Cfg.szExpColliFile));

	sprintf(szOrdiniFile,"%s/%s",Cfg.szPathExport,StrTrimAll(Cfg.szExpOrdiniFile));
	sprintf(szColliFile,"%s/%s",Cfg.szPathExport,StrTrimAll(Cfg.szExpColliFile));

	if((fpOrdini=fopen(szOrdiniFileErr,"r"))==NULL){
#ifdef TRACE
		trace_out_vstr_date(1,"do_reinvio_dati_to_sap: Errore in apertura file ordini [%s]",szOrdiniFile);
#endif
		gtk_text_printf("CYAN",txt_msgs,"Errore in apertura file ordini [%s]\n",szOrdiniFile);
		bRetVal=FALSE;
	}
	if((fpColli =fopen(szColliFileErr,"r"))==NULL){
#ifdef TRACE
		trace_out_vstr_date(1,"do_reinvio_dati_to_sap: Errore in apertura file colli [%s]",szColliFile);
#endif
		gtk_text_printf("CYAN",txt_msgs,"Errore in apertura file colli [%s]\n",szColliFile);
		bRetVal=FALSE;
	}

	if(bRetVal){

		fclose(fpOrdini);
		fclose(fpColli);

		sprintf(szCommand,"%s/cni2sap %s %s > %s/cni2sap.err",Cfg.szPathExe,szOrdiniFileErr,szColliFileErr,Cfg.szPathExport);
#ifdef TRACE
		trace_out_vstr_date(1,"ESEGUO %s",szCommand);
#endif

		nRC=system(szCommand);

#ifdef TRACE
		trace_out_vstr_date(1,"ESEGUITO %s",szCommand);
#endif

		if(nRC==0){

			/* rinomino i files di errore in .old */
			sprintf(szCommand,"mv -f %s %s.old",szOrdiniFileErr,szOrdiniFile);
			system(szCommand);

			sprintf(szCommand,"mv -f %s %s.old",szColliFileErr,szColliFile);
			system(szCommand);

			gtk_text_printf(NULL,txt_msgs,"Reinvio dati effettuato con successo\n");
#ifdef TRACE
			trace_out_vstr_date(1,"Reinvio dati effettuato con successo");
#endif
		} else {
#ifdef TRACE
			trace_out_vstr_date(1,"Errore [%d] in esecuzione comando cni2sap",nRC);
#endif
			gtk_text_printf("RED",txt_msgs,"Errore [%d] in esecuzione comando cni2sap\nContattare il responsabile SAP\n",nRC);
		}
	} else {
#ifdef TRACE
		trace_out_vstr_date(1,"Errore in apertura Files di Export Dati");
#endif
		gtk_text_printf("RED",txt_msgs,"Errore in apertura Files di Export Dati\n");
	}

	gtk_widget_destroy(dlg_message);
}

void do_start_impianto(void)
{
	GtkWidget *dlg= create_dlg_start_impianto();

	gtk_window_set_focus (GTK_WINDOW (dlg), get_widget(dlg,"pb_ok"));
	gtk_widget_show(dlg);
	gtk_window_set_transient_for(GTK_WINDOW(dlg),GTK_WINDOW(main_window));

	if(pDatiLinea->nStatoLinea_1==LINEA_IN_START){
		gtk_widget_set_sensitive(get_widget(dlg,"rb_isola_1"),FALSE);
	}
	if(pDatiLinea->nStatoLinea_2==LINEA_IN_START){
		gtk_widget_set_sensitive(get_widget(dlg,"rb_isola_2"),FALSE);
	}
}


void do_stop_impianto(void)
{
	GtkWidget *dlg= create_dlg_stop_impianto();

	gtk_window_set_focus (GTK_WINDOW (dlg), get_widget(dlg,"pb_ok"));
	gtk_widget_show(dlg);
	gtk_window_set_transient_for(GTK_WINDOW(dlg),GTK_WINDOW(main_window));

	if(pDatiLinea->nStatoLinea_1==LINEA_IN_STOP){
		gtk_widget_set_sensitive(get_widget(dlg,"rb_isola_1"),FALSE);
	}
	if(pDatiLinea->nStatoLinea_2==LINEA_IN_STOP){
		gtk_widget_set_sensitive(get_widget(dlg,"rb_isola_2"),FALSE);
	}

}

void on_dlg_start_impianto_pb_ok_clicked    (gpointer user_data,GtkButton *button)
{
	GtkWidget *dlg=GTK_WIDGET(user_data);

	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(get_widget(dlg,"rb_isola_1")))){
		StartLinea(1);
	}
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(get_widget(dlg,"rb_isola_2")))){
		StartLinea(2);
	}
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(get_widget(dlg,"rb_tutte")))){
		StartLinea(0);
	}
	gtk_widget_destroy(dlg);
}

void on_dlg_stop_impianto_pb_ok_clicked     (gpointer user_data,GtkButton *button)
{
	GtkWidget *dlg=GTK_WIDGET(user_data);

	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(get_widget(dlg,"rb_isola_1")))){
		StopLinea(1);
	}
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(get_widget(dlg,"rb_isola_2")))){
		StopLinea(2);
	}
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(get_widget(dlg,"rb_tutte")))){
		StopLinea(0);
	}
	gtk_widget_destroy(dlg);
}

void on_dlg_work_pb_cancel_clicked(GtkButton       *button, gpointer         user_data)
{

}

/*
* Callback di selezione riga in lista colli
*/
void on_lst_righe_collo_select_row(GtkCList *clist, gint row, gint column, GdkEvent *event, gpointer user_data)
{
	GList *lista;
	BOOL bOK=TRUE;

	/*
	* se il collo è stato selezionato a video -> riempio i campi della dlg
	*/
	lista=g_list_first(GTK_CLIST(clist)->selection);

	/*
	* visualizzo i dati del collo solo se è stata selezionata una sola riga della lista
	*/
	if((lista==NULL)){
		bOK=FALSE;
	}
	gtk_widget_set_sensitive(get_widget(dlg_edit_collo,"pb_qta_errata"),bOK);
	gtk_widget_set_sensitive(get_widget(dlg_edit_collo,"pb_mancante"),bOK);
	gtk_widget_set_sensitive(get_widget(dlg_edit_collo,"pb_inversione"),bOK);
	gtk_widget_set_sensitive(get_widget(dlg_edit_collo,"pb_riga_non_richiesta"),bOK);
	gtk_widget_set_sensitive(get_widget(dlg_edit_collo,"pb_annulla_errore"),bOK);

	nLstRigheColloRowSelected=row;
}

void on_lst_righe_collo_unselect_row(GtkCList *clist, gint row, gint column, GdkEvent *event, gpointer user_data)
{
	gtk_widget_set_sensitive(get_widget(dlg_edit_collo,"pb_qta_errata"),FALSE);
	gtk_widget_set_sensitive(get_widget(dlg_edit_collo,"pb_mancante"),FALSE);
	gtk_widget_set_sensitive(get_widget(dlg_edit_collo,"pb_inversione"),FALSE);
	gtk_widget_set_sensitive(get_widget(dlg_edit_collo,"pb_riga_non_richiesta"),FALSE);
	gtk_widget_set_sensitive(get_widget(dlg_edit_collo,"pb_annulla_errore"),FALSE);
}


void on_pb_qta_errata_clicked (gpointer user_data,GtkButton *button)
{
	GdkColor *cColor;
	char *pszOrdkey;
	char *pszCollo;
	char *pszCodProd;
	int nCollo;

	lst_righe_collo=get_widget(dlg_edit_collo,"lst_righe_collo");

	cColor=&cBLUE;
	gtk_clist_set_background(GTK_CLIST(lst_righe_collo),nLstRigheColloRowSelected,cColor);
	gtk_clist_set_foreground(GTK_CLIST(lst_righe_collo),nLstRigheColloRowSelected,&cWHITE);

	gtk_label_get(GTK_LABEL(get_widget(dlg_edit_collo,"lb_ORDPROG")),&pszOrdkey);
	gtk_label_get(GTK_LABEL(get_widget(dlg_edit_collo,"lb_NMCOL")),&pszCollo);
	nCollo=atoi(pszCollo);
	gtk_clist_get_text(GTK_CLIST(lst_righe_collo),nLstRigheColloRowSelected,0,&pszCodProd);
	SetErroreRiga(pszOrdkey,nCollo,pszCodProd,"QTA_ERRATA");
}


void on_pb_mancante_clicked (gpointer user_data,GtkButton *button)
{
	GdkColor *cColor;
	char *pszOrdkey;
	char *pszCollo;
	char *pszCodProd;
	int nCollo;

	lst_righe_collo=get_widget(dlg_edit_collo,"lst_righe_collo");

	cColor=&cCYAN;
	gtk_clist_set_background(GTK_CLIST(lst_righe_collo),nLstRigheColloRowSelected,cColor);
	gtk_clist_set_foreground(GTK_CLIST(lst_righe_collo),nLstRigheColloRowSelected,&cWHITE);

	gtk_label_get(GTK_LABEL(get_widget(dlg_edit_collo,"lb_ORDPROG")),&pszOrdkey);
	gtk_label_get(GTK_LABEL(get_widget(dlg_edit_collo,"lb_NMCOL")),&pszCollo);
	nCollo=atoi(pszCollo);
	gtk_clist_get_text(GTK_CLIST(lst_righe_collo),nLstRigheColloRowSelected,0,&pszCodProd);
	SetErroreRiga(pszOrdkey,nCollo,pszCodProd,"MANCANTE");
}


void on_pb_inversione_clicked (gpointer user_data,GtkButton *button)
{
	GdkColor *cColor;
	char *pszOrdkey;
	char *pszCollo;
	char *pszCodProd;
	int nCollo;

	lst_righe_collo=get_widget(dlg_edit_collo,"lst_righe_collo");

	cColor=&cRED;
	gtk_clist_set_background(GTK_CLIST(lst_righe_collo),nLstRigheColloRowSelected,cColor);
	gtk_clist_set_foreground(GTK_CLIST(lst_righe_collo),nLstRigheColloRowSelected,&cWHITE);

	gtk_label_get(GTK_LABEL(get_widget(dlg_edit_collo,"lb_ORDPROG")),&pszOrdkey);
	gtk_label_get(GTK_LABEL(get_widget(dlg_edit_collo,"lb_NMCOL")),&pszCollo);
	nCollo=atoi(pszCollo);
	gtk_clist_get_text(GTK_CLIST(lst_righe_collo),nLstRigheColloRowSelected,0,&pszCodProd);
	SetErroreRiga(pszOrdkey,nCollo,pszCodProd,"INVERSIONE");
}


void on_pb_riga_non_richiesta_clicked (gpointer user_data,GtkButton *button)
{
	GdkColor *cColor;
	char *pszOrdkey;
	char *pszCollo;
	char *pszCodProd;
	int nCollo;

	lst_righe_collo=get_widget(dlg_edit_collo,"lst_righe_collo");

	cColor=&cGREEN;
	gtk_clist_set_background(GTK_CLIST(lst_righe_collo),nLstRigheColloRowSelected,cColor);
	gtk_clist_set_foreground(GTK_CLIST(lst_righe_collo),nLstRigheColloRowSelected,&cWHITE);

	gtk_label_get(GTK_LABEL(get_widget(dlg_edit_collo,"lb_ORDPROG")),&pszOrdkey);
	gtk_label_get(GTK_LABEL(get_widget(dlg_edit_collo,"lb_NMCOL")),&pszCollo);
	nCollo=atoi(pszCollo);
	gtk_clist_get_text(GTK_CLIST(lst_righe_collo),nLstRigheColloRowSelected,0,&pszCodProd);
	SetErroreRiga(pszOrdkey,nCollo,pszCodProd,"NON_RICHIESTA");
}


void on_pb_annulla_errore_clicked (gpointer user_data,GtkButton *button)
{
	GdkColor *cColor;
	char *pszOrdkey;
	char *pszCollo;
	char *pszCodProd;
	int nCollo;

	lst_righe_collo=get_widget(dlg_edit_collo,"lst_righe_collo");

	cColor=&cWHITE;
	gtk_clist_set_background(GTK_CLIST(lst_righe_collo),nLstRigheColloRowSelected,cColor);
	gtk_clist_set_foreground(GTK_CLIST(lst_righe_collo),nLstRigheColloRowSelected,&cBLACK);

	gtk_label_get(GTK_LABEL(get_widget(dlg_edit_collo,"lb_ORDPROG")),&pszOrdkey);
	gtk_label_get(GTK_LABEL(get_widget(dlg_edit_collo,"lb_NMCOL")),&pszCollo);
	nCollo=atoi(pszCollo);
	gtk_clist_get_text(GTK_CLIST(lst_righe_collo),nLstRigheColloRowSelected,0,&pszCodProd);
	ResetErroreRiga(pszOrdkey,nCollo,pszCodProd);
}

BOOL SetErroreRiga(char *szOrdkey,int nCollo,char *szCodProd, char *szCDERR)
{
	PGresult *PGRes = NULL;
	PGresult *PGResDelete = NULL;
	PGresult *PGResInsert = NULL;

	lst_righe_collo=get_widget(dlg_edit_collo,"lst_righe_collo");

	PGRes=PGExecSQL(Cfg.nDebugVersion>1,"select * from errori_prelievo where ordkey='%s' and epnmcol=%d and epcdpro='%s';",szOrdkey,nCollo,szCodProd);
	if(PQntuples(PGRes)){
		/* riga presente - la  cancelllo */
		PGResDelete=PGRes=PGExecSQL(Cfg.nDebugVersion>1,"delete from errori_prelievo where ordkey='%s' and epnmcol=%d and epcdpro='%s';",szOrdkey,nCollo,szCodProd);
		PQclear(PGResDelete);
	}
	PQclear(PGRes);

	if (!bStorico){
		PGResInsert=PGExecSQL(Cfg.nDebugVersion>1,"insert into errori_prelievo (ordprog,ordkey,epnmcol,epcdpro,epqtspe,epcdubi,epcdopr,eptmprl,epcderr) select ordprog,ordkey,rpnmcol,rpcdpro,rpqtspe,rpcdubi,rpcdopr,rptmprl,'%s' from rig_prod where ordkey='%s' and rpnmcol=%d and rpcdpro='%s';",szCDERR,szOrdkey,nCollo,szCodProd);
		PQclear(PGResInsert);
	} else {
		PGResInsert=PGExecSQL(Cfg.nDebugVersion>1,"insert into errori_prelievo (ordprog,ordkey,epnmcol,epcdpro,epqtspe,epcdubi,epcdopr,eptmprl,epcderr) select ordprog,ordkey,rpnmcol,rpcdpro,rpqtspe,rpcdubi,rpcdopr,rptmprl,'%s' from rig_stor where ordkey='%s' and rpnmcol=%d and rpcdpro='%s';",szCDERR,szOrdkey,nCollo,szCodProd);
		PQclear(PGResInsert);
	}

	return TRUE;
}

BOOL ResetErroreRiga(char *szOrdkey,int nCollo,char *szCodProd)
{
	PGresult *PGRes = NULL;
	PGresult *PGResDelete = NULL;

	PGRes=PGExecSQL(Cfg.nDebugVersion>1,"select * from errori_prelievo where ordkey='%s' and epnmcol=%d and epcdpro='%s';",szOrdkey,nCollo,szCodProd);
	if(PQntuples(PGRes)){
		/* riga presente - la  cancelllo */
		PGResDelete=PGRes=PGExecSQL(Cfg.nDebugVersion>1,"delete from errori_prelievo where ordkey='%s' and epnmcol=%d and epcdpro='%s';",szOrdkey,nCollo,szCodProd);
		PQclear(PGResDelete);
	}
	PQclear(PGRes);

	return TRUE;
}

void do_marca_elaborazione_pallet(GtkButton *button, gpointer user_data)
{
	/*
	* Marco gli ordini delle elaborazioni selezionate
	*/
	GList *lista;
	char *pszNumElaborazione;
	char szNumElaborazione[128];
	char szStato[80];
	char szTransitPoint[64];
	char szOrdine[64];
	int nRowIndex;
	int nIndex;
	int nElaborazioni;
	PGresult *PGRes;
	PGresult *PGResUpdate;

	sprintf(szStato,"%c",ORDINE_SPEDITO);

	lista=g_list_first(GTK_CLIST(get_widget(dlg_distribuzione,"clist_distribuzione"))->selection);
	if(lista==NULL){
		return;
	}
	do {
		nRowIndex=GPOINTER_TO_INT(lista->data);
		gtk_clist_get_text(GTK_CLIST(get_widget(dlg_distribuzione,"clist_distribuzione")),nRowIndex,0,&pszNumElaborazione);
		strcpy(szNumElaborazione,pszNumElaborazione);
		/*
		* rm 16-11-2019 : se rotrnsp (transit point) in (35 o 31 o 32)
		* non consentire di marcare l'ordine a PALLET
		*/
		PGRes=PGExecSQL(FALSE,"select ordprog,rotrnsp from ric_ord where ronelab='%s' order by ordprog;",szNumElaborazione);

		nElaborazioni=PQntuples(PGRes);

		for (nIndex=0; nIndex<nElaborazioni; nIndex++){
			sprintf(szTransitPoint,"%s",PQgetvalue(PGRes,nIndex,1));
			sprintf(szOrdine,"%s",PQgetvalue(PGRes,nIndex,0));
			if(atoi(szTransitPoint) == 31 || atoi(szTransitPoint) == 32 || atoi(szTransitPoint) == 35 ){

				gtk_text_printf("RED",txt_msgs,"ORDINE %s NON A PALLET (31/32/35)\n",szOrdine);

			} else {
				PGResUpdate=PGExecSQL(Cfg.nDebugVersion>2,"update ric_ord set roflgpz=1 where ordprog='%s';",szOrdine);
				PQclear(PGResUpdate);
			}
		}
		PQclear(PGRes);
		
	} while((lista=g_list_next(lista)));

	gtk_widget_destroy(GTK_WIDGET(dlg_lancio_distribuzioni));
	ListaDistribuzione();
}

void do_marca_elaborazione_fuori_pallet(GtkButton *button, gpointer user_data)
{
	/*
	* Marco gli ordini delle elaborazioni selezionate
	*/
	GList *lista;
	char *pszNumElaborazione;
	char szNumElaborazione[128];
	char szStato[80];
	char szOrdine[64];
	int nRowIndex;
	int nIndex;
	int nElaborazioni;
	PGresult *PGRes;
	PGresult *PGResUpdate;

	sprintf(szStato,"%c",ORDINE_SPEDITO);

	lista=g_list_first(GTK_CLIST(get_widget(dlg_distribuzione,"clist_distribuzione"))->selection);
	if(lista==NULL){
		return;
	}
	do {
		nRowIndex=GPOINTER_TO_INT(lista->data);
		gtk_clist_get_text(GTK_CLIST(get_widget(dlg_distribuzione,"clist_distribuzione")),nRowIndex,0,&pszNumElaborazione);
		strcpy(szNumElaborazione,pszNumElaborazione);
		/*
		*/
		PGRes=PGExecSQL(FALSE,"select ordprog from ric_ord where ronelab='%s' order by ordprog;",szNumElaborazione);

		nElaborazioni=PQntuples(PGRes);

		for (nIndex=0; nIndex<nElaborazioni; nIndex++){
			sprintf(szOrdine,"%s",PQgetvalue(PGRes,nIndex,0));
			PGResUpdate=PGExecSQL(Cfg.nDebugVersion>2,"update ric_ord set roflgpz=0 where ordprog='%s';",szOrdine);
			PQclear(PGResUpdate);
		}
		PQclear(PGRes);
		
	} while((lista=g_list_next(lista)));

	gtk_widget_destroy(GTK_WIDGET(dlg_lancio_distribuzioni));
	ListaDistribuzione();
}


void on_dlg_prodotti_pericolosi_pb_modifica_clicked (gpointer user_data,GtkButton *button)
{
	PGresult *PGRes;
	char szCodProd[40];
	char szSqlCmd[2048];
	GtkWidget *dlg=GTK_WIDGET(user_data);

	strcpy(szCodProd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg,"entry_ppcdpro"))));
	StrTrimAll(szCodProd);

	if(strlen(szCodProd)!=0){
		strcpy(szSqlCmd,"update prodotti_pericolosi set prcdper='"); strcat(szSqlCmd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg,"entry_ppcdper"))));
		strcat(szSqlCmd,"' where ppcdpro='"); strcat(szSqlCmd,szCodProd);
		strcat(szSqlCmd,"';");
		
		PGRes=PGExecSQL(Cfg.nDebugVersion,szSqlCmd);

		if (PQresultStatus(PGRes) == PGRES_COMMAND_OK ) {
#ifdef TRACE
			trace_out_vstr_date(1,"Modificato prodotto [%s]",szCodProd);
#endif
		} else {
#ifdef TRACE
			trace_out_vstr_date(1,"Fallita modifica su prodotto [%s]",szCodProd);
#endif
		}
		PQclear(PGRes);

		RefreshListaProdottiPericolosi(dlg);
	} else {
		/*
		* nessuna riga selezionata
		*/
		GtkWidget *lb_msg;
		GtkWidget *pb_ok;

		dlg_message=create_dlg_message();
		lb_msg=get_widget(dlg_message,"lb_msg");
		pb_ok=get_widget(dlg_message,"pb_ok");

		gtk_label_printf(lb_msg,"NESSUN PRODOTTO SELEZIONATO");
		gtk_window_set_title (GTK_WINDOW (dlg_message), "Modifica Prodotto");
		gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
		gtk_widget_show(dlg_message);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));
		gtk_signal_connect (GTK_OBJECT (pb_ok), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_pb_ok_clicked), dlg_message);
	}
}


void on_dlg_prodotti_pericolosi_pb_aggiungi_clicked (gpointer user_data,GtkButton *button)
{
	PGresult *PGRes;
	char szCodProd[40];
	char szSqlCmd[1024];
	GtkWidget *dlg=GTK_WIDGET(user_data);

	strcpy(szCodProd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg,"entry_ppcdpro"))));
	StrTrimAll(szCodProd);
	if(strlen(szCodProd)!=0){

		sprintf(szSqlCmd,"select ppcdpro from prodotti_pericolosi where ppcdpro='%s';",szCodProd);
		PGRes=PGExecSQL(Cfg.nDebugVersion,szSqlCmd);
		if (PQntuples(PGRes)!=0){
			/*
			* codice prodotto gia' presente
			*/
			GtkWidget *lb_msg;
			GtkWidget *pb_ok;

			dlg_message=create_dlg_message();
			lb_msg=get_widget(dlg_message,"lb_msg");
			pb_ok=get_widget(dlg_message,"pb_ok");

			PQclear(PGRes);

			gtk_label_printf(lb_msg,"CODICE PRODOTTO\nGIA' PRESENTE");
			gtk_window_set_title (GTK_WINDOW (dlg_message), "Aggiungi Prodotto");
			gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
			gtk_widget_show(dlg_message);
			gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));
			gtk_signal_connect (GTK_OBJECT (pb_ok), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_pb_ok_clicked), dlg_message);
		} else {
			PQclear(PGRes);
			strcpy(szSqlCmd,"insert into prodotti_pericolosi (ppcdpro,ppcdper) values ('");
			strcat(szSqlCmd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg,"entry_ppcdpro")))); strcat(szSqlCmd,"', '");
			strcat(szSqlCmd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg,"entry_ppcdper")))); strcat(szSqlCmd,"');");
			
			PGRes=PGExecSQL(Cfg.nDebugVersion,szSqlCmd);
			if (PQresultStatus(PGRes) == PGRES_COMMAND_OK ) {
#ifdef TRACE
				trace_out_vstr_date(1,"Inserito prodotto [%s]",gtk_entry_get_text(GTK_ENTRY(get_widget(dlg,"entry_ppcdpro"))));
#endif
			} else {
#ifdef TRACE
				trace_out_vstr_date(1,"Fallito inserimento prodotto [%s]",gtk_entry_get_text(GTK_ENTRY(get_widget(dlg,"entry_ppcdpro"))));
#endif
			}
			PQclear(PGRes);

			RefreshListaProdottiPericolosi(dlg);
		}

	} else {
		/*
		* nessun prodotto selezionato
		*/
		GtkWidget *lb_msg;
		GtkWidget *pb_ok;

		dlg_message=create_dlg_message();
		lb_msg=get_widget(dlg_message,"lb_msg");
		pb_ok=get_widget(dlg_message,"pb_ok");

		gtk_label_printf(lb_msg,"INSERIRE IL CODICE PRODOTTO");
		gtk_window_set_title (GTK_WINDOW (dlg_message), "Aggiungi Prodotto");
		gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
		gtk_widget_show(dlg_message);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));
		gtk_signal_connect (GTK_OBJECT (pb_ok), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_pb_ok_clicked), dlg_message);
	}


}


void on_dlg_prodotti_pericolosi_pb_elimina_clicked (gpointer user_data,GtkButton *button)
{
	GtkWidget *lb_msg;
	GtkWidget *pb_ok;
	char szCodProd[40];
	GtkWidget *dlg=GTK_WIDGET(user_data);

	dlg_message=create_dlg_message();

	lb_msg=get_widget(dlg_message,"lb_msg");
	pb_ok=get_widget(dlg_message,"pb_ok");
	strcpy(szCodProd,gtk_entry_get_text(GTK_ENTRY(get_widget(dlg,"entry_ppcdpro"))));
	StrTrimAll(szCodProd);

	if (strlen(szCodProd)!=0){
		gtk_label_printf(lb_msg,"CONFERMI L'ELIMINAZIONE\nDEL PRODOTTO %s",szCodProd);
		gtk_window_set_title (GTK_WINDOW (dlg_message), "Elimina Prodotto");
		gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
		gtk_widget_show(dlg_message);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));
		/*
		* callback per pressione pulsante
		*/
		gtk_signal_connect (GTK_OBJECT (pb_ok), "clicked", GTK_SIGNAL_FUNC (do_elimina_prodotto_pericoloso), dlg);
	} else {
		gtk_label_printf(lb_msg,"NESSUN PRODOTTO SELEZIONATO");
		gtk_window_set_title (GTK_WINDOW (dlg_message), "Elimina Prodotto");
		gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
		gtk_widget_show(dlg_message);
		gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));
		gtk_signal_connect (GTK_OBJECT (pb_ok), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_pb_ok_clicked), dlg_message);
	}
}




void on_dlg_distribuzione_pb_amazon_clicked (GtkButton       *button, gpointer         user_data)
{
	/*
	* Marco gli ordini delle elaborazioni selezionate
	*/
	GList *lista;
	char *pszNumElaborazione;
	char szNumElaborazione[128];
	char szOrdine[64];
	int nRowIndex;
	int nIndex;
	int nElaborazioni;
	PGresult *PGRes;
	PGresult *PGResUpdate;

	lista=g_list_first(GTK_CLIST(get_widget(dlg_distribuzione,"clist_distribuzione"))->selection);
	if(lista==NULL){
		return;
	}
	do {
		nRowIndex=GPOINTER_TO_INT(lista->data);
		gtk_clist_get_text(GTK_CLIST(get_widget(dlg_distribuzione,"clist_distribuzione")),nRowIndex,0,&pszNumElaborazione);
		strcpy(szNumElaborazione,pszNumElaborazione);
		/*
		*/
		PGRes=PGExecSQL(FALSE,"select ordprog from ric_ord where ronelab='%s' order by ordprog;",szNumElaborazione);

		nElaborazioni=PQntuples(PGRes);

		for (nIndex=0; nIndex<nElaborazioni; nIndex++){
			sprintf(szOrdine,"%s",PQgetvalue(PGRes,nIndex,0));
			PGResUpdate=PGExecSQL(Cfg.nDebugVersion>2,"update ric_ord set roflamz=1 where ordprog='%s';",szOrdine);
			PQclear(PGResUpdate);
		}
		PQclear(PGRes);
		
	} while((lista=g_list_next(lista)));

	// gtk_widget_destroy(GTK_WIDGET(dlg_lancio_distribuzioni));
	ListaDistribuzione();
}


void on_dlg_distribuzione_pb_normale_clicked (GtkButton       *button, gpointer         user_data)
{
	/*
	* Marco gli ordini delle elaborazioni selezionate
	*/
	GList *lista;
	char *pszNumElaborazione;
	char szNumElaborazione[128];
	char szOrdine[64];
	int nRowIndex;
	int nIndex;
	int nElaborazioni;
	PGresult *PGRes;
	PGresult *PGResUpdate;

	lista=g_list_first(GTK_CLIST(get_widget(dlg_distribuzione,"clist_distribuzione"))->selection);
	if(lista==NULL){
		return;
	}
	do {
		nRowIndex=GPOINTER_TO_INT(lista->data);
		gtk_clist_get_text(GTK_CLIST(get_widget(dlg_distribuzione,"clist_distribuzione")),nRowIndex,0,&pszNumElaborazione);
		strcpy(szNumElaborazione,pszNumElaborazione);
		/*
		*/
		PGRes=PGExecSQL(FALSE,"select ordprog from ric_ord where ronelab='%s' order by ordprog;",szNumElaborazione);

		nElaborazioni=PQntuples(PGRes);

		for (nIndex=0; nIndex<nElaborazioni; nIndex++){
			sprintf(szOrdine,"%s",PQgetvalue(PGRes,nIndex,0));
			PGResUpdate=PGExecSQL(Cfg.nDebugVersion>2,"update ric_ord set roflamz=0 where ordprog='%s';",szOrdine);
			PQclear(PGResUpdate);
		}
		PQclear(PGRes);
		
	} while((lista=g_list_next(lista)));

	// gtk_widget_destroy(GTK_WIDGET(dlg_lancio_distribuzioni));
	ListaDistribuzione();

}

