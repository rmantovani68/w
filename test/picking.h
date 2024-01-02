/*
* picking.h
* Easy Picking General Include file 
* Copyright CNI srl 1999
* Autore : Roberto Mantovani
*/

#ifndef	_PICKING_H_
#define	_PICKING_H_

/* 
* gestisco al massimo 1024 righe d'ordine 
*/
#define	MAX_RIGHE_ORDINE	1024

/* 
* gestisco al massimo 10000 ubicazioni 
*/
#define MAX_UBICAZIONI    10000

/*
* gestisco 128 imballi diversi
*/
#define MAX_IMBALLI       128

#define LOW_PRIORITY  1000

#ifndef MIN_MAX_DEFINED
#define max(a,b)	((a)>=(b)?(a):(b))
#define min(a,b)	((a)<=(b)?(a):(b))
#endif

#ifndef BOOL_TYPE_DEFINED
typedef unsigned int BOOL;
#define BOOL_TYPE_DEFINED
#endif

#ifndef BYTE_TYPE_DEFINED
typedef unsigned char BYTE;
#define BYTE_TYPE_DEFINED
#endif

enum StatoRiga {
	ATTESA_PRELIEVO,
	PRELEVATA,
};
enum ModiDisplay {
	NORMAL,
	BLINK,
};

enum RigaDisplay {
	TUTTO,
	RIGA_1,
	RIGA_2,
};

enum StatiSettore {
	ATTESA,
	PRELIEVO_IMBALLI,
	PRELIEVO_COPIE,
	PASSA_SETT_SUCC,
	CHIUSURA_COLLO,
	FINE_ORDINE,
	DISABILITATO,

	NUM_STATI_SETTORE,
};

enum TipiSettore {
	SETTORE_START,
	SETTORE_INTERMEDIO,
	SETTORE_END,
};

enum StatiOrdProd {
	EVASO,
	DA_EVADERE,
	IN_EVASIONE,
};

enum TipiOrdine {
	ORDINE_AUTOMATICO,
	ORDINE_MANUALE,

	NUM_TIPI_ORDINE,
};


#define MAX_UBICAZIONI    10000
#define MAX_CEDOLE        32

#define MAX_ISOLE       32
#define MAX_SETTORI    100
#define MAX_DISPLAYS   100

#define MAX_PKL	         4	/* numero max di moduli PKL presenti */
#define MAX_RIGHE_PKL		64
#define MAX_COLONNE_PKL	64

/*
* Numero massimo di stampanti utilizzate nel sistema
*/
#define MAX_PRINTERS 32

/*
* codici vettori utilizzati da DeA
*/
#define VETTORE_DP         8
#define VETTORE_BARTOLNI  38
#define VETTORE_TNT       53
#define VETTORE_ASCOLI     1
#define VETTORE_MINERVA    9

/*
* stati dei records archivi operativi 
* NB: Gli stati degli ordini sono consequenziali
*/
#define ORDINE_RICEVUTO               ' '
#define ORDINE_SPEDITO                'S'
#define ORDINE_SPEDITO_DA_EVADERE     'K'
#define ORDINE_IN_ANALISI             'I'
#define ORDINE_ELABORATO              'A'
#define ORDINE_STAMPATA_RAC           'R'
#define ORDINE_IN_PRELIEVO            'P'
#define ORDINE_PRELEVATO              'T'
#define ORDINE_EVASO                  'E'
#define ORDINE_STAMPATA_XAB           'X'
#define ORDINE_STAMPATA_DIST          'D'
#define ORDINE_SPEDITO_HOST           'H'


#define COLLO_ELABORATO               'A'
#define COLLO_PRELIEVO                'P'
#define COLLO_CHIUSO                  'C'
#define COLLO_EVASO                   'E'
#define COLLO_SPEDITO_HOST            'H'
#define COLLO_ELIMINATO               'D'
#define COLLO_SCARTATO                'S'
#define COLLO_FUORI_LINEA             'F'
#define COLLO_INIZIATO                'I'   /* per i resi */

#define CONTENITORE_ATTIVO            'A'
#define CONTENITORE_SCARICATO         'S'
#define CONTENITORE_UBICATO           'U'
#define CONTENITORE_BANCALATO         'B'

#define BANCALE_CREATO                'C'
#define BANCALE_SPEDITO               'S'

#define CARICO_CONFERMATO             'C'
#define CARICO_SPEDITO                'S'

#define RIGA_ELABORATA                'A'
#define RIGA_PRELEVATA                'P'
#define RIGA_EVASA                    'E'
#define RIGA_SPEDITA_HOST             'H'
#define RIGA_SCARTATA                 'S'

#define RIFORNIMENTO_RICHIESTO        'S'
#define RIFORNIMENTO_EFFETTUATO       'E'
#define RIFORNIMENTO_CANCELLATO       'D'


/* ordine predestinato allo scarto in zona bilancia */
#define ORDINE_DA_SCARTARE            'X'

/* campo ubcdflg */
#define UBICAZIONE_AUTOMATICA         'A'
#define UBICAZIONE_MANUALE            'M'

/* campo ubitipo */
#define UBICAZIONE_CATALOGO           'C'
#define UBICAZIONE_LANCIO             'N'
#define UBICAZIONE_RESO               'R'

/*
* Flags dei records archivi operativi 
*/
#define ORDINE_BLOCCATO               'B'
#define ORDINE_FORZATO                'F'

#define COLLO_RETTIFICATO             'R'

#define RIGA_BLOCCATA                 'B'
#define RIGA_TAGLIATA                 'T'
#define RIGA_RETTIFICATA              'R'
#define RIGA_IMPEGNATA                'I'
#define RIGA_NON_UBICATA              'U'

#define LOTTO_RICEVUTO                'R'
#define LOTTO_IN_LAVORAZIONE          'I'
#define LOTTO_CHIUSO                  'C'
#define LOTTO_SPEDITO                 'S'

/*
* Tipi di collo particolari (cptpfor)
*/
#define COLLO_FUORI_FORMATO   'F'
#define COLLO_PALLET          'P'
#define COLLO_PRECONFEZIONATO 'C'

#define COLLO_AUTOMATICO       0
#define COLLO_MANUALE          1

/*
* Tipi di ordine (ORDTIPO :catalogo/lancio novita')
*/
#define ORDINE_CATALOGO               'C'
#define ORDINE_LANCIO                 'N'
#define ORDINE_INTERNO                'D'


#endif	/* _PICKING_H_ */
