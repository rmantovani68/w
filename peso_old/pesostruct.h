/*
* mainstruct.h
* Dichiarazione Strutture Utilizzate 
* Gestione Peso/Fine Linea
* Easy Picking 3.0
* Copyright CNI srl 1999-2000
* Autore : Roberto Mantovani
*/

typedef struct tagCfgStruct{
	char szCniCfg[128];          /* File di configurazione generale */
	char szPathExe[128];
	char szPathData[128];
	char szPathExport[128];
	char szImpOrdiniFile[128];		/* File di import dati */
	char szImpRigheFile[128];		
	char szImpArticoliFile[128];
	char szImpUbicazioniFile[128];
	char szExpOrdiniFile[128];	 	/* File di export dati */
	char szExpColliFile[128];		
	char szLanguage[128];
	char szSuperPwd[20];
	char szSuperPwdCfm[20];
	char szChiefPwd[20];
	char szChiefPwdCfm[20];
	char szPwd[20];
	int nActualPwd;
	unsigned long ulSuperPwd;
	unsigned long ulChiefPwd;
	int nPesoDelay;
	int nBilanciaTimeout;
	int nDebugVersion;
	char szPLCHost[128]; 
	int nPLC;
	int nPLCService;
	int nServicePort;
	char szSocksHost[128];
	int nShmKey; 
	char szPGHost[128]; 
	char szPGPort[128]; 
	char szPGDataBase[128]; 
	char szPrinterName[128];
	char szLabelPrinterName[128];
	int nTolleranzaAss;
	int nTolleranzaPerc;
	int nSogliaMax;
	TOLLERANZA Tolleranza;
	GESTIONE_PESATURA GestionePesatura;
	int nColloKeyField;                 /* Indice campo ordkey in lista colli */
	int nColloNumField;                 /* Indice del campo numero-collo in lista colli */
	int nSAP;
	int nExecuteCni2Sap;
} CFGSTRUCT, *PCFGSTRUCT;


