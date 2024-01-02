/*
* mainutil.c
* utility functions
* Gestione Stampanti
* Easy Picking 3.0
* Copyright CNI srl 1999-2000
* Autore : Roberto Mantovani
*/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#ifdef TRACE
	#include <trace.h>
#endif

#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <libpq-fe.h>

#include <proc_list.h>
#include <ca_file.h>
#include <pmx_msq.h>
#include <dbfun.h>

#include	"main.h"
#include	"mainstruct.h"
#include	"mainext.h"
#include	"mainfun.h"

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
* GetTimeFromHHMMSS()
* get time of day from a string formatted HHMMSS
*/
char *GetTimeFromHHMMSS(char *szFromTime,char *szTimeString)
{
	if(strlen(szFromTime)){
		strcpy(szTimeString,szFromTime);			/* ora */
		szTimeString[2]='\0';
		strcat(szTimeString,":");
		strcat(szTimeString,szFromTime+2);	/* minuti */
		szTimeString[5]='\0';
		strcat(szTimeString,":");
		strcat(szTimeString,szFromTime+4);	/* secondi */
		szTimeString[8]='\0';
	} else {
		szTimeString[0]='\0';
	}

	return szTimeString;
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
* GetTimeHHMM()
* get time of day as a string Formatted HHMM
*/
char *GetTimeHHMM(long time,char *szTimeString)
{
	struct tm *ltime;
	long curr_time;

	curr_time = time;
	ltime = localtime(&curr_time);
	sprintf(szTimeString, "%.2d%.2d", 
		ltime->tm_hour, ltime->tm_min);

	return szTimeString;
}

/*
* GetDate()
* get date a string
*/
char *GetDate(long time,char *szDateString)
{
	struct tm *ltime;
	long curr_time;

	curr_time = time;
	ltime = localtime(&curr_time);
	sprintf(szDateString, "%.2d-%.2d-%.2d", 
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
* GetDateYYMMDD()
* get date as a string format YYMMDD
*/
char *GetDateYYMMDD(long time,char *szDateString)
{
	long curr_time=time;
	struct tm *ltime=localtime(&curr_time);

	sprintf(szDateString,"%2.2d%2.2d%2.2d", ltime->tm_year%100,ltime->tm_mon+1,ltime->tm_mday);

	return szDateString;
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
* GetDateDDMM()
* get date as a string format DDMM
*/
char *GetDateDDMM(long time,char *szDateString)
{
	long curr_time=time;
	struct tm *ltime=localtime(&curr_time);

	sprintf(szDateString,"%2.2d%2.2d", ltime->tm_mday, ltime->tm_mon+1);

	return szDateString;
}

/*
* GetDateDDMMYYYY()
* get date as a string format DDMMYYYY
*/
char *GetDateDDMMYYYY(long time,char *szDateString)
{
	long curr_time=time;
	struct tm *ltime=localtime(&curr_time);

	sprintf(szDateString,"%2.2d%2.2d%4.4d", ltime->tm_mday, ltime->tm_mon+1, 1900+ltime->tm_year);

	return szDateString;
}

/*
* GetDateFromYYYYMMDD()
* get date from a string format YYYYMMDD
*/
char *GetDateFromYYYYMMDD(char *szFromDate,char *szDateString)
{
	if(strlen(szFromDate)){
		strcpy(szDateString,szFromDate+6);	/* giorno */
		strcat(szDateString,"-");
		strcat(szDateString,szFromDate+4);	/* mese */
		szDateString[5]='\0';
		strcat(szDateString,"-");
		strcat(szDateString,szFromDate+2);	/* anno */
		szDateString[8]='\0';
	} else {
		szDateString[0]='\0';
	}

	return szDateString;
}
/*
* GetDateYYMMFromDDMMYY()
* get date YYMM from a string formatted DDMMYY
*/
char *GetDateYYMMFromDDMMYY(char *szFromDate,char *szDateString)
{
	char szYear[80];
	char szMonth[80];

	/* anno */
	strcpy(szYear,szFromDate+4);
	szYear[2]='\0';	
	/* mese */
	strcpy(szMonth,szFromDate+2);
	szMonth[2]='\0';	

	sprintf(szDateString,"%02d%02d", atoi(szYear),atoi(szMonth));

	return szDateString;
}

/*
* GetDateYYYYMMDDFromDDMMYY()
* get date YYYYMMDD from a string formatted DDMMYY
*/
char *GetDateYYYYMMDDFromDDMMYY(char *szFromDate,char *szDateString)
{
	char szBuffer[80];

	if(strlen(szFromDate)<6){
		szDateString[0]='\0';
	}
	/* anno */
	strcpy(szBuffer,szFromDate+4);
	szBuffer[2]='\0';	
	sprintf(szDateString,"%04d",
		atoi(szBuffer)>90?1900+atoi(szBuffer):2000+atoi(szBuffer));
	strcat(szDateString,szFromDate+2);	/* mese */
	szDateString[6]='\0';
	strcat(szDateString,szFromDate);	/* giorno */
	szDateString[8]='\0';

	return szDateString;
}

/*
* GetDateFromDDMMYY()
* get date from a string formatted DDMMYY
*/
char *GetDateFromDDMMYY(char *szFromDate,char *szDateString)
{
	strcpy(szDateString,szFromDate);	/* giorno */
	szDateString[2]='\0';
	strcat(szDateString,"-");
	strcat(szDateString,szFromDate+2);	/* mese */
	szDateString[5]='\0';
	strcat(szDateString,"-");
	strcat(szDateString,szFromDate+4);	/* anno */

	return szDateString;
}


char *DeleteNewLine(char *str)
{
	char *dst,*src;

	src=str+strlen(str)-1;
	while(*src=='\n' || *src=='\r') {
		*src='\0';
		src--;
	}
	*(src+1)='\0';
	dst=src=str;
	while((*dst++=*src++));
	return(str);
}

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

	memset(szBuffer,(int)' ',nLen);
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

/*
* RightPad()
* mette gli spazi in coda alla stringa passata in ingresso
* ritorna la stringa passata in ingresso
*/
char *RightPad(char *szString,int nLen)
{
	char szSpaceString[256];
	int nLenUsed=min(255,nLen);
	int nStringLen;

	nStringLen=strlen(szString);

	if (nStringLen>=nLenUsed){
		return(szString);
	}
	memset(szSpaceString,' ',nLenUsed);
	szSpaceString[nLenUsed]='\0';


	memcpy(szSpaceString+(nLenUsed-nStringLen),szString,nStringLen);
	strcpy(szString,szSpaceString);

	return(szString);
}

/*
* PadZero()
* mette gli zeri in testa alla stringa passata in ingresso
* ritorna la stringa passata in ingresso
*/
char *PadZero(char *szString,int nLen)
{
	char szZeroString[256];
	int nLenUsed=min(255,nLen);
	int nStringLen;

	StrTrimAll(szString);

	nStringLen=strlen(szString);

	if (nStringLen>=nLenUsed){
		return(szString);
	}
	memset(szZeroString,'0',nLenUsed);
	szZeroString[nLenUsed]='\0';


	memcpy(szZeroString+(nLenUsed-nStringLen),szString,nStringLen);
	strcpy(szString,szZeroString);

	return(szString);
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
* ElimNewLine()
*/
char *ElimNewLine(char *str)
{
	char *dst,*src;

	dst=src=str;

	while(*src!='\n' && *src!='\r' && *src!='\0'){
		*dst++=*src++;
	}
	*dst='\0';

	return(str);
}

/*
* SendMessage()
* Spedizione messaggio con timeout e tentativi (NON BLOCCANTE)
*/
BOOL SendMessage(int nDest,int nSrce,int nMsgCode,char *szText)
{
	int nRC;

	if(ProcList[nDest].nQNumber){
		if((nRC=SendSpecificMsg(ProcList[nDest].nQNumber, ProcList[nDest].nGId, ProcList[nSrce].nGId, nMsgCode, szText))!=0){
			trace_out_vstr_date(1,"Errore [%d] in spedizione Messaggio a [%s]", nRC,ProcList[nDest].szProcName);
			return FALSE;
		}
	} else {
		return FALSE;
	}
	return TRUE;
}

/*
* SendBroadcastMessage()
* Spedizione messaggio con timeout e tentativi (NON BLOCCANTE)
*/
BOOL SendBroadcastMessage(int nSrce,int nMsgCode,char *szText)
{
	int nRC;
	int nIndex;
	int nQNumber;

	for (nIndex=0; nIndex<NUM_PROC; nIndex++) {
		if(ProcList[nIndex].bExecute && ProcList[nIndex].nQKey){
			if((nQNumber = OpenMsgQ(ProcList[nIndex].nQKey))>=0){
				if((nRC=SendSpecificMsgNoWait(nQNumber, ProcList[nIndex].nGId, ProcList[nSrce].nGId, nMsgCode, szText,100,1))!=0){
					trace_out_vstr_date(1,"Errore [%d] in spedizione Messaggio a [%s]", nRC,ProcList[nIndex].szProcName);
					return FALSE;
				}
			}
		}
	}
	return TRUE;
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
		bRetValue=FALSE;
	} else {
#ifdef TRACE
		trace_out_vstr_date(1, "Connection to database '%s' OK.", Cfg.szPGDataBase);
#endif
		bRetValue=TRUE;
	}
	return bRetValue;
}


/*
* PGresult *PGExecSQL (BOOL bTrace, char *format, ...)
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
* Funzione SafeIntFunc:
* uscita di sicurezza
*/
void SafeIntFunc()
{
#ifdef TRACE
	trace_out_vstr_date(1,"Stopped");
#endif
	PQfinish(DBConn);

	exit(0);
} /* Fine SafeIntFunc */
