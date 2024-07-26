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
#include <dirent.h>

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

gint RefreshDataTimer( gpointer data )
{
#ifdef TRACE
	trace_out_vstr_date(1, "RefreshDataTimer: INIZIO");
#endif
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
#ifdef TRACE
	trace_out_vstr_date(1, "RefreshDataTimer: FINE");
#endif

	return TRUE;
}

gint SaveStateTimer( gpointer data )
{
	char szFileName[128];

#ifdef TRACE
	trace_out_vstr_date(1, "SaveStateTimer: INIZIO");
#endif

	if(pDatiLinea->nStatoLinea_1==LINEA_IN_START || pDatiLinea->nStatoLinea_2==LINEA_IN_START){
		sprintf(szFileName,"%s/settori.stato.save",Cfg.szPathData);
		SendMessage(PROC_SETTORI, PROC_MAIN, SAVE_STATE, szFileName);
	}
	/*
	* rilancio il timeout per la successiva chiamata alla funzione
	*/
#ifdef TRACE
	trace_out_vstr_date(1, "SaveStateTimer: FINE");
#endif
	return TRUE;
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
	static LINEA_STRUCT OldLineData;
	char szBolla[256];
	char szPathPdfReceive[256];
	char szPathPdfStore[256];
	char cOPFLPDF;
	char szFileName[256];
	DIR *dir;
	struct dirent *files_in_dir;
	PGresult *PGRes=NULL;
	PGresult *PGResSelect=NULL;
	PGresult *PGResUpdate=NULL;
	char szCommand[256];
	BOOL stato_rx;
	static BOOL old_stato_rx;

	if(bFirstTime){
		gtk_text_printf(NULL,txt_msgs,"A&L - Easy Picking 3.0 - Application Ready\n");
	}
	/*
	* Gestione stati linea
	* rm : differenziato per isola
	*/

	if(bFirstTime || pDatiLinea->nStatoLinea_1!=OldLineData.nStatoLinea_1){

		OldLineData.nStatoLinea_1 = pDatiLinea->nStatoLinea_1;

		if(pDatiLinea->nStatoLinea_1==LINEA_IN_START){
			gtk_label_printf(get_widget(main_window,"lb_stato_linea_1"),"START");
			ChangeTextColor(get_widget(main_window,"lb_stato_linea_1"),&cGREEN);
			//set_pixmap(get_widget(main_window,"pixmap_stato_linea_1"),"led_green_on.xpm");
			gtk_image_set_from_stock(GTK_IMAGE(get_widget(main_window,"pixmap_stato_linea_1")), "gtk-refresh" ,GTK_ICON_SIZE_BUTTON);
		} else if(pDatiLinea->nStatoLinea_1==LINEA_IN_STOP){
			gtk_label_printf(get_widget(main_window,"lb_stato_linea_1"),"STOP");
			ChangeTextColor(get_widget(main_window,"lb_stato_linea_1"),&cRED);
			//set_pixmap(get_widget(main_window,"pixmap_stato_linea_1"),"led_red_on.xpm");
			gtk_image_set_from_stock(GTK_IMAGE(get_widget(main_window,"pixmap_stato_linea_1")), "gtk-no" ,GTK_ICON_SIZE_BUTTON);
		}
	}
	if(bFirstTime || pDatiLinea->nStatoLinea_2!=OldLineData.nStatoLinea_2){

		OldLineData.nStatoLinea_2 = pDatiLinea->nStatoLinea_2;

		if(pDatiLinea->nStatoLinea_2==LINEA_IN_START){
			gtk_label_printf(get_widget(main_window,"lb_stato_linea_2"),"START");
			ChangeTextColor(get_widget(main_window,"lb_stato_linea_2"),&cGREEN);
			//set_pixmap(get_widget(main_window,"pixmap_stato_linea_2"),"led_green_on.xpm");
			gtk_image_set_from_stock(GTK_IMAGE(get_widget(main_window,"pixmap_stato_linea_2")), "gtk-refresh" ,GTK_ICON_SIZE_BUTTON);
		} else if(pDatiLinea->nStatoLinea_2==LINEA_IN_STOP){
			gtk_label_printf(get_widget(main_window,"lb_stato_linea_2"),"STOP");
			ChangeTextColor(get_widget(main_window,"lb_stato_linea_2"),&cRED);
			//set_pixmap(get_widget(main_window,"pixmap_stato_linea_2"),"led_red_on.xpm");
			gtk_image_set_from_stock(GTK_IMAGE(get_widget(main_window,"pixmap_stato_linea_2")), "gtk-no" ,GTK_ICON_SIZE_BUTTON);
		}
	}

	stato_rx = DatiDaRicevere();

	if(bFirstTime || old_stato_rx!=stato_rx){
		gtk_image_set_from_stock(GTK_IMAGE(get_widget(main_window,"pixmap_data_rx")), stato_rx ? "gtk-yes" : "gtk-no" ,GTK_ICON_SIZE_BUTTON);
	}

	old_stato_rx = stato_rx;


	if(lActualTime<time((time_t *)0)){
		time(&lActualTime);
		/*
		*	Ogni 10 secondi
		*/
		if(lActualTime%10==0){
			do_invio_dati_bertello();
		}

		/*
		*	Ogni 5 secondi
		*/
		if(lActualTime%5==0){

/*******************************************************
* VERSIONE CON GESTIONE PDF
********************************************************/
		
			/*
			* Cerco nuovi files di stampa Pdf e aggiorno il db
			*/
#ifdef TRACE
			if(Cfg.nDebugVersion>3){ trace_out_vstr_date(1,"Ricerca nuovi file PDF ... \n");}
#endif

			sprintf(szPathPdfReceive,"%s",Cfg.szPathExportPdf);
			sprintf(szPathPdfStore,"%s",Cfg.szPathStorePdf);
			dir = opendir (szPathPdfReceive);
			if (dir != NULL) {
#ifdef TRACE
				//trace_out_vstr_date(Cfg.nDebugVersion>3,"Dir %s PDF ricevuti esistente ... \n",szPathPdfReceive);
#endif
				//
				// Faccio un giro sui file nella directory di ricezione dei pdf
				// rm+lg 06-09-2017 : limitato a 5 il numero di pdf processati ogni tot secondi
				// lg 2017-10-10 : rimesso originale su richiesta di fabio
				//
				int nFilesIndex=0;
				// Da usare nel caso si vogliano limitare ad un certo num max gli update e spostamenti di PDF
				// da fare in una volta. Non e' mai stato provato il nFilesIndex=1, forse sarebbe andato
				// meglio?
				// TODO: magari spostare tutta sta roba in un autotask per wella, in cui mettere tanta roba ora completata
				// da main. A main mandare solo dei messaggi per l'update a video.
				//while (nFilesIndex<5 && (files_in_dir = readdir (dir))){
				while ((files_in_dir = readdir (dir))){
					strcpy(szFileName,files_in_dir->d_name);
#ifdef TRACE
					//trace_out_vstr_date(Cfg.nDebugVersion>3,"File %s in Dir %s PDF ricevuti esistente ... \n",szFileName,szPathPdfReceive);
#endif
					strcpy(szBolla,SubStr(szFileName,0,10));

					PGResSelect=PGExecSQL(FALSE,"select ordprog from ric_ord where ordprog='%s';",szBolla);
					if(PQntuples(PGResSelect)==1){

						// Faccio l'update di ric_ord
						PGRes=PGExecSQL(Cfg.nDebugVersion>1,"update ric_ord set roflpdf='F' where ordprog='%s';",szBolla);
						if(PQresultStatus(PGRes) == PGRES_COMMAND_OK && PQcmdTuples(PGRes)!=0){ 
#ifdef TRACE
							trace_out_vstr_date(1,"Ordine [%s]: Trovato documento pdf tipo [F] \n",szBolla);
#endif
							/*
								* LG 2017-06-30
								* Sono cambiate un po' di cose. Con Coty le stampe PDF non sono piu' landscape ma portrait.
								* Purtroppo il file che mandano ha i margini piccolissimi e non c'e' verso di allargare i 
								* margini di stampa. Allora uso ghostscript, con questi parametri:
								* -q (non scrive l'output del comando sullo standard output)
								* -o (file di output)
								* -sDEVICE=pdfwrite (dice il tipo di device di output)
								* -dDEVICEWIDTHPOINTS=623 -dDEVICEHEIGHTPOINTS=842 (qui il discorso e' che un A4 ha misure 595pts × 842pts 210mm × 297mm
								*       quindi se voglio rendere piu' largo il doc di 10 mm => 595+28 dove 28 e' un cm in punti)
								* -sPAPERSIZE=a4 (o dell formato dell'output)
								* -dFIXEDMEDIA (costringe il doc al formato A4)
								* -dPDFFitPage (costringe il doc al formato A4)
								*
								* Quindi ora non faccio piu' una move ma creo un nuovo file in pdfstore poi cancello l'originale in pdf
								*
								*
								*/
							// Sposto il pdf nello store
							//sprintf(szCommand,"mv %s/%s %s/%s\n",szPathPdfReceive,szFileName,szPathPdfStore,szFileName);
							//system(szCommand);

							sprintf(szCommand,"gs -q -o %s/%s -sDEVICE=pdfwrite -sPAPERSIZE=a4 -dDEVICEWIDTHPOINTS=623 -dDEVICEHEIGHTPOINTS=842 -dFIXEDMEDIA -dPDFFitPage   %s/%s  && rm %s/%s ",szPathPdfStore,szFileName,szPathPdfReceive,szFileName,szPathPdfReceive,szFileName);
							system(szCommand);
							// Faccio l'update di ord_prod
							PGResUpdate=PGExecSQL(Cfg.nDebugVersion>1,"update ord_prod set opflpdf='F' where ordprog='%s' and opflpdf='%c';",szBolla,ORDINE_IN_ATTESA_PDF);
							if(PQresultStatus(PGResUpdate) == PGRES_COMMAND_OK && PQcmdTuples(PGResUpdate)!=0){ 
							} else {
#ifdef TRACE
								trace_out_vstr_date(1,"Ordine [%s]: errore in updating OPFLPDF \n",szBolla);
#endif
							}
							// Update a video
							if(Cfg.nSAP){
								UpdateOrdine(LeftStr(szBolla,10));
							} else {
								UpdateOrdine(LeftStr(szBolla,6));
							}
							PQclear(PGResUpdate);
							// Non piu' usato ma lo lascio, nel caso servisse in futuro
							nFilesIndex++;
						} else {
#ifdef TRACE
							trace_out_vstr_date(1,"Ordine [%s]: errore in updating ROFLPDF \n",szBolla);
#endif
						}
						PQclear(PGRes);
					}
					PQclear(PGResSelect);
				}
				(void) closedir (dir);
#ifdef TRACE
				if(Cfg.nDebugVersion>3){ trace_out_vstr_date(1,"... Fine ricerca nuovi file PDF \n");}
#endif
			} else {
			}
		
/*******************************************************
* FINE VERSIONE CON GESTIONE PDF
********************************************************/

		}
	}

	/*
	* Messaggi da altri processi
	*/
	while((nCarIn = ReceiveMsgNoWait(ProcList[PROC_MAIN].nQNumber, &nAllMsg, szInMsg))>=0){
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
						SendMessage(PROC_SOCK,PROC_MAIN,SOCK_OPEN_SERVICE,szBuffer);
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
				/*
				* callback per pressione pulsante
				*/
				gtk_signal_connect (GTK_OBJECT (pb_ok), "clicked", GTK_SIGNAL_FUNC (on_dlg_message_sock_display_msg_box), dlg_message);
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
				char szColor[128];

				trace_out_vstr_date(1, "DISPLAY_MSG:%s - %s", ProcList[InMsgStruct.srce-1].szProcName,szText);

				switch (InMsgStruct.srce-1){
					case PROC_VOLUM: strcpy(szColor,"RED");break;
					case PROC_SETTORI: strcpy(szColor,"GREEN");break;
					case PROC_PRINTLABEL: strcpy(szColor,"BLUE");break;
					case PROC_SOCK: strcpy(szColor,"ORANGE");break;
					default: strcpy(szColor,"BLACK");break;
				}
				gtk_text_printf(szColor,txt_msgs,szText);
			}
			break;
		}

		switch (InMsgStruct.srce-1){
			case PROC_VOLUM:
				switch (InMsgStruct.code){
					case VOLUM_START_ANALISI:
					break;
					case VOLUM_END_ANALISI:
					break;
					case VOLUM_START_ANALISI_TEST:
					break;
					case VOLUM_END_ANALISI_TEST:
					break;
					case VOLUM_END_ANALISI_ORDINE:
					{
						if(Cfg.nDebugVersion>1){
							gtk_text_printf("RED",txt_msgs,"VOLUM_END_ANALISI_ORDINE [%s]\n",szText);
						}
						UpdateOrdine(szText);
						/*
						* rm+st 13-02-2001 : modifiche per secondi invii
						* verifica lo stato ordine e stampa se richiesto etichette e packing list
						*/
						CheckStampaOrdineEvaso(szText);
					}
					break;
					case VOLUM_ERRORE_ANALISI_ORDINE:
						gtk_text_printf("RED",txt_msgs,"VOLUM_ERRORE_ANALISI_ORDINE [%s]\n",szText);
					break;
				}
			break;
			case PROC_SETTORI:
				switch (InMsgStruct.code){
					case SETTORI_ORDINE_IN_PRELIEVO:
					{
						int nIsola,nSettore;
						char szElaborazione[128],szOrdKey[128];
						char *pPtr;
						char szBuffer[256];
						char szLabelPrinter[128];
						char szPrinter[128];
						char szPdfPrinter[128];
						char *szSeparator=",\t\n";
						char cFlagPdf;
						int nAmazon=0;
						int nColli=0;


						/* Isola        */ if((pPtr=strtok(szText,szSeparator))!=NULL){ nIsola=atoi(pPtr); }
						/* Settore      */ if((pPtr=strtok(  NULL,szSeparator))!=NULL){ nSettore=atoi(pPtr); }
						/* Elaborazione */ if((pPtr=strtok(  NULL,szSeparator))!=NULL){ strcpy(szElaborazione,pPtr); }
						/* OrdKey       */ if((pPtr=strtok(  NULL,szSeparator))!=NULL){ strcpy(szOrdKey,pPtr); }

						/*
						 * Ricavo il flag pdf per sapere cosa e dove stampare
						 */
						cFlagPdf=GetPdfFlagFromOrdProd(szOrdKey);
						/* amazon */
						nAmazon=GetAmazonFlagFromRicOrd(szOrdKey);
						nColli=GetNumColliOrdProd(szOrdKey);

						if(Cfg.nDebugVersion>1){
							gtk_text_printf("GREEN",txt_msgs,"SETTORI_ORDINE_IN_PRELIEVO [%d,%d,%s,%s]\n",
								nIsola, nSettore, szElaborazione, szOrdKey);
						}

						if(Cfg.nSAP){
							UpdateOrdine(LeftStr(szOrdKey,10));
						} else {
							UpdateOrdine(LeftStr(szOrdKey,6));
						}

						switch(nIsola){
							default:
							case 1:
							{
								sprintf(szLabelPrinter,"%s",Cfg.szLabelPrinterIsola1);
								sprintf(szPrinter,"%s",Cfg.szPrinterIsola1);
								switch(cFlagPdf){
									case ORDINE_CON_FATTURA:
										sprintf(szPdfPrinter,"%s",Cfg.szFatturePrinterIsola1);
									break;
									case ORDINE_CON_DDT:
										sprintf(szPdfPrinter,"%s",Cfg.szBollePrinterIsola1);
									break;
									default:
										sprintf(szPdfPrinter,"%s",Cfg.szFatturePrinterIsola1);
									break;
								}
							}
							break;
							case 2:
							{
								sprintf(szLabelPrinter,"%s",Cfg.szLabelPrinterIsola2);
								sprintf(szPrinter,"%s",Cfg.szPrinterIsola2);
								switch(cFlagPdf){
									case ORDINE_CON_FATTURA:
										sprintf(szPdfPrinter,"%s",Cfg.szFatturePrinterIsola2);
									break;
									case ORDINE_CON_DDT:
										sprintf(szPdfPrinter,"%s",Cfg.szBollePrinterIsola2);
									break;
									default:
										sprintf(szPdfPrinter,"%s",Cfg.szFatturePrinterIsola2);
									break;
								}
							}
							break;
						}

						if(strlen(szOldOrdKey[nIsola]) && strlen(szOldElaborazione[nIsola]) && strcmp(szElaborazione,szOldElaborazione[nIsola])){

							sprintf(szBuffer,"%s,%s",szOldOrdKey[nIsola],szLabelPrinter);
							SendMessage(PROC_PRINTLABEL,PROC_MAIN,PRINT_FINE_ELABORAZIONE,szBuffer);

							sprintf(szBuffer,"%s,%s",szOrdKey,szLabelPrinter);
							SendMessage(PROC_PRINTLABEL,PROC_MAIN,PRINT_INIZIO_ELABORAZIONE,szBuffer);
						}
						strcpy(szOldOrdKey[nIsola],szOrdKey);
						strcpy(szOldElaborazione[nIsola],szElaborazione);

						/*
						* Spedizione messaggi di stampa etichette e packing list a printlabel
						*/
						if(Cfg.nStampaPL==STAMPA_AGHI){
							sprintf(szBuffer,"%s,%s",szOrdKey,szPrinter);
							SendMessage(PROC_PRINTLABEL,PROC_MAIN,PRINT_PACKING_LIST,szBuffer);
						} else {
							sprintf(szBuffer,"%s,%s,0",szOrdKey,szPdfPrinter);
							SendMessage(PROC_PRINTLABEL,PROC_MAIN,PRINT_PACKING_LIST_REPORT,szBuffer);
						}
						
						sprintf(szBuffer,"%s,%s",szOrdKey,szLabelPrinter);
						SendMessage(PROC_PRINTLABEL,PROC_MAIN,PRINT_RAC,szBuffer);

						/* rm 12-06-2021 : eliminata (Cristian) */
						/*
						if (nAmazon){
							int nIndex;

							for (nIndex=0;nIndex < nColli; nIndex++){
								sprintf(szBuffer,"%s,%d,%s",szOrdKey,nIndex+1,szPdfPrinter);
								SendMessage(PROC_PRINTLABEL,PROC_MAIN,PRINT_PACKING_LIST_COLLO,szBuffer);
							}
						}
						*/

						/*
						 * Non e' detto che il pdf debba sempre e comunque essere stampato
						 * esistono ordini senza pdf e ordini che potrei forzare
						 */
						switch(cFlagPdf){
							case ORDINE_CON_FATTURA:
							case ORDINE_CON_DDT:
								//sprintf(szBuffer,"%s",szLabelPrinter);
								sprintf(szBuffer,"%s,%s",LeftStr(szOrdKey,10),szLabelPrinter);
								SendMessage(PROC_PRINTLABEL,PROC_MAIN,PRINT_RAC_CONTIENE_DOC,szBuffer);
								sprintf(szBuffer,"%s,%s",LeftStr(szOrdKey,10),szPdfPrinter);
								SendMessage(PROC_PRINTLABEL,PROC_MAIN,PRINT_PDF,szBuffer);
							break;
							default:
							break;
						}
					}
					break;

/*******************************************************
* VERSIONE CON GESTIONE PDF
********************************************************/
					
					case SETTORI_ORDINE_INCOMPLETO_IN_PRELIEVO:
					{
						// static char szOldElaborazione[5][128]={"","","","",""};	/* memoria di ultima elaborazione */
						// static char szOldOrdKey[5][128]={"","","","",""};	/* memoria di ultima chiave ordine */
						int nIsola,nSettore;
						char szElaborazione[128],szOrdKey[128];
						char *pPtr;
						char szBuffer[128];
						char szLabelPrinter[128];
						char szPrinter[128];
						char *szSeparator=",\t\n";

						/* Isola        */ if((pPtr=strtok(szText,szSeparator))!=NULL){ nIsola=atoi(pPtr); }
						/* Settore      */ if((pPtr=strtok(  NULL,szSeparator))!=NULL){ nSettore=atoi(pPtr); }
						/* Elaborazione */ if((pPtr=strtok(  NULL,szSeparator))!=NULL){ strcpy(szElaborazione,pPtr); }
						/* OrdKey       */ if((pPtr=strtok(  NULL,szSeparator))!=NULL){ strcpy(szOrdKey,pPtr); }

						if(Cfg.nDebugVersion>1){
							gtk_text_printf("GREEN",txt_msgs,"SETTORI_ORDINE_IN_PRELIEVO [%d,%d,%s,%s]\n",
								nIsola, nSettore, szElaborazione, szOrdKey);
						}

						if(Cfg.nSAP){
							UpdateOrdine(LeftStr(szOrdKey,10));
						} else {
							UpdateOrdine(LeftStr(szOrdKey,6));
						}

						switch(nIsola){
							default:
							case 1:
								sprintf(szLabelPrinter,"%s",Cfg.szLabelPrinterIsola1);
								if(Cfg.nStampaPL==STAMPA_AGHI){
									sprintf(szPrinter,"%s",Cfg.szPrinterIsola1);
								} else {
									sprintf(szPrinter,"%s",Cfg.szBollePrinterIsola1);
								}
							break;
							case 2:
								sprintf(szLabelPrinter,"%s",Cfg.szLabelPrinterIsola2);
								if(Cfg.nStampaPL==STAMPA_AGHI){
									sprintf(szPrinter,"%s",Cfg.szPrinterIsola2);
								} else {
									sprintf(szPrinter,"%s",Cfg.szBollePrinterIsola2);
								}
							break;
						}
						
						if(strlen(szOldOrdKey[nIsola]) && strlen(szOldElaborazione[nIsola]) && strcmp(szElaborazione,szOldElaborazione[nIsola])){

							sprintf(szBuffer,"%s,%s",szOldOrdKey[nIsola],szLabelPrinter);
							SendMessage(PROC_PRINTLABEL,PROC_MAIN,PRINT_FINE_ELABORAZIONE,szBuffer);

							sprintf(szBuffer,"%s,%s",szOrdKey,szLabelPrinter);
							SendMessage(PROC_PRINTLABEL,PROC_MAIN,PRINT_INIZIO_ELABORAZIONE,szBuffer);
						}
						strcpy(szOldOrdKey[nIsola],szOrdKey);
						strcpy(szOldElaborazione[nIsola],szElaborazione);

						/*
						* Spedizione messaggi di stampa etichette e packing list a printlabel
						*/
						if(Cfg.nStampaPL==STAMPA_AGHI){
							sprintf(szBuffer,"%s,%s",szOrdKey,szPrinter);
							SendMessage(PROC_PRINTLABEL,PROC_MAIN,PRINT_PACKING_LIST_NO_PDF,szBuffer);
						} else {
							sprintf(szBuffer,"%s,%s,1",szOrdKey,szPrinter);
							SendMessage(PROC_PRINTLABEL,PROC_MAIN,PRINT_PACKING_LIST_REPORT,szBuffer);
						}

						sprintf(szBuffer,"%s,%s",szOrdKey,szLabelPrinter);
						SendMessage(PROC_PRINTLABEL,PROC_MAIN,PRINT_RAC_NO_PDF,szBuffer);

					}
					break;
					case SETTORI_ORDINE_INCOMPLETO_TERMINATO_IN_PRELIEVO:
					{
						// static char szOldElaborazione[5][128]={"","","","",""};	/* memoria di ultima elaborazione */
						// static char szOldOrdKey[5][128]={"","","","",""};	/* memoria di ultima chiave ordine */
						int nIsola,nSettore;
						char szElaborazione[128],szOrdKey[128];
						char *pPtr;
						char szBuffer[128];
						char szLabelPrinter[128];
						char szPrinter[128];
						char szPdfPrinter[128];
						char *szSeparator=",\t\n";
						char cFlagPdf;

						/* Isola        */ if((pPtr=strtok(szText,szSeparator))!=NULL){ nIsola=atoi(pPtr); }
						/* Settore      */ if((pPtr=strtok(  NULL,szSeparator))!=NULL){ nSettore=atoi(pPtr); }
						/* Elaborazione */ if((pPtr=strtok(  NULL,szSeparator))!=NULL){ strcpy(szElaborazione,pPtr); }
						/* OrdKey       */ if((pPtr=strtok(  NULL,szSeparator))!=NULL){ strcpy(szOrdKey,pPtr); }

						/*
						 * Ricavo il flag pdf per sapere cosa e dove stampare
						 */
						cFlagPdf=GetPdfFlagFromOrdProd(szOrdKey);

						if(Cfg.nDebugVersion>1){
							gtk_text_printf("GREEN",txt_msgs,"SETTORI_ORDINE_INCOMPLETO_TERMINATO_IN_PRELIEVO [%d,%d,%s,%s]\n",
								nIsola, nSettore, szElaborazione, szOrdKey);
						}

						if(Cfg.nSAP){
							UpdateOrdine(LeftStr(szOrdKey,10));
						} else {
							UpdateOrdine(LeftStr(szOrdKey,6));
						}
//						switch(nIsola){
//							default:
//							case 1:
//								sprintf(szLabelPrinter,"%s",Cfg.szLabelPrinterIsola1);
//								sprintf(szPrinter,"%s",Cfg.szPrinterIsola1);
//								sprintf(szPdfPrinter,"%s",Cfg.szPdfPrinterIsola1);
//							break;
//							case 2:
//								sprintf(szLabelPrinter,"%s",Cfg.szLabelPrinterIsola2);
//								sprintf(szPrinter,"%s",Cfg.szPrinterIsola2);
//								sprintf(szPdfPrinter,"%s",Cfg.szPdfPrinterIsola2);
//							break;
//						}
						
						switch(nIsola){
							default:
							case 1:
							{
								sprintf(szLabelPrinter,"%s",Cfg.szLabelPrinterIsola1);
								sprintf(szPrinter,"%s",Cfg.szPrinterIsola1);
								switch(cFlagPdf){
									case ORDINE_CON_FATTURA:
										sprintf(szPdfPrinter,"%s",Cfg.szFatturePrinterIsola1);
									break;
									case ORDINE_CON_DDT:
										sprintf(szPdfPrinter,"%s",Cfg.szBollePrinterIsola1);
									break;
									default:
										sprintf(szPdfPrinter,"%s",Cfg.szFatturePrinterIsola1);
									break;
								}
							}
							break;
							case 2:
							{
								sprintf(szLabelPrinter,"%s",Cfg.szLabelPrinterIsola2);
								sprintf(szPrinter,"%s",Cfg.szPrinterIsola2);
								switch(cFlagPdf){
									case ORDINE_CON_FATTURA:
										sprintf(szPdfPrinter,"%s",Cfg.szFatturePrinterIsola2);
									break;
									case ORDINE_CON_DDT:
										sprintf(szPdfPrinter,"%s",Cfg.szBollePrinterIsola2);
									break;
									default:
										sprintf(szPdfPrinter,"%s",Cfg.szFatturePrinterIsola2);
									break;
								}
							}
							break;
						}

						if(strlen(szOldOrdKey[nIsola]) && strlen(szOldElaborazione[nIsola]) && strcmp(szElaborazione,szOldElaborazione[nIsola])){

							sprintf(szBuffer,"%s,%s",szOldOrdKey[nIsola],szLabelPrinter);
							SendMessage(PROC_PRINTLABEL,PROC_MAIN,PRINT_FINE_ELABORAZIONE,szBuffer);

							sprintf(szBuffer,"%s,%s",szOrdKey,szLabelPrinter);
							SendMessage(PROC_PRINTLABEL,PROC_MAIN,PRINT_INIZIO_ELABORAZIONE,szBuffer);
						}
						/*
						 * Richiesta Rodighiero: Aggiungo un'etichetta che separa i colli di ordini incompleti
						 * da quelli con il pdf. (Non fatta)
						 */
						
						strcpy(szOldOrdKey[nIsola],szOrdKey);
						strcpy(szOldElaborazione[nIsola],szElaborazione);

						/*
						* Spedizione messaggi di stampa etichette e packing list a printlabel
						*/
						if(Cfg.nStampaPL==STAMPA_AGHI){
							sprintf(szBuffer,"%s,%s",szOrdKey,szPrinter);
							SendMessage(PROC_PRINTLABEL,PROC_MAIN,PRINT_PACKING_LIST_PDF,szBuffer);
						} else {
							sprintf(szBuffer,"%s,%s,2",szOrdKey,szPdfPrinter);
							SendMessage(PROC_PRINTLABEL,PROC_MAIN,PRINT_PACKING_LIST_REPORT,szBuffer);
						}
						
						sprintf(szBuffer,"%s,%s",szOrdKey,szLabelPrinter);
						SendMessage(PROC_PRINTLABEL,PROC_MAIN,PRINT_RAC_PDF,szBuffer);

						/*
						 * Non e' detto che il pdf debba sempre e comunque essere stampato
						 * esistono ordini senza pdf e ordini che potrei forzare
						 */
						switch(cFlagPdf){
							case ORDINE_CON_FATTURA:
							case ORDINE_CON_DDT:
								//sprintf(szBuffer,"%s",szLabelPrinter);
								sprintf(szBuffer,"%s,%s",LeftStr(szOrdKey,10),szLabelPrinter);
								SendMessage(PROC_PRINTLABEL,PROC_MAIN,PRINT_RAC_CONTIENE_DOC,szBuffer);
								sprintf(szBuffer,"%s,%s",LeftStr(szOrdKey,10),szPdfPrinter);
								SendMessage(PROC_PRINTLABEL,PROC_MAIN,PRINT_PDF,szBuffer);
							break;
							default:
							break;
						}
					}
					break;

/*******************************************************
* FINE VERSIONE CON GESTIONE PDF
********************************************************/

					case SETTORI_ORDINE_COLLO_PRELEVATO:
					{
						int nIsola,nSettore,nCollo;
						char szElaborazione[128],szOrdKey[128];
						char  *pPtr;
						char  *szSeparator=",\t\n";

						/* Isola        */ if((pPtr=strtok(szText,szSeparator))!=NULL){ nIsola=atoi(pPtr); }
						/* Settore      */ if((pPtr=strtok(NULL  ,szSeparator))!=NULL){ nSettore=atoi(pPtr); }
						/* Elaborazione */ if((pPtr=strtok(NULL  ,szSeparator))!=NULL){ strcpy(szElaborazione,pPtr); }
						/* OrdKey       */ if((pPtr=strtok(NULL  ,szSeparator))!=NULL){ strcpy(szOrdKey,pPtr); }
						/* Collo        */ if((pPtr=strtok(NULL  ,szSeparator))!=NULL){ nCollo=atoi(pPtr); }

						if(Cfg.nDebugVersion>1){
							gtk_text_printf("GREEN",txt_msgs,"SETTORI_ORDINE_COLLO_PRELEVATO [%d,%d,%s,%s,%d]\n",
								nIsola, nSettore, szElaborazione, szOrdKey,nCollo);
						}
					}
					break;
					case SETTORI_ORDINE_PRELEVATO:
					{
						int nIsola,nSettore;
						char szElaborazione[128],szOrdKey[128];
						char  *pPtr;
						char  *szSeparator=",\t\n";

						/* Isola        */ if((pPtr=strtok(szText,szSeparator))!=NULL){ nIsola=atoi(pPtr); }
						/* Settore      */ if((pPtr=strtok(NULL  ,szSeparator))!=NULL){ nSettore=atoi(pPtr); }
						/* Elaborazione */ if((pPtr=strtok(NULL  ,szSeparator))!=NULL){ strcpy(szElaborazione,pPtr); }
						/* OrdKey       */ if((pPtr=strtok(NULL  ,szSeparator))!=NULL){ strcpy(szOrdKey,pPtr); }

						if(Cfg.nDebugVersion>1){
							gtk_text_printf("GREEN",txt_msgs,"SETTORI_ORDINE_PRELEVATO [%d,%d,%s,%s]\n",
								nIsola, nSettore, szElaborazione, szOrdKey);
						}

						if(Cfg.nSAP){
							UpdateOrdine(LeftStr(szOrdKey,10));
						} else {
							UpdateOrdine(LeftStr(szOrdKey,6));
						}
					}
					break;
					case SETTORI_ORDINE_INCOMPLETO_PRELEVATO:
					{
						int nIsola,nSettore;
						char szElaborazione[128],szOrdKey[128];
						char  *pPtr;
						char  *szSeparator=",\t\n";

						/* Isola        */ if((pPtr=strtok(szText,szSeparator))!=NULL){ nIsola=atoi(pPtr); }
						/* Settore      */ if((pPtr=strtok(NULL  ,szSeparator))!=NULL){ nSettore=atoi(pPtr); }
						/* Elaborazione */ if((pPtr=strtok(NULL  ,szSeparator))!=NULL){ strcpy(szElaborazione,pPtr); }
						/* OrdKey       */ if((pPtr=strtok(NULL  ,szSeparator))!=NULL){ strcpy(szOrdKey,pPtr); }

						if(Cfg.nDebugVersion>1){
							gtk_text_printf("GREEN",txt_msgs,"SETTORI_ORDINE_INCOMPLETO_PRELEVATO [%d,%d,%s,%s]\n",
								nIsola, nSettore, szElaborazione, szOrdKey);
						}

						if(Cfg.nSAP){
							UpdateOrdine(LeftStr(szOrdKey,10));
						} else {
							UpdateOrdine(LeftStr(szOrdKey,6));
						}
					}
					break;
					case SETTORI_ORDINE_SETT_SUCC:
					{
						int nIsola,nSettore;
						char szElaborazione[128],szOrdKey[128];
						char  *pPtr;
						char  *szSeparator=",\t\n";

						/* Isola        */ if((pPtr=strtok(szText,szSeparator))!=NULL){ nIsola=atoi(pPtr); }
						/* Settore      */ if((pPtr=strtok(NULL  ,szSeparator))!=NULL){ nSettore=atoi(pPtr); }
						/* Elaborazione */ if((pPtr=strtok(NULL  ,szSeparator))!=NULL){ strcpy(szElaborazione,pPtr); }
						/* OrdKey       */ if((pPtr=strtok(NULL  ,szSeparator))!=NULL){ strcpy(szOrdKey,pPtr); }

						if(Cfg.nDebugVersion>1){
							gtk_text_printf("GREEN",txt_msgs,"SETTORI_ORDINE_SETT_SUCC [%d,%d,%s,%s]\n",
								nIsola, nSettore, szElaborazione, szOrdKey);
						}
					}
					break;
				}
			break;
			case PROC_PRINTLABEL:
				switch (InMsgStruct.code){
					case PRINTLABEL_RAC_STAMPATO:
					{
						char szOrdProg[128];
						char *pPtr;
						char *szSeparator=",\t\n";

						/* Ordine */ if((pPtr=strtok(szText,szSeparator))!=NULL){ strcpy(szOrdProg,pPtr); }

						if(Cfg.nSAP){
							//UpdateOrdine(LeftStr(szOrdProg,10));
							//CambiaStatoOrdine(LeftStr(szOrdProg,10), ORDINE_STAMPATA_RAC);
						} else {
							//UpdateOrdine(LeftStr(szOrdProg,6));
							//CambiaStatoOrdine(LeftStr(szOrdProg,6), ORDINE_STAMPATA_RAC);
						}
					}
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
