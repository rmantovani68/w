# ---------------------------------------------------------
# Makefile for Easy Picking 3.0
# Gestione Pesatura Colli / Fine Linea
# ---------------------------------------------------------
# Copyright CNI srl 1995-2000

# --------------------------
# Project Name (executable)
# --------------------------
PRJ_NAME = peso

# --------------------------
# Program debug flags
# --------------------------

# debug version
DEFDEBUG = -g -DDEBUG

# release version
#DEFDEBUG = -O 

# --------------------------
# user defines
# --------------------------

# User Definitions
#USR_DEF =  -DHAVE_CONFIG_H -DTRACE -DTRACE_PLC
USR_DEF =  -DHAVE_CONFIG_H -DTRACE
#USR_DEF =  -DHAVE_CONFIG_H -DTRACE -DTRACE_DEBUG_PESO

# include file contenente le definizioni generiche
include ../common/picking.mak


# object files componenti il progetto
OBJS = $(GEN_INC)/dbfun.o  $(GEN_INC)/ca_file.o $(GEN_INC)/sockets.o cmp_time.o

# sorgenti componenti il progetto
SRCS = peso.c pesofun.c pesoproc.c pesoutil.c pesovar.c plcsock.c callbacks.c interface.c support.c

# libraries
LIBS = $(GTK_LIB) $(MSQ_LIB) $(PSQL_LIB)

# includes 
INCLUDES = -I. -I$(GEN_INC) $(GTK_CFLAGS) $(MSQ_INC) $(PSQL_INC)

# Linker flags 
LFLAGS = $(LIBS)

# Default compile rule
.c.o:
	@echo '------ Compiling $<'
	@$(CC) $(CFLAGS) $(CDEBUGFLAGS) -c $<



# peso target
all: $(EXE_DIR)/$(PRJ_NAME) 

# dependencies
peso.o: peso.c $(COMMON_DEPS) picking.h pesofun.h pesoext.h pesostruct.h plcsock.h

pesofun.o: pesofun.c $(COMMON_DEPS) picking.h pesofun.h pesoext.h pesostruct.h plcsock.h

pesoproc.o: pesoproc.c $(COMMON_DEPS) picking.h pesofun.h pesoext.h pesostruct.h plcsock.h

pesoutil.o: pesoutil.c $(COMMON_DEPS) picking.h pesofun.h pesoext.h pesostruct.h plcsock.h

pesovar.o: pesovar.c $(COMMON_DEPS) picking.h pesofun.h pesoext.h pesostruct.h plcsock.h

plcsock.o: plcsock.c plcsock.h

callbacks.o: callbacks.c callbacks.h 

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
