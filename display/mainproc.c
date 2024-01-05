/*
* mainproc.c
* work procedures 
* Easy Picking 3.0
* Copyright A&L srl 1999-2000
* Autore : Roberto Mantovani
*/

#include <gtk/gtk.h>

#include <time.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

#ifdef TRACE
	#include <trace.h>
#endif

#include <ep-common.h>
#include <ep-db.h>
#include <gtk-support.h>
#include <dbfun-gtk.h>
#include <shared.h>
#include <pmx_msq.h>

#include <proc_list.h>
#include <picking.h>

#include "main.h"
#include "mainstruct.h"
#include "mainext.h"
#include "mainfun.h"
#include "callbacks.h"

//extern char __version__[];


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
	int nStatoLinea;
	int nAttesaBadge;
	BOOL bMemOK=FALSE;

	if(bFirstTime){
		RefreshStaticElements();
		RefreshAsDisconnected();
	}

	/*
	* Controllo lo stato della Trasmissione/Ricezione
	*/
	/* ogni secondo */
	if(lActualTime<time((time_t *)0)){
		char szBuffer[256];
		strcpy(szBuffer,Cfg.szPathData);
		strcat(szBuffer,"/");
		strcat(szBuffer,Cfg.szDisplayCfgFileName);

		gtk_label_pango_printf (GTK_LABEL(get_widget(main_window,"lb_ora")),szBuffer,"%s",GetDatef("%R"));

		time(&lActualTime);
		/*
		* Verifico la shared memory per la gestione della linea
		*/
		if((CheckShm(Cfg.nLineaShmKey,sizeof(LINEA_STRUCT))==0)||(CheckShm(Cfg.nSettoriShmKey,Cfg.nNumeroSettori*sizeof(SETTORE))==0)){

			if(pDatiLinea||pSettori){
				RefreshAsDisconnected();
				DetachShm((char *)pDatiLinea);
				DetachShm((char *)pSettori);
				pDatiLinea=NULL;
				pSettori=NULL;
			}
			bMemOK=FALSE;
		} else {
			/*
			* Gestione stati linea
			*/
			if(pDatiLinea==NULL){
				connect_linea_shm(&pDatiLinea);
			}
			if(pSettori==NULL){
				connect_settori_shm(&pSettori);
			}
			bMemOK=TRUE;
			switch(Cfg.nIsola) {
				case 1:
					nStatoLinea=pDatiLinea->nStatoLinea_1;
				break;
				case 2:
					nStatoLinea=pDatiLinea->nStatoLinea_2;
				break;
			}
			if(nStatoLinea!=nOldStatoLinea || nAttesaBadge != nOldAttesaBadge){

				nOldStatoLinea = nStatoLinea;
				nOldAttesaBadge = nAttesaBadge;
				RefreshLineaElements();

			}
			
		}

#ifdef STATISTICS
	  if((nActualFrame==FRAME_STATISTICHE) && ((lActualTime%30)==0)){
			RefreshStatisticheElements();
		}
#endif
		
	}

	/*
	* Gestione settore
	*/
	if(bMemOK){
		if(memcmp(&pSettori[Cfg.nSettoreIndex],&OldSettore,sizeof(SETTORE))!=0){
			
			// trace_out_vstr(1,"Cambiato settore [%d]",Cfg.nSettoreIndex);
			RefreshDisplayAsVoid();
			gtk_update();

			usleep(300000);

			memcpy(&OldSettore,&pSettori[Cfg.nSettoreIndex],sizeof(SETTORE));
			RefreshSettoreElements();
			if(nActualFrame==FRAME_DATI_PRODOTTO){
				RefreshDatiProdottoElements();
			}
			gtk_update();

			if (Cfg.bMaximize){
				// gtk_window_maximize(GTK_WINDOW(main_window));
				gtk_window_fullscreen(GTK_WINDOW(main_window));
			}

		}
	}
	

	/*
	* Messaggi da altri processi
	*/
	while((nCarIn = ReceiveMsgNoWait(DisplayList[Cfg.nSettoreIndex].nQNumber, &nAllMsg, szInMsg))>=0){
		UnpackStructMsg(szInMsg, &InMsgStruct, szText, nCarIn);

		trace_out_vstr_date(1, "Msg [%d] [%s] from [%s]",InMsgStruct.code,strlen(szText)?szText:"",ProcList[InMsgStruct.srce-1].szProcName);

		/*
		* notifica di Start/Stop Processi
		*/
		switch (InMsgStruct.srce-1){
			case PROC_SETTORI:
				switch (InMsgStruct.code){
				}
			break;
			case PROC_MAIN:
				switch (InMsgStruct.code){
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
