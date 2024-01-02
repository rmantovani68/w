/*
* mainproc.c
* work procedures 
* Easy Picking 3.0
* Copyright CNI srl 1999-2000
* Autore : Roberto Mantovani
*/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <libpq-fe.h>

#include "interface.h"
#include "support.h"

#ifdef TRACE
	#include <trace.h>
#endif
#include <proc_list.h>
#include <ca_file.h>
#include <pmx_msq.h>
#include <dbfun.h>

#include "picking.h"
#include "mainstruct.h"
#include "mainext.h"
#include "mainfun.h"
#include "callbacks.h"

/*
* Refresh a tempo
*/
gint RefreshTimeOut( gpointer data )
{
	GtkWidget *sw_list=get_widget(main_window,"sw_list");

	RefreshListaOrdini(sw_list,NULL);
	/* 
	* As this is a timeout function, return TRUE so that it
	* continues to get called 
	*/
	return(TRUE);
}

/*
* Gestione Eventi (Messaggi e PLC)
*/
gint ProcessMsgs( gpointer data )
{
	int nAllMsg = 0;
	int nCarIn;
	char szInMsg[INFO_LEN];
	char szText[INFO_LEN];
	MSGHDRINT InMsgStruct;
	static time_t lActualTime=0;
	static BOOL bFirstTime=TRUE;
	//static LINEA_STRUCT OldLineData;

	if(bFirstTime){
		gtk_text_printf(NULL,txt_msgs,"CNI - Easy Picking 3.0 - Application Ready\n");
	}

	
	if(lActualTime<time((time_t *)0)){
		time(&lActualTime);
	}

	/*
	* Messaggi da altri processi
	*/
	while((nCarIn = ReceiveMsgNoWait(ProcList[PROC_ISOLA].nQNumber, &nAllMsg, szInMsg))>=0){
		UnpackStructMsg(szInMsg, &InMsgStruct, szText, nCarIn);


		/*
		* notifica di Start/Stop Processi
		*/
		switch (InMsgStruct.code){
			case PROGRAM_STARTED:
				trace_out_vstr_date(1, "%s Started", ProcList[InMsgStruct.srce-1].szProcName);
				switch (InMsgStruct.srce-1){
					case PROC_SOCK:
					{
						char szBuffer[128];

						sprintf(szBuffer,"%d",Cfg.nServicePort);
						SendMessage(PROC_SOCK,PROC_ISOLA,SOCK_OPEN_SERVICE,szBuffer);
					}
					break;
				}
			break;
			case PROGRAM_STOPPED:
				trace_out_vstr_date(1, "%s Stopped", ProcList[InMsgStruct.srce-1].szProcName);
			break;
			case SOCK_CMD_DISPLAY_MSGBOX:
			{
				GtkWidget *lb_msg;
				GtkWidget *pb_ok;

				dlg_message=create_dlg_message();

				lb_msg=get_widget(dlg_message,"lb_msg");
				pb_ok=get_widget(dlg_message,"pb_ok");

				gtk_label_printf(lb_msg,szText);
				gtk_window_set_title (GTK_WINDOW (dlg_message), "Messaggio operatore");
				gtk_window_set_focus (GTK_WINDOW (dlg_message), get_widget(dlg_message,"pb_ok"));
				gtk_widget_show(dlg_message);
				gtk_window_set_transient_for(GTK_WINDOW(dlg_message),GTK_WINDOW(main_window));
			}
			break;
			case SOCK_CMD_UPDATE_ORDINE:
			{
				char szOrdProg[128];
				char cStato;
				char *pPtr;
				char *szSeparator=",\t\n";

				/* Ordine */ if((pPtr=strtok(szText,szSeparator))!=NULL){ strcpy(szOrdProg,pPtr); }
				/* Stato  */ if((pPtr=strtok(  NULL,szSeparator))!=NULL){ cStato=pPtr[0]; }

				UpdateOrdine(szOrdProg);
			}
			break;
			case SOCK_CMD_DISPLAY_MSG:
			case DISPLAY_MSG:
			{
				GdkColor *pcColor;

				switch (InMsgStruct.srce-1){
					case PROC_PRINTLABEL: pcColor=&cBLUE;break;
					case PROC_SOCK: pcColor=&cORANGE;break;
					default: pcColor=&cBLACK;break;
				}
				gtk_text_printf(pcColor,txt_msgs,szText);
			}
			break;
		}

		switch (InMsgStruct.srce-1){
			case PROC_PRINTLABEL:
				switch (InMsgStruct.code){
					case PRINTLABEL_RAC_STAMPATO:
					break;
				}
			break;
		}
	}

	if(bFirstTime){
		bFirstTime=FALSE;
	}
	/* 
	* As this is a timeout function, return TRUE so that it
	* continues to get called 
	*/
	return(TRUE);
}
