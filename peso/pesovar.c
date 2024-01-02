/*
* pesovar.c
* Gestione Peso/Fine Linea
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

#include "plcsock.h"

#include "picking.h"
#include "pesostruct.h"
#include "pesoext.h"
#include "pesofun.h"


CFGSTRUCT Cfg;
PROCESSO ProcList[NUM_PROC];	/* processi componenti il progetto */
PLINEA_STRUCT pDatiLinea;
char szDateBuffer[128];
char szTimeBuffer[128];

/*
* riga selezionata lista colli
*/
int nLstColliRowSelected=0;
int nLstColliScartatiRowSelected=0;
int nLstRigheColloRowSelected=0;
/*
* ordine in edit
*/
char szOrdineInEdit[256];
int nColloInEdit;
char szBarcodeColloBilancia[256];
char szBarcodeColloPz[256];
int nPesoColloBilancia;

/*
* Connessione al DataBase PostgreSQL
*/
PGconn *DBConn;
/*
* Connessione al PLC CNI (QPLC) via socket 
*/
int fdPLC;
int nBilanciaTimeOut;
PLCVAR PLCVars[128];
int nCPB;
int nCPLZ;
int nCB;
int nCS;
int nFO;
int nRDCPLZ;
int nRFCPLZ;
int nNCPLZ;

int nMSGTX;
int nMSGRX;
int nMSGCODE;
int nMSGCLAS;

int nINICDN;
int nINICDFD;

/*
* widgets utilizzati nell'applicazione
*/
GtkWidget *main_window;
GtkWidget *dlg_exit;
GtkWidget *dlg_message;
GtkWidget *dlg_info;
GtkWidget *dlg_config;
GtkWidget *dlg_app_error;
GtkWidget *dlg_edit_ordine;
GtkWidget *dlg_edit_collo;
GtkWidget *dlg_barcode;
GtkWidget *dlg_trace;
GtkWidget *dlg_plc_error;
GtkWidget *dlg_debug_var;
GtkWidget *dlg_elimina_collo;
GtkWidget *dlg_aggiungi_collo;
GtkWidget *dlg_scarto;
GtkWidget *dlg_plc_msg=(GtkWidget *)0;
GtkWidget *lst_var_list;
GtkWidget *sw_list;
GtkWidget *sw_list_scartati;
GtkWidget *txt_msgs;
GtkWidget *lst_colli;
GtkWidget *lst_colli_scartati;
GtkWidget *lst_righe_collo;
GtkWidget *lb_status_msg;
GtkWidget *dlg_lettura_barcode_id;


/*
* definizione dei colori
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
DBSTRUCT tOrdCni;     /* struttura file asci Ordcni da taramettere a host */
DBSTRUCT tColCni;     /* struttura file asci colli da taramettere a host */

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
  "PRINT_RAC_COLLO",

	NULL,
};
