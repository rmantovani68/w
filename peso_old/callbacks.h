/*
* Callbacks.h
* Modulo: Bilancia
* Easy Picking 3.0 - Wella Rebuild
* Copyright CNI srl 1999-2000
* Autori : Roberto Mantovani 
*          Stefano Tarroni
* Data Creazione : 16-10-2000 
*/

#include <gtk/gtk.h>

/*
* MAIN WINDOW
*/
gboolean on_main_window_destroy_event (GtkWidget *widget, GdkEvent *event, gpointer user_data);
void on_lst_colli_select_row(GtkCList *clist, gint row, gint column, GdkEvent *event, gpointer user_data);
void on_lst_colli_unselect_row(GtkCList *clist, gint row, gint column, GdkEvent *event, gpointer user_data);

/*
* TOOLBAR BUTTONS
*/
void on_main_window_pb_exit_clicked           (GtkButton *button, gpointer user_data);
void on_main_window_pb_edit_clicked           (GtkButton *button, gpointer user_data);
void on_main_window_pb_stampa_clicked         (GtkButton *button, gpointer user_data);
void on_main_window_pb_settings_clicked       (GtkButton *button, gpointer user_data);
void on_main_window_pb_elimina_collo_clicked  (GtkButton *button, gpointer user_data);
void on_main_window_pb_aggiungi_collo_clicked (GtkButton *button, gpointer user_data);
void on_main_window_pb_ordine_ok_clicked      (GtkButton *button, gpointer user_data);

/*
* MENUBAR OPTIONS
*/
void on_pbm_aggiungi_collo_activate      (GtkMenuItem *menuitem, gpointer user_data); 
void on_pbm_elimina_collo_activate       (GtkMenuItem *menuitem, gpointer user_data);
void on_pbm_ordine_controllato_activate  (GtkMenuItem *menuitem, gpointer user_data);
void on_pbm_exit_activate                (GtkMenuItem *menuitem, gpointer user_data);
void on_pbm_configurazione_activate      (GtkMenuItem *menuitem, gpointer user_data);
void on_pbm_about_activate               (GtkMenuItem *menuitem, gpointer user_data);
void on_pbm_debug_variabili_plc_activate (GtkMenuItem *menuitem, gpointer user_data);
void on_pbm_stampa_etichette_activate    (GtkMenuItem *menuitem, gpointer user_data);
void on_pbm_stampa_ordine_activate       (GtkMenuItem *menuitem, gpointer user_data);
void on_pbm_seleziona_tutto_activate     (GtkMenuItem *menuitem, gpointer user_data);
void on_pbm_deseleziona_tutto_activate   (GtkMenuItem *menuitem, gpointer user_data);

/*
* DLG_EXIT
*/
void on_dlg_exit_pb_ok_clicked           (GtkButton *button, gpointer user_data);
void on_dlg_exit_pb_cancel_clicked       (GtkButton *button, gpointer user_data);

/*
* DLG_MESSAGE
*/ 
void on_dlg_message_pb_ok_clicked        (GtkButton *button, gpointer user_data);
void on_dlg_message_pb_cancel_clicked    (GtkButton *button, gpointer user_data);

/*
* DLG_INFO
*/
void on_dlg_info_pb_ok_clicked           (GtkButton *button, gpointer user_data);

/*
* DLG_APP_ERROR
*/
void on_dlg_app_error_pb_ok_clicked      (GtkButton *button, gpointer user_data);

/*
* DLG_CONFIG
*/
void on_dlg_config_pb_ok_clicked                     (GtkButton *button, gpointer user_data);
void on_dlg_config_pb_apply_clicked                  (GtkButton *button, gpointer user_data);
void on_dlg_config_pb_cancel_clicked                 (GtkButton *button, gpointer user_data);
void on_dlg_config_pb_refresh_plc_clicked            (GtkButton *button, gpointer user_data);
void on_dlg_config_rb_tolleranza_assoluta_toggled    (GtkToggleButton *togglebutton, gpointer user_data);
void on_dlg_config_rb_tolleranza_percentuale_toggled (GtkToggleButton *togglebutton, gpointer user_data);
void on_dlg_config_rb_pesatura_ordine_toggled        (GtkToggleButton *togglebutton, gpointer user_data); 
void on_dlg_config_rb_pesatura_collo_toggled         (GtkToggleButton *togglebutton, gpointer user_data);
void on_dlg_config_cb_soglia_massima_toggled         (GtkToggleButton *togglebutton, gpointer user_data);

/*
* DLG_EDIT_ORDINE
*/
void on_dlg_edit_ordine_pb_ok_clicked         (GtkButton *button, gpointer user_data);
void on_dlg_edit_ordine_pb_cancel_clicked     (GtkButton *button, gpointer user_data);
void on_dlg_edit_ordine_pb_prev_clicked       (GtkButton *button, gpointer user_data);
void on_dlg_edit_ordine_pb_next_clicked       (GtkButton *button, gpointer user_data);

/*
* DLG_EDIT_COLLO
*/
void on_dlg_edit_collo_pb_ok_clicked          (GtkButton *button, gpointer user_data);
void on_dlg_edit_collo_pb_cancel_clicked      (GtkButton *button, gpointer user_data);
void on_dlg_edit_collo_pb_prev_clicked        (GtkButton *button, gpointer user_data);
void on_dlg_edit_collo_pb_next_clicked        (GtkButton *button, gpointer user_data);
void on_dlg_edit_collo_pb_edit_ordine_clicked (GtkButton *button, gpointer user_data);

/*
* DLG_BARCODE
*/
void on_dlg_barcode_pb_ok_clicked           (GtkButton *button, gpointer user_data);
void on_dlg_barcode_pb_cancel_clicked       (GtkButton *button, gpointer user_data);

/*
* DLG_TRACE
*/
void on_dlg_trace_pb_cancel_clicked         (GtkButton *button, gpointer user_data);

/*
* DLG_ADD_VAR
*/
void on_dlg_add_var_pb_add_clicked          (GtkButton *button, gpointer user_data);
void on_dlg_add_var_pb_cancel_clicked       (GtkButton *button, gpointer user_data);
void on_dlg_add_var_pb_force_clicked        (GtkButton *button, gpointer user_data);

/*
* DLG_FORCE
*/
void on_dlg_force_var_pb_cancel_clicked     (GtkButton *button, gpointer user_data);

/*
* DLG_DEBUG_VAR
*/
void on_dlg_debug_var_pb_force_clicked      (GtkButton *button, gpointer user_data);
void on_dlg_debug_var_pb_cancel_clicked     (GtkButton *button, gpointer user_data);

/*
* DLG_PLC_ERROR
*/
void on_dlg_plc_error_pb_ok_clicked         (GtkButton *button, gpointer user_data);
void on_dlg_plc_error_pb_cancel_clicked     (GtkButton *button, gpointer user_data);

/*
* DLG_STAMPA_ORDINE
*/
void on_dlg_stampa_ordine_pb_cancel_clicked (GtkButton *button, gpointer user_data);
void do_stampa_packing_list                 (GtkButton *button, gpointer user_data);
void do_stampa_etichette                    (GtkButton *button, gpointer user_data);

/*
* ORDINAMENTO COLONNE
*/
void on_lst_sort_column_clicked (GtkCList *clist, gint nColumnNumber, gpointer user_data);

/*
* DLG_AGGIUNGI_COLLO
*/
void on_dlg_aggiungi_collo_pb_aggiungi_clicked (GtkButton *button, gpointer user_data);
void on_dlg_aggiungi_collo_pb_cancel_clicked   (GtkButton *button, gpointer user_data);

/*
* DLG_ELIMINA_COLLO
*/
void on_dlg_elimina_collo_pb_elimina_clicked (GtkButton *button, gpointer user_data); 
void on_dlg_elimina_collo_pb_canel_clicked   (GtkButton *button, gpointer user_data);

/*
* Altre callbacks
*/
void do_ordine_controllato(GtkButton *button, gpointer user_data);

void on_dlg_scarto_pb_ok_clicked(GtkButton *button, gpointer user_data);
