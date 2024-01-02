/*
* volstruct.h
* 
* Progetto Easy Picking 2.0 : Europharco
*
* Autore : Roberto Mantovani
*
* Copyright CNI srl 1995-98
*/

typedef struct tagCfgStruct{
	char szLanguage[128];
	char szPathData[128];
	char szPathExport[128];
	char szPathDataBase[80];
	char szPathStorico[80];
	char szPathExe[128];

	char szPGHost[128];
	char szPGPort[128];
	char szPGDataBase[128];

	int nDelay;
	int nMaxColliOrdine;
	int nAttesaPDFOrdineCompleto;
	int nDebugVersion;
	int nShmKey;
	BOOL bAlone;
	int nSAP;
	int nSogliaControlloBarcode;
	/* gestione non spacchettamento */
	int nNoUnpack;
	int nPrelievoSingoloPacco;
	int nIsolaAttiva;

} CFGSTRUCT, *PCFGSTRUCT;


typedef struct TagUbicazione {
	char szCodProd[40];
	char szCodUbi[40];
	int nSettore;
	int nIsola;
	int nIOS;
	int nCPU;
	int nDisplay;
	int nRowIndex;
	int nColIndex;
	int nPriorita;
	int nCedola;
	int nIndex;
} UBICAZIONE, *PUBICAZIONE;

typedef struct TagSemaforo {
	BOOL bFree;
} SEMAFORO, *PSEMAFORO;

typedef struct TagUbicazioni {
	int nUbicazioni;
	UBICAZIONE Ubicazione[MAX_UBICAZIONI];
} UBICAZIONI, *PUBICAZIONI;

typedef struct TagSpedizione {
	char szOrdProg[80]; /* Progressivo della Spedizione */
	char szNumElab[80]; /* Numero Elaborazione */
	char cStato;        /* Stato ordine originale */
	char cStatoOrdineElaborato;
	char cStatoColloElaborato;
	char cStatoRigaElaborata;
	int nSottoOrdini;
	int nColli;
	int nRighe;
	int nCopie;
	int nPesoTotale;
	int nProgressivoLancio;
	int nProgressivoPriorita;
	char cFlagPdf;
	char cFlagFittizio;
	char cFlagBarcodeIdentificativo;
} SPEDIZIONE, *PSPEDIZIONE;

typedef struct TagOrdine {
	char szOrdProg[80]; /* Progressivo della Spedizione */
	char szOrdKey[80];  /* Progressivo della Spedizione + Sottordine (isola/sottordine) */
	char szNumElab[80]; /* Numero Elaborazione */
	char cStato;        /* Stato ordine originale */
	char cStatoOrdineElaborato;
	char cStatoColloElaborato;
	char cStatoRigaElaborata;
	int nSottoOrdini;
	int nIsola;         /* Numero Isola */
	int nSottoOrdine;   /* Codice SottoOrdine */
	int nColliAutomatici;
	int nColli;
	int nRighe;
	int nPrese;
	int nCopie;
	int nPesoTotale;
	int nProgressivoLancio;
	int nProgressivoPriorita;
	char cFlagPdf;
	char cFlagFittizio;
} ORDINE, *PORDINE;

typedef struct TagColloOrdine {
	char szCodiceImballo[80];
	char szBarcode[80];
	char szPericoloso[80];
	int nIndiceImballo;
	int nProgCollo;
	int nSwLinea;
	int nVolume;
	int nCollo;
	int nCopie;
	int nRighe;
	int nPeso;
} COLLO_ORDINE, *PCOLLO_ORDINE;

typedef struct TagRigaOrdine {
	char szCodiceProdotto[80];
	char szUbicazione[80];
	char szPericoloso[80];
	int nLength;
	int nWidth;
	int nHeight;
	int nDim[3];
	int nProgRiga;
	int nCopie;
	int nPreseProdotto;
	int nCopieCaricate;
	int nPesoCopia;
	int nPesoTotale;
	int nVolumeCopia;
	int nVolumeTotale;
	int nSwFF;
	int nSwPCF;
	int nSwAltezza;
	int nProdottoInPiedi;
	char szFCPCF[80];
	char cFlagBarcodeIdentificativo;
	int nPRQTKAR;
} RIGA_ORDINE, *PRIGA_ORDINE;

typedef struct TagImballo {
	char szCodice[40];
	int nDim[3];
	int nHeight;
	int nWidth;
	int nLength;
	int nVolume;
	/* per isola */
	int nVolumeUtile[2];
	int nTara;
	int nPesoMax;
	/* per isola */
	int nVolPerc[2];
	int nSwLinea;
	int nIsola;
} IMBALLO, *PIMBALLO;


typedef struct TagImballi {
	int nImballi;
	int nImballiAutomatici;
	IMBALLO Imballo[MAX_IMBALLI];
} IMBALLI, *PIMBALLI;
