# ---------------------------------------------------------
# CNI Easy Picking 2.0
# Makefile - Gestione IOS
# ---------------------------------------------------------
# Copyright CNI srl 1995-2000

PRJ_NAME = ios

# Program debug flags
# debug version
DEFDEBUG = -g -DDEBUG -DTRACE -DTRACE_TIME
#DEFDEBUG = -g -DDEBUG -DTRACE 

# release version
#DEFDEBUG = -O 

# user defines
#USR_DEF = -DHAS_IOS_CNI
USR_DEF = 

# include file contenente le definizioni generiche
include ../common-new/picking.mak


# object files componenti il progetto
OBJS = $(GEN_INC)/ep-common.o cmp_time.o

# sorgenti componenti il progetto
SRCS = ios.c 


# libraries
#LIBS = $(MSQ_LIB) $(DEVCNI_LIB) $(GLIB_LIB)
LIBS = $(MSQ_LIB) $(GLIB_LIB)
#LIBS = $(MSQ_LIB) -lm

# includes 
#INCLUDES = -I$(GEN_INC) $(MSQ_INC) $(DEVCNI_INC)
INCLUDES = -I$(GEN_INC) $(MSQ_INC)

# Linker flags 
LFLAGS = $(LIBS)

# Default compile rule
.c.o:
	@echo ------ Compiling $<
	$(CC) $(CFLAGS) $(CDEBUGFLAGS) -c $<


#all: $(EXE_DIR)/$(PRJ_NAME) $(EXE_DIR)/iosver $(EXE_DIR)/iosdump $(EXE_DIR)/setlampada 
all: $(EXE_DIR)/$(PRJ_NAME)

# dependencies
ios.o: ios.c ios.h $(COMMON_DEPS)
iosdump.o: iosdump.c
iosset.o: iosset.c
iossetbit.o: iossetbit.c
iosver.o: iosver.c 
setlampada.o: setlampada.c 

# Compile rules
$(EXE_DIR)/$(PRJ_NAME): $(SRCS:.c=.o) $(OBJS)
	@echo '------ Creating compile time object file'
	$(__CMP_TIME__)
	@echo '------ Compiling cmp_time.c'
	@$(CC) $(CFLAGS) $(CDEBUGFLAGS) -c cmp_time.c -o cmp_time.o
	@echo '------ Linking ...'
	@$(CC) $(SRCS:.c=.o) $(OBJS) $(LFLAGS) -o $(EXE_DIR)/$(PRJ_NAME)
	@echo '------ Terminated'

$(EXE_DIR)/iosver: iosver.o
	@echo '------ Creating compile time object file'
	$(__CMP_TIME__)
	@echo '------ Compiling cmp_time.c'
	@$(CC) $(CFLAGS) $(CDEBUGFLAGS) -c cmp_time.c -o cmp_time.o
	@echo '------ Linking ...'
	@$(CC) iosver.o $(LFLAGS) -o $(EXE_DIR)/iosver
	@echo '------ Terminated'

$(EXE_DIR)/iosdump: iosdump.o
	@echo '------ Creating compile time object file'
	$(__CMP_TIME__)
	@echo '------ Compiling cmp_time.c'
	@$(CC) $(CFLAGS) $(CDEBUGFLAGS) -c cmp_time.c -o cmp_time.o
	@echo '------ Linking ...'
	@$(CC) iosdump.o $(LFLAGS) -o $(EXE_DIR)/iosdump
	@echo '------ Terminated'

$(EXE_DIR)/iosset: iosset.o
	@echo '------ Creating compile time object file'
	$(__CMP_TIME__)
	@echo '------ Compiling cmp_time.c'
	@$(CC) $(CFLAGS) $(CDEBUGFLAGS) -c cmp_time.c -o cmp_time.o
	@echo '------ Linking ...'
	@$(CC) iosset.o $(OBJS) $(LFLAGS) -o $(EXE_DIR)/iosset
	@echo '------ Terminated'

$(EXE_DIR)/iossetbit: iossetbit.o
	@echo '------ Creating compile time object file'
	$(__CMP_TIME__)
	@echo '------ Compiling cmp_time.c'
	@$(CC) $(CFLAGS) $(CDEBUGFLAGS) -c cmp_time.c -o cmp_time.o
	@echo '------ Linking ...'
	@$(CC) iossetbit.o $(OBJS) $(LFLAGS) -o $(EXE_DIR)/iossetbit
	@echo '------ Terminated'

$(EXE_DIR)/setlampada: setlampada.o
	@echo '------ Creating compile time object file'
	$(__CMP_TIME__)
	@echo '------ Compiling cmp_time.c'
	@$(CC) $(CFLAGS) $(CDEBUGFLAGS) -c cmp_time.c -o cmp_time.o
	@echo '------ Linking ...'
	@$(CC) setlampada.o $(LFLAGS) -o $(EXE_DIR)/setlampada
	@echo '------ Terminated'

clean:
	@echo '------ Cleaning ...'
	@rm -f $(SRCS:.c=.o) $(EXE_DIR)/$(PRJ_NAME)
	@echo '------ Terminated'
