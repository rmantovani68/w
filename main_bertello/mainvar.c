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
PRINTERSSTRUCT Printers[MAX_PRINTERS];
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
char szOrdineInEdit[256];    /* codice ordine visualizzato nella dlg_edit_ordine */
int nColloInEdit;            /* numero del collo visualizzato nella dlg_edit_collo */
BOOL bStorico;               /* TRUE se l'edit dell'ordine e del collo e' riferito ai dati di storico */
char szOldElaborazione[5][128]={"","","","",""};	/* memoria di ultima elaborazione */
char szOldOrdKey[5][128]={"","","","",""};	/* memoria di ultima chiave ordine */
int nLstRigheColloRowSelected=0;

/*
* Connessione al DataBase PostgreSQL
*/
PGconn *DBConn=0;

/* widgets utilizzati nell'applicazione */
GtkWidget *main_window;
GtkWidget *sw_list;
GtkWidget *sw_isola1;
GtkWidget *sw_isola2;
GtkWidget *sw_lista_opeset;
GtkWidget *lst_ordini=NULL;
GtkWidget *lst_righe_collo=NULL;
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
GtkWidget *dlg_storico;
GtkWidget *dlg_operatori;

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
DBSTRUCT tOrdiniBertello;    /* Ordini Bertello */
DBSTRUCT tColliBertello;     /* Colli Bertello */
DBSTRUCT tRigheBertello;     /* Righe Bertello */

DBSTRUCT tOrdiniBertello_B;  /* Ordini Bertello B */
DBSTRUCT tOrdiniBertello_C;  /* Ordini Bertello C */ 
DBSTRUCT tOrdiniBertello_R;  /* Ordini Bertello R */ 
DBSTRUCT tColliBertello_B;   /* Colli Bertello B */
DBSTRUCT tColliBertello_C;   /* Colli Bertello C */ 
DBSTRUCT tColliBertello_R;   /* Colli Bertello R */ 
DBSTRUCT tRigheBertello_B;   /* Righe Bertello B */
DBSTRUCT tRigheBertello_C;   /* Righe Bertello C */ 
DBSTRUCT tRigheBertello_R;   /* Righe Bertello R */ 

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
DBSTRUCT tDettColCni; /* struttura file asci dettaglio colli da trasmettere a host */

/*
* vettore degli archivi utilizzati
*/
DB_ARRAY_STRUCT	Tables[]= {
	{&tOrdiniBertello, "ordini_bertello"},
	{&tColliBertello,  "colli_bertello"},
	{&tRigheBertello,  "righe_bertello"},
	{&tOrdiniBertello_B, "ordini_bertello_b"},
	{&tOrdiniBertello_C, "ordini_bertello_c"},
	{&tOrdiniBertello_R, "ordini_bertello_r"},
	{&tColliBertello_B,  "colli_bertello_b"},
	{&tColliBertello_C,  "colli_bertello_c"},
	{&tColliBertello_R,  "colli_bertello_r"},
	{&tRigheBertello_B,  "righe_bertello_b"},
	{&tRigheBertello_C,  "righe_bertello_c"},
	{&tRigheBertello_R,  "righe_bertello_r"},
	{&tRicOrd,         "ric_ord"},
	{&tRicArt,         "ric_art"},
	{&tArticoli,       "articoli"},
	{&tOrdProd,        "ord_prod"},
	{&tColProd,        "col_prod"},
	{&tRigProd,        "rig_prod"},
	{&tImballi,        "imballi"},
	{&tUbicazioni,     "ubicazioni"},
	{&tOrdCni,         "ord_cni"},
	{&tColCni,         "col_cni"},
	{&tDettColCni,     "dett_colli"},

	{NULL,NULL}
};
