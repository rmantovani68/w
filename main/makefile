# ---------------------------------------------------------
# Makefile for Easy Picking - Main Process
# ---------------------------------------------------------
# Copyright CNI srl 1995-2000

# --------------------------
# Project Name (executable)
# --------------------------
PRJ_NAME = main

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
USR_DEF =  -DHAVE_CONFIG_H -DTRACE 

# include file contenente le definizioni generiche
include ../common/picking.mak


# object files componenti il progetto
OBJS = $(GEN_INC)/dbfun.o $(GEN_INC)/ca_file.o cmp_time.o

# sorgenti componenti il progetto
SRCS = main.c mainfun.c mainproc.c mainutil.c mainvar.c callbacks.c interface.c support.c

# libraries
LIBS = $(GTK_LIB) $(MSQ_LIB) $(PQ_LIBS)

# includes 
INCLUDES = -I. -I$(GEN_INC) $(GTK_CFLAGS) $(MSQ_INC) $(PQ_CFLAGS)

# Linker flags 
LFLAGS = $(LIBS)

# Default compile rule
.c.o:
	@echo '------ Compiling $<'
	@$(CC) $(CFLAGS) $(CDEBUGFLAGS) -c $<

# main target
all: $(EXE_DIR)/$(PRJ_NAME) 

# dependencies
cmp_time.h:  $(SRCS:.c=.o)
	$(__CMP_TIME__H__)

cmp_time.c: $(SRCS:.c=.o)
	$(__CMP_TIME__C__)

cmp_time.o: cmp_time.c

main.o: main.c $(COMMON_DEPS) picking.h mainfun.h mainext.h mainstruct.h

mainfun.o: mainfun.c $(COMMON_DEPS) picking.h mainfun.h mainext.h mainstruct.h

mainproc.o: mainproc.c $(COMMON_DEPS) picking.h mainfun.h mainext.h mainstruct.h

mainutil.o: mainutil.c $(COMMON_DEPS) picking.h mainfun.h mainext.h mainstruct.h

mainvar.o: mainvar.c $(COMMON_DEPS) picking.h mainfun.h mainext.h mainstruct.h

callbacks.o: callbacks.c callbacks.h mainstruct.h

interface.o: interface.c interface.h 

support.o: support.c support.h 


# build rules
$(EXE_DIR)/$(PRJ_NAME): $(SRCS:.c=.o) $(OBJS)
	@echo '------ Creating compile time object file'
	$(__CMP_TIME__)
	@echo '------ Compiling cmp_time.c'
	@$(CC) $(CFLAGS) $(CDEBUGFLAGS) -c cmp_time.c -o cmp_time.o
	@echo '------ Linking ...'
	@$(CC) $(SRCS:.c=.o) $(OBJS) $(LFLAGS) -o $(EXE_DIR)/$(PRJ_NAME)
	@echo '------ Terminated'

clean:
	@echo '------ Clean ...'
	@rm -f $(SRCS:.c=.o) $(EXE_DIR)/$(PRJ_NAME)
	@echo '------ Terminated'
