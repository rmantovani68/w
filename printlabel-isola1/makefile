# ---------------------------------------------------------
# CNI Easy Picking 2.0
# Makefile - Gestione Stampe
# ---------------------------------------------------------
# Copyright CNI srl 1995-98

PRJ_NAME = printlabel

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
SRCS = printlabel.c printfun.c printvar.c printutil.c


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
printlabel.o: printlabel.c printlabel.h printext.h printstruct.h printfun.h $(COMMON_DEPS)

printvar.o: printvar.c printlabel.h printext.h printstruct.h printfun.h $(COMMON_DEPS)

printfun.o: printfun.c printlabel.h printext.h printstruct.h printfun.h $(COMMON_DEPS)

printutil.o: printutil.c printlabel.h printext.h printstruct.h printfun.h $(COMMON_DEPS)

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
