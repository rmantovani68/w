/*
* main.c
* main module 
* Easy Picking 3.0
* Copyright CNI srl 1999-2000
* Autore : Roberto Mantovani
*/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <unistd.h>
#include <signal.h>
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
#include "picking.h"
#include "mainstruct.h"
#include "mainext.h"
#include "mainfun.h"

int main (int argc, char *argv[])
{
#ifdef TRACE
	char szBufTrace[80];
#endif

  gtk_set_locale ();
	gtk_rc_parse ("../data/isola.rc");
  gtk_init (&argc, &argv);

	/* Predispongo l'utilizzo dell'uscita di sicurezza */
	signal(SIGTERM, ExitApp);
	signal(SIGINT,ExitApp);
	signal(SIGQUIT,ExitApp);
	signal(SIGILL,ExitApp);
	signal(SIGKILL,ExitApp);

	/* 
	* ignoro il segnale di uscita applicazione child 
	* (stampe in background)
	*/
	signal(SIGCHLD, SIG_IGN);

	/*
	* GTK-Error handling functions
	*/
	// g_set_error_handler((GErrorFunc)gtk_error_handler);
	// g_set_warning_handler((GWarningFunc)gtk_warning_handler);
	// g_set_message_handler((GPrintFunc)gtk_message_handler);

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
	ReadConfiguration(TRUE);

	/*
	* Lettura configurazione tabelle utilizzate
	*/
	ReadTablesInfo();


#ifdef TRACE
	trace_out_vstr(1,"Local Module ID : %d",PROC_ISOLA);
	trace_out_vstr(1,"Local Queue ID  : %d",ProcList[PROC_ISOLA].nQKey);
#endif

	/*
	* Verifico la presenza di una istanza attiva del processo MAIN
	* controllando la presenza della coda MSG
	*/
	if((ProcList[PROC_ISOLA].nQNumber=OpenMsgQ(ProcList[PROC_ISOLA].nQKey))<0){
		/*
		* creo la coda locale per MAIN
		*/
		if((ProcList[PROC_ISOLA].nQNumber= CreateMsgQ(ProcList[PROC_ISOLA].nQKey, 1))<0){
	#ifdef TRACE
			trace_out_vstr_date(1,"Creazione coda msg MAIN (%d) fallita", ProcList[PROC_ISOLA].nQKey);
	#endif
			_exit(FALSE);
		}

		main_window = create_main_window ();
		gtk_widget_show (main_window);

		sw_list=get_widget(main_window,"sw_list");
		txt_msgs=get_widget(main_window,"txt_msgs");

		ConnectDataBase(&DBConn);

		RunSimpleProcess(PROC_PRINTLABEL);
		RunSimpleProcess(PROC_SOCK);

		/*
		* Creazione e riempimento lista ordini
		*/
		RefreshListaOrdini(sw_list,NULL);

		/*
		* Timer per gestione eventi
		*/
		gtk_timeout_add(Cfg.nMainDelay,ProcessMsgs,NULL);
		/*
		* Timer per refresh lista
		*/
		if(Cfg.nRefreshDelay){
			gtk_timeout_add(Cfg.nRefreshDelay,RefreshTimeOut,NULL);
		}
	} else {
		GtkWidget *dlg_app_error;

		dlg_app_error=create_dlg_app_error();
		gtk_window_set_focus (GTK_WINDOW (dlg_app_error), get_widget(dlg_app_error,"pb_ok"));
		gtk_widget_show (dlg_app_error);
	}

	if(Cfg.nSAP){
		gtk_text_printf(NULL,txt_msgs,"Modalita' SAP\n");
#ifdef TRACE
	trace_out_vstr_date(1,"Modalita' SAP");
#endif
	}

  gtk_main ();
  return 0;
}
