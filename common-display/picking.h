/*
* picking.h
* Easy Picking General Include file 
* Copyright CNI srl 1999
* Autore : Roberto Mantovani
*/

#ifndef	_PICKING_H_
#define	_PICKING_H_

/* Name of package */
#define PACKAGE "Easy Picking"
/* Base directory  of package */
#define PACKAGE_BASE_DIR "/usr/local/easy-picking/"
/* Version number of package */
#define VERSION "3.0"
/* use postgres for database backend */
#define USE_POSTGRES
/* use mysql for database backend */
/* #define USE_MYSQL */

/* Trace files max size : 10 MB */
#define	PICKING_TRACE_FILE_MAXSIZE	(10*1024*1024)

#define MAX_STR_LEN	512

#ifndef MIN_MAX_DEFINED
#define max(a,b)	((a)>=(b)?(a):(b))
#define min(a,b)	((a)<=(b)?(a):(b))
#endif

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#endif	/* _PICKING_H_ */
