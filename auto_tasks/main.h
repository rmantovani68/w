/*
* Modulo : receive.h
* -----------------
* Include file principale (costanti , enum , macros ...)
*
* Progetto EMI Italiana SPA - Gestione Ordini
*
* Data creazione 17-05-2001
*
* Autore : Roberto Mantovani
*
* Copyright CNI srl 2001
*/

#ifndef	FALSE
#define	FALSE	(0)
#endif

#ifndef	TRUE
#define	TRUE	(!FALSE)
#endif

#ifndef BOOL_TYPE_DEFINED
typedef unsigned int BOOL;
#define BOOL_TYPE_DEFINED
#endif

#ifndef UINT_TYPE_DEFINED
typedef unsigned int UINT;
#define UINT_TYPE_DEFINED
#endif

#ifndef BYTE_TYPE_DEFINED
typedef unsigned char BYTE;
#define BYTE_TYPE_DEFINED
#endif

#define max(a,b)	((a)>=(b)?(a):(b))
#define min(a,b)	((a)<=(b)?(a):(b))
