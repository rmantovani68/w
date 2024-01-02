/*
* ProcList.h
* Strutture per la gestione dei processi
* costanti di identificazione dei messaggi
*/
#ifndef _PROC_LIST_H
#define _PROC_LIST_H

#include <sys/types.h>

#ifndef BOOL_TYPE_DEFINED
typedef unsigned int BOOL;
#define BOOL_TYPE_DEFINED
#endif

#ifndef PROCESSO_DEFINED
typedef	struct	processo{
	char	szProcName[128]; /* nome file eseguibile */
	key_t	nQKey;           /* chiave della coda utilizzata */
	int		nQNumber;        /* numero della coda utilizzata */
	int		bCreate;         /* flag per creazione coda o sola apertura*/
	int		nGId;            /* numero processo*/
	int		nProcId;         /* pid del processo*/
	int		bKill;           /* 1 uccidere con kill, 0 inviare MSG_ENDMOD*/
	int		nPriority;       /* Priority Offset */
	int		bExecute;        /* 1 eseguire, 0 non eseguire */
}	PROCESSO;
#define PROCESSO_DEFINED 
#endif

/* Indice dati processo in ProcList[] */
enum {
	PROC_MAIN,       /* Gestione Interfaccia Utente Processo MAIN */
	PROC_SETTORI,    /* Gestione Settori di prelievo */
	PROC_IOS,        /* Gestione IOS CNI */
	PROC_VOLUM,      /* Gestione Analisi Volumetrica */
	PROC_PRINTLABEL, /* Gestione Printlabel */
	PROC_PESO,       /* Interfaccia utente pesatura su fine linea */
	PROC_BARCODE,    /* Gestione Barcode Reader */
	PROC_BARCODE_PZ, /* Gestione Barcode Reader Plz */
	PROC_BILANCIA,   /* Gestione Bilancia */
	PROC_AUTOTASK,   /* AUTOTASK  */
	PROC_SOCK,       /* Gestione Sockets */
	PROC_ISOLA,      /* Interfaccia utente isola 1 e 2 */
	PROC_DISPLAY,    /* Interfaccia utente display */


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
	PRINT_RAC_AMAZON_BERTELLO,
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
	PRINT_PACKING_LIST_REPORT,
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
	SET_BADGE,
	RESET_BADGE,

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

	SIMULA_LUCE,
	BARCODE_ID_PRODOTTO,
	BADGE_ID,
};


/*
* definizioni per la gestione dei comandi da ricevere
*/
enum Commands {
	CMD_DISPLAY_MSG,
	CMD_DISPLAY_MSGBOX,
	CMD_UPDATE_ORDINE,
	CMD_PRINT_PDF,
	CMD_PRINT_RAC_COLLO,

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
	unsigned int nStatoLinea_1;
	unsigned int nStatoLinea_2;
	BOOL bAnalisi;
} LINEA_STRUCT, *PLINEA_STRUCT;

/*
* Strutture di definizione Righe di prelievo
*/
typedef struct TagRigaPrelievo {
	char szCodSped[40];
	char szCodProd[40];
	char szCodUbi[40];
	int nProgressivo;
	int nRigProg;
	int nNumCollo;
	int nNextCollo;
	int nNumCopie;
	int nModulo;
	int nRiga;
	int nColonna;
	int nStato;
	int nIOS;
	int nCPU;
	int nDisplay;
	char szFlagBarcodeId[1];
	int nQtBarcodeId;         	 
	char szBarcodeIdLetto[40]; 
} RIGA_PRELIEVO, *PRIGA_PRELIEVO;


/*
* Strutture di definizione Imballi in prelievo
*/
typedef struct TagImballoPrelievo {
	char szOrdProg[40];
	char szOrdKey[40];
	char szFormato[40];
	char szCodUbi[40];
	char szImballoSpeciale[1];    		 /* Segnalazione su Display per selezione di imballo speciale: stesse misure ma design diverso */
	int nNumCollo;
	int nIOS;
	int nCPU;
	int nModulo;
	int nRiga;
	int nColonna;
	int nDisplay;
} IMBALLO_PRELIEVO, *PIMBALLO_PRELIEVO;

typedef struct tagSettore {
	RIGA_PRELIEVO RigaOrdine[128]; /* righe d'ordine relative a questo settore */
	IMBALLO_PRELIEVO Imballo[128]; /* Imballi da prelevare */
	int  nIndiceImballo;         /* puntatore all'imballo da prelevare */
	int  nNumeroImballi;         /* numero degli imballi da prelevare */
	int  nIndiceRigaOrdine;      /* puntatore alla riga d'ordine da prelevare */
	int  nNumeroRigheOrdine;     /* numero delle righe d'ordine da prelevare */
	char szRiga_1_Display[10];   /* Riga 1 del display */
	char szRiga_2_Display[10];   /* Riga 2 del display */
	int nStatoRiga1;             /* Stato della riga 1 del display (BLINKING / NORMALE) */
	int nStatoRiga2;             /* Stato della riga 2 del display (BLINKING / NORMALE) */
	int nSettore;                /* Numero progressivo del settore (offset 0) */
	int nIsola;                  /* Codice Isola di appartenenza */
	int nAttesaBadge;            /* gestione badge - RM 30-11-2019 */
	int nStatoSettore;           /* Stato attuale del settore */
	int nTipoSettore;            /* Tipo attuale del settore */
	int nFirstMod;               /* Modulo  primo pulsante settore (priorita') */
	int nFirstRow;               /* Riga    primo pulsante settore (priorita') */
	int nFirstColumn;            /* Colonna primo pulsante settore (priorita') */
	int nFirstIOS;               /* IOS primo pulsante settore (priorita') */
	int nFirstCPU;               /* CPU primo pulsante settore (priorita') */
	int nFirstDisplay;           /* Display primo pulsante settore (priorita') */
	int nIOS;                    /* IOS  Tasto Premuto / da premere */
	int nCPU;                    /* CPU  Tasto Premuto / da premere */
	int nModulo;                 /* Modulo  Tasto Premuto / da premere */
	int nRiga;                   /* Riga    Tasto Premuto / da premere */
	int nColonna;                /* Colonna Tasto Premuto / da premere */
	int nDisplay;                /* DIsplay Attivo */
	int nNumCopie;               /* Numero Copie */
	int nRigProg;                /* Progressivo Riga - RM 30-11-2019 */
	int nNumCollo;               /* Numero Collo */
	int nStatoLampada;           /* Stato Lampada */
	char szCodSped[40];          /* Codice Ordine Attivo */
	char cStatoSped;             /* Stato Ordine attivo */
	char szCodProd[40];          /* Codice Prodotto da prelevare/prelevato*/
	char szTitoloProd[128];      /* Titolo Prodotto da prelevare/prelevato*/
	char szCodUbi[40];           /* Codice Ubicazione del prodotto da prelevare/prelevato*/
	char szCodOperatore[40];     /* Codice Operatore addetto al prelievo */
	char szNomeOperatore[40];    /* Nome Operatore addetto al prelievo */
	char szCognomeOperatore[40]; /* Cognome Operatore addetto al prelievo */
	int nOpBlinking;             /* Blinking del campo operazione in display */
	int nFronte;             		 /* Fronte 1 : destra 2 : sinistra */
	int nInPiedi;           		 /* In Piedi 1 : normale : 0 */
	int nPericoloso;         		 /* Pericoloso */
	char szFlagBarcodeId[4];
	int nQtBarcodeId;         	 
	char szBarcodeIdLetto[40]; 
	char szImballoSpeciale[1];    		 /* Segnalazione su Display per selezione di imballo speciale: stesse misure ma design diverso */
} SETTORE, *PSETTORE;

typedef struct tagDisplayStruct{
	char szRiga_1_Display[10];   /* Riga 1 del display */
	char szRiga_2_Display[10];   /* Riga 2 del display */
	int nStatoRiga1;             /* Stato della riga 1 del display (BLINKING / NORMALE) */
	int nStatoRiga2;             /* Stato della riga 2 del display (BLINKING / NORMALE) */
	int nIOS;     /* numero della scheda ios associata al display */
	int nCPU;     /* numero della CPU ios associata al display */
	int nModulo;  /* indirizzo del modulo associato al display */
	int nIsola;   /* Isola */
	int nSettore; /* Settore */
	int nEnabled; /* Abilitato */
} DISPLAYSTRUCT, *PDISPLAYSTRUCT;

/*
* stati dei records archivi operativi 
* NB: Gli stati degli ordini sono consequenziali
*/
#define ORDINE_RICEVUTO                           ' '
#define ORDINE_SPEDITO                            'S'
#define ORDINE_SPEDITO_DA_EVADERE                 'K'
#define ORDINE_SPEDITO_DA_CONSUNTIVARE            'V'
#define ORDINE_ELABORATO                          'A'
#define ORDINE_STAMPATA_RAC                       'R'
#define ORDINE_IN_PRELIEVO                        'P'
#define ORDINE_PRELEVATO                          'T'
#define ORDINE_INCOMPLETO_IN_PRELIEVO             'J'
#define ORDINE_INCOMPLETO_TERMINATO               'Z'
#define ORDINE_INCOMPLETO_TERMINATO_SPEDITO       'L'
#define ORDINE_INCOMPLETO_TERMINATO_IN_PRELIEVO   'W'
#define ORDINE_EVASO                              'E'
#define ORDINE_STAMPATA_XAB                       'X'
#define ORDINE_STAMPATA_DIST                      'D'
#define ORDINE_SPEDITO_HOST                       'H'

/*
* ROFLPDF: Flag pdf degli ordini
* OPFLPDF: Flag pdf degli ordini
*/
#define ORDINE_CON_FATTURA            'F'
#define ORDINE_CON_DDT                'D'
#define ORDINE_IN_ATTESA_PDF          ' '
#define ORDINE_SENZA_PDF              'N'

/*
* ROFLFTZ: Flag degli ordini fittizi
* OPFLFTZ: Flag degli ordini fittizi
*/
#define ORDINE_NON_FITTIZIO           ' '
#define ORDINE_FITTIZIO               'F'

/*
* CPFLFTZ: Flag degli ordini fittizi
*/
#define COLLO_NON_FITTIZIO           ' '
#define COLLO_FITTIZIO               'F'


/*
* CPFLPDF: Flag pdf dei colli
*/
#define COLLO_CON_PDF                 'P'
#define COLLO_SENZA_PDF               ' '


#define COLLO_ELABORATO               'A'
#define COLLO_PRELIEVO                'P'
#define COLLO_CHIUSO                  'C'
#define COLLO_EVASO                   'E'
#define COLLO_SPEDITO_HOST            'H'
#define COLLO_ELIMINATO               'D'
#define COLLO_PESATO                  'W'
#define COLLO_SCARTATO                'X'

#define RIGA_ELABORATA                'A'
#define RIGA_PRELEVATA                'P'
#define RIGA_EVASA                    'E'
#define RIGA_PESATA                   'W'
#define RIGA_SCARTATA                 'X'

/* ordine predestinato allo scarto in zona bilancia */
#define ORDINE_DA_SCARTARE            'X'

/*
* Tipi di collo particolari
*/
#define COLLO_FUORI_FORMATO   'F'
#define COLLO_PALLET          'P'
#define COLLO_PRECONFEZIONATO 'C'

/*
* stati prswbri 
*/
#define CONTROLLO_BARCODE_ON                           'C'
#define CONTROLLO_BARCODE_OFF                          ' '

/*
* stati rpflbri, cpflbri
*/
#define CONTROLLO_BARCODE_OK                          'C'
#define CONTROLLO_BARCODE_INCOMPLETO                  'I'
#define CONTROLLO_BARCODE_KO                          'X'
#define CONTROLLO_BARCODE_FORZATO_PARZIALE            'F'
#define CONTROLLO_BARCODE_DISABILITATO                ' '

/*
* stati prswpsp, cpflisp 
*/
#define IMBALLO_SPECIALE_OFF                          ' '
#define IMBALLO_SPECIALE_ON                           'S'



#define BARCODE_CHIUDI_LETTURE "999999999999999999999999"
#define BARCODE_ERRORE "BARCODEERRORE"
#define BARCODE_QT_SUPERATA "BARCODEQTSUPERATA"
#define BARCODE_LUNGH_ERRATA "BARCODELUNGHERRATA"
#define BARCODE_LETTURE_COMPLETATE "BARCODELETTURECOMPLETATE"
#define BARCODE_ID_LENGTH 24

#endif /* _PROC_LIST_H */
