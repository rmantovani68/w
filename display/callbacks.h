/*
* callbacks.h
* Callbacks functions declaration
* Easy Picking 3.0
* Copyright CNI srl 1999-2002
* Autori : Roberto Mantovani
*          Stefano Tarroni
*/

#include <gtk/gtk.h>
// #include "picking.h"
#include "proc_list.h"

gboolean on_main_window_destroy_event (GtkWidget *widget, GdkEvent *event, gpointer user_data) ;

void do_about(void);
void do_exit(void);

void on_main_pb_statistica_evasione_clicked (GtkButton       *button, gpointer         user_data);

void on_item1_activate                      (GtkMenuItem     *menuitem, gpointer         user_data);

void on_pbm_exit_activate                   (GtkMenuItem     *menuitem, gpointer         user_data);

void on_main_pb_sx_clicked                  (GtkButton       *button, gpointer         user_data);

void on_main_pb_ok_clicked                  (GtkButton       *button, gpointer         user_data);

void on_main_pb_dx_clicked                  (GtkButton       *button, gpointer         user_data);
void do_shift_frame_sx(void);
void do_shift_frame_dx(void);
void do_aggiorna_frame(void);

gboolean
on_main_window_delete_event            (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

gboolean
on_main_window_destroy_event           (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

gboolean on_main_window_key_press_event (GtkWidget *widget, GdkEventKey *event, gpointer user_data);
void do_simula_luce(void);

void
on_main_window_activate_default        (GtkWindow       *window,
                                        gpointer         user_data);

void
on_entry_lettura_barcode_activate      (GtkEntry        *entry,
                                        gpointer         user_data);
void do_lettura_barcode_id_prodotto(char *szBarcode);
