/*
* picking.h
* Easy Picking General Include file 
* Copyright CNI srl 1999
* Autore : Roberto Mantovani
*/

#ifndef	MAX_FIELDS_NUMBER
#define	MAX_FIELDS_NUMBER	256
#endif

#ifndef MIN_MAX_DEFINED
#define max(a,b)	((a)>=(b)?(a):(b))
#define min(a,b)	((a)<=(b)?(a):(b))
#endif

#ifndef BOOL_TYPE_DEFINED
typedef unsigned int BOOL;
#define BOOL_TYPE_DEFINED
#endif

typedef enum {
	DOPO_IL_LANCIO,
	INIZIO_PRELIEVO
} STAMPA_RAC;
