#ifndef REDIRECTION_H
#define REDIRECTION_H

int setup_output_redirection(char **args, int *saved_stdout);
void restore_output_redirection(int saved_stdout);

int setup_input_redirection(char **args, int *saved_stdin);
void restore_input_redirection(int saved_stdin);

#endif