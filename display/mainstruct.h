/*
* mainstruct.h
* Dichiarazione Strutture Utilizzate 
* Easy Picking 3.0
* Copyright CNI srl 1999-2000
* Autore : Roberto Mantovani
*/


enum ActualFrame {
	FRAME_DATI_PRODOTTO,
#ifdef STATISTICS
	FRAME_STATISTICHE,
#endif
	NUMERO_FRAME,
};


typedef struct tagCfgStruct{
	char szCfgFileName[128];
	char szDisplayCfgFileName[128];
	char szPathExe[128];
	char szPathTrace[128];
	char szPathData[128];
	char szPathStampe[128];
	char szPathExport[128];

	char szTipoOrdini[80];
	char szTipoUbicazioni[80];

	int nDebugVersion;

	int nMainDelay;
	int nTimer;
	int nBlinkingDelay;
	int nBlinkingTimer;

	char szDBHost[128]; 
	char szDBPort[128]; 
	char szDBName[128]; 

	int nLineaShmKey;
	int nSettoreIndex;
	int nSettore;
	int nIsola;
	int nNumeroSettori;
	int nSettoriShmKey;
	int bMaximize;
} CFGSTRUCT, *PCFGSTRUCT;

