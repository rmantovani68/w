# ---------------------------------------------------------
# picking.mak : include file con definizioni generali da
# includere in tutti i Makefile per Easy Picking 3.x
# ---------------------------------------------------------
# Copyright SMD srl 2002
                                                             
#GNOME_LIB    = `pkg-config libgnome-2.0 --cflags --libs` `pkg-config libgnomeui-2.0 --cflags --libs` 
#GNOME_CFLAGS = `pkg-config libgnome-2.0 --cflags --libs` `pkg-config libgnomeui-2.0 --cflags --libs` 

#GTK_LIB    = `pkg-config gtk+-2.0 --cflags --libs` 
#GTK_CFLAGS = `pkg-config gtk+-2.0 --cflags --libs` 

#GLIB_LIB    = `pkg-config glib-2.0 --cflags --libs` 
#GLIB_CFLAGS = `pkg-config glib-2.0 --cflags --libs` 

GNOME_LIB = `gnome-config --libs gnome gnomeui`
GNOME_CFLAGS = `gnome-config --cflags gnome`


GTK_LIB = `gtk-config --libs`
GTK_CFLAGS = `gtk-config --cflags`

GLIB_LIB = `glib-config --libs`
GLIB_CFLAGS = `glib-config --cflags`

GTK_EXTRA_LIBS = -L/usr/local/lib -lgtkextra
GTK_EXTRA_CFLAGS = -I/usr/local/include/gtkextra

# libreria di gestione Code Messaggi, Comunicazioni Seriali ...
MSQ_LIB = -L/u/lib/pmx -lPmxMsq
MSQ_INC = -I/u/lib/pmx/pmx_msq

# libreria di gestione Database PostgreSql
PSQL_LIB = -lpq
PSQL_INC = -I /usr/include/pgsql

CNI_INCLUDES = -I /usr/local/cni/include
DEVCNI_LIB = -L /usr/local/cni/lib -ldevcni

# Direttorio contenete file generali da includere
GEN_INC = ../common-new

# Direttorio degli eseguibili
EXE_DIR=../exe

# Include files di uso comune a tutti i processi
COMMON_DEPS=$(GEN_INC)/dbfun-gtk.h $(GEN_INC)/proc_list.h $(GEN_INC)/picking.h $(GEN_INC)/sockets.h $(GEN_INC)/ep-common.h $(GEN_INC)/ep-db.h


#
# Macro per generazione di ID RCS
# e altre variabili per la gestione delle versioni
#
VERSION=`cat VERSION`

__CMP_TIME__=@echo -en '\
char rcsid[]                  = "$$Id: '$(PRJ_NAME)' v.' $(VERSION)' - '`date +"%d/%m/%Y %H:%M:%S"` '$$";\n \
char __version__[]            = "'`cat ../doc/BASE_VERSION`'";\n \
char __module_name__[]        = "'$(PRJ_NAME)'";\n \
char __module_version__[]     = "'`cat VERSION`'";\n \
char __customer__[]           = "'`cat ../doc/CUSTOMER`'";\n \
char __authors__[]            = "'`cat ../doc/AUTHORS`'";\n \
char __copyright__[]          = "'`cat ../doc/COPYRIGHT`'";\n \
char __configuration_file__[] ="'`cat ../doc/CONFIGURATION_FILE`'";\n'> cmp_time.c


CC=gcc
DEFINES= $(CB_DEF) $(USR_DEF)
#CFLAGS= $(DEFDEBUG) -Wall -fwritable-strings $(DEFINES) $(INCLUDES) $(CNI_INCLUDES) $(GLIB_CFLAGS)
CFLAGS= $(DEFDEBUG) -Wall $(DEFINES) $(INCLUDES) $(CNI_INCLUDES) $(GLIB_CFLAGS)

#
# objs comuni a tutte le applicazioni
#

# funzioni per la gestione di linked list
$(GEN_INC)/linklist.o: $(GEN_INC)/linklist.c $(GEN_INC)/linklist.h
	@echo '---- Compiling $<'
	@$(CC) $(CFLAGS) $(CDEBUGFLAGS) -c $(GEN_INC)/linklist.c -o $(GEN_INC)/linklist.o

# funzioni per l'accesso ai database con GTK
$(GEN_INC)/dbfun-gtk.o: $(GEN_INC)/dbfun-gtk.c $(GEN_INC)/dbfun-gtk.h
	@echo '---- Compiling $<'
	@$(CC) $(CFLAGS) $(CDEBUGFLAGS) -c $(GEN_INC)/dbfun-gtk.c -o $(GEN_INC)/dbfun-gtk.o

# Gestione sockets communication
$(GEN_INC)/sockets.o: $(GEN_INC)/sockets.c $(GEN_INC)/sockets.h
	@echo '---- Compiling $<'
	@$(CC) $(CFLAGS) $(CDEBUGFLAGS) -c $(GEN_INC)/sockets.c -o $(GEN_INC)/sockets.o

# Gestione PLC socket communication
$(GEN_INC)/plcsock.o: $(GEN_INC)/plcsock.c $(GEN_INC)/plcsock.h
	@echo '---- Compiling $<'
	@$(CC) $(CFLAGS) $(CDEBUGFLAGS) -c $(GEN_INC)/plcsock.c -o $(GEN_INC)/plcsock.o

# support for gtk/glade
$(GEN_INC)/gtk-support.o: $(GEN_INC)/gtk-support.c $(GEN_INC)/gtk-support.h
	@echo '---- Compiling $<'
	@$(CC) $(CFLAGS) $(CDEBUGFLAGS) -c $(GEN_INC)/gtk-support.c -o $(GEN_INC)/gtk-support.o

# EasyPicking utilities 
$(GEN_INC)/ep-common.o: $(GEN_INC)/ep-common.c $(GEN_INC)/ep-common.h
	@echo '---- Compiling $<'
	@$(CC) $(CFLAGS) $(CDEBUGFLAGS) -c $(GEN_INC)/ep-common.c -o $(GEN_INC)/ep-common.o

# EasyPicking utilities DataBase
$(GEN_INC)/ep-db.o: $(GEN_INC)/ep-db.c $(GEN_INC)/ep-db.h
	@echo '---- Compiling $<'
	@$(CC) $(CFLAGS) $(CDEBUGFLAGS) -c $(GEN_INC)/ep-db.c -o $(GEN_INC)/ep-db.o

# gm enhanced clist
$(GEN_INC)/gm-clist.o: $(GEN_INC)/gm-clist.c $(GEN_INC)/gm-clist.h
	@echo '---- Compiling $<'
	@$(CC) $(CFLAGS) $(CDEBUGFLAGS) -c $(GEN_INC)/gm-clist.c -o $(GEN_INC)/gm-clist.o
