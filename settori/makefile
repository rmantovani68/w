# ---------------------------------------------------------
# CNI Easy Picking 3.0
# Makefile - Gestione Settori
# ---------------------------------------------------------
# Copyright CNI srl 1995-2000

PRJ_NAME = settori

# Program debug flags
# debug version
DEFDEBUG = -g -DDEBUG -DTRACE
#DEFDEBUG = -g -DDEBUG -DTRACE -DSAP

# release version
#DEFDEBUG = -O 

# user defines
USR_DEF = 

# include file contenente le definizioni generiche
include ../common/picking.mak


# object files componenti il progetto
OBJS = $(GEN_INC)/ca_file.o $(GEN_INC)/dbfun.o $(GEN_INC)/linklist.o cmp_time.o

# sorgenti componenti il progetto
SRCS = settori.c settfun.c settvar.c settutil.c


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
settori.o: settori.c settori.h settext.h settstruct.h settfun.h $(COMMON_DEPS)

settvar.o: settvar.c settori.h settext.h settstruct.h settfun.h $(COMMON_DEPS)

settfun.o: settfun.c settori.h settext.h settstruct.h settfun.h $(COMMON_DEPS)

settutil.o: settutil.c settori.h settext.h settstruct.h settfun.h $(COMMON_DEPS)

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
