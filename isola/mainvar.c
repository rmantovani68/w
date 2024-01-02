/*
* mainvar.c
* Variabili Globali
* Easy Picking 3.0
* Copyright CNI srl 1999-2000
* Autore : Roberto Mantovani
*/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#ifdef TRACE
	#include <trace.h>
#endif

#include <gtk/gtk.h>
#include <signal.h>
#include <libpq-fe.h>

#include "interface.h"
#include "support.h"

#include <proc_list.h>
#include <ca_file.h>
#include <pmx_msq.h>
#include <dbfun.h>

#include "picking.h"
#include "mainstruct.h"
#include "mainext.h"
#include "mainfun.h"


CFGSTRUCT Cfg;
PROCESSO ProcList[NUM_PROC];	/* processi componenti il progetto */
PLINEA_STRUCT pDatiLinea;
char szDateBuffer[128];
char szTimeBuffer[128];

/*
* riga selezionata lista ordini
*/
int nLstOrdiniRowSelected=0;
/*
* ordine in edit
*/
char szOrdineInEdit[256];
int nColloInEdit;

/*
* Connessione al DataBase PostgreSQL
*/
PGconn *DBConn;

/* widgets utilizzati nell'applicazione */
GtkWidget *main_window;
GtkWidget *sw_list;
GtkWidget *lst_ordini=NULL;
GtkWidget *clist_distribuzione=NULL;
GtkWidget *dlg_exit;
GtkWidget *dlg_message;
GtkWidget *dlg_edit_ordine;
GtkWidget *dlg_edit_collo;
GtkWidget *dlg_distribuzione;
GtkWidget *dlg_lancio_distribuzioni;
GtkWidget *dlg_select;
GtkWidget *dlg_import;
GtkWidget *dlg_import_file;
GtkWidget *dlg_import_ordini;
GtkWidget *dlg_export;
GtkWidget *dlg_export_message;
GtkWidget *dlg_app_error;
GtkWidget *txt_msgs;
GtkWidget *dlg_set_ordini;
GtkWidget *dlg_reset_settore;
GtkWidget *dlg_config;
GtkWidget *dlg_statistiche_elab;

/*
* Definizioni costanti colore
*/
GdkColor cLBLUE    = {0, 0x0000, 0x0000, 0xffff};
GdkColor cBLUE     = {0, 0x0000, 0x0000, 0x8000};
GdkColor cBLACK    = {0, 0x0000, 0x0000, 0x0000};
GdkColor cRED      = {0, 0x8000, 0x0000, 0x0000};
GdkColor cLRED     = {0, 0xffff, 0x0000, 0x0000};
GdkColor cLCYAN    = {0, 0xbb00, 0xccee, 0xffff};
GdkColor cLICE     = {0, 0xbefb, 0xc71b, 0xffff};
GdkColor cCYAN     = {0, 0x6000, 0xBB00, 0xFFFF};
GdkColor cWHITE    = {0, 0xffff, 0xffff, 0xffff};
GdkColor cYELLOW   = {0, 0xffff, 0x8888, 0x0000};
GdkColor cLYELLOW  = {0, 0xffff, 0xffff, 0xcccc};
GdkColor cLGREEN   = {0, 0x0000, 0xBB00, 0x0000};
GdkColor cGREEN    = {0, 0x0000, 0x8000, 0x0000};
GdkColor cGREYCYAN = {0, 0xdd00, 0xdd00, 0xee00};
GdkColor cLGREY    = {0, 0xcc00, 0xcc00, 0xcc00};
GdkColor cGREY     = {0, 0x8800, 0x8800, 0x8800};
GdkColor cSLGREY   = {0, 0xd75c, 0xd75c, 0xd75c};
GdkColor cSGREY    = {0, 0xc71b, 0xc30b, 0xc71b};
GdkColor cORANGE   = {0, 0x9b00, 0x7300, 0x0000};

/*
* Definizioni per le tabelle utilizzate in IMPORT/EXPORT
*/
DBSTRUCT tRicOrd;     /* Ordini Ricevuti */
DBSTRUCT tRicArt;     /* Righe Ricevute */
DBSTRUCT tArticoli;   /* Prodotti */
DBSTRUCT tOrdProd;    /* Ordini di Produzione */
DBSTRUCT tColProd;    /* Colli di Produzione */
DBSTRUCT tRigProd;    /* Righe di Produzione */
DBSTRUCT tImballi;    /* Anagrafica Imballi */
DBSTRUCT tUbicazioni; /* Anagrafica Ubicazioni */
DBSTRUCT tOrdCni;     /* struttura file asci Ordcni da trasmettere a host */
DBSTRUCT tColCni;     /* struttura file asci colli da trasmettere a host */

/*
* vettore degli archivi utilizzati
*/
DB_ARRAY_STRUCT	Tables[]= {
	{&tRicOrd,     "ric_ord"},
	{&tRicArt,     "ric_art"},
	{&tArticoli,   "articoli"},
	{&tOrdProd,    "ord_prod"},
	{&tColProd,    "col_prod"},
	{&tRigProd,    "rig_prod"},
	{&tImballi,    "imballi"},
	{&tUbicazioni, "ubicazioni"},
	{&tOrdCni,     "ord_cni"},
	{&tColCni,     "col_cni"},

	{NULL,NULL}
};

/*
* comandi ricevibili dal socket server 
*/
char *pszSockCmds[]={
	"DISPLAY_MSG",
	"DISPLAY_MSGBOX",
	"UPDATE_ORDINE",
	"PRINT_PDF",
	NULL,
};
