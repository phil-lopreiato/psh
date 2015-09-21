#ifndef _PSH_H
#define _PSH_H

#include "builtins.h"

/* Size to string to tokenize at a time */
#define TOK_BUFSIZE 16

/* Characters to stop tokenizing on */
#define TOK_DELIM " \t\r\n\a"

/* Fallback prompt to show */
#define DEFAULT_PROMPT "psh> "

static char *builtins[] = {
        "cd",
        "history",
        "export",
        "exit"
};

static int(*builtin_funcs[]) (char **) = {
        &psh_cd,
        &psh_history,
        &psh_export,
        &psh_exit
};

void
psh_print_prompt(void);

char *
psh_read_line(void);

char **
psh_tok_line(char *line);

int
psh_exec(char **args);

int
psh_run(char **args);

#endif
