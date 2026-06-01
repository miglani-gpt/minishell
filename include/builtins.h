#ifndef BUILTINS_H
#define BUILTINS_H

int is_builtin(char *command);
int handle_builtin(char **args);
int builtin_cd(char **args);
int builtin_pwd(void);
int builtin_help(void);
int builtin_clear(void);

#endif