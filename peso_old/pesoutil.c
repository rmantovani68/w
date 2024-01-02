/*
* pesoutil.c
* utility functions
* Gestione Peso/Fine Linea
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

#include <gtk/gtk.h>
#include <time.h>
#include <signal.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <libpq-fe.h>

#include "interface.h"
#include "support.h"

#include <proc_list.h>
#include <ca_file.h>
#include <pmx_msq.h>
#include <dbfun.h>

#include "plcsock.h"

#include "picking.h"
#include "pesostruct.h"
#include "pesoext.h"
#include "pesofun.h"

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
* funzione di stampa formattata in un gtk label widget
*/
void gtk_label_printf(GtkWidget *w,...)
{
	va_list args;
	char msgbuf[1024];
	char *fmt;

	if(w==NULL){
		return;
	}
	va_start(args,w);
	fmt=va_arg(args,char *);
	(void)vsprintf(msgbuf,fmt,args);
	va_end(args);
	gtk_label_set (GTK_LABEL(w), msgbuf);
}

/*
* funzione di stampa formattata in un gtk text widget
*/
void gtk_text_printf(GdkColor *Color,GtkWidget *w,...)
{
	va_list args;
	char msgbuf[1024];
	char *fmt;

	if(w==NULL){
		return;
	}

	va_start(args,w);
	fmt=va_arg(args,char *);
	(void)vsprintf(msgbuf,fmt,args);
	va_end(args);
	gtk_text_insert (GTK_TEXT(w), NULL, Color, NULL, msgbuf, -1);
}

/*
* SendMessage()
* Spedizione messaggio con timeout e tentativi (NON BLOCCANTE)
*/
BOOL SendMessage(int nDest,int nSrce,int nMsgCode,char *szText)
{
	int nRC;

	if((nRC=SendSpecificMsgNoWait(ProcList[nDest].nQNumber, ProcList[nDest].nGId, ProcList[nSrce].nGId, nMsgCode, szText,200,5))!=0){
		trace_out_vstr_date(1,"Errore [%d] in spedizione Messaggio a [%s]", nRC,ProcList[nDest].szProcName);
		return FALSE;
	}
	return TRUE;
}

/*
* BOOL ReadTableInfo(PDBSTRUCT pDB,char *szCfgBaseName)
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

	while(Tables[nIndex].pDB){
		if(ReadTableInfo(Tables[nIndex].pDB,Tables[nIndex].szTableCfg)==FALSE){
			gtk_text_printf(NULL,txt_msgs, "Errore in Apertura / Creazione [%s]\n",Tables[nIndex].szTableCfg);
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
		gtk_text_printf(NULL,txt_msgs,"Connection to database '%s' failed.\n", Cfg.szPGDataBase);
		gtk_text_printf(NULL,txt_msgs,"%s]\n", PQerrorMessage(*pDBConn));
		bRetValue=FALSE;
	} else {
#ifdef TRACE
		trace_out_vstr_date(1, "Connection to database '%s' OK.", Cfg.szPGDataBase);
#endif
		gtk_text_printf(NULL,txt_msgs,"Connection to database '%s' OK.\n", Cfg.szPGDataBase);
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
* Conta le righe di un file ascii
*/
int ContaRighe(char *szFileName)
{
	FILE *fp;
	int nRighe=0;
	char c;

	if((fp=fopen(szFileName,"r"))!=(FILE *)NULL){
		while((c=fgetc(fp))!=EOF){
			if(c=='\n'){
				nRighe++;
			}
		}
		fclose(fp);
	}
	return nRighe;
}


/*
* BOOL GetAsciiRecord(szBuffer,pDB)
* Legge dal buffer i dati da inserire nella tabella 
* rm + st : 16-06-2000 : portata su PGSQL
*/
BOOL GetAsciiRecord(char *szBuffer,PDBSTRUCT pDB)
{
	int nFieldPosition;
	int nLineNumber;
	int nFieldIndex;
	int nFieldLen;
	int nFirst;
	char szFieldString[128];
	char szTmp[128];
	char szSQLCmd[2048];
	int nLen;
	int nIndex;
	int nTmpIndex;
	PGresult *PGRes;

	/*
	* controllo lunghezza record
	*/
	if(strlen(szBuffer)<(unsigned int)pDB->nRecordLen){
		return FALSE;
	}

	DeleteNewLine(szBuffer);
	nLen=strlen(szBuffer);

	if(nLen >= pDB->nRecordLen && !strncmp(szBuffer+pDB->nRecordTypePos,pDB->szRecordType,pDB->nRecordTypeLen)){
		nLineNumber=0;

		/*
		* rm 19-01-1999 : gestione multilinea
		*/
		if(pDB->nLinesNumber>1){
			nLineNumber=szBuffer[pDB->nLineNumberPos]-'0';
		}

		/*
		* Comando SQL di inserimento dati in tabella
		*/
		sprintf(szSQLCmd,"INSERT INTO %s (",pDB->szTableName);
		nFirst=0;
		for(nFieldIndex=0; nFieldIndex < pDB->nFieldsNumber; nFieldIndex++){
			if(pDB->pFields[nFieldIndex]->nFieldFlag & IMPORT_MASK){
				strcat(szSQLCmd, nFirst?",":"");
				strcat(szSQLCmd,pDB->pFields[nFieldIndex]->szFieldName);
			}
		}
		strcat(szSQLCmd,") VALUES (");
		/*
		* Aggiunta di una riga alla tabella
		*/
		nFirst=0;
		nFieldPosition=pDB->nDataStart;
		for(nFieldIndex=0; nFieldIndex < pDB->nFieldsNumber; nFieldIndex++){
			if(pDB->pFields[nFieldIndex]->nLineNumber==(unsigned int)nLineNumber){

				/* calcolo la lunghezza  del campo */
				nFieldLen=pDB->pFields[nFieldIndex]->nFieldLen+(pDB->pFields[nFieldIndex]->nFieldDec ? pDB->pFields[nFieldIndex]->nFieldDec+1 : 0 );

				/*
				* rm 19-01-1999 : gestione offset
				*/
				if(pDB->pFields[nFieldIndex]->nFieldOffset){
					nFieldPosition=pDB->pFields[nFieldIndex]->nFieldOffset;
				}

				if(pDB->pFields[nFieldIndex]->nFieldFlag & IMPORT_MASK){
					/* copio il dato */
					strncpy(szFieldString, szBuffer+nFieldPosition, nFieldLen);
					if(!strlen(szFieldString)) strcpy(szFieldString," ");
					szFieldString[nFieldLen]='\0';

					/*
					* Comando SQL
					*/
					strcat(szSQLCmd, nFirst?",":"");
					switch(pDB->pFields[nFieldIndex]->cFieldType){
						case 'N': /* numero  */
							strcat (szSQLCmd, szFieldString);
							break;
						case 'C': /* carattere */
							nIndex=0;
							nTmpIndex=0;
							while(szFieldString[nIndex]){
								if(szFieldString[nIndex]=='\''){
									szTmp[nTmpIndex]=szFieldString[nIndex];
									nTmpIndex++;
								}
								szTmp[nTmpIndex]=szFieldString[nIndex];
								nIndex++;
								nTmpIndex++;
							}
							szTmp[nIndex]='\0';
							strcat (szSQLCmd, "'");
							strcat (szSQLCmd, szTmp);
							strcat (szSQLCmd, "'");
							break;
					}
					nFirst=1;
				} 
				/* calcolo la posizione del campo */
				nFieldPosition+=nFieldLen;
			}
		}
		strcat(szSQLCmd,");");
		PGRes=PGExecSQL(FALSE,szSQLCmd);
		PQclear(PGRes);
		return TRUE;
	}
	return FALSE;
}

/*
* WriteAsciiRecord()
* scrittura di un record in formato ascii
* nel file di uscita 
*/
BOOL WriteAsciiRecord(FILE *fp,PDBSTRUCT pDB,PGresult *PGRes,int nIndex)
{
	char *pBuffer;
	char szBuffer[512];
	PFIELD pField;
	int nFieldIndex;
	int nFieldPosition;
	int nFieldLen;
	int nRecordLen=0;
	int nFieldsNumber=0;

	nFieldsNumber=PQnfields(PGRes);

	/*
	* Ricavo la lunghezza del buffer di scrittura
	*/
	nRecordLen=0;
	for(nFieldIndex=0;nFieldIndex<nFieldsNumber;nFieldIndex++){
		nRecordLen+=pDB->pFields[nFieldIndex]->nFieldLen;
	}
	pBuffer=malloc(nRecordLen+3);

	if(nRecordLen){
		memset(pBuffer,' ',nRecordLen);
		pBuffer[nRecordLen+0] = '\r';
		pBuffer[nRecordLen+1] = '\n';
		pBuffer[nRecordLen+2] = '\0';
	}
	/* 
	* riempio il buffer con i dati del record 
	*/
	nFieldPosition=0;
	for(nFieldIndex=0;nFieldIndex<nFieldsNumber;nFieldIndex++){
		pField=pDB->pFields[nFieldIndex];
		/*
		* calcolo la lunghezza del campo 
		*/
		nFieldLen=pField->nFieldLen;
		/*
		* Esporto il campo
		*/
		switch(pField->cFieldType){
			default:
			case 'C':
			case 'c':
				sprintf(szBuffer, "%-*s", nFieldLen,PQgetvalue(PGRes,nIndex,nFieldIndex));
			break;
			case 'N':
			case 'n':
				sprintf(szBuffer, "%*d", nFieldLen,atoi(PQgetvalue(PGRes,nIndex,nFieldIndex)));
			break;
		}
		memcpy(pBuffer+nFieldPosition, szBuffer, nFieldLen);
		nFieldPosition+=nFieldLen;
	}
	fprintf(fp,pBuffer);
	free(pBuffer);

	return(TRUE);
}

/*
* IsAsciiRecord(szBuffer,pDB)
*/
BOOL IsAsciiRecord(char *szBuffer,PDBSTRUCT pDB)
{
	if(!strncmp(szBuffer+pDB->nRecordTypePos, pDB->szRecordType, pDB->nRecordTypeLen)){
		return TRUE;
	}
	return FALSE;
}

/*
* void gtk_update(void)
*/
void gtk_update(void)
{
  while (gtk_events_pending())
    gtk_main_iteration();
}



/*
* BOOL UpdateAsciiRecord(char *szBuffer,PDBSTRUCT pDB, BOOL bUpdate)
* + st  02/10/2000 
* Legge i dati da szBuffer:
* - se bUpdate=TRUE effettua un ricerca nella tabella utilizzando la chiave "primaria -> UNIQUE e COSTITUITA DA UN SOLO CAMPO" 
*              (la prima indicata nel file di configurazione .cfg) effettuando un update se trova gia' presente un riga; altrimenti inserisce;
* - se bUpdate=FALSE inserisce direttamente in tabellai (ATTENZIONE ALLE CHIAVI "primarie");
*/
BOOL UpdateAsciiRecord(char *szBuffer, PDBSTRUCT pDB, BOOL bUpdate)
{
	int nFieldPosition;
	int nFieldIndex;
	int nKeyFieldIndex=-1;
	int nLen;
	char szFieldString[128];
	char szTmpString[128];
	char szUpdateSQLCmd[1024];
	char szInsertSQLCmd[1024];
	char szKeyFieldData[128];
	char *pPtr;
	PGresult *PGRes;

	/*
	* inserimento dei dati in tabelle PostgreSQL
	*/
	if(pDB->Tags==(PTAG)NULL){
		fprintf(stderr,"Errore : Overwrite e Indice [0] non presente !!!\n");
#ifdef TRACE
		trace_out_vstr_date(1,"UpdateAsciiRecord(%s) : Errore : Overwrite e Indice [0] non presente !!!" , pDB->szTableName);
#endif
		return FALSE;
	}
	for(nFieldIndex=0; nKeyFieldIndex==-1 && (nFieldIndex < pDB->nFieldsNumber); nFieldIndex++){
		if(!strcmp(pDB->pFields[nFieldIndex]->szFieldName, pDB->Tags[0].szExpression)){
			nKeyFieldIndex=nFieldIndex;
		}
	}
	if(nKeyFieldIndex==-1){
		fprintf(stderr,"Errore : Overwrite attivo e campo indice non presente [%s]\n",pDB->Tags[0].szExpression);
#ifdef TRACE
		trace_out_vstr_date(1,"UpdateAsciiRecord(%s) : Errore : Overwrite attivo e campo indice non presente [%s]" , pDB->szTableName,pDB->Tags[0].szExpression);
#endif
		return FALSE;
	}

	DeleteNewLine(szBuffer);
	nLen=strlen(szBuffer);

	if(nLen >= pDB->nRecordLen && !strncmp(szBuffer+pDB->nRecordTypePos,pDB->szRecordType,pDB->nRecordTypeLen)){

		/*
		* aggiungo un record al DataBase 
		*/
		if (bUpdate){
			strcpy (szUpdateSQLCmd,"UPDATE " );
			strcat (szUpdateSQLCmd, pDB->szTableName);
			strcat (szUpdateSQLCmd," SET "); 
		}

		strcpy (szInsertSQLCmd,"INSERT INTO " );
		strcat (szInsertSQLCmd, pDB->szTableName);
		strcat (szInsertSQLCmd," VALUES ( "); 

		nFieldPosition=pDB->nDataStart;

		for(nFieldIndex=0; nFieldIndex<pDB->nFieldsNumber; nFieldIndex++){
			/*
			* gestione offset
			*/
			if(pDB->pFields[nFieldIndex]->nFieldOffset){
				nFieldPosition=pDB->pFields[nFieldIndex]->nFieldOffset;
			}
			if(pDB->pFields[nFieldIndex]->nFieldFlag & IMPORT_MASK){
				/* copio il dato */
				strncpy(szFieldString, szBuffer+nFieldPosition, pDB->pFields[nFieldIndex]->nFieldLen);
				szFieldString[pDB->pFields[nFieldIndex]->nFieldLen]='\0';

				if(nFieldIndex==nKeyFieldIndex) {
					strcpy(szKeyFieldData,szFieldString);
				}

				/*
				* Conversione per il valori numerici
				*/
				if(pDB->pFields[nFieldIndex]->cFieldType=='N'){
					sprintf(szFieldString,"%0d",atoi(szFieldString));
				}
				if (bUpdate){
					strcat (szUpdateSQLCmd, nFieldIndex?", ":" ");
				}
				strcat (szInsertSQLCmd, nFieldIndex?", ":" ");

				/*
				* Se sono in overwrite inserisco il nome del campo prima 
				* della assegnazione a valore
				*/
				if (bUpdate){
					strcat (szUpdateSQLCmd, pDB->pFields[nFieldIndex]->szFieldName);
					strcat (szUpdateSQLCmd, "=");
				}

				/* 
				* inserimento valore attributo
				*/
				if(pDB->pFields[nFieldIndex]->cFieldType=='C'){
					if (bUpdate){
						strcat (szUpdateSQLCmd, "\'");
					}
					strcat (szInsertSQLCmd, "\'");
				}
				/*
				* sostituzione del (') 
				*/ 
				pPtr=szFieldString;
				while ((pPtr=strchr(pPtr,'\''))!=(char *)NULL){
					/*
					* Tronco la stringa sull'apice
					*/
					*pPtr = '\0';
					strcpy(szTmpString,szFieldString);
					strcat(szTmpString,"\\'");
					strcat(szTmpString,(pPtr+1));

					/*
					* Riporto la modifica
					*/
					strcpy(szFieldString,szTmpString);
					pPtr+=2;
				}
				if (bUpdate){
					strcat (szUpdateSQLCmd, szFieldString);
				}
				strcat (szInsertSQLCmd, szFieldString);

				if(pDB->pFields[nFieldIndex]->cFieldType=='C'){
					if (bUpdate){
						strcat (szUpdateSQLCmd, "\'");
					}
					strcat (szInsertSQLCmd, "\'");
				}
			
			}
			nFieldPosition+=pDB->pFields[nFieldIndex]->nFieldLen;
		}
		/*
		* in overwrite inserisco la condizione di ricerca
		*/
		if (bUpdate){
			strcat (szUpdateSQLCmd, " WHERE ");
			strcat (szUpdateSQLCmd, pDB->Tags[0].szExpression);
			strcat (szUpdateSQLCmd, " = ");

			if(pDB->pFields[nKeyFieldIndex]->cFieldType=='C'){
				strcat (szUpdateSQLCmd, "\'");
			}
			/*
			* Conversione per il valori numerici
			*/
			if(pDB->pFields[nKeyFieldIndex]->cFieldType=='N'){
				sprintf(szKeyFieldData,"%0d",atoi(szKeyFieldData));
			}
			strcat (szUpdateSQLCmd, szKeyFieldData);

			if(pDB->pFields[nKeyFieldIndex]->cFieldType=='C'){
				strcat (szUpdateSQLCmd, "\'");
			}
		}
		if (bUpdate){
			strcat (szUpdateSQLCmd, ";");
		}
		strcat (szInsertSQLCmd, ");");

		if (bUpdate){
			/*
			* con bUpdate=TRUE -> se PQcmdTuples ritorna 1 
			* - la riga era gia'presente ed e' stata aggiornata;
			* - altrimenti significa che la riga non e' presente e va inserita
			*/
			PGRes =  PQexec(DBConn, szUpdateSQLCmd);
			if (!PGRes || PQresultStatus(PGRes) != PGRES_COMMAND_OK ) {
				fprintf (stderr, "\nUPDATE TABLE %s: COMMAND FAILED\n",pDB->szTableName);
				fprintf (stderr, PQerrorMessage(DBConn));
			}
			/*
			* Update fallito a causa di record non resente (lo inserisco)
			*/
			if (atoi(PQcmdTuples(PGRes))==0){
				PGRes = PQexec(DBConn, szInsertSQLCmd);
			}
			if (!PGRes || PQresultStatus(PGRes) != PGRES_COMMAND_OK) {
				fprintf (stderr, "\nINSERT IN TABLE %s: COMMAND FAILED\n",pDB->szTableName);
				fprintf (stderr, PQerrorMessage(DBConn));
			}
		} else {
			/*
			* non sono nella condizione di update
			* -> inserisco direttamente
			*/
			PGRes = PQexec(DBConn, szInsertSQLCmd);
			if (!PGRes || PQresultStatus(PGRes) != PGRES_COMMAND_OK) {
				fprintf (stderr, "\nINSERT IN TABLE %s: COMMAND FAILED\n",pDB->szTableName);
				fprintf (stderr, PQerrorMessage(DBConn));
			}
		}

		PQclear(PGRes);
		return TRUE;
	} else {
		return FALSE;
	}
}

void ChangeTextColor(GtkWidget *w,GdkColor *Color)
{
	GtkStyle *style;

	/* set foreground and text to color */
	style = gtk_style_copy(gtk_widget_get_style(w));
	gtk_style_ref(style);
	style->text[GTK_STATE_NORMAL] = *Color;
	style->fg[GTK_STATE_NORMAL] = *Color;
	gtk_widget_set_style(w, style);
	gtk_style_unref(style);
}

unsigned long CryptString(char *pszString)
{
	int   nLen=strlen(pszString);
	int   nIndex;
	unsigned long ulNum=0;


	for( nIndex=0; *pszString!='\0'; nIndex++ ){
		ulNum += (*pszString*(*pszString+nLen)*(nIndex+1));
		++pszString;
	}

	return(ulNum);
} /*  Fine CryptString */

/*
* RunSimpleProcess()
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
				trace_out_vstr(1,"Creazione coda msg %s (%d) fallita",
					ProcList[nProcID].szProcName,
					ProcList[nProcID].nQKey);
#endif
			}
		}

		sprintf(szProcName,"%s/%s",Cfg.szPathExe,ProcList[nProcID].szProcName);
		ProcList[nProcID].nProcId = ExecModule(szProcName, szProcName,NULL);

		if (ProcList[nProcID].nProcId < 0) {
#ifdef TRACE
			trace_out_vstr(1,"Fallita esecuzione modulo %s",
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




