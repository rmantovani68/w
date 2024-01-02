/*
* settutil.c : gestione settori di magazzino : funzioni di utilita'
* 
* Progetto Easy Picking 3.0 : wella
*
* Autore : Roberto Mantovani
*
* Copyright CNI srl 1995-99
*/
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termio.h>
#include <sys/stat.h>
#include <stdarg.h>

#include <libpq-fe.h>
#include <pmx_msq.h>
#include <msq_lib.h>
#include <shared.h>
#ifdef TRACE
	#include <trace.h>
#endif
#include <ca_file.h>
#include <dbfun.h>
#include <proc_list.h>

#include	"linklist.h"
#include	"settori.h"
#include	"settstruct.h"
#include	"settext.h"
#include	"settfun.h"

/*
* LeftStr()
* ritorna i primi nLen caratteri della stringa
*/
char *LeftStr(char *szString,int nLen)
{
	static char szBuffer[1024];

	strncpy(szBuffer,szString,nLen);
	szBuffer[nLen]='\0';

	return(szBuffer);
}

/*
* RightStr()
* ritorna gli ultimi nLen caratteri della stringa
*/
char *RightStr(char *szString,int nLen)
{
	static char szBuffer[1024];

	strcpy(szBuffer,szString+((strlen(szString)-nLen)));

	return(szBuffer);
}


/*
* SubStr()
* ritorna una porzione della stringa passata in ingresso
* a partire da nPos per nLen caratteri
*/
char *SubStr(char *szString,int nPos,int nLen)
{
	static char szBuffer[1024];

	/*
	* normalizzo nLen per evitare crash di applicazione
	*/
	nLen=min(1023,nLen);
	nPos=min((unsigned int)nPos,strlen(szString)-nLen);

	memset(szBuffer,' ',nLen);
	strncpy(szBuffer,szString+nPos,nLen);
	szBuffer[nLen]='\0';

	return(szBuffer);
}


/*
* StrTrimLeft()
*/
char *StrTrimLeft(char *str)
{
	char *dst,*src;

	dst=src=str;
	while(*src==' ') src++;
	while((*dst++=*src++));
	return(str);
}


/*
* StrTrimRight()
*/
char *StrTrimRight(char *str)
{
	char *dst,*src;

	src=str+strlen(str)-1;
	while(*src==' ') src--;
	*(src+1)='\0';
	dst=src=str;
	while((*dst++=*src++));
	return(str);
}


/*
* StrTrimAll()
*/
char *StrTrimAll(char *str)
{
	StrTrimLeft(str);
	StrTrimRight(str);
	return(str);
} 

char *i_strtrimall(char *str)
{
	StrTrimLeft(str);
	StrTrimRight(str);
	return(str);
}

/*
* GetTime()
* get time of day as a string
*/
char *GetTime(long time,char *szTimeString)
{
	struct tm *ltime;
	long curr_time;

	curr_time = time;
	ltime = localtime(&curr_time);
	sprintf(szTimeString, "%.2d:%.2d:%.2d", 
		ltime->tm_hour, ltime->tm_min, ltime->tm_sec);

	return szTimeString;
}

/*
* GetDate()
* get date a string dd-mm-yy
*/
char *GetDate(long time,char *szDateString)
{
	struct tm *ltime;
	long curr_time;

	curr_time = time;
	ltime = localtime(&curr_time);
	sprintf(szDateString, "%2.2d-%2.2d-%2.2d", 
		ltime->tm_mday, ltime->tm_mon+1, ltime->tm_year%100);

	return szDateString;
}

/*
* GetDateYYYYMMDD()
* get date as a string format YYYYMMDD
*/
char *GetDateYYYYMMDD(long time,char *szDateString)
{
	long curr_time=time;
	struct tm *ltime=localtime(&curr_time);

	sprintf(szDateString,"%4.4d%2.2d%2.2d", 1900+ltime->tm_year,ltime->tm_mon+1,ltime->tm_mday);

	return szDateString;
}

/*
* GetTimeHHMMSS()
* get time of day as a string Formatted HHMMSS
*/
char *GetTimeHHMMSS(long time,char *szTimeString)
{
	struct tm *ltime;
	long curr_time;

	curr_time = time;
	ltime = localtime(&curr_time);
	sprintf(szTimeString, "%.2d%.2d%.2d", 
		ltime->tm_hour, ltime->tm_min, ltime->tm_sec);

	return szTimeString;
}

void SaveStateIntFunc()
{
	char szBuffer[128];

	sprintf(szBuffer,"%s/settori.stato",Cfg.szPathData);
	SaveState(szBuffer);
}

/*
* Funzione SafeIntFunc:
* uscita di sicurezza
*/
void SafeIntFunc()
{
	/*
	* Chiusura connessione a SQL server
	*/
	PQfinish(DBConn);

#ifdef TRACE
	trace_out_vstr_date(1,"Stopped");
#endif

	DetachShm((char *)pDatiLinea);
	DetachShm((char *)pSettori);

	if(!RemoveShm(Cfg.nSettoriShmKey,Cfg.nNumeroSettori*sizeof(SETTORE))){
#ifdef TRACE
			trace_out_vstr_date(1,"Error Removing Shared Memory (%d)", Cfg.nSettoriShmKey);
#endif
	}

	if(Cfg.bAlone){
		DeleteMsgQ(ProcList[PROC_SETTORI].nQNumber);
		if(!RemoveShm(Cfg.nShmKey,sizeof(LINEA_STRUCT))){
#ifdef TRACE
			trace_out_vstr_date(1,"Error Removing Shared Memory (%d)", Cfg.nShmKey);
#endif
		}
		TerminateProcess(PROC_IOS);
	}

	/*
	* avverto MAIN dello stop
	*/
	SendMessage(PROC_MAIN, PROC_SETTORI,  PROGRAM_STOPPED, NULL);

	exit(1);
} /* Fine SafeIntFunc */

/*
* BOOL RunSimpleProcess(int nProcID)
* lancio il processo nProcID
*/
BOOL RunSimpleProcess(int nProcID)
{
	char szProcName[128];

	if(ProcList[nProcID].bExecute){

		/* 
		* Creazione coda messaggi locale per il processo
		*/
		if(ProcList[nProcID].nQKey){
			/* 
			* Creazione coda messaggi locale 
			*/
			if((ProcList[nProcID].nQNumber = CreateMsgQ(ProcList[nProcID].nQKey, 1))<0){
#ifdef TRACE
				trace_out_vstr_date(1,"Creazione coda msg %s (%d) fallita",
					ProcList[nProcID].szProcName,
					ProcList[nProcID].nQKey);
#endif
			}
		}

		sprintf(szProcName,"%s/%s",Cfg.szPathExe,ProcList[nProcID].szProcName);
		ProcList[nProcID].nProcId = ExecModule(szProcName, szProcName,NULL);

		if (ProcList[nProcID].nProcId < 0) {
#ifdef TRACE
			trace_out_vstr_date(1,"Fallita esecuzione modulo %s",
				ProcList[nProcID].szProcName);
#endif
			return FALSE;
		}
	}
	return TRUE;
}

void TerminateProcess(int nProcID)
{
	if(ProcList[nProcID].bExecute){
		kill((pid_t)ProcList[nProcID].nProcId, SIGTERM);
		if(ProcList[nProcID].nQKey){
			DeleteMsgQ(ProcList[nProcID].nQNumber);
		}
	}
}

BOOL FileExists(char *szFileName)
{
	FILE *fp;

	if((fp=fopen(szFileName,"r"))!=(FILE *)NULL){
		fclose(fp);
		return(TRUE);
	}
	return(FALSE);
}

/*
* ReadTableInfo(PDBSTRUCT pDB,char *szCfgBaseName)
* Lettura delle informazioni relative ad una tabella di PGSQL
*/
BOOL ReadTableInfo(PDBSTRUCT pDB,char *szCfgBaseName)
{
	char szCfgFile[128];

	/*
	* creo i nomi di: Archivio, Indice, Cfg file
	*/
	sprintf(szCfgFile,"%s/%s.cfg",Cfg.szPathData,szCfgBaseName);

	pDB->nTagsNumber=ReadTagsInfo(szCfgFile,pDB);

	if((ReadFieldsInfo(szCfgFile,pDB))==-1){
		return(FALSE);
	} else {
		return(TRUE);
	}
}

/*
* lettura configurazione tabelle
*/
BOOL ReadTablesInfo(void)
{
	int nIndex=0;

	while(UsedDB[nIndex].pDB){
		if(ReadTableInfo(UsedDB[nIndex].pDB,UsedDB[nIndex].szTableCfg)==FALSE){
#ifdef TRACE
			trace_out_vstr(1, "Errore in Apertura / Creazione [%s]\n",UsedDB[nIndex].szTableCfg);
#endif
		}
		nIndex++;
	}

	return(TRUE);

}

BOOL ConnectDataBase(PGconn **pDBConn)
{
	BOOL bRetValue;

	/*
	* connessione al database
	*/
	*pDBConn = PQsetdb(Cfg.szPGHost, Cfg.szPGPort, NULL, NULL, Cfg.szPGDataBase);
	
	/*
	* controllo correttezza della connessione
	*/
	if (PQstatus(*pDBConn) == CONNECTION_BAD) {
#ifdef TRACE
		trace_out_vstr_date(1, "Connection to database '%s' failed. (%s)", Cfg.szPGDataBase);
		trace_out_vstr_date(1, "%s", PQerrorMessage(*pDBConn));
#endif
		/*
		gtk_text_printf(txt_msgs,"Connection to database '%s' failed.\n", Cfg.szPGDataBase);
		gtk_text_printf(txt_msgs,"%s]\n", PQerrorMessage(*pDBConn));
		*/
		bRetValue=FALSE;
	} else {
#ifdef TRACE
		trace_out_vstr_date(1, "Connection to database '%s' OK.", Cfg.szPGDataBase);
#endif
		/*
		gtk_text_printf(txt_msgs,"Connection to database '%s' OK.\n", Cfg.szPGDataBase);
		*/
		bRetValue=TRUE;
	}
	return bRetValue;
}

/*
* PGresult *PGExecSQL (char *format, ...)
* Wrapper function for SQL statements
*/
PGresult *PGExecSQL (BOOL bTrace,char *format, ...)
{
	va_list ap;
	char command[4096];
	PGresult *retval;

	va_start (ap, format);
	vsprintf (command, format, ap);
	va_end (ap);
	if(bTrace){
#ifdef TRACE
		trace_out_vstr_date(1,"EXEC SQL [%s]", command);
#endif
	}
	retval = PQexec (DBConn, command);

	/*
	* Controllo ritorno del comando SQL
	*/
	if(bTrace){
#ifdef TRACE
		switch(PQresultStatus(retval)){
			case PGRES_EMPTY_QUERY:
				trace_out_vstr_date(1,"PGRES_EMPTY_QUERY : %s",PQresultErrorMessage(retval));
			break;
			case PGRES_COMMAND_OK:
				trace_out_vstr_date(1,"PGRES_COMMAND_OK");
			break;
			case PGRES_TUPLES_OK:
				trace_out_vstr_date(1,"PGRES_TUPLES_OK : [%5d] tuples", PQntuples (retval));
			break;
			case PGRES_COPY_OUT:
				trace_out_vstr_date(1,"PGRES_COPY_OUT");
			break;
			case PGRES_COPY_IN:
				trace_out_vstr_date(1,"PGRES_COPY_IN");
			break;
			case PGRES_BAD_RESPONSE:
				trace_out_vstr_date(1,"PGRES_BAD_RESPONSE : %s",PQresultErrorMessage(retval));
			break;
			case PGRES_NONFATAL_ERROR:
				trace_out_vstr_date(1,"PGRES_NONFATAL_ERROR : %s",PQresultErrorMessage(retval));
			break;
			case PGRES_FATAL_ERROR:
				trace_out_vstr_date(1,"PGRES_FATAL_ERROR : %s",PQresultErrorMessage(retval));
			break;
		}
#endif
	}

	return retval;
}

/*
* SendMessage()
* Spedizione messaggio con timeout e tentativi (NON BLOCCANTE)
*/
BOOL SendMessage(int nDest,int nSrce,int nMsgCode,char *szText)
{
	int nRC;

	if((nRC=SendSpecificMsg(ProcList[nDest].nQNumber, ProcList[nDest].nGId, ProcList[nSrce].nGId, nMsgCode, szText))!=0){
		trace_out_vstr_date(1,"Errore [%d] in spedizione Messaggio a [%s]", nRC,ProcList[nDest].szProcName);
		return FALSE;
	}
	return TRUE;
}
