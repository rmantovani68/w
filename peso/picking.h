/*
* picking.h
* Easy Picking General Include file 
* Copyright CNI srl 1999
* Autore : Roberto Mantovani
*/
#define PLC_SERVICE_PORT 8039

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

#define BARCODE_MSG_PESO      "Il PESO del collo non e' stato letto correttamente. E' quindi necessario provvedere all'inserimento manuale. Per scartare il collo premere  CANCEL, per ritrasmettere i dati premere OK"
#define BARCODE_MSG_BARCODE   "Il CODICE A BARRE del collo non e' stato letto correttamente.E' quindi necessario provvedere all'inserimento manuale. Per scartare il collo premere  CANCEL, per ritrasmettere i dati premere OK"
#define BARCODE_MSG_NOT_FOUND "Il CODICE A BARRE del collo non corrisponde a nessun collo presente in archivio. E' quindi necessario provvedere all'inserimento manuale. Per scartare il collo premere  CANCEL, per ritrasmettere i dati premere OK"


typedef enum {
	TOLLERANZA_ASSOLUTA,
	TOLLERANZA_PERCENTUALE,
	TOLLERANZA_PERCENTUALE_SOGLIA_MASSIMA
} TOLLERANZA;

typedef enum {
	PESATURA_ORDINE,       /* Il fine linea effettua il controllo peso su un intero ordine di produzione */ 
	PESATURA_COLLO         /* Il fine linea effettua il controllo peso su ogni collo */
} GESTIONE_PESATURA;
