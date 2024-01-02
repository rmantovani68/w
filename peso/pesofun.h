/*
* pesofun.h
* Dichiarazione Funzioni Utilizzate
* Gestione Peso/Fine Linea
* Easy Picking 3.0
* Copyright CNI srl 1999-2000
* Autore : Roberto Mantovani
*/

/*
* pesoutil.c
*/
char *GetTime(long time,char *szTimeString);
char *GetTimeFromHHMMSS(char *szFromTime,char *szTimeString);
char *GetTimeHHMMSS(long time,char *szTimeString);
char *GetTimeHHMMSSFromDB(char *szTimeString);
char *GetTimeHHMM(long time,char *szTimeString);
char *GetDate(long time,char *szDateString);
char *GetDateYYYYMMDD(long time,char *szDateString);
char *GetDateYYMMDD(long time,char *szDateString);
char *GetDatef(char *szDateFormat);
char *GetDateDDMM(long time,char *szDateString);
char *GetDateDDMMYYYY(long time,char *szDateString);
char *GetDateFromYYYYMMDD(char *szFromDate,char *szDateString);
char *GetDateYYYYMMDDFromDB(char *szTimeString);
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
* pesofun.c
*/
void gtk_error_handler(gchar *str);
void gtk_warning_handler(gchar *str);
void gtk_message_handler(gchar *str);
void ReadConfiguration(void);
void ReadProcInfo(char *szFileConfig);
void ExitApp();
void RefreshListaOrdini(GtkWidget *parent,char *szSelect);
void EditOrdine(char *szOrdineKey);
BOOL EditCollo(char *szOrdineKey,int nNumCollo);
void ListaDistribuzione(void);
char *GetStatoOrdine(char cStato,char *pszBuffer);
char *GetStatoCollo(char cStato,char *pszBuffer);
void UpdateOrdine(char *szOrdineKey);
int UpdateListFromSelect(GtkWidget *clist, char *szSqlCmd, char *szKey, int nNumCollo, int nKeyField, int nColloField);
int CreateListFromSelect(GtkWidget *super_parent, GtkWidget *parent, GtkWidget **clist, char *szCListName, int nSelectMode, char *szSqlCmd, char *szOrdProg, int nNumCollo);
void RefreshListaColli(GtkWidget *parent,char *szSelect);
void RefreshListaColliScartati(GtkWidget *parent,char *szSelect);
gint RichiestaPesoTimeOut( gpointer data );
gint BilanciaTimeOut( gpointer data );
int ColloInPallettizzatore(char *szBarcodeCollo,int *nNC);
BOOL ColloInBilancia(char *szBarcodeColloBilancia,int nPesoColloBilancia);
int CalcolaPesoCollo(char *szOrdKey,int nCollo);
int CalcPesoRealeOrdine(char *szOrdProg);
BOOL CambiaStatoSottordine(char *szOrdKey, char cStato);
BOOL CambiaStatoOrdine(char *szOrdProg, char cStato);
BOOL EvadiCollo(char *szOrdKey,int nCollo,BOOL bForzatura);
BOOL ScartaCollo(char *szOrdKey,int nCollo);
BOOL CheckSottordineEvaso(char *szOrdKey);
BOOL CheckOrdineEvaso(char *szOrdProg);
BOOL SettaPesoSottordine(char *szOrdKey, int nPesoOrdine);
void EditConfigurazione(void);
void ApplyConfigurazione(void);
void ShowDlgRequest(int nPeso,char *szBarcode,char *szMsg);
void ShowColloScartato(char *pszOrdProg);
BOOL SendSockCmd(int nServicePort,char *szHostName, int nCmd,char *szParams);
void do_aggiungi_collo(char *szOrdine, int nNumeroColli, char *szFormatoCollo);
void do_elimina_collo(char *szOrdKey, char *szSegnacollo);
void ShowColloEvaso(char *szOrdProg,int nCollo);
BOOL PesaCollo(char *szOrdKey,int nCollo,int nPesoReale,int nPesoCalcolato);
void UpdateCollo(char *szOrdKey,int nCollo);
void UpdateColloScartato(char *szOrdKey,int nCollo);
char *ColloBarcode(char *pszOrdKey,int nCollo);
BOOL ColloPericoloso(char *szOrdKey,int nCollo);
void GetPLCMsg(int nMsgCode,int nMsgClass);
void ShowPLCMsg(char *pszMsg);
BOOL SetErroreRiga(char *szOrdkey,int nCollo,char *szCodProd, char *szCDERR);
BOOL ResetErroreRiga(char *szOrdkey,int nCollo,char *szCodProd);
BOOL SetFlagBarcodeIdCollo(char *szOrdKey,int nCollo, char cStato);
BOOL CheckLettureBarcodeIDSuCollo(char *szOrdKey,int nCollo);
BOOL RefreshDlgLetturaBarcodeId(void);
BOOL do_lettura_barcode_id (void);
BOOL RefreshDlgEditCollo(void);

/*
* pesoproc.c
*/
gint ProcessMsgs( gpointer data );
void GestPlc(BOOL bCheck);
int GetVarIndex(char *szVarName);
BOOL VarValueChanged(int  nVarIndex);
void UpdateVarList(GtkWidget *clist,BOOL bRefresh);
void GetVarsIndex(void);
BOOL PLCConnect(int *fdPort,char *szHostName);
int ReadVarList(char *szCfgFile,PLCVAR *PLCVars);
int WriteVarList(char *szCfgFile,PLCVAR *PLCVars);

/*
* peso.c
*/
int main (int argc, char *argv[]);
