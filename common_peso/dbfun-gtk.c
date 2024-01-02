/*
* modulo : dbfun-gtk.c
* --------------------
* Progetto Easy Picking - DB Management - PostgreSQL - GTK
*
* Data creazione 18/06/2002
*
* Autore : Roberto Mantovani
*
* Copyright A&L srl 2002
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

#include <gnome.h>
#include <gtk/gtk.h>
#include <glib.h>

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
static char *latin1_to_utf8 (const char *text)
{
	GString *str;
	const char *p;

	str = g_string_new ("");

	p = text;
	while (*p) {
		g_string_append_unichar (str, *p);
		++p;
	}

	return g_string_free (str, FALSE);
}
*/


void float_data_func(GtkTreeViewColumn *col, GtkCellRenderer   *renderer, GtkTreeModel      *model, GtkTreeIter       *iter, gpointer  user_data)
{
	gfloat  float_value;
	gchar   buf[20];

	gtk_tree_model_get(model, iter, (int)user_data, &float_value, -1);

	g_snprintf(buf, sizeof(buf), "%.2f", float_value);

	g_object_set(renderer, "text", buf, NULL);
}


/*
* CreateListFromSelect()
* Crea una lista GTK TREE VIEW da una select SQL
* ritorna il numero di tuple o -1 se errore
*/
int CreateListFromSelect(GtkWidget *super_parent, GtkWidget *parent, GtkWidget **list, char *szListName, int nSelectMode, char *szSqlCmd)
{
	int nTuples;
	int nFields;
	int nIndex;
	int Type;
	int nFieldIndex;
	DBresult *DBRes = NULL;
	BOOL bOK=TRUE;
	GtkListStore *store;
	GtkTreeIter iter;
	GtkCellRenderer *renderer;
	GtkWidget *lst;
	GType pFieldTypes[MAX_FIELDS_NUMBER];
	GtkTreeSelection *selection;
	char szFieldValue[256];
	char szFieldName[256];
	GtkWidget *ab_main;


	ab_main=get_widget(super_parent,"ab_main");

	if((lst=get_widget(super_parent,szListName))){
		gtk_widget_destroy(lst);
	}

	/*
	* Eseguo la select specificata
	*/
	DBRes=DBExecQuery(0,szSqlCmd);
	nTuples=DBntuples(DBRes);
	nFields=DBnfields(DBRes);

	if(DBresultStatus(DBRes) == DBRES_TUPLES_OK){

		for(nIndex=0;nIndex<nFields;nIndex++){
			Type=db_get_field_type(DBftype(DBRes,nIndex));
			switch(Type){
				default: pFieldTypes[nIndex]=G_TYPE_STRING; break;
				case DB_TYPE_STRING: pFieldTypes[nIndex]=G_TYPE_STRING; break;
				case DB_TYPE_FLOAT: pFieldTypes[nIndex]=G_TYPE_FLOAT; break;
				case DB_TYPE_INTEGER: pFieldTypes[nIndex]=G_TYPE_INT; break;
				case DB_TYPE_DATETIME: pFieldTypes[nIndex]=G_TYPE_STRING; break;
			}
		}
		/* aggiungo colonna colore */
		pFieldTypes[nFields]=G_TYPE_STRING;

		store = gtk_list_store_newv (nFields+1, pFieldTypes);

		for(nIndex=0;nIndex<nTuples;nIndex++){
			gtk_list_store_append (store, &iter);  


			for (nFieldIndex=0;nFieldIndex<nFields;nFieldIndex++){
				strcpy(szFieldValue,DBgetvalue(DBRes,nIndex,nFieldIndex));
				switch(pFieldTypes[nFieldIndex]){
					case G_TYPE_STRING:  gtk_list_store_set(store, &iter, nFieldIndex, g_utf8_normalize(szFieldValue,-1,G_NORMALIZE_DEFAULT) ,-1);break;
					case G_TYPE_FLOAT:   gtk_list_store_set(store, &iter, nFieldIndex, atof(szFieldValue),-1); break;
					case G_TYPE_INT:     gtk_list_store_set(store, &iter, nFieldIndex, atoi(szFieldValue),-1); break;
				}
			}
			/* colore */
			gtk_list_store_set(store, &iter, nFields, NULL,-1);

#ifdef PROVA
			if(ab_main) {
				gnome_appbar_set_progress_percentage ((GnomeAppBar *)ab_main, (float)nIndex/(float)nTuples);
				gtk_update();
			}
#endif /* PROVA */
		}
		*list = gtk_tree_view_new_with_model (GTK_TREE_MODEL (store));
		g_object_unref (G_OBJECT (store));

		selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (*list));

		g_signal_connect(G_OBJECT(selection), "changed", G_CALLBACK(selection_changed), *list);

		gtk_tree_selection_set_mode (selection, nSelectMode);

		gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (*list), TRUE);
		

		gtk_tree_view_set_enable_search (GTK_TREE_VIEW(*list), TRUE);


		for (nFieldIndex=0;nFieldIndex<nFields;nFieldIndex++){
			/* allineamento a destra per i numeri */
			renderer = gtk_cell_renderer_text_new ();
			switch(pFieldTypes[nFieldIndex]){
				case G_TYPE_FLOAT:   g_object_set (renderer, "xalign",1.0,NULL) ; break;
				case G_TYPE_INT:     g_object_set (renderer, "xalign",1.0,NULL) ; break;
				default:     g_object_set (renderer, "xalign",0.0,NULL) ; break;
			}
			strcpy(szFieldName,DBfname(DBRes,nFieldIndex));
			gtk_tree_view_append_column (GTK_TREE_VIEW (*list), gtk_tree_view_column_new_with_attributes( g_utf8_normalize(szFieldName,-1,G_NORMALIZE_DEFAULT) ,renderer,"text", nFieldIndex, "foreground",nFields,NULL));
			gtk_tree_view_column_set_sort_column_id (gtk_tree_view_get_column (GTK_TREE_VIEW (*list), nFieldIndex), nFieldIndex);

			switch(pFieldTypes[nFieldIndex]){
				case G_TYPE_FLOAT:   
					/* numero di cifre decimali (2) nei float */
					gtk_tree_view_column_set_cell_data_func (gtk_tree_view_get_column (GTK_TREE_VIEW (*list), nFieldIndex), renderer, float_data_func, (gpointer)(nFieldIndex), NULL);
				break;
			}
		}

		/* associo il nome al wisget */
		EP_HOOKUP_OBJECT (super_parent, *list, szListName);

		gtk_widget_show (*list);
		gtk_container_add (GTK_CONTAINER (parent), *list);

	} else {
		bOK=FALSE;
	}


	DBFreeQueryResult(DBRes);

	if(ab_main) {
		gnome_appbar_set_progress_percentage ((GnomeAppBar *)ab_main, (float)0);
		gtk_update();
	}

	return bOK?nTuples:-1;
}

/*
* UpdateListFromSelect()
* Aggiorna un elemento in una lista GTK TREE VIEW da una select SQL
* ritorna il numero di tuple (1) o -1 se errore
*/
int UpdateListFromSelect(GtkTreeView *list, char *szSqlCmd,GtkTreeIter *iter)
{
	int nTuples;
	int nFields;
	int nIndex;
	int Type;
	int nFieldIndex;
	DBresult *DBRes = NULL;
	BOOL bOK=TRUE;
	GtkListStore *store;
	GType pFieldTypes[MAX_FIELDS_NUMBER];
	char szFieldValue[256];

	/*
	* Eseguo la select specificata
	*/
	DBRes=DBExecQuery(0,szSqlCmd);
	nTuples=DBntuples(DBRes);
	nFields=DBnfields(DBRes);

	/* controllo che sia presenta una sola riga */ 
	if(DBresultStatus(DBRes) == DBRES_TUPLES_OK && nTuples==1){

		for(nIndex=0;nIndex<nFields;nIndex++){
			Type=db_get_field_type(DBftype(DBRes,nIndex));
			switch(Type){
				default: pFieldTypes[nIndex]=G_TYPE_STRING; break;
				case DB_TYPE_STRING: pFieldTypes[nIndex]=G_TYPE_STRING; break;
				case DB_TYPE_FLOAT: pFieldTypes[nIndex]=G_TYPE_FLOAT; break;
				case DB_TYPE_INTEGER: pFieldTypes[nIndex]=G_TYPE_INT; break;
				case DB_TYPE_DATETIME: pFieldTypes[nIndex]=G_TYPE_STRING; break;
			}
		}
		/* aggiungo colonna colore */
		pFieldTypes[nFields]=G_TYPE_STRING;

		store = (GtkListStore *)gtk_tree_view_get_model(list);

		for(nIndex=0;nIndex<nTuples;nIndex++){
			for (nFieldIndex=0;nFieldIndex<nFields;nFieldIndex++){
				strcpy(szFieldValue,DBgetvalue(DBRes,nIndex,nFieldIndex));
				switch(pFieldTypes[nFieldIndex]){
					case G_TYPE_STRING:  gtk_list_store_set(store, iter, nFieldIndex, g_utf8_normalize(szFieldValue,-1,G_NORMALIZE_DEFAULT) ,-1);break;
					case G_TYPE_FLOAT:   gtk_list_store_set(store, iter, nFieldIndex, atof(szFieldValue),-1); break;
					case G_TYPE_INT:     gtk_list_store_set(store, iter, nFieldIndex, atoi(szFieldValue),-1); break;
				}
			}
		}
	} else {
		bOK=FALSE;
	}


	DBFreeQueryResult(DBRes);

	return bOK?nTuples:-1;
}

/*
* InsertListFromSelect()
* Inserisce un elemento in una lista GTK TREE VIEW da una select SQL
* ritorna il numero di tuple (1) o -1 se errore
*/
int InsertListFromSelect(GtkTreeView *list, char *szSqlCmd,GtkTreeIter *iter)
{
	int nTuples;
	int nFields;
	int nIndex;
	int Type;
	int nFieldIndex;
	DBresult *DBRes = NULL;
	BOOL bOK=TRUE;
	GtkListStore *store;
	GType pFieldTypes[MAX_FIELDS_NUMBER];
	char szFieldValue[256];


	/*
	* Eseguo la select specificata
	*/
	DBRes=DBExecQuery(0,szSqlCmd);
	nTuples=DBntuples(DBRes);
	nFields=DBnfields(DBRes);

	if(DBresultStatus(DBRes) == DBRES_TUPLES_OK){

		for(nIndex=0;nIndex<nFields;nIndex++){
			Type=db_get_field_type(DBftype(DBRes,nIndex));
			switch(Type){
				default: pFieldTypes[nIndex]=G_TYPE_STRING; break;
				case DB_TYPE_STRING: pFieldTypes[nIndex]=G_TYPE_STRING; break;
				case DB_TYPE_FLOAT: pFieldTypes[nIndex]=G_TYPE_FLOAT; break;
				case DB_TYPE_INTEGER: pFieldTypes[nIndex]=G_TYPE_INT; break;
				case DB_TYPE_DATETIME: pFieldTypes[nIndex]=G_TYPE_STRING; break;
			}
		}
		/* aggiungo colonna colore */
		pFieldTypes[nFields]=G_TYPE_STRING;

		store = (GtkListStore *)gtk_tree_view_get_model(list);

		for(nIndex=0;nIndex<nTuples;nIndex++){
			gtk_list_store_append (store, iter);  

			for (nFieldIndex=0;nFieldIndex<nFields;nFieldIndex++){
				strcpy(szFieldValue,DBgetvalue(DBRes,nIndex,nFieldIndex));
				switch(pFieldTypes[nFieldIndex]){
					case G_TYPE_STRING:  gtk_list_store_set(store, iter, nFieldIndex, g_utf8_normalize(szFieldValue,-1,G_NORMALIZE_DEFAULT) ,-1);break;
					case G_TYPE_FLOAT:   gtk_list_store_set(store, iter, nFieldIndex, atof(szFieldValue),-1); break;
					case G_TYPE_INT:     gtk_list_store_set(store, iter, nFieldIndex, atoi(szFieldValue),-1); break;
				}
			}
			/* colore */
			gtk_list_store_set(store, iter, nFields, NULL,-1);

		}
	} else {
		bOK=FALSE;
	}


	DBFreeQueryResult(DBRes);

	return bOK?nTuples:-1;
}

/*
* UpdateTable()
* Aggiorna una riga di una lista GTK TREEVIEW_VERSION
*/
int UpdateTable(GtkWidget *parent,char *szListName,char *szCfgFileName,char *szParagraph,char *szTableName,char *szKey,GList *PSList,BOOL bSetPosition)
{
	GtkListStore *store;
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
	char szDBValue[256];
	char szBuffer[256];
	GtkTreeView *tree_view=(GtkTreeView *)get_widget(parent,szListName);
	GtkTreeModel *model=gtk_tree_view_get_model(tree_view);
	GtkTreeIter iter;
	GType pFieldTypes[MAX_FIELDS_NUMBER];
	
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
			for(nIndex=0;nIndex<nFields;nIndex++){
				nType=db_get_field_type(DBftype(DBRes,nIndex));
				switch(nType){
					default: pFieldTypes[nIndex]=G_TYPE_STRING; break;
					case DB_TYPE_STRING: pFieldTypes[nIndex]=G_TYPE_STRING; break;
					case DB_TYPE_FLOAT: pFieldTypes[nIndex]=G_TYPE_FLOAT; break;
					case DB_TYPE_INTEGER: pFieldTypes[nIndex]=G_TYPE_INT; break;
					case DB_TYPE_DATETIME: pFieldTypes[nIndex]=G_TYPE_STRING; break;
				}
			}
			/* aggiungo colonna colore */
			pFieldTypes[nFields]=G_TYPE_STRING;

			if(gtk_tree_model_get_iter_first(model, &iter)){
				do {
					gtk_tree_model_get_text(model,&iter,nKeyIndex,&pszText);
					if(!strcmp(szKey,pszText)){
						/*
						* trovata la riga
						*/
						bRecordFound=TRUE;

						for(nFieldIndex=0;nFieldIndex<nFields;nFieldIndex++){

							strcpy(szDBValue,DBgetvalue(DBRes,0,nFieldIndex));
					
							switch(pFieldTypes[nFieldIndex]){
								case G_TYPE_STRING: gtk_list_store_set((GtkListStore *)model, &iter, nFieldIndex, g_utf8_normalize(szDBValue,-1,G_NORMALIZE_DEFAULT) ,-1); break;
								case G_TYPE_FLOAT:  gtk_list_store_set((GtkListStore *)model, &iter, nFieldIndex, atof(szDBValue),-1);break;
								case G_TYPE_INT:    gtk_list_store_set((GtkListStore *)model, &iter, nFieldIndex, atoi(szDBValue),-1); break;
							}
						}
						break;
					}
					g_free(pszText);
				} while( gtk_tree_model_iter_next(model, &iter));
			}
			/* se non trovo aggiungo */
			if(bRecordFound==FALSE){
				store = gtk_list_store_newv (nFields+1, pFieldTypes);
				gtk_list_store_append (store, &iter);  

				for(nFieldIndex=0;nFieldIndex<nFields;nFieldIndex++){

					strcpy(szDBValue,DBgetvalue(DBRes,0,nFieldIndex));
			
					switch(pFieldTypes[nFieldIndex]){
						case G_TYPE_STRING: gtk_list_store_set((GtkListStore *)model, &iter, nFieldIndex, g_utf8_normalize(szDBValue,-1,G_NORMALIZE_DEFAULT) ,-1); break;
						case G_TYPE_FLOAT:  gtk_list_store_set((GtkListStore *)model, &iter, nFieldIndex, atof(szDBValue),-1);break;
						case G_TYPE_INT:    gtk_list_store_set((GtkListStore *)model, &iter, nFieldIndex, atoi(szDBValue),-1); break;
					}
				}
				/* background */
				gtk_list_store_set(store, &iter, nFields, NULL,-1);

				if(bSetPosition){
					gtk_tree_view_scroll_to_cell(tree_view, gtk_tree_model_get_path(model,&iter), NULL, FALSE, 0, 0);
				}
			}
		}
		
		DBFreeQueryResult(DBRes);
	}

	return nTuples;
}

int UpdateInTable(GtkTreeView *list,char *szCfgFileName,char *szParagraph,char *szTableName,GList *PSList,GtkTreeIter *iter)
{
	char szSelectCmd[4096];
	char szCmd[2048];
	int nRC=-1;


	if(strlen(szParagraph) && strlen(szTableName)){

		GetFileString(szParagraph,szTableName, "", szSelectCmd, sizeof(szSelectCmd),szCfgFileName,NULL); 

		if(PSList){
			ParseStatement(szSelectCmd,PSList);
		}

		strcpy(szCmd,szSelectCmd);

		/*
		* refresh lista a video
		*/
		nRC=UpdateListFromSelect(GTK_TREE_VIEW(list),szSelectCmd,iter);
	}
	return nRC;
}

int InsertInTable(GtkTreeView *list,char *szCfgFileName,char *szParagraph,char *szTableName,GList *PSList,GtkTreeIter *iter)
{
	char szSelectCmd[4096];
	char szCmd[2048];
	int nRC=-1;


	if(strlen(szParagraph) && strlen(szTableName)){

		GetFileString(szParagraph,szTableName, "", szSelectCmd, sizeof(szSelectCmd),szCfgFileName,NULL); 

		if(PSList){
			ParseStatement(szSelectCmd,PSList);
		}

		strcpy(szCmd,szSelectCmd);

		/*
		* refresh lista a video
		*/
		nRC=InsertListFromSelect(GTK_TREE_VIEW(list),szSelectCmd,iter);
	}
	return nRC;
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

void UpdateVarList(GtkWidget *super_parent, GtkWidget *parent, GtkWidget **list, char *szListName, int nSelectMode,BOOL bRefresh)
{
	GtkWidget *lst;
	int nIndex=0;
	int nTreeViewIndex=0;
	GtkListStore *store;
	GtkTreeIter iter;
	GtkCellRenderer *renderer;
	GType pFieldTypes[MAX_FIELDS_NUMBER];
	GtkTreeSelection *selection;

	if(bRefresh){
		if(super_parent){
			if((lst=get_widget(super_parent,szListName))){
				gtk_widget_destroy(lst);
			}
		}
	} else {
		if(super_parent){
			*list=get_widget(super_parent,szListName);
		}
	}

	if(super_parent){
		pFieldTypes[0]=G_TYPE_STRING;
		pFieldTypes[1]=G_TYPE_STRING;
		pFieldTypes[2]=G_TYPE_STRING;
		pFieldTypes[3]=G_TYPE_INT;
		/* aggiungo colonna colore */
		pFieldTypes[4]=G_TYPE_STRING;

		store = gtk_list_store_newv (5, pFieldTypes);
	}

	if(bRefresh){
		nIndex=0;
		while(strlen(PLCVars[nIndex].szName)){
			if(super_parent){
				gtk_list_store_append (store, &iter);  

				gtk_list_store_set(store, &iter, 0, g_utf8_normalize(PLCVars[nIndex].szName,-1,G_NORMALIZE_DEFAULT) ,-1); 
				gtk_list_store_set(store, &iter, 1, g_utf8_normalize(PLCVars[nIndex].szDescription,-1,G_NORMALIZE_DEFAULT) ,-1); 
				gtk_list_store_set(store, &iter, 2, g_utf8_normalize(PLCTypes[PLCVars[nIndex].usType].lpszName,-1,G_NORMALIZE_DEFAULT) ,-1); 
				gtk_list_store_set(store, &iter, 3, PLCVars[nIndex].Value.DWORDValue,-1); 
				/* background */
				gtk_list_store_set(store, &iter, 4, NULL,-1);
			}

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
				if(super_parent){
					/* cerco la variabile in oggetto */
					if(gtk_tree_model_get_iter_first(gtk_tree_view_get_model(GTK_TREE_VIEW(*list)) ,&iter)){
						nTreeViewIndex=0;
						do {
							if(nTreeViewIndex==nIndex){
								/* setto il valore alla riga indicata */
								gtk_list_store_set((GtkListStore *)gtk_tree_view_get_model(GTK_TREE_VIEW(*list)), &iter, 3, PLCVars[nIndex].Value.DWORDValue,-1); 
							}
							nTreeViewIndex++;
						} while(gtk_tree_model_iter_next(gtk_tree_view_get_model(GTK_TREE_VIEW(*list)) ,&iter));
					}
				}
				/*
				* Aggiorno i valori delle variabili
				*/
				PLCVars[nIndex].OldValue.DWORDValue=PLCVars[nIndex].Value.DWORDValue;
			}
			nIndex++;
		}
	}
	if(bRefresh){
		if(super_parent){

			*list = gtk_tree_view_new_with_model (GTK_TREE_MODEL (store));
			g_object_unref (G_OBJECT (store));

			selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (*list));

			gtk_tree_selection_set_mode (selection, nSelectMode);

			gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (*list), TRUE);


			/* nome */
			renderer = gtk_cell_renderer_text_new ();
			g_object_set (renderer, "xalign",0.0,NULL) ;
			gtk_tree_view_append_column (GTK_TREE_VIEW (*list), gtk_tree_view_column_new_with_attributes( g_utf8_normalize("VARIABILE",-1,G_NORMALIZE_DEFAULT) ,renderer,"text", 0, "foreground",4,NULL));

			/* descrizione */
			renderer = gtk_cell_renderer_text_new ();
			g_object_set (renderer, "xalign",0.0,NULL) ;
			gtk_tree_view_append_column (GTK_TREE_VIEW (*list), gtk_tree_view_column_new_with_attributes( g_utf8_normalize("DESCRIZIONE",-1,G_NORMALIZE_DEFAULT) ,renderer,"text", 1, "foreground",4,NULL));

			/* tipo */
			renderer = gtk_cell_renderer_text_new ();
			g_object_set (renderer, "xalign",0.0,NULL) ;
			gtk_tree_view_append_column (GTK_TREE_VIEW (*list), gtk_tree_view_column_new_with_attributes( g_utf8_normalize("TIPO",-1,G_NORMALIZE_DEFAULT) ,renderer,"text", 2, "foreground",4,NULL));

			/* valore */
			renderer = gtk_cell_renderer_text_new ();
			g_object_set (renderer, "xalign",1.0,NULL) ;
			gtk_tree_view_append_column (GTK_TREE_VIEW (*list), gtk_tree_view_column_new_with_attributes( g_utf8_normalize("VALORE",-1,G_NORMALIZE_DEFAULT) ,renderer,"text", 3, "foreground",4,NULL));

			/* associo il nome al wisget */
			EP_HOOKUP_OBJECT (super_parent, *list, szListName);

			gtk_widget_show (*list);
			gtk_container_add (GTK_CONTAINER (parent), *list);
		}
	}
}


void selection_changed(GtkTreeSelection *selection, gpointer data)
{
	GtkTreeModel *model;
	GtkWidget *w=GTK_WIDGET(data);
	GtkWidget *ab_main;
	GtkTreeView *tree_view;
	GList *lista;
	char szBuffer[128];
#ifdef TREE_VIEW_TEST
	GList *lst;
	GtkTreePath *path;
	GtkTreeIter iter;
	GType type;
	char szValore[128];
	int nValue;
	float fValue;
	char *pszValue;
	GValue *value;
#endif

	ab_main=get_widget(w,"ab_main");

	lista=gtk_tree_selection_get_selected_rows(selection, NULL);
	tree_view=gtk_tree_selection_get_tree_view(selection);
	model=gtk_tree_view_get_model(tree_view);

	if(ab_main){
		gnome_appbar_pop((GnomeAppBar *)ab_main);
		if(g_list_length(lista)){
			sprintf(szBuffer,"Selezionate %d righe",g_list_length(lista));
			gnome_appbar_push((GnomeAppBar *)ab_main, szBuffer);
		}
	}
#ifdef TREE_VIEW_TEST
	if((lst=g_list_first(lista))){
		do{
			path=(GtkTreePath *)(lst->data);

			if(gtk_tree_model_get_iter(model, &iter, path)){
				/* ho ottenuto l'iter */
				type=gtk_tree_model_get_column_type(model, 0);

				switch(type){
					case G_TYPE_STRING: gtk_tree_model_get(model,&iter,0,&pszValue,-1);strcpy(szValore,pszValue);g_free(pszValue);break;
					case G_TYPE_FLOAT:  gtk_tree_model_get(model,&iter,0,&fValue,-1);sprintf(szValore,"%.2f",fValue);break;
					case G_TYPE_INT:    gtk_tree_model_get(model,&iter,0,&nValue,-1);sprintf(szValore,"%d",nValue);break;
				}
				printf("Valore = %s\n",szValore);
			}
		} while((lst=g_list_next(lst)));
	}
#endif
}

