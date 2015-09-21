#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/history.h"
#include "../include/utils.h"

FILE *
psh_open_writable_history()
{
        char *path = __psh_history_path();
        FILE *file;
        if (!path) {
                return NULL;
        }

        file = fopen(path, "a");
        return file;
}

FILE *
psh_open_readable_history()
{
        char *path = __psh_history_path();
        FILE *file;
        if (!path) {
                return NULL;
        }

        file = fopen(path, "r");
        return file;
}

/* Take a command/args array and append it to the history file */
int
psh_append_history(char ** cmd)
{
        int i = 0;
        FILE *history = psh_open_writable_history();

        if (!history) {
                fprintf(stderr, "Couldn't open history file :(\n");
                return 1;
        }

        /* Write the command to the file */
        while (cmd[i]) {
                fprintf(history, "%s", cmd[i]);
                if (cmd[++i]) fprintf(history, " ");
        }
        fprintf(history, "\n");

        if(fclose(history)) {
                perror("psh");
                return 1;
        }

        return 0;
}

/* Account for special !! and !n commands to execute past commands */
void
psh_replace_history(char *line, size_t *size)
{
        FILE *histFile;
        int timeWarpSize = 1;
        int i;

        /* Check the special history commands first nth item, then last */
        if (strcmp(line, BANG_BANG) == 0) {
                histFile = psh_open_readable_history();
                if (!histFile) {
                        fprintf(stderr, "No history found\n");
                        return NULL;
                }
                fseek(histFile, -2, SEEK_END);
                __psh_seek_back(histFile);
                getline(&line, size, histFile);
                printf("%s", line);
                if (fclose(histFile)) perror("psh");
        } else if (line[0] == '!') {
                timeWarpSize = atoi(line + 1);
                histFile = psh_open_readable_history();
                if (!histFile) {
                        fprintf(stderr, "No history found\n");
                        return NULL;
                }
                for (i = 0; i < timeWarpSize; i++) {
                        if (getline(&line, size, histFile) == -1) {
                                fprintf(stderr, "No item in history\n");
                                free(line);
                                return NULL;
                        }
                }
                printf("%s", line);
                if (fclose(histFile)) perror("psh");
        }
}

/* Get the path for the history file
 * Should be ~/.psh_history */
char *
__psh_history_path()
{
        char *home;
        char *path;

        home = __get_home_dir();
        path = malloc(strlen(home) + FILENAME_LENGTH);
        if (!path) return NULL;
        strcpy(path, home);
        path = strcat(path, HISTORY_FILENAME);
        return path;
}

/* Advance one line in the stream */
void
__psh_seek_one_line(FILE* file)
{
        while (fgetc(file) != '\n');
}

/* Seek one line backwards in file from the current pointer*/
void
__psh_seek_back(FILE* file)
{
        while (fgetc(file) != '\n') {
                fseek(file, -2, SEEK_CUR);
        }
}
