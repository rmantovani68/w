/*
* modulo : dbfun-gtk.c
* --------------------
* Progetto Easy Picking - DB Management - PostgreSQL - GTK
*
* Data creazione 18/06/2002
*
* Autore : Roberto Mantovani
*
* Copyright SMD srl 2002
*
* Modifiche
* -------------   -------------------------------------------------------
* rm 18-06-2002 : Creazione
* -------------   -------------------------------------------------------
*/

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include <gtk/gtk.h>
#include <gm-clist.h>

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#if defined(TRACE) || defined(TRACE_SQL)
	#include <trace.h>
#endif

#include <ep-common.h>
#include <ep-db.h>
#include <gtk-support.h>
#include <dbfun-gtk.h>

/*
* void on_lst_sort_column_clicked (GtkCList *clist, gint nColumnNumber, gpointer user_data)
*
* Effettua un ordinamento sulla clist : utilizza la colonna (nColumnNumber) selezionata come discriminante;
* se la stessa colonna viene selezionanata di seguito l'ordinamento varia da ascendente a discentente.
*/
void on_lst_sort_column_clicked (GtkCList *clist, gint nColumnNumber, gpointer user_data)
{
	static int nOldColumnNumber=-1;
	static int nOrd;

	if (nColumnNumber!=nOldColumnNumber){
		nOrd=0;
		nOldColumnNumber=nColumnNumber;
	} else {
		nOrd++;
	}
	gtk_clist_set_sort_type(GTK_CLIST(clist),(nOrd%2==0)?GTK_SORT_ASCENDING:GTK_SORT_DESCENDING);
	gtk_clist_set_sort_column(GTK_CLIST(clist), nColumnNumber);
	gtk_clist_sort(GTK_CLIST(clist));
}



/*
* UpdateListFromSelect()
* Aggiorna una riga di una lista GTK da una select SQL
*/
int UpdateListFromSelect(GtkWidget *clist, char *szSqlCmd, char *szKey, int nKeyField)
{
	int nTuples;
	int nFields;
	int nIndex;
	DBFieldType nType;
	int nFieldIndex;
	char *pszText;
	DBresult *DBRes = NULL;
	char szText[256];
	

	/*
	* Eseguo la select specificata
	*/
	DBRes=DBExecQuery(FALSE,szSqlCmd);

	nTuples=DBntuples(DBRes);
	nFields=DBnfields(DBRes);

	if(nTuples==1){
		for(nIndex=0;nIndex<(GTK_CLIST(clist)->rows);nIndex++){
			gtk_clist_get_text(GTK_CLIST(clist),nIndex,nKeyField,&pszText);
			/*
			* cerco la chiave nella posizione specificata 
			*/
			if(!strcmp(szKey,pszText)){
				for(nFieldIndex=0;nFieldIndex<nFields;nFieldIndex++){
					int nLength;

					nType=db_get_field_type(DBftype(DBRes,nFieldIndex));
					switch(nType){
						default:
						case DB_TYPE_STRING:
							nLength=DBgetlength(DBRes,0,nFieldIndex);
							sprintf(szText,"%*s",nLength,DBgetvalue(DBRes,0,nFieldIndex));
						break;
						case DB_TYPE_FLOAT:
							nLength=strlen(DBfname(DBRes,nFieldIndex));
							sprintf(szText,"%*.*f",nLength,3,atof(DBgetvalue(DBRes,0,nFieldIndex)));
						break;
						case DB_TYPE_INTEGER:
							nLength=strlen(DBfname(DBRes,nFieldIndex));
							sprintf(szText,"%*d",nLength,atoi(DBgetvalue(DBRes,0,nFieldIndex)));
						break;
					}

					/*
					* Setto il testo del campo
					*/
					gtk_clist_set_text(GTK_CLIST(clist),nIndex,nFieldIndex,szText);
				}
			}
		}
	}
	
	DBFreeQueryResult(DBRes);

	return nTuples;
}

/*
* CreateListFromSelect()
* Crea una lista GTK da una select SQL
* ritorna il numero di tuple o -1 se errore
*/
int CreateListFromSelect(GtkWidget *super_parent, GtkWidget *parent, GtkWidget **clist, char *szCListName, int nSelectMode, char *szSqlCmd)
{
	int nTuples;
	int nFields;
	int nIndex;
	int Type;
	int SortDataType;
	int Justification;
	int nFieldIndex;
	char *RecordData[MAX_FIELDS_NUMBER];
	DBresult *DBRes = NULL;
	BOOL bOK=TRUE;
	
	/*
	* Eseguo la select specificata
	*/
	DBRes=DBExecQuery(0,szSqlCmd);

	nTuples=DBntuples(DBRes);
	nFields=DBnfields(DBRes);

	if(DBresultStatus(DBRes) == DBRES_TUPLES_OK){
		/*
		* Creazione clist widget 
		*/

		// *clist = gtk_clist_new (nFields);
		*clist = gm_clist_new (nFields);

		gtk_widget_ref (*clist);
		gtk_object_set_data_full (GTK_OBJECT (super_parent), szCListName, *clist, (GtkDestroyNotify) gtk_widget_unref);
		gtk_container_add (GTK_CONTAINER (parent), *clist);

		for (nFieldIndex=0;nFieldIndex<nFields;nFieldIndex++){
			GtkWidget *Label;
			gtk_clist_set_column_auto_resize(GTK_CLIST(*clist), nFieldIndex, TRUE);

			/*
			* impostazione titolo colonna : utilizzo la .._set_column_title per avere la possibilita'
			* di ottenere la larghezza ottimale in fase di visualizzazione
			*/ 
			// gtk_clist_set_column_title(GTK_CLIST(*clist), nFieldIndex, DBfname(DBRes,nFieldIndex));

			Label = gtk_label_new(DBfname(DBRes,nFieldIndex));
			gtk_widget_show(Label);
			gm_clist_set_column_widget(GM_CLIST(*clist), nFieldIndex, Label);

			Type=db_get_field_type(DBftype(DBRes,nFieldIndex));
			switch(Type){
				default:
					SortDataType=GMCLIST_TYPE_NONE;
					Justification=GTK_JUSTIFY_LEFT;
				break;
				case DB_TYPE_STRING:
					SortDataType=GMCLIST_TYPE_STRING;
					Justification=GTK_JUSTIFY_LEFT;
				break;
				case DB_TYPE_FLOAT:
					SortDataType=GMCLIST_TYPE_FLOAT;
					Justification=GTK_JUSTIFY_RIGHT;
				break;
				case DB_TYPE_INTEGER:
					SortDataType=GMCLIST_TYPE_INTEGER;
					Justification=GTK_JUSTIFY_RIGHT;
				break;
				case DB_TYPE_DATETIME:
					SortDataType=GMCLIST_TYPE_DATETIME;
					Justification=GTK_JUSTIFY_LEFT;
				break;
			}
			gtk_clist_set_column_justification(GTK_CLIST(*clist), nFieldIndex, Justification);
			gm_clist_set_column_sortdata_type(GM_CLIST(*clist), nFieldIndex, SortDataType);


		}
		gtk_clist_column_titles_show (GTK_CLIST (*clist));
		gtk_clist_set_selection_mode (GTK_CLIST (*clist),nSelectMode);

		/*
		* Aggiungo le tuple alla lista (una alla volta)
		*/
		for(nIndex=0;nIndex<nTuples;nIndex++){

			for(nFieldIndex=0;nFieldIndex<nFields;nFieldIndex++){

				Type=db_get_field_type(DBftype(DBRes,nFieldIndex));
				switch(Type){
					default:
					case DB_TYPE_STRING:
						RecordData[nFieldIndex]=g_strdup(DBgetvalue(DBRes,nIndex,nFieldIndex));
					break;
					case DB_TYPE_FLOAT:
						RecordData[nFieldIndex]=g_strdup_printf("%.*f",3,g_strtod(DBgetvalue(DBRes,nIndex,nFieldIndex),NULL));
					break;
					case DB_TYPE_INTEGER:
						RecordData[nFieldIndex]=g_strdup(DBgetvalue(DBRes,nIndex,nFieldIndex));
					break;
				}
			}

			gtk_clist_append( (GtkCList *) *clist, RecordData);

			/* 
			* libero la memoria allocata 
			*/
			for(nFieldIndex=0;nFieldIndex<nFields;nFieldIndex++){
				g_free(RecordData[nFieldIndex]);
			}
		}

	} else {
		bOK=FALSE;
	}


#ifdef _ELIMINATO_
	
	/*
	* + st + 04/10/2000
	* Aggiusto la larghezza delle colonne in modo da poter visualizzare completamente i dati contenuti;
	* Per le colonne vuote imposto una larghezza minima che permette di visualizzare completamente il titolo;
	*/
	for(nColumnIndex=0;nColumnIndex<nFields;nColumnIndex++){
		sprintf(szColumnName, gtk_clist_get_column_title(GTK_CLIST(*clist), nColumnIndex));
		nColumnWidth=10*strlen(StrTrimAll(szColumnName));
		gtk_clist_set_column_min_width(GTK_CLIST(*clist), nColumnIndex,	nColumnWidth);
		gtk_clist_set_column_width(GTK_CLIST(*clist), nColumnIndex, gtk_clist_optimal_column_width(GTK_CLIST(*clist),nColumnIndex));
	}
	/*
	* callback per ordinamento clist
	*/
	gtk_signal_connect (GTK_OBJECT (*clist), "click-column", GTK_SIGNAL_FUNC (on_lst_sort_column_clicked), NULL);
#endif
	
	if(bOK){
		gtk_widget_show (*clist);
	}
	DBFreeQueryResult(DBRes);

	return nTuples;
}

int RefreshTable(GtkWidget *parent,char *szScrolledWindowName,char *szCListName,char *szLabelName,char *szCfgFileName,char *szParagraph,char *szTableName,int nSelectionMode,GList *PSList)
{
	char szSelectCmd[4096];
	char szCmd[2048];
	char szBuffer[256];
	char szTitle[256];
	GtkWidget *lst;
	GtkWidget *sw=get_widget(parent,szScrolledWindowName);
	int nTuples=0;

	if(strlen(szParagraph) && strlen(szTableName)){

		GetFileString(szParagraph,szTableName, "", szSelectCmd, sizeof(szSelectCmd),szCfgFileName,NULL); 

		if(PSList){
			ParseStatement(szSelectCmd,PSList);
		}

		strcpy(szCmd,szSelectCmd);

		/*
		* refresh lista a video
		*/
		if((lst=get_widget(sw,szCListName))){
			gtk_widget_destroy(lst);
		}
		nTuples=CreateListFromSelect(parent,sw,&lst,szCListName,nSelectionMode,szSelectCmd);
		/* titolo */
		if(szLabelName && strlen(szLabelName) && get_widget(parent,szLabelName)){
			strcpy(szBuffer,szTableName);
			strcat(szBuffer,"_descr");
			GetFileString(szParagraph,szBuffer, "", szTitle, sizeof(szTitle),szCfgFileName,NULL); 
			gtk_label_printf(get_widget(parent,szLabelName),szTitle);
		}

	}
	return nTuples;
}


/*
* UpdateTable()
* Aggiorna una riga di una lista GTK
*/
int UpdateTable(GtkWidget *parent,char *szCListName,char *szCfgFileName,char *szParagraph,char *szTableName,char *szKey,GList *PSList,BOOL bSetPosition)
{
	BOOL bRecordFound=FALSE;
	int nTuples;
	int nFields;
	int nIndex;
	DBFieldType nType;
	int nKeyIndex;
	int nFieldIndex;
	char szCmd[4096];
	char szSqlTmp[4096];
	char *pszText;
	DBresult *DBRes = NULL;
	char szText[256];
	char szDBValue[256];
	char szBuffer[256];
	GtkCList *lst=GTK_CLIST(get_widget(parent,szCListName));
	
	if(strlen(szParagraph) && strlen(szTableName)){
		strcpy(szBuffer,szTableName);
		strcat(szBuffer,"_key");
		nKeyIndex=GetFileInt(szParagraph,szBuffer, 0,szCfgFileName,NULL); 

		strcpy(szBuffer,szTableName);
		strcat(szBuffer,"_update");
		GetFileString(szParagraph,szBuffer, "", szCmd, sizeof(szSqlTmp),szCfgFileName,NULL); 

		if(PSList){
			ParseStatement(szCmd,PSList);
		}

		/*
		* Eseguo la select specificata
		*/
		DBRes=DBExecQuery(FALSE,szCmd);

		nTuples=DBntuples(DBRes);
		nFields=DBnfields(DBRes);

		if(nTuples){
			for(nIndex=0;nIndex<lst->rows;nIndex++){
				gtk_clist_get_text(lst,nIndex,nKeyIndex,&pszText);
				if(!strcmp(szKey,pszText)){

					/*
					* trovata la riga
					*/
					bRecordFound=TRUE;

					for(nFieldIndex=0;nFieldIndex<nFields;nFieldIndex++){
						int nLength;

						strcpy(szDBValue,DBgetvalue(DBRes,0,nFieldIndex));
				
						nType=db_get_field_type(DBftype(DBRes,nFieldIndex));

						switch(nType){
							default:
							case DB_TYPE_STRING:
								strcpy(szText,szDBValue);
							break;
							case DB_TYPE_FLOAT:
								nLength=strlen(DBfname(DBRes,nFieldIndex));
								sprintf(szText,"%*.*f",nLength,3,g_strtod(szDBValue,NULL));
							break;
							case DB_TYPE_INTEGER:
								nLength=strlen(DBfname(DBRes,nFieldIndex));
								sprintf(szText,"%*d",nLength,atoi(szDBValue));
							break;
						}

						/*
						* Setto il testo del campo
						*/
						gtk_clist_set_text(lst,nIndex,nFieldIndex,szText);
					}
				}
			}
			/* se non trovo aggiungo */
			if(bRecordFound==FALSE){
				char *RecordData[1][MAX_FIELDS_NUMBER];
				int nMallocSize;
				int nRowIndex;
				int nLength;

				for(nFieldIndex=0;nFieldIndex<nFields;nFieldIndex++){
					/*
					* Alloco lo spazio per una tupla
					*/
					nMallocSize=max(100,DBgetlength(DBRes,0,nFieldIndex)+1);
					RecordData[0][nFieldIndex]=malloc(nMallocSize);
					/*
					* aggiungo una riga alla lista GTK
					*/
					nType=db_get_field_type(DBftype(DBRes,nFieldIndex));
					strcpy(szDBValue,DBgetvalue(DBRes,0,nFieldIndex));

					switch(nType){
						default:
						case DB_TYPE_STRING:
							strcpy(RecordData[0][nFieldIndex],szDBValue);
						break;
						case DB_TYPE_FLOAT:
							nLength=strlen(DBfname(DBRes,nFieldIndex));
							sprintf(RecordData[0][nFieldIndex],"%*.*f",nLength,3,g_strtod(szDBValue,NULL));
						break;
						case DB_TYPE_INTEGER:
							nLength=strlen(DBfname(DBRes,nFieldIndex));
							sprintf(RecordData[0][nFieldIndex],"%*d",nLength,atoi(szDBValue));
						break;
					}
				}

				nRowIndex=gtk_clist_append(lst, RecordData[0]);

				if(bSetPosition){
					gtk_clist_moveto(lst, nRowIndex, -1, 0.9, 0);
				}


				/* 
				* libero la memoria allocata 
				*/
				for(nFieldIndex=0;nFieldIndex<nFields;nFieldIndex++){
					free(RecordData[0][nFieldIndex]);
				}
			}
		}
		
		DBFreeQueryResult(DBRes);
	}

	return nTuples;
}


void UpdateVarList(GtkWidget *clist,BOOL bRefresh)
{
	static char *FieldData[1][10];
	static BOOL bFirst=TRUE;
	int nIndex=0;

	if(bFirst){
		FieldData[0][0]=malloc(100);
		FieldData[0][1]=malloc(100);
		FieldData[0][2]=malloc(100);
		FieldData[0][3]=malloc(100);
		bFirst=FALSE;
	}

	if(bRefresh){
		gtk_clist_clear(GTK_CLIST(clist));
		nIndex=0;
		while(strlen(PLCVars[nIndex].szName)){
			strcpy(FieldData[0][0],PLCVars[nIndex].szName);
			strcpy(FieldData[0][1],PLCVars[nIndex].szDescription);
			strcpy(FieldData[0][2],PLCTypes[PLCVars[nIndex].usType].lpszName);
			sprintf(FieldData[0][3],"%ld",PLCVars[nIndex].Value.DWORDValue);

			gtk_clist_append( GTK_CLIST(clist), FieldData[0]);

			/*
			* Aggiorno i valori delle variabili
			*/
			PLCVars[nIndex].OldValue.DWORDValue=PLCVars[nIndex].Value.DWORDValue;

			nIndex++;
		}
	} else {
		nIndex=0;
		while(strlen(PLCVars[nIndex].szName)){
			if(PLCVars[nIndex].Value.DWORDValue!=PLCVars[nIndex].OldValue.DWORDValue){
				if(clist){
					sprintf(FieldData[0][3],"%ld",PLCVars[nIndex].Value.DWORDValue);
					gtk_clist_set_text( GTK_CLIST(clist), nIndex,3,FieldData[0][3]);
				}
				/*
				* Aggiorno i valori delle variabili
				*/
				PLCVars[nIndex].OldValue.DWORDValue=PLCVars[nIndex].Value.DWORDValue;
			}
			nIndex++;
		}
	}
}
