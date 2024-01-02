/*
* printlabel.h
* 
* Progetto Mondadori Picking
*
* Autore : Roberto Mantovani
*
* Copyright CNI srl 1998
*/
#ifndef BOOL_TYPE_DEFINED
typedef unsigned int BOOL;
#define FALSE	0
#define TRUE 	1
#define BOOL_TYPE_DEFINED
#endif

#define LEFT	0
#define RIGHT	1

#ifndef MIN_MAX_DEFINED
#define max(a,b)	((a)>=(b)?(a):(b))
#define min(a,b)	((a)<=(b)?(a):(b))
#endif

enum ERRORI_BARTOLINI{
	NO_ERROR_BART,
	ERR_VER,
	ERR_COD,
	ERR_KEYCAP,
	ERR_LOC,
	ERR_LOCSIN,

	ERROR_NUMBER_BART
};
	
enum ERRORI_STAMPA_ETICHETTE{
	NO_ERROR_ETI,
	PROG_NOT_FOUND_ORDINI,
	PROG_NOT_FOUND_COLLI,
	PROV_NOT_FOUND,
	VETT_NOT_FOUND,
	OPEN_FILE_ERROR,
	FILE_EOF,

	ERROR_NUMBER_ETI
};

enum ID_LABEL_FIELDS {
	ID_DB_FIELD,
	ID_RAGSOC_1,
	ID_RAGSOC_2,
	ID_RAGSOC_3,
	ID_VT_BRCDE,
	ID_TP_BRCDE,
	ID_COD_PROD,
	ID_TIT_PROD,

	LABEL_FIELDS_NUMBER,
};
