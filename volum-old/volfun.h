/*
* volfun.h
* 
* Progetto Easy Picking 3.0
*
* Autore : Roberto Mantovani
*
* Copyright CNI srl 1995-2000
*/

/*
* volfun.c
*/
int ReadImballi(PIMBALLI pImb);
void SortImballi(PIMBALLI pImb);
int CmpInt(int *pInt1,int *pInt2);
int CmpUbicazione(PUBICAZIONE pUbi1,PUBICAZIONE pUbi2);
int CmpImballo(PIMBALLO pImb1,PIMBALLO pImb2);
BOOL AnalisiVolumetrica(void);
int InsertRiga(PRIGA_ORDINE pRiga,PCOLLO_ORDINE pCollo);
void CreazioneCollo(PCOLLO_ORDINE pCollo,int nCollo,BOOL bPallet);
char *ColloBarcode(PORDINE pOrdine,PCOLLO_ORDINE pCollo);
BOOL UpdateOrdProd(PORDINE pOrdine);
BOOL InsertOrdProd(PORDINE pOrdine,char cStato);
BOOL InsertColProd(PORDINE pOrdine,PCOLLO_ORDINE pCollo,char cStato);
BOOL InsertRigProd(PORDINE pOrdine,PCOLLO_ORDINE pCollo,PRIGA_ORDINE pRigaOrdine, char *szTable,char cStato);
BOOL UpdateSpedizione(PSPEDIZIONE pSpedizione,char cStato);
void UpdateTables(void);

/*
* volutil.c
*/
void ReadConfiguration(BOOL bReadProcInfo);
void ReadProcInfo(char *szFileConfig);
BOOL FileExists(char *szFileName);
char *LeftStr(char *szString,int nLen);
char *SubStr(char *szString,int nPos,int nLen);
char *StrTrimLeft(char *str);
char *StrTrimRight(char *str);
char *StrTrimAll(char *str);
char *GetTime(long time,char *szTimeString);
char *GetTimeHHMMSS(long time,char *szTimeString);
char *GetDate(long time,char *szDateString);
char *GetDateYYYYMMDD(long time,char *szDateString);
void SafeIntFunc();
BOOL SendMessage(int nDest,int nSrce,int nMsgCode,char *szText);
char *GetDatef(char *szDateFormat);
BOOL ReadTableInfo(PDBSTRUCT pDB,char *szCfgBaseName);
BOOL ReadTablesInfo(void);
BOOL ConnectDataBase(PGconn **pDBConn);
PGresult *PGExecSQL (BOOL bTrace,char *format, ...);

/*
* volum.c
*/
int main(int argc,char** argv);

