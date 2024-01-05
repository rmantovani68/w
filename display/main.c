/*
* main.c
* main module 
* Easy Picking 3.0
* Copyright A&L srl 2003
* Autore : Roberto Mantovani
*/

//#include <gnome.h>
#include <gtk/gtk.h>

#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef TRACE
	#include <trace.h>
#endif
#include <ep-common.h>
#include <ep-db.h>
#include <shared.h>
#include <pmx_msq.h>
#include <dbfun-gtk.h>
#include <gtk-support.h>

#include <proc_list.h>
#include <picking.h>

#include "main.h"
#include "interface.h"
#include "mainstruct.h"
#include "mainext.h"
#include "mainfun.h"
#include "callbacks.h"

extern char rcsid[];
extern char __version__[];
extern char __module_name__[];
extern char __module_version__[];
extern char __customer__[];
extern char __authors__[];
extern char __copyright__[];
extern char __configuration_file__[];

int main (int argc, char *argv[])
{
	int nOpt;
	int nSettoreIndex=0;
	int nSettore=0;
	int nIsola=0;
	int nErr=0;
	BOOL bExit=FALSE;
	BOOL bMaximize=FALSE;
#ifdef TRACE
	char szBufTrace[80];
#endif
	char szBuffer[512];

	gchar **remaining_args = NULL;
	gint arg_settore=0;
	gint arg_settore_index=0;
	gint arg_isola=0;
	gboolean arg_massimizza=FALSE;
	GOptionEntry option_entries[] = {
		/* ... your application's command line options go here ... */
		/* last but not least a special option that collects filenames */
		{ "settore", 's', 0, G_OPTION_ARG_INT, &arg_settore, NULL, NULL },
		{ "indice", 'n', 0, G_OPTION_ARG_INT, &arg_settore_index, NULL, NULL },
		{ "isola", 'i', 0, G_OPTION_ARG_INT, &arg_isola, NULL, NULL },
		{ "massimizza", 'm', 0, G_OPTION_ARG_NONE, &arg_massimizza, NULL, NULL },
		{ G_OPTION_REMAINING, 0, 0, G_OPTION_ARG_FILENAME_ARRAY, &remaining_args, "Special option that collects any remaining arguments for us" },
		{ NULL }
	};

	GOptionContext *option_context;
	// GnomeProgram *my_app;

	option_context = g_option_context_new ("display");
	gtk_init(&argc, &argv);

	/* if you are using any libraries that have command line options
	* of their own and provide a GOptionGroup with them, you can
	* add them here as well using g_option_context_add_group()
	*/

	/* now add our own application's command-line options. If you
	* are using gettext for translations, you should be using
	* GETTEXT_PACKAGE here instead of NULL
	*/
	g_option_context_add_main_entries (option_context, option_entries, NULL);

	/* We assume PACKAGE and VERSION are set to the program name and version
	* number respectively. Also, assume that 'option_entries' is a global
	* array of GOptionEntry structures.
	*/
	// my_app = gtk_program_init(PACKAGE, VERSION, LIBGNOMEUI_MODULE, argc, argv, GNOME_PARAM_GOPTION_CONTEXT, option_context, GNOME_PARAM_NONE);

	nSettoreIndex=arg_settore_index;
	nSettore=arg_settore;
	nIsola=arg_isola;
	Cfg.bMaximize=arg_massimizza;


	Cfg.nSettore=nSettore;
	Cfg.nIsola=nIsola;
	Cfg.nSettoreIndex=nSettoreIndex;


	gtk_rc_parse ("../data/display.rc");	
	// gnome_program_init("display", VERSION, LIBGNOMEUI_MODULE, argc, argv, GNOME_PARAM_NONE);

	/* Predispongo l'utilizzo dell'uscita di sicurezza */
	signal(SIGTERM, ExitApp);
	signal(SIGINT,ExitApp);
	signal(SIGQUIT,ExitApp);
	signal(SIGILL,ExitApp);
	signal(SIGKILL,ExitApp);
	signal(SIGSEGV,SIG_IGN);

	/* 
	* ignoro il segnale di uscita applicazione child 
	* (stampe in background)
	*/
	signal(SIGCHLD, SIG_IGN);
	/* 
	* Ignoro il SIGPIPE (applicazione chiamate)
	*/
	signal(SIGPIPE, SIG_IGN);

	/*
	* GTK-Error handling functions
	*/
	/*
	g_set_error_handler((GErrorFunc)gtk_error_handler);
	g_set_warning_handler((GWarningFunc)gtk_warning_handler);
	g_set_message_handler((GPrintFunc)gtk_message_handler);
	*/

	add_pixmap_directory (PACKAGE_BASE_DIR "/pixmaps");


	/*
	* Lettura configurazione
	*/
	if(ReadConfiguration(__configuration_file__,TRUE)){
		gchar *szTitle;

		memset(&OldSettore,0,sizeof(SETTORE));
		nOldStatoLinea=LINEA_IN_STOP;
		nActualFrame=FRAME_DATI_PRODOTTO;

#ifdef TRACE
		/* Apro il file trace */
		sprintf(szBufTrace,"%s/%s_%d%s",Cfg.szPathTrace,__module_name__,nSettoreIndex,TRACE_FILE_SUFFIX);
		open_trace(__module_name__, szBufTrace, PICKING_TRACE_FILE_MAXSIZE);

		trace_out_vstr(1,"Module Name     : %s",__module_name__);
		trace_out_vstr(1,"Version         : %s",__version__);
		trace_out_vstr(1,"Customer Name   : %s",__customer__);
		trace_out_vstr(1,"Authors         : %s",__authors__);
		trace_out_vstr(1,"Copyright       : %s",__copyright__);
		trace_out_vstr(1,"RCSID String    : %s",rcsid);
		trace_out_vstr_date(1,"Started");
#endif



		/* Apro la coda messaggi principale */
		if((ProcList[PROC_MAIN].nQNumber = OpenMsgQ(ProcList[PROC_MAIN].nQKey))<0){
#ifdef TRACE
			trace_out_vstr(1, "%s : Apertura coda messaggi principale fallita", argv[0]);
#endif
  	}

		/* Apro la coda messaggi di settori */
		if((ProcList[PROC_SETTORI].nQNumber = OpenMsgQ(ProcList[PROC_SETTORI].nQKey))<0){
#ifdef TRACE
			trace_out_vstr(1, "%s : Apertura coda messaggi principale fallita", argv[0]);
#endif
  	}
						
						


		/*
		* alloco le shared memory di settori e linea
		*/
		if(!connect_settori_shm(&pSettori)||!connect_linea_shm(&pDatiLinea)){
			RefreshAsDisconnected();
		}

		main_window = create_main_window ();

		g_signal_connect ((gpointer) main_window, "key_press_event", G_CALLBACK (on_main_window_key_press_event), NULL);

		szTitle=g_strdup_printf("Easy Picking %s-%s - Display", __version__, __customer__);
		gtk_window_set_title (GTK_WINDOW (main_window), szTitle);
		g_free(szTitle);
		do_aggiorna_frame();


		/*
		* Creazione coda messaggi locale
		*/
		DisplayList[Cfg.nSettoreIndex].nGId=PROC_DISPLAY+Cfg.nSettoreIndex;
		if((DisplayList[Cfg.nSettoreIndex].nQNumber = CreateMsgQ(ProcList[PROC_DISPLAY].nQKey+Cfg.nSettoreIndex, 1))<0){
#ifdef TRACE
			trace_out_vstr(1,"Creazione coda msg %s (%d) fallita",
			ProcList[PROC_DISPLAY].szProcName,
			ProcList[PROC_DISPLAY].nQKey+Cfg.nSettoreIndex);
#endif
		}
				

		if (Cfg.bMaximize){
			// gtk_window_maximize(GTK_WINDOW(main_window));
			gtk_window_fullscreen(GTK_WINDOW(main_window));
		}

		gtk_widget_show (main_window);


#ifdef STATISTICS
		/*
		* Connessione Database 
		*/
		if(!DBConnect(Cfg.szDBHost, Cfg.szDBPort, Cfg.szDBName)){
			trace_debug("RED",TRUE,TRUE,NULL,"Connessione al database [%s:%s] fallita.", Cfg.szDBHost,Cfg.szDBName);
		}
#endif

		/*
		* avverto MAIN dello start
		*/
		sprintf(szBuffer,"%d",Cfg.nSettoreIndex);
		SendMessage(ProcList,PROC_MAIN,PROC_DISPLAY, PROGRAM_STARTED, szBuffer);

		/*
		* Timer per gestione eventi
		*/
		Cfg.nTimer = g_timeout_add(Cfg.nMainDelay,ProcessMsgs,NULL);
		
		Cfg.nBlinkingTimer = g_timeout_add(Cfg.nBlinkingDelay,Blinking,NULL);


	} else {
		GtkWidget *dlg;

		dlg = gtk_message_dialog_new (GTK_WINDOW(main_window),0,GTK_MESSAGE_WARNING,GTK_BUTTONS_CLOSE,"Dati di configurazione non presenti");
		gtk_dialog_run(GTK_DIALOG (dlg));
		gtk_widget_destroy(dlg);
		bExit=TRUE;
	}
	if(bExit){
		exit(1);
	}

  gtk_main ();
  return 0;
}
