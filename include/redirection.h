#ifndef REDIRECTION_H
#define REDIRECTION_H

#include "command.h"

int apply_redirections(t_redirection *redirections, int *saved_stdin, int *saved_stdout);
void restore_redirections(int saved_stdin, int saved_stdout);

#endif
