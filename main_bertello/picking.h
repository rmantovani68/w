/*
* picking.h
* Easy Picking General Include file 
* Copyright CNI srl 1999
* Autore : Roberto Mantovani
*/

#ifndef	MAX_FIELDS_NUMBER
#define	MAX_FIELDS_NUMBER	256
#endif
#define LOW_PRIORITY  1000

#ifndef MIN_MAX_DEFINED
#define max(a,b)	((a)>=(b)?(a):(b))
#define min(a,b)	((a)<=(b)?(a):(b))
#endif

#ifndef BOOL_TYPE_DEFINED
typedef unsigned int BOOL;
#define BOOL_TYPE_DEFINED
#endif

typedef enum {
	STAMPA_AGHI,
	STAMPA_LASER
} STAMPA_PL;

typedef enum {
	DOPO_IL_LANCIO,
	INIZIO_PRELIEVO
} STAMPA_RAC;

/*
* Numero massimo di stampanti utilizzate nel sistema
*/
#define MAX_PRINTERS 32
