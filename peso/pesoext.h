/*
* pesoext.h
* Dichiarazioni External
* Gestione Peso/Fine Linea
* Easy Picking 3.0
* Copyright CNI srl 1999-2000
* Autore : Roberto Mantovani
*/

extern CFGSTRUCT Cfg;
extern PROCESSO ProcList[NUM_PROC];	/* processi componenti il progetto */
extern PLINEA_STRUCT pDatiLinea;
extern char szDateBuffer[128];
extern char szTimeBuffer[128];

/*
* riga selezionata lista colli
*/
extern int nLstColliRowSelected;
extern int nLstColliScartatiRowSelected;
extern int nLstRigheColloRowSelected;
extern char szOrdineInEdit[256];
extern int nColloInEdit;
extern char szBarcodeColloBilancia[256];
extern char szBarcodeColloPz[256];
extern int nPesoColloBilancia;

/*
* Connessione al DataBase PostgreSQL
*/
extern PGconn *DBConn;
/*
* Connessione al PLC CNI (QPLC) via socket 
*/
extern int fdPLC;
extern int nBilanciaTimeOut;
extern PLCVAR PLCVars[128];
extern int nCPB;
extern int nCPLZ;
extern int nCB;
extern int nCS;
extern int nFO;
extern int nRDCPLZ;
extern int nRFCPLZ;
extern int nNCPLZ;
extern int nMSGTX;
extern int nMSGRX;
extern int nMSGCODE;
extern int nMSGCLAS;
extern int nINICDN;
extern int nINICDFD;

/*
* widgets utilizzati nell'applicazione 
*/
extern GtkWidget *main_window;
extern GtkWidget *dlg_exit;
extern GtkWidget *dlg_message;
extern GtkWidget *dlg_info;
extern GtkWidget *dlg_config;
extern GtkWidget *dlg_app_error;
extern GtkWidget *dlg_edit_ordine;
extern GtkWidget *dlg_edit_collo;
extern GtkWidget *dlg_barcode;
extern GtkWidget *dlg_trace;
extern GtkWidget *dlg_plc_error;
extern GtkWidget *dlg_debug_var;
extern GtkWidget *dlg_elimina_collo;
extern GtkWidget *dlg_aggiungi_collo;
extern GtkWidget *dlg_scarto;
extern GtkWidget *dlg_plc_msg;
extern GtkWidget *lst_var_list;
extern GtkWidget *sw_list;
extern GtkWidget *sw_list_scartati;
extern GtkWidget *txt_msgs;
extern GtkWidget *lst_colli;
extern GtkWidget *lst_colli_scartati;
extern GtkWidget *lst_righe_collo;
extern GtkWidget *lb_status_msg;
extern GtkWidget *dlg_lettura_barcode_id;

/*
* Definizioni costanti colore
*/
extern GdkColor cLBLUE;
extern GdkColor cBLUE;
extern GdkColor cBLACK;
extern GdkColor cRED;
extern GdkColor cLRED;
extern GdkColor cLCYAN;
extern GdkColor cLICE;
extern GdkColor cCYAN;
extern GdkColor cWHITE;
extern GdkColor cYELLOW;
extern GdkColor cLYELLOW;
extern GdkColor cLGREEN;
extern GdkColor cGREEN;
extern GdkColor cGREYCYAN;
extern GdkColor cLGREY;
extern GdkColor cGREY;
extern GdkColor cSLGREY;
extern GdkColor cSGREY;
extern GdkColor cORANGE;

/*
* Definizioni per le tabelle utilizzate in IMPORT/EXPORT
*/
extern DBSTRUCT tRicOrd;          /* Ordini Ricevuti */
extern DBSTRUCT tRicArt;          /* Righe Ricevute */
extern DBSTRUCT tArticoli;        /* Prodotti */
extern DBSTRUCT tOrdProd;         /* Bolle di Produzione */
extern DBSTRUCT tColProd;         /* Colli di Produzione */
extern DBSTRUCT tRigProd;         /* Righe di Produzione */
extern DBSTRUCT tImballi;         /* Anagrafica Imballi */
extern DBSTRUCT tUbicazioni;      /* Anagrafica Ubicazioni */
extern DBSTRUCT tOrdCni;					/* Struttura file di Export Ordcni.txt */
extern DBSTRUCT tColCni;					/* Struttura file di Export Colli.txt */

/*
* vettore degli archivi utilizzati
*/
extern DB_ARRAY_STRUCT Tables[];
extern char *pszSockCmds[];
