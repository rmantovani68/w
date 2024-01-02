# -------------------------------------
# Makefile for dbf2sql, txt2sql, dblist
# -------------------------------------
# Copyright CNI 2000

# Program debug flags
# debug version
DEFDEBUG = -DDEBUG

CC = gcc

# release version
DEFDEBUG = -g 

# defines
DEFINES = $(DEFDEBUG) -D_TERMCAP -DSYSV 

INCLUDES = -I /usr/include/pgsql

LIBS = -lpq 


# debug compiler flags
#CFLAGS = $(DEFINES) -Od -g -lc -W3

# release compiler flags
#CFLAGS = $(DEFINES) -Ot -lc -W3

CFLAGS = $(DEFINES) $(INCLUDES) -g -Wall 

# Linker flags (for termcap and own library)
LFLAGS = $(LIBS)

# Default compile rule
.c.o:
		gcc $(CFLAGS) -c $<

all: dbf2sql txt2sql dblist

# dependencies
dbf2sql.o: dbfun.h dbf.h 

txt2sql.o: dbfun.h dbf.h 

dbfun.o: ca_file.h dbfun.h 

ca_file.o: ca_file.h

# Compile rules
dbf2sql: dbf2sql.o ca_file.o dbfun.o
	gcc dbf2sql.o ca_file.o dbfun.o $(LFLAGS) -o dbf2sql

txt2sql: txt2sql.o ca_file.o dbfun.o
	gcc txt2sql.o ca_file.o dbfun.o $(LFLAGS) -o txt2sql

dblist: dblist.o 
	gcc dblist.o $(LFLAGS) -o dblist

install:
	cp dblist txt2sql dbf2sql $(HOME)/bin

clean:
	rm -f *.o
	rm -f dbf2sql txt2sql dblist

backup:
	tar cv *.c
	tar rv *.h
	tar rv makefile
	tar rv *.cfg
