/*
* volutil.c : Analisi Volumetrica : funzioni di utilita'
* 
* Progetto Easy Picking 3.0 : wella
*
* Autore : Roberto Mantovani
*
* Copyright CNI srl 1995-2000
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

#include	"volum.h"
#include	"volstruct.h"
#include	"volext.h"
#include	"volfun.h"

/*
* LeftStr()
* ritorna gli ultimi nLen caratteri della stringa
*/
char *LeftStr(char *szString,int nLen)
{
	static char szBuffer[1024];

	strcpy(szBuffer,szString+(strlen(szString)-nLen));

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
	nPos=min(nPos,strlen(szString)-nLen);

	memset(szBuffer,' ',nLen);
	strncpy(szBuffer,szString+nPos,nLen);
	szBuffer[nLen]='\0';

	return(szBuffer);
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
	if(Cfg.bAlone){
		DeleteMsgQ(ProcList[PROC_VOLUM].nQNumber);
	}
		

	/*
	* avverto MAIN dello stop
	*/
	SendMessage(PROC_MAIN, PROC_VOLUM, PROGRAM_STOPPED, NULL);

	exit(1);
} /* Fine SafeIntFunc */

/*
* ReadConfiguration()
* lettura della configurazione dal file cni.cfg
*/
void ReadConfiguration(BOOL bReadProcInfo)
{
	char szParagraph[128];
	char szCurrentDirectory[128];
	char szCniCfg[128];

	/*
	* leggo il file di configurazione dalla directory corrente
	*/
	getcwd(szCurrentDirectory,sizeof(szCurrentDirectory));
	
	strcpy(szCniCfg,szCurrentDirectory);
	strcat(szCniCfg,"/cni.cfg");
	/*
	* lettura del file di configurazione
	*/
	strcpy(szParagraph,"General Settings");

	/* SAP */
	Cfg.nSAP = xncGetFileInt(szParagraph, "SAP", 0, szCniCfg, NULL);

	xncGetFileString(szParagraph,"Language",   "ita",      Cfg.szLanguage,   80,szCniCfg,NULL);
	xncGetFileString(szParagraph,"PathData",   "../data",  Cfg.szPathData,   80,szCniCfg,NULL);
	xncGetFileString(szParagraph,"PathExport", "../export",Cfg.szPathExport, 80,szCniCfg,NULL);
	xncGetFileString(szParagraph,"PathExe",    "./",       Cfg.szPathExe,    80,szCniCfg,NULL);

	if(bReadProcInfo){
		ReadProcInfo(szCniCfg);
	}
	/*
	* Shared Memory generale (Struttura pDatiLinea)
	*/
	Cfg.nShmKey=xncGetFileInt(szParagraph,"ShmKey",1000,szCniCfg,NULL);
	Cfg.nDebugVersion=xncGetFileInt(szParagraph,"DebugVersion",0,szCniCfg,NULL);
	Cfg.nMaxColliOrdine=xncGetFileInt(szParagraph,"MaxColliOrdine",0,szCniCfg,NULL);


	/*
	* lettura del file di configurazione del DataBase (PostgreSQL)
	*/
	strcpy(szParagraph,"DataBase Settings");

	xncGetFileString(szParagraph,"PGHost",    "localhost",Cfg.szPGHost,     80,szCniCfg,NULL); 
	xncGetFileString(szParagraph,"PGPort",    "5432",     Cfg.szPGPort,     80,szCniCfg,NULL); 
	xncGetFileString(szParagraph,"PGDataBase","picking",  Cfg.szPGDataBase, 80,szCniCfg,NULL); 
}

/*
* ReadProcInfo()
* Lettura dei parametri relativi ai processi 
*/
void ReadProcInfo(char *szFileConfig)
{
	char szProcIndex[128];
	char szProcBuffer[128];
	int nIndex;

	for (nIndex=0; nIndex<NUM_PROC; nIndex++) {
		sprintf(szProcIndex,"Proc_%02d",nIndex);
		xncGetFileString("procinfo",szProcIndex, "", szProcBuffer, 80,szFileConfig,NULL);
		sscanf(szProcBuffer, "%[^,],%d,%d,%d",
				ProcList[nIndex].szProcName,
				&ProcList[nIndex].nQKey,
				&ProcList[nIndex].nPriority,
				&ProcList[nIndex].bExecute);

		ProcList[nIndex].nGId=nIndex+1;

		/*
		* resetto il numero coda messaggi del processo
		*/
		ProcList[nIndex].nQNumber=0;
	}
}

/*
* BOOL FileExists(char *szFileName)
*/
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
* SendMessage()
* Spedizione messaggio con timeout e tentativi (NON BLOCCANTE)
*/
BOOL SendMessage(int nDest,int nSrce,int nMsgCode,char *szText)
{
	int nRC;

	if((nRC=SendSpecificMsgNoWait(ProcList[nDest].nQNumber, ProcList[nDest].nGId, ProcList[nSrce].nGId, nMsgCode, szText,200,5))!=0){
		trace_out_vstr(1,"Errore [%d] in spedizione Messaggio a [%s]", nRC,ProcList[nDest].szProcName);
		return FALSE;
	}
	return TRUE;
}

/*
* GetDatef(char *szDateFormat)
* Funzione generica di formattazione della data attuale
* Formato di stampa : strftime()
* -------------------------------------------------------
* %% - same as % 
* %a - day of week, using the locale's abbreviated weekday names 
* %A - day of week, using the locale's full weekday names 
* %b - month, using the locale's abbreviated month names 
* %B - month, using the locale's full month names 
* %c - date and time as %x %X 
* %C - century (00-99)
* %d - day of the month (01-31) 
* %D - date as %m/%d/%y 
* %e - day of the month (1-31; single digits are preceded by a blank) 
* %h - synonym for %b
* %H - hour using 24-hour clock (00-23) 
* %I - hour using 12-hour clock (01-12) 
* %j - day of the year (001-366) 
* %k - hour using 24-hour clock (0-23; single digits are preceded by a blank) 
* %l - hour using 12-hour clock (1-12; single digits are preceded by a blank) 
* %m - month number (01-12) 
* %M - minute (00-59) 
* %n - same as \n
* %p - locale's equivalent of the AM/PM designation associated with a 12-hour clock
* %r - time as %I:%M:%S %p 
* %R - time as %H:%M 
* %S - second (00-59) 
* %t - same as \t
* %T - time as %H:%M:%S 
* %U - week number of the year (00-53), where the first Sunday is the first day of week 1
* %w - day of week (0-6), where Sunday is day 0
* %W - week number of the year (00-53), where the first Monday is the first day of week 1
* %x - date, using the locale's date format 
* %X - time, using the locale's time format 
* %y - year within century (00-99) 
* %Y - year, including century (for example, 1988) 
* %Z - time zone abbreviation
*
*/
char *GetDatef(char *szDateFormat)
{
	static char szDateBuffer[128];
	long curr_time=time((long *)0);
	struct tm *ltime=localtime(&curr_time);

	strftime(szDateBuffer,sizeof(szDateBuffer),szDateFormat,ltime);

	return szDateBuffer;
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
