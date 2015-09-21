#define _GNU_SOURCE

#include <libgen.h>
#include <limits.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "history.h"
#include "psh.h"
#include "utils.h"

int
main()
{
        char *line;
        char **args;
        int status;

        do {
                /* Give the user a prompt */
                psh_print_prompt();

                /* Read and parse the input */
                line = psh_read_line();
                args = psh_tok_line(line);

                /* Replace environment variables */
                __replace_env(args);

                /* Run the command */
                status = psh_exec(args);

                /* Cleanup memory */
                free(line);
                free(args);
        } while (status >= 0);

        return EXIT_SUCCESS;
}

void
psh_print_prompt(void)
{
        struct passwd *pw;
        char *host;
        char *dir = NULL;
        char *base;
        size_t size = 0;

        /* Get user name from /etc/passwd file */
        pw = getpwuid(getuid());
        if (!pw) {
                printf("%s", DEFAULT_PROMPT);
                return;
        }
        printf("%s@", pw->pw_name);

        /* Get hostname */
        host = malloc(HOST_NAME_MAX + 1);
        if (gethostname(host, HOST_NAME_MAX + 1)) {
                perror("psh");
                printf("%s", DEFAULT_PROMPT);
                return;
        }
        printf("%s ", host);
        free(host);

        /* Get current working directory */
        dir = getcwd(dir, size);
        if (!dir) {
                perror("psh");
                printf("%s", DEFAULT_PROMPT);
                return;
        }
        base = basename(dir);
        printf("%s $ ", base);
        free(dir);
}

char *
psh_read_line(void)
{
        char *line = NULL;
        size_t size = 0;

        /* Read a line from the user */
        getline(&line, &size, stdin);

        /* Parse for history commands (!! and !n) */
        psh_replace_history(line, &size);

        return line;
}

char **
psh_tok_line(char *line)
{
        int buffer = TOK_BUFSIZE;
        int position = 0;
        char *token;
        char **tokens;
        char **tokens_backup;

        tokens = malloc(buffer * sizeof(char *));
        if (!tokens) {
                fprintf(stderr, "psh: Allocation failure :(\n");
                exit(EXIT_FAILURE);
        }

        /* Tokenize the input string
         * Can't handle spaces inside quoted strings :(  */
        token = strtok(line, TOK_DELIM);
        while (token) {
                tokens[position++] = token;

                if (position >= buffer) {
                        /* If the new string is greater than our buffer, increase the size
                         * Make sure to backup the old version, in case realloc fails */
                        buffer += TOK_BUFSIZE;
                        tokens_backup = tokens;
                        tokens = realloc(tokens, buffer * sizeof(char *));
                        if (!tokens) {
                                /* Free the pre-realloc array and exit with failure */
                                free(tokens_backup);
                                fprintf(stderr, "psh: Allocation failure :(\n");
                                exit(EXIT_FAILURE);
                        }
                }
                token = strtok(NULL, TOK_DELIM);
        }
        tokens[position] = NULL;
        return tokens;
}

/*
 * Used to run a command, first checking against the list of builtins and history commands
 */
int
psh_exec(char **args)
{
        int i;

        /* Check for empty command */
        if (args[0] == NULL) return 0;

        /* Append the command to the history */
        psh_append_history(args);

        /* Check the list of builtin functions */
        for (i = 0; i < psh_builtin_count(); i++) {
                if (strcmp(args[0], builtins[i]) == 0) {
                        return (*builtin_funcs[i])(args);
                }
        }

        /* Simple fork/exec */
        return psh_run(args);
}

/*
 * Forks this process and runs what it has been passed
 */
int
psh_run(char **args)
{
        pid_t pid;
        int status;
        int i;
        int concurrent = 0;

        /* Check if the last argument is "&" */
        i = 0;
        while (args[i]) i++;
        if (strcmp("&", args[i - 1]) == 0) {
                concurrent = 1;
                args[i - 1] = NULL;
        }

        /* Fork the process and execute stuff! */
        pid = fork();
        if (pid < 0) {
                /* Can't fork
                 *   ________   .==.
                 *  [________>c((_  )
                 *              '=='
                 * Spoon instead */
        } else if (pid == 0) {
                /* Child process */
                if (execvp(args[0], args) == -1) {
                        perror("psh");
                }
                exit(EXIT_FAILURE);
        } else {
                /* Parent Process */

                if (concurrent) {
                        /* Print the child PID and keep the shell going */
                        printf("[%d]\n", pid);
                        return 0;
                }

                /* Wait for the child to change state (and actually exit) */
                do {
                        waitpid(pid, &status, WUNTRACED);
                } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        }

        return 0;
}
