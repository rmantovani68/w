/*
* mainstruct.h
* Dichiarazione Strutture Utilizzate 
* Easy Picking 3.0
* Copyright CNI srl 1999-2000
* Autore : Roberto Mantovani
*/

typedef struct tagCfgStruct{
	int nSAP;
	int nIsola;                 /* Isola attiva */
	int nOrdineKeyField;        /* Indice campo chiave */
	char szCniCfg[80];
	char szPathExe[80];
	char szPathData[80];
	char szPathExport[80];
	char szPrinterName[80];
	char szBollePrinterName[80];
	char szFatturePrinterName[80];
	char szLabelPrinterName[80];
	char szLanguage[80];
	int nMainDelay;
	int nRefreshDelay;
	int nDebugVersion;
	char szPGHost[128]; 
	char szPGPort[128]; 
	char szPGDataBase[128]; 
	int nServicePort;
	char szSocksHost[80];
} CFGSTRUCT, *PCFGSTRUCT;


