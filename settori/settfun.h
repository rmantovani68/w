/*
* settfun.h
* 
* Progetto Easy Picking 2.0 : Europharco
*
* Autore : Roberto Mantovani
*
* Copyright CNI srl 1995-99
*/
/*
* settutil.c
*/
char *SubStr(char *szString,int nPos,int nLen);
char *RightStr(char *szString,int nLen);
char *LeftStr(char *szString,int nLen);
char *SubStr(char *szString,int nPos,int nLen);
char *StrTrimLeft(char *str);
char *StrTrimRight(char *str);
char *StrTrimAll(char *str);
char *i_strtrimall(char *str);
char *GetTime(long time,char *szTimeString);
char *GetDate(long time,char *szDateString);
char *GetDateYYYYMMDD(long time,char *szDateString);
char *GetTimeHHMMSS(long time,char *szTimeString);
void SaveStateIntFunc();
void SafeIntFunc();
BOOL RunSimpleProcess(int nProcID);
void TerminateProcess(int nProcID);
BOOL ConnectDataBase(PGconn **pDBConn);
BOOL ReadTablesInfo(void);
BOOL SendMessage(int nDest,int nSrce,int nMsgCode,char *szText);
PGresult *PGExecSQL (BOOL bTrace,char *format, ...);
/*
* settfun.c
*/
void ReadConfiguration(BOOL bReadProcInfo);
void ReadStatoSettori(void);
void ReadProcInfo(char *szFileConfig);
BOOL FileExists(char *szFileName);
BOOL GetOrdine(PORDINE pOrdine,int nCedola);
BOOL InizioPrelievoSettore(PORDINE pOrdine,PSETTORE pSettore);
BOOL GestioneSettore(EVENTO *pEvento);
BOOL GestioneBadge(EVENTO *pEvento);
void ReadOperatori(void);
int ReadUbicazioni(PUBICAZIONI pUbi);
void SortUbicazioni(PUBICAZIONI pUbi);
PUBICAZIONE SearchUbicazione(PUBICAZIONI pUbi,int nIOS,int nCPU,int nModulo,int nRowIndex,int nColIndex);
int CmpUbicazione(PUBICAZIONE pUbi1,PUBICAZIONE pUbi2);
void UpdateDisplay(PDISPLAYSTRUCT pDisplay,int nRiga);
void DisplayWriteString(int nIOS, int nCPU, int nModulo, int nRowIndex, int nMode, char *szString);
int GetBCDNibble(unsigned char cChar);
void DisplayWriteStringBCD(int nIOS, int nCPU, int nModulo, int nRowIndex, int nMode, char *szString,unsigned char nDot,unsigned char nBlink);
void SetLampada(int nIOS,int nCPU,int nModulo,int nRiga,int nColonna);
void ResetLampada(int nIOS,int nCPU,int nModule,int nRowIndex, int nColumn);
void ResetLuci(void);
void ResetSettore(int nIsola,int nSettore);
BOOL CambiaStatoOrdine( char *szOrdKey, char cStato);
BOOL CambiaStatoCollo(char *szOrdKey,int nCollo,char cStato);
BOOL CambiaStatoColliOrdine( char *szOrdKey, char cStato);
BOOL CambiaStatoRigheOrdine(char *szOrdKey,int nCollo,char cStato);
BOOL UpdateRigaOrdine(PRIGA_PRELIEVO pRigaOrdine,PSETTORE pSettore);
void ScongelaPKL(void);
void CongelaPKL(void);
int GetSettore(int nIsola,int nSettore);
BOOL RestoreState(char *pszFileName);
BOOL SaveState(char *pszFileName);
char *GetDisplayData(char *pszOrdKey);
void do_simula_luce(int nIsola, int nSettore);
int get_letture_badge(int nIsola, int nSettore);

/*
* settori.c
*/
int main(int argc,char** argv);

