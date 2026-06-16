#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "parser.h"
#include "command.h"

void execute_parsed_input(t_parsed_input *parsed);
void execute_command(t_command *command, int background);
void execute_external_command(t_command *command, int background);

#endif
