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
* main.c
*/
int main(int argc,char** argv);

/*
* mainutil.c
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


void ReadConfiguration(BOOL bReadProcInfo);
void ReadProcInfo(char *szFileConfig);
