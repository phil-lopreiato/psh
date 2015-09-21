#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../include/builtins.h"
#include "../include/history.h"
#include "../include/utils.h"

int
psh_builtin_count() {
        return BUILTIN_COUNT;
}

/* cd implementation */
int
psh_cd(char **args)
{
        char *dir;

        dir = args[1];

        /* Run with no args. Change to home directory */
        if (!dir) {
                dir = __get_home_dir();
        }

        if (dir == NULL) {
                perror("cd");
                return 1;
        }

        /* Change the directory */
        if (chdir(dir) != 0) {
                perror("cd");
                return 1;
        }

        return 0;
}

/* Prints command history
 * Implemented to work similar to bash/zsh where history is printed with
 * the most recent command at the bottom
 */
int
psh_history(char **args)
{
        FILE *hist;
        char *line = NULL;
        size_t line_size = 0;
        size_t read;
        int count = 1;

        hist = psh_open_readable_history();
        if (!hist) {
                fprintf(stderr, "Unable to open history file\n");
                return 1;
        }

        while ((read = getline(&line, &line_size, hist)) != -1) {
                printf("[%d] %s", count, line);
                count++;
        }

        if (fclose(hist)) {
                perror("history");
                return 1;
        }

        return 0;
}

/* Sets an environment variable.
 * Use like export <var> <val> to set <var = <val>
 * Or also <var>=<val>
 */
int
psh_export(char **args)
{
        int i;

        if (!args[1]) {
                fprintf(stderr, "Must supply a variable to export\n");
                return 1;
        } else if (!args[2]) {
                /* Format <var>=<val>, split to use setenv */
                i = 0;
                while (args[1][i] != '=') i++;
                args[1][i] = '\0';
                if (setenv(args[1], args[1] + i + 1, 1)) {
                        perror("export");
                }
        } else if (setenv(args[1], args[2], 1)) {
                perror("export");
                return 1;
        }
        return 0;
}

/* Leave. */
int
psh_exit(char **args) {
        return -1;
}
