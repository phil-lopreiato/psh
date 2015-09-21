#ifndef PSH_UTILS_H
#define PSH_UTILS_H

/* Environment variable for home directory */
#define HOME_DIR_ENV "HOME"

char *
__get_home_dir(void);

void
__replace_env(char **args);

#endif
