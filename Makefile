.PHONY: all

CC=gcc
CFLAGS=-std=c99 -Wall -pedantic -g

all: psh

utils: include/utils.h src/utils.c
	${CC} ${CFLAGS} -c src/utils.c
	@mkdir -p build
	@mv utils.o build/

builtins: utils include/builtins.h src/builtins.c
	${CC} ${CFLAGS} -c src/builtins.c
	@mkdir -p build
	@mv builtins.o build/

psh: utils builtins src/psh.c include/psh.h src/history.c include/history.h
	${CC} ${CFLAGS} src/history.c src/psh.c -o build/psh build/utils.o build/builtins.o

clean:
	@rm -f build/*
