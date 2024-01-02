/*
* ep.h
* ----------
* Include per funzioni EasyPicking
*
* Data creazione : Fri May 31 17:33:07 CEST 2002
*/

#ifndef _ep_h_
#define _ep_h_

#ifndef UINT_TYPE_DEFINED
typedef unsigned int UINT;
#define UINT_TYPE_DEFINED
#endif

#ifndef BOOL_TYPE_DEFINED
	typedef unsigned int BOOL;
	#define BOOL_TYPE_DEFINED

	#ifndef	FALSE
	#define	FALSE	(0)
	#endif

	#ifndef	TRUE
	#define	TRUE	(!FALSE)
	#endif
#endif

#ifndef MIN_MAX_DEFINED
	#define max(a,b)	((a)>=(b)?(a):(b))
	#define min(a,b)	((a)<=(b)?(a):(b))
#endif


#ifndef MIN_MAX_DEFINED
#define max(a,b)	((a)>=(b)?(a):(b))
#define min(a,b)	((a)<=(b)?(a):(b))
#endif

#define CHAR_LF					0x0A
#define CHAR_CR					0x0D
#define BACK_EXTENSION	".back"

typedef void DBconn;
typedef void DBresult;

typedef enum
{
	DBRES_EMPTY_QUERY = 0,
	DBRES_COMMAND_OK,
	DBRES_TUPLES_OK,
	DBRES_COPY_OUT,
	DBRES_COPY_IN,
	DBRES_BAD_RESPONSE,
	DBRES_NONFATAL_ERROR,
	DBRES_FATAL_ERROR
} DBExecStatusType;

typedef enum
{
	DB_TYPE_NONE,
	DB_TYPE_STRING,
	DB_TYPE_INTEGER,
	DB_TYPE_FLOAT,
	DB_TYPE_DATE,
	DB_TYPE_TIME,
	DB_TYPE_DATETIME,
	DB_TYPE_TIMESTAMP,
	DB_TYPE_YEAR
} DBFieldType;

typedef struct _DbType {
	int type_id;
	char *type_name;
	DBFieldType type;
} DbType;

typedef enum {
	CFG_TYPE_NONE,
	CFG_TYPE_BOOL,
	CFG_TYPE_INT,
	CFG_TYPE_STRING
} CFG_ITEM_TYPE;

typedef enum {
	CFG_NO_WIDGET,
	CFG_SPIN_BUTTON_WIDGET,
	CFG_ENTRY_WIDGET,
	CFG_TOGGLE_BUTTON_WIDGET,
} CFG_WIDGET_TYPE;

typedef union tagCfgValue {
	unsigned int *pbBool;
	int  *pnInt;
	char *pszString;
} CFGVALUE, *PCFGVALUE;

typedef union tagCfgDefaultValue {
	unsigned int bBool;
	int  nInt;
	char *szString;
} CFGDEFAULTVALUE, *PCFGDEFAULTVALUE;

typedef struct tagCfgItem {
	char *pszParagraph;
	char *pszItemName;
	CFG_ITEM_TYPE ItemType;
	CFGVALUE ItemValue;
	CFGDEFAULTVALUE ItemDefaultValue;
	CFG_WIDGET_TYPE WidgetType;
	char *pszWidgetName;
} CFGITEM, *PCFGITEM;

typedef struct tagCfgArray{
	CFGITEM *Items;
} CFGARRAY, *PCFGARRAY;

typedef	struct	processo {
	char	szProcName[128]; /* nome file eseguibile */
	int   nQKey;           /* chiave della coda utilizzata */
	int		nQNumber;        /* numero della coda utilizzata */
	int		bCreate;         /* flag per creazione coda o sola apertura*/
	int		nGId;            /* numero processo*/
	int		nProcId;         /* pid del processo*/
	int		bKill;           /* 1 uccidere con kill, 0 inviare MSG_ENDMOD*/
	int		nPriority;       /* Priority Offset */
	int		bExecute;        /* 1 eseguire, 0 non eseguire */
}	PROCESSO;

char *DeleteNewLine(char *str);
char *ElimNewLine(char *str);
char *GetDate(long time,char *szDateString);
char *GetDateDDMM(long time,char *szDateString);
char *GetDateDDMMYYYY(long time,char *szDateString);
char *GetDateFromDDMMYY(char *szFromDate,char *szDateString);
char *GetDateFromYYYYMMDD(char *szFromDate,char *szDateString);
char *GetDateYYMMDD(long time,char *szDateString);
char *GetDateYYMMFromDDMMYY(char *szFromDate,char *szDateString);
char *GetDateYYYYMMDD(long time,char *szDateString);
char *GetDateYYYYMMDDFromDDMMYY(char *szFromDate,char *szDateString);
char *GetDatef(char *szDateFormat);
char *GetTimeSpanHHMMSS(const char *szFromTime, const char* szToTime, char *szTimeString);
char *GetTime(long time,char *szTimeString);
char *GetTimeFromHHMMSS(char *szFromTime,char *szTimeString);
char *GetTimeHHMM(long time,char *szTimeString);
char *GetTimeHHMMSS(long time,char *szTimeString);
char *GetDateYYYYMMDDFromDD_MM_YYYY(char *szFromDate,char *szDateString);
char *GetDateYYYYMMDDFromYYYY_MM_DD(char *szFromDate,char *szDateString);
char *LeftStr(char *szString,int nLen);
char *PadZero(char *szString,int nLen);
char *RightPad(char *szString,int nLen);
char *LeftPad(char *szString,int nLen);
char *RightStr(char *szString,int nLen);
char *StrTrimAll(char *str);
char *StrTrimLeft(char *str);
char *StrTrimRight(char *str);
char *SubStr(char *szString,int nPos,int nLen);
char *i_strtrimall(char *str);
int CmpFileDate(char *pszFileName);
BOOL FileExists(char *szFileName);
BOOL StrIsAlpha(char *str);
BOOL StrIsDigit(char *str);
BOOL StrIsAlnum(char *str);
BOOL StrIsSpace(char *str);
BOOL StrIsLower(char *str);
BOOL StrIsUpper(char *str);
BOOL StrIsXdigit(char *str);
BOOL StrIsAscii(char *str);
char *Substitute(char *szString,char c1,char c2);
BOOL RunSimpleProcess(PROCESSO *ProcList,int nProcID,char *szPath);
void TerminateProcess(PROCESSO *PrcoList,int nProcID);
void ReadProcInfo(PROCESSO *ProcList,char *szFileConfig);
BOOL SendMessage(PROCESSO *ProcList,int nDest,int nSrce,int nMsgCode,char *szText);

char	*GetFileBuffer	(char *, FILE *);
char	*GetBufferParagraph	(char *, char *, char *);
BOOL	GetBufferString	(char *, char *, char *, char *, int, char *);
int		GetBufferInt		(char *, char *, int, char *);
BOOL	GetFileString	(char *, char *, char *, char *, int, char *, FILE *);
int		GetFileInt		(char *, char *, int, char *, FILE *);
BOOL	PutFileString	(char *, char *, char *, char *, FILE *);
BOOL	PutFileInt		(char *, char *, int, char *, FILE *);
BOOL	DelFileString	(char *, char *, char *, FILE *);
BOOL	DelFileParagraph	(char *, char *, FILE *);
BOOL	PutFileBuffer	(char *, char *, long);
BOOL	BackupFile		(char *);


BOOL ReadCfgItems(PCFGITEM pItems,char *pszCfgFileName);
BOOL WriteCfgItems(PCFGITEM pItems,char *pszCfgFileName);

#define DISPLAY_MASK  0x01
#define EDIT_MASK     0x02
#define IMPORT_MASK	  0x04
#define EXPORT_MASK	  0x08
#define SELECT_MASK	  0x10

#define MAX_FIELDS_NUMBER	256
#define MAX_TAGS_NUMBER		 64
#define FIELD_TITLE_LEN    40
#define FIELD_DESCR_LEN    80

typedef struct tagTag {
	char szTagName[80];                      /* Tag Name */
	char szExpression[128];                  /* Expression */
	int  nUnique;                            /* Unique */
} TAG, *PTAG;

typedef struct tagField {
	char szFieldName[20];                    /* Field Name */
	char szFieldTitle[FIELD_TITLE_LEN];      /* Field Title */
	char szFieldDescr[FIELD_DESCR_LEN];      /* Field Description */
	int  nFieldLen;                          /* Field Len */
	int  nFieldDec;                          /* Field Dec */
	char cFieldType;                         /* Field Type */
	unsigned int nLineNumber;               /* Line Number */
	unsigned int nFieldOffset;              /* Field Offset */
	unsigned char nFieldFlag;               /* Field Flag */
	unsigned int nDisplayLength;            /* Display Lenght*/
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
	char szCfgPath[128];
	char szCfgName[128];
} DB_ARRAY_ITEM_STRUCT, *PDB_ARRAY_ITEM_STRUCT;


char *NormalizeString(char *szString);
char *NormalizeFormat(char *szString);
BOOL ReadTableInfo(PDBSTRUCT pDB,char *szPath,char *szCfgBaseName);
void ReadTablesInfo(void);
void InsertTableInfo(PDBSTRUCT pDB,char *szCfgPath,char *szCfgName);
void FreeTablesInfo(void);
BOOL IsAsciiRecord(char *szBuffer,PDBSTRUCT pDB);
BOOL GetAsciiRecord(char *szBuffer,PDBSTRUCT pDB);
BOOL UpdateAsciiRecord(char *szBuffer, PDBSTRUCT pDB, BOOL bUpdate);
BOOL WriteAsciiRecord(FILE *fp,PDBSTRUCT pDB,DBresult *DBRes,int nIndex,BOOL bFillZero,BOOL bSigned);
int ReadFieldsInfo(char *szCfgFile,PDBSTRUCT pDB);
int ReadTagsInfo(char *szCfgFile,PDBSTRUCT pDB);
BOOL DBConnect(char *szHost,char *szPort,char *szDataBase);
BOOL DBDisconnect(void);
BOOL DBConnectionActive(void);
DBresult *DBExecQuery(BOOL bTrace,char *format, ...) __attribute__ ((format(printf,2,3)));
DBExecStatusType DBresultStatus(DBresult *DBRes);
char *DBcmdTuples(DBresult *DBRes);
void DBFreeQueryResult(DBresult *DBRes);
int DBntuples(DBresult *DBRes);
int DBnfields(DBresult *DBRes);
unsigned int DBgetlength(DBresult *DBRes,int nTuple,int nField);
char *DBfname(DBresult *DBRes,int nField);
int DBftype(DBresult *DBRes,int nField);
char *DBgetvalue(DBresult *DBRes,int nTuple,int nField);
char *DBresultErrorMessage(DBresult *DBRes);
DBFieldType db_get_field_type(int nType);

#ifdef USE_POSTGRES
#include <ep_pgsql.h>
#endif

#endif /* _ep_h_ */
