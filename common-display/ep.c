/*
* Modulo : ep.c
* -----------------------
* Funzioni di utilità per Easy Picking
*
* Data creazione : Fri May 31 17:33:07 CEST 2002
*
* Autore : Roberto Mantovani
*
* Copyright SMD srl 2002
*/

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <glib.h>
#if defined(TRACE) || defined(TRACE_SQL)
	#include <trace.h>
#endif

#include <pmx_msq.h>

#include <picking.h>
#include <ep.h>


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
	strcpy(szTimeString,szFromTime);			/* ora */
	szTimeString[2]='\0';
	strcat(szTimeString,":");
	strcat(szTimeString,szFromTime+2);	/* minuti */
	szTimeString[5]='\0';
	strcat(szTimeString,":");
	strcat(szTimeString,szFromTime+4);	/* secondi */
	szTimeString[8]='\0';

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
* GetTimeSpanHHMMSS(char *szToTime, char *szFromTime, char *szTimeString);
*/
char *GetTimeSpanHHMMSS(const char *szFromTime, const char* szToTime, char *szTimeString)
{
	int nTimeStart,nTimeEnd,nTimeSpan;
	int nOre,nMin,nSec;
	char szToOre[20],szToMin[20],szToSec[20];
	char szFromOre[20],szFromMin[20],szFromSec[20];

	szTimeString[0]='\0';

	if (strlen(szToTime)!=6 && strlen(szFromTime)!=6){
		return szTimeString;
	} else {
		sprintf(szFromSec,szFromTime+4);
		szFromSec[2]='\0';
		sprintf(szFromMin,szFromTime+2);
		szFromMin[2]='\0';
		sprintf(szFromOre,szFromTime);
		szFromOre[2]='\0';

		sprintf(szToSec,szToTime+4);
		szToSec[2]='\0';
		sprintf(szToMin,szToTime+2);
		szToMin[2]='\0';
		sprintf(szToOre,szToTime);
		szToOre[2]='\0';

		nTimeStart=(atoi(szFromSec) + (atoi(szFromMin)*60) + (atoi(szFromOre)*3600) );
		nTimeEnd=(atoi(szToSec) + (atoi(szToMin)*60) + (atoi(szToOre)*3600) );
		nTimeSpan=nTimeEnd-nTimeStart;

		nOre=nTimeSpan/3600;	
		nMin=(nTimeSpan%3600)/60;
		nSec=(nTimeSpan%3600)%60;

		sprintf(szTimeString,"%02d%02d%02d",nOre,nMin,nSec);

		return szTimeString;
	}
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
		szDateString[0]='\0';
	}
	strcpy(szDateString,szFromDate+6);	/* giorno */
	strcat(szDateString,"-");
	strcat(szDateString,szFromDate+4);	/* mese */
	szDateString[5]='\0';
	strcat(szDateString,"-");
	strcat(szDateString,szFromDate+2);	/* anno */
	szDateString[8]='\0';

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
		return "";
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

/*
* GetDateYYYYMMDDFromDD_MM_YYYY()
* get date YYYYMMDD from a string formatted DD-MM-YYYY
*/
char *GetDateYYYYMMDDFromDD_MM_YYYY(char *szFromDate,char *szDateString)
{

	if(strlen(szFromDate)!=10){
		szDateString[0]='\0';
	}
	strcpy(szDateString,szFromDate+6); /* anno */
	szDateString[4]='\0';	
	strcat(szDateString,szFromDate+3);	/* mese */
	szDateString[6]='\0';
	strcat(szDateString,szFromDate);	/* giorno */
	szDateString[8]='\0';

	return szDateString;
}

/*
* GetDateYYYYMMDDFromYYYY_MM_DD()
* get date YYYYMMDD from a string formatted YYYY-MM-DD
*/
char *GetDateYYYYMMDDFromYYYY_MM_DD(char *szFromDate,char *szDateString)
{

	if(strlen(szFromDate)!=10){
		szDateString[0]='\0';
	}
	strcpy(szDateString,szFromDate); /* anno */
	szDateString[4]='\0';	
	strcat(szDateString,szFromDate+5);	/* mese */
	szDateString[6]='\0';
	strcat(szDateString,szFromDate+8);	/* giorno */
	szDateString[8]='\0';

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
	nPos=min(nPos,(int)strlen(szString)-nLen);

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
* LeftPad()
* mette gli spazi in TESTA alla stringa passata in ingresso
* ritorna la stringa passata in ingresso
*/
char *LeftPad(char *szString,int nLen)
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
* CmpFileDate()
* Confronta la data del file in oggetto con quella attuale.
* Ritorna :
*  0 : Data odierna
*  1 : Data maggiore della data odierna
* -1 : Data minore della data odierna
* -2 : file non esistente (verificare errno)
*/
int CmpFileDate(char *pszFileName)
{
	struct stat st;
	struct tm *ftime;
	char szFileDate[128];
	char szActualDate[128];

	GetDateYYYYMMDD(time((long *)0), szActualDate);

	/* 
	* ricavo la data del file 
	*/
	if(!stat(pszFileName,&st)){
		ftime = localtime(&(st.st_mtime));
		sprintf(szFileDate,"%4.4d%2.2d%2.2d", 1900+ftime->tm_year,ftime->tm_mon+1,ftime->tm_mday);
		return(strcmp(szFileDate,szActualDate));
	}
	return -2;
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
* StrIsAlpha()
*/
BOOL StrIsAlpha(char *str)
{
	int nIndex=0;

	while(str[nIndex]){
		if(!isalpha(str[nIndex]))
			return FALSE;
		nIndex++;
	}
	return TRUE;
}

/*
* StrIsDigit()
*/
BOOL StrIsDigit(char *str)
{
	int nIndex=0;

	while(str[nIndex]){
		if(!isdigit(str[nIndex]))
			return FALSE;
		nIndex++;
	}
	return TRUE;
}

/*
* StrIsAlnum()
*/
BOOL StrIsAlnum(char *str)
{
	int nIndex=0;

	while(str[nIndex]){
		if(!isalnum(str[nIndex]))
			return FALSE;
		nIndex++;
	}
	return TRUE;
}

/*
* StrIsSpace()
*/
BOOL StrIsSpace(char *str)
{
	int nIndex=0;

	while(str[nIndex]){
		if(!isspace(str[nIndex]))
			return FALSE;
		nIndex++;
	}
	return TRUE;
}

/*
* StrIsLower()
*/
BOOL StrIsLower(char *str)
{
	int nIndex=0;

	while(str[nIndex]){
		if(!islower(str[nIndex]))
			return FALSE;
		nIndex++;
	}
	return TRUE;
}


/*
* StrIsUpper()
*/
BOOL StrIsUpper(char *str)
{
	int nIndex=0;

	while(str[nIndex]){
		if(!isupper(str[nIndex]))
			return FALSE;
		nIndex++;
	}
	return TRUE;
}


/*
* StrIsXdigit()
*/
BOOL StrIsXdigit(char *str)
{
	int nIndex=0;

	while(str[nIndex]){
		if(!isxdigit(str[nIndex]))
			return FALSE;
		nIndex++;
	}
	return TRUE;
}


/*
* StrIsAscii()
*/
BOOL StrIsAscii(char *str)
{
	int nIndex=0;

	while(str[nIndex]){
		if(!isascii(str[nIndex]))
			return FALSE;
		nIndex++;
	}
	return TRUE;
}

/*
* Sostituzione di caretteri
*/
char *Substitute(char *szString,char c1,char c2)
{
	char *pPtr;

	if(c1==c2){
		return szString;
	}

	/*
	* sostituzione di (c1 )
	*/ 
	while ((pPtr=strchr(szString,c1))){
		*pPtr = c2;
	}
	return szString;
}

/*
* RunSimpleProcess()
* lancio il processo nProcID
*/
BOOL RunSimpleProcess(PROCESSO *ProcList,int nProcID,char *szPath)
{
	char szProcName[128];
	char szPID[128];

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

		sprintf(szProcName,"%s/%s",szPath,ProcList[nProcID].szProcName);
		sprintf(szPID,"-p %d",nProcID);
		ProcList[nProcID].nProcId = ExecModule(szProcName, szProcName,szPID,NULL);

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

void TerminateProcess(PROCESSO *ProcList,int nProcID)
{
	if(ProcList[nProcID].bExecute){
		kill((pid_t)ProcList[nProcID].nProcId, SIGTERM);
		if(ProcList[nProcID].nQKey){
			DeleteMsgQ(ProcList[nProcID].nQNumber);
		}
	}
}


/*
* ReadProcInfo()
* Lettura dei parametri relativi ai processi 
*/
void ReadProcInfo(PROCESSO *ProcList,char *szFileConfig)
{
	char szProcIndex[128];
	char szProcBuffer[128];
	int nIndex;
	BOOL bOK=TRUE;

	nIndex=0;
	while(bOK){
		sprintf(szProcIndex,"Proc_%02d",nIndex);
		if(GetFileString("procinfo",szProcIndex, "", szProcBuffer, 80,szFileConfig,NULL)){
			sscanf(szProcBuffer, "%[^,],%d,%d,%d",
					ProcList[nIndex].szProcName,
					&ProcList[nIndex].nQKey,
					&ProcList[nIndex].nPriority,
					&ProcList[nIndex].bExecute);
			ProcList[nIndex].nGId=nIndex+1;
			/* 
			* rm 16-01-2002 : Elimino gli spazi in testa e in coda per 
			*                 problemi con la RunSimpleProcess() 
			*/
			StrTrimAll(ProcList[nIndex].szProcName);

			/*
			* resetto il numero della coda messaggi del processo
			*/
			ProcList[nIndex].nQNumber=0;
		} else {
			ProcList[nIndex].szProcName[0]='\0';
			ProcList[nIndex].nQKey=0;
			ProcList[nIndex].nPriority=0;
			ProcList[nIndex].bExecute=0;
			ProcList[nIndex].nGId=0;
			bOK=FALSE;
		}
		nIndex++;
	}
}

/*
* SendMessage()
* Spedizione messaggio con timeout e tentativi (NON BLOCCANTE)
*/
BOOL SendMessage(PROCESSO *ProcList,int nDest,int nSrce,int nMsgCode,char *szText)
{
	int nRC;

	if((nRC=SendSpecificMsg(ProcList[nDest].nQNumber, ProcList[nDest].nGId, ProcList[nSrce].nGId, nMsgCode, szText))!=0){
		trace_out_vstr_date(1,"Errore [%d] in spedizione Messaggio a [%s]", nRC,ProcList[nDest].szProcName);
		return FALSE;
	}
	return TRUE;
}



/*
 *	****************************************************************************
 *
 *	Funzione GetFileBuffer
 *
 *	Legge il file in ingresso e ne deposita il contenuto in un buffer allocato
 *	dinamicamente.
 *
 *	char	*pszFile	-	nome del file,
 *	FILE	*pFile		-	puntatore al file.
 *
 *	Se si passa il puntatore al file viene usato questo e quindi non e'
 *	necessario passare anche il nome del file.
 *
 *	Ritorna:
 *		il puntatore al buffer allocato (bisogna fare una free quando non si
 *		usa piu').
 *
 *	****************************************************************************
 */
char *GetFileBuffer(char *pszFile, FILE *pFile)
{
	int			nRead;
	char		*pszFileBuffer=NULL;
	long		lFileSize;
	FILE		*fp;
	BOOL		bMyOpen=FALSE;
	struct stat	FileStat;


	if( pFile!=NULL )
	{
		/*
		 *	Puntatore al file gia' aperto
		 */
		fp=pFile;
		bMyOpen=FALSE;

		fseek(fp, 0, 0);
	}
	else
	{
		/*
		 *	Apro il file per conto mio
		 */
		if( (fp=fopen(pszFile, "r"))==NULL )
		{
			printf("File %s not found\r\n", pszFile);
			return(NULL);
		}
		bMyOpen=TRUE;
	}

	/*
	 *	Ricavo la dimensione del file.
	 */
	fstat(fileno(fp), &FileStat);
	lFileSize=FileStat.st_size;

	if( lFileSize>0 )
	{
		/*
		 *	Alloco il buffer necessario (+1 per il \0)
		 */
		if( (pszFileBuffer=malloc(lFileSize+1))==NULL )
		{

			if( bMyOpen )
				fclose(fp);
			return(NULL);
		}

		/*
		 *	Leggo il file ed aggiungo un \0 in fondo.
		 */
		nRead=fread(pszFileBuffer, sizeof(char), (int)lFileSize, fp);
		pszFileBuffer[lFileSize]='\0';

		if( nRead!=(int)lFileSize )
		{
			free(pszFileBuffer);

			if( bMyOpen )
				fclose(fp);
			return(NULL);
		}
	}

	if( bMyOpen )
		fclose(fp);

	return(pszFileBuffer);
}	/*	Fine GetFileBuffer */


/*
 *	****************************************************************************
 *
 *	Funzione GetBufferParagraph
 *
 *	Cerca il paragrafo pszParagraph all'interno del buffer del file e ne ritorna
 *	il puntatore.
 *
 *	char	*pszFile		-	nome del file per printf di errore,
 *	char	*pszParagraph	-	[PARAGRAFO],
 *	char	*pszFileBuffer	-	buffer del file.
 *
 *	Ritorna:
 *		puntatore al paragrafo nel buffer del file
 *
 *	****************************************************************************
 */
char *GetBufferParagraph( char	*pszFile, char	*pszParagraph, char	*pszFileBuffer)
{
	char	*pszPara;
	char	szPara[128];


	if( pszFileBuffer==NULL )
	{
		/*
		 *	ERRORE puntatore a file nullo
		 */
		return(NULL);
	}

	sprintf(szPara, "[%s]", pszParagraph);

	if( (pszPara=strstr(pszFileBuffer, szPara))!=NULL )
	{
		/*
		 *	Paragrafo trovato.
		 *
		 *	19.06.95	MF	aggiunta la somma di strlen
		 */
		return(pszPara+strlen(szPara));
	}

	/*
	 *	Debug solo se necessario
	 */
	if( pszFile!=NULL )
		printf("file %s - paragraph %s not found\r\n", pszFile, pszParagraph);

	/*
	 *	Paragrafo non trovato
	 */
	return(NULL);
}	/*	Fine GetBufferParagraph */


/*
 *	****************************************************************************
 *
 *	Funzione GetBufferString
 *
 *	Riempie pszBufDep con il valore a destra dell'= di pszItem nel paragrafo
 *	pszParagraph letto da pszFileBuffer.
 *	La ricerca viene iniziata dal puntatore pszParagraph.
 *
 *	char	*pszParagraph	-	puntatore al [PARAGRAFO] nel buffer
 *	char	*pszItem		-	DATO=
 *	char	*pszDefault		-	stringa di default,
 *	char	*pszBufDep		-	deposito della stringa (dato=STRINGA)
 *	int		nLen			-	lunghezza buffer di deposito (escluso \0)
 *	char	*pszFileBuffer	-	buffer del file
 *
 *	Ritorna:
 *		TRUE	-	OK
 *		FALSE	-	errore.
 *
 *	****************************************************************************
 */
BOOL GetBufferString( char	*pszParagraph, char	*pszItem, char	*pszDefault, char	*pszBufDep, int		nLen, char	*pszFileBuffer)
{
	int		nLunghString;
	char	*pszTmp=NULL;
	char	*pszNewLine=NULL;
	char	szElem[128];


	/*
	 *	Per prima cosa metto il default
	 */
	strcpy(pszBufDep, pszDefault);

	if( pszFileBuffer==NULL || pszParagraph==NULL )
		return(FALSE);

	sprintf(szElem, "%s=", pszItem);

	pszTmp=pszParagraph;

	/*
	 *	Loop sulla ricerca del LineFeed
	 */
	while( *pszTmp!='\0' && (pszNewLine=strchr(pszTmp, CHAR_LF))!=NULL )
	{
		++pszNewLine;

		/*
		 *	Salto l'eventuale CarriageReturn
		 */
		if( *pszNewLine==CHAR_CR )
			++pszNewLine;

		switch( *pszNewLine )
		{
			case '[':
				/*
				 *	Il paragrafo che volevo era gia' stato trovato e quindi,
				 *	avendone trovato un altro, ho finito la ricerca.
				 */
				return(FALSE);
			break;

			default:
				/*
				 *	Cerco il <dato>=
				 */
				if( memcmp(pszNewLine, szElem, strlen(szElem))==0 )
				{
					/*
					 *	Trovato!
					 */
					pszTmp=pszNewLine+strlen(szElem);

					/*
					 *	Cerco l'eventuale 0A e calcolo quanti caratteri copiare
					 */
					if( (pszNewLine=strchr(pszTmp, CHAR_LF))!=NULL )
						nLunghString=min(pszNewLine-pszTmp, nLen);
					else
						nLunghString=min((int)strlen(pszTmp), nLen);

					memcpy(pszBufDep, pszTmp, nLunghString);
					pszBufDep[nLunghString]='\0';

					return(TRUE);
				}
			break;
		}

		pszTmp=pszNewLine;
	}

	return(FALSE);
}	/*	Fine GetBufferString */


/*
 *	****************************************************************************
 *
 *	Funzione GetBufferInt
 *
 *	Riempie pszBufDep con il valore a destra dell'= di pszItem nel paragrafo
 *	pszParagraph letto da pszFileBuffer.
 *	La ricerca viene iniziata dal puntatore pszParagraph.
 *
 *	char	*pszParagraph	-	puntatore al [PARAGRAFO] nel buffer
 *	char	*pszItem		-	DATO=
 *	int		nDefault		-	valore di default,
 *	char	*pszFileBuffer	-	buffer del file
 *
 *	Ritorna:
 *		il numero desiderato.
 *
 *	****************************************************************************
 */
int GetBufferInt( char	*pszParagraph, char	*pszItem, int		nDefault, char	*pszFileBuffer)
{
	char	szBuffer[80];
	BOOL	bRC;

	bRC=GetBufferString(
		pszParagraph,
		pszItem,
		"",
		szBuffer,
		sizeof(szBuffer)-1,
		pszFileBuffer
	);

	/*
	 *	Con errore ritorno il default
	 */
	if( !bRC )
		return(nDefault);

	return(atoi(szBuffer));
}	/*	Fine GetBufferInt */


/*
 *	****************************************************************************
 *
 *	Funzione GetFileString
 *
 *	Riempie pszBufDep con il valore a destra dell'= di pszItem nel paragrafo
 *	pszParagraph letto da pFile.
 *
 *	char	*pszParagraph	-	[PARAGRAFO]
 *	char	*pszItem		-	DATO=
 *	char	*pszDefault		-	valore di default con cui riempire pszBufDep
 *	char	*pszBufDep		-	deposito della stringa (dato=STRINGA)
 *	int		nLen			-	lunghezza buffer di deposito (escluso \0)
 *	char	*pszFile		-	nomefile
 *	FILE	*pFile			-	puntatore a file gia' aperto (lettura o
 *								modifica).
 *
 *	E' possibile passare in ingresso il nome del file da aprire o il
 *	puntatore
 *	al file aperto.
 *	Se il puntatore al file e' diverso da NULL il nome del file puo' essere
 *	NULL, ma se il puntatore al file e' NULL il nome del file non deve esser
 *	NULL.
 *
 *	Ritorna:
 *		TRUE	-	OK
 *		FALSE	-	errore.
 *
 *	****************************************************************************
 */
BOOL GetFileString( char	*pszParagraph, char	*pszItem, char	*pszDefault, char	*pszBufDep, int		nLen, char	*pszFile, FILE	*pFile)
{
	int		nLunghString;
	char	*pszTmp;
	char	szPara[128];
	char	szElem[128];
	char	szBuffer[4096];
	FILE	*fp;
	BOOL	bPara=FALSE;
	BOOL	bMyOpen=FALSE;


	sprintf(szPara, "[%s]", pszParagraph);
	sprintf(szElem, "%s=", pszItem);
	strcpy(pszBufDep, pszDefault);

#ifdef CASE_INSENSITIVE
	SdmStrlwr(szPara);
	SdmStrlwr(szElem);
#endif

	if( pFile!=NULL )
	{
		/*
		 *	Puntatore a file gia' aperto
		 */
		fp=pFile;
		bMyOpen=FALSE;

		fseek(fp, 0, 0);
	}
	else
	{
		/*
		 *	Apro il file per conto mio
		 */
		if( (fp=fopen(pszFile, "r"))==NULL )
		{
			printf("File %s not found\r\n", pszFile);
			return(FALSE);
		}
		bMyOpen=TRUE;
	}

	/*
	 *	Loop di lettura riga x riga del file
	 */
	while( fgets(szBuffer, sizeof(szBuffer), fp)!=NULL )
	{
		if(
			(pszTmp=strchr(szBuffer, CHAR_CR))!=NULL ||
			(pszTmp=strchr(szBuffer, CHAR_LF))!=NULL
		)
		{
			*pszTmp='\0';
		}

		if( *szBuffer=='[' )
		{
			/*
			 *	E' un paragrafo
			 */

#ifdef CASE_INSENSITIVE
			/*
			 *	Controllo il minuscolo del paragrafo
			 */
			SdmStrlwr(szBuffer);
#endif

			if( strcmp(szPara, szBuffer)==0 )
			{
				/*
				 *	Trovato il paragrafo ricercato.
				 */
				bPara=TRUE;
			}
			else
			{
				if( bPara )
				{
					/*
					 *	Il paragrafo che volevo era gia' stato trovato e quindi,
					 *	avendone trovato un altro, ho finito la ricerca.
					 */
					if( bMyOpen )
						fclose(fp);
					return(FALSE);
				}
			}
		}
		else
		{
			/*
			 *	Non e' un paragrafo e quindi sara' un elemento
			 */
			if( bPara && (pszTmp=strchr(szBuffer, '='))!=NULL )
			{
#ifdef CASE_INSENSITIVE
				/*
				 *	I controlli vengono fatti tutti sul minuscolo
				 */
				SdmStrlwr(szBuffer);
#endif

				/*
				 *	Il paragrafo corrisponde, l'uguale e' stato trovato,
				 *	controllo che sia effettivamente quello che cerco.
				 */
				if( memcmp(szElem, szBuffer, strlen(szElem))==0 )
				{
					/*
					 *	Trovato!!
					 */
					++pszTmp;
					nLunghString=min((int)strlen(pszTmp), nLen);
					memcpy(pszBufDep, pszTmp, nLunghString);
					pszBufDep[nLunghString]='\0';

					if( bMyOpen )
						fclose(fp);
					return(TRUE);
				}
			}
		}
	}

	if( bMyOpen )
		fclose(fp);

	return(FALSE);
}	/*	Fine GetFileString */


/*
 *	****************************************************************************
 *
 *	Funzione GetFileInt
 *
 *	Ritorna l'intero a destra dell'= dell'elemento letto da file nel
 *	paragrafo specificato.
 *
 *	char	*pszParagraph	-	[PARAGRAFO]
 *	char	*pszItem		-	DATO=
 *	int		nDefault		-	valore di default da ritornare
 *	char	*szFile			-	nomefile
 *	FILE	*pFile			-	puntatore a file gia' aperto (lettura o
 *								modifica).
 *
 *	E' possibile passare in ingresso il nome del file da aprire o il puntatore
 *	al file aperto.
 *	Se il puntatore al file e' diverso da NULL il nome del file puo' essere
 *	NULL, ma se il puntatore al file e' NULL il nome del file non deve essere
 *	NULL.
 *
 *	Ritorna:
 *		il valore letto
 *
 *	****************************************************************************
 */
int GetFileInt( char	*pszParagraph, char	*pszItem, int		nDefault, char	*pszFile, FILE	*pFile)
{
	char	szBuffer[80];
	BOOL	bRC;

	bRC=GetFileString( pszParagraph, pszItem, "", szBuffer, sizeof(szBuffer)-1, pszFile, pFile);

	/*
	 *	Con errore ritorno il default
	 */
	if( !bRC )
		return(nDefault);

	return(atoi(szBuffer));
}	/*	Fine GetFileInt */

/*
 *	****************************************************************************
 *
 *	Funzione PutFileString
 *
 *	Scrive nel paragrafo specificato elemento=valore
 *
 *	char	*pszParagraph	-	[PARAGRAFO]
 *	char	*pszItem		-	DATO=
 *	char	*pszValue		-	dato=VALORE
 *	char	*szFile			-	nomefile
 *	FILE	*pFile			-	puntatore a file gia' aperto in modifica (r+).
 *
 *	E' possibile passare in ingresso il nome del file da aprire o il puntatore
 *	al file aperto.
 *	Se il puntatore al file e' diverso da NULL il nome del file puo' essere
 *	NULL, ma se il puntatore al file e' NULL il nome del file non deve essere
 *	NULL.
 *
 *	Ritorna:
 *		TRUE	-	OK
 *		FALSE	-	errore.
 *
 *	****************************************************************************
 */
BOOL PutFileString( char	*pszParagraph, char	*pszItem, char	*pszValue, char	*pszFile, FILE	*pFile)
{
	int			nPos;
	int			nRead;
	int			nOldLen=0;
	int			nLenElemVal;
	char		*pszFileBuffer=NULL;
	char		*pszTmp=NULL;
	char		*pszNewLine=NULL;
	char		szPara[128];
	char		szElem[128];
	char		szElemVal[128];
	long		lFileSize;
	FILE		*fp=NULL;
	BOOL		bMyOpen=FALSE;
	BOOL		bFound=FALSE;
	struct stat	FileStat;


	sprintf(szPara, "[%s]", pszParagraph);
	sprintf(szElem, "%s=", pszItem);
	sprintf(szElemVal, "%s%s", szElem, pszValue);

	nLenElemVal=strlen(szElemVal);

	if( pFile!=NULL )
	{
		/*
		 *	Utilizzo il puntatore al file gia' aperto
		 */
		fp=pFile;
		bMyOpen=FALSE;

		fseek(fp, 0, 0);
	}
	else
	{
		/*
		 *	Apro il file per conto mio
		 */
		if( (fp=fopen(pszFile, "r+"))==NULL )
		{
			/*
			 *	File non trovato, lo apro in scrittura
			 */
			if( (fp=fopen(pszFile, "w"))==NULL )
			{
				return(FALSE);
			}
		}
		bMyOpen=TRUE;
	}

	/*
	 *	Ricavo la dimensione del file
	 */
	fstat(fileno(fp), &FileStat);
	lFileSize=FileStat.st_size;

	if( lFileSize>0 )
	{
		/*
		 *	Alloco il buffer (+1 per il \0)
		 */
		if( (pszFileBuffer=malloc(lFileSize+1))==NULL )
		{
			if( bMyOpen )
				fclose(fp);
			return(FALSE);
		}

		/*
		 *	Leggo il file e metto il \0 finale
		 */
		nRead=fread(pszFileBuffer, sizeof(char), (int)lFileSize, fp);
		pszFileBuffer[lFileSize]='\0';

		if( nRead!=(int)lFileSize )
		{
			free(pszFileBuffer);

			if( bMyOpen )
				fclose(fp);
			return(FALSE);
		}
	}

	/*
	 *	La dimensione effettiva del file non e' quella letta ma la lunghezza
	 *	del buffer perche' in caso di cancellazione vengono messi degli zeri
	 *	in fondo che fanno dei casini.
	 */
	lFileSize=strlen(pszFileBuffer);

	if( lFileSize>0 && (pszTmp=strstr(pszFileBuffer, szPara))!=NULL )
	{
		/*
		 *	Paragrafo trovato.
		 */

		/*
		 *	Loop sulla ricerca del LineFeed
		 */
		while(
			!bFound &&
			pszTmp-pszFileBuffer<=lFileSize &&
			(pszNewLine=strchr(pszTmp, CHAR_LF))!=NULL
		)
		{
			++pszNewLine;

			/*
			 *	Salto l'eventuale CarriageReturn
			 */
			if( *pszNewLine==CHAR_CR )
				++pszNewLine;

			switch( *pszNewLine )
			{
				case '[':
					/*
					 *	Nuovo paragrafo -> ho finito la ricerca
					 */
					bFound=TRUE;
				break;

				default:
					if( isprint(*pszNewLine) )
					{
						/*
						 *	Cerco il <dato>=
						 */
						if( memcmp(pszNewLine, szElem, strlen(szElem))==0 )
						{
							bFound=TRUE;

							/*
							 *	Controllo <dato>=<value>
							 */
							if(
								memcmp(pszNewLine, szElemVal, nLenElemVal)==0 &&
								(*(pszNewLine+nLenElemVal)==CHAR_CR ||
								*(pszNewLine+nLenElemVal)==CHAR_LF)
							)
							{
								/*
								 *	Trovato dato e valore uguali nello stesso
								 *	paragrafo (DATO=VAL).
								 */
								if( bMyOpen )
									fclose(fp);
								free(pszFileBuffer);

								return(TRUE);
							}
						}
					}
				break;
			}

			pszTmp=pszNewLine;
		}

		if( pszNewLine==NULL )
			pszNewLine=pszFileBuffer+lFileSize;

		/*
		 *	Calcolo l'offset per FSEEK
		 */
		nPos=min((int)(pszNewLine-pszFileBuffer), (int)lFileSize);

		/*
		 *	Mi posiziono e scrivo il dato sul file
		 */
		fseek(fp, nPos, 0);
		fputs(szElemVal, fp);
		fputs("\n", fp);

		if( nPos<(int)lFileSize )
		{
			/*
			 *	Ci sono altri caratteri da accodare
			 */
			if( memcmp(pszNewLine, szElem, strlen(szElem))==0 )
			{
				char *pszOldVal;

				/*
				 *	Sono fermo sul vecchio dato
				 */
				pszOldVal=strchr(pszNewLine, CHAR_LF);

				if( pszOldVal!=NULL ){
					/*
					* Ricavo la lunghezza del dato precedente
					*/
					nOldLen=pszOldVal-pszNewLine;

					++pszOldVal;
				}
				pszNewLine=pszOldVal;
			}

			if( pszNewLine!=NULL )
			{
				fwrite(
					pszNewLine,
					sizeof(char),
					lFileSize-(pszNewLine-pszFileBuffer),
					fp
				);

				/*
				* rm + mf 04-12-2001 : cambio la dimensione del file per evitare 'sporcature in fondo'
				*/
				if( nOldLen>nLenElemVal ){
					ftruncate(fileno(fp),lFileSize-(nOldLen-nLenElemVal));
				}
			}
		}
	}
	else
	{
		/*
		 *	Paragrafo non trovato, accodo il tutto.
		 */
		fseek(fp, lFileSize, 0);

		fputs("\n", fp);
		fputs(szPara, fp);
		fputs("\n", fp);
		fputs(szElemVal, fp);
		fputs("\n", fp);
	}

	if( bMyOpen )
		fclose(fp);
	else
		fflush(fp);

	free(pszFileBuffer);

	return(TRUE);
}	/*	Fine PutFileString */


/*
 *	****************************************************************************
 *
 *	Funzione PutFileInt
 *
 *	Scrive nel paragrafo specificato elemento=numero intero.
 *
 *	char	*pszParagraph	-	[PARAGRAFO]
 *	char	*pszItem		-	DATO=
 *	int		nValue			-	dato=VALORE
 *	char	*szFile			-	nomefile
 *	FILE	*pFile			-	puntatore a file gia' aperto in modifica (r+).
 *
 *	E' possibile passare in ingresso il nome del file da aprire o il puntatore
 *	al file aperto.
 *	Se il puntatore al file e' diverso da NULL il nome del file puo' essere
 *	NULL, ma se il puntatore al file e' NULL il nome del file non deve essere
 *	NULL.
 *
 *	Ritorna:
 *		TRUE	-	OK
 *		FALSE	-	errore.
 *
 *	****************************************************************************
 */
BOOL PutFileInt( char	*pszParagraph, char	*pszItem, int		nValue, char	*pszFile, FILE	*pFile)
{
	char	szBuffer[80];
	BOOL	bRC;

	sprintf(szBuffer, "%d", nValue);

	bRC=PutFileString(
		pszParagraph,
		pszItem,
		szBuffer,
		pszFile,
		pFile
	);

	return(bRC);
}	/*	Fine PutFileInt */


/*
 *	****************************************************************************
 *
 *	Funzione DelFileString
 *
 *	Cancella dal paragrafo specificato la stringa elemento=valore.
 *
 *	char	*pszParagraph	-	[PARAGRAFO]
 *	char	*pszItem		-	DATO=
 *	char	*szFile			-	nomefile
 *	FILE	*pFile			-	puntatore a file gia' aperto in modifica (r+).
 *
 *	E' possibile passare in ingresso il nome del file da aprire o il puntatore
 *	al file aperto.
 *	Se il puntatore al file e' diverso da NULL il nome del file puo' essere
 *	NULL, ma se il puntatore al file e' NULL il nome del file non deve essere
 *	NULL.
 *
 *	Ritorna:
 *		TRUE	-	OK
 *		FALSE	-	errore.
 *
 *	****************************************************************************
 */
BOOL DelFileString( char	*pszParagraph, char	*pszItem, char	*pszFile, FILE	*pFile)
{
	int			nPos;
	int			nRead;
	int			nIndex;
	char		*pszFileBuffer=NULL;
	char		*pszTmp=NULL;
	char		*pszNewLine=NULL;
	char		szPara[128];
	char		szElem[128];
	long		lFileSize;
	FILE		*fp=NULL;
	BOOL		bMyOpen=FALSE;
	BOOL		bFound=FALSE;
	struct stat	FileStat;


	sprintf(szPara, "[%s]", pszParagraph);
	sprintf(szElem, "%s=", pszItem);

	if( pFile!=NULL )
	{
		/*
		 *	Utilizzo il puntatore al file gia' aperto
		 */
		fp=pFile;
		bMyOpen=FALSE;

		fseek(fp, 0, 0);
	}
	else
	{
		/*
		 *	Apro il file per conto mio
		 */
		if( (fp=fopen(pszFile, "r+"))==NULL )
		{
			printf("File %s not found\r\n", pszFile);
			return(FALSE);
		}
		bMyOpen=TRUE;
	}

	/*
	 *	Ricavo la dimensione del file
	 */
	fstat(fileno(fp), &FileStat);
	lFileSize=FileStat.st_size;

	if( lFileSize<=0 )
	{
		/*
		 *	Il file e' gia' vuoto, cosa cancello?
		 */
		if( bMyOpen )
			fclose(fp);
		return(FALSE);
	}

	if( (pszFileBuffer=malloc(lFileSize+1))==NULL )
	{
		/*
		 *	Non ottengo la memoria che mi serve.
		 */
		if( bMyOpen )
			fclose(fp);
		return(FALSE);
	}

	/*
	 *	Leggo tutto il file e metto il barra-zero di sicurezza
	 */
	nRead=fread(pszFileBuffer, sizeof(char), (int)lFileSize, fp);
	pszFileBuffer[lFileSize]='\0';

	if( nRead!=(int)lFileSize )
	{
		free(pszFileBuffer);

		if( bMyOpen )
			fclose(fp);
		return(FALSE);
	}

	if( (pszTmp=strstr(pszFileBuffer, szPara))!=NULL )
	{
		/*
		 *	Paragrafo trovato.
		 */

		/*
		 *	Cerco il LineFeed
		 */
		while(
			!bFound &&
			pszTmp-pszFileBuffer<=lFileSize &&
			(pszNewLine=strchr(pszTmp, CHAR_LF))!=NULL
		)
		{
			++pszNewLine;

			/*
			 *	Salto l'eventuale CarriageReturn
			 */
			if( *pszNewLine==CHAR_CR )
				++pszNewLine;

			switch( *pszNewLine )
			{
				case '[':
					/*
					 *	E' finito il paragrafo perche' ne e' iniziato uno nuovo
					 *	e percio' ho finito la ricerca
					 */
					if( bMyOpen )
						fclose(fp);
					free(pszFileBuffer);

					return(FALSE);
				break;

				default:
					/*
					 *	Cerco il <DATO>=
					 */
					if( memcmp(pszNewLine, szElem, strlen(szElem))==0 )
						bFound=TRUE;
				break;
			}

			pszTmp=pszNewLine;
		}

		if( pszNewLine==NULL )
			pszNewLine=pszFileBuffer+lFileSize;

		nPos=min((int)(pszNewLine-pszFileBuffer), (int)lFileSize);

		if( nPos<(int)lFileSize )
		{
			/*
			 *	Ci sono altri caratteri da accodare
			 */
			fseek(fp, nPos, 0);

			/*
			 *	Sono fermo sul <dato>= e percio' vado al prossimo LF
			 */
			pszNewLine=strchr(pszNewLine, CHAR_LF);

			if( pszNewLine!=NULL )
			{
				++pszNewLine;

				/*
				 *	Salto l'eventuale CarriageReturn
				 */
				if( *pszNewLine==CHAR_CR )
					++pszNewLine;

				fwrite(
					pszNewLine,
					sizeof(char),
					lFileSize-(pszNewLine-pszFileBuffer)-1,
					fp
				);

				for(
					nIndex=0;
					nIndex<(pszNewLine-pszFileBuffer)-nPos;
					nIndex++
				)
				{
					/*
					 *	Scrivo zeri perche' non interferiscono con il file se
					 *	viene editato con un editor (e a me non da fastidio).
					 */
					fputc(0x00, fp);
				}
			}
		}
	}

	if( bMyOpen )
		fclose(fp);
	else
		fflush(fp);

	free(pszFileBuffer);

	return(TRUE);
}	/*	Fine DelFileString */


/*
 *	****************************************************************************
 *
 *	Funzione DelFileParagraph
 *
 *	Cancella il paragrafo specificato.
 *
 *	char	*pszParagraph	-	[PARAGRAFO]
 *	char	*szFile			-	nomefile
 *	FILE	*pFile			-	puntatore a file gia' aperto in modifica (r+).
 *
 *	E' possibile passare in ingresso il nome del file da aprire o il puntatore
 *	al file aperto.
 *	Se il puntatore al file e' diverso da NULL il nome del file puo' essere
 *	NULL, ma se il puntatore al file e' NULL il nome del file non deve essere
 *	NULL.
 *
 *	Ritorna:
 *		TRUE	-	OK
 *		FALSE	-	errore.
 *
 *	****************************************************************************
 */
BOOL DelFileParagraph(char *pszParagraph, char *pszFile, FILE *pFile)
{
	int			nPos;
	int			nRead;
	int			nIndex;
	char		*pszStartPara=NULL;
	char		*pszFileBuffer=NULL;
	char		*pszTmp=NULL;
	char		*pszNewLine=NULL;
	char		szPara[128];
	long		lFileSize;
	FILE		*fp=NULL;
	BOOL		bMyOpen=FALSE;
	BOOL		bFound=FALSE;
	struct stat	FileStat;


	sprintf(szPara, "[%s]", pszParagraph);

	if( pFile!=NULL )
	{
		/*
		 *	Utilizzo il puntatore al file gia' aperto
		 */
		fp=pFile;
		bMyOpen=FALSE;

		fseek(fp, 0, 0);
	}
	else
	{
		/*
		 *	Apro il file per conto mio
		 */
		if( (fp=fopen(pszFile, "r+"))==NULL )
		{
			printf("File %s not found\r\n", pszFile);
			return(FALSE);
		}
		bMyOpen=TRUE;
	}

	/*
	 *	Ricavo la dimensione del file
	 */
	fstat(fileno(fp), &FileStat);
	lFileSize=FileStat.st_size;

	if( lFileSize<=0 )
	{
		/*
		 *	Il file e' gia' vuoto, cosa cancello?
		 */
		if( bMyOpen )
			fclose(fp);
		return(FALSE);
	}

	if( (pszFileBuffer=malloc(lFileSize+1))==NULL )
	{
		/*
		 *	Non ottengo la memoria che mi serve.
		 */
		if( bMyOpen )
			fclose(fp);
		return(FALSE);
	}

	/*
	 *	Leggo tutto il file e metto il barra-zero di sicurezza
	 */
	nRead=fread(pszFileBuffer, sizeof(char), (int)lFileSize, fp);
	pszFileBuffer[lFileSize]='\0';

	if( nRead!=(int)lFileSize )
	{
		free(pszFileBuffer);

		if( bMyOpen )
			fclose(fp);
		return(FALSE);
	}

	if( (pszStartPara=strstr(pszFileBuffer, szPara))!=NULL )
	{
		/*
		 *	Paragrafo trovato.
		 */
		pszTmp=pszStartPara;

		/*
		 *	Cerco il LineFeed
		 */
		while(
			!bFound &&
			pszTmp-pszFileBuffer<=lFileSize &&
			(pszNewLine=strchr(pszTmp, CHAR_LF))!=NULL
		)
		{
			++pszNewLine;

			/*
			 *	Salto l'eventuale CarriageReturn
			 */
			if( *pszNewLine==CHAR_CR )
				++pszNewLine;

			switch( *pszNewLine )
			{
				case '[':
					/*
					 *	E' finito il paragrafo perche' ne e' iniziato uno nuovo
					 *	e percio' ho finito la ricerca
					 */
					bFound=TRUE;
				break;
			}

			pszTmp=pszNewLine;
		}

		if( pszNewLine==NULL )
			pszNewLine=pszFileBuffer+lFileSize;

		nPos=min((int)(pszStartPara-pszFileBuffer), (int)lFileSize);

		if( nPos<(int)lFileSize )
		{
			/*
			 *	Ci sono altri caratteri da accodare
			 */
			fseek(fp, nPos, 0);

			fwrite(
				pszNewLine,
				sizeof(char),
				lFileSize-(pszNewLine-pszFileBuffer)-1,
				fp
			);

			for(
				nIndex=0;
				nIndex<(pszNewLine-pszFileBuffer)-nPos;
				nIndex++
			)
			{
				/*
				 *	Scrivo zeri perche' non interferiscono con il file se
				 *	viene editato con un editor (e a me non da fastidio).
				 */
				fputc(0x00, fp);
			}
		}
	}

	if( bMyOpen )
		fclose(fp);
	else
		fflush(fp);

	free(pszFileBuffer);

	if( pszStartPara==NULL )
		return(FALSE);

	return(TRUE);
}	/*	Fine DelFileParagraph */


#ifdef PROVE_PER_SCRITTURA_SU_BUFFER
/*
 *	****************************************************************************
 *
 *	Funzione PutBufferString
 *
 *	Scrive nel paragrafo specificato elemento=valore
 *
 *	char	*pszParagraph	-	[PARAGRAFO]
 *	char	*pszItem		-	DATO=
 *	char	*pszValue		-	dato=VALORE
 *	char	*pszFileBuffer	-	buffer del file
 *
 *	E' possibile passare in ingresso il nome del file da aprire o il puntatore
 *	al file aperto.
 *	Se il puntatore al file e' diverso da NULL il nome del file puo' essere
 *	NULL, ma se il puntatore al file e' NULL il nome del file non deve essere
 *	NULL.
 *
 *	Ritorna:
 *		TRUE	-	OK
 *		FALSE	-	errore.
 *
 *	****************************************************************************
 */
char *PutBufferString(
	char	*pszParagraph,
	char	*pszItem,
	char	*pszValue,
	char	*pszFileBuffer
)
{
	char	*pszTmp=NULL;
	char	*pszNewLine=NULL;
	char	szPara[128];
	char	szElem[128];
	char	szElemVal[128];
	char	*pszNewFileBuffer=NULL;


	if( pszFileBuffer==NULL || pszParagraph==NULL || strlen(pszParagraph)<=0 )
	{
		return(NULL);
	}

	sprintf(szPara, "[%s]", pszParagraph);
	sprintf(szElem, "%s=", pszItem);
	sprintf(szElemVal, "%s%s", szElem, pszValue);

	if( (pszTmp=strstr(pszFileBuffer, szPara))!=NULL )
	{
		/*
		 *	Paragrafo trovato.
		 */

		/*
		 *	Loop sulla ricerca del LineFeed
		 */
		while( *pszTmp!='\0' && (pszNewLine=strchr(pszTmp, CHAR_LF))!=NULL )
		{
			++pszNewLine;
			/*
			 *	Salto l'eventuale CarriageReturn
			 */
			if( *pszNewLine==CHAR_CR )
				++pszNewLine;

			switch( *pszNewLine )
			{
				/*
				 *	Nuovo paragrafo -> ho finito la ricerca
				 */
				case '[':
				{
					char	*pszDove;

					pszDove=pszNewLine;

					/*
					 *	La realloc e' da modificare
					 */
					pszNewFileBuffer=realloc(pszFileBuffer, strlen(szElemVal));
					if( pszNewFileBuffer==NULL )
						return(NULL);

					pszFileBuffer=pszNewFileBuffer;

					memmove(
						pszDove+strlen(szElemVal),
						pszDove,
						strlen(szElemVal)
					);
					memcpy(pszDove, szElemVal, strlen(szElemVal));
				}
				break;

				default:
					/*
					 *	Linea vuota? (se non e' un carattere ascii)
					 */
					if( isprint(*pszNewLine) )
					{
						/*
						 *	Cerco il <dato>=
						 */
						if( memcmp(pszNewLine, szElem, strlen(szElem))==0 )
						{
							/*
							 *	Controllo <dato>=<value>
							 */
							if( memcmp(pszNewLine, szElemVal, strlen(szElemVal))!=0 )
							{
								int		nStart=strlen(szElem);
								int		nEnd=strlen(pszValue);
								int		nIndex;


								for(
									nIndex=0;
									*(pszNewLine+nStart+nIndex)!=BLANK &&
										isprint(*(pszNewLine+nStart+nIndex)) &&
											nIndex<nEnd;
									nIndex++
								)
								{
									*(pszNewLine+nStart+nIndex)=*(pszValue+nIndex);
								}

								if( nIndex<nEnd )
								{
									/*
									 *	La realloc e' da modificare
									 */
									pszNewFileBuffer=realloc(
										pszFileBuffer,
										nEnd-nIndex
									);
									if( pszNewFileBuffer==NULL )
										return(NULL);

									pszFileBuffer=pszNewFileBuffer;

									memmove(
										pszNewLine+nStart+nEnd,
										pszNewLine+nStart+nIndex,
										nEnd-nIndex
									);

									memcpy(
										pszNewLine+nStart+nIndex,
										pszValue+nIndex,
										nEnd-nIndex
									);
								}
							}

							return(pszFileBuffer);
						}
					}
				break;
			}

			pszTmp=pszNewLine;
		}
	}

	pszTmp=pszFileBuffer;
	while( *pszTmp!='\0' )
		++pszTmp;

	/*
	 *	La realloc e' da modificare
	 */
	pszNewFileBuffer=realloc(pszFileBuffer, strlen(szPara)+strlen(szElemVal)+3);
	if( pszNewFileBuffer==NULL )
		return(NULL);

	pszFileBuffer=pszNewFileBuffer;

	/*
	 *	Paragrafo non trovato, accodo il tutto.
	 */
	strcpy(pszTmp, "\n");
	strcpy(pszTmp+strlen(pszTmp), szPara);
	strcpy(pszTmp+strlen(pszTmp), "\n");
	strcpy(pszTmp+strlen(pszTmp), szElemVal);
	strcpy(pszTmp+strlen(pszTmp), "\n");

	return(pszFileBuffer);
}	/*	Fine PutBufferString */
#endif


/*
 *	****************************************************************************
 *
 *	Funzione PutFileBuffer
 *
 *	Scrive il file con il contenuto passato in ingresso.
 *
 *	char	*pszFile		-	nome del file
 *	char	*pszFileBuffer	-	buffer del file
 *	long	lSize			-	dimensione del buffer
 *
 *	Ritorna:
 *		TRUE	-	OK
 *		FALSE	-	errore.
 *
 *	****************************************************************************
 */
BOOL PutFileBuffer(char *pszFile, char *pszFileBuffer, long lSize)
{
	UINT	uWrite;
	FILE	*fp;

	/*
	 *	Apro il file per conto mio
	 */
	if( (fp=fopen(pszFile, "w"))==NULL )
	{
		return(FALSE);
	}

	uWrite=(UINT)fwrite(pszFileBuffer, sizeof(char), (UINT)lSize, fp);

	fclose(fp);

	if( uWrite!=(UINT)lSize )
		return(FALSE);

	return(TRUE);
}	/*	Fine PutFileBuffer */


/*
 *	****************************************************************************
 *
 *	Funzione BackupFile
 *
 *	Copia il file con l'estensione in piu' ".bak".
 *
 *	char	*pszFile		-	nome del file
 *
 *	Ritorna:
 *		TRUE	-	OK
 *		FALSE	-	errore.
 *
 *	****************************************************************************
 */
BOOL BackupFile(char *pszFile)
{
	char		*pszFileBuffer;
	char		*pszFileBak;
	BOOL		bRC;
	struct stat	FileStat;


	pszFileBak=malloc(strlen(pszFile)+strlen(BACK_EXTENSION));
	if( pszFileBak==NULL )
	{
		return(FALSE);
	}

	/*
	 *	Compongo il nome file .bak
	 */
	sprintf(pszFileBak, "%s%s", pszFile, BACK_EXTENSION);

	/*
	 *	Ottengo il buffer del file
	 */
	pszFileBuffer=GetFileBuffer(pszFile, NULL);
	if( pszFileBuffer==NULL )
	{
		free(pszFileBak);
		return(FALSE);
	}

	/*
	 *	Ricavo la dimensione del file.
	 */
	stat(pszFile, &FileStat);

	/*
	 *	Scrivo il nuovo file
	 */
	bRC=PutFileBuffer(pszFileBak, pszFileBuffer, (long)FileStat.st_size);

	free(pszFileBak);
	free(pszFileBuffer);

	return(bRC);
}	/*	Fine BackupFile */


BOOL ReadCfgItems(PCFGITEM pItems,char *pszCfgFileName)
{
	int nIndex=0;

	while(pItems[nIndex].pszItemName){
		switch(pItems[nIndex].ItemType){
			case CFG_TYPE_BOOL:
				*(pItems[nIndex].ItemValue.pbBool) = GetFileInt( pItems[nIndex].pszParagraph, pItems[nIndex].pszItemName, pItems[nIndex].ItemDefaultValue.nInt, pszCfgFileName,NULL); 
			break;
			case CFG_TYPE_INT:
				*(pItems[nIndex].ItemValue.pnInt) = GetFileInt( pItems[nIndex].pszParagraph, pItems[nIndex].pszItemName, pItems[nIndex].ItemDefaultValue.nInt, pszCfgFileName,NULL); 
			break;
			case CFG_TYPE_STRING:
				GetFileString( pItems[nIndex].pszParagraph, pItems[nIndex].pszItemName, pItems[nIndex].ItemDefaultValue.szString, pItems[nIndex].ItemValue.pszString, 80,pszCfgFileName,NULL); 
			break;
			case CFG_TYPE_NONE:
				/* eliminate compiler warning */
			break;
		}
		nIndex++;
	}
	return TRUE;
}

BOOL WriteCfgItems(PCFGITEM pItems,char *pszCfgFileName)
{
	int nIndex=0;

	while(pItems[nIndex].pszItemName){
		switch(pItems[nIndex].ItemType){
			case CFG_TYPE_BOOL:
				PutFileInt( pItems[nIndex].pszParagraph,pItems[nIndex].pszItemName,*(pItems[nIndex].ItemValue.pnInt),pszCfgFileName,NULL); 
			break;
			case CFG_TYPE_INT:
				PutFileInt( pItems[nIndex].pszParagraph,pItems[nIndex].pszItemName,*(pItems[nIndex].ItemValue.pnInt),pszCfgFileName,NULL); 
			break;
			case CFG_TYPE_STRING:
				PutFileString( pItems[nIndex].pszParagraph, pItems[nIndex].pszItemName, pItems[nIndex].ItemValue.pszString,pszCfgFileName,NULL); 
			break;
			case CFG_TYPE_NONE:
				/* eliminate compiler warning */
			break;
		}
		nIndex++;
	}
	return TRUE;
}


static DBconn *DB;
static GArray *tArray=NULL;


/*
* Normalizza il fastidiosissimo apice (per PGSQL !)
*/
char *NormalizeString(char *szString)
{
	static char szDestination[1024];
	char szTmpString[1024];
	char *pPtr;

	/*
	* sostituzione del (') 
	*/ 
	strcpy(szDestination,szString);
	pPtr=szDestination;
	while ((pPtr=strchr(pPtr,'\''))!=(char *)NULL){
		/*
		* Tronco la stringa sull'apice
		*/
		*pPtr = '\0';
		strcpy(szTmpString,szDestination);
		strcat(szTmpString,"\\'");
		strcat(szTmpString,(pPtr+1));

		/*
		* Riporto la modifica
		*/
		strcpy(szDestination,szTmpString);
		pPtr+=2;
	}
	return (szDestination);
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
	DBresult *DBRes;

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
		DBRes=DBExecQuery(FALSE,szSQLCmd);
		DBFreeQueryResult(DBRes);
		return TRUE;
	}
	return FALSE;
}


/*
* WriteAsciiRecord()
* scrittura di un record in formato ascii
* bFillZero : i campi numerici vengono 'riempiti' con '0' a sinistra, es: 23 (6 cifre) -> 000023
* bSigned : i campi numerici iniziano con il segno, es: 56 (6 cifre) -> +00056 
*/
BOOL WriteAsciiRecord(FILE *fp,PDBSTRUCT pDB,DBresult *DBRes,int nIndex,BOOL bFillZero,BOOL bSigned)
{
	char szRecord[4096];
	char szBuffer[2048];
	PFIELD pField;
	int nFieldIndex;
	int nFieldPosition;
	int nFieldLen;
	int nRecordLen=0;
	int nFieldsNumber=0;

	nFieldsNumber=DBnfields(DBRes);

	/*
	* Ricavo la lunghezza del buffer di scrittura
	*/

#ifdef RECORD_LEN /* modifica per Mondadori */
	
	nRecordLen=0;
	for(nFieldIndex=0;nFieldIndex<nFieldsNumber;nFieldIndex++){
		if(pDB->pFields[nFieldIndex]->nFieldFlag & IMPORT_MASK){
			nRecordLen+=pDB->pFields[nFieldIndex]->nFieldLen;
		}
	}
	nRecordLen+=pDB->nRecordTypeLen;
#endif

	nRecordLen=pDB->nRecordLen;
	memset(szRecord,' ',nRecordLen+1);
	szRecord[nRecordLen-1] = '*';
	szRecord[nRecordLen+0] = '\r';
	szRecord[nRecordLen+1] = '\n';
	szRecord[nRecordLen+2] = '\0';

	if(pDB->nRecordTypeLen){
		memcpy(szRecord+pDB->nRecordTypePos,pDB->szRecordType,pDB->nRecordTypeLen);
	}

	/* 
	* riempio il buffer con i dati del record 
	*/
	nFieldPosition=pDB->nDataStart;
	for(nFieldIndex=0;nFieldIndex<nFieldsNumber;nFieldIndex++){
		pField=pDB->pFields[nFieldIndex];
		if(pField->nFieldFlag & IMPORT_MASK){
			/*
			* calcolo la lunghezza del campo 
			*/
			nFieldLen=pField->nFieldLen;
			/*
			* Esporto il campo
			*/
			switch(pField->cFieldType){
				default:
					/* anche i campi data */
				case 'C':
				case 'c':
					sprintf(szBuffer, "%-*s", nFieldLen,DBgetvalue(DBRes,nIndex,nFieldIndex));
				break;
				case 'N':
				case 'n':
					if(bFillZero){
						sprintf(szBuffer, "%0*d", nFieldLen,atoi(DBgetvalue(DBRes,nIndex,nFieldIndex)));
					} else {
						sprintf(szBuffer, "%*d", nFieldLen,atoi(DBgetvalue(DBRes,nIndex,nFieldIndex)));
					}
					if(bSigned){
						szBuffer[0]=atoi(DBgetvalue(DBRes,nIndex,nFieldIndex))>=0?'+':'-';
					}
				break;
			}
			memcpy(szRecord+nFieldPosition, szBuffer, nFieldLen);
			nFieldPosition+=nFieldLen;
		}
	}
	fprintf(fp,szRecord);

	return(TRUE);
}


/*
* IsAsciiRecord(szBuffer,pDB)
*/
BOOL IsAsciiRecord(char *szBuffer,PDBSTRUCT pDB)
{
	int nLen;

	DeleteNewLine(szBuffer);
	nLen=strlen(szBuffer);

	if(nLen >= pDB->nRecordLen && !strncmp(szBuffer+pDB->nRecordTypePos,pDB->szRecordType,pDB->nRecordTypeLen)){
		return TRUE;
	}
	return FALSE;
}

/*
* BOOL UpdateAsciiRecord(char *szBuffer,PDBSTRUCT pDB, BOOL bUpdate)
* + st  02/10/2000 
* Legge i dati da szBuffer:
* - se bUpdate=TRUE effettua un ricerca nella tabella utilizzando la chiave "primaria -> UNIQUE e COSTITUITA DA UN SOLO CAMPO" 
*              (la prima indicata nel file di configurazione .cfg) effettuando un update se trova gia' presente un riga; altrimenti inserisce;
* - se bUpdate=FALSE inserisce direttamente in tabella (ATTENZIONE ALLE CHIAVI "primarie");
* + rm  08/08/2001 
* gestione indice composto da piu' campi
*/
BOOL UpdateAsciiRecord(char *szBuffer, PDBSTRUCT pDB, BOOL bUpdate)
{
	int nFieldPosition;
	int nFieldIndex;
	int nIndex;
	int nLen;
	char szFieldString[128];
	char szUpdateSQLCmd[1024];
	char szInsertSQLCmd[1024];
	char *pszKeyFieldsData[128];
	int nKeyFields[128];
	int nKeyFieldsNumber=0;
	char *pszBuffer;
	char szTmpBuffer[128];
	char *pPtr;
	char  *szSeparator="+";
	DBresult *DBRes;
	DBresult *DBResInsert;


	DeleteNewLine(szBuffer);
	nLen=strlen(szBuffer);

	/*
	* Verifico corrispondenza tipo record
	*/
	if(nLen >= pDB->nRecordLen && !strncmp(szBuffer+pDB->nRecordTypePos,pDB->szRecordType,pDB->nRecordTypeLen)){
		/*
		* inserimento dei dati in tabelle SQL
		*/
		/*
		* Se Update allora genero la lista dei campi chiave
		*/
		if(bUpdate){
			if(pDB->Tags==(PTAG)NULL){
				fprintf(stderr,"Errore : Overwrite e Indice [0] non presente !!!\n");
#ifdef TRACE
				trace_out_vstr_date(1,"UpdateAsciiRecord(%s) : Errore : Overwrite e Indice [0] non presente !!!" , pDB->szTableName);
#endif
				return FALSE;
			}

			/*
			* Genero la lista campi chiave (solo del primo indice)
			*/
			nKeyFieldsNumber=0;
			strcpy(szTmpBuffer,pDB->Tags[0].szExpression);
			pszBuffer=szTmpBuffer;
			while((pPtr=strtok(pszBuffer,szSeparator))!=NULL){
				for(nFieldIndex=0; nFieldIndex<pDB->nFieldsNumber; nFieldIndex++){
					if(!strcmp(pPtr, pDB->pFields[nFieldIndex]->szFieldName)){
						nKeyFields[nKeyFieldsNumber]=nFieldIndex;
						nKeyFieldsNumber++;
						break;
					}
				}
				pszBuffer=NULL;
			}

			if(nKeyFieldsNumber==0){
				fprintf(stderr,"Errore : Overwrite attivo e campo indice non presente [%s]\n",pDB->Tags[0].szExpression);
#ifdef TRACE
				trace_out_vstr_date(1,"UpdateAsciiRecord(%s) : Errore : Overwrite attivo e campo indice non presente [%s]" , pDB->szTableName,pDB->Tags[0].szExpression);
#endif
				return FALSE;
			}
		}

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


				/*
				* Conversione per il valori numerici
				*/
				if(pDB->pFields[nFieldIndex]->cFieldType=='N'){
					if(pDB->pFields[nFieldIndex]->nFieldDec){
						sprintf(szFieldString,"%0f",atof(szFieldString));
					} else {
						sprintf(szFieldString,"%0d",atoi(szFieldString));
					}
				}

				/*
				* se si tratta di un campo chiave (e sono in update) memorizzo il valore
				*/
				if(bUpdate){
					for(nIndex=0;nIndex<nKeyFieldsNumber;nIndex++){
						if(nFieldIndex==nKeyFields[nIndex]) {
							pszKeyFieldsData[nIndex]=strdup(szFieldString);
						}
					}
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
				strcpy(szFieldString,NormalizeString(szFieldString));

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
			strcat (szUpdateSQLCmd, " WHERE (");
			for(nIndex=0;nIndex<nKeyFieldsNumber;nIndex++){
				if(nIndex){
					strcat (szUpdateSQLCmd, " AND ");
				}
				strcat (szUpdateSQLCmd, pDB->pFields[nKeyFields[nIndex]]->szFieldName);
				strcat (szUpdateSQLCmd, " = ");

				if(pDB->pFields[nKeyFields[nIndex]]->cFieldType=='C'){
					strcat (szUpdateSQLCmd, "\'");
				}

				strcat (szUpdateSQLCmd, pszKeyFieldsData[nIndex]);

				if(pDB->pFields[nKeyFields[nIndex]]->cFieldType=='C'){
					strcat (szUpdateSQLCmd, "\'");
				}

			}
			strcat (szUpdateSQLCmd, " ) ");


		}
		if (bUpdate){
			strcat (szUpdateSQLCmd, ";");
		}
		strcat (szInsertSQLCmd, ");");

		if (bUpdate){
			/*
			* con bUpdate=TRUE -> se DBcmdTuples ritorna 1 
			* - la riga era gia'presente ed e' stata aggiornata;
			* - altrimenti significa che la riga non e' presente e va inserita
			*/
#ifdef TRACE_ELIMINATO
			trace_out_vstr_date (1, "Exec Query : %s",szUpdateSQLCmd);
#endif
#ifdef TRACE_SQL
			DBRes =  DBExecQuery(TRUE,szUpdateSQLCmd);
#else
			DBRes =  DBExecQuery(FALSE,szUpdateSQLCmd);
#endif
			/*
			* Update fallito a causa di record non presente (lo inserisco)
			*/
			if (atoi(DBcmdTuples(DBRes))==0){
				DBResInsert = DBExecQuery(FALSE, szInsertSQLCmd);
				DBFreeQueryResult(DBResInsert);
			}
			DBFreeQueryResult(DBRes);
		} else {
			/*
			* non sono nella condizione di update
			* -> inserisco direttamente
			*/
			DBRes = DBExecQuery(FALSE, szInsertSQLCmd);
			DBFreeQueryResult(DBRes);
		}

		if (bUpdate){
			/* faccio pulizia */
			for(nIndex=0;nIndex<nKeyFieldsNumber;nIndex++){
				free(pszKeyFieldsData[nIndex]);
			}
		}
		return TRUE;
	} else {
		return FALSE;
	}
}



/*
* BOOL ReadTableInfo(PDBSTRUCT pDB,char *szCfgBaseName)
* Lettura delle informazioni relative ad una tabella 
*/
BOOL ReadTableInfo(PDBSTRUCT pDB,char *szPath,char *szCfgBaseName)
{
	char szCfgFile[128];

	if(pDB==NULL){
		return FALSE;
	}
	/*
	* creo i nomi di: Archivio, Indice, Cfg file
	*/
	sprintf(szCfgFile,"%s/%s.cfg",szPath,szCfgBaseName);

	pDB->nTagsNumber=ReadTagsInfo(szCfgFile,pDB);

	if((ReadFieldsInfo(szCfgFile,pDB))==-1){
		return(FALSE);
	} else {
		return(TRUE);
	}
}


void InsertTableInfo(PDBSTRUCT pDB,char *szCfgPath,char *szCfgName)
{
	DB_ARRAY_ITEM_STRUCT DBA;

	if(tArray==NULL){
		tArray=g_array_new(TRUE,TRUE,sizeof(DB_ARRAY_ITEM_STRUCT));
	}
	DBA.pDB=pDB;
	strcpy(DBA.szCfgPath,szCfgPath);
	strcpy(DBA.szCfgName,szCfgName);

	g_array_append_val(tArray,DBA);
}

void FreeTablesInfo(void)
{
	if(tArray) g_array_free(tArray,TRUE);
}

/*
* lettura configurazione tabelle
*/
void ReadTablesInfo(void)
{
	int nIndex=0;
	PDB_ARRAY_ITEM_STRUCT pDBA;

	while((pDBA=&g_array_index(tArray,DB_ARRAY_ITEM_STRUCT,nIndex))){

		if(pDBA->pDB==NULL){
			break;
		}
		if(ReadTableInfo(pDBA->pDB,pDBA->szCfgPath,pDBA->szCfgName)==FALSE){
#ifdef TRACE
			trace_out_vstr_date(1, "Errore in Apertura / Creazione [%s/%s]",pDBA->szCfgPath,pDBA->szCfgName);
#endif
		}

		nIndex++;
	}
}

/*
* ReadFieldsInfo()
*
* Fields structure inizialization
* legge i dati contenuti nel file di inizializzazione e
* riempie la struttura preposta alla loro memorizzazione
*
* ritorna:
* >=0 : numero di campi allocati
*  -1 : errore in allocazione campi
*/
int ReadFieldsInfo(char *szCfgFile,PDBSTRUCT pDB)
{
	PFIELD pField;
	int   nFieldIndex;
	char  szBuffer[256];
	char  szFieldNumber[80];
	char  szParagraph[80];
	char  *pPtr;
	char  *szSeparator=" ,\t\n";
	char  *szDescrSeparator=",\n";

	/*
	* lettura del file di configurazione
	*/
	strcpy(szParagraph,"General Settings");

	/*
	* Nome Data Base
	*/
	GetFileString(szParagraph,"DataBaseName", "", pDB->szDBName,sizeof(pDB->szDBName),szCfgFile,NULL);

	/*
	* Nome Tabella
	*/
	GetFileString(szParagraph,"TableName", "", pDB->szTableName,sizeof(pDB->szTableName),szCfgFile,NULL);

	pDB->nFieldsNumber   = GetFileInt(szParagraph,"FieldsNumber", 0,szCfgFile,NULL);
	pDB->nRecordLen      = GetFileInt(szParagraph,"RecordLen",    0,szCfgFile,NULL);
	pDB->nRecordTypePos  = GetFileInt(szParagraph,"RecordTypePos",1,szCfgFile,NULL);
	pDB->nRecordTypeLen  = GetFileInt(szParagraph,"RecordTypeLen",0,szCfgFile,NULL);
	pDB->nLineNumberPos  = GetFileInt(szParagraph,"LineNumberPos",1,szCfgFile,NULL);
	pDB->nDataStart      = GetFileInt(szParagraph,"DataStart",    1,szCfgFile,NULL);
	pDB->nLinesNumber    = GetFileInt(szParagraph,"LinesNumber",  0,szCfgFile,NULL);

	GetFileString(szParagraph,"RecordType","",pDB->szRecordType,sizeof(pDB->szRecordType),szCfgFile,NULL);

	/*
	* normalizzazione dati (offset 0)
	*/
	pDB->nRecordTypePos = max(0,pDB->nRecordTypePos-1);
	pDB->nLineNumberPos = max(0,pDB->nLineNumberPos-1);
	pDB->nDataStart     = max(0,pDB->nDataStart-1);

	nFieldIndex=0;
	strcpy(szParagraph,"Fields");
	while(nFieldIndex<pDB->nFieldsNumber){
		sprintf(szFieldNumber,"Field_%02d",nFieldIndex);
		GetFileString(szParagraph,szFieldNumber,"",szBuffer,sizeof(szBuffer),szCfgFile,NULL);
		if(strlen(szBuffer)){
			pDB->pFields[nFieldIndex]=(PFIELD)malloc(sizeof(FIELD));
			memset(pDB->pFields[nFieldIndex],0,sizeof(FIELD));
			pField=pDB->pFields[nFieldIndex];
			/* 
			* Field Name 
			*/
			if((pPtr=strtok(szBuffer,szSeparator))!=NULL){
				strcpy(pField->szFieldName,pPtr);
			} else {return(-1);}
			/* 
			* Field Type
			*/
			if((pPtr=strtok(NULL,szSeparator))!=NULL){
				pField->cFieldType=*pPtr;
			} else {return(-1);}
			/*
			* Field Len
			*/
			if((pPtr=strtok(NULL,szSeparator))!=NULL){
				pField->nFieldLen=atoi(pPtr);
			} else {return(-1);}
			/*
			* Field Dec
			*/
			if((pPtr=strtok(NULL,szSeparator))!=NULL){
				pField->nFieldDec=atoi(pPtr);
			} else {return(-1);}
			/*
			* rm 15-01-1999 : Dati per multilinea solo se LinesNumber>0
			*/
			if(pDB->nLinesNumber){
				/*
				* Line Number : Numero di riga se record multilinea
				*/
				if((pPtr=strtok(NULL,szSeparator))!=NULL){
					pField->nLineNumber=atoi(pPtr);
				} else {return(-1);}
				/*
				* Field Offset : offset del campo nel file ascii (offset 1)
				*/
				if((pPtr=strtok(NULL,szSeparator))!=NULL){
					pField->nFieldOffset=atoi(pPtr);
				} else {return(-1);}
				/*
				* normalizzazione dati (offset 0)
				*/
				pField->nFieldOffset -=1;
			}
			/* Field Flag  */
			if((pPtr=strtok(NULL,szSeparator))!=NULL){
				pField->nFieldFlag=atoi(pPtr);
			} else {return(-1);}
			/*
			* Field Title
			*/
			if((pPtr=strtok(NULL,szDescrSeparator))!=NULL){
				strncpy(pField->szFieldTitle,pPtr,FIELD_TITLE_LEN-1);
			} else {return(-1);}
			/*
			* Field Description
			*/
			if((pPtr=strtok(NULL,szDescrSeparator))!=NULL){
				strncpy(pField->szFieldDescr,pPtr,FIELD_DESCR_LEN-1);
			} else {return(-1);}
		}
		nFieldIndex++;
	}

	return nFieldIndex;
}

/*
* ReadTagsInfo()
* alloca e legge i dati relativi ai tag per gli indici
*/
int ReadTagsInfo(char *szCfgFile,PDBSTRUCT pDB)
{
	int   nTagIndex;
	int   nTagsNumber;
	char  szBuffer[128];
	char  szTagNumber[80];
	char  szParagraph[80];
	char  *pPtr;
	char  *szSeparator=" ,\n";
	char  *szExprSeparator="\n";
	/*
	* lettura del file di configurazione
	*/
	strcpy(szParagraph,"General Settings");
	nTagsNumber=GetFileInt(szParagraph,"TagsNumber",0,szCfgFile,NULL);

	nTagIndex=0;
	strcpy(szParagraph,"Tags");
	while(nTagIndex<nTagsNumber){
		sprintf(szTagNumber,"Tag_%02d",nTagIndex);
		GetFileString(szParagraph,szTagNumber,"",szBuffer,sizeof(szBuffer),szCfgFile,NULL);
		if(strlen(szBuffer)){
			/* Nome tag */
			if((pPtr=strtok(szBuffer,szSeparator))!=NULL){
				strcpy(pDB->Tags[nTagIndex].szTagName,pPtr);
			} else {return(-1);}
			/* Unique */
			if((pPtr=strtok(NULL,szSeparator))!=NULL){
				pDB->Tags[nTagIndex].nUnique=atoi(pPtr);
			} else {return(-1);}
			/* Expression */
			if((pPtr=strtok(NULL,szExprSeparator))!=NULL){
				strcpy(pDB->Tags[nTagIndex].szExpression,pPtr);
			} else {return(-1);}
		}
		nTagIndex++;
	}

	return nTagsNumber;
}

#ifdef USE_POSTGRES
#include "ep_pgsql.c"
#endif

DBExecStatusType DBresultStatus(DBresult *DBRes)
{
#ifdef USE_POSTGRES
	return PQresultStatus((PGresult *)DBRes);
#endif
}

char *DBcmdTuples(DBresult *DBRes)
{
#ifdef USE_POSTGRES
	return PQcmdTuples((PGresult *)DBRes);
#endif
}

int DBnfields(DBresult *DBRes)
{
#ifdef USE_POSTGRES
	return PQnfields((PGresult *)DBRes);
#endif
}

int DBntuples(DBresult *DBRes)
{
#ifdef USE_POSTGRES
	return PQntuples((PGresult *)DBRes);
#endif
}

unsigned int DBgetlength(DBresult *DBRes,int nTuple,int nField)
{
#ifdef USE_POSTGRES
	return PQgetlength((PGresult *)DBRes,nTuple,nField);
#endif
}

char *DBfname(DBresult *DBRes,int nField)
{
#ifdef USE_POSTGRES
	return PQfname((PGresult *)DBRes,nField);
#endif
}

int DBftype(DBresult *DBRes,int nField)
{
#ifdef USE_POSTGRES
	return PQftype((PGresult *)DBRes,nField);
#endif
}

char *DBgetvalue(DBresult *DBRes,int nTuple,int nField)
{
#ifdef USE_POSTGRES
	return PQgetvalue((PGresult *)DBRes,nTuple,nField);
#endif
}

char *DBresultErrorMessage(DBresult *DBRes)
{
#ifdef USE_POSTGRES
	return PQresultErrorMessage((DBresult *)DBRes);
#endif
}
static BOOL ConnectionActive=FALSE;

BOOL DBConnect(char *szHost,char *szPort,char *szDataBase)
{
	BOOL bRetValue=FALSE;

	if(ConnectionActive){
		return FALSE;
	}

#ifdef USE_POSTGRES
	if((DB=pgsql_connect_db(szHost,szPort,szDataBase))!=(DBconn *)NULL){
		bRetValue=TRUE;
	}
#endif

	ConnectionActive=bRetValue;;

	return bRetValue;
}

BOOL DBDisconnect(void)
{
	if(!ConnectionActive){
		return FALSE;
	}
#ifdef USE_POSTGRES
	pgsql_disconnect_db(DB);
#endif
	ConnectionActive=FALSE;
}

BOOL DBConnectionActive(void)
{
	return ConnectionActive;
}

DBresult *DBExecQuery(BOOL bTrace,char *format, ...)
{
	va_list ap;
	char szQuery[4096];
	DBresult *DBRes=(DBresult *)NULL;

	va_start (ap, format);
	vsprintf (szQuery, format, ap);
	va_end (ap);
#ifdef TRACE
	if(bTrace) trace_out_vstr_date(1,"EXEC SQL [%s]", szQuery);
#endif

#ifdef USE_POSTGRES
	DBRes=pgsql_exec(DB,szQuery);
#endif

#ifdef TRACE
	/*
	* Controllo ritorno del comando SQL
	* visualizzo comunque l'eventuale errore anche se bTrace e' FALSE
	*/
	switch(DBresultStatus(DBRes)){
		case DBRES_COMMAND_OK:
			if(bTrace)trace_out_vstr_date(1,"DBRES_COMMAND_OK : [%5d] tuples",atoi(DBcmdTuples(DBRes)));
		break;
		case DBRES_EMPTY_QUERY:
			trace_out_vstr_date(1,"DBRES_EMPTY_QUERY : %s",DBresultErrorMessage(DBRes));
		break;
		case DBRES_TUPLES_OK:
			if(bTrace)trace_out_vstr_date(1,"DBRES_TUPLES_OK : [%5d] tuples", DBntuples (DBRes));
		break;
		case DBRES_COPY_OUT:
			if(bTrace)trace_out_vstr_date(1,"DBRES_COPY_OUT");
		break;
		case DBRES_COPY_IN:
			if(bTrace)trace_out_vstr_date(1,"DBRES_COPY_IN");
		break;
		case DBRES_BAD_RESPONSE:
			trace_out_vstr_date(1,"DBRES_BAD_RESPONSE : %s",DBresultErrorMessage(DBRes));
		break;
		case DBRES_NONFATAL_ERROR:
			trace_out_vstr_date(1,"DBRES_NONFATAL_ERROR : %s\n -> %s",DBresultErrorMessage(DBRes),szQuery);
		break;
		case DBRES_FATAL_ERROR:
			trace_out_vstr_date(1,"DBRES_FATAL_ERROR : %s\n -> %s",DBresultErrorMessage(DBRes),szQuery);
		break;
	}
#endif

	return DBRes;
}

void DBFreeQueryResult(DBresult *DBRes)
{
#ifdef USE_POSTGRES
	pgsql_free_result(DBRes);
#endif
}

static DbType db_types[] = {
	{  16,  "bool",           DB_TYPE_STRING},
	{  17,  "bytea",          DB_TYPE_STRING},
	{  18,  "char",           DB_TYPE_STRING},
	{  19,  "name",           DB_TYPE_STRING},
	{  20,  "int8",           DB_TYPE_INTEGER},
	{  21,  "int2",           DB_TYPE_INTEGER},
	{  22,  "int2vector",     DB_TYPE_STRING},
	{  23,  "int4",           DB_TYPE_INTEGER},
	{  24,  "regproc",        DB_TYPE_STRING},
	{  25,  "text",           DB_TYPE_STRING},
	{  26,  "oid",            DB_TYPE_STRING},
	{  27,  "tid",            DB_TYPE_STRING},
	{  28,  "xid",            DB_TYPE_STRING},
	{  29,  "cid",            DB_TYPE_STRING},
	{  30,  "oidvector",      DB_TYPE_STRING},
	{  32,  "SET",            DB_TYPE_STRING},
	{  71,  "pg_type",        DB_TYPE_STRING},
	{  75,  "pg_attribute",   DB_TYPE_STRING},
	{  81,  "pg_proc",        DB_TYPE_STRING},
	{  83,  "pg_class",       DB_TYPE_STRING},
	{  86,  "pg_shadow",      DB_TYPE_STRING},
	{  87,  "pg_group",       DB_TYPE_STRING},
	{  88,  "pg_database",    DB_TYPE_STRING},
	{ 210,  "smgr",           DB_TYPE_STRING},
	{ 600,  "point",          DB_TYPE_STRING},
	{ 601,  "lseg",           DB_TYPE_STRING},
	{ 602,  "path",           DB_TYPE_STRING},
	{ 603,  "box",            DB_TYPE_STRING},
	{ 604,  "polygon",        DB_TYPE_STRING},
	{ 628,  "line",           DB_TYPE_STRING},
	{ 629,  "_line",          DB_TYPE_STRING},
	{ 650,  "cidr",           DB_TYPE_STRING},
	{ 651,  "_cidr",          DB_TYPE_STRING},
	{ 700,  "float4",         DB_TYPE_FLOAT},
	{ 701,  "float8",         DB_TYPE_FLOAT},
	{ 702,  "abstime",        DB_TYPE_DATETIME},
	{ 703,  "reltime",        DB_TYPE_STRING},
	{ 704,  "tinterval",      DB_TYPE_STRING},
	{ 705,  "unknown",        DB_TYPE_STRING},
	{ 718,  "circle",         DB_TYPE_STRING},
	{ 719,  "_circle",        DB_TYPE_STRING},
	{ 790,  "money",          DB_TYPE_STRING},
	{ 791,  "_money",         DB_TYPE_STRING},
	{ 829,  "macaddr",        DB_TYPE_STRING},
	{ 869,  "inet",           DB_TYPE_STRING},
	{1000,  "_bool",          DB_TYPE_STRING},
	{1001,  "_bytea",         DB_TYPE_STRING},
	{1002,  "_char",          DB_TYPE_STRING},
	{1003,  "_name",          DB_TYPE_STRING},
	{1005,  "_int2",          DB_TYPE_INTEGER},
	{1006,  "_int2vector",    DB_TYPE_STRING},
	{1007,  "_int4",          DB_TYPE_INTEGER},
	{1008,  "_regproc",       DB_TYPE_STRING},
	{1009,  "_text",          DB_TYPE_STRING},
	{1010,  "_tid",           DB_TYPE_STRING},
	{1011,  "_xid",           DB_TYPE_STRING},
	{1012,  "_cid",           DB_TYPE_STRING},
	{1013,  "_oidvector",     DB_TYPE_STRING},
	{1014,  "_bpchar",        DB_TYPE_STRING},
	{1015,  "_varchar",       DB_TYPE_STRING},
	{1016,  "_int8",          DB_TYPE_INTEGER},
	{1017,  "_point",         DB_TYPE_STRING},
	{1018,  "_lseg",          DB_TYPE_STRING},
	{1019,  "_path",          DB_TYPE_STRING},
	{1020,  "_box",           DB_TYPE_STRING},
	{1021,  "_float4",        DB_TYPE_FLOAT},
	{1022,  "_float8",        DB_TYPE_FLOAT},
	{1023,  "_abstime",       DB_TYPE_STRING},
	{1024,  "_reltime",       DB_TYPE_STRING},
	{1025,  "_tinterval",     DB_TYPE_STRING},
	{1027,  "_polygon",       DB_TYPE_STRING},
	{1028,  "_oid",           DB_TYPE_STRING},
	{1033,  "aclitem",        DB_TYPE_STRING},
	{1034,  "_aclitem",       DB_TYPE_STRING},
	{1040,  "_macaddr",       DB_TYPE_STRING},
	{1041,  "_inet",          DB_TYPE_STRING},
	{1042,  "bpchar",         DB_TYPE_STRING},
	{1043,  "varchar",        DB_TYPE_STRING},
	{1082,  "date",           DB_TYPE_DATE},
	{1083,  "time",           DB_TYPE_TIME},
	{1114,  "timestamp",      DB_TYPE_DATETIME},
	{1115,  "_timestamp",     DB_TYPE_DATETIME},
	{1182,  "_date",          DB_TYPE_DATE},
	{1183,  "_time",          DB_TYPE_TIME},
	{1184,  "timestamptz",    DB_TYPE_DATETIME},
	{1185,  "_timestamptz",   DB_TYPE_DATETIME},
	{1186,  "interval",       DB_TYPE_STRING},
	{1187,  "_interval",      DB_TYPE_STRING},
	{1231,  "_INTEGER",       DB_TYPE_INTEGER},
	{1266,  "timetz",         DB_TYPE_TIME},
	{1270,  "_timetz",        DB_TYPE_TIME},
	{1560,  "bit",            DB_TYPE_STRING},
	{1561,  "_bit",           DB_TYPE_STRING},
	{1562,  "varbit",         DB_TYPE_STRING},
	{1563,  "_varbit",        DB_TYPE_STRING},
	{1700,  "numeric",        DB_TYPE_INTEGER},
	{1790,  "refcursor",      DB_TYPE_STRING},
	{0,NULL,0}
};

DBFieldType db_get_field_type(int nType)
{
	int i=0;

	while(db_types[i].type_name){
		if(db_types[i].type_id == nType){
			return db_types[i].type;
		}
		i++;
	}
	return DB_TYPE_NONE;
}
