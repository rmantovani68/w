/*
* mainext.h
* Dichiarazioni External
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
* riga selezionata lista ordini
*/
extern int nLstOrdiniRowSelected;
extern char szOrdineInEdit[256];
extern int nColloInEdit;

/*
* Connessione al DataBase PostgreSQL
*/
extern PGconn *DBConn;

/*
* widgets utilizzati nell'applicazione 
*/
extern GtkWidget *main_window;
extern GtkWidget *sw_list;
extern GtkWidget *lst_ordini;
extern GtkWidget *clist_distribuzione;
extern GtkWidget *dlg_exit;
extern GtkWidget *dlg_message;
extern GtkWidget *dlg_edit_ordine;
extern GtkWidget *dlg_edit_collo;
extern GtkWidget *dlg_distribuzione;
extern GtkWidget *dlg_lancio_distribuzioni;
extern GtkWidget *dlg_select;
extern GtkWidget *dlg_import;
extern GtkWidget *dlg_import_file;
extern GtkWidget *dlg_import_ordini;
extern GtkWidget *dlg_export;
extern GtkWidget *dlg_export_message;
extern GtkWidget *txt_msgs;
extern GtkWidget *dlg_set_ordini;
extern GtkWidget *dlg_reset_settore;
extern GtkWidget *dlg_config;
extern GtkWidget *dlg_statistiche_elab;

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