/*
* modulo : dbfun-gtk.h
* --------------------
* Progetto Easy Picking - DB Management - PostgreSQL - GTK
*
* Data creazione 18/06/2002
*
* Autore : Roberto Mantovani
*
* Copyright HSD SpA - CNI 1995-2002
*
* Modifiche
* -------------   -------------------------------------------------------
* rm 18-06-2002 : Creazione
* -------------   -------------------------------------------------------
*/


void on_lst_sort_column_clicked (GtkCList *clist, gint nColumnNumber, gpointer user_data);
int CreateListFromSelect(GtkWidget *super_parent, GtkWidget *parent, GtkWidget **clist, char *szCListName, int nSelectMode, char *szSqlCmd);
int RefreshTable(GtkWidget *parent,char *szScrolledWindowName,char *szCListName,char *szLabelName,char *szCfgFileName,char *szParagraph,char *szTableName,int nSelectionMode,GList *PSList);
int UpdateListFromSelect(GtkWidget *clist, char *szSqlCmd, char *szKey, int nKeyField);
int UpdateTable(GtkWidget *parent,char *szCListName,char *szCfgFileName,char *szParagraph,char *szTableName,char *szKey,GList *PSList,BOOL bSetPosition);
void UpdateVarList(GtkWidget *clist,BOOL bRefresh);


