.PHONY: all

CC=gcc
CFLAGS=-std=c99 -Wall -pedantic -g

all: psh

utils: utils.h utils.c
	${CC} ${CFLAGS} -c utils.c

builtins: utils builtins.h builtins.c
	${CC} ${CFLAGS} -c builtins.c

psh: utils builtins psh.c psh.h history.c history.h
	${CC} ${CFLAGS} history.c psh.c -o psh utils.o builtins.o

clean:
	@rm -f *.o
	@rm -f psh
