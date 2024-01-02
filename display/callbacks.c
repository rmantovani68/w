/*
* callbacks.c
* Callbacks functions
* Easy Picking 3.0
* Copyright A&L srl 1999-2001
* Autori : Roberto Mantovani
*/


#include <gnome.h>

#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#ifdef TRACE
#include <trace.h>
#endif

#include <pmx_msq.h>
#include <ep-common.h>
#include <ep-db.h>
#include <dbfun-gtk.h>
#include <gtk-support.h>
#include <shared.h>

#include <proc_list.h>
#include <picking.h>

#include "callbacks.h"
#include "interface.h"

#include "mainstruct.h"
#include "mainext.h"
#include "mainfun.h"


#define LST (get_widget(main_window,"lst"))

gboolean on_main_window_delete_event            (GtkWidget       *widget, GdkEvent        *event, gpointer         user_data)
{

  return FALSE;
}

gboolean on_main_window_destroy_event (GtkWidget *widget, GdkEvent *event, gpointer user_data) 
{
	do_exit();
  return FALSE;
}


/*
* ------------------------------------------------------------
*                   INIZIO MENU CALLBACKS
* ------------------------------------------------------------
*/

void on_pbm_exit_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	do_exit();
}



/*
* ------------------------------------------------------------
*                   FINE MENU CALLBACKS
* ------------------------------------------------------------
*/

/*
* ------------------------------------------------------------
*                   INIZIO TOOLBOX CALLBACKS
* ------------------------------------------------------------
*/

/*
* ------------------------------------------------------------
*                   FINE TOOLBOX CALLBACKS
* ------------------------------------------------------------
*/

/*
* ------------------------------------------------------------
*                   INIZIO DIALOGS CALLBACKS
* ------------------------------------------------------------
*/


/*
* ------------------------------------------------------------
*                   FINE DIALOG BOX CALLBACKS
* ------------------------------------------------------------
*/

/*
* ------------------------------------------------------------
*                   INIZIO UTILITY FUNCTIONS (DO_...)
* ------------------------------------------------------------
*/

void do_exit(void)
{
	GtkWidget *dlg;
	int nRC;

	dlg = gtk_message_dialog_new (GTK_WINDOW(main_window),0,GTK_MESSAGE_QUESTION,GTK_BUTTONS_YES_NO,"Uscire dall'applicazione ?");
	nRC = gtk_dialog_run(GTK_DIALOG (dlg));
	switch (nRC){
		case GTK_RESPONSE_YES:
			ExitApp();
		break;
	}
	gtk_widget_destroy(dlg);
}

extern char rcsid[];
extern char __version__[];
extern char __customer__[];
extern char __copyright__[];
extern char __authors__[];


void do_about(void)
{
	GtkWidget *dlg = create_dlg_about();

	gtk_label_printf(get_widget(dlg,"lb_version"), __version__);
	gtk_label_printf(get_widget(dlg,"lb_customer"), __customer__);
	gtk_label_printf(get_widget(dlg,"lb_rcsid"), rcsid);
	gtk_label_printf(get_widget(dlg,"lb_authors"), __authors__);
	gtk_label_printf(get_widget(dlg,"lb_copyright"), __copyright__);

	gtk_dialog_run(GTK_DIALOG (dlg));
	gtk_widget_destroy(dlg);
}








void on_main_pb_sx_clicked                  (GtkButton       *button, gpointer         user_data)
{
	do_shift_frame_sx();
}


void on_main_pb_ok_clicked                  (GtkButton       *button, gpointer         user_data)
{
}


void on_main_pb_dx_clicked                  (GtkButton       *button, gpointer         user_data)
{
	do_shift_frame_dx();
}

void do_shift_frame_sx(void)
{
	if(nActualFrame>0){
		nActualFrame=nActualFrame-1;
	} else {
		nActualFrame=NUMERO_FRAME-1;
	}
	do_aggiorna_frame();
}

void do_shift_frame_dx(void)
{
	if(nActualFrame==NUMERO_FRAME-1){
		nActualFrame=0;
	} else {
		nActualFrame=nActualFrame+1;
	}
	do_aggiorna_frame();
}

void do_aggiorna_frame(void)
{
	switch(nActualFrame){
		case FRAME_DATI_PRODOTTO:
			gtk_widget_show(get_widget(main_window,"frame_dati_prodotto"));
			gtk_widget_hide(get_widget(main_window,"frame_statistiche"));
			//gtk_widget_hide(get_widget(main_window,"frame_lettura_barcode_id"));
		break;
#ifdef STATISTICS
		case FRAME_STATISTICHE:
			gtk_widget_hide(get_widget(main_window,"frame_dati_prodotto"));
			gtk_widget_show(get_widget(main_window,"frame_statistiche"));
			RefreshStatisticheElements();
		break;
#endif
	}

}


gboolean on_main_window_key_press_event (GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
	trace_debug(NULL,TRUE,TRUE,NULL,"key_event: %d - ", event->keyval);

	switch (event->keyval){
		
		case GDK_Num_Lock:             trace_debug(NULL,FALSE,FALSE,NULL,"GDK_Num_Lock");                           break;
		case GDK_KP_Space:             trace_debug(NULL,FALSE,FALSE,NULL,"GDK_KP_Space");                           break;
		case GDK_KP_Tab:               trace_debug(NULL,FALSE,FALSE,NULL,"GDK_KP_Tab");                             break;
		case GDK_KP_Enter:             trace_debug(NULL,FALSE,FALSE,NULL,"GDK_KP_Enter");                           break;
		case GDK_KP_F1:                trace_debug(NULL,FALSE,FALSE,NULL,"GDK_KP_F1");                              break;
		case GDK_KP_F2:                trace_debug(NULL,FALSE,FALSE,NULL,"GDK_KP_F2");                              break;
		case GDK_KP_F3:                trace_debug(NULL,FALSE,FALSE,NULL,"GDK_KP_F3");                              break;
		case GDK_KP_F4:                trace_debug(NULL,FALSE,FALSE,NULL,"GDK_KP_F4");                              break;
		case GDK_KP_Home:              trace_debug(NULL,FALSE,FALSE,NULL,"GDK_KP_Home");                            break;
		case GDK_KP_Left:              trace_debug(NULL,FALSE,FALSE,NULL,"GDK_KP_Left");                            break;
		case GDK_KP_Up:                trace_debug(NULL,FALSE,FALSE,NULL,"GDK_KP_Up");                              break;
		case GDK_KP_Right:             trace_debug(NULL,FALSE,FALSE,NULL,"GDK_KP_Right");                           break;
		case GDK_KP_Down:              trace_debug(NULL,FALSE,FALSE,NULL,"GDK_KP_Down");                            break;
		case GDK_KP_Page_Up:           trace_debug(NULL,FALSE,FALSE,NULL,"GDK_KP_Page_Up");                         break;
		case GDK_KP_Page_Down:         trace_debug(NULL,FALSE,FALSE,NULL,"GDK_KP_Page_Down");                       break;
		case GDK_KP_End:               trace_debug(NULL,FALSE,FALSE,NULL,"GDK_KP_End");                             break;
		case GDK_KP_Begin:             trace_debug(NULL,FALSE,FALSE,NULL,"GDK_KP_Begin");                           break;
		case GDK_KP_Insert:            trace_debug(NULL,FALSE,FALSE,NULL,"GDK_KP_Insert");                          break;
		case GDK_KP_Delete:            trace_debug(NULL,FALSE,FALSE,NULL,"GDK_KP_Delete");                          break;
		case GDK_KP_Equal:             trace_debug(NULL,FALSE,FALSE,NULL,"GDK_KP_Equal");                           break;
		case GDK_KP_Multiply:          trace_debug(NULL,FALSE,FALSE,NULL,"GDK_KP_Multiply");                        break;
		case GDK_KP_Add:               trace_debug(NULL,FALSE,FALSE,NULL,"GDK_KP_Add");                             break;
		case GDK_KP_Separator:         trace_debug(NULL,FALSE,FALSE,NULL,"GDK_KP_Separator");                       break;
		case GDK_KP_Subtract:          trace_debug(NULL,FALSE,FALSE,NULL,"GDK_KP_Subtract");                        break;
		case GDK_KP_Decimal:           trace_debug(NULL,FALSE,FALSE,NULL,"GDK_KP_Decimal");                         break;
		case GDK_KP_Divide:            trace_debug(NULL,FALSE,FALSE,NULL,"GDK_KP_Divide");                          break;
		case GDK_KP_0:                 trace_debug(NULL,FALSE,FALSE,NULL,"GDK_KP_0");                               break;
		case GDK_KP_1:                 trace_debug(NULL,FALSE,FALSE,NULL,"GDK_KP_1");                               break;
		case GDK_KP_2:                 trace_debug(NULL,FALSE,FALSE,NULL,"GDK_KP_2");                               break;
		case GDK_KP_3:                 trace_debug(NULL,FALSE,FALSE,NULL,"GDK_KP_3");                               break;
		case GDK_KP_4:                 trace_debug(NULL,FALSE,FALSE,NULL,"GDK_KP_4");                               break;
		case GDK_KP_5:                 trace_debug(NULL,FALSE,FALSE,NULL,"GDK_KP_5");                               break;
		case GDK_KP_6:                 trace_debug(NULL,FALSE,FALSE,NULL,"GDK_KP_6");                               break;
		case GDK_KP_7:                 trace_debug(NULL,FALSE,FALSE,NULL,"GDK_KP_7");                               break;
		case GDK_KP_8:                 trace_debug(NULL,FALSE,FALSE,NULL,"GDK_KP_8");                               break;
		case GDK_KP_9:                 trace_debug(NULL,FALSE,FALSE,NULL,"GDK_KP_9");                               break;
		case GDK_percent:              trace_debug(NULL,FALSE,FALSE,NULL,"GDK_percent");                        break;
	}


	/*
	* Tasti sempre disponibili
	*/
	switch (event->keyval){
		/* tastierino usb - tasto non documentato ... /usr/include/gtk-2.0/gdk/gdkkeysyms.h */
		/* 
		 * LG: 65293 dovrebbe essere il GDK_Return e non si puo' piu' usare
		 * perche' altrimenti la lettura del barcode fa passare la luce!!!
		 */
		//case 65293:
		case GDK_KP_Enter:
		case GDK_KP_Multiply:
			trace_debug(NULL,FALSE,FALSE,NULL,"SIMULA LUCE");
			do_simula_luce();
		break;
		case GDK_KP_Subtract:
			trace_debug(NULL,FALSE,FALSE,NULL,"STOP LINEA");
//			do_stop_linea();
		break;
		case GDK_KP_Add:
			trace_debug(NULL,FALSE,FALSE,NULL,"START LINEA");
//			do_start_linea();
		break;
		case GDK_KP_Right:
//			do_shift_frame_dx();
		break;
		case GDK_KP_Left:
//			do_shift_frame_sx();
		break;
		/*
		* il formato di lettura del badge e' il seguente 
		* %<TEXT>?
		*/

 		case GDK_plus:
 		case GDK_percent:
			trace_debug(NULL,FALSE,FALSE,NULL,"LETTURA BARCODE");
			gtk_entry_set_text (GTK_ENTRY(get_widget(main_window,"entry_lettura_barcode")), "");
			gtk_widget_grab_focus (get_widget(main_window,"entry_lettura_barcode"));
		break;
	}
	
  return FALSE;
}

void do_simula_luce(void)
{
	char szSettore[128];

	/* Apro la coda messaggi di settori */
	if((ProcList[PROC_SETTORI].nQNumber = OpenMsgQ(ProcList[PROC_SETTORI].nQKey))<0){
#ifdef TRACE
		trace_out_vstr(1, "Apertura coda messaggi principale fallita");
#endif
 	}

	sprintf(szSettore,"%d,%d",pSettori[Cfg.nSettoreIndex].nIsola,pSettori[Cfg.nSettoreIndex].nSettore);
	SendMessage(ProcList,PROC_SETTORI, PROC_MAIN, SIMULA_LUCE, szSettore);
}


void
on_main_window_activate_default        (GtkWindow       *window,
                                        gpointer         user_data)
{

}


void on_entry_lettura_barcode_activate      (GtkEntry        *entry, gpointer         user_data)
{
	char szBarcode[128];

	strcpy(szBarcode,gtk_entry_get_text(GTK_ENTRY(get_widget(main_window,"entry_lettura_barcode"))));
#ifdef TRACE
	trace_out_vstr(1, "Lettura barcode: %s",szBarcode);
#endif
	do_lettura_barcode_id_prodotto(szBarcode);

}


void do_lettura_barcode_id_prodotto(char *szBarcode)
{
	char szText[256];
	char szBarcodeRipulito[256];
	int nSettore;
	int nIsola;

	/* Apro la coda messaggi di settori */
	if((ProcList[PROC_SETTORI].nQNumber = OpenMsgQ(ProcList[PROC_SETTORI].nQKey))<0){
#ifdef TRACE
		trace_out_vstr(1, "Apertura coda messaggi principale fallita");
#endif
 	}
	/*
	 * Tolgo il % davanti al barcode
	 */
	strcpy(szBarcodeRipulito,szBarcode+1);

	sprintf(szText,"%d,%d,",pSettori[Cfg.nSettoreIndex].nIsola, pSettori[Cfg.nSettoreIndex].nSettore);
	strcat(szText,szBarcodeRipulito);

	SendMessage(ProcList,PROC_SETTORI, PROC_MAIN, BARCODE_ID_PRODOTTO, szText);
}
