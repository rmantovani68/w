/*
* peso.c
* modulo principale
* Gestione Peso/Fine Linea
* Easy Picking 3.0
* Copyright CNI srl 1999-2000
* Autore : Roberto Mantovani
*/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <signal.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <libpq-fe.h>


#ifdef TRACE
	#include <trace.h>
#endif
#include <proc_list.h>
#include <ca_file.h>
#include <shared.h>
#include <pmx_msq.h>
#include <dbfun.h>

#include "interface.h"
#include "support.h"

#include "plcsock.h"

#include "picking.h"
#include "pesostruct.h"
#include "pesoext.h"
#include "pesofun.h"

int main (int argc, char *argv[])
{
	int nPLCError;
#ifdef TRACE
	char szBufTrace[80];
#endif

  gtk_set_locale ();
	gtk_rc_parse ("../data/peso.rc");
  gtk_init (&argc, &argv);

	/* Predispongo l'utilizzo dell'uscita di sicurezza */
	signal(SIGTERM, ExitApp);
	signal(SIGINT,ExitApp);
	signal(SIGQUIT,ExitApp);
	signal(SIGILL,ExitApp);
	signal(SIGKILL,ExitApp);

	/* 
	* ignoro il segnale di broken pipe (PLC)
	*/
	signal(SIGPIPE, SIG_IGN);

	/* 
	* ignoro il segnale di uscita applicazione child 
	* (stampe in background)
	*/
	signal(SIGCHLD, SIG_IGN);

	/*
	* GTK-Error handling functions
	*/
	g_set_error_handler((GErrorFunc)gtk_error_handler);
	g_set_warning_handler((GWarningFunc)gtk_warning_handler);
	g_set_message_handler((GPrintFunc)gtk_message_handler);

	add_pixmap_directory (PACKAGE_BASE_DIR "/pixmaps");

#ifdef TRACE
	/* Apro il file trace */
	sprintf(szBufTrace,"%s%s",argv[0],TRACE_FILE_SUFFIX);
	open_trace(argv[0], szBufTrace, TRACE_FILE_MAXSIZE);

	trace_out_vstr_date(1,"Started");
	trace_out_vstr(1,"Module Name     : %s",argv[0]);
#endif

	/*
	* Lettura configurazione
	*/
	ReadConfiguration();

	/*
	* Lettura configurazione tabelle utilizzate
	*/
	ReadTablesInfo();


#ifdef TRACE
	trace_out_vstr(1,"Local Module ID : %d",PROC_PESO);
	trace_out_vstr(1,"Local Queue ID  : %d",ProcList[PROC_PESO].nQKey);
#endif

	/*
	* Verifico la presenza di una istanza attiva del processo PESO
	* controllando la presenza della coda MSG
	*/
	if((ProcList[PROC_PESO].nQNumber=OpenMsgQ(ProcList[PROC_PESO].nQKey))<0){
		/*
		* creo la coda locale per PESO
		*/
		if((ProcList[PROC_PESO].nQNumber= CreateMsgQ(ProcList[PROC_PESO].nQKey, 1))<0){
#ifdef TRACE
			trace_out_vstr_date(1,"Creazione coda msg PESO (%d) fallita", ProcList[PROC_PESO].nQKey);
#endif
			exit(FALSE);
		}
		main_window = create_main_window ();
		gtk_widget_show (main_window);

		/*
		* Dati per il debug
		*/
		dlg_debug_var=create_dlg_debug_var();
		lst_var_list=get_widget(dlg_debug_var,"lst_var_list");
		gtk_widget_hide(dlg_debug_var);

		/*
		* Dati per inserimento barcode o peso
		*/
		dlg_barcode=create_dlg_barcode();
		gtk_widget_hide(dlg_barcode);

		sw_list=get_widget(main_window,"sw_list");
		lb_status_msg=get_widget(main_window,"lb_status_msg");
		txt_msgs=get_widget(main_window,"txt_msgs");

		/*
		* Apertura del canale di comunicazione con il PLC server (QPLC)
		*/
		if(Cfg.nPLC){
			if((nPLCError=PLCConnect(&fdPLC,Cfg.szPLCHost))==FALSE){
				gtk_label_printf( lb_status_msg, "Errore in connessione PLC");
				Cfg.nPLC=0;
				dlg_plc_error = create_dlg_plc_error();
				gtk_window_set_focus (GTK_WINDOW (dlg_plc_error), get_widget(dlg_plc_error,"pb_ok"));
				gtk_widget_show (dlg_plc_error);
				gtk_window_set_transient_for(GTK_WINDOW(dlg_plc_error),GTK_WINDOW(main_window));
			}
		}

		ConnectDataBase(&DBConn);

		RunSimpleProcess(PROC_BARCODE);
		RunSimpleProcess(PROC_BILANCIA);
		RunSimpleProcess(PROC_BILSIM);
		RunSimpleProcess(PROC_PRINTLABEL);

		/*
		* Creazione e riempimento lista ordini
		*/
		RefreshListaColli(sw_list,NULL);
		
		/*
		* Disattivo la stampa degli ordini
		*/	
		gtk_widget_set_sensitive(get_widget(main_window,"pbm_stampa_ordine"),FALSE);

		/*
		* Timer per gestione eventi
		*/
		gtk_timeout_add(Cfg.nPesoDelay,ProcessMsgs,NULL);
	} else {
		GtkWidget *dlg_app_error;

		dlg_app_error=create_dlg_app_error();
		gtk_window_set_focus (GTK_WINDOW (dlg_app_error), get_widget(dlg_app_error,"pb_ok"));
		gtk_widget_show (dlg_app_error);
	}
	if(Cfg.nSAP){
#ifdef TRACE
		trace_out_vstr_date(1,"Modalità SAP");
#endif
		gtk_text_printf(NULL,txt_msgs,"Modalità SAP\n");
	}

  gtk_main ();
  return 0;
}
