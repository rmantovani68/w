# ---------------------------------------------------------
# CNI Easy Picking 2.0
# Makefile - Gestione Stampe
# ---------------------------------------------------------
# Copyright CNI srl 1995-98

PRJ_NAME = autotask

# Program debug flags
# debug version
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
SRCS = main.c mainfun.c mainvar.c mainutil.c


# libraries
LIBS = $(MSQ_LIB) $(PSQL_LIB)

# includes 
INCLUDES = -I. -I$(GEN_INC) $(MSQ_INC) $(PSQL_INC)

# Linker flags 
LFLAGS = $(LIBS)

# Default compile rule
.c.o:
	@echo ------ Compiling $<
	@$(CC) $(CFLAGS) $(CDEBUGFLAGS) -c $<

# main target
all: $(EXE_DIR)/$(PRJ_NAME) 

# dependencies
main.o: main.c main.h $(COMMON_DEPS)

mainvar.o: mainvar.c main.h mainext.h mainstruct.h mainfun.h $(COMMON_DEPS)

mainfun.o: mainfun.c main.h mainext.h mainstruct.h mainfun.h $(COMMON_DEPS)

mainutil.o: mainutil.c main.h mainext.h mainstruct.h mainfun.h $(COMMON_DEPS)

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
