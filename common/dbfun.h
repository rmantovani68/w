/*
* modulo : dbfun.h
* -----------------
* Include file per libreria di gestione database
*
* Progetto Picking - DB Management - PostgreSQL
*
* Data creazione 13/12/1995
*
* Autore : Roberto Mantovani
*
* Copyright CNI srl 1995
*
* Modifiche
* -------------   -------------------------------------------------------
* rm 07-07-1998 : Aggiunta compilazione condizionale per CodeBase 4.5/5.1
* rm 18-01-1999 : Aggiunta la gestione multi linea per DeAgostini
* rm 23-02-1999 : Aggiunta struttura DB Array
* rm 16-06-2000 : Eliminati i riferimenti a CodeBase - si usa PGSQL
* -------------   -------------------------------------------------------
*/

#define DISPLAY_MASK  0x01
#define EDIT_MASK     0x02
#define IMPORT_MASK	  0x04
#define EXPORT_MASK	  0x08

#ifndef BOOL_TYPE_DEFINED
typedef unsigned int BOOL;
#ifndef FALSE
#define FALSE	0
#endif
#ifndef TRUE
#define TRUE 	1
#endif
#define BOOL_TYPE_DEFINED
#endif

#define MAX_FIELDS_NUMBER	64
#define MAX_TAGS_NUMBER		10
#define FIELD_TITLE_LEN   40
#define FIELD_DESCR_LEN   80

typedef struct tagTag {
	char szTagName[80];                      /* Tag Name */
	char szExpression[128];                  /* Expression */
	int  nOrder;                             /* Order */
} TAG, *PTAG;

typedef struct tagField {
	char szFieldName[20];                    /* Field Name */
	char szFieldTitle[FIELD_TITLE_LEN];      /* Field Title */
	char szFieldDescr[FIELD_DESCR_LEN];      /* Field Description */
	int  nFieldLen;                          /* Field Len */
	int  nFieldDec;                          /* Field Dec */
	char cFieldType;                         /* Field Type */
	unsigned  int nLineNumber;               /* Line Number */
	unsigned  int nFieldOffset;              /* Field Offset */
	unsigned  char nFieldFlag;               /* Field Flag */
	unsigned  int nDisplayLength;            /* Display Lenght*/
} FIELD, *PFIELD;

typedef struct tagDBStruct {
	char szDBName[80];                            /* Nome della base di dati */
	char szTableName[80];                         /* Nome della Tabella */
	PFIELD pFields[MAX_FIELDS_NUMBER];            /* puntatore all'array dei campi */
	TAG Tags[MAX_TAGS_NUMBER];	                  /* puntatore all'array dei tags (indici) */
	int nFieldsNumber;                            /* Numero dei campi contenuti in struttura */
	int nTagsNumber;                              /* Numero dei tags contenuti nell'indice */
	int nActualTag;                               /* indice corrente */
	int nRecordLen;                               /* Lunghezza Record ASCII */
	char szRecordType[80];                        /* Codice Tipo Record */
	int nRecordTypePos;                           /* Posizione Tipo Record */
	int nRecordTypeLen;                           /* Lunghezza Tipo Record */
	int nDataStart;                               /* Start of Data */
	int nLinesNumber;                             /* Number of Lines (multiline records) */
	int nLineNumberPos;                           /* Line Number Position (file ascii)*/
} DBSTRUCT, *PDBSTRUCT;

typedef struct {
	DBSTRUCT *pDB;
	char *szTableCfg;
} DB_ARRAY_STRUCT, *PDB_ARRAY_STRUCT;


int ReadFieldsInfo(char *szCfgFile,PDBSTRUCT pDB);
int ReadTagsInfo(char *szCfgFile,PDBSTRUCT pDB);
