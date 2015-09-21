#ifndef _PSH_BUILTINS_H
#define _PSH_BUILTINS_H

#define BUILTIN_COUNT 4

int
psh_builtin_count(void);

int
psh_cd(char **args);

int
psh_history(char **args);

int
psh_export(char **args);

int
psh_exit(char **args);


#endif
