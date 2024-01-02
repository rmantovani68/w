/*
* pesoproc.c
* work procedures 
* Gestione Peso/Fine Linea
* Easy Picking 3.0
* Copyright CNI srl 1999-2000
* Autore : Roberto Mantovani
*/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
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

#include "plcsock.h"

#include "picking.h"
#include "pesostruct.h"
#include "pesoext.h"
#include "pesofun.h"



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
	static int nStatoLinea=LINEA_IN_START;
	static int nOldStatoLinea=-1;

	if(bFirstTime){
		gtk_text_printf(NULL,txt_msgs,"CNI - Easy Picking 3.0\nGestione Fine Linea\nApplication Ready\n");
	}
	/*
	* Gestione stati linea
	*/
	if(bFirstTime || nStatoLinea!=nOldStatoLinea){

		nOldStatoLinea = nStatoLinea;

		if(nStatoLinea==LINEA_IN_START){
			gtk_label_printf(get_widget(main_window,"lb_stato_linea"),"START");
			ChangeTextColor(get_widget(main_window,"lb_stato_linea"),&cGREEN);
			set_pixmap(get_widget(main_window,"pixmap_stato_linea"),"led_green_on.xpm");
		} else if(nStatoLinea==LINEA_IN_STOP){
			gtk_label_printf(get_widget(main_window,"lb_stato_linea"),"STOP");
			ChangeTextColor(get_widget(main_window,"lb_stato_linea"),&cRED);
			set_pixmap(get_widget(main_window,"pixmap_stato_linea"),"led_red_on.xpm");
		}
	}

	if(lActualTime<time((time_t *)0)){
		char szOrdiniFile[128];
		char szColliFile[128];
		int nOrdini,nColli;

		time(&lActualTime);

		/* controllo file Ordini.txt e Colli.txt */
		sprintf(szOrdiniFile,"%s/%s",Cfg.szPathExport,StrTrimAll(Cfg.szExpOrdiniFile));
		sprintf(szColliFile,"%s/%s",Cfg.szPathExport,StrTrimAll(Cfg.szExpColliFile));

		nOrdini=ContaRighe(szOrdiniFile);
		nColli=ContaRighe(szColliFile);

		if(nOrdini){
			// set_pixmap(get_widget(main_window,"pixmap_colli_in_evasione"),"led_green_on.xpm");
		} else {
			// set_pixmap(get_widget(main_window,"pixmap_colli_in_evasione"),"led_green_off.xpm");
		}
		gtk_label_printf(get_widget(main_window,"lb_colli_in_evasione"),"%4d - %4d",nOrdini,nColli);
	}

	/*
	* Messaggi da altri processi
	*/
	while((nCarIn = ReceiveMsgNoWait(ProcList[PROC_PESO].nQNumber, &nAllMsg, szInMsg))>=0){
		UnpackStructMsg(szInMsg, &InMsgStruct, szText, nCarIn);


		/*
		* notifica di Start/Stop Processi
		*/
		switch (InMsgStruct.code){
			case PROGRAM_STARTED:
				trace_out_vstr_date(1, "%s Started", ProcList[InMsgStruct.srce-1].szProcName);
			break;
			case PROGRAM_STOPPED:
				trace_out_vstr_date(1, "%s Stopped", ProcList[InMsgStruct.srce-1].szProcName);
			break;
			case DISPLAY_MSG:
			{
				GdkColor *pcColor;

				switch (InMsgStruct.srce-1){
					case PROC_BARCODE: pcColor=&cRED;break;
					case PROC_BILANCIA: pcColor=&cGREEN;break;
					default: pcColor=&cBLACK;break;
				}
				gtk_text_printf(pcColor,txt_msgs,szText);
			}
			break;
		}

		switch (InMsgStruct.srce-1){
			case PROC_BARCODE:
				switch (InMsgStruct.code){
					case BCRD_RX_BARCODE:
						/*
						* copio il dato nel buffer di ricezione barcode 
						*/
#ifdef TRACE
						trace_out_vstr_date(1,"Arrivato Barcode : %s",szText);
#endif
						strcpy(szBarcodeColloBilancia,szText);
						gtk_label_set_text(GTK_LABEL(get_widget(main_window,"lb_barcode")),szBarcodeColloBilancia);
					break;
					case BCRD_RX_NOREAD:
						gtk_label_printf( lb_status_msg, "Errore di NOREAD da Scanner");
						szBarcodeColloBilancia[0]='\0';
						gtk_label_set_text(GTK_LABEL(get_widget(main_window,"lb_barcode")),"NOREAD");
					break;
					case BCRD_RX_ERROR:
						gtk_label_printf( lb_status_msg, "Errore di Lettura da Scanner");
						szBarcodeColloBilancia[0]='\0';
						gtk_label_set_text(GTK_LABEL(get_widget(main_window,"lb_barcode")),"ERROR");
					break;
				}
			break;

			case PROC_BARCODE_PZ:
				switch (InMsgStruct.code){
					case BCRD_RX_BARCODE:
						/*
						* copio il dato nel buffer di ricezione barcode 
						*/
#ifdef TRACE
						trace_out_vstr_date(1,"Arrivato Barcode Pallet : %s",szText);
#endif
						strcpy(szBarcodeColloPz,szText);
						gtk_label_set_text(GTK_LABEL(get_widget(main_window,"lb_barcode_pz")),szBarcodeColloPz);
					break;
					case BCRD_RX_NOREAD:
						gtk_label_printf( lb_status_msg, "Errore di NOREAD da Scanner Pallet");
						szBarcodeColloPz[0]='\0';
						gtk_label_set_text(GTK_LABEL(get_widget(main_window,"lb_barcode_pz")),"NOREAD");
					break;
					case BCRD_RX_ERROR:
						gtk_label_printf( lb_status_msg, "Errore di Lettura da Scanner Pallet");
						szBarcodeColloPz[0]='\0';
						gtk_label_set_text(GTK_LABEL(get_widget(main_window,"lb_barcode_pz")),"ERROR");
					break;
				}
			break;

			case PROC_BILANCIA:
				switch (InMsgStruct.code){
					case BILANCIA_RX_DATA:
					{
						char szPeso[128];
						char szStatus[128];
						char *pPtr;
						int nPeso;

						gtk_timeout_remove(nBilanciaTimeOut);
						
						/*
						* interpreto il dato ricevuto
						*
						* tara         : da 1 a 7 (compreso) - NON CONSIDERATA
						* netto        : da 8 a 14 (compreso)
						* peso stabile : byte 15
						*/
						strncpy(szPeso,szText+8,7);
						strncpy(szStatus,szText+15,1);
						szPeso[7]='\0';
						szStatus[1]='\0';

						/*
						* ricavo il peso della bilancia in grammi
						*/
						if((pPtr=strchr(szPeso,','))){
							*pPtr='\0';
							nPeso=1000*atoi(szPeso);
							nPeso+=atoi(pPtr+1);
						}


						nPesoColloBilancia=nPeso;
#ifdef TRACE
						trace_out_vstr_date(1,"Arrivata Pesatura Collo : %7.3f Kg Status : %s", (float)nPesoColloBilancia/(float)1000,szStatus);
#endif
						/*
						if(szStatus[0]=='1'){
						*/
						if(szStatus[0]){
							/*
							* Pesatura OK - Peso Stabile
							*/
							gtk_label_printf(get_widget(main_window,"lb_peso_bilancia"),"%7.3f", (float)nPesoColloBilancia/(float)1000);
							if(strlen(szBarcodeColloBilancia)){
								/* 
								*	arrivato un peso stabile dalla bilancia, il barcode del collo e' significativo
								* --> passo al controllo peso
								*/
								ColloInBilancia(szBarcodeColloBilancia,nPesoColloBilancia);
							} else {
								ShowDlgRequest(nPesoColloBilancia,szBarcodeColloBilancia,BARCODE_MSG_BARCODE);
							}
							
						} else {
							/*
							* Pesatura Non corretta - Peso Instabile
							* richiedo un'altra pesata
							*/
#ifdef TRACE
							trace_out_vstr_date(1,"Peso INSTABILE : richiesta peso reiterata");
#endif
							nBilanciaTimeOut=gtk_timeout_add(Cfg.nBilanciaTimeout,RichiestaPesoTimeOut,NULL);
						}
					}
					break;
					case BILANCIA_RX_ERROR:
						gtk_timeout_remove(nBilanciaTimeOut);
						gtk_label_printf( lb_status_msg, "Errore di Ricezione dalla bilancia");
#ifdef TRACE
						trace_out_vstr_date(1,"Errore di Ricezione dalla bilancia");
#endif
					break;
					case BILANCIA_TX_ERROR:
						gtk_timeout_remove(nBilanciaTimeOut);
						gtk_label_printf( lb_status_msg, "Errore di Spedizione alla bilancia");
#ifdef TRACE
						trace_out_vstr_date(1,"Errore di Spedizione alla bilancia");
#endif
					break;
				}
			break;
			case PROC_PRINTLABEL:
				switch (InMsgStruct.code){
				}
			break;
		}
	}
	/*
	* Gestione dei Messaggi PLC (NON LA PRIMA VOLTA)
	*/
	GestPlc(!bFirstTime);

	UpdateVarList(lst_var_list,bFirstTime);

	if(bFirstTime){
		bFirstTime=FALSE;
	}
	/* 
	* As this is a timeout function, return TRUE so that it
	* continues to get called 
	*/
	return(TRUE);
}

gint RichiestaPesoTimeOut( gpointer data )
{
	/*
	* Richiesta Peso alla Bilancia
	*/
	SendMessage(PROC_BILANCIA, PROC_PESO,  MAIN_REQ_WEIGHT, NULL);
	nBilanciaTimeOut=gtk_timeout_add(Cfg.nBilanciaTimeout,BilanciaTimeOut,NULL);

	return(FALSE);
}

gint BilanciaTimeOut( gpointer data )
{
	ShowDlgRequest(0,szBarcodeColloBilancia,BARCODE_MSG_PESO);
	return(FALSE);
}

/*
* Gestione segnalistica PLC
*/
void GestPlc(BOOL bCheck)
{
	int nRC;
	int nNC;
	char szValue[128];
	/*
	* Fronte di salita MSGTX (Msg da PLC)
	*/
	if(PLCVars[nMSGTX].Value.BOOLValue && !PLCVars[nMSGRX].Value.BOOLValue){
		GetPLCMsg(PLCVars[nMSGCODE].Value.BOOLValue,PLCVars[nMSGCLAS].Value.BOOLValue);
		SendCmd(fdPLC,CMD_SET,"MSGRX 0 0 1",TRUE);
	}
	/*
	* Fronte di discesa MSGTX (Msg da PLC)
	*/
	if(!PLCVars[nMSGTX].Value.BOOLValue && PLCVars[nMSGRX].Value.BOOLValue){
		SendCmd(fdPLC,CMD_SET,"MSGRX 0 0 0",TRUE);
	}
	/*
	* Richiesta informazioni sulle variabili registrate per il DEBUG
	*/
	if(Cfg.nPLC){
		if(SendCmd(fdPLC,CMD_MEMCOPY,(char *)NULL,TRUE)){
			GetMemCopyInfo(PLCVars,szRcvBuffer);
		}
	}

	if(bCheck){
	
		if(VarValueChanged(nINICDN)){
			gtk_label_printf( GTK_LABEL(get_widget(main_window,"lb_INICDN")), "%d - %s",PLCVars[nINICDN].Value.DWORDValue, PLCVars[nINICDFD].Value.BOOLValue?"SU PALLET":"FUORI PALLET");
		}

		/*
		* Fronte di salita CPB (collo in bilancia)
		*/
		if(VarValueChanged(nCPB) && PLCVars[nCPB].Value.BOOLValue){
#ifdef TRACE
			trace_out_vstr_date(1,"Fronte di salita CPB (collo in bilancia)");
#endif
			if(ProcList[PROC_BILANCIA].bExecute){
				/*
				* Attivo il timeout di richiesta peso alla bilancia
				*/
				nBilanciaTimeOut=gtk_timeout_add(Cfg.nBilanciaTimeout,RichiestaPesoTimeOut,NULL);
			} else {
				/*
				* Attivo il timeout di richiesta peso manuale (bilancia non presente)
				*/
				nBilanciaTimeOut=gtk_timeout_add(Cfg.nBilanciaTimeout,BilanciaTimeOut,NULL);
			}
		}
		/*
		* Fronte di discesa CPB (collo fuori bilancia)
		*/
		if(VarValueChanged(nCPB) && !PLCVars[nCPB].Value.BOOLValue){
			/*
			* Nascondo il dlg barcode
			*/
			gtk_widget_hide(dlg_barcode);
			gtk_label_printf(get_widget(main_window,"lb_peso_bilancia"),"");
#ifdef TRACE
			trace_out_vstr_date(1,"Fronte di discesa CPB (collo fuori bilancia)");
#endif
			/*
			* Cancello il barcode ed il peso del collo per il collo successivo
			*/
			szBarcodeColloBilancia[0]='\0';
			nPesoColloBilancia=0;

			if(Cfg.nPLC){
				SendCmd(fdPLC,CMD_SET,"CB 0 0 0",TRUE);	/* Forzatura variabile - Collo Buono */
				SendCmd(fdPLC,CMD_SET,"CS 0 0 0",TRUE);	/* Forzatura variabile - Collo Scartato */
				SendCmd(fdPLC,CMD_SET,"FO 0 0 0",TRUE);	/* Forzatura variabile - Fine Ordine */
			}
		}

		/*
		* Fronte di salita CPLZ (collo pallet)
		*/
		if(VarValueChanged(nCPLZ) && PLCVars[nCPLZ].Value.BOOLValue){
#ifdef TRACE
			trace_out_vstr_date(1,"Fronte di salita CPLZ (collo in pallettizzatore)");
#endif
		}
		if(strlen(szBarcodeColloPz) && PLCVars[nCPLZ].Value.BOOLValue && (PLCVars[nRDCPLZ].Value.BOOLValue==0 && PLCVars[nRFCPLZ].Value.BOOLValue==0)){
			nRC=ColloInPallettizzatore(szBarcodeColloPz,&nNC);
			switch(nRC){
				case 1: // collo dentro 
					sprintf(szValue,"NCPLZ 0 0 %08X",nNC);
					SendCmd(fdPLC,CMD_SET,szValue,TRUE);	/* Forzatura variabile - Collo Dentro */
					SendCmd(fdPLC,CMD_SET,"RDCPLZ 0 0 1",TRUE);	/* Forzatura variabile - Collo Dentro */
#ifdef TRACE
					trace_out_vstr_date(1,"Collo [%s] Dentro Pallet",szBarcodeColloPz);
#endif
				break;
				case 0: // collo fuori  
					sprintf(szValue,"NCPLZ 0 0 %08X",nNC);
					SendCmd(fdPLC,CMD_SET,szValue,TRUE);	/* Forzatura variabile - Collo Dentro */
					SendCmd(fdPLC,CMD_SET,"RFCPLZ 0 0 1",TRUE);	/* Forzatura variabile - Collo Fuori */
#ifdef TRACE
					trace_out_vstr_date(1,"Collo [%s] Fuori Pallet",szBarcodeColloPz);
#endif
				break;
			}
			
		}
		/*
		* Fronte di discesa CPB (collo fuori pallet)
		*/
		if(VarValueChanged(nCPLZ) && !PLCVars[nCPLZ].Value.BOOLValue){
#ifdef TRACE
			trace_out_vstr_date(1,"Fronte di discesa CPLZ (collo fuori pallettizzatore)");
#endif
			/*
			* Cancello il barcode ed il peso del collo per il collo successivo
			*/
			szBarcodeColloPz[0]='\0';

			if(Cfg.nPLC){
				SendCmd(fdPLC,CMD_SET,"RDCPLZ 0 0 0",TRUE);	/* Forzatura variabile - Collo Dentro PLZ */
				SendCmd(fdPLC,CMD_SET,"RFCPLZ 0 0 0",TRUE);	/* Forzatura variabile - Collo Fuori PLZ */
				SendCmd(fdPLC,CMD_SET,"NCPLZ 0 0 00000000",TRUE);	/* Forzatura variabile - Numero Collo */
			}
		}
	} else {
		/*
		* La prima volta azzero collo buono e collo scarto e fine ordine
		*/
		if(Cfg.nPLC){
			SendCmd(fdPLC,CMD_SET,"CB 0 0 0",TRUE);	/* Forzatura variabile - Fine lettura collo */
			SendCmd(fdPLC,CMD_SET,"CS 0 0 0",TRUE);	/* Forzatura variabile - Fine lettura collo */
			SendCmd(fdPLC,CMD_SET,"FO 0 0 0",TRUE);	/* Forzatura variabile - Fine lettura collo */
			SendCmd(fdPLC,CMD_SET,"RDCPLZ 0 0 0",TRUE);	/* Forzatura variabile - Collo dentro  */
			SendCmd(fdPLC,CMD_SET,"RFCPLZ 0 0 0",TRUE);	/* Forzatura variabile - Collo fuori */
			SendCmd(fdPLC,CMD_SET,"NCPLZ 0 0 00000000",TRUE);	/* Forzatura variabile - segnacollo */
		}
	}
}

void GetVarsIndex(void)
{
	if((nCPB = GetVarIndex("CPB"))==-1){ gtk_label_printf( lb_status_msg, "Variabile CPB assente"); }
	if((nCB  = GetVarIndex("CB")) ==-1){ gtk_label_printf( lb_status_msg, "Variabile CB assente"); }
	if((nCS  = GetVarIndex("CS")) ==-1){ gtk_label_printf( lb_status_msg, "Variabile CS assente"); }
	if((nFO  = GetVarIndex("FO")) ==-1){ gtk_label_printf( lb_status_msg, "Variabile FO assente"); }

	if((nCPLZ    = GetVarIndex("CPLZ"))==-1)   { gtk_label_printf( lb_status_msg, "Variabile CPLZ assente"); }
	if((nRDCPLZ  = GetVarIndex("RDCPLZ")) ==-1){ gtk_label_printf( lb_status_msg, "Variabile RDCPLZ assente"); }
	if((nRFCPLZ  = GetVarIndex("RFCPLZ")) ==-1){ gtk_label_printf( lb_status_msg, "Variabile RFCPLZ assente"); }
	if((nNCPLZ   = GetVarIndex("NCPLZ")) ==-1) { gtk_label_printf( lb_status_msg, "Variabile NCPLZ assente"); }

	if((nMSGTX    = GetVarIndex("MSGTX")) ==-1){ gtk_label_printf( lb_status_msg,   "Variabile MSGTX   assente"); }
	if((nMSGRX    = GetVarIndex("MSGRX")) ==-1){ gtk_label_printf( lb_status_msg,   "Variabile MSGRX   assente"); }
	if((nMSGCODE  = GetVarIndex("MSGCODE")) ==-1){ gtk_label_printf( lb_status_msg, "Variabile MSGCODE assente"); }
	if((nMSGCLAS  = GetVarIndex("MSGCLAS")) ==-1){ gtk_label_printf( lb_status_msg, "Variabile MSGCLAS assente"); }

	if((nINICDN  = GetVarIndex("INICDN"))  ==-1){ gtk_label_printf( lb_status_msg, "Variabile INICDN assente"); }
	if((nINICDFD = GetVarIndex("INICDFD")) ==-1){ gtk_label_printf( lb_status_msg, "Variabile INICDFD assente"); }
}

void UpdateVarList(GtkWidget *clist,BOOL bRefresh)
{
	static char *FieldData[1][10];
	static BOOL bFirst=TRUE;
	int nIndex=0;

	if(bFirst){
		FieldData[0][0]=malloc(100);
		FieldData[0][1]=malloc(100);
		FieldData[0][2]=malloc(100);
		FieldData[0][3]=malloc(100);
		bFirst=FALSE;
	}

	if(bRefresh){
		gtk_clist_clear(GTK_CLIST(clist));
		nIndex=0;
		while(strlen(PLCVars[nIndex].szName)){
			strcpy(FieldData[0][0],PLCVars[nIndex].szName);
			strcpy(FieldData[0][1],PLCVars[nIndex].szDescription);
			strcpy(FieldData[0][2],PLCTypes[PLCVars[nIndex].usType].lpszName);
			sprintf(FieldData[0][3],"%ld",PLCVars[nIndex].Value.DWORDValue);

			gtk_clist_append( GTK_CLIST(clist), FieldData[0]);

			/*
			* Aggiorno i valori delle variabili
			*/
			PLCVars[nIndex].OldValue.DWORDValue=PLCVars[nIndex].Value.DWORDValue;

			nIndex++;
		}
	} else {
		nIndex=0;
		while(strlen(PLCVars[nIndex].szName)){
			if(PLCVars[nIndex].Value.DWORDValue!=PLCVars[nIndex].OldValue.DWORDValue){
				sprintf(FieldData[0][3],"%ld",PLCVars[nIndex].Value.DWORDValue);
				gtk_clist_set_text( GTK_CLIST(clist), nIndex,3,FieldData[0][3]);
				/*
				* Aggiorno i valori delle variabili
				*/
				PLCVars[nIndex].OldValue.DWORDValue=PLCVars[nIndex].Value.DWORDValue;
			}
			nIndex++;
		}
	}
}

/*
* scrittura variabili PLC utilizzate ...
*/
int WriteVarList(char *szCfgFile,PLCVAR *PLCVars)
{
	int   nIndex;
	char  szBuffer[128];
	char  szField[80];
	char  szParagraph[80];


	strcpy(szParagraph,"Variables");

	/*
	* Cancellazione del paragrafo attuale
	*/
	xncDelFileParagraph(szParagraph, szCfgFile, NULL);

	nIndex=0;
	while(strlen(PLCVars[nIndex].szName)){
		sprintf(szField,"VAR_%02d",nIndex);
		sprintf(szBuffer,"%s,%s",PLCVars[nIndex].szName,PLCVars[nIndex].szDescription);

		xncPutFileString(szParagraph,szField,szBuffer,szCfgFile,NULL);
		nIndex++;
	}

	return nIndex;
}

/*
* lettura variabili PLC utilizzate ...
*/
int ReadVarList(char *szCfgFile,PLCVAR *PLCVars)
{
	int   nIndex;
	char  szBuffer[128];
	char  szField[80];
	char  szParagraph[80];
	char  *pPtr;
	char  *szSeparator=",\t\n";

	/*
	* lettura del file di configurazione
	*/
	strcpy(szParagraph,"Variables");

	nIndex=0;
	sprintf(szField,"VAR_%02d",nIndex);
	while(xncGetFileString(szParagraph,szField,"",szBuffer,sizeof(szBuffer),szCfgFile,NULL)){
		if(strlen(szBuffer)){
			/* Nome Variabile */
			if((pPtr=strtok(szBuffer,szSeparator))!=NULL){
				strcpy(PLCVars[nIndex].szName,pPtr);
			} else {return(-1);}
			/* Titolo Variabile */
			if((pPtr=strtok(NULL,szSeparator))!=NULL){
				strcpy(PLCVars[nIndex].szDescription,pPtr);
			} else {return(-1);}
		}
		nIndex++;
		sprintf(szField,"VAR_%02d",nIndex);
	}
	PLCVars[nIndex].szName[0]='\0';

	return nIndex;
}

BOOL VarValueChanged(int  nVarIndex)
{
	if(memcmp(&PLCVars[nVarIndex].OldValue,&PLCVars[nVarIndex].Value,sizeof(VAR_VALUE))){
		return TRUE;
	}
	return FALSE;
}

int GetVarIndex(char *szVarName)
{
	int nIndex;

	nIndex=0;
	while(strlen(PLCVars[nIndex].szName)){
		if(!strcmp(szVarName,PLCVars[nIndex].szName)){
			/*
			* Trovata la Variabile
			*/
			return nIndex;
		}
		nIndex++;
	}
	return -1;
}

BOOL PLCConnect(int *fdPort,char *szHostName)
{
	if((OpenSocket(fdPort,szHostName,PLC_SERVICE_PORT))==0){
		return FALSE;
	}

	CheckPLCResponse(*fdPort,TRUE);

	/* versione del PLC */
	if(SendCmd(*fdPort,CMD_VER,(char *)NULL,TRUE)){	
		GetPLCVersion(szRcvBuffer);
	}

	/* status del PLC */
	if(SendCmd(*fdPort,CMD_STATUS,(char *)NULL,TRUE)){
		GetPLCStatus(szRcvBuffer);
	}

	/*
	* Lettura dati variabili
	*/
	GetVarsInfo(*fdPort,PLCVars);

	/*
	* Registrazione delle variabili
	*/
	SendVarListMemChk(*fdPort,PLCVars);
	/*
	* Memorizzo gli indici nel vettore interno
	*/
	GetVarsIndex();

	if(Cfg.nPLC){
		SendCmd(*fdPort,CMD_SET,"CB 0 0 0",TRUE);	/* Forzatura variabile */
		SendCmd(*fdPort,CMD_SET,"CS 0 0 0",TRUE);	/* Forzatura variabile */
		SendCmd(*fdPort,CMD_SET,"FO 0 0 0",TRUE);	/* Forzatura variabile */
	}

	return TRUE;
}

