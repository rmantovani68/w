/*
* callbacks.h
* Callbacks functions declaration
* Easy Picking 3.0
* Copyright CNI srl 1999-2000
* Autori : Roberto Mantovani
*          Stefano Tarroni
*/

#include <gtk/gtk.h>

gboolean on_main_window_destroy_event               (GtkWidget *widget, GdkEvent *event, gpointer user_data);
void on_lst_ordini_select_row                       (GtkCList *clist, gint row, gint column, GdkEvent *event, gpointer user_data);
void on_lst_sort_column_clicked                     (GtkCList *clist, gint nColumnNumber, gpointer user_data);
void on_mb_main_activate_current                    (GtkMenuShell *menushell, gboolean force_hide, gpointer user_data);
void on_lista_imballi_row_selected                  (GtkCList *clist, gint row, gint column, GdkEvent *event, gpointer user_data);
void on_lista_opeset_row_selected                   (GtkCList *clist, gint row, gint column, GdkEvent *event, gpointer user_data);
void on_lista_opeana_row_selected                   (GtkCList *clist, gint row, gint column, GdkEvent *event, gpointer user_data);
void on_lst_prodotti_row_selected                   (GtkCList *clist, gint row, gint column, GdkEvent *event, gpointer user_data);
void on_lst_prodotti_pericolosi_row_selected        (GtkCList *clist, gint row, gint column, GdkEvent *event, gpointer user_data);
/* GENERAL CALLBACK */
void on_delete_ricezione_pb_ok_clicked              (GtkButton *button, gpointer user_data);
void on_delete_produzione_pb_ok_clicked             (GtkButton *button, gpointer user_data);
/* DLG_ABOUT */
void on_dlg_about_pb_cancel_clicked                 (gpointer user_data,GtkButton *button);
void on_dlg_about_pb_ok_clicked                     (gpointer user_data,GtkButton *button);
/* DLG_CONFIG */
void on_dlg_config_pb_apply_clicked                 (GtkButton *button, gpointer user_data);
//void on_dlg_config_pb_cancel_clicked                (GtkButton *button, gpointer user_data);
void on_dlg_config_pb_cancel_clicked                (gpointer user_data,GtkButton * button);
void on_dlg_config_pb_ok_clicked                    (gpointer user_data,GtkButton *button);
void on_dlg_config_pb_modifica_imb_clicked          (GtkButton *button, gpointer user_data);
void on_dlg_config_pb_aggiungi_imb_clicked          (GtkButton *button, gpointer user_data);
void on_dlg_config_pb_elimina_imb_clicked           (GtkButton *button, gpointer user_data);
void on_dlg_config_do_elimina_imballo               (GtkButton *button, gpointer user_data);
/* DLG_EDIT_COLLO */
void on_dlg_edit_collo_pb_cancel_clicked            (GtkButton *button, gpointer user_data);
void on_dlg_edit_collo_pb_next_clicked              (GtkButton *button, gpointer user_data);
void on_dlg_edit_collo_pb_ok_clicked                (GtkButton *button, gpointer user_data);
void on_dlg_edit_collo_pb_prev_clicked              (GtkButton *button, gpointer user_data);
/* DLG_EDIT_ORDINE */
void on_dlg_edit_ordine_pb_cancel_clicked           (GtkButton *button, gpointer user_data);
void on_dlg_edit_ordine_pb_edit_collo_clicked       (GtkButton *button, gpointer user_data);
void on_dlg_edit_ordine_pb_next_clicked             (GtkButton *button, gpointer user_data);
void on_dlg_edit_ordine_pb_ok_clicked               (GtkButton *button, gpointer user_data);
void on_dlg_edit_ordine_pb_prev_clicked             (GtkButton *button, gpointer user_data);
/* DLG_EXIT */
void on_dlg_exit_pb_cancel_clicked                  (GtkButton *button, gpointer user_data);
void on_dlg_exit_pb_ok_clicked                      (GtkButton *button, gpointer user_data);
/* DLG_AGGIORNA_ARCHIVI */
void on_dlg_aggiorna_archivi_pb_ok_clicked          (GtkButton *button, gpointer user_data);
/* MAIN TOOLBAR */
void on_main_pb_carico_clicked                      (GtkButton *button, gpointer user_data);
void on_main_pb_exit_clicked                        (GtkButton *button, gpointer user_data);
void on_main_pb_help_clicked                        (GtkButton *button, gpointer user_data);
void on_main_pb_config_clicked                      (GtkButton *button, gpointer user_data);
void on_main_pb_select_clicked                      (GtkButton *button, gpointer user_data);
void on_main_pb_start_clicked                       (GtkButton *button, gpointer user_data);
void on_main_pb_stop_clicked                        (GtkButton *button, gpointer user_data);
void on_main_pb_edit_clicked                        (GtkButton *button, gpointer user_data);
/* MAIN MENU' */
void on_pbm_aggiorna_lista_activate                 (GtkMenuItem *menuitem, gpointer user_data);
void on_pbm_linea_di_instradamento_activate         (GtkMenuItem *menuitem, gpointer user_data);
void on_pbm_distribuzione_activate                  (GtkMenuItem *menuitem, gpointer user_data);
void on_pbm_visualizza_messaggi_activate            (GtkMenuItem *menuitem, gpointer user_data);
void on_pbm_visualizza_ordini_spediti_host_activate (GtkMenuItem *menuitem, gpointer user_data);
void on_pbm_codice_spedizione_activate              (GtkMenuItem *menuitem, gpointer user_data);
void on_pbm_configurazione_activate                 (GtkMenuItem *menuitem, gpointer user_data);
void on_pbm_prodotti_activate                       (GtkMenuItem *menuitem, gpointer user_data);
void on_pbm_seleziona_tutto_activate                (GtkMenuItem *menuitem, gpointer user_data);
void on_pbm_deseleziona_tutto_activate              (GtkMenuItem *menuitem, gpointer user_data);
void on_pbm_edit_ordine_activate                    (GtkMenuItem *menuitem, gpointer user_data);
void on_pbm_operatori_activate                      (GtkMenuItem *menuitem, gpointer user_data); 
void on_pbm_lancio_ordini_activate                  (GtkMenuItem *menuitem, gpointer user_data);
void on_pbm_scarto_ordini_activate                  (GtkMenuItem *menuitem, gpointer user_data);
void on_pbm_cambia_priorita                         (GtkMenuItem *menuitem, gpointer user_data);
void on_pbm_statistiche_carico_settori_activate     (GtkMenuItem *menuitem, gpointer user_data);
void on_pbm_reset_di_un_settore_activate            (GtkMenuItem *menuitem, gpointer user_data);
void on_pbm_reset_settori_activate                  (GtkMenuItem *menuitem, gpointer user_data);
void on_pbm_reset_stato_ordine_activate             (GtkMenuItem *menuitem, gpointer user_data);
void on_pbm_setta_stato_ordine_activate             (GtkMenuItem *menuitem, gpointer user_data);
void on_pbm_start_impianto_activate                 (GtkMenuItem *menuitem, gpointer user_data);
void on_pbm_stop_impianto_activate                  (GtkMenuItem *menuitem, gpointer user_data);
void on_pbm_carica_stato_activate                   (GtkMenuItem *menuitem, gpointer user_data);
void on_pbm_salva_stato_activate                    (GtkMenuItem *menuitem, gpointer user_data);
void on_pbm_pulizia_dati_produzione_activate        (GtkMenuItem *menuitem, gpointer user_data);
void on_pbm_pulizia_dati_ricezione_activate         (GtkMenuItem *menuitem, gpointer user_data);
void on_pbm_aggiorna_archivi_activate               (GtkMenuItem *menuitem, gpointer user_data);
void on_pbm_ricezione_tutto_activate                (GtkMenuItem *menuitem, gpointer user_data);
void on_pbm_ricezione_ordini_activate               (GtkMenuItem *menuitem, gpointer user_data);
void on_pbm_ricezione_articoli_activate             (GtkMenuItem *menuitem, gpointer user_data);
void on_pbm_ricezione_ubicazioni_activate           (GtkMenuItem *menuitem, gpointer user_data);
void on_pbm_stat_prelievo_activate                  (GtkMenuItem *menuitem, gpointer user_data);
void on_pbm_spedizione_dati_activate                (GtkMenuItem *menuitem, gpointer user_data);
void on_pbm_storicizzazione_dati_activate           (GtkMenuItem *menuitem, gpointer user_data);
void on_pbm_ricerca_in_storico_activate             (GtkMenuItem *menuitem, gpointer user_data);
void on_pbm_stampa_da_storico_activate              (GtkMenuItem *menuitem, gpointer user_data);
void on_pbm_help_activate                           (GtkMenuItem *menuitem, gpointer user_data);
void on_pbm_about_activate                          (GtkMenuItem *menuitem, gpointer user_data);
void on_pbm_test_ios_activate                       (GtkMenuItem *menuitem, gpointer user_data);
void on_pbm_exit_activate                           (GtkMenuItem *menuitem, gpointer user_data);
void on_pbm_invio_dati_activate                     (GtkMenuItem *menuitem, gpointer user_data);
void on_pbm_crea_dischetto_activate                 (GtkMenuItem *menuitem, gpointer user_data);
void on_pbm_reinvio_dati_to_sap_activate            (GtkMenuItem *menuitem, gpointer user_data);
/* DLG_DISTRIBUZIONE */
void on_dlg_distribuzione_pb_ok_clicked             (GtkButton *button, gpointer user_data);
void on_dlg_distribuzione_pb_cancel_clicked         (GtkButton *button, gpointer user_data);
void on_dlg_distribuzione_pb_lancia_clicked         (GtkButton *button, gpointer user_data);
void on_dlg_distribuzione_pb_statistiche_clicked    (GtkButton *button, gpointer user_data);
void on_dlg_distribuzione_pb_elimina_elaborazione   (GtkButton *button, gpointer user_data);
void on_dlg_distribuzione_pb_anticipa_consuntivo    (GtkButton *button, gpointer user_data);
/* DLG_EXPAND */
void on_dlg_expand_pb_ok_clicked                    (gpointer user_data,GtkButton *button);
void on_dlg_expand_pb_cancel_clicked                (gpointer user_data,GtkButton *button);
/* DLG_MESSAGE */
void on_dlg_message_pb_ok_clicked                   (GtkButton *button, gpointer user_data);
void on_dlg_message_pb_cancel_clicked               (gpointer user_data, GtkButton *button);
void on_dlg_message_sock_display_msg_box            (GtkButton *button, gpointer user_data);
void on_dlg_message_pb_reset_settori                (GtkButton *button, gpointer user_data);
void on_dlg_message_elimina_elaborazione_clicked    (GtkButton *button, gpointer user_data);
void do_reinvio_dati_to_sap                         (GtkButton *button, gpointer user_data);
void do_invio_dati_to_sap                           (GtkButton *button, gpointer user_data);
void do_copia_dati_to_floppy_for_sap                (GtkButton *button, gpointer user_data);
/* DLG_EXPORT */
void on_dlg_export_pb_cancel_clicked                (gpointer user_data,GtkButton *button);
void on_dlg_export_do_spedizione_cedola             (GtkButton *button, gpointer user_data);
/* DLG_APP_ERROR */
void on_dlg_app_error_pb_ok_clicked                 (GtkButton *button, gpointer user_data);
/* DLG_IMPORT */
void on_dlg_import_pb_cancel_clicked                (gpointer user_data,GtkButton *button);
void on_dlg_import_do_ricezione_cedola              (GtkButton *button, gpointer user_data);
/* DLG_IMPORT_FILE */
void on_dlg_import_file_pb_cancel_clicked           (gpointer user_data,GtkButton *button);
void on_dlg_import_file_do_ricezione_articoli       (GtkButton *button, gpointer user_data);
void on_dlg_import_file_do_ricezione_ubicazioni     (GtkButton *button, gpointer user_data);
/* DLG_IMPORT_ORDINI */
void on_dlg_import_ordini_pb_cancel_clicked         (gpointer user_data,GtkButton *button);
void on_dlg_import_ordini_do_ricezione_ordini       (GtkButton *button, gpointer user_data);
/* DLG_SET_ORDINI */
void on_dlg_set_ordini_pb_ok_clicked                (gpointer user_data,GtkButton *button);
void on_dlg_set_ordini_pb_cancel_clicked            (gpointer user_data,GtkButton *button);
void on_dlg_set_ordini_rb_ricevuto_toggled          (GtkToggleButton *togglebutton, gpointer user_data);
void on_dlg_set_ordini_rb_prelevato_toggled         (GtkToggleButton *togglebutton, gpointer user_data);
void on_dlg_set_ordini_rb_spedito_toggled           (GtkToggleButton *togglebutton, gpointer user_data);
void on_dlg_set_ordini_rb_evaso_toggled             (GtkToggleButton *togglebutton, gpointer user_data);
void on_dlg_set_ordini_rb_elaborato_toggled         (GtkToggleButton *togglebutton, gpointer user_data);
void on_dlg_set_ordini_rb_stampato_xab_toggled      (GtkToggleButton *togglebutton, gpointer user_data);
void on_dlg_set_ordini_rb_stampato_rac_toggled      (GtkToggleButton *togglebutton, gpointer user_data);
void on_dlg_set_ordini_rb_stampata_distinta_toggled (GtkToggleButton *togglebutton, gpointer user_data);
void on_dlg_set_ordini_rb_in_prelievo_toggled       (GtkToggleButton *togglebutton, gpointer user_data);
void on_dlg_set_ordini_rb_spedito_host_toggled      (GtkToggleButton *togglebutton, gpointer user_data);
/*DLG_RESET_SETTORE*/
void on_dlg_reset_settore_pb_resetta_clicked        (GtkButton *button, gpointer user_data);
void on_dlg_reset_settore_pb_cancel_clicked         (gpointer user_data,GtkButton *button);
/* DLG_LANCIO_DISTRIBUZIONI */
void on_dlg_lancio_distribuzioni_pb_cancel_clicked  (gpointer user_data,GtkButton *button);
/* DLG_SELECT */
void on_dlg_select_pb_cancel_clicked                (gpointer user_data,GtkButton *button);
void on_dlg_select_pb_lancio_clicked                (GtkButton *button, gpointer user_data);
void on_dlg_select_pb_ok_clicked                    (gpointer user_data,GtkButton *button);
void on_dlg_select_pb_expand_clicked                (GtkButton *button, gpointer user_data);
void on_dlg_select_pb_lista_articoli_clicked        (GtkButton *button, gpointer user_data);
void on_dlg_select_pb_reset_sel_clicked             (GtkButton *button, gpointer user_data);
void on_dlg_select_pb_select_all_clicked            (GtkButton *button, gpointer user_data);
void on_dlg_select_pb_start_sel_clicked             (GtkButton *button, gpointer user_data);
void on_dlg_select_pb_and_clicked                   (GtkButton *button, gpointer user_data);
void on_dlg_select_pb_or_clicked                    (GtkButton *button, gpointer user_data);
/* DLG_CARICO */
void on_dlg_carico_settori_pb_apply_clicked         (GtkButton *button, gpointer user_data);
void on_dlg_carico_settori_pb_cancel_clicked        (gpointer user_data,GtkButton *button);
void on_salva_stato_linea_pb_ok_clicked             (GtkButton *button, gpointer user_data);
void on_carica_stato_linea_pb_ok_clicked            (GtkButton *button, gpointer user_data);
/* DLG_EXPORT_MESSAGE */
void on_dlg_export_message_pb_cancel_clicked        (gpointer user_data,GtkButton *button);
void on_dlg_export_message_add                      (GtkButton *button, gpointer user_data);
void on_dlg_export_message_overwrite                (GtkButton *button, gpointer user_data);
/* STORICIZZAZIONE DATI */
void do_storicizzazione_dati                        (GtkButton *button, gpointer user_data);
/* DLG_TEST_IOS */
void on_pkdis_pb_send_clicked                       (gpointer user_data,GtkButton *button);
void on_dlg_test_ios_pb_ok_clicked                  (gpointer user_data,GtkButton *button);
void on_dlg_test_ios_pb_cancel_clicked              (gpointer user_data,GtkButton *button);
/* RICERCA STORICO */
void on_dlg_storico_pb_cancel_clicked               (gpointer user_data,GtkButton *button);
void on_dlg_ricerca_storico_pb_ok_clicked           (GtkButton *button, gpointer user_data);
/* DLG_STAMPA */
void on_dlg_stampa_storico_pb_ok_clicked            (GtkButton *button, gpointer user_data);
void on_dlg_stampa_pb_stampa_clicked                (GtkButton *button, gpointer user_data);
/* DLG_STAT_PRELIEVO */
void on_dlg_stat_prelievo_pb_ok_clicked             (gpointer user_data,GtkButton *button);
void on_dlg_stat_prelievo_cal_from_date_day_selected(GtkCalendar *calendar, gpointer user_data);
void on_dlg_stat_prelievo_cal_to_date_day_selected  (GtkCalendar *calendar, gpointer user_data);
/* DLD_SET_PRIO */
void on_dlg_set_prio_pb_ok_clicked                  (gpointer user_data,GtkButton *button);
/* DLG_OPERATORI */
void on_dlg_operatori_pb_modifica_op_clicked        (GtkButton *button, gpointer user_data);
void on_dlg_operatori_pb_aggiungi_op_clicked        (GtkButton *button, gpointer user_data);
void on_dlg_operatori_pb_elimina_op_clicked         (GtkButton *button, gpointer user_data); 
void on_dlg_operatori_pb_assegna_locazione_clicked  (GtkButton *button, gpointer user_data); 
void on_dlg_operatori_pb_vouta_locazione_clicked    (GtkButton *button, gpointer user_data);
void on_dlg_operatori_do_elimina_operatore          (GtkButton *button, gpointer user_data);
void on_dlg_lancio_evasione_pb_lancio_clicked       (gpointer user_data,GtkButton *button); 
void on_dlg_lancio_consuntivo_pb_lancio_clicked     (gpointer user_data,GtkButton *button); 
void on_dlg_distribuzione_pb_lancia_evadi_clicked   (GtkButton *button, gpointer user_data);
void do_lancio_elaborazione                         (GtkButton *button, gpointer user_data);
void do_lancio_elaborazione_senza_documento         (GtkButton *button, gpointer user_data);
void do_lancio_evasione_elaborazione                (GtkButton *button, gpointer user_data);
void do_lancio_consuntivo_elaborazione              (GtkButton *button, gpointer user_data);
void do_consuntivo_elaborazione                     (GtkButton *button, gpointer user_data);

void do_invio_dati_bertello                         (void);

/* DLG_PRODOTTI */
void on_dlg_prodotti_pb_modifica_pr_clicked         (gpointer user_data,GtkButton *button);
void on_dlg_prodotti_pb_aggiungi_pr_clicked         (gpointer user_data,GtkButton *button);
void on_dlg_prodotti_pb_elimina_pr_clicked          (gpointer user_data,GtkButton *button);
void on_dlg_prodotti_pb_cerca_pr_clicked            (gpointer user_data,GtkButton *button);
void do_elimina_prodotto                            (GtkButton *button, gpointer user_data);
void do_elimina_prodotto_pericoloso                 (GtkButton *button, gpointer user_data);


void do_start_impianto(void);
void do_stop_impianto(void);

void on_dlg_start_impianto_pb_ok_clicked    (gpointer user_data,GtkButton *button);
void on_dlg_stop_impianto_pb_ok_clicked     (gpointer user_data,GtkButton *button);

void on_pbm_forza_ordine_senza_documento_activate (GtkMenuItem     *menuitem, gpointer         user_data);

void on_pbm_stampa_documenti_activate       (GtkMenuItem     *menuitem, gpointer         user_data);

void on_dlg_distribuzione_pb_lancia_senza_documento_clicked (GtkButton       *button, gpointer         user_data);

void on_pbm_seleziona_elaborazione_activate (GtkMenuItem     *menuitem, gpointer         user_data);

void on_dlg_select_elaborazione_pb_ok_clicked (gpointer user_data,GtkButton *button);

void
on_dlg_distribuzione_pb_lancia_consuntivo_clicked
                                        (GtkButton       *button,
                                        gpointer         user_data);

void on_dlg_work_pb_cancel_clicked          (GtkButton       *button, gpointer         user_data);

void on_lst_righe_collo_select_row(GtkCList *clist, gint row, gint column, GdkEvent *event, gpointer user_data);
void on_lst_righe_collo_unselect_row(GtkCList *clist, gint row, gint column, GdkEvent *event, gpointer user_data);

void on_pb_qta_errata_clicked         (gpointer user_data,GtkButton *button);

void on_pb_mancante_clicked           (gpointer user_data,GtkButton *button);

void on_pb_inversione_clicked         (gpointer user_data,GtkButton *button);

void on_pb_riga_non_richiesta_clicked (gpointer user_data,GtkButton *button);

void on_pb_annulla_errore_clicked     (gpointer user_data,GtkButton *button);

void on_dlg_distribuzione_pb_fuori_pallet_clicked(gpointer user_data,GtkButton *button);
void on_dlg_distribuzione_pb_pallet_clicked(gpointer user_data,GtkButton *button);
void do_marca_elaborazione_fuori_pallet(GtkButton *button, gpointer user_data);
void do_marca_elaborazione_pallet(GtkButton *button, gpointer user_data);


void on_dlg_prodotti_pericolosi_pb_modifica_clicked (gpointer user_data,GtkButton *button);
void on_dlg_prodotti_pericolosi_pb_aggiungi_clicked (gpointer user_data,GtkButton *button);
void on_dlg_prodotti_pericolosi_pb_elimina_clicked (gpointer user_data,GtkButton *button);

void
on_pbm_prodotti_pericolosi_activate    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_dlg_distribuzione_pb_amazon_clicked (GtkButton       *button,
                                        gpointer         user_data);

void
on_dlg_distribuzione_pb_normale_clicked
                                        (GtkButton       *button,
                                        gpointer         user_data);
