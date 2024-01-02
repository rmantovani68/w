# ---------------------------------------------------------
# CNI Easy Picking 3.0
# Makefile - Gestione Socket TX/RX
# ---------------------------------------------------------
# Copyright CNI srl 1995-2000

PRJ_NAME = sock

# Program debug flags
# debug version
DEFDEBUG = -g -DDEBUG -DTRACE

# release version
#DEFDEBUG = -O 

# user defines
USR_DEF = 

# include file contenente le definizioni generiche
include ../common/picking.mak


# object files componenti il progetto
OBJS = $(GEN_INC)/ca_file.o $(GEN_INC)/sockets.o  cmp_time.o

# sorgenti componenti il progetto
SRCS = sock.c 

# libraries
LIBS = $(MSQ_LIB) 

# includes 
INCLUDES = -I$(GEN_INC) $(MSQ_INC)

# Linker flags 
LFLAGS = $(LIBS)

# Default compile rule
.c.o:
	@echo ------ Compiling $<
	@$(CC) $(CFLAGS) $(CDEBUGFLAGS) -c $<


all: $(EXE_DIR)/$(PRJ_NAME)

# dependencies
sock.o: sock.c sock.h $(COMMON_DEPS)

# Compile rules
$(EXE_DIR)/$(PRJ_NAME): $(SRCS:.c=.o) $(OBJS)
	@echo '------ Creating compile time object file'
	$(__CMP_TIME__)
	@echo '------ Compiling cmp_time.c'
	@$(CC) $(CFLAGS) $(CDEBUGFLAGS) -c cmp_time.c -o cmp_time.o
	@echo ------ Linking $<
	@$(CC) $(SRCS:.c=.o) $(OBJS) $(LFLAGS) -o $(EXE_DIR)/$(PRJ_NAME)
	@echo ------ Terminated

clean:
	@echo ------ Cleaning ...
	@rm -f $(SRCS:.c=.o) $(EXE_DIR)/$(PRJ_NAME)
	@echo ------ Terminated
