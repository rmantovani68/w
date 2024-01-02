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
#include <stdlib.h>
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

extern char rcsid[];
extern char __version__[];
extern char __customer__[];
extern char __authors__[];
extern char __copyright__[];

int main (int argc, char *argv[])
{
	PGresult *PGRes;
#ifdef TRACE
	char szBufTrace[80];
#endif
	gchar *szTitle;

  gtk_set_locale ();
	gtk_rc_parse ("../data/main.rc");
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
	//g_set_error_handler((GErrorFunc)gtk_error_handler);
	//g_set_warning_handler((GWarningFunc)gtk_warning_handler);
	//g_set_message_handler((GPrintFunc)gtk_message_handler);

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
	trace_out_vstr(1,"Local Module ID : %d",PROC_MAIN);
	trace_out_vstr(1,"Local Queue ID  : %d",ProcList[PROC_MAIN].nQKey);
#endif

	/*
	* Verifico la presenza di una istanza attiva del processo MAIN
	* controllando la presenza della coda MSG
	*/
	if((ProcList[PROC_MAIN].nQNumber=OpenMsgQ(ProcList[PROC_MAIN].nQKey))<0){
		/*
		* creo la coda locale per MAIN
		*/
		if((ProcList[PROC_MAIN].nQNumber= CreateMsgQ(ProcList[PROC_MAIN].nQKey, 1))<0){
	#ifdef TRACE
			trace_out_vstr_date(1,"Creazione coda msg MAIN (%d) fallita", ProcList[PROC_MAIN].nQKey);
	#endif
			_exit(FALSE);
		}
		/*
		* Creo la shared memory per la gestione della linea
		*/
		if((pDatiLinea=(PLINEA_STRUCT)SetupShm(Cfg.nShmKey,sizeof(LINEA_STRUCT)))==NULL){
			GtkWidget *dlg_app_error;
#ifdef TRACE
			trace_out_vstr_date(1,"Cannot Allocate Shared Memory [%d] !", Cfg.nShmKey);
#endif

			dlg_app_error=create_dlg_app_error();
			gtk_widget_show (dlg_app_error);
		} else {
			pDatiLinea->nStatoLinea_1=LINEA_IN_STOP;
			pDatiLinea->nStatoLinea_2=LINEA_IN_STOP;
		}
		main_window = create_main_window ();

		sleep(1);

		szTitle=g_strdup_printf("Easy Picking %s-%s - Gestione Impianto %s", __version__, __customer__, rcsid);
		gtk_window_set_title (GTK_WINDOW (main_window), szTitle);
		g_free(szTitle);

		gtk_widget_show (main_window);

		/*
		* se mi interfaccio con SAP attivo il sottomenu' che permette
		* il reinvio dei dati relativi agli ordini gia' evasi;
		* N.B. -> il normale funzionamento prevede che sia la procedura di pesatura colli a finelinea
		* 				ad occuparsi di spedire a sap i dati degli ordini (comando: cni2sap)
		*/
		if(!Cfg.nSAP){
			gtk_widget_set_sensitive(get_widget(main_window,"pbm_sap"),FALSE);
		}

		sw_list=get_widget(main_window,"sw_list");
		txt_msgs=get_widget(main_window,"txt_msgs");

		if(ConnectDataBase(&DBConn)){
			int nAnno;

			PGRes=PGExecSQL(FALSE,"set client_encoding to SQL_ASCII;");
			PQclear(PGRes);

			/*
			*	- st - 5/12/2000
			* controllo il cambiamento dell'anno per aggiornare il valore del segnacollo
			*/
//#ifdef ELIMINATO_RESET_PROGCOLLO
			PGRes=PGExecSQL(Cfg.nDebugVersion,"select last_value from sequence_anno;");
			nAnno=atoi(PQgetvalue(PGRes,0,0));
			PQclear(PGRes);

			if (atoi(LeftStr(GetDateYYYYMMDD(time((long *)0),szDateBuffer),4))>nAnno){
				nAnno=atoi(LeftStr(GetDateYYYYMMDD(time((long *)0),szDateBuffer),4));
				/*
				* IL SEGNACOLLO INIZIA DA 0 (WELLA)
				*/
				/* resetto le sequenze di progressivo collo e aggiorno l'anno */
				PGRes=PGExecSQL(Cfg.nDebugVersion,"select setval ('sequence_anno',%d);", nAnno);
				PQclear(PGRes);
				PGRes=PGExecSQL(Cfg.nDebugVersion,"select setval ('progcollo_add',2000000);");
				PQclear(PGRes);
				PGRes=PGExecSQL(Cfg.nDebugVersion,"select setval ('progcollo',999999);");
				PQclear(PGRes);
			}
//#endif



			RunSimpleProcess(PROC_VOLUM);
			RunSimpleProcess(PROC_SETTORI);
			RunSimpleProcess(PROC_IOS);
			RunSimpleProcess(PROC_PRINTLABEL);
			RunSimpleProcess(PROC_SOCK);
			RunSimpleProcess(PROC_AUTOTASK);

			/*
			* Creazione e riempimento lista ordini
			*/
			RefreshListaOrdini(sw_list,NULL);
			/*
			* Refresh dati isole
			*/
			RefreshDatiIsola(1,get_widget(main_window,"clist_isola1"));
			RefreshDatiIsola(2,get_widget(main_window,"clist_isola2"));
			RefreshDatiIsola(0,get_widget(main_window,"clist_impianto"));
			RefreshElaborazioni(get_widget(main_window,"clist_lista_elaborazioni"),0);
			RefreshElaborazioni(get_widget(main_window,"clist_lista_elaborazioni_1"),1);
			RefreshElaborazioni(get_widget(main_window,"clist_lista_elaborazioni_2"),2);
			RefreshOperatoriSettori();

			/*
			* Timer per gestione eventi
			*/
			gtk_timeout_add(Cfg.nMainDelay,ProcessMsgs,NULL);
			/*
			* Timer per statistiche isola
			*/
			gtk_timeout_add(Cfg.nSaveStateDelay, RefreshDataTimer, NULL);

			/*
			* Timer per salvataggio stato settori
			*/
			gtk_timeout_add(Cfg.nSaveStateDelay, SaveStateTimer, NULL);
		} else {
			GtkWidget *dlg_app_error;

			dlg_app_error=create_dlg_app_error();
			gtk_window_set_focus (GTK_WINDOW (dlg_app_error), get_widget(dlg_app_error,"pb_ok"));
			gtk_widget_show (dlg_app_error);
		}
	} else {
		GtkWidget *dlg_app_error;

		dlg_app_error=create_dlg_app_error();
		gtk_window_set_focus (GTK_WINDOW (dlg_app_error), get_widget(dlg_app_error,"pb_ok"));
		gtk_widget_show (dlg_app_error);
	}

  gtk_main ();
  return 0;
}
