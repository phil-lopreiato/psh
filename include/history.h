#ifndef PSH_HISTORY_H
#define PSH_HISTORY_H

#include <stdio.h>

#define HISTORY_FILENAME "/.psh_history"
#define FILENAME_LENGTH 14

/* History special commands */
#define BANG_BANG "!!\n"
#define BANG '!'

FILE *
psh_open_writable_history();

FILE *
psh_open_readable_history();

int
psh_append_history(char ** cmd);

void
psh_replace_history(char *line, size_t *size);

char *
__psh_history_path();

void
__psh_seek_back(FILE* file);

void
__psh_seek_one_line(FILE* file);

#endif
