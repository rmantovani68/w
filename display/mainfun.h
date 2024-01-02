/*
* mainfun.h
* Dichiarazione Funzioni Utilizzate
* Easy Picking 3.0
* Copyright CNI srl 1999-2000
* Autore : Roberto Mantovani
*/

/*
* mainfun.c
*/
void gtk_error_handler(gchar *str);
void gtk_warning_handler(gchar *str);
void gtk_message_handler(gchar *str);
BOOL ReadConfiguration(char *szCfgFileName,BOOL bReadProcInfo);
BOOL ReadDisplayConfiguration(char *szCfgFileName);
void ExitApp();
void RefreshAsDisconnected(void);
void RefreshSettoreElements(void);
void RefreshLineaElements(void);
void RefreshStaticElements(void);
void RefreshDatiProdottoElements(void);
void RefreshStatisticheElements(void);
void RefreshDisplayAsVoid(void);
BOOL connect_linea_shm(PLINEA_STRUCT *pDatiLinea);
BOOL connect_settori_shm(PSETTORE *pSettori);
BOOL disconnect_linea_shm(PLINEA_STRUCT *pDatiLinea);
BOOL disconnect_settori_shm(PSETTORE *pSettori);
gint Blinking( gpointer data );
int GetSettore(int nIsola,int nSettore);

/*
* mainproc.c
*/
gint ProcessMsgs( gpointer data );

/*
* main.c
*/
int main (int argc, char *argv[]);
