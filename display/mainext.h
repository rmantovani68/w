/*
* mainext.h
* Dichiarazioni External
* Easy Picking 3.0
* Copyright CNI srl 1999-2000
* Autore : Roberto Mantovani
*/

extern PROCESSO ProcList[NUM_PROC];	/* processi componenti il progetto */
extern PROCESSO DisplayList[MAX_DISPLAYS];
extern CFGSTRUCT Cfg;
//extern DISPLAY_CFGSTRUCT DisplayCfg;
extern PSETTORE pSettori;
extern SETTORE OldSettore;
extern PLINEA_STRUCT pDatiLinea;
extern char szDateBuffer[128];
extern char szTimeBuffer[128];
extern int nOldStatoLinea;
extern int nOldAttesaBadge;
extern int nActualFrame;

/*
* widgets utilizzati nell'applicazione 
*/
/* widgets utilizzati nell'applicazione */
extern GtkWidget *main_window;

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

extern CFGITEM CFGItems[];
//extern CFGITEM CFGDisplayItems[];


#define TREE_VIEW_LST(parent)				((GtkTreeView *)(get_widget(parent,"lst")))
#define TREE_SELECTION_LST(parent)	((GtkTreeSelection *)(gtk_tree_view_get_selection(TREE_VIEW_LST(parent))))
#define TREE_MODEL_LST(parent)			((GtkTreeModel *)(gtk_tree_view_get_model(TREE_VIEW_LST(parent))))
