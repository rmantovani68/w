# ---------------------------------------------------------
# CNI Easy Picking 2.0
# Makefile - Analisi Volumetrica
# ---------------------------------------------------------
# Copyright CNI srl 1995-98

PRJ_NAME = volum

# Program debug flags
# debug version
# rm 25-06-2001 SAP : modifiche per SAP
#DEFDEBUG = -g -DDEBUG -DTRACE -DSAP
DEFDEBUG = -g -DDEBUG -DTRACE 

# release version
#DEFDEBUG = -O 

# user defines
USR_DEF = 

# include file contenente le definizioni generiche
include ../common/picking.mak


# object files componenti il progetto
OBJS = $(GEN_INC)/ca_file.o $(GEN_INC)/dbfun.o cmp_time.o

# sorgenti componenti il progetto
SRCS = volum.c volfun.c volutil.c volvar.c 


# libraries
LIBS = $(MSQ_LIB) $(PQ_LIBS) 

# includes 
INCLUDES = -I. -I$(GEN_INC) $(MSQ_INC) $(PQ_CFLAGS)

# Linker flags 
LFLAGS = $(LIBS)

# Default compile rule
.c.o:
	@echo ------ Compiling $<
	@$(CC) $(CFLAGS) $(CDEBUGFLAGS) -c $<

# main target
all: $(EXE_DIR)/$(PRJ_NAME) 

# dependencies
volum.o: volum.c volum.h volext.h volstruct.h volfun.h $(COMMON_DEPS)

volvar.o: volvar.c volum.h volext.h volstruct.h volfun.h $(COMMON_DEPS)

volfun.o: volfun.c volum.h volext.h volstruct.h volfun.h $(COMMON_DEPS)

volutil.o: volutil.c volum.h volext.h volstruct.h volfun.h $(COMMON_DEPS)

# Compile rules
$(EXE_DIR)/$(PRJ_NAME): $(SRCS:.c=.o) $(OBJS)
	@echo '------ Creating compile time object file'
	$(__CMP_TIME__)
	@echo '------ Compiling cmp_time.c'
	@$(CC) $(CFLAGS) $(CDEBUGFLAGS) -c cmp_time.c -o cmp_time.o
	@echo '------ Linking ...'
	@$(CC) $(SRCS:.c=.o) $(OBJS) $(LFLAGS) -o $(EXE_DIR)/$(PRJ_NAME)
	@echo '------ Terminated'

clean:
	@echo '------ Cleaning ...'
	@rm -f $(SRCS:.c=.o) $(EXE_DIR)/$(PRJ_NAME)
	@echo '------ Terminated'
