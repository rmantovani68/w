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

#ifdef GTK_CLIST_USED
void on_lst_sort_column_clicked (GtkCList *clist, gint nColumnNumber, gpointer user_data);
gint compare_rows(GtkCList * clist, gconstpointer ptr1, gconstpointer ptr2);
void UpdateVarList(GtkWidget *clist,BOOL bRefresh);
#endif
int UpdateListFromSelect(GtkTreeView *list, char *szSqlCmd,GtkTreeIter *iter);
int InsertListFromSelect(GtkTreeView *list, char *szSqlCmd,GtkTreeIter *iter);
int UpdateInTable(GtkTreeView *list,char *szCfgFileName,char *szParagraph,char *szTableName,GList *PSList,GtkTreeIter *iter);
int InsertInTable(GtkTreeView *list,char *szCfgFileName,char *szParagraph,char *szTableName,GList *PSList,GtkTreeIter *iter);
int RefreshTable(GtkWidget *parent,char *szScrolledWindowName,char *szCListName,char *szLabelName,char *szCfgFileName,char *szParagraph,char *szTableName,int nSelectionMode,GList *PSList);
int UpdateTable(GtkWidget *parent,char *szCListName,char *szCfgFileName,char *szParagraph,char *szTableName,char *szKey,GList *PSList,BOOL bSetPosition);
void selection_changed(GtkTreeSelection *selection, gpointer data);
void UpdateVarList(GtkWidget *super_parent, GtkWidget *parent, GtkWidget **list, char *szListName, int nSelectMode,BOOL bRefresh);
int CreateListFromSelect(GtkWidget *super_parent, GtkWidget *parent, GtkWidget **clist, char *szCListName, int nSelectMode, char *szSqlCmd);
