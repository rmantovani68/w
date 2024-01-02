/*
* mainfun.h
* Dichiarazione Funzioni Utilizzate
* Easy Picking 3.0
* Copyright CNI srl 1999-2000
* Autore : Roberto Mantovani
*/

/*
* mainutil.c
*/
char *GetTime(long time,char *szTimeString);
char *GetTimeFromHHMMSS(char *szFromTime,char *szTimeString);
char *GetTimeHHMMSS(long time,char *szTimeString);
char *GetTimeHHMM(long time,char *szTimeString);
char *GetDate(long time,char *szDateString);
char *GetDateYYYYMMDD(long time,char *szDateString);
char *GetDateYYMMDD(long time,char *szDateString);
char *GetDatef(char *szDateFormat);
char *GetDateDDMM(long time,char *szDateString);
char *GetDateDDMMYYYY(long time,char *szDateString);
char *GetDateFromYYYYMMDD(char *szFromDate,char *szDateString);
char *GetDateYYMMFromDDMMYY(char *szFromDate,char *szDateString);
char *GetDateYYYYMMDDFromDDMMYY(char *szFromDate,char *szDateString);
char *GetDateFromDDMMYY(char *szFromDate,char *szDateString);
char *DeleteNewLine(char *str);
BOOL FileExists(char *szFileName);
BOOL FileNotEmpty(char *szFileName);
char *LeftStr(char *szString,int nLen);
char *SubStr(char *szString,int nPos,int nLen);
char *StrTrimAll(char *str);
char *StrTrimRight(char *str);
char *StrTrimLeft(char *str);
char *ElimNewLine(char *str);
char *RightPad(char *szString,int nLen);
char *PadZero(char *szString,int nLen);
char *NormalizeString(char *szString);
BOOL SendMessage(int nDest,int nSrce,int nMsgCode,char *szText);
BOOL ConnectDataBase(PGconn **pDBConn);
PGresult *PGExecSQL (BOOL bTrace,char *format, ...);
BOOL ReadTableInfo(PDBSTRUCT pDB,char *szCfgBaseName);
BOOL ReadTablesInfo(void);
int ContaRighe(char *szFileName);
BOOL IsAsciiRecord(char *szBuffer,PDBSTRUCT pDB);
BOOL GetAsciiRecord(char *szBuffer,PDBSTRUCT pDB);
BOOL UpdateAsciiRecord(char *szBuffer, PDBSTRUCT pDB, BOOL bUpdate);
BOOL WriteAsciiRecord(FILE *fp,PDBSTRUCT pDB,PGresult *PGRes,int nIndex);
//void gtk_label_printf(GtkWidget *w,...);
void gtk_label_printf(GtkWidget *w,char *fmt,...);
//void gtk_text_printf(GdkColor *Color,GtkWidget *w,...);
void gtk_text_printf(char *szColor,GtkWidget *w,char *fmt,...);
void gtk_text_read_file(GtkWidget *w,char *szFileName);
void gtk_update(void);
void ChangeTextColor(GtkWidget *w,GdkColor *Color);
unsigned long CryptString(char *pszString);
BOOL RunSimpleProcess(int nProcID);
void TerminateProcess(int nProcID);

/*
* mainfun.c
*/
void gtk_error_handler(gchar *str);
void gtk_warning_handler(gchar *str);
void gtk_message_handler(gchar *str);
void ReadConfiguration(BOOL bReadProcInfo);
void ReadProcInfo(char *szFileConfig);
int ReadPrinters(char *szCfgFile);
void ExitApp();
void RefreshListaOrdini(GtkWidget *parent,char *szSelect);
void RefreshDatiIsola(int nIsola,GtkWidget *clist);
void RefreshElaborazioni(GtkWidget *lst,int nIsola);
void RefreshOperatoriSettori(void);
int GetOrdineIndex(char *pszOrdine);
int GetNumColliOrdProd(char *szOrdKey);
int GetAmazonFlagFromRicOrd(char *szOrdKey);
char GetPdfFlagFromOrdProd(char *szOrdKey);
char GetPdfFlagFromRicOrd(char *szOrdine);
char GetFtzFlagFromOrdProd(char *szOrdKey);
char GetFtzFlagFromRicOrd(char *szOrdine);
void EditOrdine(char *szOrdineKey);
BOOL EditCollo(char *szOrdineKey,int nNumCollo);
void ListaDistribuzione(void);
char *GetStatoOrdine(char cStato,char *pszBuffer);
char *GetStatoCollo(char cStato,char *pszBuffer);
void UpdateOrdine(char *szOrdineKey);
int UpdateListFromSelect(GtkWidget *clist, char *szSqlCmd, char *szKey, int nKeyField);
int CreateListFromSelect(GtkWidget *super_parent, GtkWidget *parent, GtkWidget **clist, char *szCListName, int nSelectMode, char *szSqlCmd, char *szOrdProg, int nNumCollo);
void RicezioneCedola(void);
void RicezioneOrdini(void);
void RicezioneArticoli(void);
void RicezioneUbicazioni(void);
void SpedizioneCedola(char *szAperturaFile);
BOOL LanciaOrdine(char *pszOrdineKey,char *szStato);
void StartLinea(int nIsola);
void StopLinea(int nIsola);
void EditConfigurazione(void);
void ApplyConfigurazione(void);
BOOL CambiaStatoOrdine(char *szOrdProg, char cStato);
BOOL StampaStatistica(char *szFromDate,char *szToDate,char *szFileName);
BOOL CheckUbicazioni(void);
BOOL SetPriority(char *pszOrdineKey,int nPrio);
void RefreshListaAnagOperatori(void);
void RefreshListaProdotti(GtkWidget *dlg);
void RefreshListaProdottiPericolosi(GtkWidget *dlg);
void CheckStampaOrdineEvaso(char *pszOrdine);
BOOL ScartaOrdine(char *pszOrdineKey,char *pszStato);
BOOL InviaOrdine(char *pszOrdineKey,FILE *fp);
BOOL InviaColliOrdine(char *pszOrdineKey,FILE *fp);
BOOL InviaDettaglioColliOrdine(char *pszOrdineKey,FILE *fp);
void StampaCompletaOrdine(int nIsola, char *szElaborazione,char *szOrdKey);
BOOL SetErroreRiga(char *szOrdkey,int nCollo,char *szCodProd, char *szCDERR);
BOOL ResetErroreRiga(char *szOrdkey,int nCollo,char *szCodProd);
BOOL OrdineBertelloConOrdiniDiLinea(char *szOrdProg);
BOOL StampaOrdiniBertelloSenzaOrdinidiLinea(void);

/*
* mainproc.c
*/
gint RefreshDataTimer( gpointer data );
gint SaveStateTimer( gpointer data );
gint ProcessMsgs( gpointer data );

/*
* main.c
*/
int main (int argc, char *argv[]);
