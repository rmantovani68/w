# ---------------------------------------------------------
# CNI Easy Picking 2.0
# Makefile - Gestione Bilancia
# ---------------------------------------------------------
# Copyright CNI srl 1995-2000

PRJ_NAME = bilancia

SIM_NAME = bilsim

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
OBJS = $(GEN_INC)/ca_file.o cmp_time.o

# sorgenti componenti il progetto
SRCS = bilancia.c 

SIM_SRCS = bilsim.c 


# libraries
LIBS = $(MSQ_LIB) 

# includes 
INCLUDES = -I$(GEN_INC) $(MSQ_INC)

# Linker flags 
LFLAGS = $(LIBS)

# Default compile rule
.c.o:
	@echo ------ Compiling $<
	$(CC) $(CFLAGS) $(CDEBUGFLAGS) -c $<


all: $(EXE_DIR)/$(PRJ_NAME) $(EXE_DIR)/$(SIM_NAME)

# dependencies
bilancia.o: bilancia.c bilancia.h $(COMMON_DEPS)

# Compile rules
$(EXE_DIR)/$(PRJ_NAME): $(SRCS:.c=.o) $(OBJS)
	@echo ------ Linking $<
	$(CC) $(SRCS:.c=.o) $(OBJS) $(LFLAGS) -o $(EXE_DIR)/$(PRJ_NAME)
	@echo ------ Terminated

$(EXE_DIR)/$(SIM_NAME): $(SIM_SRCS:.c=.o) $(OBJS)
	@echo '------ Creating compile time object file'
	$(__CMP_TIME__)
	@echo '------ Compiling cmp_time.c'
	@$(CC) $(CFLAGS) $(CDEBUGFLAGS) -c cmp_time.c -o cmp_time.o
	@echo ------ Linking $<
	$(CC) $(SIM_SRCS:.c=.o) $(OBJS) $(LFLAGS) -o $(EXE_DIR)/$(SIM_NAME)
	@echo ------ Terminated

# dependencies
bilsim.o: bilsim.c bilancia.h $(COMMON_DEPS)
txser.o: txser.c $(COMMON_DEPS)
rxser.o: rxser.c $(COMMON_DEPS)
test_bilancia.o: test_bilancia.c $(COMMON_DEPS)

# Compile rules
txser: txser.o
	@echo ------ Linking $<
	@$(CC) txser.o $(LFLAGS) -o txser
	@echo ------ Terminated

# Compile rules
rxser: rxser.o
	@echo ------ Linking $<
	@$(CC) rxser.o $(LFLAGS) -o rxser
	@echo ------ Terminated

# Compile rules
test_bilancia: test_bilancia.o
	@echo ------ Linking $<
	@$(CC) test_bilancia.o $(LFLAGS) -o test_bilancia
	@echo ------ Terminated


clean:
	@echo ------ Cleaning ...
	@rm -f $(SRCS:.c=.o) $(EXE_DIR)/$(PRJ_NAME) $(SIM_SRCS:.c=.o) $(EXE_DIR)/$(SIM_NAME)
	@echo ------ Terminated
