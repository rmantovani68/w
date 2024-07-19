/*
* printfun.h
* 
* Progetto Easy Picking 3.0
*
* Autore : Roberto Mantovani
*
* Copyright CNI srl 1998-2000
*/

/*
* printlabel.c
*/
int main(int argc,char** argv);

/*
* printfun.c
*/
int GetMaxLineWidth(char *szFile);
BOOL PrintFile(char *szFile,char *szPrinter,char *szTitle,int nPrintType,BOOL bDelete);
void ReadConfiguration(BOOL bReadProcInfo);
void ReadProcInfo(char *szFileConfig);
int ReadPrintFieldsInfo(char *szCfgFile);
void ReadPrintersConfiguration(void);
char *GetStatoCollo(char cStato,char *pszBuffer);
BOOL StampaOrdini(char *pszPrinterName,char *szSequence);
BOOL StampaOrdineBertello(char *pszOrdKey,char *pszPrinterName,BOOL bSendMsg,int nGID);
BOOL StampaOrdine(char *pszOrdKey,char *pszPrinterName,BOOL bSendMsg,int nGID,char* szSequence);
BOOL StampaOrdineSenzaColliPdf(char *pszOrdKey,char *pszPrinterName,BOOL bSendMsg,int nGID,char *szSequence);
BOOL StampaOrdineSoloColliPdf(char *pszOrdKey,char *pszPrinterName,BOOL bSendMsg,int nGID,char *szSequence);
BOOL StampaOrdineStorico(char *pszOrdProg, char *pszPrinterName);
BOOL StampaCollo(char *pszOrdKey,int nCollo,char *pszPrinterName,char *szSequence);
BOOL StampaEtichette(char *szSQLCmd,FILE *fp,char *szSequence);
BOOL StampaEtichetteAmazon(char *szSQLCmd,FILE *fp);
BOOL StampaEtichetta(char *szSQLCmd,FILE *fp,char *szLabelBaseName);
BOOL StampaInizioElaborazione(char *pszOrdKey,char *pszPrinterName);
BOOL StampaFineElaborazione(char *pszOrdKey,char *pszPrinterName);
//BOOL StampaContieneDocumento(char *pszPrinterName);
BOOL StampaContieneDocumento(char *pszOrdKey, char *pszPrinterName);
int	CalcDigit(char value);
char CalcCheck(char *szString);
BOOL StampaPackingListReport(char *szOrdKey, char *szPrinterName, int tipo);
BOOL StampaPackingList(char *szOrdKey, char *szPrinterName);
BOOL StampaPackingListSenzaColliPdf(char *szOrdKey, char *szPrinterName);
BOOL StampaPackingListSoloColliPdf(char *szOrdKey, char *szPrinterName);
BOOL StampaPackingListCollo(char *szOrdKey, int nCollo, char *szPrinterName);
BOOL StampaPdfOrdine(char *szOrdine, char *szPrinterName);
BOOL StampaPdfOrdineAdvanced(char *szOrdine, char *szPrinterName);
int PrintListFromSelect(FILE *fp,char *szSqlCmd, char *szOrdProg, int nNumCollo,char *szPrinterName,int nSeparatorFieldIndex);
int CustomPrintListFromSelect(FILE *fp,char *szSqlCmd, char *szOrdProg, int nNumCollo,char *szPrinterName,int nSeparatorFieldIndex,int nCodiceProdottoFieldIndex);
char GetPdfFlagFromOrdProd(char *szOrdKey);
char GetPdfFlagFromRicOrd(char *szOrdine);
char GetFtzFlagFromOrdProd(char *szOrdKey);
char GetFtzFlagFromRicOrd(char *szOrdine);

/*
* printutil.c
*/
BOOL FileExists(char *szFileName);
char *LeftStr(char *szString,int nLen);
char *RightStr(char *szString,int nLen);
char *SubStr(char *szString,int nPos,int nLen);
char *StrTrimLeft(char *str);
char *StrTrimRight(char *str);
char *StrTrimAll(char *str);
char *GetTime(long time,char *szTimeString);
char *GetDate(long time,char *szDateString);
char *GetDateYYYYMMDD(long time,char *szDateString);
void SafeIntFunc();
BOOL SendBroadcastMessage(int nSrce,int nMsgCode,char *szText);
BOOL SendMessage(int nDest,int nSrce,int nMsgCode,char *szText);
char *GetDatef(char *szDateFormat);
BOOL ReadTableInfo(PDBSTRUCT pDB,char *szCfgBaseName);
BOOL ReadTablesInfo(void);
BOOL ConnectDataBase(PGconn **pDBConn);
PGresult *PGExecSQL (BOOL bTrace,char *format, ...);
void SafeIntFunc();
