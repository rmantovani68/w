/*
* printstruct.h
* 
* Progetto Mondadori Picking
*
* Autore : Roberto Mantovani
*
* Copyright CNI srl 1998
*/


typedef struct tagCfgStruct{
	/*
	* Dati generali
	*/
	char szCniCfg[128];
	char szLanguage[128];
	char szPathData[128];
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

typedef struct tagLabelFieldStruct{
	char szFieldID[80];
	char szFieldType[80];
	char szTableName[80];
	char szFieldName[80];
} LABEL_FIELD_STRUCT, *PLABEL_FIELD_STRUCT;

