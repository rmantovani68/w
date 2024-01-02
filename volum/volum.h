/*
* volum.h
* 
* Progetto Easy Picking 2.0 : Europharco
*
* Autore : Roberto Mantovani
*
* Copyright CNI srl 1995-98
*/
#ifndef BOOL_TYPE_DEFINED
typedef unsigned int BOOL;
#define FALSE	0
#define TRUE 	1
#define BOOL_TYPE_DEFINED
#endif

#ifndef MIN_MAX_DEFINED
#define max(a,b)	((a)>=(b)?(a):(b))
#define min(a,b)	((a)<=(b)?(a):(b))
#endif

/* 
* gestisco al massimo 10000 ubicazioni 
*/
#define MAX_UBICAZIONI    10000

/*
* gestisco 16 imballi diversi
*/
#define MAX_IMBALLI       16

enum {
	VOL_PRELIEVO_OK,
	VOL_CHIUDI_COLLO,
	VOL_RIGA_SPEZZATA,
	VOL_DIM_PROD_ERRATE,
	VOL_DIM_PROD_ERRATE_ALTEZZA,
	VOL_COLLO_FF_NON_PRESENTE,
};

