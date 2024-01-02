/*
* mainfun.c
* funzioni 
* Easy Picking 3.0
* Copyright A&L srl 1999-2000
* Autore : Roberto Mantovani
*/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <signal.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <libpq-fe.h>
#include <shared.h>

#include "interface.h"
#include "support.h"

#ifdef TRACE
	#include <trace.h>
#endif
#include <ca_file.h>
#include <pmx_msq.h>
#include <dbfun.h>

#include "picking.h"
#include "mainstruct.h"
#include "mainext.h"
#include "mainfun.h"

#include "callbacks.h"


/*
* GTK-Error handling functions
*/
void gtk_error_handler(gchar *str)
{
	gtk_text_printf(NULL,txt_msgs,str);
}

void gtk_warning_handler(gchar *str)
{
	gtk_text_printf(NULL,txt_msgs,str);
}

void gtk_message_handler(gchar *str)
{
	gtk_text_printf(NULL,txt_msgs,str);
}


/*
* ReadConfiguration()
* lettura della configurazione dal file cni.cfg
*/
BOOL ReadConfiguration(void)
{
	char szCurrentDirectory[128];

	/*
	* leggo il file di configurazione dalla directory corrente
	*/
	getcwd(szCurrentDirectory,sizeof(szCurrentDirectory));
	strcpy(Cfg.szCfgFile,szCurrentDirectory);
	strcat(Cfg.szCfgFile,"/cni.cfg");

	if(!FileExists(Cfg.szCfgFile)){
#ifdef TRACE
	trace_out_vstr_date(1,"Read Configuration : file cni.cfg not present");
#endif
		return FALSE;
	}

	/* CFGItems viene dichiarato e inizializzato in mainvar.c */
	ReadCfgItems(CFGItems,Cfg.szCfgFile);
	
	return TRUE;
}

void ExitApp()
{
#ifdef TRACE
	trace_out_vstr_date(1,"Stopped");
#endif

	DisconnectDataBase();

	/* chiudo la GUI */
	gtk_widget_destroy(main_window);
	gtk_main_quit();
}

void EditConfigurazione(GtkWidget *dlg)
{	
}

void ApplyConfigurazione(GtkWidget *dlg)
{
}


/*
* int GetKeyIndex(char *pszKey,GtkCList *clist,int nColumn,int nStartRow);
*
* effettua, nella GtkClist passatagli, la ricerca
* per il campo pszKey presente in colonna nColumn
* a partire dalla riga nStartRow
*/
int GetKeyIndex(char *pszKey,GtkCList *clist,int nColumn, int nStartRow)
{
	int nIndex=nStartRow;
	char *pszText;

	/* la chiave della lista e' costituita sempre dal parametro nColumn */
	while(gtk_clist_get_text(GTK_CLIST(clist),nIndex,nColumn,&pszText)){
		if(!strcasecmp(StrTrimAll(pszKey),StrTrimAll(pszText))){
			return(nIndex);
		}
		nIndex++;
	}
	return(-1);
}

/*
* void clist_move_to_row(GtkCList *clist, int nRow)
*
* posiziona la <clist> passatagli alla riga <nRow>
*
* parametri formali:
*
* - GtkCList *clist: gtk columned list;
* - int nRow: riga della GtkCList su cui posizionarsi (da 0 a ...).
*
* ritorna (void)
*/
void clist_move_to_row(GtkCList *clist, int nRow)
{
	if( (GTK_CLIST(clist)==NULL) || (nRow>(GTK_CLIST(clist)->rows)) ) return;

	gtk_clist_moveto(GTK_CLIST(clist), nRow, -1, 0.5, 0);
	gtk_clist_select_row(GTK_CLIST(clist), nRow, -1);
}

/*
* void clist_update_selected_row(GtkCList *clist, int nStep)
*
* aggiorna la selezione della riga di una GtkCList in Selection Mode = Single
*
* parametri formali:
*
* - GtkCList *clist: gtk columned list;
* - int nStep: valore incrementale riferito alla Riga gia' selezionata
*              oppure a partire dall'inizio della clist se nessuna riga e' selezionata
*							
* ritorna (void)
*/
void clist_update_selected_row(GtkCList *clist, int nStep)
{
	int nRowSelected;

	if(GTK_CLIST(clist)==NULL) return;

	if((GTK_CLIST(clist)->selection)){
		nRowSelected=GPOINTER_TO_INT(GTK_CLIST(clist)->selection->data);
		gtk_clist_moveto(GTK_CLIST(clist), nRowSelected+nStep, -1, 0.5, 0);
		gtk_clist_select_row(GTK_CLIST(clist), nRowSelected+nStep, -1);
	} else {
		gtk_clist_moveto(GTK_CLIST(clist), nStep, -1, 0.5, 0);
		gtk_clist_select_row(GTK_CLIST(clist), nStep, -1);
	}
}


#ifdef OLD_VERSION

BOOL get_media_oraria(float *pfColli,float *pfRighe,float *pfCopie,float *pfScarti,float *pfOperatori)
{
	PGresult *PGRes;
	PGresult *PGResData;
	int nTotGiorni=0;
	int nTotOre=0;
	int nTotColli=0;
	int nTotRighe=0;
	int nTotCopie=0;
	int nTotScarti=0;
	int nMaxValue;
	int nMinValue;
	int nTuples;
	int nTuplesData;
	int nIndex;
	int nOra;
	int nOraIndex;
	char szBuffer[128];
	BOOL bOK=TRUE;

	szBuffer[0]='\0';
	PGRes=PGExecSQL(Cfg.nDebugVersion>1,"select data(cptmeva) as data,substring(ora(cptmeva),1,2) as ora,count(ordprog) as colli,sum(cpnmrgh) as righe,sum(cpnmcpe) as copie ,sum(cpcntsc) as scarti from col_prod where cpstato in ('E','H') group by data(cptmeva),substring(ora(cptmeva),1,2);");
	if((nTuples=PQntuples(PGRes))){
		nMaxValue=0;
		nMinValue=0;
		for(nIndex=0;nIndex<nTuples;nIndex++){
			if(strcmp(szBuffer,PQgetvalue(PGRes,nIndex,0))){
				/* cambio data */
				nTotGiorni++;
				strcpy(szBuffer,PQgetvalue(PGRes,nIndex,0));
			}
			/* calcolo valore max e media giornaliera */
			nMaxValue=max(nMaxValue,atoi(PQgetvalue(PGRes,nIndex,2)));
			nMinValue=max(nMinValue,atoi(PQgetvalue(PGRes,nIndex,2)));
			nTotOre++;
			nTotColli+=atoi(PQgetvalue(PGRes,nIndex,2));
			nTotRighe+=atoi(PQgetvalue(PGRes,nIndex,3));
			nTotCopie+=atoi(PQgetvalue(PGRes,nIndex,4));
			nTotScarti+=atoi(PQgetvalue(PGRes,nIndex,5));
		}
		*pfColli=(float)nTotColli/(float)max(nTotOre,1);
		*pfRighe=(float)nTotRighe/(float)max(nTotOre,1);
		*pfCopie=(float)nTotCopie/(float)max(nTotOre,1);
		*pfScarti=(float)nTotScarti/(float)max(nTotOre,1);
		*pfOperatori=*pfColli/Cfg.nNumeroOperatori;
	} else {
		bOK=FALSE;
	}
	PQclear(PGRes);

  return bOK;
}

#endif /* OLD_VERSION */

BOOL get_media_oraria(float *pfColli,float *pfRighe,float *pfCopie,float *pfScarti,float *pfOperatori)
{
	PGresult *PGRes;
	PGresult *PGResData;
	PGresult *PGResTime;
	int nTotGiorni=0;
	int nTotOre=0;
	int nTotColli=0;
	int nTotRighe=0;
	int nTotCopie=0;
	int nTotScarti=0;
	int nMaxValue;
	int nMinValue;
	int nTuples;
	int nTuplesData;
	int nIndex;
	int nOra;
	int nOraIndex;
	char szBuffer[128];
	BOOL bOK=TRUE;
	BOOL bStart=FALSE;
	char szEvento[128];
	char szTime[128];
	char szTimeStart[128];
	char szTimeStop[128];
	int nTempoStart;

	/* ricavo il numero di colli/righe/copie della giornata odierna */

	szBuffer[0]='\0';
	PGRes=PGExecSQL(Cfg.nDebugVersion>1,"select data(cptmeva) as data,substring(ora(cptmeva),1,2) as ora,count(ordprog) as colli,sum(cpnmrgh) as righe,sum(cpnmcpe) as copie ,sum(cpcntsc) as scarti from col_prod where cpstato in ('E','H') and date(cptmeva)=date('now') group by data(cptmeva),substring(ora(cptmeva),1,2);");
	if((nTuples=PQntuples(PGRes))){
		nMaxValue=0;
		nMinValue=0;
		for(nIndex=0;nIndex<nTuples;nIndex++){
			if(strcmp(szBuffer,PQgetvalue(PGRes,nIndex,0))){
				/* cambio data */
				nTotGiorni++;
				strcpy(szBuffer,PQgetvalue(PGRes,nIndex,0));
			}
			/* calcolo valore max e media giornaliera */
			nMaxValue=max(nMaxValue,atoi(PQgetvalue(PGRes,nIndex,2)));
			nMinValue=max(nMinValue,atoi(PQgetvalue(PGRes,nIndex,2)));
			nTotOre++;
			nTotColli+=atoi(PQgetvalue(PGRes,nIndex,2));
			nTotRighe+=atoi(PQgetvalue(PGRes,nIndex,3));
			nTotCopie+=atoi(PQgetvalue(PGRes,nIndex,4));
			nTotScarti+=atoi(PQgetvalue(PGRes,nIndex,5));
		}
	} else {
		bOK=FALSE;
	}
	PQclear(PGRes);


	/* ricavo il tempo di start dell'impianto */


	PGRes=PGExecSQL(Cfg.nDebugVersion>1,"select evento,data from eventi where date(data)=date('now') order by data;");

	bStart=FALSE;
	nTempoStart=0;

	if((nTuples=PQntuples(PGRes))){
		for(nIndex=0;nIndex<nTuples;nIndex++){
			strcpy(szEvento,PQgetvalue(PGRes,nIndex,0));
			strcpy(szTime,PQgetvalue(PGRes,nIndex,1));
			if(!strncmp(szEvento,"START",strlen("START"))){
				/* comincio a 'contare' */
				bStart=TRUE;
				strcpy(szTimeStart,szTime);
			} else if(!strncmp(szEvento,"STOP",strlen("STOP"))){
				/* Aggiungo al tempo di start */
				bStart=FALSE;
				strcpy(szTimeStop,szTime);
				PGResTime=PGExecSQL(Cfg.nDebugVersion>1,"select extract(epoch from '%s'::timestamp)-extract(epoch from '%s'::timestamp);",szTimeStop,szTimeStart);
				if(PQntuples(PGResTime)){
					nTempoStart+=atoi(PQgetvalue(PGResTime,0,0));
				}
				PQclear(PGResTime);
			}
		}
	}
	/*
	* se alla fine del ciclo sono ancora in start
	* sommo al tempo in start la differenza fra adesso e l'ultimo evento di start 
	*/
	if(bStart){
		PGResTime=PGExecSQL(Cfg.nDebugVersion>1,"select extract(epoch from 'now'::timestamp)-extract(epoch from '%s'::timestamp);",szTimeStart);
		if(PQntuples(PGResTime)){
			nTempoStart+=atoi(PQgetvalue(PGResTime,0,0));
		}
		PQclear(PGResTime);
	}

#ifdef TRACE
	trace_out_vstr_date(1,"Numero Colli   : %4d",nTotColli);
	trace_out_vstr_date(1,"Numero Righe   : %4d",nTotRighe);
	trace_out_vstr_date(1,"Numero Copie   : %4d",nTotCopie);
	trace_out_vstr_date(1,"Colli Scartati : %4d",nTotScarti);
	trace_out_vstr_date(1,"Secondi        : %4d",nTempoStart);
	trace_out_vstr_date(1,"Ore            : %4f",(float)nTempoStart/(float)3600);
	trace_out_vstr_date(1,"Colli/Ora      : %4f",(float)nTotColli/((float)nTempoStart/(float)3600));
	trace_out_vstr_date(1,"Scarti/Ora     : %4f",(float)nTotScarti/((float)nTempoStart/(float)3600));
#endif




	

	PQclear(PGRes);

	
	*pfColli=(float)nTotColli/(float)max((float)nTempoStart/(float)3600,(float)1);
	*pfRighe=(float)nTotRighe/(float)max((float)nTempoStart/(float)3600,(float)1);
	*pfCopie=(float)nTotCopie/(float)max((float)nTempoStart/(float)3600,(float)1);
	*pfScarti=(float)nTotScarti/(float)max((float)nTempoStart/(float)3600,(float)1);
	*pfOperatori=*pfColli/Cfg.nNumeroOperatori;


  return bOK;
}
