/*
* ProcList.h
* Strutture per la gestione dei processi
* costanti di identificazione dei messaggi
*/
#ifndef _PROC_LIST_H
#define _PROC_LIST_H

/* Indice dati processo in ProcList[] */
enum {
	PROC_MAIN,      /* Gestione Interfaccia Utente Processo MAIN */
	PROC_SETTORI,   /* Gestione Settori di prelievo */
	PROC_IOS,       /* Gestione IOS CNI */
	PROC_VOLUM,     /* Gestione Analisi Volumetrica */
	PROC_PRINTLABEL,/* Gestione Printlabel */
	PROC_PESO,      /* Interfaccia utente pesatura su fine linea */
	PROC_BARCODE,   /* Gestione Barcode Reader */
	PROC_BILANCIA,  /* Gestione Bilancia */
	PROC_BILSIM,    /* Simulazione Bilancia */
	PROC_SOCK,      /* Gestione Sockets */
	PROC_ISOLA,     /* Interfaccia utente isola 1 e 2 */


	NUM_PROC,
};


/* 
* Messaggi
* I messaggi scambiabili fra i processi sono divisi in fasce da 20
* attenzione a non superare il totale di 255 messaggi perche' il limite 
* massimo di messaggi gestibili e' appunto 255 (vedi PutField in msq_lib.c)
*/
enum {
	PROGRAM_STARTED=PROC_MAIN*20,
	PROGRAM_STOPPED,
	DISPLAY_MSG,

	REFRESH_DATA,
	REFRESH_CONFIGURATION,

	START_ANALISI,
	START_ANALISI_TEST,

	PRINT_RAC,
	PRINT_RAC_NO_PDF,
	PRINT_RAC_PDF,
	PRINT_RAC_CONTIENE_DOC,
	PRINT_PDF,
	//REPRINT_RAC,
	PRINT_RAC_COLLO,
	PRINT_RAC_COLLO_ADD,
	PRINT_PACKING_LIST_STORICO,
	PRINT_ALL_RAC,
	PRINT_ALL_PACKING_LIST,
	PRINT_PACKING_LIST,
	PRINT_PACKING_LIST_NO_PDF,
	PRINT_PACKING_LIST_PDF,
	PRINT_PACKING_LIST_COLLO,
	//PRINT_ORDINI_DATA,
	PRINT_INIZIO_ELABORAZIONE,
	PRINT_FINE_ELABORAZIONE,

	START_LINEA,
	STOP_LINEA,

	RESET_SETTORE,
	RESET_ALL,
	SAVE_STATE,
	RESTORE_STATE,

	MAIN_REQ_WEIGHT,

	IOS_PKL_SET_RC=PROC_IOS*20,		/* messaggi IOS */
	IOS_PKL_RESET_RC,
	IOS_PKL_CONGELA,
	IOS_PKL_SCONGELA,
	IOS_PKL_RESET_ALL,
	IOS_PKL_TEST,
	IOS_PKL_BUTTON_PRESSED,
	IOS_PKL_LIGHT_DAMAGED,
	IOS_DPY_CHAR,
	IOS_DPY_STRING,
	IOS_DPY_BCD,
	IOS_TEST_CMD,

	SETTORI_ORDINE_IN_PRELIEVO=PROC_SETTORI*20,
	SETTORI_ORDINE_INCOMPLETO_IN_PRELIEVO,
	SETTORI_ORDINE_INCOMPLETO_TERMINATO_IN_PRELIEVO,
	SETTORI_ORDINE_SETT_SUCC,
	SETTORI_ORDINE_COLLO_PRELEVATO,
	SETTORI_ORDINE_PRELEVATO,
	SETTORI_ORDINE_INCOMPLETO_PRELEVATO,
	SETTORI_ORDINE_EVASO,

	VOLUM_START_ANALISI_ORDINE=PROC_VOLUM*20,
	VOLUM_END_ANALISI_ORDINE,
	VOLUM_START_ANALISI,
	VOLUM_END_ANALISI,
	VOLUM_START_ANALISI_TEST,
	VOLUM_END_ANALISI_TEST,
	VOLUM_ERRORE_ANALISI_ORDINE,

	PRINTLABEL_RAC_STAMPATO=PROC_PRINTLABEL*20,
	PRINTLABEL_RAC_PDF_STAMPATO,
	PRINTLABEL_RAC_NO_PDF_STAMPATO,
	PRINTLABEL_RAC_COLLO_STAMPATO,
	PRINTLABEL_RAC_ERROR,
	PRINTLABEL_RAC_COLLO_ERROR,
	PRINTLABEL_PACKING_LIST_STAMPATO,
	PRINTLABEL_ALL_RAC_STAMPATO,
	PRINTLABEL_ALL_PACKING_LIST_STAMPATO,
	PRINTLABEL_PACKING_LIST_ERROR,
	PRINTLABEL_ORDINI_DATA_STAMPATO,
	PRINTLABEL_ORDINI_DATA_ERROR,
	PRINTLABEL_ALL_RAC_ERROR,


	BILANCIA_RX_DATA=PROC_BILANCIA*20,
	BILANCIA_RX_ERROR,
	BILANCIA_TX_ERROR,

	BCRD_RX_BARCODE=PROC_BARCODE*20,
	BCRD_RX_NOREAD,
	BCRD_RX_ERROR,

	SOCK_CMD_DISPLAY_MSG=PROC_SOCK*20,
	SOCK_CMD_DISPLAY_MSGBOX,
	SOCK_CMD_UPDATE_ORDINE,

	SOCK_OPEN_SERVICE=PROC_SOCK,
	SOCK_CLOSE_SERVICE,

};


/*
* definizioni per la gestione dei comandi da ricevere
*/
enum Commands {
	CMD_DISPLAY_MSG,
	CMD_DISPLAY_MSGBOX,
	CMD_UPDATE_ORDINE,

	NUM_SOCK_CMDS,
};

enum StatiLinea {
	LINEA_IN_START,
	LINEA_IN_STOP,

	NUM_STATI_LINEA,
};

/*
* Strutture comuni a tutti i moduli componenti il progetto
*/
typedef struct TagLineaStruct {
	unsigned int nStatoLinea;
	BOOL bAnalisi;
	BOOL bStampaRAC;
	BOOL bStampa;
	BOOL bRxData;
	BOOL bTxData;
	BOOL bRxActive;
	BOOL bTxActive;
	BOOL bPLCStatus;
} LINEA_STRUCT, *PLINEA_STRUCT;


#endif /* _PROC_LIST_H */