#include <gtk/gtk.h>

void on_lst_ordini_select_row(GtkCList *clist, gint row, gint column, GdkEvent *event, gpointer user_data);

void on_lst_sort_column_clicked (GtkCList *clist, gint nColumnNumber, gpointer user_data);

gboolean on_main_window_destroy_event(GtkWidget * widget, GdkEvent * event, gpointer user_data);

void on_mb_main_activate_current(GtkMenuShell * menushell, gboolean force_hide, gpointer user_data);

void on_pbm_exit_activate(GtkMenuItem * menuitem, gpointer user_data);

void on_pbm_aggiorna_lista_activate(GtkMenuItem * menuitem, gpointer user_data);

void on_pbm_visualizza_messaggi_activate(GtkMenuItem * menuitem, gpointer user_data);

void on_pbm_configurazione_activate(GtkMenuItem * menuitem, gpointer user_data);

void on_pbm_stampa_etichette_activate(GtkMenuItem * menuitem, gpointer user_data);

void on_pbm_stampa_etichetta_collo_activate(GtkMenuItem * menuitem, gpointer user_data);

void on_pbm_stampa_ordine_activate(GtkMenuItem * menuitem, gpointer user_data);

void on_pbm_edit_ordine_activate(GtkMenuItem * menuitem, gpointer user_data);

void on_pbm_distribuzione_activate(GtkMenuItem * menuitem, gpointer user_data);

void on_pbm_about_activate(GtkMenuItem * menuitem, gpointer user_data);

void on_pbm_help_activate(GtkMenuItem * menuitem, gpointer user_data);

void on_main_pb_config_clicked(GtkButton * button, gpointer user_data);

void on_main_pb_print_clicked(GtkButton * button, gpointer user_data);

void on_main_pb_help_clicked(GtkButton * button, gpointer user_data);

void on_main_pb_edit_clicked(GtkButton * button, gpointer user_data);

void on_main_pb_exit_clicked(GtkButton * button, gpointer user_data);

void on_dlg_about_pb_ok_clicked(GtkButton * button, gpointer user_data);

void on_dlg_exit_pb_ok_clicked(GtkButton * button, gpointer user_data);

void on_dlg_exit_pb_cancel_clicked(GtkButton * button, gpointer user_data);

void on_dlg_message_pb_cancel_clicked(GtkButton * button, gpointer user_data);

void on_dlg_distribuzione_pb_ok_clicked(GtkButton * button, gpointer user_data);

void on_dlg_distribuzione_pb_lancia_clicked(GtkButton * button, gpointer user_data);

void on_dlg_distribuzione_pb_cancel_clicked(GtkButton * button, gpointer user_data);

void on_dlg_edit_ordine_pb_ok_clicked(GtkButton * button, gpointer user_data);

void on_dlg_edit_ordine_pb_edit_collo_clicked(GtkButton * button, gpointer user_data);

void on_dlg_edit_ordine_pb_prev_clicked(GtkButton * button, gpointer user_data);

void on_dlg_edit_ordine_pb_next_clicked(GtkButton * button, gpointer user_data);

void on_dlg_edit_ordine_pb_cancel_clicked(GtkButton * button, gpointer user_data);

void on_dlg_config_pb_ok_clicked(GtkButton * button, gpointer user_data);

void on_dlg_config_pb_apply_clicked(GtkButton * button, gpointer user_data);

void on_dlg_config_pb_cancel_clicked(GtkButton * button, gpointer user_data);

void on_dlg_edit_collo_pb_ok_clicked(GtkButton * button, gpointer user_data);

void on_dlg_edit_collo_pb_prev_clicked(GtkButton * button, gpointer user_data);

void on_dlg_edit_collo_pb_next_clicked(GtkButton * button, gpointer user_data);

void on_dlg_edit_collo_pb_cancel_clicked(GtkButton * button, gpointer user_data);

void on_dlg_app_error_pb_ok_clicked(GtkButton * button, gpointer user_data);

void on_main_pb_settori_clicked(GtkButton *button, gpointer user_data);

void on_dlg_carico_settori_pb_apply_clicked(GtkButton * button, gpointer user_data);

void on_dlg_carico_settori_pb_cancel_clicked(GtkButton * button, gpointer user_data);

void on_dlg_stampa_ordine_pb_cancel_clicked (GtkButton       *button, gpointer         user_data);
void do_stampa_packing_list     (GtkButton       *button, gpointer         user_data);
void do_stampa_etichette     (GtkButton       *button, gpointer         user_data);
void do_stampa_pdf (GtkButton       *button, gpointer         user_data);

void on_pbm_stampa_pdf_activate             (GtkMenuItem     *menuitem, gpointer         user_data);
