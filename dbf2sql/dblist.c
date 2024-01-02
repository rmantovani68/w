/* 
* Modulo : sqlist.c
* ------------------
* - Visualizzazione di archivi postgreSQL
*
* Progetto: Easy Picking
*
* Data creazione Giugno 2000
*
* Autore : Roberto Mantovani - Stefano Tarroni
*
* Copyright CNI srl 2000 - 2005
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <malloc.h>
#include <libpq-fe.h>
#include "boolean.h"

#define MAX_FIELDS	512

typedef struct _config { 
	BOOL bOmitInfo;
	BOOL bUserID;
	BOOL bpghost;
	BOOL bpgport;
	BOOL bFieldList;
	BOOL bSort;
	int nSelectFields;
	int nUserID;
	char szTableName[128];
	char pszpghost[128];
	char pszpgport[128];
	char pszdbName[128];
	char *pszFieldList[MAX_FIELDS];
	char szSortExpression[128];
} CFGSTRUCT;


extern char *optarg;
extern int optind;

/*--------------------------------------------------------*/
/*-------------- function declaration --------------------*/
/*--------------------------------------------------------*/

void main(int argc, char **argv);
void PrintUsage (char **argv);
void exit_nicely (PGconn *conn);
void set_cfg_str (CFGSTRUCT *cfgstruct, int argc, char **argv); 

/*--------------------------------------------------------*/
/*--------------- function definition --------------------*/
/*--------------------------------------------------------*/

void PrintUsage(char **argv)
{
	printf("\nUsage : ");
	printf("dblist  <DataBaseName> <TableName> [ <field list> ] ...\n");
	printf("\t\t ... [-u <user_id> -h <hostname> -p <port> -o -?]\n\n");
	printf("<fields list> : Fields to be displayed - default all fields -\n");
	printf("-u <User ID> \n");
	printf("-h <Hostname> \n");
	printf("-p <Port> \n");
	printf("-o : Omit Fields Titles and Separation Spaces \n");
	printf("-s <sort expression>: sort by expression\n");
	printf("-? : Print This Help Message \n");
	printf("\n");
}

/*--------------------------------------------------------*/

void exit_nicely (PGconn *conn)	
{
	PQfinish(conn);
	exit(1);
}
	
/*--------------------------------------------------------*/

void set_cfg_str(CFGSTRUCT *cfgstruct, int argc, char **argv) 
/*
* settaggio dei parametri per la connessione
*/
{
	int nFieldIndex;	/* numero di campi da visualizzare */
	int nIndex=1;			/* indice dell'argomento attuale  della riga di comando */
	int nOpt;
	
	memset (cfgstruct, '\0', sizeof(CFGSTRUCT) );	
	while ((nOpt=getopt(argc, argv, "h:p:u:s:o?")) != EOF) {
		switch(nOpt){
			case 'h': /* hostname */
				cfgstruct->bpghost=TRUE;
				strcpy(cfgstruct->pszpghost,optarg);
				nIndex+=2;
				break;
			case 'p': /* port */
				cfgstruct->bpgport=TRUE;
				strcpy(cfgstruct->pszpgport,optarg);
				nIndex+=2;
				break;
			case 's': /* sort */
				cfgstruct->bSort=TRUE;
				strcpy(cfgstruct->szSortExpression,optarg);
				nIndex+=2;
				break;
			case 'u': /* User ID */
				cfgstruct->bUserID=TRUE;
				cfgstruct->nUserID = atoi(optarg);
				nIndex+=2;
				break;
			case'o':
				cfgstruct->bOmitInfo=TRUE;
				nIndex++;
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
	* la connessione (DataBaseName e TableName)
	*/
	
	if(argc==nIndex){
		printf("\nMissing DatabaseName\n");
		PrintUsage(argv);
		exit(1);
	} else {
		strcpy(cfgstruct->pszdbName, argv[nIndex]);
		++nIndex;
	}
	if(argc==nIndex){
		printf("\nMissing TableName\n");
		PrintUsage(argv);
		exit(1);
	} else {
		strcpy(cfgstruct->szTableName, argv[nIndex]);
		++nIndex;
	}
	
	/*
	* gestione lista di campi da visualizzare 
	*/
	if(argc>nIndex){
		cfgstruct->bFieldList=TRUE;
		for(nFieldIndex=nIndex;nFieldIndex<argc;nFieldIndex++){
			cfgstruct->pszFieldList[nFieldIndex-nIndex]=argv[nFieldIndex];
		}
		cfgstruct->nSelectFields=nFieldIndex-nIndex;
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
	CFGSTRUCT cfgstruct;	  /* The Cfg Data Struct */
	char szSQLCmd[1024];		/* Query da passare a posgreSQL */
	PGconn *conn;	        	/* Connessione al DataBase */
	PGresult *res;					/* Risultato della query */
	int nFieldIndex;
	FILE *fd_out = stdout;
	
	set_cfg_str(&cfgstruct, argc, argv); 
	
	/*	
	* connessione al database
	*/
	conn = PQsetdb(cfgstruct.pszpghost, cfgstruct.pszpgport, NULL, NULL, cfgstruct.pszdbName);
	
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
 	* controllo presenza della tabella indicata e degli eventuali campi
 	*/ 
	strcpy (szSQLCmd, "SELECT ");
	if (cfgstruct.bFieldList){
		for (nFieldIndex=0; nFieldIndex < cfgstruct.nSelectFields; nFieldIndex++){
			strcat (szSQLCmd, cfgstruct.pszFieldList[nFieldIndex]);
			strcat (szSQLCmd, (nFieldIndex<(cfgstruct.nSelectFields-1))?", ":" ");
		}
	} else {
		strcat (szSQLCmd, "* ");
	}
	
	strcat (szSQLCmd, "FROM ");
	strcat (szSQLCmd, cfgstruct.szTableName);
	if(cfgstruct.bSort){
		strcat (szSQLCmd, " order by ");
		strcat (szSQLCmd, cfgstruct.szSortExpression);
	}
	strcat (szSQLCmd, ";");
	
	res = PQexec(conn, szSQLCmd);
	/* 
	* controllo l'esecuzione di PQexec() 
	*/ 
	if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) {
		fprintf (stderr, "SELECT .. FROM %s : COMMAND FAILED\n",cfgstruct.szTableName);
		fprintf (stderr, "%s\n", szSQLCmd);
		fprintf (stderr, "%s\n", PQerrorMessage(conn));
		exit_nicely (conn);
	}
	
	PQdisplayTuples(res, fd_out, cfgstruct.bOmitInfo?0:1, cfgstruct.bOmitInfo?"":"|", cfgstruct.bOmitInfo?0:1, cfgstruct.bOmitInfo?1:0);

	PQclear(res);
	PQfinish(conn);
}
