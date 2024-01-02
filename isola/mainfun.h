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
char *LeftStr(char *szString,int nLen);
char *SubStr(char *szString,int nPos,int nLen);
char *StrTrimAll(char *str);
char *StrTrimRight(char *str);
char *StrTrimLeft(char *str);
char *ElimNewLine(char *str);
char *RightPad(char *szString,int nLen);
char *PadZero(char *szString,int nLen);
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
void gtk_label_printf(GtkWidget *w,...);
void gtk_text_printf(GdkColor *Color,GtkWidget *w,...);
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
void ExitApp();
void RefreshListaOrdini(GtkWidget *parent,char *szSelect);
int GetOrdineIndex(char *pszOrdine);
void EditOrdine(char *szOrdineKey);
BOOL EditCollo(char *szOrdineKey,int nNumCollo);
void ListaDistribuzione(void);
char *GetStatoOrdine(char cStato,char *pszBuffer);
char *GetStatoCollo(char cStato,char *pszBuffer);
void UpdateOrdine(char *szOrdineKey);
int UpdateListFromSelect(GtkWidget *clist, char *szSqlCmd, char *szKey, int nKeyField);
int CreateListFromSelect(GtkWidget *super_parent, GtkWidget *parent, GtkWidget **clist, char *szCListName, int nSelectMode, char *szSqlCmd, char *szOrdProg, int nNumCollo);
void EditConfigurazione(void);
void ApplyConfigurazione(void);
BOOL SendSockCmd(int nServicePort,char *szHostName, int nCmd,char *szParams);

/*
* mainproc.c
*/
gint RefreshTimeOut( gpointer data );
gint ProcessMsgs( gpointer data );

/*
* main.c
*/
int main (int argc, char *argv[]);
