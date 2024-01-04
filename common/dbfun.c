/*
* modulo : dbfun.c
* -----------------
* Libreria di gestione tabelle e configurazioni
*
* Progetto Picking - DB Management - PostgreSQL
*
* Data creazione 13/12/1995
*
* Autore : Roberto Mantovani
*
* Copyright CNI srl 1995-98
*
* Modifiche
* -------------   -------------------------------------------------------
* rm 07-07-1998 : Aggiunta compilazione condizionale per CodeBase 4.5/5.1
* rm 18-01-1999 : Aggiunta la gestione multi linea per DeAgostini
* rm 16-06-2000 : Eliminati i riferimenti a CodeBase - si usa PGSQL
* -------------   -------------------------------------------------------
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "dbfun.h"
#include "ca_file.h"

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
	xncGetFileString(szParagraph,"DataBaseName", "", pDB->szDBName,sizeof(pDB->szDBName),szCfgFile,NULL);

	/*
	* Nome Tabella
	*/
	xncGetFileString(szParagraph,"TableName", "", pDB->szTableName,sizeof(pDB->szTableName),szCfgFile,NULL);

	pDB->nFieldsNumber   = xncGetFileInt(szParagraph,"FieldsNumber", 0,szCfgFile,NULL);
	pDB->nRecordLen      = xncGetFileInt(szParagraph,"RecordLen",    0,szCfgFile,NULL);
	pDB->nRecordTypePos  = xncGetFileInt(szParagraph,"RecordTypePos",1,szCfgFile,NULL);
	pDB->nRecordTypeLen  = xncGetFileInt(szParagraph,"RecordTypeLen",0,szCfgFile,NULL);
	pDB->nLineNumberPos  = xncGetFileInt(szParagraph,"LineNumberPos",1,szCfgFile,NULL);
	pDB->nDataStart      = xncGetFileInt(szParagraph,"DataStart",    1,szCfgFile,NULL);
	pDB->nLinesNumber    = xncGetFileInt(szParagraph,"LinesNumber",  0,szCfgFile,NULL);

	xncGetFileString(szParagraph,"RecordType","",pDB->szRecordType,sizeof(pDB->szRecordType),szCfgFile,NULL);

	/*
	* normalizzazione dati (offset 0)
	*/
	pDB->nRecordTypePos -=1;
	pDB->nLineNumberPos -=1;
	pDB->nDataStart -=1;

	nFieldIndex=0;
	strcpy(szParagraph,"Fields");
	while(nFieldIndex<pDB->nFieldsNumber){
		sprintf(szFieldNumber,"Field_%02d",nFieldIndex);
		xncGetFileString(szParagraph,szFieldNumber,"",szBuffer,sizeof(szBuffer),szCfgFile,NULL);
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
	nTagsNumber=xncGetFileInt(szParagraph,"TagsNumber",0,szCfgFile,NULL);

	nTagIndex=0;
	strcpy(szParagraph,"Tags");
	while(nTagIndex<nTagsNumber){
		sprintf(szTagNumber,"Tag_%02d",nTagIndex);
		xncGetFileString(szParagraph,szTagNumber,"",szBuffer,sizeof(szBuffer),szCfgFile,NULL);
		if(strlen(szBuffer)){
			/* Nome tag */
			if((pPtr=strtok(szBuffer,szSeparator))!=NULL){
				strcpy(pDB->Tags[nTagIndex].szTagName,pPtr);
			} else {return(-1);}
			/* Order */
			if((pPtr=strtok(NULL,szSeparator))!=NULL){
				pDB->Tags[nTagIndex].nOrder=atoi(pPtr);
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
