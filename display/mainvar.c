/*
* mainvar.c
* Variabili Globali
* Easy Picking 3.0
* Copyright CNI srl 1999-2000
* Autore : Roberto Mantovani
*/


#include <gtk/gtk.h>

#include <signal.h>

#ifdef TRACE
	#include <trace.h>
#endif
#include <ep-common.h>
#include <ep-db.h>
#include <pmx_msq.h>
#include <dbfun-gtk.h>
#include <gtk-support.h>

#include <proc_list.h>
#include <picking.h>

#include "main.h"
#include "mainstruct.h"
#include "mainext.h"
#include "mainfun.h"


PROCESSO ProcList[NUM_PROC];	/* processi componenti il progetto */
PROCESSO DisplayList[MAX_DISPLAYS];
CFGSTRUCT Cfg;
//DISPLAY_CFGSTRUCT DisplayCfg;
PLINEA_STRUCT pDatiLinea;
PSETTORE pSettori;
SETTORE OldSettore;
char szDateBuffer[128];
char szTimeBuffer[128];
int nOldStatoLinea;
int nOldAttesaBadge;
int nActualFrame;

/* widgets utilizzati nell'applicazione */
GtkWidget *main_window;
GtkWidget *txt_msgs;

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


CFGITEM CFGItems[] =  {
	{"General Settings","PathData",       CFG_TYPE_STRING,{pszString:Cfg.szPathData},     {szString:"../data"},   CFG_ENTRY_WIDGET, "entry_data_path"},
	{"General Settings","PathStampe",     CFG_TYPE_STRING,{pszString:Cfg.szPathStampe},   {szString:"../stampe"}, CFG_ENTRY_WIDGET, "entry_print_path"},
	{"General Settings","PathTrace",      CFG_TYPE_STRING,{pszString:Cfg.szPathTrace},    {szString:"../trace"},  CFG_NO_WIDGET,    ""},
	
	{"General Settings","DisplayCfgFileName",        CFG_TYPE_STRING,{pszString:Cfg.szDisplayCfgFileName},      {szString:"display.cfg"},        CFG_ENTRY_WIDGET, "entry_display_cfg_file_name"},

	{"DataBase Settings","DBHost",                CFG_TYPE_STRING, {pszString:Cfg.szDBHost},               {szString:"localhost"},    CFG_ENTRY_WIDGET,         "entry_db_host"},
	{"DataBase Settings","DBPort",                CFG_TYPE_STRING, {pszString:Cfg.szDBPort},               {szString:"5432"},         CFG_ENTRY_WIDGET,         "entry_db_port"},
	{"DataBase Settings","DBName",                CFG_TYPE_STRING, {pszString:Cfg.szDBName},               {szString:"database"},     CFG_ENTRY_WIDGET,         "entry_db_name"},

	{"settori","MainDelay",           CFG_TYPE_INT,   {pnInt:&Cfg.nMainDelay},     {nInt:100},               CFG_NO_WIDGET, ""},
	{"Display","BlinkingDelay",       CFG_TYPE_INT,   {pnInt:&Cfg.nBlinkingDelay},     {nInt:300},               CFG_NO_WIDGET, ""},
	
	/* parametri senza widget */
	{"General Settings","TipoOrdini",     CFG_TYPE_STRING,{pszString:Cfg.szTipoOrdini},    {szString:"N"},         CFG_NO_WIDGET, ""},
	{"General Settings","TipoUbicazioni", CFG_TYPE_STRING,{pszString:Cfg.szTipoUbicazioni},{szString:"N"},         CFG_NO_WIDGET, ""},
	{"General Settings","ShmKey",         CFG_TYPE_INT,   {pnInt:&Cfg.nLineaShmKey},       {nInt:0},               CFG_NO_WIDGET, ""},
	{"General Settings","SettoriShmKey",  CFG_TYPE_INT,   {pnInt:&Cfg.nSettoriShmKey},     {nInt:0},               CFG_NO_WIDGET, ""},
	{"settori","NumeroSettori",           CFG_TYPE_INT,   {pnInt:&Cfg.nNumeroSettori},     {nInt:0},               CFG_NO_WIDGET, ""},

	{"Monitor","DebugVersion",   CFG_TYPE_INT,   {pnInt:&Cfg.nDebugVersion},     {nInt:0},               CFG_SPIN_BUTTON_WIDGET, "sb_debug_version"},

	{NULL,NULL,CFG_TYPE_NONE,{pszString:(char *)NULL},{szString:(char *)NULL}},
};

//CFGITEM CFGDisplayItems[] =  {
//	{"pango markups","lb_titolo_ordine_collo_pango_prefix",      CFG_TYPE_STRING,{pszString:DisplayCfg.szTitoloOrdineColloPrefix},     {szString:""},         CFG_NO_WIDGET, ""},
//	{"pango markups","lb_titolo_ordine_collo_pango_postfix",     CFG_TYPE_STRING,{pszString:DisplayCfg.szTitoloOrdineColloPostfix},    {szString:""},         CFG_NO_WIDGET, ""},
//	{"pango markups","lb_ordine_collo_pango_prefix",             CFG_TYPE_STRING,{pszString:DisplayCfg.szOrdineColloPrefix},           {szString:""},         CFG_NO_WIDGET, ""},
//	{"pango markups","lb_ordine_collo_pango_postfix",            CFG_TYPE_STRING,{pszString:DisplayCfg.szOrdineColloPostfix},          {szString:""},         CFG_NO_WIDGET, ""},
//	{"pango markups","lb_titolo_settore_pango_prefix",           CFG_TYPE_STRING,{pszString:DisplayCfg.szTitoloSettorePrefix},         {szString:""},         CFG_NO_WIDGET, ""},    
//	{"pango markups","lb_titolo_settore_pango_postfix",          CFG_TYPE_STRING,{pszString:DisplayCfg.szTitoloSettorePostfix},        {szString:""},         CFG_NO_WIDGET, ""},   
//	{"pango markups","lb_settore_pango_prefix",                  CFG_TYPE_STRING,{pszString:DisplayCfg.szSettorePrefix},               {szString:""},         CFG_NO_WIDGET, ""},          
//	{"pango markups","lb_settore_pango_postfix",                 CFG_TYPE_STRING,{pszString:DisplayCfg.szSettorePostfix},              {szString:""},         CFG_NO_WIDGET, ""},         
//	{"pango markups","lb_stato_linea_pango_prefix",              CFG_TYPE_STRING,{pszString:DisplayCfg.szStatoLineaPrefix},            {szString:""},         CFG_NO_WIDGET, ""},       
//	{"pango markups","lb_stato_linea_pango_postfix",             CFG_TYPE_STRING,{pszString:DisplayCfg.szStatoLineaPostfix},           {szString:""},         CFG_NO_WIDGET, ""},      
//	{"pango markups","lb_titolo_operatore_pango_prefix",         CFG_TYPE_STRING,{pszString:DisplayCfg.szTitoloOperatorePrefix},       {szString:""},         CFG_NO_WIDGET, ""},  
//	{"pango markups","lb_titolo_operatore_pango_postfix",        CFG_TYPE_STRING,{pszString:DisplayCfg.szTitoloOperatorePostfix},      {szString:""},         CFG_NO_WIDGET, ""}, 
//	{"pango markups","lb_codice_operatore_pango_prefix",         CFG_TYPE_STRING,{pszString:DisplayCfg.szCodiceOperatorePrefix},       {szString:""},         CFG_NO_WIDGET, ""},  
//	{"pango markups","lb_codice_operatore_pango_postfix",        CFG_TYPE_STRING,{pszString:DisplayCfg.szCodiceOperatorePostfix},      {szString:""},         CFG_NO_WIDGET, ""}, 
//	{"pango markups","lb_dati_operatore_pango_prefix",           CFG_TYPE_STRING,{pszString:DisplayCfg.szDatiOperatorePrefix},         {szString:""},         CFG_NO_WIDGET, ""},    
//	{"pango markups","lb_dati_operatore_pango_postfix",          CFG_TYPE_STRING,{pszString:DisplayCfg.szDatiOperatorePostfix},        {szString:""},         CFG_NO_WIDGET, ""},   
//	{"pango markups","lb_operazione_pango_prefix",               CFG_TYPE_STRING,{pszString:DisplayCfg.szOperazionePrefix},            {szString:""},         CFG_NO_WIDGET, ""},       
//	{"pango markups","lb_operazione_pango_postfix",              CFG_TYPE_STRING,{pszString:DisplayCfg.szOperazionePostfix},           {szString:""},         CFG_NO_WIDGET, ""},      
//	{"pango markups","lb_copie_pango_prefix",                    CFG_TYPE_STRING,{pszString:DisplayCfg.szCopiePrefix},                 {szString:""},         CFG_NO_WIDGET, ""},            
//	{"pango markups","lb_copie_pango_postfix",                   CFG_TYPE_STRING,{pszString:DisplayCfg.szCopiePostfix},                {szString:""},         CFG_NO_WIDGET, ""},           
//	{"pango markups","lb_titolo_ubicazione_pango_prefix",        CFG_TYPE_STRING,{pszString:DisplayCfg.szTitoloUbicazionePrefix},      {szString:""},         CFG_NO_WIDGET, ""}, 
//	{"pango markups","lb_titolo_ubicazione_pango_postfix",       CFG_TYPE_STRING,{pszString:DisplayCfg.szTitoloUbicazionePostfix},     {szString:""},         CFG_NO_WIDGET, ""},
//	{"pango markups","lb_ubicazione_pango_prefix",               CFG_TYPE_STRING,{pszString:DisplayCfg.szUbicazionePrefix},            {szString:""},         CFG_NO_WIDGET, ""},       
//	{"pango markups","lb_ubicazione_pango_postfix",              CFG_TYPE_STRING,{pszString:DisplayCfg.szUbicazionePostfix},           {szString:""},         CFG_NO_WIDGET, ""},      
//	{"pango markups","lb_titolo_codice_pango_prefix",            CFG_TYPE_STRING,{pszString:DisplayCfg.szTitoloCodicePrefix},          {szString:""},         CFG_NO_WIDGET, ""}, 
//	{"pango markups","lb_titolo_codice_pango_postfix",           CFG_TYPE_STRING,{pszString:DisplayCfg.szTitoloCodicePostfix},         {szString:""},         CFG_NO_WIDGET, ""},
//	{"pango markups","lb_codice_pango_prefix",                   CFG_TYPE_STRING,{pszString:DisplayCfg.szCodicePrefix},                {szString:""},         CFG_NO_WIDGET, ""},       
//	{"pango markups","lb_codice_pango_postfix",                  CFG_TYPE_STRING,{pszString:DisplayCfg.szCodicePostfix},               {szString:""},         CFG_NO_WIDGET, ""},      
//	{"pango markups","lb_titolo_titolo_pango_prefix",            CFG_TYPE_STRING,{pszString:DisplayCfg.szTitoloTitoloPrefix},          {szString:""},         CFG_NO_WIDGET, ""},     
//	{"pango markups","lb_titolo_titolo_pango_postfix",           CFG_TYPE_STRING,{pszString:DisplayCfg.szTitoloTitoloPostfix},         {szString:""},         CFG_NO_WIDGET, ""},    
//	{"pango markups","lb_titolo_pango_prefix",                   CFG_TYPE_STRING,{pszString:DisplayCfg.szTitoloPrefix},                {szString:""},         CFG_NO_WIDGET, ""},           
//	{"pango markups","lb_titolo_pango_postfix",                  CFG_TYPE_STRING,{pszString:DisplayCfg.szTitoloPostfix},               {szString:""},         CFG_NO_WIDGET, ""},          
//
//
//	
//
//	{NULL,NULL,CFG_TYPE_NONE,{pszString:(char *)NULL},{szString:(char *)NULL}},
//};
