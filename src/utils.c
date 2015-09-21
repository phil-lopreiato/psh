#include <pwd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "../include/utils.h"

/* Get the current user's home directory
 * First check HOME environment variable
 * and try /etc/passwd if not
 */
char *
__get_home_dir(void)
{
        char *dir;
        struct passwd *pw;

        /* Switch to the user's home directory */
        dir = getenv(HOME_DIR_ENV);
        if (!dir) {
                /* Environment variable not set, read from /etc/passwd */
                pw = getpwuid(getuid());
                if (!pw) {
                        return NULL;
                }
                dir = pw->pw_dir;
                free(pw);
        }
        return dir;
}

/* Take the passed args array and replace anything
 * starting with $ with the corresponding environment variable.
 *
 * If the variable isn't set, leave it as is
 */
void
__replace_env(char **args)
{
        int i = 0;
        char *var;

        while(args[i]) {
                if (args[i][0] == '$') {
                        var = getenv(args[i] + 1);
                        args[i] = var ? var : args[i];
                }
                i++;
        }
}
