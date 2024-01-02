# ---------------------------------------------------------
# picking.mak : include file con definizioni generali da
# includere in tutti i Makefile per Easy Picking 3.0
# ---------------------------------------------------------
# Versione per LynxOS 3.0 / Linux
# ---------------------------------------------------------
# Copyright CNI srl 1995-2000

#GTK_LIB = `gtk-config --libs`
#GTK_CFLAGS = `gtk-config --cflags`

GNOME_LIB    = `pkg-config libgnome-2.0 --libs` `pkg-config libgnomeui-2.0 --libs` 
GNOME_CFLAGS = `pkg-config libgnome-2.0 --cflags` `pkg-config libgnomeui-2.0 --cflags` 

GTK_LIB    = `pkg-config gtk+-2.0 --libs` 
GTK_CFLAGS = `pkg-config gtk+-2.0 --cflags` 

GLIB_LIB    = `pkg-config glib-2.0 --libs` 
GLIB_CFLAGS = `pkg-config glib-2.0 --cflags` 

# libreria di gestione Code Messaggi, Comunicazioni Seriali ...
MSQ_LIB = -L/u/lib/pmx -lPmxMsq
MSQ_INC = -I/u/lib/pmx/pmx_msq

# libreria di gestione Database PostgreSql
PSQL_LIB = -lpq
PSQL_INC = -I /usr/include/pgsql

CNI_INCLUDES = -I /usr/local/cni/include
DEVCNI_LIB = -L /usr/local/cni/lib -ldevcni

# Direttorio contenete file generali da includere
GEN_INC = ../common_peso

# Direttorio degli eseguibili
EXE_DIR=../exe

# Include files di uso comune a tutti i processi
COMMON_DEPS=$(GEN_INC)/dbfun.h $(GEN_INC)/proc_list.h

# Macro per generazione di ID RCS
# __CMP_TIME__=@echo -en 'char rcsid[]="$$Id: '$(PRJ_NAME)' '`date +"%d/%m/%Y %H:%M:%S"` '$$";\n'> cmp_time.c
__CMP_TIME__=@echo -en '\
char rcsid[]="$$Id: '$(PRJ_NAME)' '`date +"%d/%m/%Y %H:%M:%S"` '$$";\n \
char __version__[]="'`cat ../doc/VERSION`'";\n \
char __customer__[]="'`cat ../doc/CUSTOMER`'";\n \
char __authors__[]="'`cat ../doc/AUTHORS`'";\n \
char __copyright__[]="'`cat ../doc/COPYRIGHT`'";\n'> cmp_time.c

CC=gcc
DEFINES= $(CB_DEF) $(USR_DEF)
CFLAGS= $(DEFDEBUG) -Wall $(DEFINES) $(INCLUDES) $(CNI_INCLUDES)

#
# objs comuni a tutte le applicazioni
#

# funzioni per la gestione di linked list
$(GEN_INC)/linklist.o: $(GEN_INC)/linklist.c $(GEN_INC)/linklist.h
	@echo '------ Compiling $<'
	@$(CC) $(CFLAGS) $(CDEBUGFLAGS) -c $(GEN_INC)/linklist.c -o $(GEN_INC)/linklist.o

# funzioni per l'accesso ai database
$(GEN_INC)/dbfun.o: $(GEN_INC)/dbfun.c $(GEN_INC)/dbfun.h
	@echo '------ Compiling $<'
	@$(CC) $(CFLAGS) $(CDEBUGFLAGS) -c $(GEN_INC)/dbfun.c -o $(GEN_INC)/dbfun.o

# lettura e scrittura file di configurazione
$(GEN_INC)/ca_file.o: $(GEN_INC)/ca_file.c $(GEN_INC)/ca_file.h
	@echo '------ Compiling $<'
	@$(CC) $(CFLAGS) $(CDEBUGFLAGS) -c $(GEN_INC)/ca_file.c -o $(GEN_INC)/ca_file.o

# Gestione sockets communication
$(GEN_INC)/sockets.o: $(GEN_INC)/sockets.c $(GEN_INC)/sockets.h
	@echo '------ Compiling $<'
	@$(CC) $(CFLAGS) $(CDEBUGFLAGS) -c $(GEN_INC)/sockets.c -o $(GEN_INC)/sockets.o


