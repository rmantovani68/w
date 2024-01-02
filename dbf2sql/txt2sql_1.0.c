/* 
* Modulo : txt2sql.c
* ------------------
* Lettura e conversione archivi Ascii Format -> (PostGres) SQL Format
*
* Progetto Picking
*
* Data creazione 22/05/2000
*
* Autore : Roberto Mantovani - Stefano Tarroni
*
* Copyright CNI srl 1995-2000
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <libpq-fe.h>
#include "dbfun.h"		/* Data Base Functions */
#include "ca_file.h"	/* Configuration Files Functions */
#include "dbf.h"			/* definisce la struttura di un file dbf */

typedef struct _config { 
	BOOL bVerbose;	
	BOOL bReplaceInTable;
	BOOL bUserID;
	BOOL bConfigFile;
	BOOL bImportFile;
	BOOL bTableName;
	BOOL bpghost;
	BOOL bpgport;
	BOOL bdbName;
	int nUserID;
	char szConfigFile[128];
	char szImportFile[128];
	char szTableName[128];
	char *pszpghost;
	char *pszpgport;
	char pszdbName[128];
} CFGSTRUCT;

extern char *optarg;
extern int optind;

/*--------------------------------------------------------*/
/*-------------- function declaration --------------------*/
/*--------------------------------------------------------*/

BOOL FileExists(char *szFileName);
void PrintUsage (char **argv);
void set_cfg_str (CFGSTRUCT *cfgstruct, int argc, char **argv); 
void main(int argc,char **argv);


/*--------------------------------------------------------*/
/*--------------- function definition --------------------*/
/*--------------------------------------------------------*/

BOOL FileExists(char *szFileName)
{
	FILE *fp;

	if((fp=fopen(szFileName,"r"))!=(FILE *)NULL){
		fclose(fp);
		return(TRUE);
	}
	return(FALSE);
}

/*--------------------------------------------------------*/

void exit_nicely (PGconn *conn)	
{
	PQfinish(conn);
	exit(1);
}
	
/*--------------------------------------------------------*/

void PrintUsage(char **argv)
{
	printf("\nUsage: %s [-u <user_id> -c <config> -f <text_file> -t <table> -h <hostname> -p <port> -d <DataBase>  -r -v -?]\n",argv[0]);
	printf("-u <User ID> \n");
	printf("-c <Source_Configuration_File> \n");
	printf("-i <Text_File> \n");
	printf("-h <Hostname> \n");
	printf("-p <Port> \n");
	printf("-d <Destination_DataBase> \n");
	printf("-t <Name_Destination_Table> - default: name of the DBF or CFG file - \n");
	printf("-r : Replace data in existintg table - default: Add Data -\n");
	printf("-v : Verbose sql query, date and time\n");
	printf("-? : Print This Help Message \n");
	printf("\n");
}

/*--------------------------------------------------------*/

void set_cfg_str(CFGSTRUCT *cfgstruct, int argc, char **argv) 
/*
* settaggio dei parametri per la connessione
*/
{
	int nOpt;
	char *pPtr;
	char szTableName_aux[128];
	
	memset (cfgstruct, '\0', sizeof(CFGSTRUCT) );	
	while ((nOpt = getopt(argc, argv, "c:d:i:h:p:t:u:rv?")) != EOF) {
		switch(nOpt){
			case 'c': /* Configuration File */
				cfgstruct->bConfigFile=TRUE;
				strcpy(cfgstruct->szConfigFile ,optarg);
				break;
			case 'd': /* database */
				cfgstruct->bdbName=TRUE;
				strcpy(cfgstruct->pszdbName ,optarg);
				break;
			case 'i': /* text import file */
				cfgstruct->bImportFile=TRUE;
				strcpy(cfgstruct->szImportFile ,optarg);
				break;
			case 'h': /* hostname */
				cfgstruct->bpghost=TRUE;
				(int)cfgstruct->pszpghost=atoi(optarg);
				break;
			case 'p': /* port */
				cfgstruct->bpgport=TRUE;
				(int)cfgstruct->pszpgport = atoi(optarg);
				break;
			case 'r': /* replace data in table */
				cfgstruct->bReplaceInTable=TRUE;
				break;
			case 't': /* table name */
				cfgstruct->bTableName=TRUE;
				strcpy(cfgstruct->szTableName ,optarg);
				break;
			case 'u': /* User ID */
				cfgstruct->bUserID=TRUE;
				cfgstruct->nUserID = atoi(optarg);
				break;
			case 'v': /* User ID */
				cfgstruct->bVerbose=TRUE;
				break;
			case '?':
				PrintUsage(argv);
				exit(1);
				break;
			default:
				printf("Bad Arguments\n");
				break;
		}
	}
	
	/*
	* controllo presenza delle opzioni fondamentali per
	* la connessione (ConfigFile, DataBaseName e TextFile)
	*/
	if (!(cfgstruct->bdbName  && cfgstruct->bImportFile  && cfgstruct->bConfigFile)) {
		printf("\n\t!!! Missing Basic Arguments\n");
		PrintUsage(argv);
		exit(1);
	}
	
	/* 
	* se non è stato inserito, al nome della tabella
	* viene assegnato al nome del file .cfg
	*/	
	if (!cfgstruct->bTableName){
		strcpy (szTableName_aux, cfgstruct->szConfigFile);
		if ((pPtr=strrchr(szTableName_aux, '.')) != (char *)NULL){
			*pPtr = '\0';	/* eliminazione dell'estensione (.dbf o .cfg)  */
		}	
		if ((pPtr=(strrchr(szTableName_aux, '/' ))) != (char *)NULL){
			strcpy(szTableName_aux, ++pPtr);	/* eliminazione del path */ 
		}
		strcpy (cfgstruct->szTableName, szTableName_aux);
	}

	/*
	* settaggio user-id
	*/
	if(cfgstruct->bUserID){
		seteuid (cfgstruct->nUserID);
	}
}

/*--------------------------------------------------------*/
/*----------------------- MAIN ---------------------------*/
/*--------------------------------------------------------*/
void main(int argc,char **argv)
{
	char szBuffer[1024];
	char szFieldString[128];
	char *pPtr;
	int nRecords;
	int nFieldIndex;
	int nFieldPosition;
	int nLineNumber;
	FILE *txt_fp;
	time_t t;
	
	CFGSTRUCT cfgstruct;	   	/* Cfg Data Struct */
	DBSTRUCT DB;							/* DataBase Structure */
	char szSQLCmd[1024];		 	/* Command Query */
	PGconn *conn;	        	  /* DBMS Connection */
	PGresult *res;					 	/* Query Result */
	
	if(argc==1){
		PrintUsage(argv);
		exit(1);
	}

	/*
	* settaggio struttura di configurazione
  */ 
	set_cfg_str(&cfgstruct, argc, argv); 

	/*
	* gestione Configuration File
	*/
	if(cfgstruct.bConfigFile){
		if((DB.nTagsNumber=ReadTagInfo(cfgstruct.szConfigFile,&DB))==-1){
			fprintf(stderr,"Error reading tags structure %s", cfgstruct.szConfigFile);
			exit(1);
		}

		if((DB.nFieldsNumber=ReadFieldStruct(cfgstruct.szConfigFile,&DB))==-1){
			fprintf(stderr,"Error reading field structure %s", cfgstruct.szConfigFile);
			exit(1);
		}
	}

	/*
	* gestione text file
	*/ 
	if (FileExists(cfgstruct.szImportFile)==TRUE){
		txt_fp=fopen(cfgstruct.szImportFile,"r");
	} else {
		fprintf (stderr, "Errore in apertura %s",cfgstruct.szImportFile);
		exit(1);
	}
	
	/*
	* connessione al database
	*/
	conn = PQsetdb(cfgstruct.pszpghost, cfgstruct.pszpgport, NULL, NULL, cfgstruct.pszdbName);
	
	/*
	* controllo correttezza della connessione
	*/
	if (PQstatus(conn) == CONNECTION_BAD) {
		fprintf(stderr, "Connection to database '%s' FAILED\n", cfgstruct.pszdbName);
		fprintf(stderr, "%s", PQerrorMessage(conn));
		exit_nicely (conn);
	} else {
		fprintf(stderr, "Connection to database '%s' OK\n", cfgstruct.pszdbName);
	}

	if (cfgstruct.bReplaceInTable) {
		/*
		* eliminazione delle tuple presenti
		*/
		printf ("\nELIMINAZIONE RECORD PRESENTI IN TABELLA :");
		strcpy (szSQLCmd, "DELETE FROM ");
		strcat (szSQLCmd, cfgstruct.szTableName);
		strcat (szSQLCmd, ";");
		res = PQexec(conn, szSQLCmd);
		/* 
		* controllo l'esecuzione di PQexec() 
		*/ 
		if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
			fprintf (stderr, "DELETE FROM %s: COMMAND FAILED\n",cfgstruct.szTableName);
			fprintf (stderr, PQerrorMessage(conn));
			exit_nicely (conn);
		}
		PQclear(res);
		printf ("\n DONE\n");
	}
	
	/*
	* inserimenrto dei dati in tabelle PostgreSQL
	*/
	
	/* 
	* comincio la conversione dei dati 
	*/
	printf ("\nINSERT IN TABLE %s: ",cfgstruct.szTableName);
	nRecords=0;
	while(fgets(szBuffer,sizeof(szBuffer)-1,txt_fp)){
		/* 
		* Tipo record corretto 
		*/
		if((!DB.nRecordLen || strlen(szBuffer)>= DB.nRecordLen ) && 
			(!DB.nRecordTypeLen || !strncmp(szBuffer+DB.nRecordTypePos,DB.szRecordType,DB.nRecordTypeLen))){
			nLineNumber=0;

			/*
			* aggiungo un record al DataBase 
			*/
			nRecords++;
			strcpy (szSQLCmd,"INSERT INTO " );
			strcat (szSQLCmd, cfgstruct.szTableName);
			strcat (szSQLCmd," VALUES ( "); 

			nFieldPosition=DB.nDataStart;

			for(nFieldIndex=0;nFieldIndex<DB.nFieldsNumber;nFieldIndex++){

				/*
				* gestione offset
				*/
				if(DB.pFields[nFieldIndex]->nFieldOffset){
					nFieldPosition=DB.pFields[nFieldIndex]->nFieldOffset;
				}
				if(DB.pFields[nFieldIndex]->nFieldFlag & IMPORT_MASK){
					/* copio il dato */
					strncpy(szFieldString, szBuffer+nFieldPosition,DB.pFields[nFieldIndex]->nFieldLen);
					szFieldString[DB.pFields[nFieldIndex]->nFieldLen]='\0';

					/*
					* Conversione per il valori numerici
					*/
					if(DB.pFields[nFieldIndex]->cFieldType=='N'){
						sprintf(szFieldString,"%0d",atoi(szFieldString));
					}
					strcat (szSQLCmd, nFieldIndex?", ":" ");
					/* 
					* inserimento valore attributo
					*/
					if(DB.pFields[nFieldIndex]->cFieldType=='C'){
						strcat (szSQLCmd, "'");
					}
					/*
				 	* sostituzione del ' con uno spazio, in attesa di un soluzione migliore
				 	*/ 
					while ((pPtr=strchr(szFieldString,'\''))!=(char *)NULL){
						*pPtr = ' ';
					}
					strcat (szSQLCmd, szFieldString);
					if(DB.pFields[nFieldIndex]->cFieldType=='C'){
						strcat (szSQLCmd, "'");
					}
				}
				nFieldPosition+=DB.pFields[nFieldIndex]->nFieldLen;
			}
			strcat (szSQLCmd, ");");

			if (cfgstruct.bVerbose){
				t = time(NULL);
				fprintf (stdout, "\n%s\t- '%s'", ctime (&t), szSQLCmd);
			} else {
				if (((nRecords-1) % 100) == 0){
					fprintf(stdout,".");
					fflush(stdout);
				}
			}
			res = PQexec(conn, szSQLCmd);
			/* 
			* controllo l'esecuzione di PQexec() 
			*/
			if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
				fprintf (stderr, "\nINSERT IN TABLE %s: COMMAND FAILED\n",cfgstruct.szTableName);
				fprintf (stderr, "RECORD NUMBER %d\n", nRecords);
				fprintf (stderr, PQerrorMessage(conn));
				exit_nicely (conn);					
			}
			PQclear(res);
		}
		memset(szBuffer,'\0',sizeof(szBuffer));
	}
	fclose(txt_fp);

	/*
	* chiusura connessione con postgreSQL
	*/
	printf ("\n DONE : LETTI ED INSERTI %d RECORD\n\n", nRecords );
	PQfinish(conn);
}
