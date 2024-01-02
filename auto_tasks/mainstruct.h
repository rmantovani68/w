/*
* Modulo : mainstruct.h
* -----------------------
* Definizione delle strutture utilizzate nel processo Rifornimenti
*
* Progetto EMI Italiana SPA - Gestione Ordini
*
* Data creazione 14/03/1996
*
* Autore : Roberto Mantovani
*
* Copyright CNI srl 1996-2001
*/

typedef struct tagCfgStruct{
	/*
	* Dati generali
	*/
	char szCniCfg[128];
	char szLanguage[128];
	char szPathData[128];
	char szPathStampe[128];
	char szPathExport[128];
	char szPathExe[128];

	char szLabelPrinterName[80];    /* Stampante Etichette */
	char szPrinterName[80];	        /* Stampante Generica */
	int nDebugVersion;              /* versione di debug */

	/*
	* Database settings
	*/
	char szPGHost[128]; 
	char szPGPort[128]; 
	char szPGDataBase[128]; 

	/*
	* Dati specifici del processo (printlabel)
	*/
	int nShmKey;
	int nDelay;

	char szLabelBaseName[80];		/* Etichetta Base */

	//int nAnno;											/* Anno corrente */
	int nSAP;
} CFGSTRUCT, *PCFGSTRUCT;


typedef struct tagTask{
	int nId;
	int nCodice;
	char szParametri[512];
} ATASK, *PATASK;
