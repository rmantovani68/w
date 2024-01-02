/* 
* Modulo : dbf2sql.c
* ------------------
* - Creazione di tabelle e indici postgreSQL utilizzando come modello file .cfg
* - Inserimento, aggiornamento, sostituzione di dati in tabelle, importandoli da file .dbf
*
* Progetto: Easy Picking 3.0
*
* Data creazione Maggio 2000
*
* Autore : Roberto Mantovani - Stefano Tarroni
*
* Copyright CNI srl 2000 - 2005
*
* 27-09-2000 rm+st : Aggiunto parametro -o per sostituzione di righe gia' esistenti.
*                    L'indice di ricerca e' il primo della lista (vedi .cfg)
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <malloc.h>
#include <libpq-fe.h>
#include "dbfun.h"		/* struttura delle tabelle del database (file.cfg) */
#include "dbf.h"			/* struttura di un file dbf */
#include "ca_file.h"	/* include per la gestione di un file.cfg */

#define MAX_FIELDS	512

typedef struct config { 
	BOOL bVerbose;	
	BOOL bReplaceInTable;
	BOOL bAddToTable;
	BOOL bUserID;
	BOOL bConfigFile;
	BOOL bDbfDataFile;
	BOOL bTableName;
	BOOL bpghost;
	BOOL bpgport;
	BOOL bdbName;
	BOOL bOverwrite;
	int nUserID;
	char szConfigFile[128];
	char szDbfDataFile[128];
	char szTableName[128];
	char szpghost[128];
	int npgport;
	char pszdbName[128];
} CFGSTRUCT;

extern char *optarg;
char szSQLCmd[1024];		/* Query da passare a posgreSQL */
PGconn *conn;	        	/* Connessione al DataBase */
PGresult *res;					/* Risultato della query */
DBF_HEAD dbf_head;


/*--------------------------------------------------------*/
/*-------------- function declaration --------------------*/
/*--------------------------------------------------------*/

void main(int argc, char **argv);
void PrintUsage (char **argv);
void exit_nicely (PGconn *conn);
void set_cfg_str (CFGSTRUCT *cfgstruct, int argc, char **argv); 
void cfg_create_table (CFGSTRUCT *cfgstruct, PDBSTRUCT pDB);
void cfg_create_index (CFGSTRUCT *cfgstruct, PDBSTRUCT pDB);
void dbf_insert_in_table (int fd, CFGSTRUCT *cfgstruct, PDBSTRUCT pDB);
BOOL ReadFieldsStructFromDBF (int fd, PDBSTRUCT pDB);


/*--------------------------------------------------------*/
/*--------------- function definition --------------------*/
/*--------------------------------------------------------*/

void PrintUsage(char **argv)
{
	printf("\nUsage:\n");
	printf("dbf2sql [-d <database> -c <cfg_file> -f <dbf_file> -t <table_name> ...\n");
	printf("\t ... -u <user_id> -h <hostname> -p <port> -a -r -v -?]\n\n");
	printf("\t-d <Name of the destination DataBase> \n");
	printf("\t-c <Source Configuration_File> \n");
	printf("\t-f <dbf Data_File> \n");
	printf("\t-t <Name of the destination Table> - default is the name of the .CFG - \n");
	printf("\t-u <User ID> \n");
	printf("\t-h <Hostname> \n");
	printf("\t-p <Port> \n");
	printf("\t-a : Add data in existing table \n");
	printf("\t-r : Replace data in existing table \n");
	printf("\t-o : Overwrite identical rows\n");
	printf("\t-v : Verbose sql query, date and time\n");
	printf("\t-? : Print This Help Message \n");
	printf("\nOption need for:\n");
	printf("\n\t - Only create a table        : -d -c    [-t]");
	printf("\n\t - Create and insert in table : -d -c -f [-t]");
	printf("\n\t - Add data in a table        : -d -c -f [-t] -a");
	printf("\n\t - Add data  with overwrite)  : -d -c -f [-t] -a -o");
	printf("\n\t - Replace data in table      : -d -c -f [-t] -r");
	printf("\n\n");
}

/*--------------------------------------------------------*/

void exit_nicely (PGconn *conn)	
{
	PQfinish(conn);
	exit(1);
}
	
/*--------------------------------------------------------*/
BOOL ReadFieldsStructFromDBF(int fd, PDBSTRUCT pDB)
{
	BOOL bmore_fields = TRUE;
	BOOL bRetValue = TRUE;
	FIELD_REC field_rec[MAX_FIELDS];
	int nFieldIndex;
	int status;
	PFIELD pField;

	/* 
	* lista dei campi 
	*/
	if(lseek(fd,(long)sizeof(dbf_head),0)==-1){
		fprintf(stderr,"Error seeking in database file\n");
		bRetValue = FALSE;
	}

	/* 
	* conteggio campi del record (nFieldNumber) 
	*/
	nFieldIndex=0;
	while(bRetValue && bmore_fields){
		status=read(fd,(char *)&field_rec[nFieldIndex],sizeof(FIELD_REC));
		bmore_fields=(status==sizeof(FIELD_REC));
		if (bmore_fields){
			bmore_fields=(field_rec[nFieldIndex].field_name[0] != 0x0D);
			if (bmore_fields){
				pDB->pFields[nFieldIndex]=(PFIELD)malloc(sizeof(FIELD));
				memset(pDB->pFields[nFieldIndex],0,sizeof(FIELD));

				pField=pDB->pFields[nFieldIndex];

				strcpy(pField->szFieldName,field_rec[nFieldIndex].field_name);
				pField->cFieldType=field_rec[nFieldIndex].field_type;
				switch(pField->cFieldType){
					case 'D':
						pField->nFieldLen=0;
						pField->nFieldDec=0;
					break;
					case 'N':
						pField->nFieldLen=field_rec[nFieldIndex].len_info.num_size.len;
						pField->nFieldDec=field_rec[nFieldIndex].len_info.num_size.dec;
					break;
					default:
						pField->nFieldLen=field_rec[nFieldIndex].len_info.char_len;
						pField->nFieldDec=0;
					break;

				}
				nFieldIndex++;
			}
		}
	}
	pDB->nFieldsNumber=nFieldIndex;

	return bRetValue;;
}

/*--------------------------------------------------------*/

/*
*	void dbf_insert_in_table (int fd, CFGSTRUCT *cfgstruct, PDBSTRUCT pDB)
* inserisce i dati in tabella utilizzando un file dbf
*/
void dbf_insert_in_table (int fd, CFGSTRUCT *cfgstruct, PDBSTRUCT pDB)
{
	int nFieldOffset;
	int nFieldIndex;
	unsigned rec_num;
	char *record;
	char szFieldData[256];
	char *pPtr;
	time_t t;
	PFIELD pField;
	int nKeyFieldIndex=-1;
	char szKeyFieldData[256];

	fprintf (stdout, "\nINSERT IN TABLE %s: ",cfgstruct->szTableName);
	if(cfgstruct->bOverwrite){
		fprintf (stdout, "(WITH OVERWRITE)");
	}
	/* 
	* lista dei campi 
	*/
	if(lseek(fd,(long)sizeof(dbf_head),0)==-1){
		fprintf(stderr,"Error seeking in database file\n");
		exit_nicely (conn);
	}

	if(lseek(fd,(long)dbf_head.data_offset,0)==-1){
		fprintf(stderr,"Error seeking in database file\n");
		exit_nicely (conn);
	}
	record=(char *)malloc(dbf_head.rec_size+1);
	record[dbf_head.rec_size]='\0';

	if(cfgstruct->bOverwrite){
		if(pDB->pIndex[0]==(PINDEX)NULL){
			fprintf(stderr,"Errore : Overwrite e Indice [0] non presente !!!\n");
			exit_nicely (conn);
		}
		for(nFieldIndex=0; nKeyFieldIndex==-1 || nFieldIndex<pDB->nFieldsNumber; nFieldIndex++){
			if(!strcmp(pDB->pFields[nFieldIndex]->szFieldName,pDB->pIndex[0]->szExpression)){
				nKeyFieldIndex=nFieldIndex;
			}
		}
		if(nKeyFieldIndex==-1){
			fprintf(stderr,"Errore : Overwrite attivo e campo indice non presente [%s]\n",pDB->pIndex[0]->szExpression);
			exit_nicely (conn);
		}
	}
	/*
	* generazione della stringa di inserimento tupla
	*/	
	for(rec_num=1;rec_num<=dbf_head.last_rec;rec_num++){
		if(cfgstruct->bOverwrite){
			strcpy (szSQLCmd,"UPDATE " );
			strcat (szSQLCmd, cfgstruct->szTableName);
			strcat (szSQLCmd," SET "); 
		} else {
			strcpy (szSQLCmd,"INSERT INTO " );
			strcat (szSQLCmd, cfgstruct->szTableName);
			strcat (szSQLCmd," VALUES ( "); 
		}
		
		read(fd,record,dbf_head.rec_size);
		nFieldOffset=1;
		for(nFieldIndex=0; nFieldIndex<pDB->nFieldsNumber; nFieldIndex++){
			pField=pDB->pFields[nFieldIndex];
			strncpy(szFieldData, record+nFieldOffset, pField->nFieldLen);
			szFieldData[pField->nFieldLen]='\0';
			if(cfgstruct->bOverwrite && nFieldIndex==nKeyFieldIndex) {
				strcpy(szKeyFieldData,szFieldData);
			}
			/* sostituzione dell'apostrofo (') */
			while((pPtr=strchr(szFieldData,'\''))!=(char *)NULL){
				*pPtr = '"';
			}
			/* 
			* aggiorno il field offset 
			*/
			nFieldOffset+=pField->nFieldLen;
			/*
			* inserimento della ','
			*/
			strcat (szSQLCmd, nFieldIndex?", ":" ");

			/*
			* Se sono in overwrite inserisco il nome del campo prima 
			* della assegnazione a valore
			*/
			if(cfgstruct->bOverwrite){
				strcat (szSQLCmd, pField->szFieldName);
				strcat (szSQLCmd, "=");
			}

			/* 
			* inserimento valore attributo
			*/
			if(pField->cFieldType=='C'){
				strcat (szSQLCmd, "'");
			}
			/*
			* Conversione per il valori numerici
			*/
			if(pField->cFieldType=='N'){
				sprintf(szFieldData,"%0d",atoi(szFieldData));
			}
			strcat (szSQLCmd, szFieldData);

			if(pField->cFieldType=='C'){
				strcat (szSQLCmd, "'");
			}
		}
	
		/*
		* Se sono in overwrite inserisco la condizione di ricerca
		*/
		if(cfgstruct->bOverwrite){
			strcat (szSQLCmd, " WHERE ");
			strcat (szSQLCmd, pDB->pIndex[0]->szExpression);
			strcat (szSQLCmd, " = ");

			if(pDB->pFields[nKeyFieldIndex]->cFieldType=='C'){
				strcat (szSQLCmd, "'");
			}
			/*
			* Conversione per il valori numerici
			*/
			if(pDB->pFields[nKeyFieldIndex]->cFieldType=='N'){
				sprintf(szKeyFieldData,"%0d",atoi(szKeyFieldData));
			}
			strcat (szSQLCmd, szKeyFieldData);

			if(pDB->pFields[nKeyFieldIndex]->cFieldType=='C'){
				strcat (szSQLCmd, "'");
			}
			strcat (szSQLCmd, ";");
		} else {
			strcat (szSQLCmd, ");");
		}
	
		if (cfgstruct->bVerbose){
			t = time(NULL);
			fprintf (stdout, "\n%s\t- '%s'", ctime (&t), szSQLCmd);
		} else {
			/*
			* visualizzo sullo stdout un '.' ogni 100 record inseriti in tabella
			*/
			if (((rec_num-1) % 100) == 0){
				fprintf(stdout,".");
				fflush(stdout);
			}
		}
		/*
		* eseguo il comando
		*/
		res = PQexec(conn, szSQLCmd);
		/* 
		* controllo l'esecuzione di PQexec() 
		*/
		if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
			fprintf (stderr, "\nINSERT IN TABLE %s: COMMAND FAILED\n",cfgstruct->szTableName);
			fprintf (stderr, "RECORD NUMBER[%d]\n", rec_num);
			fprintf (stderr, "SQL CMD      [%s]\n", szSQLCmd);
			fprintf (stderr, PQerrorMessage(conn));
			/*
			exit_nicely (conn);
			*/
		}
		PQclear(res);
	}
	fprintf (stdout, "\n DONE : LETTI E INSERITI %d RECORDS \n\n", rec_num );
	free(record);
}



/*--------------------------------------------------------*/

/*
* void cfg_create_table (CFGSTRUCT *cfgstruct, PDBSTRUCT pDB)
* crea la tabella utilizzando i dati contenuti in un file .cfg
*/ 
void cfg_create_table (CFGSTRUCT *cfgstruct, PDBSTRUCT pDB)
{
	time_t t;
	int nFieldIndex;
	char szTmpBuffer[128];

	fprintf (stdout, "\nCREATE TABLE %s:",cfgstruct->szTableName);
	/*
	* inserimento nome tabella
	*/
	strcpy (szSQLCmd, "CREATE TABLE ");
	strcat (szSQLCmd, cfgstruct->szTableName);
	strcat (szSQLCmd, " (");
	
	if (cfgstruct->bConfigFile){
		for (nFieldIndex=0; nFieldIndex < pDB->nFieldsNumber; nFieldIndex++){
			/*
			* inserimento nome dell'attributo
			*/
			strcat (szSQLCmd, (nFieldIndex==pDB->nFieldsNumber || nFieldIndex==0)?" ":", ");
			strcat (szSQLCmd, pDB->pFields[nFieldIndex]->szFieldName);
			/* 
			* inserimento tipo dell'attributo 
			*/
			switch(pDB->pFields[nFieldIndex]->cFieldType){
				case 'N': /* numero  */
					if(pDB->pFields[nFieldIndex]->nFieldDec == 0){
						if(pDB->pFields[nFieldIndex]->nFieldLen <= 4){
							strcat (szSQLCmd, " INT2");
						} else if (pDB->pFields[nFieldIndex]->nFieldLen <= 9){
							strcat (szSQLCmd, " INT4");
						} else {
							strcat (szSQLCmd, " INT8");
						}
					} else {
						if(pDB->pFields[nFieldIndex]->nFieldDec <= 6){
							strcat (szSQLCmd, " FLOAT4");
						} else {
							strcat (szSQLCmd, " FLOAT8");	
						}
					}
					break;
				case 'C': /* carattere/i  */
					sprintf(szTmpBuffer, " CHAR(%d)", pDB->pFields[nFieldIndex]->nFieldLen);
					strcat (szSQLCmd, szTmpBuffer);
					break;
				case 'L': /* logico */
					strcat (szSQLCmd, " BOOL");
					break;
				case 'D': /* data */
					strcat (szSQLCmd, " TIMESTAMP");
					break;
				default:
					strcat (szSQLCmd, " TEXT");
					break;
			}
		} 
		strcat (szSQLCmd, " );");
		
		if (cfgstruct->bVerbose){
			t = time(NULL);
			fprintf (stdout, "%s\t- '%s'\n", ctime (&t), szSQLCmd);
		}
    
		res = PQexec(conn, szSQLCmd);
		
		/* 
		* controllo l'esecuzione di PQexec() 
		*/ 
		if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
			fprintf (stderr, "\nCREATE TABLE %s: COMMAND FAILED\n",cfgstruct->szTableName);
			fprintf (stderr, PQerrorMessage(conn));
			exit_nicely (conn);
		} else {
			fprintf (stdout, "\n DONE\n");
		}
		PQclear(res);
	}
}

/*--------------------------------------------------------*/

/*
* void cfg_create_index (CFGSTRUCT *cfgstruct, PDBSTRUCT pDB)
* crea gli indici della tabella utilizzando un file cfg
*/ 
void cfg_create_index (CFGSTRUCT *cfgstruct, PDBSTRUCT pDB)
{
	time_t t;
	int i;
	char *pPtr;
	char szIndexField[512];

	/*
	*                   ! ! !   N.B.   ! ! !
	* creazione indici: al nome dell'indice viene anteposto il nome della
	* tabella per evitare indici aventi lo stesso nome all'interno del DataBase
	*/
	fprintf (stdout, "\nCREATE INDEXES ON %s:\n",cfgstruct->szTableName);
	for (i=0; i<pDB->nTagsNumber; i++){
		strcpy (szSQLCmd, "CREATE ");
		if(pDB->pIndex[i]->nUniqueFlag){
			strcat (szSQLCmd, "UNIQUE ");
		}
		strcat (szSQLCmd, "INDEX ");
		strcat (szSQLCmd, cfgstruct->szTableName);
		fprintf (stdout, cfgstruct->szTableName);
		strcat (szSQLCmd, "_");
		fprintf (stdout, "_");
		strcat (szSQLCmd, pDB->pIndex[i]->szIndexName);
		fprintf (stdout, "%s  ", pDB->pIndex[i]->szIndexName);
		strcat (szSQLCmd, " ON ");
		strcat (szSQLCmd, cfgstruct->szTableName);
		strcat (szSQLCmd, " ( ");
		strcpy (szIndexField, pDB->pIndex[i]->szExpression);
		while ((pPtr=strchr(szIndexField, '+')) != (char *)NULL){
			*pPtr = ',';						/* sostituzione del '+' con la ','*/ 
		}		
		strcat (szSQLCmd, szIndexField);
		strcat (szSQLCmd, " );");
		if(pDB->pIndex[i]->nUniqueFlag){
			fprintf (stdout, "UNIQUE");
		}
		fprintf (stdout, "\n");
		
		if (cfgstruct->bVerbose){
			t = time(NULL);
			fprintf (stdout, "\n%s\t- '%s'", ctime (&t), szSQLCmd);
		}
		
		res = PQexec(conn, szSQLCmd);
		/* 
		* controllo l'esecuzione di PQexec() 
		*/ 
		if (!res || PQresultStatus(res) != PGRES_COMMAND_OK) {
			fprintf (stderr, "CREATE INDEX %s: COMMAND FAILED\n",pDB->pIndex[i]->szIndexName);
			fprintf (stderr, PQerrorMessage(conn));
			exit_nicely (conn) ;
		}
		PQclear(res);
	}
	fprintf (stdout, "DONE\n");
}

/*--------------------------------------------------------*/

/*
* settaggio dei parametri per la connessione
*/
void set_cfg_str(CFGSTRUCT *cfgstruct, int argc, char **argv) 
{
	int nOpt;
	char *pPtr;
	char szTableName_aux[128];	/* nome della tabella ricavato dal nome del file cfg o dbf */
	
	/*
	* Inizializzazione struttura CFG
	*/
	memset (cfgstruct, '\0', sizeof(CFGSTRUCT) );	

	while ((nOpt = getopt(argc, argv, "c:d:f:h:p:t:u:aorv?")) != EOF) {
		switch(nOpt){
			case 'a': /* add to table */
				cfgstruct->bAddToTable=TRUE;
				break;
			case 'c': /* Configuration File */
				cfgstruct->bConfigFile=TRUE;
				strcpy(cfgstruct->szConfigFile ,optarg);
				break;
			case 'd': /* database */
				cfgstruct->bdbName=TRUE;
				strcpy(cfgstruct->pszdbName ,optarg);
				break;
			case 'f': /* dbf data */
				cfgstruct->bDbfDataFile=TRUE;
				strcpy(cfgstruct->szDbfDataFile ,optarg);
				break;
			case 'h': /* hostname */
				cfgstruct->bpghost=TRUE;
				strcpy(cfgstruct->szpghost,optarg);
				break;
			case 'p': /* port */
				cfgstruct->bpgport=TRUE;
				cfgstruct->npgport = atoi(optarg);
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
			case 'o': /* Sostituzione righe gia' presenti */
				cfgstruct->bOverwrite=TRUE;
				break;
			case 'v': /* visualizza a video i comandi */
				cfgstruct->bVerbose=TRUE;
				break;
			case '?':
			default:
				PrintUsage(argv);
				exit(1);
				break;
		}
	}
	
	/*
	* controllo presenza delle opzioni fondamentali per
	* la connessione ((DbfData o ConfigFile) e DataBase)
	*/
	if (!(cfgstruct->bdbName && cfgstruct->bConfigFile)) {
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
/*------------------------ MAIN --------------------------*/
/*--------------------------------------------------------*/

void main(int argc,char **argv)
{
	CFGSTRUCT cfgstruct;	/* The Cfg Data Struct */
	DBSTRUCT DB;
	time_t t;
	int dbf_fd;						/* file descriptor del file.dbf */
	
	if(argc==1){
		PrintUsage(argv);
		exit(1);
	}

	set_cfg_str(&cfgstruct, argc, argv); 

	/*
	* Gestione Configuration File
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
	* apertura del file .dbf
	*/
 	if (cfgstruct.bDbfDataFile){
		dbf_fd=open(cfgstruct.szDbfDataFile, O_RDONLY);
		if (dbf_fd == -1){
			printf("Error opening %s\n", cfgstruct.szDbfDataFile);
			exit(1);
		}
		if (read(dbf_fd,(char *)&dbf_head,sizeof(dbf_head))!=sizeof(DBF_HEAD)) {
			printf("Read error\n");
			exit_nicely (conn);
		}
	}
	
	/*
	* connessione al database
	*/
	conn = PQsetdb(cfgstruct.szpghost, cfgstruct.npgport, NULL, NULL, cfgstruct.pszdbName);
	
	/*
	* controllo correttezza della connessione
	*/
	if (PQstatus(conn) == CONNECTION_BAD) {
		fprintf(stderr, "Connection to database '%s' failed.\n", cfgstruct.pszdbName);
		fprintf(stderr, "%s", PQerrorMessage(conn));
		exit_nicely (conn);
	} else {
		fprintf(stderr, "Connection to database '%s' OK.\n", cfgstruct.pszdbName);
	}

	/*
	* creazione della tabella
	*/ 
	if (!(cfgstruct.bAddToTable || cfgstruct.bReplaceInTable)){
		cfg_create_table(&cfgstruct, &DB);
	}
	
	if (cfgstruct.bReplaceInTable) {
		/*
		* eliminazione delle tuple presenti
		*/
		printf ("\nELIMINAZIONE RECORD PRESENTI IN TABELLA :");
		strcpy (szSQLCmd, "DELETE FROM ");
		strcat (szSQLCmd, cfgstruct.szTableName);
		strcat (szSQLCmd, ";");
		if (cfgstruct.bVerbose){
			t = time(NULL);
			fprintf (stdout, "\n%s\t- '%s'", ctime (&t), szSQLCmd);
			}
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
	* inserimento dati
	*/
	if (cfgstruct.bDbfDataFile) {
		dbf_insert_in_table(dbf_fd, &cfgstruct, &DB);
		close(dbf_fd);
	}	

	/*
	* creazione degli indici
	*/ 
	if (!(cfgstruct.bAddToTable || cfgstruct.bReplaceInTable)){
		cfg_create_index(&cfgstruct, &DB);
		printf("\n");
	}
	PQfinish(conn);
}
