# ---------------------------------------------------------
# Makefile for Easy Picking - Equilibratura
# ---------------------------------------------------------
# Copyright CNI srl 1995-2002

# --------------------------
# Project Name (executable)
# --------------------------
PRJ_NAME = display

# --------------------------
# Program debug flags
# --------------------------

# debug version
DEFDEBUG = -g -DDEBUG -DTRACE 

# release version
#DEFDEBUG = -O 

# --------------------------
# user defines
# --------------------------

# User Definitions
#USR_DEF =  -DTRACE -DG_DISABLE_DEPRECATED -DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED -DGDK_PIXBUF_DISABLE_DEPRECATED -DGNOME_DISABLE_DEPRECATED
USR_DEF =  -DTRACE 

# include file contenente le definizioni generiche
#include ../common-display/picking.mak
include ../common/picking.mak


# object files componenti il progetto
OBJS = $(GEN_INC)/dbfun-gtk.o $(GEN_INC)/ep-common.o $(GEN_INC)/ep-db.o cmp_time.o $(GEN_INC)/gtk-support.o

# sorgenti componenti il progetto
SRCS = main.c mainproc.c mainfun.c mainvar.c callbacks.c interface.c 

# libraries
LIBS = $(GNOME_LIB) $(GTK_LIB) $(MSQ_LIB) $(PQ_LIBS)

# includes 
INCLUDES = -I. -I$(GEN_INC) $(GNOME_CFLAGS)  $(GTK_CFLAGS) $(MSQ_INC) $(PQ_CFLAGS)

# Linker flags 
LFLAGS = $(LIBS)

# Default compile rule
.c.o:
	@echo '------ Compiling $<'
	@$(CC) $(CFLAGS) $(CDEBUGFLAGS) -c $<

# main target
all: $(EXE_DIR)/$(PRJ_NAME) 

# dependencies
main.o: main.h main.c callbacks.h $(COMMON_DEPS) mainfun.h mainext.h mainstruct.h

mainfun.o: main.h mainfun.c $(COMMON_DEPS) mainfun.h mainext.h mainstruct.h

mainvar.o: main.h mainvar.c $(COMMON_DEPS) mainfun.h mainext.h mainstruct.h

callbacks.o: main.h callbacks.c callbacks.h $(COMMON_DEPS) mainfun.h mainext.h mainstruct.h

interface.o: main.h interface.c interface.h 

support.o: support.c support.h 


# build rules
cmp_time.h: $(SRCS:.c=.o)
	@echo '------ Creating compile time include file'
	$(__CMP_TIME__H__)

cmp_time.c: cmp_time.h
	@echo '------ Creating compile time source file'
	$(__CMP_TIME__C__)

cmp_time.o: cmp_time.c
	@echo '------ Compiling cmp_time.c'
	@$(CC) $(CFLAGS) $(CDEBUGFLAGS) -c cmp_time.c -o cmp_time.o

# Compile rules
$(EXE_DIR)/$(PRJ_NAME): $(SRCS:.c=.o) $(OBJS)
	@echo '------ Linking ...'
	@$(CC) $(SRCS:.c=.o) $(OBJS) $(LFLAGS) -o $(EXE_DIR)/$(PRJ_NAME)
	@echo '------ Terminated'



clean:
	@echo '------ Clean ...'
	@rm -f $(SRCS:.c=.o) $(EXE_DIR)/$(PRJ_NAME)
	@echo '------ Terminated'
