/*
* mainstruct.h
* Dichiarazione Strutture Utilizzate 
* Easy Picking 3.0
* Copyright CNI srl 1999-2000
* Autore : Roberto Mantovani
*/

typedef struct tagCfgStruct{
	int nOrdiniKeyField;        /* Indice campo chiave */
	int nOrdiniStatusField;     /* Indice campo Status */
	int nOrdiniPdfStatusField;  /* Indice campo Pdf Status */
	int nOrdiniFtzStatusField;  /* Indice campo Fittizio Status */
	int nOrdiniElabField;       /* Indice campo elaborazione */
	int nOrdinamento;           /* Ordinamento lista ordini principale */
	int nAnno;                  /* Anno di lavoro (vedi sotto) */
	int nProgressivoLancio;     /* Progressivo lancio - Azzerato annualmente */
	int nChiusuraCollo;         /* Gestione chiusura collo */
	int nImballiUbicati;        /* Gestione Imballi */
	int nAttesaPDFOrdineCompleto; /* Trattieni tutto l'ordine o solo l'ultimo collo */
	int nMaxColliOrdine;        /* Gestione SottoOrdine per numero colli maggiore di  */
	int nVisualizzaSpeditiHost; /* Visualizza/Non Visualizza gli ordini in stato H */
	STAMPA_PL  nStampaPL;		  	/* Stampa RAC (STAMPA_AGHI - STAMPA_LASER) */
	STAMPA_RAC nStampaRAC;			/* Stampa RAC (DOPO_IL_LANCIO - INIZIO_PRELIEVO) */
	char szCniCfg[80];
	char szPathExe[80];
	char szPathData[80];
	char szPathStampe[80];
	char szPathExport[80];
	char szPathExportPdf[80];
	char szPathStorePdf[80];
	char szImpOrdiniBertello[80];		/* File di import dati */
	char szImpColliBertello[80];		/* File di import dati */
	char szImpRigheBertello[80];		/* File di import dati */

	char szImpOrdiniBertelloN[80];		/* File di import dati bertello new */
	char szImpColliBertelloN[80];		/* File di import dati bertello new */
	char szImpRigheBertelloN[80];		/* File di import dati bertello new */

	char szImpOrdiniFile[80];		/* File di import dati */
	char szImpRigheFile[80];		
	char szImpArticoliFile[80];
	char szImpUbicazioniFile[80];
	char szExpOrdiniFile[80];	 	/* File di export dati */
	char szExpColliFile[80];		
	char szExpDettColliFile[80];		
	char szPrinterConsole[80];
	char szPrinterIsola1[80];
	char szPrinterIsola2[80];
	char szFatturePrinterIsola1[80];
	char szFatturePrinterIsola2[80];
	char szBollePrinterIsola1[80];
	char szBollePrinterIsola2[80];
	char szPrinterBilancia[80];
	char szLabelPrinterConsole[80];
	char szLabelPrinterIsola1[80];
	char szLabelPrinterIsola2[80];
	char szLabelPrinterBilancia[80];
	char szLanguage[80];
	char szSuperPwd[20];
	char szSuperPwdCfm[20];
	char szChiefPwd[20];
	char szChiefPwdCfm[20];
	char szPwd[20];
	int nActualPwd;
	unsigned long ulSuperPwd;
	unsigned long ulChiefPwd;
	int nMainDelay;
	int nSaveStateDelay;
	int nDebugVersion;
	int nShmKey; 
	char szPGHost[128]; 
	char szPGPort[128]; 
	char szPGDataBase[128]; 
	int nServicePort;
	BOOL bStampaEtichette;
	BOOL bStampaPackingList;
	char szStampanteEtichette[128];
	char szStampantePackingList[128];
	int nSAP;
	int nSogliaControlloBarcode;
	int nGestioneBadge;
} CFGSTRUCT, *PCFGSTRUCT;


typedef struct tagPrinters{
	char szName[128];
	char szDescription[128];
} PRINTERSSTRUCT, *PPRINTERSSTRUCT;
