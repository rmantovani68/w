/*
* modulo : dbfun.h
* -----------------
* Include file per libreria di gestione database
*
* Progetto EMI - Gestione Resi
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
* rm 15-05-2000 : Versione per dbf2sql (senza CB)
* -------------   -------------------------------------------------------
*/
#ifndef _dbfun_
#define _dbfun_


#define DISPLAY_MASK  0x01
#define EDIT_MASK     0x02
#define IMPORT_MASK	  0x04

#ifndef BOOL_TYPE_DEFINED
typedef unsigned int BOOL;
#define FALSE	0
#define TRUE 	1
#define BOOL_TYPE_DEFINED
#endif

#define MAX_FIELDS_NUMBER	64
#define MAX_TAGS_NUMBER		10
#define FIELD_TITLE_LEN   40
#define FIELD_DESCR_LEN   80

typedef struct TagField {
	char szFieldName[20];                    /* Field Name */
	char szFieldTitle[FIELD_TITLE_LEN];      /* Field Title */
	char szFieldDescr[FIELD_DESCR_LEN];      /* Field Description */
	int  nFieldLen;                          /* Field Len */
	int  nFieldDec;                          /* Field Dec */
	char cFieldType;                         /* Field Type */
	unsigned  int nLineNumber;               /* Line Number */
	unsigned  int nFieldOffset;              /* Field Offset */
	unsigned  char nFieldFlag;               /* Field Flag */
} FIELD, *PFIELD;

typedef struct TagIndex {
	char szIndexName[512];                   /* Index Name */
	char szExpression[512];                  /* Index Expression */
	int  nUniqueFlag;                        /* Unique Flag (0:NO 1:YES) */
} INDEX, *PINDEX;

typedef struct tagDBStruct {
	char szDBName[80];
	int nFieldsNumber;                            /* Numero dei campi contenuti in struttura */
	int nTagsNumber;                              /* Numero dei tags contenuti nell'indice */
	PFIELD pFields[MAX_FIELDS_NUMBER];            /* puntatore all'array dei campi */
	PINDEX pIndex[MAX_TAGS_NUMBER];               /* puntatore all'array degli indici */
	long lActualRecord;		                      /* record corrente */
	char szActualTag[128];                        /* indice corrente */
	int nRecordLen;                               /* Lunghezza Record ASCII */
	char szRecordType[80];                        /* Codice Tipo Record */
	int nRecordTypePos;                           /* Posizione Tipo Record */
	int nRecordTypeLen;                           /* Lunghezza Tipo Record */
	int nDataStart;                               /* Start of Data */
	int nLinesNumber;                             /* Number of Lines (multiline records) */
	int nLineNumberPos;                           /* Line Number Position (file ascii) */

} DBSTRUCT, *PDBSTRUCT;

typedef struct {
	DBSTRUCT *pDB;
	char *szDBNameItem;
	char *szDBNameDefault;
	char *szDBCfgItem;
	char *szDBCfgDefault;
	char *szPath;
} DB_ARRAY_STRUCT, *PDB_ARRAY_STRUCT;


int ReadFieldStruct(char *szCfgFile,PDBSTRUCT pDB);
int ReadTagInfo(char *szCfgFile,PDBSTRUCT pDB);
char *i_strtrimall(char *);


#endif /* _dbfun_ */
